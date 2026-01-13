#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/controller.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/system.h"
#include "Speed/Indep/Src/Generated/Events/ESimulate.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDisposable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Math/SimRandom.h"
#include "Speed/Indep/Src/Misc/Platform.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"
#include "Speed/Indep/Src/Physics/Behaviors/SimpleRigidBody.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/SimEntity.h"
#include "Speed/Indep/Src/Sim/SimModel.h"
#include "Speed/Indep/Src/Sim/SimProfile.h"
#include "Speed/Indep/Src/Sim/SimSubSystem.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"
#include "Speed/Indep/Src/World/WTrigger.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#include <types.h>

#include <algorithm>
#include <vector>

DECLARE_VECTOR_TYPE(CollisionListener);
DECLARE_VECTOR_TYPE(CollisionParticipant);

class SimSystem;

namespace Sim {

void ProfileTask(HSIMTASK htask, const char *name);

void SubSystem::ValidateHeap(bool before, bool initializing) {}

namespace Internal {

static float mProfileTime;
static int mCallCount;
static unsigned int mTick;
static unsigned int mRenderFrame = 0;
static float mFrameTime = 0.0f;
static float mTime;
static void *mWorkspace = nullptr;
static void *mStackFrame = nullptr;
static SimSystem *mSystem;
static eUserMode mUserMode = USER_SINGLE;

void InitTimers() {
    mFrameTime = 0.0f;
    mRenderFrame = eGetFrameCounter();
    mTime = 0;
    mTick = 0;
}

// TODO right place? or more down in the file?
// total size: 0x24
struct CDispatcher : public UTL::Collections::Singleton<CDispatcher> {
    // total size: 0x10
    struct Node {
        // total size: 0x8
        struct Timing {
            Timing(const char *who) : ID(who), HWM(0.0f) {}

            const char *ID; // offset 0x0, size 0x4
            float HWM;      // offset 0x4, size 0x4
        };

        Node(HSIMABLE participant, Collision::IListener *listener, const char *who)
            : Participant(participant), //
              Listener(listener),       //
              mTiming(who) {}

        bool operator<(const Node &rhs) const {
            return Participant < rhs.Participant;
        }

        void Respond(const COLLISION_INFO &cinfo) {
            Listener->OnCollision(cinfo);
        }

        HSIMABLE Participant;           // offset 0x0, size 0x4
        Collision::IListener *Listener; // offset 0x4, size 0x4
        Timing mTiming;                 // offset 0x8, size 0x8
    };
    // total size: 0x8
    struct Finder {
        Finder(HSIMABLE participant, Collision::IListener *listener)
            : Participant(participant), //
              Listener(listener) {}

        bool operator()(const Node &h) const {
            return (!Listener || Listener == h.Listener) && (!Participant || Participant == h.Participant);
        }

        HSIMABLE Participant;           // offset 0x0, size 0x4
        Collision::IListener *Listener; // offset 0x4, size 0x4
    };

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    CDispatcher() {
        mParticpants.reserve(160); // TODO magic
        mList.reserve(256);        // TODO magic
    }

    ~CDispatcher() {}

    void Profile() {}

    void Respond(HSIMABLE participant, const COLLISION_INFO &cinfo) {
        Node pair(participant, nullptr, nullptr);
        List::iterator iter = std::lower_bound(mList.begin(), mList.end(), pair);
        for (; iter != mList.end(); ++iter) {
            Node &node = *iter;
            if (node.Participant != participant) {
                break;
            }
            node.Respond(cinfo);
        }
    }

    bool FindListener(Collision::IListener *listener, HSIMABLE participant) const {
        List::const_iterator iter = std::find_if(mList.begin(), mList.end(), Finder(participant, listener));
        return iter != mList.end();
    }

    void AddListener(Collision::IListener *listener, HSIMABLE participant, const char *who) {
        Participants::iterator piter = std::lower_bound(mParticpants.begin(), mParticpants.end(), participant);
        if (!FindListener(listener, participant)) {
            Node pair(participant, listener, who);
            mList.insert(std::upper_bound(mList.begin(), mList.end(), pair), pair);
        }
    }

