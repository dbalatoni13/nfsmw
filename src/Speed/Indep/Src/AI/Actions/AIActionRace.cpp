#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
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
    this->MakeDebugable(DBG_AI);
    this->mIRigidBody = params->mOwner->GetRigidBody();
    params->mOwner->QueryInterface(&this->mIEngine);
    params->mOwner->QueryInterface(&this->mIInput);
    params->mOwner->QueryInterface(&this->mCheater);
    params->mOwner->QueryInterface(&this->mPerpetrator);
    this->mTurnAroundActive = false;
    this->ComputePotentials();
}

void AIActionRace::OnBehaviorChange(const UCrc32 &mechanic) {
    if (BEHAVIOR_MECHANIC_INPUT == mechanic) {
        this->GetOwner()->QueryInterface(&this->mIInput);
    } else if (BEHAVIOR_MECHANIC_RIGIDBODY == mechanic) {
        this->GetOwner()->QueryInterface(&this->mIRigidBody);
    } else if (BEHAVIOR_MECHANIC_ENGINE == mechanic) {
        this->GetOwner()->QueryInterface(&this->mIEngine);
    }
}

bool AIActionRace::OnTask(HSIMTASK hTask, float dT) {
    if (hTask == this->mResetTask) {
        this->CheckOffPath(dT);
        return true;
    }
    return false;
}

AIAction *AIActionRace::Construct(AIActionParams *params) {
    return new AIActionRace(params, AIACTION_SCORE_LOW);
}

bool AIActionRace::CanBeAttempted(float dT) {
    if (this->mIRigidBody == nullptr) {
        return false;
    }
    if (this->GetAI() == nullptr) {
        return false;
    }
    if (this->GetVehicle() == nullptr) {
        return false;
    }
    if (this->mIEngine == nullptr) {
        return false;
    }
    if (this->mIInput == nullptr) {
        return false;
    }
    WRoadNav test_nav;
    const float dir_weight = 1.0f;
    const bool force_centre_lane = true;
    UMath::Vector3 forwardVector;
    this->mIRigidBody->GetForwardVector(forwardVector);
    test_nav.SetNavType(WRoadNav::kTypeDirection);
    test_nav.InitAtPoint(this->mIRigidBody->GetPosition(), forwardVector, force_centre_lane, dir_weight);
    return test_nav.IsValid();
}

AIActionRace::~AIActionRace() {
    if (this->mResetTask != nullptr) {
        this->RemoveTask(this->mResetTask);
        this->mResetTask = nullptr;
    }
}

void AIActionRace::BeginAction(float dT) {
    const bool force_centre_lane = false;
    WRoadNav *road_nav = this->GetAI()->GetDriveToNav();
    road_nav->SetNavType(WRoadNav::kTypeDirection);
    road_nav->SetLaneType(WRoadNav::kLaneRacing);
    road_nav->SetCookieTrail(true);
    road_nav->ResetCookieTrail();
    this->GetAI()->ResetDriveToNav(SELECT_VALID_LANE);
    this->GetAI()->GetLastSpawnTime();

    IPursuitAI *ipv;
    this->GetAI()->QueryInterface(&ipv);

    this->bIsFleeMode = this->GetAI()->GetGoalName() == "AIGoalFleePursuit";

    // TODO
    if (!this->bIsFleeMode) {
        if (this->GetAI()->GetPursuit() != nullptr) {
            this->bIsPursuitMode = ComparePtr(this->GetAI()->GetTarget()->GetSimable(), this->GetAI()->GetPursuit()->GetTarget()->GetSimable());
        } else {
            this->bIsPursuitMode = false;
        }
    } else {
        this->bIsPursuitMode = true;
    }

    // TODO
    if (this->bIsPursuitMode && ipv != nullptr) {
        this->bDontSeekAhead = ipv->GetSupportGoal() == "AIGoalHeadOnRam";
    } else {
        this->bDontSeekAhead = false;
    }

    this->ComputePotentials();

    this->mLastSpeed = this->GetVehicle()->GetSpeed();
    this->mLastAccel = 0.0f;
    this->mUnstageTimer = 0.0f;
    this->fSpeedLimit = UMath::Max(this->mLastSpeed, 0.0f);
    this->mNOSTimer = 0.0f;

    if (this->bIsFleeMode) {
        this->GetAI()->GetDriveToNav()->SetRaceFilter(false);
    }

    if (this->mResetTask == nullptr) {
        this->mResetTask = this->AddTask(UCrc32("Physics"), 0.25f, 1.0f, Sim::TASK_FRAME_FIXED);
        Sim::ProfileTask(this->mResetTask, "AIActionRace");
    }
}

