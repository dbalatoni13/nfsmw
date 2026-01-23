#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/rigidbodyspecs.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICheater.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

// total size: 0x48
class AIActionRace : public AIAction, public Debugable {
  public:
    typedef AIAction Base;

    static AIAction *Construct(AIActionParams *params);

    AIActionRace(AIActionParams *params, float score);
    void ComputePotentials();
    float GetPotentialSpeed(const float curvature, const float skill, bool is_drag) const;
    float GetPotentialAcceleration(const float speed, const float skill, bool using_nos, bool is_drag) const;
    float GetPotentialNOS(float speed, bool was_on, float skill) const;
    void CheckOffPath(float dT);
    float UpdateNavPos(float lookAheadDistance, const UMath::Vector3 &direction);
    bool CheckSpeedTraps(float speed, float skill, float potential_nos, bool was_nos) const;

    virtual void OnDebugDraw();

    // Virtual overrides
    // IUnknown
    ~AIActionRace() override;

    // AIAction
    bool CanBeAttempted(float dT) override;

    bool IsFinished() override {
        return false;
    }

    // Overrides: ITaskable
    bool OnTask(HSIMTASK hTask, float dT) override;

    void BeginAction(float dT) override;
    void FinishAction(float dT) override;
    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    IRigidBody *mIRigidBody;                     // offset 0x4C, size 0x4
    EventSequencer::IEngine *mIEngine;           // offset 0x50, size 0x4
    IInput *mIInput;                             // offset 0x54, size 0x4
    ICheater *mCheater;                          // offset 0x58, size 0x4
    float fSpeedLimit;                           // offset 0x5C, size 0x4
    float fPotentialSpeed;                       // offset 0x60, size 0x4
    float fDriveSpeed;                           // offset 0x64, size 0x4
    bool bIsPursuitMode;                         // offset 0x68, size 0x1
    bool bDontSeekAhead;                         // offset 0x6C, size 0x1
    bool bIsFleeMode;                            // offset 0x70, size 0x1
    bool mTurnAroundActive;                      // offset 0x74, size 0x1
    UMath::Vector3 mLastFindPosition;            // offset 0x78, size 0xC
    IPerpetrator *mPerpetrator;                  // offset 0x84, size 0x4
    float mLastAccel;                            // offset 0x88, size 0x4
    float mLastSpeed;                            // offset 0x8C, size 0x4
    Physics::Info::Performance mPerformanceBias; // offset 0x90, size 0xC
    float mNosCapability;                        // offset 0x9C, size 0x4
    float mUsableNOS;                            // offset 0xA0, size 0x4
    float mBottleTime;                           // offset 0xA4, size 0x4
    float mStartGrip;                            // offset 0xA8, size 0x4
    float mEndGrip;                              // offset 0xAC, size 0x4
    float mTopSpeed;                             // offset 0xB0, size 0x4
    float mUnstageTimer;                         // offset 0xB4, size 0x4
    float mNOSTimer;                             // offset 0xB8, size 0x4
    HSIMTASK mResetTask;                         // offset 0xBC, size 0x4
};

AIActionRace::AIActionRace(AIActionParams *params, float score)
    : AIAction(params, score),                                    //
      fSpeedLimit(0.0f),                                          //
      fPotentialSpeed(0.0f),                                      //
      fDriveSpeed(0.0f),                                          //
      bIsPursuitMode(false),                                      //
      bDontSeekAhead(false),                                      //
      mLastFindPosition(UMath::Vector3Make(0.0f, -1.0e8f, 0.0f)), //
      mLastAccel(0.0f),                                           //
      mLastSpeed(0.0f),                                           //
      mNOSTimer(0.0f),                                            //
      mNosCapability(0.0f),                                       //
      mUsableNOS(1.0f),                                           //
      mBottleTime(0.0f),                                          //
      mStartGrip(0.0f),                                           //
      mEndGrip(0.0f),                                             //
      mTopSpeed(0.0f),                                            //
      mUnstageTimer(0.0f),                                        //
      mResetTask(nullptr) {
    MakeDebugable(DBG_AI);
    mIRigidBody = params->mOwner->GetRigidBody();
    params->mOwner->QueryInterface(&mIEngine);
    params->mOwner->QueryInterface(&mIInput);
    params->mOwner->QueryInterface(&mCheater);
    params->mOwner->QueryInterface(&mPerpetrator);
    mTurnAroundActive = false;
    ComputePotentials();
}

void AIActionRace::OnBehaviorChange(const UCrc32 &mechanic) {
    if (BEHAVIOR_MECHANIC_INPUT == mechanic) {
        GetOwner()->QueryInterface(&mIInput);
    } else if (BEHAVIOR_MECHANIC_RIGIDBODY == mechanic) {
        GetOwner()->QueryInterface(&mIRigidBody);
    } else if (BEHAVIOR_MECHANIC_ENGINE == mechanic) {
        GetOwner()->QueryInterface(&mIEngine);
    }
}