    void RemoveListener(Collision::IListener *listener, HSIMABLE participant) {
        List::iterator newend = std::remove_if(mList.begin(), mList.end(), Finder(participant, listener));
        if (newend != mList.end()) {
            mList.erase(newend, mList.end());
        }
    }

    void RemoveParticipant(HSIMABLE participant) {
        Participants::iterator piter = std::lower_bound(mParticpants.begin(), mParticpants.end(), participant);
        mParticpants.erase(piter);

        List::iterator newend = std::remove_if(mList.begin(), mList.end(), Finder(participant, nullptr));
        if (newend != mList.end()) {
            mList.erase(newend, mList.end());
        }
    }

    void AddParticipant(HSIMABLE participant) {
        mParticpants.insert(std::upper_bound(mParticpants.begin(), mParticpants.end(), participant), participant);
    }

    typedef UTL::Std::vector<Node, _type_CollisionListener> List;
    typedef UTL::Std::vector<HSIMABLE, _type_CollisionParticipant> Participants;

    Participants mParticpants; // offset 0x4, size 0x10
    List mList;                // offset 0x14, size 0x10
};

}; // namespace Internal

}; // namespace Sim

class SimTask : public UTL::Collections::Countable<SimTask> {
  public:
    enum eFlags {
        ModeFlags = 15,
        DirtyFlag = 16,
    };

    static void UpdateAll(float dT_sim, float dT_render);

    SimTask(unsigned int priority, float rate, Sim::ITaskable *handler, float start_offset, Sim::TaskMode mode);
    ~SimTask();
    void Run(float dT_sim, float dT_render);

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    static SimTask *FindInstance(HSIMTASK htask) {
        SimTask *p = mRoot;
        while (p) {
            if (p->mHandle == htask) {
                return p;
            }
            p = p->mTail;
        }
        return nullptr;
    }

    void SetRate(float r) {
        mRate = r;
    }

    static void Shutdown() {
        SimTask *p = mRoot;
        while (p) {
            SimTask *next = p->mTail;
            delete p;
            p = next;
        }
    }

    static void Collect() {
        SimTask *p = mRoot;
        while (p) {
            SimTask *next = p->mTail;
            if (p->IsDirty()) {
                delete p;
            }
            p = next;
        }
    }

    HSIMTASK GetInstanceHandle() const {
        return mHandle;
    }

    bool IsDirty() const {
        return mFlags & DirtyFlag;
    }

    void Release() {
        mFlags |= DirtyFlag;
    }

