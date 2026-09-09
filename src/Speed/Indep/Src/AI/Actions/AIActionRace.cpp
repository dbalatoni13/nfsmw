#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/rigidbodyspecs.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICheater.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

// TODO use these
static const bool bDrawNavTrail = false;        // size: 0x1, Decl: 37
static const bool bDrawSpeedLimits = false;     // size: 0x1, Decl: 38
static const bool bFollowPlayer = false;        // size: 0x1, Decl: 39
static const bool bSeparationBySteering = true; // size: 0x1, Decl: 40

// static const float fCurvatureFudge; // TODO not in carbon
float fDragDifficulty = 0.5f;
// static const float fDragWinningMargin; // TODO not in carbon

static const float fNosSpeedGapOn = 15.0f;        // size: 0x4, Decl: 49
static const float fNosSpeedGapOff = 5.0f;        // size: 0x4, Decl: 50
static const float fNosAccelGapOff = 0.15f;       // size: 0x4, Decl: 51
static const float fNosAccelGapOn = 0.5f;         // size: 0x4, Decl: 52
static const float fMinSpeedForNOS = 10.0f;       // size: 0x4, Decl: 53
static const float fNosTimeOffMin = 20.0f;        // size: 0x4, Decl: 54
static const float fNosTimeOffMax = 4.0f;         // size: 0x4, Decl: 55
static const float fNosTimeOnMin = 2.0f;          // size: 0x4, Decl: 56
static const float fNosTimeOnMax = 4.0f;          // size: 0x4, Decl: 57
static const float fAINosTrapTimeMin = 1.0f;      // size: 0x4, Decl: 58
static const float fAINosTrapTimeMax = 3.0f;      // size: 0x4, Decl: 59
static const float fAINosAvailabilityMin = 0.33f; // size: 0x4, Decl: 60
static const float fAINosAvailabilityMax = 1.0f;  // size: 0x4, Decl: 61

static const float fAINosOnCapacityMin = 0.5f; // size: 0x4, Decl: 63
static const float fAINosOnCapacityMax = 0.3f; // size: 0x4, Decl: 64

static const float Tweak_MinPerformance = 0.0f; // size: 0x4, Decl: 66

static const float Tweak_SpeedTrapSkillBoost = 1.0f; // size: 0x4, Decl: 70

// Decl: 71
float aNosScaleData[2] = {0.25f, 1.0f};
Table AiNosScaleTable(aNosScaleData, NUM_ELEMENTS(aNosScaleData), 0.0f, 1.0f);

// Decl: 73
float aSpeedScaleData[2] = {0.85f, 1.0f};
Table AiSpeedScaleTable(aSpeedScaleData, NUM_ELEMENTS(aSpeedScaleData), 0.0f, 1.0f);

float aSpeedScaleDataDrag[2] = {0.85f, 1.0f};
Table AiSpeedScaleTableDrag(aSpeedScaleDataDrag, NUM_ELEMENTS(aSpeedScaleDataDrag), 0.0f, 1.0f);

// Decl: 83
float aAccelScaleData[2] = {0.65f, 1.0f};
Table AiAccelScaleTable(aAccelScaleData, NUM_ELEMENTS(aAccelScaleData), 0.0f, 1.0f);

float aAccelScaleDataDrag[2] = {0.82f, 1.0f};
Table AiAccelScaleTableDrag(aAccelScaleDataDrag, NUM_ELEMENTS(aAccelScaleDataDrag), 0.0f, 1.0f);

float AiCatchupAccelerationData[4] = {1.0f, 1.33f, 1.66f, 2.0f};
Table AiCatchupAcceleration(AiCatchupAccelerationData, NUM_ELEMENTS(AiCatchupAccelerationData), 0.0f, 1.0f);

static const float Tweak_AIAccelExpMin = 1.5f; // Decl: 94
static const float Tweak_AIAccelExpMax = 2.0f; // Decl: 95

// Decl: 98
float aCorneringScaleData[2] = {0.36f, 0.9f};
Table AICorneringScaleTable(aCorneringScaleData, NUM_ELEMENTS(aCorneringScaleData), 0.0f, 1.0f);

// Decl: 105
float aAiNavLookAheadData[2] = {30.0f, 100.0f};
Table AiNavLookAheadTable(aAiNavLookAheadData, NUM_ELEMENTS(aAiNavLookAheadData), 0.0f, 100.0f);

