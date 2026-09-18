#include "EScheduleEventUpdate.hpp"

#include "EFireEventList.hpp"

#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

EScheduleEventUpdate::EScheduleEventUpdate(void *pStaticData, CARP::EventList *pEventList, int pTaskHandle, float pExecuteOnSimTime, int pResetCount)
    : Event(0x20), fStaticData(pStaticData), fEventList(pEventList), fTaskHandle(pTaskHandle), fExecuteOnSimTime(pExecuteOnSimTime), fResetCount(pResetCount) {
}

EScheduleEventUpdate::~EScheduleEventUpdate() {
    if (fResetCount) {
        Scheduler::Get().fSchedule_SimRate->RemoveTask(fTaskHandle);

        delete (StaticData *) fStaticData;
    } else if (Sim::GetState() != Sim::STATE_IDLE && Sim::GetTime() >= fExecuteOnSimTime) {
        new EFireEventList(fEventList, 0, 0);

        Scheduler::Get().fSchedule_SimRate->RemoveTask(fTaskHandle);

        delete (StaticData *) fStaticData;
    }
}

const char *EScheduleEventUpdate::GetEventName() const {
    return "EScheduleEventUpdate";
}

void EScheduleEventUpdate_MakeEvent_Callback(const void *staticData) {
    new EScheduleEventUpdate(((EScheduleEventUpdate::StaticData *) staticData)->fStaticData, ((EScheduleEventUpdate::StaticData *) staticData)->fEventList, ((EScheduleEventUpdate::StaticData *) staticData)->fTaskHandle, ((EScheduleEventUpdate::StaticData *) staticData)->fExecuteOnSimTime, ((EScheduleEventUpdate::StaticData *) staticData)->fResetCount);
}