    Sim::TaskMode GetMode() const {
        return static_cast<Sim::TaskMode>(mFlags & 0xF);
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

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    float GetTimeStep() const {
        return mTimeStep;
    }

    float GetSpeed() const {
        return mSpeed;
    }

    Sim::State GetState() {
        return mState;
    }

    // Overrides
    // IUnknown
    ~SimSystem() override;

    // ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

  private:
    static void DoFetchInput(IInputPlayer *o) {
        return o->FetchInput();
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
      ITaskable(this),                               //
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

SimSystem::~SimSystem() {
    if (mKernel) {
        mKernel->Release();
        mKernel = nullptr;
    }

    RemoveTask(mSimStart, this);
    RemoveTask(mWorldUpdate, this);
    RemoveTask(mSimEnd, this);
    RemoveTask(mSimFrameEnd, this);
    CollectGarbage();

    Sim::Profile::Release(mTasksProfile);

    Sim::Activity::GetGC().Shutdown();
    Sim::Entity::GetGC().Shutdown();
    PhysicsObject::GetGC().Shutdown();
    Sim::Model::GetGC().Shutdown();

    SimTask::Shutdown();

    for (int i = mAttribs.Num_SimSubSystems() - 1; i >= 0; i--) {
        Sim::SubSystem::Shutdown(UCrc32(mAttribs.SimSubSystems(i)));
    }
    Scheduler::Get().fSchedule_SimRate->RemoveTask(mEvent);
}

float SimSystem::DistanceToCamera(const UMath::Vector3 &v) const {
    float dist1 = UMath::Distance(v, UMath::Vector4To3(mCameras[0])) * mCameras[0].w;
    float dist2 = UMath::Distance(v, UMath::Vector4To3(mCameras[1]));

    return UMath::Min(dist1, dist2);
}

void SimSystem::FetchCameras() {
    mCameras[0] = UMath::Vector4::kZero;
    mCameraTargets[0] = 0;
    mCameraTargets[1] = 0;

    eView *view = eGetView(1, false);
    if (view && view->GetCameraMover()) {
        bVector3 *pos = view->GetCameraMover()->GetPosition();
        eUnSwizzleWorldVector(*pos, *reinterpret_cast<bVector3 *>(&mCameras[0]));
        mCameras[0].w = 1.0f;

        CameraAnchor *anchor = view->GetCameraMover()->GetAnchor();
        if (anchor) {
            mCameraTargets[0] = anchor->GetWorldID();
        }
    }
    view = eGetView(2, false);
    if (view && view->GetCameraMover()) {
        bVector3 *pos = view->GetCameraMover()->GetPosition();
        eUnSwizzleWorldVector(*pos, *reinterpret_cast<bVector3 *>(&mCameras[1]));
        mCameras[1].w = 1.0f;

        CameraAnchor *anchor = view->GetCameraMover()->GetAnchor();
        if (anchor) {
            mCameraTargets[1] = anchor->GetWorldID();
        }
    } else {
        mCameras[1] = mCameras[0];
    }
}

bool SimSystem::OnTask(HSIMTASK htask, float dT) {
    ProfileNode profile_node;

    if (htask == mSimStart) {
        FetchCameras();
        SimpleRigidBody::Update(dT, Sim::Internal::mWorkspace);
        RigidBody::PushSP(Sim::Internal::mWorkspace);
        RigidBody::Update(dT);
        AITarget::TrackAll();
    } else if (htask == mWorldUpdate) {
        WTriggerManager::Get().Update(dT);
        WGridManagedDynamicElem::UpdateElems();
        EventSequencer::UpdateDelta(dT);
        IInputPlayer::ForEach(DoFetchInput);
        SimSurface::UpdateSystem();
    } else if (htask == mSimFrameEnd) {
        for (IModel::List::const_iterator iter = IModel::GetList().begin(); iter != IModel::GetList().end(); ++iter) {
            IModel *model = *iter;
            model->OnProcessFrame(dT);
        }
    } else if (htask == mSimEnd) {
        RigidBody::PopSP();
    } else {
        return false;
    }
    return true;
}

void SimSystem::CollectGarbage() {
    ProfileNode profile_node("TODO", 0);

    PhysicsObject::GetGC().Collect();
    Sim::Activity::GetGC().Collect();
    Sim::Model::GetGC().Collect();
    Sim::Entity::GetGC().Collect();

    SimTask::Collect();
}

void SimSystem::RunAllTasks(float frame_time) {
    ProfileNode profile_node;
    Sim::Profile::Scope scope(mTasksProfile);

    SimTask::UpdateAll(mTimeStep, frame_time);
}

void SimSystem::UpdateFrame() {
    ProfileNode profile_node;

    CollectGarbage();
    if (mKernel) {
        Sim::ITimeManager *timeMgr;
        if (mKernel->QueryInterface(&timeMgr)) {
            mSpeed = timeMgr->OnManageTime(mScheduleStep, mSpeed);
        }

        Sim::IStateManager *statemgr;
        if (mKernel->QueryInterface(&statemgr)) {
            SetState(statemgr->OnManageState(mState));
            PauseInput(statemgr->ShouldPauseInput());
        }
    } else {
        mState = Sim::STATE_NONE;
        mSpeed = 1.0f;
    }
    const unsigned int current_render_frame = eGetFrameCounter();
    mTimeStep = mScheduleStep * mSpeed;

    if ((mState != Sim::STATE_ACTIVE) || (mTimeStep <= FLOAT_EPSILON)) {
        Sim::Internal::mFrameTime = 0.0f;
        Sim::Internal::mRenderFrame = current_render_frame;
        return;
    }
    Sim::Profile::Begin();

    int start_ticks = bGetTicker();
    Sim::Internal::mTime += mTimeStep;
    Sim::Internal::mTick++;
    float render_step = 0.0f;
    if (Sim::Internal::mRenderFrame != current_render_frame) {
        render_step = Sim::Internal::mFrameTime;
        Sim::Internal::mFrameTime = 0.0f;
        Sim::Internal::mRenderFrame = current_render_frame;
    }
    Sim::Internal::mFrameTime += mTimeStep;
    RunAllTasks(render_step);
    Sim::Internal::mProfileTime += bGetTickerDifference(start_ticks);
    Sim::Internal::mCallCount++;

    Sim::Profile::End();
}

static void PauseFFB(IPlayer *player) {
    IFeedback *ffb = player->GetFFB();
    if (ffb) {
        ffb->PauseEffects();
        ffb->ResetEffects();
    }
}

#ifndef EA_BUILD_A124
static void ClearInput(IPlayer *player) {
    if (player && player->IsLocal()) {
        ISimable *isimable = player->GetSimable();

        IInput *iinput;
        if (isimable->QueryInterface(&iinput)) {
            iinput->ClearInput();
        }
    }
}
#endif

void SimSystem::PauseInput(bool bPaused) {
    if (mInputPaused == bPaused) {
        return;
    }
    mInputPaused = bPaused;
    if (bPaused) {
        IPlayer::ForEach(PLAYER_LOCAL, PauseFFB);
    }
    for (ActionQueue::List::const_iterator iter = ActionQueue::GetList().begin(); iter != ActionQueue::GetList().end(); ++iter) {
        ActionQueue *aq = *iter;
        if (bPaused != aq->IsEnabled()) {
            continue;
        }
        Attrib::Gen::controller attribs(aq->GetConfig(), 0, nullptr);
        if (attribs.Pauseable()) {
            aq->Enable(!bPaused);
        }
    }
#ifndef EA_BUILD_A124
    IPlayer::ForEach(PLAYER_LOCAL, ClearInput);
#endif
}

void SimSystem::SetState(Sim::State newstate) {
    if (newstate != mState) {
        mState = newstate;
    }
}

void SimSystem::ModifyTask(HSIMTASK hTask, float rate) {
    SimTask *task = SimTask::FindInstance(hTask);
    task->SetRate(rate);
}

// UNSOLVED, but should be functionally matching
HSIMTASK SimSystem::AddTask(const UCrc32 &schedule, float rate, ITaskable *handler, float start_offset, Sim::TaskMode mode) {
    rate = UMath::Min(rate, 1.0f);
    start_offset = UMath::Max(start_offset, 0.0f);

    unsigned int instance_count = SimTask::Count();
    if (instance_count >= 1000) {
        return nullptr;
    }
    unsigned int c = mAttribs.Num_SimTasks();
    for (unsigned int priortity = 0; priortity < c; priortity++) {
        Attrib::StringKey name = mAttribs.SimTasks(priortity);
        if (schedule.GetValue() == name.GetHash32() && name.IsValid()) {
            SimTask *task = new SimTask(priortity, rate, handler, start_offset, mode);
            return task->GetInstanceHandle();
        }
    }
    return nullptr;
}

void SimSystem::RemoveTask(HSIMTASK hTask, ITaskable *handler) {
    SimTask *task = SimTask::FindInstance(hTask);
    task->Release();
}

void SimSystem::Start(const UCrc32 objclass) {
    mKernel = UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance(objclass, Sim::Param());
}

IRigidBody *SimCollisionMap::GetRB(int rbIndex) const {
    return RigidBody::Get(rbIndex);
}

class IRigidBody *SimCollisionMap::GetSRB(int srbIndex) const {
    return SimpleRigidBody::Get(srbIndex);
}

class IRigidBody *SimCollisionMap::GetOrderedBody(int index) const {
    if (index < RIGID_BODY_MAX) {
        return SimCollisionMap::GetRB(index);
    } else {
        return SimCollisionMap::GetSRB(index - RIGID_BODY_MAX);
    }
}

namespace Sim {
namespace Collision {

// UNSOLVED I think the diff is in UTLAllocator.h Allocator::allocate
void AddListener(IListener *listener, HSIMABLE participant, const char *who) {
    Internal::CDispatcher::Get()->AddListener(listener, participant, who);
}

void AddListener(IListener *listener, const UTL::COM::IUnknown *participant, const char *who) {
    const ISimable *isimable;
    participant->QueryInterface(&isimable);
    Internal::CDispatcher::Get()->AddListener(listener, isimable->GetInstanceHandle(), who);
}

void RemoveListener(IListener *listener, const UTL::COM::IUnknown *participant) {
    const ISimable *isimable;
    participant->QueryInterface(&isimable);
    Internal::CDispatcher::Get()->RemoveListener(listener, isimable->GetInstanceHandle());
}

void RemoveListener(IListener *listener) {
    Internal::CDispatcher::Get()->RemoveListener(listener, nullptr);
}

void AddParticipant(HSIMABLE participant) {
    Internal::CDispatcher::Get()->AddParticipant(participant);
}

void RemoveParticipant(HSIMABLE participant) {
    Internal::CDispatcher::Get()->RemoveParticipant(participant);
}

void Respond(const Info &cinfo) {
    Internal::CDispatcher *dispatcher = Internal::CDispatcher::Get();
    if (dispatcher) {
        dispatcher->Respond(cinfo.objA, cinfo);
        if (cinfo.type == COLLISION_INFO::OBJECT && cinfo.objB) {
            dispatcher->Respond(cinfo.objB, cinfo);
        }
    }
}

}; // namespace Collision

SimRandom &GetRandom() {
    static SimRandom r;

    return r;
}

bool Exists() {
    return Internal::mSystem != nullptr;
}

const State GetState() {
    if (Internal::mSystem) {
        return Internal::mSystem->GetState();
    }
    return STATE_NONE;
}

namespace Internal {

void Update() {
    mSystem->UpdateFrame();
}

}; // namespace Internal

void Update() {
    if (Internal::mSystem) {
        if (Internal::mStackFrame) {
            bDoWithStack((void *)Internal::Update, Internal::mStackFrame, 0, 0);
        } else {
            Internal::Update();
        }
    }
}

const float GetFrameTimeElapsed() {
    if (Internal::mSystem) {
        return Internal::mFrameTime;
    }
    return 0.0f;
}

float GetTimeStep(void) {
    if (Internal::mSystem) {
        return Internal::mSystem->GetTimeStep();
    }
    return 0.0f;
}

float GetSpeed() {
    if (Internal::mSystem) {
        return Internal::mSystem->GetSpeed();
    }
    return 0.0f;
}

float GetTime() {
    return Internal::mTime;
}

void Shutdown() {
    Profile::Shutdown();
    if (Internal::mSystem) {
        delete Internal::mSystem;
        Internal::mSystem = nullptr;
    }
    GetRandom().Reset();
    delete Internal::CDispatcher::Get();
    Internal::mTime = 0.0f;
}

eUserMode GetUserMode() {
    return Internal::mUserMode;
}

namespace Internal {

void CheckHeap() {}

}; // namespace Internal

// UNSOLVED, stack problems, but it's functionally matching
void Init(const UCrc32 activity, eUserMode mode) {
    Internal::CheckHeap();
    Internal::InitTimers();
    new Internal::CDispatcher(); // the instance pointer is saved automatically
    Internal::mUserMode = mode;
    Profile::Init(); // in which line is this?
    GetRandom().Reset();
    Internal::mSystem = new SimSystem();
    Internal::mSystem->Start(activity);
    EventSequencer::Reset(Internal::mTime);
}

bool CanSpawnRigidBody(const UMath::Vector3 &position, bool highPriority) {
    unsigned int spawnLimit = highPriority ? 64 : 52;

    if (RigidBody::GetCount() > spawnLimit) {
        float closestDist = 0.0f;
        ISimable *result = nullptr;
        for (IDisposable::List::const_iterator iter = IDisposable::GetList().begin(); iter != IDisposable::GetList().end(); ++iter) {
            IDisposable *idispose = *iter;
            ISimable *isimable;
            ICollisionBody *irbc;
            if (!idispose->IsRequired() && idispose->QueryInterface(&isimable) && isimable->QueryInterface(&irbc)) {
                IRigidBody &rigidBody = *isimable->GetRigidBody();
                if (irbc->IsSleeping()) {
                    const UMath::Vector3 &pos = rigidBody.GetPosition();
                    float dist = V3DistanceSquared(pos, position);
                    if (dist > closestDist) {
                        closestDist = dist;
                        result = isimable;
                    }
                }
            }
        }
        if (result) {
            result->Kill();
        }
    }
    return RigidBody::GetCount() < spawnLimit;
}

bool CanSpawnSimpleRigidBody(const UMath::Vector3 &position, bool highPriority) {
    const int kLowPrioLimit = 72;
    const unsigned int spawnLimit = highPriority ? 96 : kLowPrioLimit;

    volatile int overflow = SimpleRigidBody::GetCount() - kLowPrioLimit;
    if (overflow >= 0) {
        for (IDisposable::List::const_iterator iter = IDisposable::GetList().begin(); iter != IDisposable::GetList().end() && overflow >= 0; ++iter) {
            IDisposable *idispose = *iter;
            ISimable *isimable;
            if (idispose->IsRequired() && idispose->QueryInterface(&isimable) && isimable->IsRigidBodySimple()) {
                isimable->Kill();
                overflow--;
            }
        }
    }
    return SimpleRigidBody::GetCount() < spawnLimit;
}

void StartProfile() {
    Profile::Capture();
    if (Internal::CDispatcher::Exists()) {
        Internal::CDispatcher::Get()->Profile();
    }
    Internal::mProfileTime = 0;
    Internal::mCallCount = 0;
}

void Suspend() {
    if (Internal::mSystem) {
        Internal::mSystem->PauseInput(true);
    }
}

void SetStream(const UMath::Vector3 &location, bool blocking) {
    if (Internal::mSystem) {
        bVector3 streamLoc;
        bConvertFromBond(streamLoc, location);
        TheTrackStreamer.ClearStreamingPositions();
        TheTrackStreamer.SetStreamingPosition(0, &streamLoc);
        if (blocking) {
            Internal::mSystem->PauseInput(true);
            TheTrackStreamer.ServiceNonGameState();
            TheTrackStreamer.BlockUntilLoadingComplete();
            TheTrackStreamer.SetStreamingPosition(0, &streamLoc);
            TheTrackStreamer.ServiceGameState();
        } else {
            TheTrackStreamer.ServiceGameState();
            TheTrackStreamer.ServiceNonGameState();
        }
    }
}

HSIMTASK AddTask(const UCrc32 &schedule, float rate, ITaskable *handler, float start_offset, TaskMode mode) {
    return Internal::mSystem->AddTask(schedule, rate, handler, start_offset, mode);
}

void RemoveTask(HSIMTASK hTask, ITaskable *handler) {
    Internal::mSystem->RemoveTask(hTask, handler);
}

void ModifyTask(HSIMTASK hTask, float rate) {
    Internal::mSystem->ModifyTask(hTask, rate);
}

float DistanceToCamera(const UMath::Vector3 &v) {
    if (Internal::mSystem) {
        return Internal::mSystem->DistanceToCamera(v);
    } else {
        return 0.0f;
    }
}

void ProfileTask(HSIMTASK htask, const char *name) {}

unsigned int GetTick() {
    return Internal::mTick;
}

}; // namespace Sim