float aAiDragNavLookAheadData[2] = {30.0f, 100.0f};
Table AiDragNavLookAheadTable(aAiDragNavLookAheadData, NUM_ELEMENTS(aAiDragNavLookAheadData), 0.0f, 100.0f);

// Decl: 113
float aAiSeparationMin[5] = {0.0f, 2.0f, 3.0f, 4.0f, 5.0f};
Table AiSeparationMinTable(aAiSeparationMin, NUM_ELEMENTS(aAiSeparationMin), 0.0f, 100.0f);

// Decl: 116
float aAiSeparationMax[5] = {1.0f, 5.0f, 10.0f, 15.0f, 20.0f};
#ifdef SANE_CODE
Table AiSeparationMaxTable(aAiSeparationMax, NUM_ELEMENTS(aAiSeparationMax), 0.0f, 100.0f);
#else
Table AiSeparationMaxTable(aAiSeparationMin, NUM_ELEMENTS(aAiSeparationMin), 0.0f, 100.0f);
#endif

static const float Tweak_UnstageTime = 2.5f;   // size: 0x4, Decl: 119
static const float Tweak_UnStageSpeed = 24.0f; // size: 0x4, Decl: 120
static const float Tweak_StageSpeedMin = 2.5f; // size: 0x4, Decl: 121
static const float Tweak_StageSpeedMax = 3.0f; // size: 0x4, Decl: 122

static const int NUM_ACCEL_TABLE_ENTRIES = 20; // size: 0x4, Decl: 214

// total size: 0x48
// Decl: 217
class AIActionRace : public AIAction, public Debugable {
  public:
    typedef AIAction Base;

    AIActionRace(AIActionParams *params, float score);
    ~AIActionRace() override;

    static AIAction *Construct(AIActionParams *params);

    // AIAction
    bool CanBeAttempted(float dT) override;

    bool IsFinished() override {
        return false;
    }

    void BeginAction(float dT) override;
    void FinishAction(float dT) override;
    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // Overrides: ITaskable
    bool OnTask(HSIMTASK hTask, float dT) override;

    virtual void OnDebugDraw();

  private:
    void CheckOffPath(float dT);
    float GetPotentialNOS(float speed, bool was_on, float skill) const;
    float GetPotentialAcceleration(const float speed, const float skill, bool using_nos, bool is_drag) const;
    float GetPotentialSpeed(const float curvature, const float skill, bool is_drag) const;
    float UpdateNavPos(float lookAheadDistance, const UMath::Vector3 &direction);
    void ComputePotentials();
    bool CheckSpeedTraps(float speed, float skill, float potential_nos, bool was_nos) const;

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
    Mps mLastAccel;                              // offset 0x88, size 0x4
    Mps mLastSpeed;                              // offset 0x8C, size 0x4
    Physics::Info::Performance mPerformanceBias; // offset 0x90, size 0xC
    float mNosCapability;                        // offset 0x9C, size 0x4
    float mUsableNOS;                            // offset 0xA0, size 0x4
    float mBottleTime;                           // offset 0xA4, size 0x4
    float mStartGrip;                            // offset 0xA8, size 0x4
    float mEndGrip;                              // offset 0xAC, size 0x4
    Mps mTopSpeed;                               // offset 0xB0, size 0x4
    Mps mUnstageTimer;                           // offset 0xB4, size 0x4
    Seconds mNOSTimer;                           // offset 0xB8, size 0x4
    HSIMTASK mResetTask;                         // offset 0xBC, size 0x4
};

BIND_AIACTION_FACTORY(AIActionRace);

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

AIAction *AIActionRace::Construct(AIActionParams *params) {
    return new AIActionRace(params, AIACTION_SCORE_LOW);
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
    Object::OnTask(hTask, dT);
    if (hTask == this->mResetTask) {
        this->CheckOffPath(dT);
        return true;
    }
    return false;
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

    this->bIsFleeMode = this->GetAI()->GetGoalName() == UCrc32("AIGoalFleePursuit");

    this->bIsPursuitMode =
        this->bIsFleeMode || (this->GetAI()->GetPursuit() != nullptr &&
                              ComparePtr(this->GetAI()->GetTarget()->GetSimable(), this->GetAI()->GetPursuit()->GetTarget()->GetSimable()));

    this->bDontSeekAhead = this->bIsPursuitMode && ipv != nullptr && ipv->GetSupportGoal() == UCrc32("AIGoalHeadOnRam");

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
        this->mResetTask = this->AddTask("Physics", 0.25f, 1.0f, Sim::TASK_FRAME_FIXED);
        Sim::ProfileTask(this->mResetTask, "AIActionRace");
    }
}

