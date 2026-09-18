#include "Speed/Indep/Src/Main/Scheduler.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

bool LOCK_TO_30 = false;

Scheduler *Scheduler::fgScheduler = 0;

Scheduler::Scheduler(float timestep)
    : fSchedule_OncePerGameLoop(NULL), fSchedule_SimRate(NULL), fSchedule_HalfSimRate(NULL), //
      fSchedule_QuarterSimRate(NULL),                                                       //
      mTime(0.0f), mTickBank(0.0f), fTimeStep(timestep), fSingleStepMode(false), fFullSpeedMode(false), mEventTick(0) {
    fScheduleList = new ScheduleList;

    fSchedule_OncePerGameLoop = new Schedule_OncePerGameLoop;

    fSchedule_SimRate = new Schedule_SimRate;
    AddSchedule(fSchedule_SimRate);

    fSchedule_HalfSimRate = new Schedule_HalfSimRate;
    AddSchedule(fSchedule_HalfSimRate);

    fSchedule_QuarterSimRate = new Schedule_QuarterSimRate;
    AddSchedule(fSchedule_QuarterSimRate);
}

void Scheduler::Init(float timestep) {
    if (fgScheduler == NULL)
        fgScheduler = new ("Scheduler", 0) Scheduler(timestep);
}

void Scheduler::AddSchedule(Schedule *schedule) {
    this->fScheduleList->push_back(schedule);
}

void Scheduler::RunNormalSpeed(unsigned int delta_tick) {
    const unsigned int MaxCatchup = 6;
    const unsigned int MinCatchup = 2;

    if (fSingleStepMode && delta_tick)
        delta_tick = 1;

    unsigned int max_elapsed = LOCK_TO_30 ? MinCatchup : MaxCatchup;

    if (delta_tick > max_elapsed)
        delta_tick = max_elapsed;

    for (unsigned int i = 0; i < delta_tick; i++) {
        for (int priority = 0; priority < kNumPriorityLevels; priority++) {
            for (ScheduleList::iterator iter = fScheduleList->begin(); iter != fScheduleList->end(); ++iter)
                (*iter)->Process(mEventTick, priority);
        }

        EventManager::RunEvents();
        mEventTick++;
    }

    for (int priority = 0; priority < kNumPriorityLevels; priority++)
        fSchedule_OncePerGameLoop->Process(0, priority);

    EventManager::RunEvents();
}

Timer Scheduler::GetTargetTimer() {
    return RealTimer;
}

float Scheduler::Run(bool full_speed) {
    Timer target_timer = GetTargetTimer();
    float time_delta_seconds = (target_timer - mTimer).GetSeconds();

    if (time_delta_seconds < 0.0f) {
        mTimer = target_timer;
        return 0.0f;
    }

    mTimer = target_timer;
    time_delta_seconds = UMath::Max(time_delta_seconds, 0.0f);

    float tick = time_delta_seconds / fTimeStep;
    float delta_tick = mTickBank + tick;

    mTime += time_delta_seconds;

    mTickBank = delta_tick - (unsigned int) delta_tick;
    unsigned int ticks_to_run = (unsigned int) delta_tick;

    if (full_speed) {
        ticks_to_run = 4;
        mTickBank = 0.0f;
    }

    RunNormalSpeed(ticks_to_run);

    return ticks_to_run * fTimeStep;
}

void Scheduler::Synchronize(Timer to) {
    mTimer = to;
    mTime = 0.0f;
    mEventTick = 0;

    mTickBank = 0.0f;
}

Schedule::Schedule(int numBuckets) : fNumBuckets(numBuckets), fAllocationHandle(1000) {
    for (int i = 0; i < 8; i++)
        fTaskList[i] = NULL;

    for (int i = 0; i < fNumBuckets; i++)
        fTaskList[i] = new TaskList;
}

Schedule::~Schedule() {
    for (int i = 0; i < fNumBuckets; i++)
        delete fTaskList[i];
}

int Schedule::AddTask(int tag, Event::StaticData *staticData, unsigned short priority, bool deterministic, int tickDelay, int repeat) {
    int bucket = EmptiestBucket();

    fTaskList[bucket]->push_back(TaskRecord(tag, staticData, tickDelay, repeat, priority, fAllocationHandle));

    return fAllocationHandle++;
}

bool Schedule::RemoveTask(int handle) {
    for (int bankIndex = 0; bankIndex < 8; bankIndex++) {
        for (TaskList::iterator iter = fTaskList[bankIndex]->begin(); iter != fTaskList[bankIndex]->end(); ++iter) {
            if ((*iter).fAllocationHandle == handle) {
                fTaskList[bankIndex]->erase(iter);

                return true;
            }
        }
    }

    return false;
}

int Schedule::EmptiestBucket() {
    unsigned int least = 999999;
    int leastInd = -1;

    for (int i = 0; i < fNumBuckets; i++) {
        if (fTaskList[i]->size() < least) {
            least = fTaskList[i]->size();
            leastInd = i;
        }
    }

    return leastInd;
}

void Schedule::RunTasks(int bankIndex, unsigned short priority) {
    TaskList::iterator iter = fTaskList[bankIndex]->begin();

    while (iter != fTaskList[bankIndex]->end()) {
        TaskRecord &tl = *iter;

        if (tl.fPriority == priority) {
            if (tl.fTickDelay == 0) {
                RegisterEvent::LookupEvent(tl.fTag)(tl.fStaticData);

                if (tl.fRepeat != 0) {
                    tl.fRepeat--;

                    if (tl.fRepeat == 0) {
                        TaskList::iterator saveIter = iter;
                        ++saveIter;
                        RemoveTask(tl.fAllocationHandle);
                        iter = saveIter;
                        continue;
                    }
                }
            } else {
                tl.fTickDelay--;
            }
        }

        ++iter;
    }
}

void Schedule_OncePerGameLoop::Process(int time, unsigned short priority) {
    RunTasks(0, priority);
}

void Schedule_SimRate::Process(int time, unsigned short priority) {
    RunTasks(0, priority);
}

void Schedule_HalfSimRate::Process(int time, unsigned short priority) {
    int bank = time & 1;
    RunTasks(bank, priority);
}

void Schedule_QuarterSimRate::Process(int time, unsigned short priority) {
    int bank = time & 3;
    RunTasks(bank, priority);
}