void AIActionRace::FinishAction(float dT) {
    WRoadNav *road_nav = this->GetAI()->GetDriveToNav();
    if (road_nav != nullptr) {
        road_nav->SetLaneType(WRoadNav::kLaneRacing);
    }
    if (this->mResetTask != nullptr) {
        this->RemoveTask(this->mResetTask);
        this->mResetTask = nullptr;
    }
}

// total size: 0xC
struct GripTor {
    GripTor(IVehicle *vehicle) {
        this->StartGrip = 0.0f;
        this->EndGrip = 0.0f;
        this->Valid = false;

        IVehicleAI *ai;
        if (vehicle->QueryInterface(&ai)) {
            Attrib::Gen::pvehicle pvehicle(vehicle->GetVehicleAttributes());
            Attrib::Gen::chassis chassis(pvehicle.chassis(0), 0, nullptr);
            Attrib::Gen::tires tires(pvehicle.tires(0), 0, nullptr);
            Attrib::Gen::rigidbodyspecs rigidbodyspecs(pvehicle.rigidbodyspecs(), 0, nullptr);

            float gravity = rigidbodyspecs.GRAVITY();
            this->StartGrip = UMath::Min(tires.STATIC_GRIP().Front, tires.STATIC_GRIP().Rear);
            float down = -Physics::Info::AerodynamicDownforce(chassis, ai->GetTopSpeed()) / pvehicle.MASS() + gravity;
            this->EndGrip = this->StartGrip * down / gravity;
            this->Valid = true;
        }
    }

    void operator()(IVehicle *vehicle) {
        GripTor g(vehicle);
        if (g.Valid) {
            this->StartGrip = UMath::Min(g.StartGrip, this->StartGrip);
            this->EndGrip = UMath::Min(g.EndGrip, this->EndGrip);
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
        this->Boost = UMath::Max(Physics::Info::NosBoost(nos, vehicle->GetTunings()) - 1.0f, 0.0f);
        this->Capacity = Physics::Info::NosCapacity(nos, vehicle->GetTunings());
    }

    void operator()(IVehicle *vehicle) {
        NosTor n(vehicle);
        this->Boost = UMath::Min(this->Boost, n.Boost);
        this->Capacity = UMath::Min(this->Capacity, n.Capacity);
    }

    float Boost;    // offset 0x0, size 0x4
    float Capacity; // offset 0x4, size 0x4
};

// total size: 0x4
struct SpeedTor {
    SpeedTor(IVehicle *vehicle) {
        this->Speed = 0.0f;
        if (vehicle == nullptr) {
            return;
        }
        IVehicleAI *ai;
        if (vehicle->QueryInterface(&ai)) {
            this->Speed = ai->GetTopSpeed();
        }
    }

    void operator()(IVehicle *vehicle) {
        SpeedTor s(vehicle);

        if (s.Speed > 0.0f) {
            this->Speed = UMath::Min(s.Speed, this->Speed);
        }
    }

    float Speed; // offset 0x0, size 0x4
};

// total size: 0x10
struct PerformaTor {
    PerformaTor() {
        this->Valid = false;
    }

    void operator()(IVehicle *vehicle) {
        Physics::Info::Performance p;
        if (vehicle->GetPerformance(p)) {
            this->Performance.Maximize(p);
            this->Valid = true;
        }
    }

    Physics::Info::Performance Performance; // offset 0x0, size 0xC
    bool Valid;                             // offset 0xC, size 0x1
};

void AIActionRace::ComputePotentials() {
    if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() != nullptr &&
        GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        float min_perf = 0.0f;

        if (this->mPerpetrator != nullptr) {
            GRacerInfo *racer_info = this->mPerpetrator->GetRacerInfo();
            if (racer_info != nullptr && racer_info->GetGameCharacter() != nullptr) {
                min_perf = racer_info->GetGameCharacter()->MinimumAIPerformance();
            }
        }

        PerformaTor max_player = IVehicle::ForEach(VEHICLE_PLAYERS, PerformaTor());

        if (max_player.Valid) {
            this->mPerformanceBias.Acceleration = UMath::Ramp(min_perf, max_player.Performance.Acceleration, 1.0f);
            this->mPerformanceBias.Handling = UMath::Ramp(min_perf, max_player.Performance.Handling, 1.0f);
            this->mPerformanceBias.TopSpeed = UMath::Ramp(min_perf, max_player.Performance.TopSpeed, 1.0f);
        }
    } else {
        this->mPerformanceBias.Default();
    }

    GripTor my_grip(this->GetVehicle());
    GripTor lowest_grip = IVehicle::ForEach(VEHICLE_PLAYERS, my_grip);

    this->mStartGrip = UMath::Lerp(lowest_grip.StartGrip, my_grip.StartGrip, this->mPerformanceBias.Handling);
    this->mEndGrip = UMath::Lerp(lowest_grip.EndGrip, my_grip.EndGrip, this->mPerformanceBias.Handling);