bool AIActionRace::OnTask(HSIMTASK hTask, float dT) {
    if (hTask == mResetTask) {
        CheckOffPath(dT);
        return true;
    }
    return false;
}

AIAction *AIActionRace::Construct(AIActionParams *params) {
    return new AIActionRace(params, 0.0f);
}

bool AIActionRace::CanBeAttempted(float dT) {
    if (mIRigidBody && GetAI() && GetVehicle() && mIEngine) {
        if (!mIInput) {
            return false;
        }
        // TODO this isn't in a scope
        WRoadNav test_nav;
        const float dir_weight = 1.0f;
        const bool force_centre_lane = true;
        UMath::Vector3 forwardVector;
        mIRigidBody->GetForwardVector(forwardVector);
        test_nav.SetNavType(WRoadNav::kTypeDirection);
        test_nav.InitAtPoint(mIRigidBody->GetPosition(), forwardVector, force_centre_lane, dir_weight);
        return test_nav.IsValid();
    }
    return false;
}

AIActionRace::~AIActionRace() {
    if (mResetTask) {
        RemoveTask(mResetTask);
        mResetTask = nullptr;
    }
}

void AIActionRace::BeginAction(float dT) {
    const bool force_centre_lane = false;
    WRoadNav *road_nav = GetAI()->GetDriveToNav();
    road_nav->SetNavType(WRoadNav::kTypeDirection);
    road_nav->SetLaneType(WRoadNav::kLaneRacing);
    road_nav->SetCookieTrail(true);
    road_nav->ResetCookieTrail();
    GetAI()->ResetDriveToNav(SELECT_VALID_LANE);
    GetAI()->GetLastSpawnTime();

    IPursuitAI *ipv;
    GetAI()->QueryInterface(&ipv);

    bIsFleeMode = GetAI()->GetGoalName() == UCrc32("AIGoalFleePursuit");

    // TODO
    if (!bIsFleeMode) {
        if (GetAI()->GetPursuit()) {
            bIsPursuitMode = ComparePtr(GetAI()->GetTarget()->GetSimable(), GetAI()->GetPursuit()->GetTarget()->GetSimable());
        } else {
            bIsPursuitMode = false;
        }
    } else {
        bIsPursuitMode = true;
    }

    // TODO
    if (bIsPursuitMode && ipv) {
        bDontSeekAhead = ipv->GetSupportGoal() == "AIGoalHeadOnRam";
    } else {
        bDontSeekAhead = false;
    }

    ComputePotentials();

    mLastSpeed = GetVehicle()->GetSpeed();
    mLastAccel = 0.0f;
    mUnstageTimer = 0.0f;
    fSpeedLimit = UMath::Max(mLastSpeed, 0.0f);
    mNOSTimer = 0.0f;

    if (bIsFleeMode) {
        GetAI()->GetDriveToNav()->SetRaceFilter(false);
    }

    if (mResetTask == 0) {
        mResetTask = AddTask("Physics", 0.25f, 1.0f, Sim::TASK_FRAME_FIXED);
        Sim::ProfileTask(mResetTask, "AIActionRace");
    }
}

void AIActionRace::FinishAction(float dT) {
    WRoadNav *road_nav = GetAI()->GetDriveToNav();
    if (road_nav) {
        road_nav->SetLaneType(WRoadNav::kLaneRacing);
    }
    if (mResetTask) {
        RemoveTask(mResetTask);
        mResetTask = nullptr;
    }
}

// total size: 0xC
struct GripTor {
    GripTor(IVehicle *vehicle) {
        StartGrip = 0.0f;
        EndGrip = 0.0f;
        Valid = false;

        IVehicleAI *ai;
        if (vehicle->QueryInterface(&ai)) {
            Attrib::Gen::pvehicle pvehicle(vehicle->GetVehicleAttributes());
            Attrib::Gen::chassis chassis(pvehicle.chassis(0), 0, nullptr);
            Attrib::Gen::tires tires(pvehicle.tires(0), 0, nullptr);
            Attrib::Gen::rigidbodyspecs rigidbodyspecs(pvehicle.rigidbodyspecs(), 0, nullptr);

            float gravity = rigidbodyspecs.GRAVITY();
            StartGrip = UMath::Min(tires.STATIC_GRIP().Front, tires.STATIC_GRIP().Rear);
            float down = -Physics::Info::AerodynamicDownforce(chassis, ai->GetTopSpeed()) / pvehicle.MASS() + gravity;
            EndGrip = StartGrip * down / gravity;
            Valid = true;
        }
    }

