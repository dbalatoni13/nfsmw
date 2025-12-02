#include "../SimSubSystem.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/system.h"
#include "Speed/Indep/Src/Generated/Events/ESimulate.hpp"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/SimProfile.h"

namespace Sim {

void ProfileTask(HSIMTASK htask, const char *name);

void SubSystem::ValidateHeap(bool before, bool initializing) {}

namespace Internal {

static unsigned int mTick;
static unsigned int mRenderFrame;
static float mFrameTime;
static float mTime;

void InitTimers() {
    mFrameTime = 0.0f;
    mRenderFrame = eGetFrameCounter();
    mTime = 0;
    mTick = 0;
}

}; // namespace Internal

}; // namespace Sim

class SimTask : public UTL::Collections::Countable<SimTask> {
  public:
    enum eFlags {
        ModeFlags = 15,
        DirtyFlag = 16,
    };

    SimTask(unsigned int priority, float rate, Sim::ITaskable *handler, float start_offset, Sim::TaskMode mode);
    ~SimTask();
    void Run(float dT_sim, float dT_render);
    void UpdateAll(float dT_sim, float dT_render);

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    bool IsDirty() const {
        return mFlags & (1 << 4);
    }

    Sim::TaskMode GetMode() const {
        return static_cast<Sim::TaskMode>(mFlags & 0xF);
    }

    HSIMTASK GetInstanceHandle() const {
        return mHandle;
    }

  private:
    void Link();
    void UnLink();

    static unsigned int mNextHandle;
    static SimTask *mRoot;

    float mRate;              // offset 0x4, size 0x4
    Sim::ITaskable *mHandler; // offset 0x8, size 0x4
    float mUpdate;            // offset 0xC, size 0x4
    unsigned int mPriority;   // offset 0x10, size 0x4
    SimTask *mHead;           // offset 0x14, size 0x4
    SimTask *mTail;           // offset 0x18, size 0x4
    unsigned int mFlags;      // offset 0x1C, size 0x4
    float mTimeBank;          // offset 0x20, size 0x4
    HSIMTASK mHandle;         // offset 0x24, size 0x4
    HSIMPROFILE mProfile;     // offset 0x28, size 0x4
};

// UNSOLVED
SimTask::SimTask(unsigned int priority, float rate, Sim::ITaskable *handler, float start_offset, Sim::TaskMode mode)
    : mRate(UMath::Min(rate, 1.0f)), mHandle((HSIMTASK)SimTask::mNextHandle), mHandler(handler), mUpdate(-start_offset), mPriority(priority),
      mFlags(mode & ModeFlags), mTimeBank(0.0f), mHead(nullptr), mTail(nullptr), mProfile(nullptr) {
    SimTask::mNextHandle++;
    Link();
}

// UNSOLVED, probably wrong functionally, the branching is annoying
void SimTask::Link() {
    if (!mRoot) {
        mRoot = this;
        return;
    }
    SimTask *p = mRoot;
    while (mPriority > p->mPriority) {
        p = p->mTail;
        mTail = p;
        mHead = p->mHead;
        if (p == mRoot) {
            mRoot = this;
        }
        if (mHead) {
            mHead->mTail = this;
        }
        if (mTail) {
            mTail->mHead = this;
            return;
        }
        if (!p->mTail) {
            mHead = p;
            p->mTail = this;
            return;
        }
    }
}

void SimTask::UnLink() {
    if (mRoot == this) {
        mRoot = mTail;
    }
    if (mHead) {
        mHead->mTail = mTail;
    }
    if (mTail) {
        mTail->mHead = mHead;
    }
}

SimTask::~SimTask() {
    Sim::Profile::Release(mProfile);
    UnLink();
}

// UNSOLVED, should be functionally matching
void SimTask::Run(float dT_sim, float dT_render) {
    if (IsDirty() || mRate <= 0.0f) {
        return;
    }
    ProfileNode profile_node("TODO", 1);
    Sim::TaskMode mode = GetMode();

    if (mode == Sim::TASK_FRAME_FIXED) {
        mUpdate += mRate;
        if (mUpdate < 1.0f) {
            return;
        }
        Sim::Profile::Scope profile(mProfile);
        bool handled = mHandler->OnTask(GetInstanceHandle(), dT_sim / mRate);
        mUpdate -= 1.0f;
    } else {
        if (mode != Sim::TASK_FRAME_VARIABLE || dT_render <= 0.0f) {
            return;
        }
        mUpdate += mRate;
        mTimeBank += dT_render;
        if (mUpdate < 1.0f) {
            return;
        }
        Sim::Profile::Scope profile(mProfile);
        bool handled = mHandler->OnTask(GetInstanceHandle(), mUpdate);
        mTimeBank = 0.0f;
        mUpdate -= 1.0f;
    }
}

