#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Gameplay/GTrigger.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/rigidbodyspecs.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICheater.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

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
    IEngine *mIEngine;                           // offset 0x50, size 0x4
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

UTL::COM::Factory<AIActionParams *, AIAction, UCrc32>::Prototype _AIActionRace("AIActionRace", AIActionRace::Construct);

bool AIActionRace::CanBeAttempted(float dT) {
    if (!mIRigidBody) {
        return false;
    }
    if (!GetAI()) {
        return false;
    }
    if (!GetVehicle()) {
        return false;
    }
    if (!mIEngine) {
        return false;
    }
    if (!mIInput) {
        return false;
    }
    WRoadNav test_nav;
    const float dir_weight = 1.0f;
    const bool force_centre_lane = true;
    UMath::Vector3 forwardVector;
    mIRigidBody->GetForwardVector(forwardVector);
    test_nav.SetNavType(WRoadNav::kTypeDirection);
    test_nav.InitAtPoint(mIRigidBody->GetPosition(), forwardVector, force_centre_lane, dir_weight);
    return test_nav.IsValid();
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

    bIsFleeMode = GetAI()->GetGoalName() == "AIGoalFleePursuit";

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

float GetSpeedLimitForCurvature(float friction, float curvature, float top_speed) {
    const float gravity = 9.8f;
    float side_force = friction * gravity;
    float min_denominator = side_force / (top_speed * top_speed);
    float denominator = bSqrt(side_force / bMax(min_denominator, bAbs(curvature)));

    return denominator;
}

float GetSpeedLimit(float curvature, float f0, float f1, float top_speed) {
    float g = 9.8f;
    float gf1 = f1 * g;
    float abs_curvature = bAbs(curvature);
    float numerator = gf1 + bSqrt((gf1 * gf1) + (abs_curvature * 4.0f * g * f0));
    float denominator = numerator / bMax(numerator / bMax(0.1f, top_speed), 2.0f * abs_curvature);

    return denominator;
}

float aNosScaleData[2] = {0.25f, 1.0f};
Table AiNosScaleTable(aNosScaleData, 2, 0.0f, 1.0f);

float aSpeedScaleData[2] = {0.85f, 1.0f};
Table AiSpeedScaleTable(aSpeedScaleData, 2, 0.0f, 1.0f);

float aSpeedScaleDataDrag[2] = {0.85f, 1.0f};
Table AiSpeedScaleTableDrag(aSpeedScaleDataDrag, 2, 0.0f, 1.0f);

float aAccelScaleData[2] = {0.65f, 1.0f};
Table AiAccelScaleTable(aAccelScaleData, 2, 0.0f, 1.0f);

float aAccelScaleDataDrag[2] = {0.82f, 1.0f};
Table AiAccelScaleTableDrag(aAccelScaleDataDrag, 2, 0.0f, 1.0f);

float AiCatchupAccelerationData[4] = {1.0f, 1.33f, 1.66f, 2.0f};
Table AiCatchupAcceleration(AiCatchupAccelerationData, 4, 0.0f, 1.0f);

float aCorneringScaleData[2] = {0.36f, 0.9f};
Table AICorneringScaleTable(aCorneringScaleData, 2, 0.0f, 1.0f);

// UNSOLVED nightmare
// https://decomp.me/scratch/jAGQL
float AIActionRace::GetPotentialSpeed(const float curvature, const float skill, bool is_drag) const {
    float result = mTopSpeed;
    float maxdesired = -1.0f;

    if (mTopSpeed <= 0.0f) {
        result = 0.0f;
    } else if (!is_drag) {
        if (!bIsPursuitMode || bIsFleeMode) {
            if (GetAI()->GetDriveToNav()->HitDeadEnd()) {
                return 0.0f;
            }
            float scale = AICorneringScaleTable.GetValue(skill);
            float start_grip = mStartGrip;
            float end_grip = mEndGrip;
            float f0 = start_grip;
            float f1 = (UMath::Lerp(start_grip, end_grip, scale) - start_grip) / mTopSpeed;
            result = GetSpeedLimit(curvature, f0, f1, mTopSpeed);
        } else {
            WRoadNav *road_nav = GetAI()->GetDriveToNav();
            UMath::Vector3 myForwardVector;
            mIRigidBody->GetForwardVector(myForwardVector);

            UMath::Vector3 navForwardVector = road_nav->GetForwardVector();
            UMath::Normalize(navForwardVector);

            UMath::Vector3 seek_dir;
            AITarget *target = GetAI()->GetTarget();

            UMath::Sub(mLastFindPosition, target->GetPosition(), seek_dir);
            UMath::Normalize(seek_dir);

            UMath::Vector3 steerDir;
            UMath::Sub(road_nav->GetPosition(), mIRigidBody->GetPosition(), steerDir);
            UMath::Vector3 targetSteerDir = steerDir;
            IVehicleAI *targetai;
            if (target->QueryInterface(&targetai)) {
                UMath::Sub(targetai->GetDriveToNav()->GetPosition(), target->GetPosition(), targetSteerDir);
            }
            UMath::Normalize(steerDir);
            UMath::Normalize(targetSteerDir);
            UMath::Vector3 offset_to_target;

            UMath::Vector3 targetPosition = target->GetPosition();
            UMath::Sub(mIRigidBody->GetPosition(), targetPosition, offset_to_target);

            float scalar_offset_to_target = UMath::Dot(offset_to_target, seek_dir);
            float forward_near_speed = target->GetSpeed();
            float var_f13 = scalar_offset_to_target > 0.0f ? 100.0f : 200.0f;
            forward_near_speed -= scalar_offset_to_target * 0.01f * KPH2MPS(var_f13);
            float distant_cop_speed = KPH2MPS(GetAI()->GetAttributes().MAXIMUM_AI_SPEED());
            if (GetAI()->GetPursuit() && GetAI()->GetPursuit()->GetIsAJerk()) {
                distant_cop_speed *= 1.1f;
            }
            float temp_f13 = bClamp(forward_near_speed, KPH2MPS(10.0f), distant_cop_speed);
            float var_f31 =
                scalar_offset_to_target > 0.0f ? ((scalar_offset_to_target * 0.01f * KPH2MPS(50.0f)) - target->GetSpeed()) : distant_cop_speed;
            float reverse_near_speed = bClamp(var_f31, KPH2MPS(40.0f), distant_cop_speed);
            float near_speed =
                bClamp(UMath::Dot(myForwardVector, seek_dir) + 0.5f, 0.0f, 1.0f) * (temp_f13 - reverse_near_speed) + reverse_near_speed;

            UMath::Vector3 side_offset;
            UMath::ScaleAdd(seek_dir, -scalar_offset_to_target, offset_to_target, side_offset);
            // side_offset_to_target
            float temp_f1_7 = UMath::Length(side_offset) * 2.5f;
            if (scalar_offset_to_target > 0.0f) {
                scalar_offset_to_target *= 0.5f;
            }
            float temp_f1_8 = UMath::Sqrt((scalar_offset_to_target * scalar_offset_to_target) + (temp_f1_7 * temp_f1_7));
            temp_f1_8 = 1.0f - (temp_f1_8 - 150.0f) * 0.006666667f;
            float temp_f1_9 = bClamp(temp_f1_8, 0.0f, 1.0f);
            float temp_f13_2 = UMath::Dot(steerDir, targetSteerDir);
            float temp_f31_4 = bClamp(UMath::Abs(temp_f13_2) + 0.2f, 0.0f, 1.0f) * temp_f1_9;
            float temp_f31_5 = bClamp((temp_f31_4 * near_speed) + (1.0f - temp_f31_4) * distant_cop_speed, 0.0f, distant_cop_speed);

            float f0 = mStartGrip;
            float f1 = (mEndGrip - mStartGrip) / mTopSpeed;
            float speed = !road_nav->HitDeadEnd() ? GetSpeedLimit(curvature, f0, f1, mTopSpeed) : 0.0f;

            result = UMath::Min(temp_f31_5, speed);
            maxdesired = temp_f31_5;
        }
    }

    float attrib_scale = GetAI()->GetAttributes().TopSpeedMultiplier();

    if (bIsPursuitMode && GRaceStatus::Exists() && GRaceStatus::Get().GetActivelyRacing()) {
        attrib_scale += attrib_scale;
    } else {
        if (GetAI()->GetPursuit() && GetAI()->GetPursuit()->GetIsAJerk()) {
            attrib_scale *= 1.2f;
        }
    }

    float skill_scale = is_drag ? AiSpeedScaleTableDrag.GetValue(skill) : AiSpeedScaleTable.GetValue(skill);

    result *= attrib_scale * skill_scale;
    if (maxdesired > 0.0f) {
        result = bMin(maxdesired, result);
    }
    return result;
}

// total size: 0x8
struct AccelTor {
    AccelTor(float speed, const IVehicleAI *ai) {
        Speed = speed;
        Accel = ai->GetAcceleration(speed);
    }

    void operator()(const IVehicle *vehicle) {
        const IVehicleAI *ai = vehicle->GetAIVehiclePtr();
        if (ai) {
            AccelTor a(Speed, ai);
            Accel = UMath::Min(Accel, a.Accel);
        }
    }

    float Speed; // offset 0x0, size 0x4
    float Accel; // offset 0x4, size 0x4
};

float AIActionRace::GetPotentialAcceleration(const float speed, const float skill, bool using_nos, bool is_drag) const {
    AccelTor my_accel(speed, GetAI());
    AccelTor lowest_accel = IVehicle::ForEach(VEHICLE_PLAYERS, my_accel);
    float result = UMath::Lerp(lowest_accel.Accel, my_accel.Accel, mPerformanceBias.Acceleration);

    float attrib_scale = GetAI()->GetAttributes().AccelerationMultiplier();

    if (bIsPursuitMode && GRaceStatus::Exists() && GRaceStatus::Get().GetActivelyRacing()) {
        attrib_scale *= 2.0f;
    } else {
        if (GetAI()->GetPursuit() && GetAI()->GetPursuit()->GetIsAJerk()) {
            attrib_scale *= 1.5f;
        }
    }

    const float accel_scale = is_drag ? AiAccelScaleTableDrag.GetValue(skill) : AiAccelScaleTable.GetValue(skill);

    float nos_scale = using_nos ? mNosCapability + 1.0f : 1.0f;

    float catchup_scale = 1.0f;
    float gravity_acc = 0.0f;
    if (mIRigidBody && GetVehicle()->GetPhysicsMode() == PHYSICS_MODE_SIMULATED) {
        UMath::Vector3 forward;
        mIRigidBody->GetForwardVector(forward);
        const float Gravity = 9.81f;
        const float grade = forward.y;
        gravity_acc = -Gravity * grade;
    }

    if (mCheater) {
        catchup_scale = AiCatchupAcceleration.GetValue(mCheater->GetCatchupCheat());
    }

    result = result * attrib_scale * nos_scale * accel_scale * catchup_scale + gravity_acc;

    return UMath::Max(result, 0.0f);
}

float AIActionRace::GetPotentialNOS(float speed, bool was_on, float skill) const {
    if (speed < 10.0f || speed >= fSpeedLimit) {
        return 0.0f;
    }
    if (mNosCapability <= 0.0f || mUsableNOS <= 0.0f) {
        return 0.0f;
    }
    if (!mIEngine) {
        return 0.0f;
    }
    float useable_nos = mUsableNOS * UMath::Lerp(0.33f, 1.0f, skill);
    float off_limit = 1.0f - useable_nos;
    float on_limit; // TODO
    float needed_capacity = (1.0f - off_limit) * UMath::Lerp(0.5f, 0.3f, skill);
    if (was_on) {
        needed_capacity = off_limit;
    }
    float bottle_amount = mIEngine->GetNOSCapacity();
    if (bottle_amount <= needed_capacity) {
        return 0.0f;
    }
    return (bottle_amount - needed_capacity) * mBottleTime;
}

float aAiNavLookAheadData[2] = {30.0f, 100.0f};
Table AiNavLookAheadTable(aAiNavLookAheadData, 2, 0.0f, 100.0f);

float aAiDragNavLookAheadData[2] = {30.0f, 100.0f};
Table AiDragNavLookAheadTable(aAiDragNavLookAheadData, 2, 0.0f, 100.0f);

void AIActionRace::CheckOffPath(float dT) {
    WRoadNav *road_nav = GetAI()->GetDriveToNav();
    if (!road_nav) {
        return;
    }
    IRigidBody *rigid_body = GetOwner()->GetRigidBody();
    if (!rigid_body) {
        return;
    }
    UMath::Vector3 car_forward_vector;

    GetVehicle()->ComputeHeading(&car_forward_vector);
    float current_speed = rigid_body->GetSpeed();
    bool reset_nav = false;

    if (!bIsPursuitMode) {
        float old_out_of_bounds = road_nav->GetOutOfBounds();
        if (old_out_of_bounds > 2.0f) {
            WRoadNavWithCookies nav;
            nav.SetNavType(WRoadNav::kTypeDirection);
            nav.SetPathType(road_nav->GetPathType());
            nav.SetLaneType(road_nav->GetLaneType());
            nav.SetRaceFilter(road_nav->GetRaceFilter());
            nav.SetTrafficFilter(road_nav->GetTrafficFilter());
            nav.SetDecisionFilter(road_nav->GetDecisionFilter());

            nav.InitAtPoint(rigid_body->GetPosition(), car_forward_vector, false, 1.0f);

            if (nav.IsValid() && !nav.GetSegment()->IsDecision()) {
                int segment_number = nav.GetSegmentInd();
                if (!road_nav->IsSegmentInCookieTrail(segment_number, false) && !road_nav->IsSegmentInPath(segment_number)) {
                    const bool occlude_avoidables = false;
                    nav.UpdateOccludedPosition(occlude_avoidables);
                    float new_out_of_bounds = nav.GetOutOfBounds();
                    if (new_out_of_bounds < old_out_of_bounds) {
                        reset_nav = true;
                    }
                }
            }
        }
    }

    if (reset_nav) {
        GetAI()->ResetDriveToNav(SELECT_VALID_LANE);
        road_nav->SetNavType(WRoadNav::kTypeDirection);
        float look_ahead = AiNavLookAheadTable.GetValue(current_speed);
        road_nav->IncNavPosition(look_ahead, car_forward_vector, 0.0f);
        road_nav->UpdateOccludedPosition(true);
    }
}

float AIActionRace::UpdateNavPos(float lookAheadDistance, const UMath::Vector3 &direction) {
    if (GetAI()->GetDriveToNav()->HitDeadEnd()) {
        return 0.0f;
    }
    WRoadNav *road_nav = GetAI()->GetDriveToNav();
    UMath::Vector3 navPos = road_nav->GetPosition();
    UMath::Vector3 carPosition = mIRigidBody->GetPosition();
    UMath::Vector3 carToNav = navPos - carPosition;
    float nav_distance = UMath::Length(carToNav);
    UMath::Vector3 navForwardVector = road_nav->GetForwardVector();
    UMath::Normalize(navForwardVector);

    const UMath::Vector3 &N = carToNav;
    const UMath::Vector3 &F = navForwardVector;
    float a = F.x * F.x + F.z * F.z;
    float b = 2 * (N.x * F.x + N.z * F.z);
    float c = N.x * N.x + N.z * N.z - lookAheadDistance * lookAheadDistance;

    float square_term = b * b - 4 * a * c;
    float denominator = 2 * a;
    if ((square_term >= 0.0f) && (denominator > 1e-05f)) {
        float inc_distance = (-b + bSqrt(square_term)) / denominator;
        if (inc_distance > 0.0f) {
            road_nav->IncNavPosition(inc_distance, direction, lookAheadDistance);
        }
    }

    road_nav->UpdateOccludedPosition(true);
    return nav_distance;
}

bool AIActionRace::CheckSpeedTraps(float speed, float skill, float potential_nos, bool was_nos) const {
    if (mPerpetrator && GRaceStatus::Get().GetActivelyRacing()) {
        if (!GRaceStatus::IsSpeedTrapRace()) {
            return false;
        }
        GRacerInfo *info = mPerpetrator->GetRacerInfo();
        if (info && speed >= 10.0f) {
            if (!was_nos && speed > fSpeedLimit - 5.0f) {
                return false;
            }
            const UMath::Vector3 &my_position = mIRigidBody->GetPosition();
            UMath::Vector3 my_direction;
            mIRigidBody->GetForwardVector(my_direction);
            int num_speed_traps = GRaceStatus::Get().GetNumRaceSpeedTraps();
            for (int i = 0; i < num_speed_traps; i++) {
                GTrigger *trap = GRaceStatus::Get().GetRaceSpeedTrap(i);
                if (trap->IsEnabled()) {
                    UMath::Vector3 position;
                    trap->GetPosition(position);
                    float look_ahead = speed * (skill + skill + 1.0f);
                    if (UMath::DistanceSquare(position, my_position) < look_ahead * look_ahead) {
                        UMath::Vector3 relative_position;
                        UMath::Sub(position, my_position, relative_position);
                        float distance = UMath::Normalize(relative_position);
                        float time_ahead = distance / speed;
                        if ((time_ahead < potential_nos || was_nos) &&
                            UMath::Dot(my_direction, relative_position) > 0.85f) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void AIActionRace::Update(float dT) {
    bool need_to_stop = GetVehicle()->IsDestroyed();
    bool drag_racing = GetVehicle()->GetDriverStyle() == 1;
    bool is_staging = GetVehicle()->IsStaging();
    WRoadNav *road_nav = GetAI()->GetDriveToNav();
    UMath::Vector3 desired_direction = UMath::Vector3::kZero;
    const UMath::Vector3 &car_position = mIRigidBody->GetPosition();

    if (bIsPursuitMode) {
        road_nav->SetLaneType(WRoadNav::kLaneRacing);
        if (!road_nav->FindingPath()) {
            AITarget *target = GetAI()->GetTarget();
            IVehicleAI *targetai;
            target->QueryInterface(&targetai);
            UMath::Vector3 findPosition;

            if (bIsFleeMode) {
                UMath::Vector3 fleecenter;
                UMath::Vector3 fleeforward;
                IPlayer *iplayer = IPlayer::First(PLAYER_LOCAL);
                UMath::Vector3 offset;
                float offlen;

                if (iplayer) {
                    fleecenter = iplayer->GetSimable()->GetPosition();
                    iplayer->GetSimable()->GetLinearVelocity(fleeforward);
                    fleeforward.y = 0.0f;
                    offlen = UMath::Normalize(fleeforward);
                    if (offlen < 5.0f) {
                        iplayer->GetSimable()->GetRigidBody()->GetForwardVector(fleeforward);
                    }
                } else {
                    fleecenter = target->GetPosition();
                    fleeforward = target->GetLinearVelocity();
                    fleeforward.y = 0.0f;
                    offlen = UMath::Normalize(fleeforward);
                    if (offlen < 5.0f) {
                        target->GetSimable()->GetRigidBody()->GetForwardVector(fleeforward);
                    }
                }

                UMath::Sub(fleecenter, car_position, offset);
                float dist = UMath::Length(offset);
                UMath::Scale(fleeforward, dist * 0.5f, offset);
                UMath::ScaleAdd(offset, 1.0f, fleecenter, findPosition);

                if (UMath::Distance(findPosition, car_position) < 50.0f) {
                    findPosition = mLastFindPosition;
                }
                if (UMath::Distance(target->GetPosition(), car_position) < 100.0f) {
                    findPosition = mLastFindPosition;
                }
            } else {
                findPosition = target->GetPosition();
            }

            mLastFindPosition = findPosition;
            road_nav->FindPath(&findPosition, nullptr, nullptr);
        }

        UMath::Vector3 temp = UVector3(mLastFindPosition) - car_position;
        desired_direction = temp;
    } else {
        WRoadNav::ELaneType lane = WRoadNav::kLaneRacing;
        if (drag_racing) {
            lane = WRoadNav::kLaneDrag;
        }
        road_nav->SetLaneType(lane);

        if (GRaceStatus::Exists() &&
            GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing &&
            GRaceStatus::Get().GetRaceParameters() &&
            GRaceStatus::Get().GetRaceParameters()->HasFinishLine()) {
            AITarget *target = GetAI()->GetTarget();
            if (!target->IsValid() || GRaceStatus::Get().GetRaceRouteError()) {
                need_to_stop = true;
            } else {
                const UMath::Vector3 &target_position = target->GetPosition();
                UMath::Vector3 car_to_target = UVector3(target_position) - car_position;
                WRoadNav *rn = GetAI()->GetDriveToNav();
                if (rn->GetNavType() != WRoadNav::kTypePath && !rn->FindingPath()) {
                    bool find_path = true;
                    if (UMath::DistanceSquare(target_position, mLastFindPosition) < 10000.0f) {
                        if (rn->IsGoalInCookieTrail()) {
                            find_path = false;
                        } else {
                            find_path = UMath::LengthSquare(car_to_target) > 0.0f;
                        }
                    }
                    if (find_path) {
                        UMath::Vector3 target_direction = target->GetDirection();
                        rn->FindPath(&target_position, &target_direction, nullptr);
                        mLastFindPosition = target_position;
                    }
                }
            }
        }
    }

    Table &nav_look_ahead_table = drag_racing ? AiDragNavLookAheadTable : AiNavLookAheadTable;
    float look_ahead_distance = nav_look_ahead_table.GetValue(fSpeedLimit);
    UpdateNavPos(look_ahead_distance, desired_direction);
    GetAI()->SetAvoidableRadius(look_ahead_distance);

    WRoadNav *nav = GetAI()->GetDriveToNav();
    const UMath::Vector3 *nav_pos_ptr;
    if (nav->IsOccluded()) {
        nav_pos_ptr = &nav->GetOccludedPosition();
    } else {
        const UMath::Vector3 &apex = nav->GetApexPosition();
        nav_pos_ptr = &apex;
    }

    UMath::Vector3 car_velocity;
    mIRigidBody->GetForwardVector(car_velocity);
    float curvature = nav->CookieTrailCurvature(car_position, car_velocity);

    const float skill = GetAI()->GetSkill();
    fPotentialSpeed = GetPotentialSpeed(curvature, skill, drag_racing);
    const float actual_speed = mIRigidBody->GetSpeed();
    const float nos_capacity = mIEngine->GetNOSCapacity();
    InputControls &controls = mIInput->GetControls();
    const bool was_nos = controls.fNOS && nos_capacity > 0.0f;

    float drive_speed = fSpeedLimit;
    if (fSpeedLimit < actual_speed) {
        drive_speed = actual_speed;
    }

    const float potential_acceleration = GetPotentialAcceleration(drive_speed, skill, was_nos, drag_racing);
    float last_speed = mLastSpeed;
    mLastSpeed = actual_speed;
    const float actual_acceleration = (actual_speed - last_speed) / dT;
    mLastAccel = actual_acceleration;

    if (actual_acceleration >= 0.0f) {
        if (fSpeedLimit < fPotentialSpeed) {
            float t = UMath::Ramp(actual_speed, 0.0f, fPotentialSpeed);
            float exp = UMath::Lerp(1.0f, 3.0f, skill);
            float delta_acc = UMath::Pow(t, exp);
            delta_acc = UMath::Clamp(potential_acceleration - actual_acceleration * delta_acc,
                                     0.0f, potential_acceleration);
            fSpeedLimit += delta_acc * dT;
        }
    } else {
        if (actual_speed < fSpeedLimit) {
            float delta_acc = UMath::Min(potential_acceleration + actual_acceleration, 0.0f);
            fSpeedLimit += delta_acc * dT;
        }
        if (fSpeedLimit < fPotentialSpeed) {
            float t = UMath::Ramp(actual_speed, 0.0f, fPotentialSpeed);
            float exp = UMath::Lerp(1.0f, 3.0f, skill);
            float delta_acc = UMath::Pow(t, exp);
            delta_acc = UMath::Clamp(
                potential_acceleration - actual_acceleration * delta_acc,
                0.0f, potential_acceleration);
            fSpeedLimit += delta_acc * dT;
        }
    }

    fSpeedLimit = UMath::Max(fSpeedLimit, 0.0f);
    drive_speed = UMath::Clamp(fSpeedLimit, 0.0f, fPotentialSpeed);

    if (is_staging) {
        mUnstageTimer = 2.0f;
        drive_speed = 0.0f;
    } else {
        if (mUnstageTimer > 0.0f) {
            drive_speed = UMath::Lerp(0.3f, 0.7f, skill) * mTopSpeed;
            float t = mUnstageTimer - dT;
            mUnstageTimer = UMath::Max(t, 0.0f);
        }
    }

    drive_speed = UMath::Lerp(drive_speed, drive_speed * MPH2MPS(1.0f), mUnstageTimer * 0.5f);

    UMath::Vector3 drive_target;
    if (nav->IsOccluded()) {
        drive_target = nav->GetOccludedPosition();
    } else {
        drive_target = nav->GetPosition();
    }

    if (need_to_stop) {
        drive_speed = 0.0f;
    }

    mTurnAroundActive = false;
    {
        const float kTurnAroundSpeed = KPH2MPS(30.0f);
        if (mIRigidBody->GetSpeed() > kTurnAroundSpeed && bIsPursuitMode) {
            UMath::Vector3 race_steer;
            UMath::Sub(drive_target, car_position, race_steer);
            float dirdot = UMath::Dot(car_velocity, race_steer);
            float dist = UMath::Length(race_steer);

            if (dirdot < mIRigidBody->GetSpeed() * dist * -0.5f) {
                WRoadNav *future_nav = GetAI()->GetFutureRoad();
                drive_target = future_nav->GetPosition();
                drive_speed = 0.0f;
                mTurnAroundActive = true;
            }
        }
    }

    GetAI()->SetDriveTarget(drive_target);
    GetAI()->SetDriveSpeed(drive_speed);
    fDriveSpeed = drive_speed;

    float potential_nos = GetPotentialNOS(actual_speed, was_nos, skill);
    bool want_nos = false;
    if (!mTurnAroundActive && !need_to_stop && !is_staging) {
        want_nos = potential_nos > 0.0f;
    }

    int nos_flag = 0;
    if (want_nos && !GetOwner()->IsPlayer()) {
        float skill_scale = AiNosScaleTable.GetValue(skill);
        bool speed_trap = CheckSpeedTraps(actual_speed, skill, potential_nos, was_nos);
        if (speed_trap) {
            skill_scale += 1.0f;
        }

        float speed_gap = skill_scale * (fPotentialSpeed - actual_speed);
        float accel_gap = skill_scale * (potential_acceleration - actual_acceleration);

        if (was_nos) {
            nos_flag = 1;
            float time_on = UMath::Lerp(1.0f, 3.0f, skill);
            if (time_on < mNOSTimer &&
                (speed_gap < 0.5f || accel_gap < potential_acceleration * 0.25f)) {
                nos_flag = 0;
            }
        } else {
            float time_off = -(UMath::Lerp(1.0f, 3.0f, skill));
            if (mNOSTimer < time_off) {
                bool on_road = false;
                if (nav->HasCookieTrail() && nav->IsOccluded()) {
                    on_road = true;
                }
                if (!on_road && speed_gap > 5.0f &&
                    potential_acceleration * 0.5f < accel_gap) {
                    nos_flag = 1;
                }
            }
        }
    }

    if (nos_flag != 0) {
        float t = mNOSTimer + dT;
        mNOSTimer = UMath::Max(t, 0.0f);
    } else {
        float t = mNOSTimer - dT;
        mNOSTimer = UMath::Min(t, 0.0f);
    }

    mIInput->SetControlNOS(nos_flag);
    GetAI()->DoDriving(0);
}

void AIActionRace::OnDebugDraw() {}