// Decl: 507
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
// Decl: 530
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
// Decl: 575
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
// Decl: 593
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
// Decl: 615
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

// Decl: 563
void AIActionRace::ComputePotentials() {
    if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() != nullptr &&
        GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        float min_perf = 0.0f;

        if (this->mPerpetrator != nullptr) {
            GRacerInfo *racer_info = this->mPerpetrator->GetRacerInfo();
            if (racer_info != nullptr) {
                GCharacter *character = racer_info->GetGameCharacter();
                if (character != nullptr) {
                    min_perf = character->MinimumAIPerformance();
                }
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

static const float Tweak_TestPerfBias = 0.0f; // size: 0x4, Decl: 634

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
            forward_near_speed -= ((scalar_offset_to_target > 0.0f ? KPH2MPS(100.0f) : KPH2MPS(200.0f)) * 0.01f) * scalar_offset_to_target;

            float distant_cop_speed = KPH2MPS(this->GetAI()->GetAttributes().MAXIMUM_AI_SPEED());
            if (this->GetAI()->GetPursuit() != nullptr && this->GetAI()->GetPursuit()->GetIsAJerk()) {
                distant_cop_speed *= 1.1f;
            }

            forward_near_speed = bClamp(forward_near_speed, KPH2MPS(10.0f), distant_cop_speed);

            float reverse_near_speed =
                scalar_offset_to_target > 0.0f ? -target->GetSpeed() + scalar_offset_to_target * (KPH2MPS(50.0f) * 0.01f) : distant_cop_speed;
            reverse_near_speed = bClamp(reverse_near_speed, KPH2MPS(40.0f), distant_cop_speed);

            float direction_scale = bClamp(UMath::Dot(myForwardVector, seek_dir) + 0.5f, 0.0f, 1.0f);
            float near_speed = direction_scale * (forward_near_speed - reverse_near_speed) + reverse_near_speed;

            UMath::Vector3 side_offset;
            UMath::ScaleAdd(seek_dir, -scalar_offset_to_target, offset_to_target, side_offset);

            float side_offset_to_target = UMath::Length(side_offset) * 2.5f;
            if (scalar_offset_to_target > 0.0f) {
                scalar_offset_to_target *= 0.5f;
            }

            float apparent_distance_to_target =
                UMath::Sqrt((scalar_offset_to_target * scalar_offset_to_target) + (side_offset_to_target * side_offset_to_target));

            float near_scale = bClamp(1.0f - (apparent_distance_to_target - 150.0f) / 150.0f, 0.0f, 1.0f);
            near_scale *= bClamp(UMath::Abs(UMath::Dot(steerDir, targetSteerDir)) + 0.2f, 0.0f, 1.0f);

            float max_cop_speed = (near_scale * near_speed) + (1.0f - near_scale) * distant_cop_speed;
            max_cop_speed = bClamp(max_cop_speed, 0.0f, distant_cop_speed);

            const float kZeroSpeedMaxLateralGForce = this->mStartGrip;
            const float kTopSpeedMaxLateralGForce = this->mEndGrip;
            float f0 = kZeroSpeedMaxLateralGForce;
            float f1 = (kTopSpeedMaxLateralGForce - kZeroSpeedMaxLateralGForce) / this->mTopSpeed;
            float speed = !road_nav->HitDeadEnd() ? GetSpeedLimit(curvature, f0, f1, this->mTopSpeed) : 0.0f;

            result = UMath::Min(max_cop_speed, speed);
            maxdesired = max_cop_speed;
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

// STRIPPED
// Decl: 1057
bool LineIntersection(const bVector2 &a, const bVector2 &b, const bVector2 &c, const bVector2 &d, bVector2 *intersection) {
    return false;
}

// STRIPPED
// Decl: 1068
float Curvature(const bVector2 *p0, const bVector2 *p1, const bVector2 *p2, bVector2 *centre) {
    return 0.0f;
}

// total size: 0x8
// Decl: 1095
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

static const float fAINavOutOfBounds = 2.0f; // size: 0x4, Decl: 1333

static const float Tweak_DotToTrapForNOS = 0.5f; // size: 0x4, Decl: 1463

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
    if (speed < fMinSpeedForNOS || speed >= this->fSpeedLimit) {
        return 0.0f;
    }
    if (this->mNosCapability <= 0.0f || this->mUsableNOS <= 0.0f) {
        return 0.0f;
    }
    if (this->mIEngine == nullptr) {
        return 0.0f;
    }
    float useable_nos = this->mUsableNOS * UMath::Lerp(fAINosAvailabilityMin, fAINosAvailabilityMax, skill);
    float off_limit = 1.0f - useable_nos;
    float on_limit = 1.0f - off_limit;
    float needed_capacity = on_limit * UMath::Lerp(fAINosOnCapacityMin, fAINosOnCapacityMax, skill);
    if (was_on) {
        needed_capacity = off_limit;
    }
    float bottle_amount = this->mIEngine->GetNOSCapacity();
    if (bottle_amount <= needed_capacity) {
        return 0.0f;
    }
    return (bottle_amount - needed_capacity) * this->mBottleTime;
}

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
        if (old_out_of_bounds > fAINavOutOfBounds) {
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

bool AIActionRace::CheckSpeedTraps(float speed, float skill, float potential_nos, bool was_nos) const {
    if (this->mPerpetrator == nullptr) {
        return false;
    }
    if (!GRaceStatus::Get().GetActivelyRacing()) {
        return false;
    }
    if (!GRaceStatus::IsSpeedTrapRace()) {
        return false;
    }

    GRacerInfo *info = this->mPerpetrator->GetRacerInfo();

    if (info == nullptr || speed < fMinSpeedForNOS) {
        return false;
    }
    if (!was_nos && speed > this->fSpeedLimit - 5.0f) {
        return false;
    }

    const UMath::Vector3 &my_position = this->mIRigidBody->GetPosition();

    UMath::Vector3 my_direction;
    this->mIRigidBody->GetForwardVector(my_direction);

    int num_speed_traps = GRaceStatus::Get().GetNumRaceSpeedTraps();

    for (int i = 0; i < num_speed_traps; i++) {
        GTrigger *trap = GRaceStatus::Get().GetRaceSpeedTrap(i);

        if (trap->IsEnabled()) {
            UMath::Vector3 position;
            trap->GetPosition(position);

            float desired_time_ahead = UMath::Lerp(fAINosTrapTimeMin, fAINosTrapTimeMax, skill);
            float look_ahead = speed * desired_time_ahead;

            if (UMath::DistanceSquare(position, my_position) < look_ahead * look_ahead) {
                UMath::Vector3 relative_position;
                UMath::Sub(position, my_position, relative_position);

                float distance = UMath::Normalize(relative_position);
                float time_ahead = distance / speed;

                if ((time_ahead < potential_nos || was_nos) && UMath::Dot(my_direction, relative_position) > Tweak_DotToTrapForNOS) {
                    return true;
                }
            }
        }
    }
    return false;
}

void AIActionRace::Update(float dT) {
    bool need_to_stop = this->GetVehicle()->IsDestroyed();
    bool drag_racing = this->GetVehicle()->GetDriverStyle() == STYLE_DRAG;
    bool is_staging = this->GetVehicle()->IsStaging();
    WRoadNav *road_nav = this->GetAI()->GetDriveToNav();
    UMath::Vector3 desired_direction = UMath::Vector3::kZero;
    const UMath::Vector3 &car_position = this->mIRigidBody->GetPosition();

    if (false) {
        IRigidBody *player_rigid_body = this->mIRigidBody;
        road_nav->SetLaneType(WRoadNav::kLaneRacing);

        UMath::Vector3 player_position;
        player_position = UVector3(player_position) - car_position;
        WRoadNav *road_nav = this->GetAI()->GetDriveToNav();

        {
            UMath::Vector3 direction;
        }
    }

    if (this->bIsPursuitMode) {
        road_nav->SetLaneType(WRoadNav::kLaneRacing);

        if (!road_nav->FindingPath()) {
            IVehicleAI *targetai;
            AITarget *target = this->GetAI()->GetTarget();
            target->QueryInterface(&targetai);
            UMath::Vector3 findPosition;

            if (this->bIsFleeMode) {
                UMath::Vector3 fleecenter;
                UMath::Vector3 fleeforward;
                IPlayer *iplayer = IPlayer::First(PLAYER_LOCAL);
                UMath::Vector3 offset;
                float offlen;

                if (iplayer != nullptr) {
                    fleecenter = iplayer->GetSimable()->GetPosition();
                    iplayer->GetSimable()->GetLinearVelocity(fleeforward);
                    offlen = UMath::Normalize(fleeforward);

                    if (offlen < 0.00001f) {
                        iplayer->GetSimable()->GetRigidBody()->GetForwardVector(fleeforward);
                    }
                } else {
                    fleecenter = target->GetPosition();
                    fleeforward = target->GetLinearVelocity();
                    offlen = UMath::Normalize(fleeforward);

                    if (offlen < 0.00001f) {
                        target->GetSimable()->GetRigidBody()->GetForwardVector(fleeforward);
                    }
                }

                UMath::Sub(fleecenter, this->mIRigidBody->GetPosition(), offset);

                if ((offlen = UMath::Length(offset)) > 50.0f) {
                    UMath::Scale(offset, 1.0f / offlen, fleeforward);
                }

                UMath::ScaleAdd(fleeforward, -500.0f, fleecenter, findPosition);

                if (UMath::Distance(this->mLastFindPosition, findPosition) < 60.0f && UMath::Distance(fleecenter, this->mLastFindPosition) > 450.0f) {
                    findPosition = this->mLastFindPosition;
                }
            } else {
                if (this->bDontSeekAhead) {
                    findPosition = target->GetPosition();
                } else {
                    findPosition = targetai->GetSeekAheadPosition();
                }
            }

            this->mLastFindPosition = findPosition;
            road_nav->FindPath(&findPosition, nullptr, nullptr);
        }

        desired_direction = UVector3(this->mLastFindPosition) - car_position;
    } else {
        if (drag_racing) {
            road_nav->SetLaneType(WRoadNav::kLaneDrag);
        } else {
            road_nav->SetLaneType(WRoadNav::kLaneRacing);
        }

        if (GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing &&
            GRaceStatus::Get().GetRaceParameters() != nullptr && GRaceStatus::Get().GetRaceParameters()->HasFinishLine()) {
            AITarget *target = this->GetAI()->GetTarget();

            if (target->IsValid() && !GRaceStatus::Get().GetRaceRouteError()) {
                const UMath::Vector3 &target_position = target->GetPosition();
                UMath::Vector3 car_to_target = UVector3(target_position) - car_position;
                WRoadNav *road_nav = this->GetAI()->GetDriveToNav();

                if (road_nav->GetNavType() != WRoadNav::kTypePath && !road_nav->FindingPath()) {
                    bool find_path = true;

                    if (UMath::DistanceSquare(target_position, this->mLastFindPosition) < 1.0f) {
                        if (road_nav->IsGoalInCookieTrail()) {
                            find_path = false;
                        } else {
                            UMath::LengthSquare(car_to_target);
                        }
                    }

                    if (find_path) {
                        UMath::Vector3 target_direction = target->GetDirection();
                        road_nav->FindPath(&target_position, &target_direction, nullptr);
                        this->mLastFindPosition = target_position;
                    }
                }
            } else {
                need_to_stop = true;
            }
        }
    }

    Table &nav_look_ahead_table = drag_racing ? AiDragNavLookAheadTable : AiNavLookAheadTable;
    float look_ahead_distance = nav_look_ahead_table.GetValue(this->fSpeedLimit);
    float distance_to_nav = this->UpdateNavPos(look_ahead_distance, desired_direction);
    this->GetAI()->SetAvoidableRadius(look_ahead_distance);

    WRoadNav *nav = this->GetAI()->GetDriveToNav();
    const UMath::Vector3 &nav_position = nav->GetPosition();
    const UMath::Vector3 &apex_position = nav->GetApexPosition();
    const UMath::Vector3 &occluded_position = nav->GetOccludedPosition();

    float curvature = nav->CookieTrailCurvature(car_position, this->mIRigidBody->GetLinearVelocity());

    const float skill = this->GetAI()->GetSkill();
    this->fPotentialSpeed = this->GetPotentialSpeed(curvature, skill, drag_racing);

    const float actual_speed = this->GetVehicle()->GetSpeed();
    const float nos_capacity = this->mIEngine->GetNOSCapacity();
    const bool was_nos = this->mIInput->GetControls().fNOS && nos_capacity > 0.0f;

    const float potential_acceleration = this->GetPotentialAcceleration(UMath::Max(actual_speed, this->fSpeedLimit), skill, was_nos, drag_racing);

    const float actual_acceleration = (actual_speed - this->mLastSpeed) / dT;
    this->mLastSpeed = actual_speed;
    this->mLastAccel = actual_acceleration;

    if (actual_acceleration < 0.0f) {
        if (this->fSpeedLimit<this->fPotentialSpeed &&this->fSpeedLimit> actual_speed) {
            float delta_acc = UMath::Min(potential_acceleration + actual_acceleration, 0.0f);
            this->fSpeedLimit += delta_acc * dT;
        }
    }

    if (this->fSpeedLimit < this->fPotentialSpeed) {
        float t = UMath::Ramp(this->fSpeedLimit, 0.0f, this->fPotentialSpeed);
        float exp = UMath::Lerp(1.5f, 2.0f, skill);
        float delta_acc = UMath::Pow(t, exp);
        delta_acc = UMath::Clamp(potential_acceleration - actual_acceleration * delta_acc, 0.0f, potential_acceleration);
        this->fSpeedLimit += delta_acc * dT;
    }

    if (is_staging) {
        this->mUnstageTimer = Tweak_UnstageTime;
    } else if (this->mUnstageTimer > 0.0f && actual_speed > Tweak_UnStageSpeed) {
        this->mUnstageTimer -= dT;
        this->mUnstageTimer = UMath::Max(this->mUnstageTimer, 0.0f);
    }

    this->fSpeedLimit = UMath::Clamp(this->fSpeedLimit, 0.0f, this->fPotentialSpeed);

    const float delta_acceleration = potential_acceleration - actual_acceleration;

    UMath::Vector3 drive_target = occluded_position;

    if (is_staging) {
        this->fSpeedLimit = UMath::Lerp(Tweak_StageSpeedMin, Tweak_StageSpeedMax, skill) * MPH2MPS(1.0f);
        drive_target = nav_position;
    }

    float drive_speed = need_to_stop ? 0.0f : this->fSpeedLimit;

    this->mTurnAroundActive = false;

    {
        const float kTurnAroundSpeed = KPH2MPS(40.0f);

        if (this->mIRigidBody->GetSpeed() > kTurnAroundSpeed && this->bIsPursuitMode) {
            UMath::Vector3 race_steer;
            UMath::Sub(drive_target, car_position, race_steer);
            float dirdot = UMath::Dot(this->mIRigidBody->GetLinearVelocity(), race_steer);

            if (dirdot < (this->mIRigidBody->GetSpeed() * -0.3f) * UMath::Length(race_steer)) {
                drive_target = this->GetAI()->GetFutureRoad()->GetPosition();
                drive_speed = 0.0f;
                this->mTurnAroundActive = true;
            }
        }

        this->GetAI()->SetDriveTarget(drive_target);
        this->GetAI()->SetDriveSpeed(drive_speed);
    }
    this->fDriveSpeed = drive_speed;

    bool want_nos = false;
    const float potential_nos = this->GetPotentialNOS(actual_speed, was_nos, skill);
    const bool can_nos = !this->mTurnAroundActive && !need_to_stop && !is_staging && potential_nos > 0.0f;

    if (can_nos && !this->GetOwner()->IsPlayer()) {
        float skill_scale = AiNosScaleTable.GetValue(skill);

        if (this->CheckSpeedTraps(actual_speed, skill, potential_nos, was_nos)) {
            skill_scale += Tweak_SpeedTrapSkillBoost;
        }

        float speed_gap = skill_scale * (this->fPotentialSpeed - actual_speed);
        float accel_gap = skill_scale * (potential_acceleration - actual_acceleration);

        if (!was_nos) {
            float time_off = UMath::Lerp(fNosTimeOffMin, fNosTimeOffMax, skill);

            if (this->mNOSTimer < -time_off && !road_nav->IsOccluded() && speed_gap > fNosSpeedGapOn &&
                accel_gap > potential_acceleration * fNosAccelGapOn) {
                want_nos = true;
            }
        } else {
            want_nos = true;
            float time_on = UMath::Lerp(fNosTimeOnMin, fNosTimeOnMax, skill);

            if (this->mNOSTimer > time_on && (speed_gap < fNosSpeedGapOff || accel_gap < potential_acceleration * fNosAccelGapOff)) {
                want_nos = false;
            }
        }
    }

    if (want_nos) {
        this->mNOSTimer = UMath::Max(this->mNOSTimer + dT, 0.0f);
    } else {
        this->mNOSTimer = UMath::Min(this->mNOSTimer - dT, 0.0f);
    }

    this->mIInput->SetControlNOS(want_nos);
    this->GetAI()->DoDriving(7);
}

void AIActionRace::OnDebugDraw() {}