    NosTor my_nos(this->GetVehicle());
    NosTor lowest_nos = IVehicle::ForEach(VEHICLE_PLAYERS, my_nos);

    this->mNosCapability = UMath::Lerp(lowest_nos.Boost, my_nos.Boost, this->mPerformanceBias.Acceleration);

    if (this->bIsPursuitMode || this->bIsFleeMode) {
        this->mUsableNOS = 1.0f;
        this->mBottleTime = my_nos.Capacity;
    } else {
        if (my_nos.Capacity > UMath::Epsilon && this->mNosCapability > 0.0f) {
            this->mUsableNOS = (lowest_nos.Capacity / my_nos.Capacity) * (this->mNosCapability / my_nos.Boost);
            this->mBottleTime = my_nos.Capacity;
        } else {
            this->mUsableNOS = 0.0f;
            this->mBottleTime = 0.0f;
        }
    }

    SpeedTor my_speed(this->GetVehicle());
    SpeedTor lowest_speed = IVehicle::ForEach(VEHICLE_PLAYERS, my_speed);

    this->mTopSpeed = UMath::Lerp(lowest_speed.Speed, my_speed.Speed, this->mPerformanceBias.TopSpeed);
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
    float result = this->mTopSpeed;
    float maxdesired = -1.0f;