    void operator()(IVehicle *vehicle) {
        GripTor g(vehicle);
        if (g.Valid) {
            StartGrip = UMath::Min(g.StartGrip, StartGrip);
            EndGrip = UMath::Min(g.EndGrip, EndGrip);
        }
    }

    float StartGrip; // offset 0x0, size 0x4
    float EndGrip;   // offset 0x4, size 0x4
    bool Valid;      // offset 0x8, size 0x1
};

// total size: 0x8
struct NosTor {
    NosTor(IVehicle *vehicle) {
        Attrib::Gen::nos nos(vehicle->GetVehicleAttributes().nos(0), 0, nullptr);
        Boost = UMath::Max(Physics::Info::NosBoost(nos, vehicle->GetTunings()) - 1.0f, 0.0f);
        Capacity = Physics::Info::NosCapacity(nos, vehicle->GetTunings());
    }

    void operator()(IVehicle *vehicle) {
        NosTor n(vehicle);
        Boost = UMath::Min(Boost, n.Boost);
        Capacity = UMath::Min(Capacity, n.Capacity);
    }

    float Boost;    // offset 0x0, size 0x4
    float Capacity; // offset 0x4, size 0x4
};

// total size: 0x4
struct SpeedTor {
    SpeedTor(IVehicle *vehicle) {
        Speed = 0.0f;
        if (!vehicle) {
            return;
        }
        IVehicleAI *ai;
        if (vehicle->QueryInterface(&ai)) {
            Speed = ai->GetTopSpeed();
        }
    }

    void operator()(IVehicle *vehicle) {
        SpeedTor s(vehicle);

        if (s.Speed > 0.0f) {
            Speed = UMath::Min(s.Speed, Speed);
        }
    }

    float Speed; // offset 0x0, size 0x4
};

// total size: 0x10
struct PerformaTor {
    PerformaTor() {
        Valid = false;
    }

    void operator()(IVehicle *vehicle) {
        Physics::Info::Performance p;
        if (vehicle->GetPerformance(p)) {
            Performance.Maximize(p);
            Valid = true;
        }
    }

    Physics::Info::Performance Performance; // offset 0x0, size 0xC
    bool Valid;                             // offset 0xC, size 0x1
};

void AIActionRace::ComputePotentials() {
    if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        float min_perf = 0.0f;

        if (mPerpetrator) {
            GRacerInfo *racer_info = mPerpetrator->GetRacerInfo();
            if (racer_info && racer_info->GetGameCharacter()) {
                min_perf = racer_info->GetGameCharacter()->MinimumAIPerformance();
            }
        }

        PerformaTor max_player = IVehicle::ForEach(VEHICLE_PLAYERS, PerformaTor());

        if (max_player.Valid) {
            mPerformanceBias.Acceleration = UMath::Ramp(min_perf, max_player.Performance.Acceleration, 1.0f);
            mPerformanceBias.Handling = UMath::Ramp(min_perf, max_player.Performance.Handling, 1.0f);
            mPerformanceBias.TopSpeed = UMath::Ramp(min_perf, max_player.Performance.TopSpeed, 1.0f);
        }
    } else {
        mPerformanceBias.Default();
    }

    GripTor my_grip(GetVehicle());
    GripTor lowest_grip = IVehicle::ForEach(VEHICLE_PLAYERS, my_grip);

    mStartGrip = UMath::Lerp(lowest_grip.StartGrip, my_grip.StartGrip, mPerformanceBias.Handling);
    mEndGrip = UMath::Lerp(lowest_grip.EndGrip, my_grip.EndGrip, mPerformanceBias.Handling);

    NosTor my_nos(GetVehicle());
    NosTor lowest_nos = IVehicle::ForEach(VEHICLE_PLAYERS, my_nos);

    mNosCapability = UMath::Lerp(lowest_nos.Boost, my_nos.Boost, mPerformanceBias.Acceleration);

    if (bIsPursuitMode || bIsFleeMode) {
        mUsableNOS = 1.0f;
        mBottleTime = my_nos.Capacity;
    } else {
        if (my_nos.Capacity > FLOAT_EPSILON && mNosCapability > 0.0f) {
            mUsableNOS = (lowest_nos.Capacity / my_nos.Capacity) * (mNosCapability / my_nos.Boost);
            mBottleTime = my_nos.Capacity;
        } else {
            mUsableNOS = 0.0f;
            mBottleTime = 0.0f;
        }
    }

    SpeedTor my_speed(GetVehicle());
    SpeedTor lowest_speed = IVehicle::ForEach(VEHICLE_PLAYERS, my_speed);

    mTopSpeed = UMath::Lerp(lowest_speed.Speed, my_speed.Speed, mPerformanceBias.TopSpeed);
}

void AIActionRace::Update(float dT) {}

void AIActionRace::OnDebugDraw() {}