void SimTask::UpdateAll(float dT_sim, float dT_render) {
    SimTask *p = mRoot;
    unsigned int currentpriority;
    while (p) {
        p->Run(dT_sim, dT_render);
        p = p->mTail;
    }
}

// total size: 0x90
class SimSystem : public UTL::COM::Object, public Sim::ITaskable {
  public:
    SimSystem();
    float DistanceToCamera(const UMath::Vector3 &v) const;
    void RunAllTasks(float frame_time);
    void UpdateFrame();
    void PauseInput(bool bPaused);
    void SetState(Sim::State newstate);
    void ModifyTask(HSIMTASK hTask, float rate);
    HSIMTASK AddTask(const UCrc32 &schedule, float rate, ITaskable *handler, float start_offset, Sim::TaskMode mode);
    void RemoveTask(HSIMTASK hTask, ITaskable *handler);
    void Start(const UCrc32 objclass);

    // Overrides
    // IUnknown
    override virtual ~SimSystem();

    // ITaskable
    override virtual bool OnTask(HSIMTASK htask, float dT);

  private:
    void DoFetchInput(IInputPlayer *o) {
        // TODO
    }

    void CollectGarbage();
    void FetchCameras();

    const float mScheduleStep;        // offset 0x18, size 0x4
    float mTimeStep;                  // offset 0x1C, size 0x4
    float mTargetSpeed;               // offset 0x20, size 0x4
    float mSpeed;                     // offset 0x24, size 0x4
    int mEvent;                       // offset 0x28, size 0x4
    Sim::State mState;                // offset 0x2C, size 0x4
    ESimulate::StaticData mEventData; // offset 0x30, size 0x8
    Sim::IActivity *mKernel;          // offset 0x38, size 0x4
    bool mInputPaused;                // offset 0x3C, size 0x1
    HSIMTASK mWorldUpdate;            // offset 0x40, size 0x4
    HSIMTASK mSimStart;               // offset 0x44, size 0x4
    HSIMTASK mSimEnd;                 // offset 0x48, size 0x4
    HSIMTASK mSimFrameEnd;            // offset 0x4C, size 0x4
    Attrib::Gen::system mAttribs;     // offset 0x50, size 0x14
    UMath::Vector4 mCameras[2];       // offset 0x64, size 0x20
    unsigned int mCameraTargets[2];   // offset 0x84, size 0x8
    HSIMPROFILE mTasksProfile;        // offset 0x8C, size 0x4
};

SimSystem::SimSystem()
    : UTL::COM::Object(3),                           //
      Sim::ITaskable(this),                          //
      mScheduleStep(Scheduler::Get().GetTimeStep()), //
      mState(Sim::STATE_NONE),                       //
      mKernel(nullptr),                              //
      mTargetSpeed(1.0f),                            //
      mSpeed(1.0f),                                  //
      mEvent(-1),                                    //
      mInputPaused(false),                           //
      mAttribs(0xeec2271a, 0, nullptr),              // "default"
      mTasksProfile(Sim::Profile::Create()) {
    mCameras[0] = mCameras[1] = UMath::Vector4::kZero;
    mCameraTargets[0] = mCameraTargets[1] = 0;
    mTimeStep = mScheduleStep;
    mEventData.fSim = this;
    mEventData.fEventID = 0x522d67db; // TODO magic
    mEvent = Scheduler::Get().fSchedule_SimRate->AddTask(0x522d67db, &mEventData, 0, true, 0, 0);

    unsigned int c = mAttribs.Num_SimSubSystems();
    for (unsigned int i = 0; i < c; i++) {
        Sim::SubSystem::Init(UCrc32(mAttribs.SimSubSystems(i)));
    }

    mSimStart = SimSystem::AddTask(UCrc32("SimStart"), 1.0f, this, 0.0f, Sim::TASK_FRAME_FIXED);
    mWorldUpdate = AddTask(UCrc32("WorldUpdate"), 1.0f, this, 0.0f, Sim::TASK_FRAME_FIXED);
    mSimFrameEnd = AddTask(UCrc32("SimEnd"), 1.0f, this, 0.0f, Sim::TASK_FRAME_VARIABLE); // bug, this should be "SimFrameEnd"
    mSimEnd = AddTask(UCrc32("SimEnd"), 1.0f, this, 0.0f, Sim::TASK_FRAME_FIXED);

    Sim::ProfileTask(mSimStart, "SimStart");
    Sim::ProfileTask(mWorldUpdate, "WorldUpdate");
    Sim::ProfileTask(mSimFrameEnd, "SimFrameEnd");
    Sim::ProfileTask(mSimEnd, "SimEnd");
}