    if (this->mTopSpeed <= 0.0f) {
        result = 0.0f;
    } else if (!is_drag) {
        if (!this->bIsPursuitMode || this->bIsFleeMode) {
            if (this->GetAI()->GetDriveToNav()->HitDeadEnd()) {
                return 0.0f;
            }
            float scale = AICorneringScaleTable.GetValue(skill);
            float start_grip = this->mStartGrip;
            float end_grip = this->mEndGrip;
            float f0 = start_grip;
            float f1 = (UMath::Lerp(start_grip, end_grip, scale) - start_grip) / this->mTopSpeed;
            result = GetSpeedLimit(curvature, f0, f1, this->mTopSpeed);
        } else {
            WRoadNav *road_nav = this->GetAI()->GetDriveToNav();
            UMath::Vector3 myForwardVector;
            this->mIRigidBody->GetForwardVector(myForwardVector);

            UMath::Vector3 navForwardVector = road_nav->GetForwardVector();
            UMath::Normalize(navForwardVector);

            UMath::Vector3 seek_dir;
            AITarget *target = this->GetAI()->GetTarget();

            UMath::Sub(this->mLastFindPosition, target->GetPosition(), seek_dir);
            UMath::Normalize(seek_dir);

            UMath::Vector3 steerDir;
            UMath::Sub(road_nav->GetPosition(), this->mIRigidBody->GetPosition(), steerDir);
            UMath::Vector3 targetSteerDir = steerDir;
            IVehicleAI *targetai;
            if (target->QueryInterface(&targetai)) {
                UMath::Sub(targetai->GetDriveToNav()->GetPosition(), target->GetPosition(), targetSteerDir);
            }
            UMath::Normalize(steerDir);
            UMath::Normalize(targetSteerDir);
            UMath::Vector3 offset_to_target;

            UMath::Vector3 targetPosition = target->GetPosition();
            UMath::Sub(this->mIRigidBody->GetPosition(), targetPosition, offset_to_target);

            float scalar_offset_to_target = UMath::Dot(offset_to_target, seek_dir);
            float forward_near_speed = target->GetSpeed();
            float var_f13 = scalar_offset_to_target > 0.0f ? 100.0f : 200.0f;
            forward_near_speed -= scalar_offset_to_target * 0.01f * KPH2MPS(var_f13);
            float distant_cop_speed = KPH2MPS(this->GetAI()->GetAttributes().MAXIMUM_AI_SPEED());
            if (this->GetAI()->GetPursuit() != nullptr && this->GetAI()->GetPursuit()->GetIsAJerk()) {
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

            float f0 = this->mStartGrip;
            float f1 = (this->mEndGrip - this->mStartGrip) / this->mTopSpeed;
            float speed = !road_nav->HitDeadEnd() ? GetSpeedLimit(curvature, f0, f1, this->mTopSpeed) : 0.0f;

            result = UMath::Min(temp_f31_5, speed);
            maxdesired = temp_f31_5;
        }
    }

    float attrib_scale = this->GetAI()->GetAttributes().TopSpeedMultiplier();

    if (this->bIsPursuitMode && GRaceStatus::Exists() && GRaceStatus::Get().GetActivelyRacing()) {
        attrib_scale += attrib_scale;
    } else {
        if (this->GetAI()->GetPursuit() != nullptr && this->GetAI()->GetPursuit()->GetIsAJerk()) {
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
        this->Speed = speed;
        this->Accel = ai->GetAcceleration(speed);
    }

    void operator()(const IVehicle *vehicle) {
        const IVehicleAI *ai = vehicle->GetAIVehiclePtr();
        if (ai != nullptr) {
            AccelTor a(this->Speed, ai);
            this->Accel = UMath::Min(this->Accel, a.Accel);
        }
    }

    float Speed; // offset 0x0, size 0x4
    float Accel; // offset 0x4, size 0x4
};

float AIActionRace::GetPotentialAcceleration(const float speed, const float skill, bool using_nos, bool is_drag) const {
    AccelTor my_accel(speed, this->GetAI());
    AccelTor lowest_accel = IVehicle::ForEach(VEHICLE_PLAYERS, my_accel);
    float result = UMath::Lerp(lowest_accel.Accel, my_accel.Accel, this->mPerformanceBias.Acceleration);

    float attrib_scale = this->GetAI()->GetAttributes().AccelerationMultiplier();

    if (this->bIsPursuitMode && GRaceStatus::Exists() && GRaceStatus::Get().GetActivelyRacing()) {
        attrib_scale *= 2.0f;
    } else {
        if (this->GetAI()->GetPursuit() != nullptr && this->GetAI()->GetPursuit()->GetIsAJerk()) {
            attrib_scale *= 1.5f;
        }
    }

    const float accel_scale = is_drag ? AiAccelScaleTableDrag.GetValue(skill) : AiAccelScaleTable.GetValue(skill);

    float nos_scale = using_nos ? this->mNosCapability + 1.0f : 1.0f;

    float catchup_scale = 1.0f;
    float gravity_acc = 0.0f;
    if (this->mIRigidBody != nullptr && this->GetVehicle()->GetPhysicsMode() == PHYSICS_MODE_SIMULATED) {
        UMath::Vector3 forward;
        this->mIRigidBody->GetForwardVector(forward);
        const float Gravity = 9.81f;
        const float grade = forward.y;
        gravity_acc = -Gravity * grade;
    }

    if (this->mCheater != nullptr) {
        catchup_scale = AiCatchupAcceleration.GetValue(this->mCheater->GetCatchupCheat());
    }

    result = result * attrib_scale * nos_scale * accel_scale * catchup_scale + gravity_acc;

    return UMath::Max(result, 0.0f);
}

float AIActionRace::GetPotentialNOS(float speed, bool was_on, float skill) const {
    if (speed < 10.0f || speed >= this->fSpeedLimit) {
        return 0.0f;
    }
    if (this->mNosCapability <= 0.0f || this->mUsableNOS <= 0.0f) {
        return 0.0f;
    }
    if (this->mIEngine == nullptr) {
        return 0.0f;
    }
    float useable_nos = this->mUsableNOS * UMath::Lerp(0.33f, 1.0f, skill);
    float off_limit = 1.0f - useable_nos;
    float on_limit; // TODO
    float needed_capacity = (1.0f - off_limit) * UMath::Lerp(0.5f, 0.3f, skill);
    if (was_on) {
        needed_capacity = off_limit;
    }
    float bottle_amount = this->mIEngine->GetNOSCapacity();
    if (bottle_amount <= needed_capacity) {
        return 0.0f;
    }
    return (bottle_amount - needed_capacity) * this->mBottleTime;
}

float aAiNavLookAheadData[2] = {30.0f, 100.0f};
Table AiNavLookAheadTable(aAiNavLookAheadData, 2, 0.0f, 100.0f);

void AIActionRace::CheckOffPath(float dT) {
    WRoadNav *road_nav = this->GetAI()->GetDriveToNav();
    if (road_nav == nullptr) {
        return;
    }
    IRigidBody *rigid_body = this->GetOwner()->GetRigidBody();
    if (rigid_body == nullptr) {
        return;
    }
    UMath::Vector3 car_forward_vector;

    this->GetVehicle()->ComputeHeading(&car_forward_vector);
    float current_speed = rigid_body->GetSpeed();
    bool reset_nav = false;

    if (!this->bIsPursuitMode) {
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
        this->GetAI()->ResetDriveToNav(SELECT_VALID_LANE);
        road_nav->SetNavType(WRoadNav::kTypeDirection);
        float look_ahead = AiNavLookAheadTable.GetValue(current_speed);
        road_nav->IncNavPosition(look_ahead, car_forward_vector, 0.0f);
        road_nav->UpdateOccludedPosition(true);
    }
}

float AIActionRace::UpdateNavPos(float lookAheadDistance, const UMath::Vector3 &direction) {
    if (this->GetAI()->GetDriveToNav()->HitDeadEnd()) {
        return 0.0f;
    }
    WRoadNav *road_nav = this->GetAI()->GetDriveToNav();
    UMath::Vector3 navPos = road_nav->GetPosition();
    UMath::Vector3 carPosition = this->mIRigidBody->GetPosition();
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

void AIActionRace::Update(float dT) {}

void AIActionRace::OnDebugDraw() {}
