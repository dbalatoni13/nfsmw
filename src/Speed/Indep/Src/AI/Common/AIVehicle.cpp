#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AIGoal.h"
#include "Speed/Indep/Src/AI/AIMath.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicleHelicopter.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aivehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/collisionreactions.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Generated/Events/EEnableAIPhysics.hpp"
#include "Speed/Indep/Src/Generated/Messages/MForcePursuitStart.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICause.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICheater.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WRoadElem.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#define AI_DEFAULT_AVOID_RADIUS 20.f // Decl: 58
#define AI_STAGGERED 1               // Decl: 59

static const bool ForcePerpHidden = false; // Decl: 72

static const float fHumanNavTooFar = 70.0f; // Decl: 74

static const float fHumanNavOutOfBounds = 2.0f; // Decl: 75

static const bool bAiGlue = true;          // size: Decl: 78 // TODO use
static const float fGlueInterval = 1.0f;   // size: Decl: 79
static const int nGlueIntegralTerms = 10;  // size: Decl: 80
static const int nGlueDerivativeTerms = 5; // size: Decl: 81

float aHumanNavLookAheadData[2] = {50.0f, 60.0f};                                                         // Decl: 83
Table HumanNavLookAheadTable(aHumanNavLookAheadData, NUM_ELEMENTS(aHumanNavLookAheadData), 0.0f, 100.0f); // Decl: 84

float aHumanDragNavLookAheadData[2] = {8.0f, 40.0f};
Table HumanDragNavLookAheadTable(aHumanDragNavLookAheadData, 2, 0.0f, 100.0f);

static const float Tweak_CTS_NOSRecharge = 0.0f;         // Decl: 93
static const float Tweak_CTS_GameBreakerRecharge = 0.0f; // Decl: 94
static const float Tweak_MaxAICausalityTime = 2.0f;      // Decl: 95
static const float Tweak_MinAICausalityHitSpeed = 4.0f;  // Decl: 96
static const float Tweak_MaxAICausalityHitSpeed = 30.0f; // Decl: 97

static const bool Tweak_PrintCausalityChains = false;          // Decl: 99
static const int Tweak_CostToStateHittingCop_Explosion = 2000; // Decl: 100
static const int Tweak_CostToStateHittingCop_Direct = 2000;    // Decl: 101
static const int Tweak_CostToStateHittingCop_Indirect = 500;   // Decl: 102

const char *GetCaffeineLayerName(int driver_class) {
    switch (driver_class) {
        case DRIVER_COP:
            return "CopCars";
        case DRIVER_HUMAN:
            return "PlayerCars";
        case DRIVER_RACER:
            return "RacingCars";
        default:
            return "TrafficCars";
    }
}

class AIVehicleEmpty : public AIVehicle {
  public:
    static Behavior *Construct(const BehaviorParams &bp);

    AIVehicleEmpty(const BehaviorParams &bp);

    // Overrides: AIVehicle
    void Update(float dT) override {
        this->AIVehicle::Update(dT);
    }

    // Overrides: AIVehicle
    void OnDebugDraw() override {}
};

BIND_BEHAVIOR_FACTORY(AIVehicleEmpty);

AIVehicleEmpty::AIVehicleEmpty(const BehaviorParams &bp) : AIVehicle(bp, 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE) {}

Behavior *AIVehicleEmpty::Construct(const BehaviorParams &bp) {
    return new AIVehicleEmpty(bp);
}

bool bToggleAiControl = false;

// total size: 0x80C
class AIVehicleHuman : public AIVehicleRacecar, public IHumanAI {
  public:
    AIVehicleHuman(const BehaviorParams &bp);

    static Behavior *Construct(const BehaviorParams &bp);

    // Overrides: IUnknown
    ~AIVehicleHuman() override;

    void UpdateWrongWay();

    // Overrides: IHumanAI
    void SetAiControl(bool ai_control) override;

    bool IsDragRacing();

    bool IsDragSteering();

    // Overrides: IHumanAI
    void ChangeDragLanes(bool left) override;

    // Overrides: AIVehicle
    void OnDebugDraw() override;

    // Overrides: AIVehicle
    void Update(float dT) override;

    // Overrides: IHumanAI
    // Decl: 149
    bool IsPlayerSteering() override {
        return !this->bAiControl && !this->IsDragSteering();
    }

    // Overrides: IHumanAI
    bool GetAiControl() override {
        return this->bAiControl;
    }

    // Overrides: IHumanAI
    void SetWorldMoment(const UMath::Vector3 &position, float radius) override {
        this->vMomentPosition = position;
        this->fMomentRadius = radius;
    }

    // Overrides: IHumanAI
    const UMath::Vector3 &GetWorldMomentPosition() override {
        return this->vMomentPosition;
    }

    // Overrides: IHumanAI
    float GetWorldMomentRadius() override {
        return this->fMomentRadius;
    }

    // Overrides: IHumanAI
    void ClearWorldMoment() override {
        this->fMomentRadius = 0.0f;
    }

    // Overrides: IVehicleAI
    float GetSkill() const override {
        return 1.0f;
    }

    // Overrides: IHumanAI
    bool IsFacingWrongWay() const override {
        return this->mWrongWay;
    }

    // Overrides: ICheater
    float GetCatchupCheat() const override {
        return 0.0f;
    }

  private:
    bool bAiControl;                // offset 0x7F4, size 0x1
    UMath::Vector3 vMomentPosition; // offset 0x7F8, size 0xC
    float fMomentRadius;            // offset 0x804, size 0x4
    bool mWrongWay;                 // offset 0x808, size 0x1
};

BIND_BEHAVIOR_FACTORY(AIVehicleHuman);

AIVehicleHuman::AIVehicleHuman(const BehaviorParams &bp) : AIVehicleRacecar(bp), IHumanAI(bp.fowner) {
    this->MakeDebugable(DBG_AI);
    this->fMomentRadius = 0.0f;
    this->bAiControl = false;
    this->mWrongWay = false;
}

Behavior *AIVehicleHuman::Construct(const BehaviorParams &bp) {
    AIVehicleHuman *aivh = new AIVehicleHuman(bp);
    return aivh;
}

AIVehicleHuman::~AIVehicleHuman() {
    int player_num = 0;
    IPerpetrator *ip;
    if (this->GetSimable()->QueryInterface(&ip)) {
        float Heat = ip->GetHeat();
        if (Heat > 5.0f) {
            Heat = 5.0f;
        }
        unsigned int player_car = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(player_num);
        if (FEDatabase->IsCareerMode()) {
            UserProfile *prof = FEDatabase->CurrentUserProfiles[player_num];
            CareerSettings *career = FEDatabase->GetCareerSettings();
            career->GetCurrentCar();
            prof->GetCareer();
            if ((uintptr_t)prof != 0xffffff1c) {
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(player_num);
                if (stable != nullptr) {
                    FECarRecord *fe_car = stable->GetCarByIndex(career->GetCurrentCar());
                    if (fe_car != nullptr) {
                        FECareerRecord *fe_career = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
                        if (fe_career != nullptr) {
                            fe_career->SetVehicleHeat(Heat);
                        }
                    }
                }
            }
        }
    }
}

void AIVehicleHuman::UpdateWrongWay() {
    this->mWrongWay = false;
    IVehicle *vehicle = this->GetVehicle();
    if (vehicle->GetPhysicsMode() != PHYSICS_MODE_SIMULATED || vehicle->IsAnimating() || vehicle->IsStaging()) {
        return;
    }
    if (!GRaceStatus::Exists()) {
        return;
    }
    if (GRaceStatus::Get().GetPlayMode() != GRaceStatus::kPlayMode_Racing || !GRaceStatus::Get().GetActivelyRacing()) {
        return;
    }
    WRoadNav *road_nav = this->GetDriveToNav();
    if (road_nav == nullptr) {
        return;
    }
    UMath::Vector3 drive_dir;
    vehicle->ComputeHeading(&drive_dir);

    WRoadNav nav;
    nav.SetNavType(WRoadNav::kTypeDirection);
    nav.SetPathType(WRoadNav::kPathPlayer);
    nav.SetLaneType(WRoadNav::kLaneRacing);
    nav.SetRaceFilter(true);
    nav.SetTrafficFilter(false);
    nav.SetDecisionFilter(false);
    nav.InitAtPoint(vehicle->GetPosition(), drive_dir, false, 1.0f);
    if (nav.IsValid()) {
        const WRoadSegment *seg = nav.GetSegment();
        if (seg != nullptr && seg->IsInRace()) {
            UMath::Vector3 fwd = nav.GetForwardVector();
            if (UMath::Dot(fwd, drive_dir) < 0.0f) {
                this->mWrongWay = true;
            }
        }
    }
}

void AIVehicleHuman::SetAiControl(bool ai_control) {
    if (this->bAiControl != ai_control) {
        this->ClearGoal();
        this->SetGoal(UCrc32("AIGoalRacer"));
        this->bAiControl = ai_control;
        new EEnableAIPhysics(reinterpret_cast<uintptr_t>(this->GetOwner()->GetInstanceHandle()), this->GetVehicle()->GetSpeed(), ai_control ? 1 : 0);
    }
}

bool AIVehicleHuman::IsDragRacing() {
    return this->GetVehicle()->GetDriverStyle() == STYLE_DRAG;
}

bool AIVehicleHuman::IsDragSteering() {
    if (!this->IsDragRacing()) {
        return false;
    }
    if (this->GetVehicle()->GetSpeed() < 1.0f) {
        return false;
    }
    IPlayer *player = this->GetOwner()->GetPlayer();
    if (player != nullptr && player->InGameBreaker()) {
        return false;
    }
    return this->mWrongWay == false;
}

void AIVehicleHuman::ChangeDragLanes(bool left) {
    if (!this->IsDragSteering()) {
        return;
    }
    WRoadNav *road_nav = this->GetDriveToNav();
    if (road_nav == nullptr) {
        return;
    }
    road_nav->ChangeDragLanes(left ? -1 : 1);
}

void AIVehicleHuman::OnDebugDraw() {}

void AIVehicleHuman::Update(float dT) {
    ProfileNode profile_node("TODO", 0);

    if (bToggleAiControl) {
        this->SetAiControl(!this->GetAiControl());
        bToggleAiControl = false;
    }

    this->UpdateWrongWay();

    if (this->GetAiControl()) {
        InputControls controls = this->GetInput()->GetControls();
        AIVehicleRacecar::Update(dT);
        return;
    }

    AIPerpVehicle::Update(dT);

    UMath::Vector3 car_forward_vector;
    this->GetVehicle()->ComputeHeading(&car_forward_vector);

    IRigidBody *rigid_body = this->GetSimable()->GetRigidBody();
    float current_speed = rigid_body->GetSpeed();
    Table &nav_look_ahead_table = this->IsDragRacing() ? HumanDragNavLookAheadTable : HumanNavLookAheadTable;

    bool reset_nav = false;
    WRoadNav *road_nav = this->GetDriveToNav();
    if (road_nav->GetNavType() == WRoadNav::kTypeNone) {
        reset_nav = true;
    } else {
        float look_ahead_distance = nav_look_ahead_table.GetValue(current_speed);
        float distance_to_nav = UMath::Distance(road_nav->GetPosition(), rigid_body->GetPosition());

        if (distance_to_nav < look_ahead_distance) {
            if (road_nav->HitDeadEnd() == 0) {
                road_nav->IncNavPosition(look_ahead_distance - distance_to_nav, car_forward_vector, look_ahead_distance);
            }
        } else if (distance_to_nav > fHumanNavTooFar) {
            reset_nav = true;
        }

        road_nav->UpdateOccludedPosition(!this->IsDragRacing());
    }

    if (!reset_nav) {
        float old_out_of_bounds = road_nav->GetOutOfBounds();
        if (old_out_of_bounds > fHumanNavOutOfBounds) {
            WRoadNavWithCookies nav;
            nav.SetNavType(WRoadNav::kTypeDirection);
            nav.SetPathType(road_nav->GetPathType());
            nav.SetLaneType(road_nav->GetLaneType());
            nav.SetRaceFilter(road_nav->GetRaceFilter());
            nav.SetTrafficFilter(road_nav->GetTrafficFilter());
            nav.SetDecisionFilter(road_nav->GetDecisionFilter());

            nav.InitAtPoint(rigid_body->GetPosition(), car_forward_vector, false, 1.0f);

            if (nav.IsValid()) {
                if (!nav.GetSegment()->IsDecision()) {
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
    }

    if (reset_nav) {
        this->ResetDriveToNav(SELECT_VALID_LANE);
        road_nav->SetNavType(WRoadNav::kTypeDirection);
        float look_ahead = nav_look_ahead_table.GetValue(current_speed);
        road_nav->IncNavPosition(look_ahead, car_forward_vector, 0.0f);
        road_nav->UpdateOccludedPosition(true);
    }

    if (GRaceStatus::Exists()) {
        if ((GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming || GRaceStatus::Get().GetActivelyRacing()) &&
            road_nav->GetNavType() != WRoadNav::kTypePath && !road_nav->FindingPath() && road_nav->IsValid()) {
            AITarget *target = this->GetTarget();
            if (target->IsValid()) {
                road_nav->FindPath(&target->GetPosition(), &target->GetDirection(), nullptr);
            }
        }
    }

    if (this->IsDragRacing()) {
        road_nav->SetLaneType(WRoadNav::kLaneDrag);
        if (this->IsDragSteering()) {
            this->DoSteering();
            this->SetDriveTarget(road_nav->GetPosition());
        } else {
            road_nav->DetermineDragLane();
        }
    } else {
        road_nav->SetLaneType(WRoadNav::kLaneRacing);
    }

    if (this->GetPursuit() != nullptr && this->GetPursuit()->IsPerpInSight() && this->GetPursuit()->IsPlayerPursuit()) {
        if (!this->IsOnLegalRoad()) {
            if (this->GetPursuit()->GetMinDistanceToTarget() < 25.0f) {
                GInfractionManager::Get().ReportDrivingOffRoadWay();
            }
        }
    }
}

BIND_BEHAVIOR_FACTORY(AIVehicle);

Behavior *AIVehicle::Construct(const BehaviorParams &bp) {
    return new AIVehicle(bp, 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE);
}

AIVehicle::AIVehicle(const BehaviorParams &bp, float update_rate, float stagger, Sim::TaskMode taskmode)
    : VehicleBehavior(bp, 0),                                   //
      IVehicleAI(bp.fowner),                                    //
      AIAvoidable(this->GetOwner()),                            //
      mDriveSpeed(0.0f),                                        //
      mThinkTask(nullptr),                                      //
      mCurrentGoal(nullptr),                                    //
      mPursuit(nullptr),                                        //
      mRoadBlock(nullptr),                                      //
      mDriveFlags(0),                                           //
      mGoalName(UCrc32::kNull),                                 //
      mDampedAngularVel(5.6f, 3.0f),                            //
      mDampedAngle(5.6f, 3.0f),                                 //
      mAvoidableRadius(AI_DEFAULT_AVOID_RADIUS),                //
      mRoadUpdateTimer(Sim::GetTime() - 100.0f),                //
      mRoadIncrementTimer(Sim::GetTime() - 100.0f),             //
      mSeekAheadTimer(Sim::GetTime() - 100.0f),                 //
      mSeekAheadPosition(UMath::Vector3Make(0.0f, 0.0f, 0.0f)), //
      mLastFutureSegment(-1),                                   //
      mTopSpeed(0.0f) {
    this->EnableProfile("AIVehicle");
    this->GetOwner()->QueryInterface(&this->mCollisionBody);

    const Attrib::Gen::pvehicle &pvehicle = this->GetVehicle()->GetVehicleAttributes();
    bMemSet(this->mAccelData, 0, sizeof(this->mAccelData));
    Physics::Info::ComputeAccelerationTable(pvehicle, this->mTopSpeed, this->mAccelData, sizeof(this->mAccelData) / sizeof(this->mAccelData[0]));

    this->mAttributes = new Attrib::Gen::aivehicle(pvehicle.aivehicle(), 0, nullptr);
    this->mDriveToNav = new WRoadNav();
    this->mDrivableToNav = false;

    ISimable *isimable = this->GetSimable();
    IRigidBody *irigidbody = isimable->GetRigidBody();
    const UMath::Vector3 &rigidBodyPos = irigidbody->GetPosition();
    this->SetDriveTarget(rigidBodyPos);

    bool cookie_trail = false;
    bool decision_filter = false;
    IVehicle *v = this->GetVehicle();
    WRoadNav::EPathType path_type = WRoadNav::kPathNone;

    if (v->GetVehicleClass() == VehicleClass::CHOPPER) {
        path_type = WRoadNav::kPathChopper;
    } else {
        switch (this->GetVehicle()->GetDriverClass()) {
            case DRIVER_COP:
                path_type = WRoadNav::kPathCop;
                cookie_trail = true;
                break;

            case DRIVER_HUMAN:
                path_type = WRoadNav::kPathPlayer;
                cookie_trail = true;
                decision_filter = true;
                break;

            case DRIVER_REMOTE:
                path_type = WRoadNav::kPathPlayer;
                cookie_trail = true;
                decision_filter = true;
                break;

            case DRIVER_RACER:
                path_type = WRoadNav::kPathRacer;
                cookie_trail = true;
                decision_filter = true;
                break;

            case DRIVER_TRAFFIC:
                cookie_trail = true;
                break;

            default:
                break;
        }
    }
    this->mDriveToNav->SetVehicle(this);
    this->mDriveToNav->SetPathType(path_type);
    this->mDriveToNav->SetCookieTrail(cookie_trail);
    this->mDriveToNav->SetDecisionFilter(decision_filter);

    this->mTarget = new AITarget(this->GetSimable());
    this->mCollNav = new WRoadNav();

    this->MakeDebugable(DBG_AI);
    this->ResetInternals();
    this->mThinkTask = this->AddTask(UCrc32("AIVehicle"), update_rate, stagger, taskmode);
    Sim::ProfileTask(this->mThinkTask, "AI Think");

    const char *layer_name = GetCaffeineLayerName(this->GetVehicle()->GetDriverClass());
}

AIVehicle::~AIVehicle() {
    delete this->mDriveToNav;
    delete this->mTarget;
    delete this->mCollNav;
    delete this->mCurrentGoal;
    if (this->mThinkTask != nullptr) {
        this->RemoveTask(this->mThinkTask);
    }
    delete this->mAttributes;
}

void AIVehicle::ResetInternals() {
    this->GetOwner()->QueryInterface(&this->mITransmission);
    this->GetOwner()->QueryInterface(&this->mISuspension);
    this->GetOwner()->QueryInterface(&this->mIEngine);
    this->GetOwner()->QueryInterface(&this->mIInput);
    this->ClearGoal();
    this->mLastSpawnTime = 0.0f;
    this->mCanRespawn = false;
    this->mDrivableToNav = false;
    this->mReverseOverrideTimer = 0.0f;
    this->mReverseOverrideSteer = 0.0f;
    this->mReverseOverrideDirection = false;
    this->mDrivableToTargetPos = false;
    this->mDestSegment = -1;
    this->mReversingSpeed = false;
    this->mSteeringBehind = false;
}

void AIVehicle::OnTaskSimulate(float dT) {
    if (this->GetVehicle()->IsActive() && this->mDriveFlags != 0) {
        this->OnDriving(dT);
    }
}

bool AIVehicle::OnClearCausality(float start_time) {
    return Sim::GetTime() - start_time > Tweak_MaxAICausalityTime;
}

float AIVehicle::GetAcceleration(float at) const {
    if (this->mTopSpeed > 0.0f) {
        Table table(this->mAccelData, NUM_ELEMENTS(this->mAccelData), 0.0f, this->mTopSpeed);
        return table.GetValue(UMath::Abs(at));
    } else {
        return 0.0f;
    }
}

bool AIVehicle::OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) {
    if (this->mAvoidableRadius > 0.0f && this->GetVehicle()->IsActive()) {
        IRigidBody *rb = this->GetOwner()->GetRigidBody();
        if (rb != nullptr) {
            sweep = UMath::Max(this->mAvoidableRadius, 2.0f * rb->GetSpeed());
            pos = rb->GetPosition();
            return true;
        }
    }
    return false;
}

void AIVehicle::DoNOS() {
    bool wantnos = false;
    bool isnos = this->GetInput()->GetControls().fNOS;
    float myspeed = this->GetVehicle()->GetSpeed();
    float desiredspeed = this->GetDriveSpeed();

    if (desiredspeed > (isnos ? KPH2MPS(90.0f) : KPH2MPS(120.0f))) {
        if (myspeed > (isnos ? KPH2MPS(50.0f) : KPH2MPS(80.0f))) {
            if ((desiredspeed - myspeed) > (isnos ? KPH2MPS(10.0f) : KPH2MPS(60.0f))) {
                IRigidBody *ibody;
                if (this->GetVehicle()->QueryInterface(&ibody)) {
                    UMath::Vector3 driveoff = this->GetDriveTarget() - this->GetVehicle()->GetPosition();
                    float drivelength = UMath::Length(driveoff);
                    float align = UMath::Dot(driveoff, ibody->GetLinearVelocity());
                    drivelength = UMath::Length(driveoff);

                    if (align > (isnos ? drivelength * 0.8f * myspeed : drivelength * 0.95f * myspeed)) {
                        wantnos = true;
                    }
                }
            }
        }
    }
    this->GetInput()->SetControlNOS(wantnos);
}

bool AIVehicle::OnTask(HSIMTASK hTask, float dT) {
    ProfileNode profile_node("TODO", 0);
    if (hTask == this->mThinkTask) {
        if (this->IsPaused() || TheOnlineManager.GetState() == OLS_RACE_END) {
            return true;
        }
        this->ClearDriveFlags();
        if (this->GetVehicle()->IsActive()) {
            this->Update(dT);
        }
        HCAUSE hcause = this->GetOwner()->GetCausality();
        if (hcause != nullptr) {
            float start_time = this->GetOwner()->GetCausalityTime();
            if (this->OnClearCausality(start_time)) {
                this->GetOwner()->SetCausality(nullptr, 0.0f);
            }
        }
        return true;
    } else {
        this->Sim::Object::OnTask(hTask, dT);
        return false;
    }
}

void AIVehicle::OnOwnerAttached(IAttachable *pOther) {
    IPursuit *ipursuit;
    if (this->mPursuit == nullptr && pOther->QueryInterface(&ipursuit)) {
        this->mPursuit = ipursuit;
    }
    IRoadBlock *iroadblock;
    if (this->mRoadBlock == nullptr && pOther->QueryInterface(&iroadblock)) {
        this->mRoadBlock = iroadblock;
    }
    this->Behavior::OnOwnerAttached(pOther);
}

void AIVehicle::OnOwnerDetached(IAttachable *pOther) {
    if (UTL::COM::ComparePtr(pOther, this->mPursuit)) {
        this->mPursuit = nullptr;
    }
    if (UTL::COM::ComparePtr(pOther, this->mRoadBlock)) {
        this->mRoadBlock = nullptr;
    }
    this->Behavior::OnOwnerDetached(pOther);
}

void AIVehicle::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        this->GetOwner()->QueryInterface(&this->mIEngine);
        this->GetOwner()->QueryInterface(&this->mITransmission);
    }
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        this->GetOwner()->QueryInterface(&this->mISuspension);
    }
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mCollisionBody);
    }
    if (this->mCurrentGoal != nullptr) {
        this->mCurrentGoal->OnBehaviorChange(mechanic);
    }
    this->Behavior::OnBehaviorChange(mechanic);
}

void AIVehicle::ClearGoal() {
    if (this->mCurrentGoal != nullptr) {
        delete this->mCurrentGoal;
        this->mCurrentGoal = nullptr;
    }
    this->mGoalName = UCrc32::kNull;
}

void AIVehicle::SetGoal(const UCrc32 &name) {
    if (this->mGoalName == name) {
        return;
    }
    delete this->mCurrentGoal;
    this->mGoalName = name;
    this->mCurrentGoal = AIGoal::CreateInstance(name, this->GetOwner());
    DriverClass driverclass = this->GetVehicle()->GetDriverClass();
    if (driverclass >= DRIVER_TRAFFIC && driverclass <= DRIVER_RACER) {
        IRBVehicle *ivehiclebody;
        if (this->GetOwner()->QueryInterface(&ivehiclebody)) {
            const Attrib::Gen::aivehicle &attributes = this->GetAttributes();
            unsigned int num_reactions = attributes.Num_PlayerCollisions();
            bool found = false;
            Attrib::Gen::collisionreactions reactions(static_cast<Attrib::Collection *>(nullptr), 0, nullptr);

            for (size_t i = 0; i < num_reactions; i++) {
                const AICollisionReactionRecord &record = attributes.PlayerCollisions(i);
                if (record.Goal == this->mGoalName.GetValue()) {
                    reactions.Change(record.Reaction);
                    found = true;
                    break;
                }
            }
            if (!reactions.IsValid()) {
                reactions.Change(attributes.PlayerCollisionsDefault());
            }
            ivehiclebody->SetPlayerReactions(reactions);
        }
    }
}

void AIVehicle::Update(float dT) {
    IRigidBody *rb = this->GetOwner()->GetRigidBody();
    UMath::Vector3 vfwd;
    rb->GetForwardVector(vfwd);

    float yaw = UMath::Atan2r(vfwd.x, vfwd.z);
    this->mDampedAngularVel.Integrate(rb->GetAngularVelocity().y, dT);

    if (yaw < -1.5707964f && this->mDampedAngle.GetPosition() > 1.5707964f) {
        this->mDampedAngle.SetPosition(yaw);
    } else if (yaw > 1.5707964f && this->mDampedAngle.GetPosition() < -1.5707964f) {
        this->mDampedAngle.SetPosition(yaw);
    } else {
        this->mDampedAngle.Integrate(yaw, dT);
    }

    this->UpdateSimplePhysics(dT);
}

void AIVehicle::ResetDriveToNav(eLaneSelection lane_selection) {
    bool force_center_lane = lane_selection == SELECT_CENTER_LANE;
    UMath::Vector3 forwardVector;
    this->GetVehicle()->ComputeHeading(&forwardVector);

    IRigidBody *irb = this->GetSimable()->GetRigidBody();

    const float dir_weight = 1.0f;
    WRoadNav *road_nav = this->GetDriveToNav();
    road_nav->InitAtPoint(irb->GetPosition(), forwardVector, force_center_lane, dir_weight);
    road_nav->CancelPathFinding();

    if (lane_selection == SELECT_VALID_LANE) {
        road_nav->SnapToSelectableLane();
    }
    this->UpdateRoadNavInfo();
}

bool AIVehicle::ResetVehicleToRoadNav(WRoadNav *other_nav) {
    WRoadNav *road_nav = this->GetDriveToNav();
    road_nav->InitFromOtherNav(other_nav, false);
    bool success = this->GetVehicle()->SetVehicleOnGround(road_nav->GetPosition(), road_nav->GetForwardVector());

    this->UpdateRoadNavInfo();
    return success;
}

bool AIVehicle::ResetVehicleToRoadNav(short segInd, char laneInd, float timeStep) {
    WRoadNav *pdrivetonav = this->GetDriveToNav();
    pdrivetonav->InitAtSegment(segInd, laneInd, timeStep);
    bool success = this->GetVehicle()->SetVehicleOnGround(pdrivetonav->GetPosition(), pdrivetonav->GetForwardVector());

    this->UpdateRoadNavInfo();
    return success;
}

bool AIVehicle::ResetVehicleToRoadPos(const UMath::Vector3 &position, const UMath::Vector3 &forwardVector) {
    const float dir_weight = 1.0f;
    const bool force_centre_lane = false;

    this->GetDriveToNav()->InitAtPoint(position, forwardVector, force_centre_lane, dir_weight);
    bool success = this->GetVehicle()->SetVehicleOnGround(position, forwardVector);

    this->UpdateRoadNavInfo();
    return success;
}

void AIVehicle::UpdateRoadNavInfo() {
    WRoadNav *pdrivetonav = this->GetDriveToNav();

    if (!pdrivetonav->IsValid()) {
        return;
    }

    UMath::Vector3 position;
    UMath::ScaleAdd(this->GetLinearVelocity(), 0.65f, this->GetPosition(), position);
    this->mDrivableToNav = !this->WorldCollision(position, pdrivetonav->GetPosition());
    UMath::Sub(pdrivetonav->GetPosition(), this->GetPosition(), this->mDirToNav);
    UMath::Unit(this->mDirToNav, this->mDirToNav);
}

void AIVehicle::OnReverse(float dT) {
    if ((this->mDriveFlags & 4) == 0 || this->GetReverseOverride() || this->mITransmission == nullptr) {
        return;
    }
    bool wasReversing = this->mITransmission->IsReversing();

    if (!wasReversing) {
        if (this->GetVehicle()->GetSpeed() >= 15.0f) {
            this->mReversingSpeed = false;
            return;
        }
    }

    this->mReversingSpeed = true;

    UMath::Vector3 dirVector;
    UMath::Sub(this->mDest, this->GetPosition(), dirVector);
    UMath::Unit(dirVector, dirVector);

    const UMath::Vector3 &forwardVector = this->GetForwardVector();
    float facingDot = UMath::Dot(forwardVector, dirVector);

    if (this->mITransmission->IsReversing() && facingDot > 0.0f) {
        this->mITransmission->Shift(G_FIRST);
    } else {
        if (!this->mITransmission->IsReversing() && facingDot < -0.707f) {
            this->mITransmission->Shift(G_REVERSE);
        }
    }
}

float AIVehicle::GetOverSteerCorrection(float steer) {
    // TODO the dwarf suggests something much more complicated
    return 0.0f;
}

void AIVehicle::OnSteering(float dT) {
    if ((this->mDriveFlags & 1) == 0 || this->GetInput() == nullptr) {
        return;
    }

    this->GetInput()->SetControlSteering(0.0f);
    this->GetInput()->SetControlSteeringVertical(0.0f);

    float currentSpeed = this->GetSimable()->GetRigidBody()->GetSpeedXZ();

    if (this->mDriveSpeed == 0.0f && currentSpeed < 1.0f) {
        return;
    }

    if (this->GetVehicle()->GetDriverClass() == 1) {
        WRoadNav *road_nav = this->GetDriveToNav();
        road_nav->UpdateOccludedPosition(true);
        this->SetDriveTarget(road_nav->GetOccludedPosition());
    }

    UMath::Vector3 dirVector;
    UMath::Sub(this->mDest, this->GetPosition(), dirVector);
    dirVector.y = 0.0f;
    UMath::Unit(dirVector, dirVector);

    UMath::Vector3 forwardVector;
    forwardVector = this->GetForwardVector();
    forwardVector.y = 0.0f;
    UMath::Unit(forwardVector, forwardVector);

    UMath::Vector3 steerProd;
    UMath::Cross(forwardVector, dirVector, steerProd);
    steerProd.y = UMath::Clamp(steerProd.y, -1.0f, 1.0f);
    float steer = asinf(steerProd.y);

    steer /= ANGLE2RAD(this->GetSuspension()->GetMaxSteering());
    float steerCorrection = this->GetOverSteerCorrection(steer);

    this->mSteeringBehind = false;
    if (this->mITransmission != nullptr && this->mITransmission->IsReversing()) {
        steer = -steer;
    } else {
        if (UMath::Dot(dirVector, forwardVector) < -0.2f) {
            steer = (steer < 0.0f) ? -1.0f : 1.0f;
            this->mSteeringBehind = true;
        } else {
            if (this->GetInput()->GetControls().fHandBrake == 0.0f) {
                steer += steerCorrection;
            }
        }
    }

    steer = UMath::Clamp(steer, -1.0f, 1.0f);
    this->GetInput()->SetControlSteering(steer);
}

void AIVehicle::OnGasBrake(float dT) {
    if ((this->mDriveFlags & 2) == 0 || this->GetInput() == nullptr) {
        return;
    }

    bool reversing = false;
    this->GetInput()->SetControlGas(0.0f);
    this->GetInput()->SetControlBrake(0.0f);
    this->GetInput()->SetControlHandBrake(0.0f);
    this->GetInput()->SetControlSteeringVertical(0.0f);

    if (this->mITransmission != nullptr) {
        if (this->mITransmission->IsReversing()) {
            reversing = true;
        }

        if (this->mITransmission != nullptr && this->GetVehicle()->GetDriverClass() == DRIVER_TRAFFIC) {
            bool in_shock = this->GetVehicle()->InShock();
            GearID drive_gear = reversing ? G_REVERSE : G_FIRST;
            bool in_neutral = this->mITransmission->GetGear() == G_NEUTRAL;

            if (in_shock ^ in_neutral) {
                this->mITransmission->Shift(in_shock ? G_NEUTRAL : drive_gear);
            }

            if (in_shock) {
                return;
            }
        }
    }

    float currentSpeed = this->GetVehicle()->GetSpeed();
    float desiredSpeed = this->mDriveSpeed;
    float steer;

    if (!this->mReversingSpeed && this->mSteeringBehind) {
        this->GetInput()->SetControlGas(1.0f);
        this->GetInput()->SetControlHandBrake(1.0f);
        return;
    }

    this->GetInput()->GetControls();

    if (desiredSpeed < 0.5f) {
        this->GetInput()->SetControlBrake(1.0f);
        return;
    }

    if (reversing) {
        if (currentSpeed > 1.0f) {
            this->GetInput()->SetControlBrake(1.0f);
        } else {
            this->GetInput()->SetControlGas(1.0f);
        }
        return;
    }

    if (currentSpeed < -1.0f) {
        this->GetInput()->SetControlBrake(1.0f);
        return;
    }

    if (desiredSpeed < currentSpeed) {
        if (UMath::Abs(desiredSpeed - currentSpeed) > 2.5f || desiredSpeed < 5.0f) {
            this->GetInput()->SetControlBrake(1.0f);
        }
        return;
    }

    this->GetInput()->SetControlGas(1.0f);
}

void AIVehicle::OnDriving(float dT) {
    this->OnReverse(dT);
    this->OnSteering(dT);
    this->OnGasBrake(dT);
}

float AIVehicle::GetPathDistanceRemaining() {
    float distance = 0.0f;
    WRoadNav *road_nav = this->GetDriveToNav();
    if (road_nav != nullptr) {
        const UMath::Vector3 &car_position = this->GetSimable()->GetRigidBody()->GetPosition();
        float path_distance_remaining = road_nav->GetPathDistanceRemaining();

        if (path_distance_remaining == 0.0f) {
            AITarget *target = this->GetTarget();
            if (target->IsValid()) {
                const UMath::Vector3 &target_position = target->GetPosition();
                if (road_nav->IsPointInCookieTrail(target_position, 30.0f)) {
                    UMath::Vector3 target_direction;
                    UMath::Unit(target->GetDirection(), target_direction);

                    UMath::Vector3 car_to_target = target_position - car_position;
                    distance = bMax(0.0f, UMath::Dot(car_to_target, target_direction));
                }
            }
        } else {
            UMath::Vector3 nav_forward;
            UMath::Unit(road_nav->GetForwardVector(), nav_forward);

            UMath::Vector3 car_to_nav = road_nav->GetPosition() - car_position;
            distance = UMath::Dot(car_to_nav, nav_forward) + path_distance_remaining;
        }
    }

    return distance;
}

void AIVehicle::ClearReverseOverride() {
    this->mSteeringBehind = false;
    this->mReversingSpeed = false;
    this->mReverseOverrideTimer = 0.0f;
}

void AIVehicle::SetReverseOverride(float time) {
    if (this->mITransmission != nullptr) {
        this->mReverseOverrideTimer = time;
        if (this->mITransmission->IsReversing()) {
            this->mITransmission->Shift(G_FIRST);
        } else {
            this->mITransmission->Shift(G_REVERSE);
        }
    }
}

void AIVehicle::UpdateReverseOverride(float dT) {
    if (this->mReverseOverrideTimer > 0.0f && (this->mReverseOverrideTimer -= dT) <= 0.0f) {
        this->mReverseOverrideTimer = 0.0f;
        this->mSteeringBehind = false;
        if (this->mITransmission != nullptr && this->mITransmission->IsReversing()) {
            this->mITransmission->Shift(G_FIRST);
        }
    }
}

float AIVehicle::GetLookAhead() {
    IRigidBody *irigidbody = this->GetSimable()->GetRigidBody();

    UMath::Vector3 dimension;
    irigidbody->GetDimension(dimension);

    float currentSpeed = irigidbody->GetSpeedXZ();
    float lookAhead = UMath::Min(15.0f, UMath::Max(dimension.z + 7.5f, currentSpeed * 0.5f));

    return lookAhead;
}

void AIVehicle::UpdateTargeting() {
    if (!this->mTarget->IsValid()) {
        return;
    }
    ProfileNode profile_node("TODO", 0);
    this->mDrivableToTargetPos = !this->WorldCollision(this->GetPosition(), this->mTarget->GetPosition());
}

bool AIVehicle::WorldCollision(const UMath::Vector3 &pos, const UMath::Vector3 &dest) {
    if (UMath::DistanceSquare(pos, dest) > 40000.0f) {
        return true;
    }

    UMath::Vector4 posToDest[2];
    posToDest[0] = UMath::Vector4Make(pos, 1.0f);
    posToDest[0].y += 0.5f;
    posToDest[1] = UMath::Vector4Make(dest, 1.0f);
    posToDest[1].y += 0.5f;

    WCollisionMgr::WorldCollisionInfo cInfo;
    if (WCollisionMgr(0, 3).CheckHitWorld(posToDest, cInfo, 2) != 0) {
        if (0.5f < UMath::DistanceSquarexyz(posToDest[1], cInfo.fCollidePt)) {
            return true;
        }
    }

    return false;
}

// STRIPPED
bool AIVehicle::BarriersInPath(bool reverse) {
    return false;
}

void AIVehicle::OnCollision(const COLLISION_INFO &cinfo) {}

bool AIVehicle::GetWorldAvoidanceInfo(float dT, UMath::Vector3 &leftCollNormal, UMath::Vector3 &rightCollNormal) const {
    if (this->mITransmission != nullptr && this->mITransmission->IsReversing()) {
        return false;
    }

    IRigidBody *irb = this->GetSimable()->GetRigidBody();
    const UMath::Vector3 &position = irb->GetPosition();

    UMath::Vector3 forwardVector;
    irb->GetForwardVector(forwardVector);

    UMath::Vector3 rightVector;
    irb->GetRightVector(rightVector);

    UMath::Vector3 dimension;
    irb->GetDimension(dimension);

    leftCollNormal = UMath::Vector3::kZero;
    rightCollNormal = UMath::Vector3::kZero;

    bool foundCollision = false;
    for (float i = -1.0f; i <= 1.0f; i += 2.0f) {
        UMath::Vector3 collVec;
        UMath::Scale(rightVector, i, collVec);
        UMath::Vector3 boundPos;
        UMath::ScaleAdd(collVec, dimension.x, position, boundPos);
        UMath::ScaleAdd(forwardVector, dimension.z, boundPos, boundPos);

        UMath::Scale(rightVector, i * 0.25f, collVec);
        UMath::Add(forwardVector, collVec, collVec);
        UMath::Unit(collVec, collVec);

        UMath::Vector3 collPos;
        float minDistance = irb->GetRadius() + 2.5f;
        float collisionScale = UMath::Max(irb->GetRadius() + irb->GetSpeedXZ() * 0.25f, minDistance);
        UMath::ScaleAdd(collVec, collisionScale, position, collPos);

        UMath::Vector4 posToDest[2];
        posToDest[0] = UMath::Vector4Make(boundPos, 1.0f);
        posToDest[1] = UMath::Vector4Make(collPos, 1.0f);

        WCollisionMgr::WorldCollisionInfo cInfo;

        if (WCollisionMgr(0, 3).CheckHitWorld(posToDest, cInfo, 2) != 0) {
            foundCollision = true;
            if (i < 0.0f) {
                leftCollNormal = UMath::Vector4To3(cInfo.fNormal);
            } else {
                rightCollNormal = UMath::Vector4To3(cInfo.fNormal);
            }
        }
    }

    return foundCollision;
}

WRoadNav *AIVehicle::GetCollNav(const UMath::Vector3 &forwardVector, float predictTime) {
    this->mCollNav->SetNavType(WRoadNav::kTypeDirection);

    if (predictTime > 0.0f) {
        UMath::Matrix4 orientMat = this->GetOrientation();
        UMath::Vector3 predictionresult;

        AI::Math::PredictPosition(predictTime, this->GetPosition(), orientMat, this->GetLinearVelocity(), this->GetAngularVelocity(),
                                  predictionresult);
        this->mCollNav->InitAtPoint(predictionresult, forwardVector, false, 0.0f);
    } else {
        this->mCollNav->InitAtPoint(this->GetPosition(), forwardVector, false, 0.0f);
    }

    return this->mCollNav;
}

void AIVehicle::SetSpawned() {
    this->ResetInternals();
    IDamageable *idamage;
    if (this->GetSimable()->QueryInterface(&idamage)) {
        idamage->ResetDamage();
    }
    EventSequencer::IEngine *ievents = this->GetOwner()->GetEventSequencer();
    if (ievents != nullptr) {
        ievents->Reset(Sim::GetTime());
    }
    IArticulatedVehicle *iarticulation;
    if (this->GetOwner()->QueryInterface(&iarticulation)) {
        IVehicle *itrailer = iarticulation->GetTrailer();
        IVehicleAI *iai;
        if (itrailer != nullptr && itrailer->QueryInterface(&iai)) {
            iai->SetSpawned();
        }
    }
    IAIHelicopter *ih;
    if (this->GetOwner()->QueryInterface(&ih)) {
        ih->SetFuelFull();
    }
    this->mCanRespawn = false;
}

extern AIVehicleHelicopter *gHeliVehicle;

void AIVehicle::UnSpawn() {
    IAIHelicopter *ih;
    if (this->GetOwner()->QueryInterface(&ih)) {
        gHeliVehicle = nullptr;
    }
    if (this->IsSimplePhysicsActive()) {
        this->DisableSimplePhysics();
    }
    this->ClearGoal();
    this->GetVehicle()->Deactivate();

    IPursuitAI *ipai;
    if (this->GetOwner()->QueryInterface(&ipai)) {
        ipai->SetSupportGoal(UCrc32(static_cast<const char *>(nullptr)));
    }
}

static const float kNeverVisibleRespawnTime = 10.0f; // size: 0x4, Decl: 1522

bool AIVehicle::CanRespawn(bool respawnAvailable) {
    if (!respawnAvailable) {
        this->mCanRespawn = true;
    }
    bool rv = false;
    if (this->mCanRespawn) {
        rv = this->mLastSpawnTime > 8.0f;
    }
    if (!rv && respawnAvailable && this->mLastSpawnTime > kNeverVisibleRespawnTime) {
        rv = true;
    }
    return rv;
}

static const float Tweak_OffWorldAccel[2] = {0.5f, 1.0f};
static const float Tweak_OffWorldSpeed[2] = {0.75f, 1.0f};

void AIVehicle::UpdateSimplePhysics(float dT) {
    if (!this->IsSimplePhysicsActive()) {
        return;
    }

    ISimable *isimable = this->GetSimable();
    IVehicle *ivehicle = this->GetVehicle();
    IRigidBody *irigidbody = isimable->GetRigidBody();
    const UMath::Vector3 &position = irigidbody->GetPosition();

    UMath::Vector3 newPosition = position;
    UMath::Matrix4 vehicleMat;
    UMath::Init(vehicleMat);

    UMath::Vector3 destPos = this->mDest;
    destPos.y += 1.0f;

    UMath::Vector3 dirVector = UVector3(destPos) - position;
    UMath::Unit(dirVector, dirVector);

    float skill = this->GetSkill();
    float currentSpeed = irigidbody->GetSpeed();
    float driveSpeed = currentSpeed;

    if (currentSpeed > this->mDriveSpeed) {
        driveSpeed -= dT * 30.0f;
        driveSpeed = UMath::Max(driveSpeed, this->mDriveSpeed);
    } else {
        driveSpeed += this->GetAcceleration(currentSpeed) * dT * UMath::Lerp(Tweak_OffWorldAccel[0], Tweak_OffWorldAccel[1], skill);
        driveSpeed = UMath::Min(driveSpeed, this->mDriveSpeed);
    }

    float top_speed = this->GetTopSpeed() * UMath::Lerp(Tweak_OffWorldSpeed[0], Tweak_OffWorldSpeed[1], skill);

    driveSpeed = UMath::Min(top_speed, UMath::Max(0.0f, driveSpeed));

    if (this->mITransmission->IsReversing()) {
        driveSpeed *= -0.5f;
    }

    UMath::ScaleAdd(dirVector, driveSpeed * dT, newPosition, newPosition);

    WWorldPos &wpos = isimable->GetWPos();
    bool up_valid = false;

    wpos.FindClosestFace(position, true);

    UMath::Vector4 newUpVector = {};
    newUpVector.y = 1.0f;

    if (!ivehicle->IsOffWorld()) {
        UMath::Vector4 worldNormal = {};
        worldNormal.y = 1.0f;

        wpos.UNormal(&UMath::Vector4To3(worldNormal));
        UMath::Unitxyz(worldNormal, worldNormal);
        worldNormal.w = 0.0f;

        if (UMath::LengthSquare(worldNormal) > 0.0f && worldNormal.y >= 0.707f) {
            up_valid = true;
            newUpVector = worldNormal;
        }
    }

    UMath::Init(vehicleMat.v3);
    UMath::Vector4To3(vehicleMat.v2) = dirVector;

    UMath::UnitCrossxyz(newUpVector, vehicleMat.v2, vehicleMat.v0);

    if (up_valid) {
        vehicleMat.v1 = newUpVector;
        UMath::UnitCrossxyz(vehicleMat.v0, newUpVector, vehicleMat.v2);
    } else {
        UMath::UnitCrossxyz(vehicleMat.v2, vehicleMat.v0, vehicleMat.v1);
    }

    vehicleMat.v0.w = vehicleMat.v1.w = vehicleMat.v2.w = 0.0f;

    float elevation = destPos.y;

    WRoadNav *road_nav = this->GetDriveToNav();
    if (road_nav != nullptr && road_nav->HasCookieTrail()) {
        elevation = road_nav->GetCurrentCookie().Centre.y + 1.0f;
    }

    if (!ivehicle->IsOffWorld()) {
        elevation = wpos.HeightAtPoint(position);
    }

    float rideheight = 0.0f;
    UMath::Vector3 dimension;
    UMath::Vector3 linearVelocity;

    irigidbody->GetDimension(dimension);

    if (this->GetSuspension() != nullptr) {
        for (int i = 0; i < 4; i++) {
            rideheight = UMath::Max(rideheight, this->GetSuspension()->GetRideHeight(i));
        }
    }

    elevation += rideheight + dimension.y;
    newPosition.y = elevation;

    irigidbody->SetPosition(newPosition);
    irigidbody->SetOrientation(vehicleMat);

    UMath::Scale(dirVector, driveSpeed, linearVelocity);
    irigidbody->SetLinearVelocity(linearVelocity);
    irigidbody->SetAngularVelocity(UMath::Vector3::kZero);
}

void AIVehicle::EnableSimplePhysics() {
    if (this->IsSimplePhysicsActive()) {
        return;
    }
    IVehicle *vehicle = this->GetVehicle();
    vehicle->SetPhysicsMode(PHYSICS_MODE_EMULATED);
}

void AIVehicle::DisableSimplePhysics() {
    if (!this->IsSimplePhysicsActive()) {
        return;
    }
    IVehicle *vehicle = this->GetVehicle();
    if (vehicle->GetPhysicsMode() == PHYSICS_MODE_EMULATED) {
        vehicle->SetPhysicsMode(PHYSICS_MODE_SIMULATED);
    }

    UMath::Vector3 forward;
    IRigidBody *irigidbody = this->GetSimable()->GetRigidBody();
    UMath::Vector3 angular_velocity = irigidbody->GetAngularVelocity();
    irigidbody->GetForwardVector(forward);
    float speed = irigidbody->GetSpeed();

    vehicle->SetVehicleOnGround(irigidbody->GetPosition(), forward);
    irigidbody->SetAngularVelocity(angular_velocity);
    vehicle->SetSpeed(speed);

    IRBVehicle *rigid_body_vehicle;
    if (this->GetOwner()->QueryInterface(&rigid_body_vehicle)) {
        rigid_body_vehicle->SetInvulnerability(INVULNERABLE_FROM_PHYSICS_SWITCH, 1.0f);
    }
}

bool AIVehicle::IsSimplePhysicsActive() {
    return this->GetVehicle()->GetPhysicsMode() == PHYSICS_MODE_EMULATED;
}

// total size: 0x10
// Decl: 1912
struct path_spot {
    // Decl: 1913
    path_spot() {}
    // Decl: 1914
    path_spot(short s, int i, float p, float l) : segmentindex(s), nodeind(i), param(p), laneoffset(l) {}
    // Decl: 1915
    path_spot(const WRoadNav &nav) {
        this->segmentindex = nav.GetSegmentInd();
        this->nodeind = nav.GetNodeInd();
        this->param = nav.GetSegmentTime();
        this->laneoffset = nav.GetLaneOffset();
    }

    void init_nav(WRoadNav &nav) const;                              // Decl: 1918
    void init_nav(WRoadNav &nav, const UMath::Vector3 &point) const; // Decl: 1919

    short segmentindex; // offset 0x0, size 0x2, Decl: 1921
    int nodeind;        // offset 0x4, size 0x4, Decl: 1922
    float param;        // offset 0x8, size 0x4, Decl: 1923
    float laneoffset;   // offset 0xC, size 0x4, Decl: 1924
};

// Decl: 1927
void path_spot::init_nav(WRoadNav &nav) const {
    WRoadNetwork &roadnetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadnetwork.GetSegment(segmentindex);
    float tparam = this->param;

    UMath::Vector3 dir;
    roadnetwork.GetSegmentForwardVector(*segment, dir);
    if (nodeind == 0) {
        tparam = 1.0f - tparam;
        UMath::Negate(dir);
    }

    UMath::Vector3 point;
    segment->GetStartControl(point);

    nav.SetPathType(WRoadNav::kPathCop);
    nav.SetLaneType(WRoadNav::kLaneCop);
    nav.SetNavType(WRoadNav::kTypeDirection);
    nav.InitAtSegment(segmentindex, tparam, point, dir, true);
    nav.ChangeLanes(laneoffset, 0.0f);
}

void path_spot::init_nav(WRoadNav &nav, const UMath::Vector3 &point) const {
    WRoadNetwork &roadnetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadnetwork.GetSegment(segmentindex);
    float tparam = this->param;
    UMath::Vector3 dir;

    roadnetwork.GetSegmentForwardVector(*segment, dir);
    if (nodeind == 0) {
        tparam = 1.0f - tparam;
        UMath::Negate(dir);
    }

    nav.SetPathType(WRoadNav::kPathCop);
    nav.SetLaneType(WRoadNav::kLaneCop);
    nav.SetNavType(WRoadNav::kTypeDirection);
    nav.InitAtSegment(segmentindex, tparam, point, dir, false);

    float laneoff = nav.SnapToSelectableLane(nav.GetLaneOffset());
    nav.ChangeLanes(laneoff, 0.0f);
}

static const float kRoadRethinkTimeout = 0.33f; // Decl: 2930

// total size: 0x9C
// Decl: 2933
struct road_walker {
    typedef UTL::Std::set<short int, _type_set> segment_set;

    // Decl: 2936
    road_walker() {}

    // Decl: 2938
    void set_race_routes(bool on) {
        this->raceroutes = on;
    }

    bool walk_road(const UMath::Vector3 &start, const UMath::Vector3 &dir, float futuredist, float targetdist, short prevfuture,
                   int prevnodeind); // Decl: 2940

    // Decl: 2942
    const path_spot &get_best_start_spot() {
        return this->beststartspot;
    }
    // Decl: 2943
    const path_spot &get_best_future_spot() {
        return this->bestfuturespot;
    }
    // Decl: 2944
    const path_spot &get_best_target_spot() {
        return this->besttargetspot;
    }

  private:
    bool raceroutes;            // offset 0x0, size 0x1, Decl: 2948
    float bestscore;            // offset 0x4, size 0x4, Decl: 2950
    path_spot beststartspot;    // offset 0x8, size 0x10, Decl: 2951
    path_spot bestfuturespot;   // offset 0x18, size 0x10, Decl: 2952
    path_spot besttargetspot;   // offset 0x28, size 0x10, Decl: 2953
    UMath::Vector3 direction;   // offset 0x38, size 0xC, Decl: 2955
    UMath::Vector3 futurepoint; // offset 0x44, size 0xC, Decl: 2956
    UMath::Vector3 targetpoint; // offset 0x50, size 0xC, Decl: 2957
    short previousfutures[2];   // offset 0x5C, size 0x4, Decl: 2958
    float futurescale;          // offset 0x60, size 0x4, Decl: 2959
    path_spot futurespot;       // offset 0x64, size 0x10, Decl: 2961
    path_spot startspot;        // offset 0x74, size 0x10, Decl: 2962
    UMath::Vector3 startpoint;  // offset 0x84, size 0xC, Decl: 2964
    float startscore;           // offset 0x90, size 0x4, Decl: 2965

    // Decl: 2968
    struct start_record {
        // Decl: 2970
        start_record(float s, WRoadNav &nav) : score(s), spot(nav), point(nav.GetPosition()) {}

        float score;          // offset 0x0, size 0x4, Decl: 2972
        path_spot spot;       // offset 0x4, size 0x10, Decl: 2973
        UMath::Vector3 point; // offset 0x14, size 0xC, Decl: 2974

        // Decl: 2976
        bool operator<(const start_record &o) const {
            return this->score < o.score;
        }
    };

    typedef std::vector<road_walker::start_record> start_vector;

    int numwalkallpaths; // offset 0x94, size 0x4, Decl: 2979
    int numevaluates;    // offset 0x98, size 0x4, Decl: 2980

    // Decl: 2982
    void walk_all_paths(const path_spot &start, float futuredist, float targetdist, bool coppenalty);
    void evaluate_end(const path_spot &targetspot, bool coppenalty); // Decl: 2983

    static const int walk_limit = 32;     // Decl: 2985
    static const int evaluate_limit = 10; // Decl: 2986

    // Decl: 2988
    float node_find_radius() const {
        return 25.0f;
    }
};

static const int NumPrintWalkAllPaths = 0; // Decl: 2991
float TotalWalkPathTime = 0.0f;            // Decl: 2992

bool road_walker::walk_road(const UMath::Vector3 &start, const UMath::Vector3 &dir, float futuredist, float targetdist, short prevfuture,
                            int prevnodeind) {
    ProfileNode profile_node("TODO", 0);
    int ticks = bGetTicker();
    const WGrid &grid = WGrid::Get();
    WRoadNetwork &roadnetwork = WRoadNetwork::Get();

    this->direction = dir;
    this->previousfutures[0] = prevfuture;
    this->previousfutures[1] = -1;
    this->futurescale = futuredist / targetdist;

    if (prevfuture >= 0) {
        const WRoadNode *node = roadnetwork.GetNode(roadnetwork.GetSegment(prevfuture)->fNodeIndex[prevnodeind]);
        const WRoadSegment *checksegment = GetAttachedDirectionalSegment(node, prevfuture);
        if (checksegment != nullptr) {
            this->previousfutures[1] = checksegment->fIndex;
        }
    }

    UMath::Normalize(this->direction);
    UMath::ScaleAdd(this->direction, futuredist, start, this->futurepoint);
    UMath::ScaleAdd(this->direction, targetdist, start, this->targetpoint);

    UTL::Std::set<short, _type_set> segments;
    UTL::FastVector<unsigned int> nodes;
    nodes.reserve(64);
    WGrid::Get().FindNodes(start, this->node_find_radius(), nodes);

    for (UTL::FastVector<unsigned int>::iterator i = nodes.begin(); i != nodes.end(); ++i) {
        WGridNode *grid_node = grid.fNodes[*i];
        if (grid_node != nullptr) {
            int numSegments = grid_node->GetElemTypeCount(WGrid_kRoadSegment);
            for (int i = 0; i < numSegments; ++i) {
                short index = static_cast<short>(grid_node->GetElemType(i, WGrid_kRoadSegment));
                if (!this->raceroutes || roadnetwork.GetSegment(index)->IsInRace()) {
                    segments.insert(index);
                }
            }
        }
    }

    WRoadNav startnav;
    startnav.SetPathType(WRoadNav::kPathCop);
    startnav.SetLaneType(WRoadNav::kLaneCop);
    startnav.SetNavType(WRoadNav::kTypeDirection);

    start_vector sortedstarts;
    sortedstarts.reserve(UMath::Max(33, static_cast<int>(segments.size())));

    for (UTL::Std::set<short, _type_set>::const_iterator i = segments.begin(); i != segments.end(); ++i) {
        short index = *i;

        if (index >= static_cast<int>(roadnetwork.GetNumSegments())) {
            continue;
        }
        startnav.InitAtSegment(index, start, this->direction, false);

        if (startnav.GetSegment()->IsOneWay() && startnav.GetNodeInd() == 0) {
            startnav.Reverse();
        }

        float laneoffset = startnav.SnapToSelectableLane(startnav.GetLaneOffset());
        if (laneoffset != startnav.GetLaneOffset()) {
            startnav.ChangeLanes(laneoffset, 0.0f);
        }

        float score = 0.0f;
        score += UMath::Max(0.0f, UMath::Distancexz(start, startnav.GetPosition()) - 2.0f);
        score += bMax(0.0f, bAbs(start.y - startnav.GetPosition().y) - 2.0f) * 3.0f;

        UMath::Vector3 startdir;
        startdir = startnav.GetForwardVector();
        startdir.y = 0.0f;
        UMath::Normalize(startdir);
        score += (1.0f - UMath::Dot(this->direction, startdir)) * 2.0f;

        sortedstarts.push_back(start_record(score, startnav));
    }

    if (sortedstarts.empty()) {
        return false;
    }

    std::sort(sortedstarts.begin(), sortedstarts.end());

    this->numwalkallpaths = 0;
    this->bestscore = 3.402823466e+38f; // __FLT_MAX__
    this->numevaluates = 0;

    for (start_vector::iterator i = sortedstarts.begin();
         this->numevaluates < evaluate_limit && this->numwalkallpaths < walk_limit && i != sortedstarts.end(); ++i) {
        this->startscore = i->score;
        if (this->startscore >= this->bestscore) {
            break;
        }
        this->startspot = i->spot;
        this->startpoint = i->point;
        this->walk_all_paths(this->startspot, futuredist, targetdist, false);
    }

    TotalWalkPathTime += bGetTickerDifference(ticks);
    return this->bestscore < 3.402823466e+38f; // __FLT_MAX__
}

void road_walker::walk_all_paths(const path_spot &start, float futuredist, float targetdist, bool coppenalty) {
    this->numwalkallpaths++;

    short segmentindex = start.segmentindex;
    int nodeind = start.nodeind;
    float param = start.param;
    WRoadNetwork &roadnetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadnetwork.GetSegment(segmentindex);
    const WRoadNode *node;

    while (true) {
        if (segment->IsOneWay() && nodeind == 0) {
            return;
        }

        float segmentlength = segment->GetLength();

        coppenalty = coppenalty || !segment->ShouldCopsConsider();

        if (futuredist > 0.0f) {
            float futurefraction = param + futuredist / segmentlength;
            if (futurefraction <= 1.0f) {
                this->futurespot = path_spot(segmentindex, nodeind, futurefraction, 0.0f);
            }
        }

        float targetfraction = param + targetdist / segmentlength;
        if (targetfraction <= 1.0f) {
            path_spot endspot(segmentindex, nodeind, targetfraction, 0.0f);
            this->evaluate_end(endspot, coppenalty);
            return;
        }

        futuredist -= segmentlength * (1.0f - param);
        targetdist -= segmentlength * (1.0f - param);

        const WRoadNode *node = roadnetwork.GetNode(segment->fNodeIndex[nodeind]);
        const WRoadSegment *checksegment = GetAttachedDirectionalSegment(node, segmentindex);
        if (checksegment == nullptr) {
            break;
        }

        segmentindex = checksegment->fIndex;
        nodeind = static_cast<int>(node == roadnetwork.GetNode(checksegment->fNodeIndex[0]));
        param = 0.0f;
        segment = checksegment;
    }

    node = roadnetwork.GetNode(segment->fNodeIndex[nodeind]);

    for (int i = 0; this->numevaluates < this->evaluate_limit && this->numwalkallpaths < this->walk_limit && i < node->fNumSegments; i++) {
        short newsegmentindex = static_cast<short>(node->fSegmentIndex[i]);

        if (newsegmentindex == segmentindex) {
            continue;
        }
        const WRoadSegment *newsegment = roadnetwork.GetSegment(node->fSegmentIndex[i]);

        if (newsegment->CrossesBarrier(true) || (this->raceroutes && !newsegment->IsInRace())) {
            continue;
        }
        int newnodeind = static_cast<int>(roadnetwork.GetNode(newsegment->fNodeIndex[0]) == node);
        path_spot newspot(newsegmentindex, newnodeind, 0.0f, 0.0f);
        this->walk_all_paths(newspot, futuredist, targetdist, coppenalty);
    }
}

void road_walker::evaluate_end(const path_spot &targetspot, bool coppenalty) {
    this->numevaluates++;

    float score = this->startscore;
    if (coppenalty) {
        score += 5.0f;
    }

    if (score >= this->bestscore) {
        return;
    }

    WRoadNav targetnav;
    targetspot.init_nav(targetnav, this->targetpoint);

    UMath::Vector3 deltarget = UVector3(targetnav.GetPosition()) - this->startpoint;
    deltarget.y = 0.0f;
    UMath::Normalize(deltarget);

    float score1 = (1.0f - UMath::Dot(deltarget, this->direction)) * 16.0f;
    score += score1;

    if (score >= this->bestscore) {
        return;
    }

    UMath::Vector3 localfuture;
    UMath::Lerp(this->startpoint, targetnav.GetPosition(), this->futurescale, localfuture);
    UMath::Lerp(this->futurepoint, localfuture, this->futurescale, localfuture);

    WRoadNav futurenav;
    this->futurespot.init_nav(futurenav, localfuture);

    UMath::Vector3 delfuture = UVector3(futurenav.GetPosition()) - this->startpoint;
    delfuture.y = 0.0f;
    UMath::Normalize(delfuture);

    float score2 = (1.0f - UMath::Dot(delfuture, this->direction)) * 24.0f;
    score += score2;

    UMath::Vector3 targetdir = targetnav.GetForwardVector();
    targetdir.y = 0.0f;
    UMath::Normalize(targetdir);

    float score3 = 1.0f - UMath::Dot(targetdir, this->direction);
    score += score3 + score3;

    if (futurenav.GetSegmentInd() != this->previousfutures[0] && futurenav.GetSegmentInd() != this->previousfutures[1]) {
        score += 2.0f;
    }

    if (score < this->bestscore) {
        this->beststartspot = this->startspot;
        this->bestfuturespot = path_spot(futurenav.GetSegmentInd(), futurenav.GetNodeInd(), futurenav.GetSegmentTime(), futurenav.GetLaneOffset());
        this->besttargetspot = path_spot(targetnav.GetSegmentInd(), targetnav.GetNodeInd(), targetnav.GetSegmentTime(), targetnav.GetLaneOffset());
        this->bestscore = score;
    }
}

static const bool bDoRoadUpdates = true; // Decl: 3278

void AIVehicle::UpdateRoads() {
    if (!bDoRoadUpdates) {
        return;
    }

    ProfileNode profile_node("TODO", 0);

    ICollisionBody *ibody;
    this->GetOwner()->QueryInterface(&ibody);

    UMath::Vector3 currentoff;
    UMath::Sub(ibody->GetPosition(), this->mCurrentRoad.GetPosition(), currentoff);

    bool isvalid = false;
    if (UMath::Length(currentoff) < 20.0f && this->mCurrentRoad.IsValid() && this->mFutureRoad.IsValid()) {
        isvalid = true;
    }

    float timeSinceIncrement = Sim::GetTime() - this->mRoadIncrementTimer;
    if (isvalid && timeSinceIncrement < 0.02f) {
        return;
    }

    UMath::Vector3 velocity;
    this->GetSimable()->GetLinearVelocity(velocity);
    float speed = UMath::Length(velocity);

    IPerpetrator *iperp;
    bool bRaceRouteOnly = false;
    if (this->GetOwner()->QueryInterface(&iperp) && iperp->IsRacing()) {
        bRaceRouteOnly = true;
    }

    this->mCurrentRoad.SetRaceFilter(bRaceRouteOnly);
    this->mFutureRoad.SetRaceFilter(bRaceRouteOnly);

    float timeSinceUpdate = Sim::GetTime() - this->mRoadUpdateTimer;
    if (isvalid && timeSinceUpdate < 0.33f) {
        this->mRoadIncrementTimer = Sim::GetTime();

        UMath::Vector3 road_direction;
        UMath::Unit(this->mCurrentRoad.GetForwardVector(), road_direction);

        UMath::Vector3 road_side = UMath::Vector3Make(road_direction.z, 0.0f, -road_direction.x);
        UMath::Normalize(road_side);

        float lanedelta = UMath::Dot(road_side, currentoff);
        float dist = UMath::Dot(road_direction, currentoff);

        if (dist > 0.05f) {
            float laneoffset = this->mCurrentRoad.SnapToSelectableLane(this->mCurrentRoad.GetLaneOffset() + lanedelta);
            this->mCurrentRoad.ChangeLanes(laneoffset, 0.0f);
            this->mCurrentRoad.IncNavPosition(dist, this->mCurrentRoad.GetForwardVector(), 0.0f);
        }

        dist = UMath::Length(velocity) - UMath::Distance(this->mFutureRoad.GetPosition(), this->mCurrentRoad.GetPosition());

        if (dist > 0.05f) {
            UMath::Vector3 incdir = UVector3(this->mFarFuturePosition) - this->mFutureRoad.GetPosition();
            UMath::Normalize(incdir);

            UMath::Unit(this->mFutureRoad.GetForwardVector(), road_direction);
            road_side = UMath::Vector3Make(road_direction.z, 0.0f, -road_direction.x);

            float lanedelta = UMath::Dot(velocity, road_side) * dist / speed;
            float laneoffset = this->mFutureRoad.SnapToSelectableLane(this->mFutureRoad.GetLaneOffset() + lanedelta);
            dist *= UMath::Dot(incdir, road_direction);

            this->mFutureRoad.ChangeLanes(laneoffset, 0.0f);
            this->mFutureRoad.IncNavPosition(dist, incdir, 0.0f);
        }

        return;
    }

    this->mRoadUpdateTimer = Sim::GetTime();

    UMath::Vector3 position = ibody->GetPosition();

    if (speed < 1.0f) {
        velocity = ibody->GetForwardVector();
        speed = UMath::Length(velocity);
    }

    UMath::Vector3 direction;
    UMath::Scale(velocity, 1.0f / speed, direction);

    float futuredistance = speed * UMath::Max(1.0f, 2.0f / speed);
    float targetdistance = speed * UMath::Max(2.0f, 90.0f / speed);

    road_walker walker;
    walker.set_race_routes(bRaceRouteOnly);

    if (walker.walk_road(position, direction, futuredistance, targetdistance, this->mLastFutureSegment, this->mLastFutureNodeInd)) {
        this->mCurrentRoad.SetRaceFilter(bRaceRouteOnly);
        this->mCurrentRoad.SetTrafficFilter(false);
        this->mCurrentRoad.SetCopFilter(false);

        this->mFutureRoad.SetRaceFilter(bRaceRouteOnly);
        this->mFutureRoad.SetTrafficFilter(false);
        this->mFutureRoad.SetCopFilter(false);

        walker.get_best_start_spot().init_nav(this->mCurrentRoad);
        walker.get_best_future_spot().init_nav(this->mFutureRoad);

        this->mLastFutureSegment = this->mFutureRoad.GetSegmentInd();
        this->mLastFutureNodeInd = static_cast<short>(this->mFutureRoad.GetNodeInd());

        WRoadNav targetnav;
        walker.get_best_target_spot().init_nav(targetnav);

        this->mFarFuturePosition = targetnav.GetPosition();
        UMath::Unit(targetnav.GetForwardVector(), this->mFarFutureDirection);
    }
}

WRoadNav *AIVehicle::GetCurrentRoad() {
    this->UpdateRoads();
    return &this->mCurrentRoad;
}

WRoadNav *AIVehicle::GetFutureRoad() {
    this->UpdateRoads();
    return &this->mFutureRoad;
}

const UMath::Vector3 &AIVehicle::GetFarFuturePosition() {
    this->UpdateRoads();
    return this->mFarFuturePosition;
}

const UMath::Vector3 &AIVehicle::GetFarFutureDirection() {
    this->UpdateRoads();
    return this->mFarFutureDirection;
}

const UMath::Vector3 &AIVehicle::GetSeekAheadPosition() {
    this->UpdateRoads();

    float timeSinceThink = Sim::GetTime() - this->mSeekAheadTimer;
    if (timeSinceThink > 0.33f) {
        IPerpetrator *iperp;
        bool bRaceRouteOnly = false;

        if (this->GetOwner()->QueryInterface(&iperp) && iperp->IsRacing()) {
            bRaceRouteOnly = true;
        }

        WRoadNav nav;
        nav.InitFromOtherNav(this->GetCurrentRoad(), false);

        nav.SetRaceFilter(bRaceRouteOnly);
        nav.SetLaneType(WRoadNav::kLaneCop);
        nav.SetCopFilter(!bRaceRouteOnly && this->mCurrentRoad.GetSegment()->ShouldCopsConsider());
        nav.SetPathType(WRoadNav::kPathCop);
        nav.SetNavType(WRoadNav::kTypeDirection);

        if (nav.IsValid()) {
            const float kSeekAheadTime = 7.8f;
            const float kSeekAheadOffset = 0.4f;
            this->mSeekAheadTimer = Sim::GetTime();
            float seekaheadtime = kSeekAheadTime;
            if (this->mPursuit != nullptr && this->mPursuit->GetIsAJerk()) {
                seekaheadtime *= kSeekAheadOffset;
            }

            UMath::Vector3 velocity;
            this->GetSimable()->GetLinearVelocity(velocity);
            float speed = UMath::Length(velocity);
            float inc_distance = UMath::Min(500.0f, speed * seekaheadtime + 8.0f);

            nav.IncNavPosition(inc_distance, UMath::Vector3::kZero, 0.0f);
            this->mSeekAheadPosition = nav.GetPosition();
        }
    }
    return this->mSeekAheadPosition;
}

void AIVehicle::OnDebugDraw() {}

static const float Perp_AIUpdateRate = 0.5f;

float AIPerpVehicle::mStagger = 0.0f;

AIPerpVehicle::AIPerpVehicle(const BehaviorParams &bp)
    : AIVehiclePid(bp, Perp_AIUpdateRate, mStagger, Sim::TASK_FRAME_FIXED), //
      IPerpetrator(bp.fowner),                                              //
      ICause(bp.fowner),                                                    //
      ICheater(bp.fowner),                                                  //
      LastTrafficHitTime(-1.0f),                                            //
      mHeat(0.0f),                                                          //
      mCostToState(0),                                                      //
      mPendingRepPointsNormal(0),                                           //
      mPendingRepPointsFromCopDestruction(0),                               //
      mHiddenFromCars(false),                                               //
      mHiddenFromHelicopters(false),                                        //
      mWasInRaceEventLastHeatUpdate(false),                                 //
      mHiddenZoneTimer(0.0f),                                               //
      mWasInZoneLastUpdate(false),                                          //
      mPursuitZoneCheck(0),                                                 //
      pRacerInfo(nullptr),                                                  //
      fBaseSkill(0.5f),                                                     //
      fGlueSkill(0.0f),                                                     //
      fGlueOutput(0.0f),                                                    //
      m911CallTimer(0.0f) {
    mStagger += Perp_AIUpdateRate;
    if (mStagger > 1.0f) {
        mStagger = 0.0f;
    }
    this->mPursuitEscalationAttrib = new Attrib::Gen::pursuitescalation(Attrib::key_default, 0, nullptr);
    this->mPursuitLevelAttrib = nullptr;
    this->mPursuitSupportAttrib = nullptr;
    this->SetHeat(1.0f);
    this->GetOwner()->SetCausality(this->GetInstanceHandle(), 0.0f);
    this->mHiddenZoneLatchTime = 0.05f;
    this->pGlueError = new (nullptr) PidError(nGlueIntegralTerms, nGlueDerivativeTerms, fGlueInterval);
    this->fGlueTimer = bRandom(fGlueInterval);
}

AIPerpVehicle::~AIPerpVehicle() {
    delete this->mPursuitLevelAttrib;
    delete this->mPursuitSupportAttrib;
    delete this->mPursuitEscalationAttrib;
    delete this->pGlueError;
}

static const float Tweak_AdaptiveSkillUp[3] = {0.5f, 0.75f, 1.0f};
Table AdaptiveSkillUpTable(Tweak_AdaptiveSkillUp, NUM_ELEMENTS(Tweak_AdaptiveSkillUp), 0.0f, 1.0f);

static const float Tweak_AdaptiveSkillDown[3] = {0.5f, 0.375f, 0.25f};
Table AdaptiveSkillDownTable(Tweak_AdaptiveSkillDown, NUM_ELEMENTS(Tweak_AdaptiveSkillDown), 0.0f, 1.0f);

static const float Tweak_QuickRaceSkills[3] = {0.15f, 0.4f, 1.0f};
static const float Tweak_QuickRaceSkillsNoGlue[3] = {0.15f, 0.4f, 0.8f};

void AIPerpVehicle::ComputeSkill() {
    this->fBaseSkill = 0.0f;

    if (GRaceStatus::Exists() && !this->GetOwner()->IsPlayer()) {
        GRace::Context context = GRaceStatus::Get().GetRaceContext();

        if (context == GRace::kRaceContext_QuickRace) {
            GRaceParameters *params = GRaceStatus::Get().GetRaceParameters();

            if (params != nullptr && !params->GetCatchUp()) {
                this->fBaseSkill = Tweak_QuickRaceSkillsNoGlue[GRaceStatus::Get().GetRaceParameters()->GetDifficulty()];
            } else {
                this->fBaseSkill = Tweak_QuickRaceSkills[GRaceStatus::Get().GetRaceParameters()->GetDifficulty()];
            }
        } else if (context == GRace::kRaceContext_Career) {
            if (this->pRacerInfo != nullptr) {
                GCharacter *character = this->pRacerInfo->GetGameCharacter();

                if (character != nullptr) {
                    float character_skill = UMath::Clamp(static_cast<float>(character->SkillLevel()) * 0.01f, 0.0f, 1.0f);
                    float difficulty = GRaceStatus::Get().GetAdaptiveDifficutly();

                    if (difficulty > 0.0f) {
                        difficulty *= AdaptiveSkillUpTable.GetValue(character_skill);
                    } else {
                        difficulty *= AdaptiveSkillDownTable.GetValue(character_skill);
                    }

                    this->fBaseSkill = UMath::Clamp(character_skill + difficulty, 0.0f, 1.0f);
                }
            }
        }
    }
}

void AIPerpVehicle::SetRacerInfo(GRacerInfo *info) {
    this->pRacerInfo = info;
    this->ComputeSkill();
}

// total size: 0x8
struct FindAvgComplete {
    FindAvgComplete() : Percent(0.0f), Count(0.0f) {}

    void operator()(IVehicle *vehicle) {
        IPerpetrator *ai;
        if (vehicle->QueryInterface(&ai)) {
            GRacerInfo *info = ai->GetRacerInfo();
            if (info != nullptr) {
                this->Percent += info->GetPctRaceComplete();
                this->Count += 1.0f;
            }
        }
    }

    float Result() const {
        if (this->Count > 0.0f) {
            return this->Percent / this->Count;
        }
        return 0.0f;
    }

    float Percent; // offset 0x0, size 0x4
    float Count;   // offset 0x4, size 0x4
};

static const float Tweak_PerformanceGlue = 0.5f;

static const float Tweak_SpeedTrapCatchupGlue = 0.5f;

static const float Tweak_SpeedTrapSlowDownGlue = 0.5f;

static const float Tweak_CatchupGlueSkill[3] = {0.33f, 0.66f, 1.0f};

Table CatchupGlueTable(Tweak_CatchupGlueSkill, NUM_ELEMENTS(Tweak_CatchupGlueSkill), 0.0f, 1.0f);

static const float Tweak_SlowDownGlueSkill[3] = {1.0f, 1.0f, 0.66f};

Table SlowDownGlueTable(Tweak_SlowDownGlueSkill, NUM_ELEMENTS(Tweak_SlowDownGlueSkill), 0.0f, 1.0f);

void AIPerpVehicle::Update(float dT) {
    ProfileNode profile_node("TODO", 0);

    static const int car_hash = bStringHash("Car");
    static const int heli_hash = bStringHash("Heli");

    this->m911CallTimer -= dT;
    AIVehicle::Update(dT);

    this->mDriveToNav->SetRaceFilter(this->IsRacing());
    this->fGlueSkill = 0.0f;
    this->fGlueOutput = 0.0f;

    bool catchup = this->IsRacing() && !this->GetVehicle()->IsStaging();

    GRacerInfo *racer_info = this->GetRacerInfo();

    if (catchup && racer_info != nullptr && !this->GetOwner()->IsPlayer()) {
        IVehicle *player = IVehicle::First(VEHICLE_PLAYERS);

        this->fGlueTimer += dT;

        if (this->fGlueTimer > 1.0f && player != nullptr) {
            float percent_complete = racer_info->GetPctRaceComplete();

            float average_complete = IVehicle::ForEach(VEHICLE_PLAYERS, FindAvgComplete()).Result();

            float glue_error = (GRaceStatus::Get().GetRaceLength() / 100.0f) * (average_complete - percent_complete);

            Physics::Info::Performance perf;
            if (player->GetPerformance(perf)) {
                float performance_ratio = (1.0f - perf.TopSpeed) * Tweak_PerformanceGlue + 1.0f;
                glue_error *= performance_ratio;
            }

            this->pGlueError->Record(glue_error, this->fGlueTimer, false, false);
            this->fGlueTimer -= 1.0f;
        }

        bool off_world = this->IsSimplePhysicsActive();

        if (GRaceStatus::Get().ComputeCatchUpSkill(racer_info, this->pGlueError, &this->fGlueOutput, &this->fGlueSkill, off_world)) {
            if (!off_world) {
                if (this->fGlueSkill > 0.0f) {
                    if (!GRaceStatus::IsSpeedTrapRace()) {
                        this->fGlueSkill *= CatchupGlueTable.GetValue(this->fBaseSkill);
                    } else {
                        this->fGlueSkill *= Tweak_SpeedTrapCatchupGlue;
                    }
                } else if (this->fGlueSkill < 0.0f) {
                    if (GRaceStatus::IsSpeedTrapRace()) {
                        this->fGlueSkill *= Tweak_SpeedTrapSlowDownGlue;
                    } else {
                        this->fGlueSkill *= SlowDownGlueTable.GetValue(this->fBaseSkill);
                    }
                }
            }
        } else {
            this->fGlueOutput = 0.0f;
            this->fGlueSkill = 0.0f;
        }
    }

    if (ForcePerpHidden) {
        this->mHiddenFromCars = true;
        this->mHiddenZoneTimer = this->mHiddenZoneLatchTime + 1.0f;
        this->mHiddenFromHelicopters = true;
        return;
    }

    bVector3 nfspos;
    IRigidBody *rigid_body = this->GetSimable()->GetRigidBody();
    UMath::Vector3 myPos = rigid_body->GetPosition();
    myPos.y -= rigid_body->GetDimension().y;
    eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(myPos), nfspos);

    bVector2 pos2(nfspos.x, nfspos.y);

    this->mHiddenFromCars = false;
    this->mHiddenFromHelicopters = false;

    bool NotSeenRightNow = true;
    IPursuit *ip = this->GetPursuit();

    if (ip != nullptr) {
        NotSeenRightNow = ip->GetEvadeLevel() >= 0.05f;

        if (ip->IsPerpBusted()) {
            IInput *ii;
            if (this->GetOwner()->QueryInterface(&ii)) {
                ii->SetControlGas(0.0f);
                ii->SetControlBrake(1.0f);
                ii->SetControlSteering(0.0f);
                ii->SetControlSteeringVertical(0.0f);
                ii->SetControlHandBrake(1.0f);
                ii->SetControlNOS(false);
            }
        }
    } else {
        this->mPursuitZoneCheck--;
        if (this->mPursuitZoneCheck < 0) {
            this->mPursuitZoneCheck = 10;
#ifdef EA_BUILD_A124
            if (ICopMgr::Exists() && (!GRaceStatus::Exists() || GRaceStatus::Get().GetPlayMode() == 0)) {
#else
            if (ICopMgr::Exists() && ICopMgr::Get()->VehicleSpawningEnabled(false) &&
                (!GRaceStatus::Exists() || GRaceStatus::Get().GetPlayMode() == 0)) {
#endif
                TrackPathZone *azone = TheTrackPathManager.FindZone(&pos2, TRACK_PATH_ZONE_PURSUIT_START, nullptr);
                if (azone != nullptr) {
                    ICopMgr::Get()->LockoutCops(false);
                    MForcePursuitStart(static_cast<int>(this->GetHeat())).Post(UCrc32("AICopManager"));
                }
            }
        }
    }

    int zoneCount = 0;

    for (TrackPathZone *azone = TheTrackPathManager.FindZone(&pos2, TRACK_PATH_ZONE_HIDDEN, nullptr); azone != nullptr;
         azone = TheTrackPathManager.FindZone(&pos2, TRACK_PATH_ZONE_HIDDEN, azone)) {
        float elevation = azone->GetElevation();

        if (elevation == 0.0f || UMath::Abs(myPos.y - elevation) < 1.25f) {
            if (!this->mWasInZoneLastUpdate) {
                this->mWasInZoneLastUpdate = true;

                if (NotSeenRightNow) {
                    this->mHiddenZoneLatchTime = 0.05f;
                } else {
                    this->mHiddenZoneLatchTime = 99999.0f;
                }
            } else if (this->mHiddenZoneLatchTime > 999.0f) {
                this->mHiddenZoneTimer = 0.0f;
            } else {
                this->mHiddenZoneTimer += dT;
            }

            dT = 0.0f;

            bool inZoneLongEnough = this->mHiddenZoneTimer > this->mHiddenZoneLatchTime;

            if (inZoneLongEnough) {
                if (azone->GetData(0) != car_hash) {
                    if (azone->GetData(0) == heli_hash) {
                        this->mHiddenFromHelicopters = true;
                    } else {
                        this->mHiddenFromHelicopters = true;
                        this->mHiddenFromCars = true;
                    }
                } else {
                    this->mHiddenFromCars = true;
                }
            }

            zoneCount++;
        }
    }

    if (zoneCount == 0) {
        this->mWasInZoneLastUpdate = false;
        this->mHiddenZoneTimer = 0.0f;
    }
}

void AIPerpVehicle::Set911CallTime(float time) {
    this->m911CallTimer = bMax(time, this->m911CallTimer);
}

void AIPerpVehicle::OnBehaviorChange(const UCrc32 &mechanic) {
    this->AIVehicle::OnBehaviorChange(mechanic);
}

bool AIPerpVehicle::IsPartiallyHidden(float &HowHidden) const {
    if (this->mHiddenZoneTimer > 0.07f) {
        HowHidden = UMath::Min(1.0f, this->mHiddenZoneTimer / this->mHiddenZoneLatchTime);
        return true;
    }
    HowHidden = 0.0f;
    return false;
}

void AIPerpVehicle::SetCostToState(int cts) {
    this->mCostToState = cts;
}

int AIPerpVehicle::GetCostToState() const {
    return this->mCostToState;
}

float ForcePursuitHeatLevel = 0.0f;

void AIPerpVehicle::SetHeat(float heat) {
    int current = static_cast<int>(this->mHeat);
    int now = static_cast<int>(heat);
    bool useRaceHeatNow = false;

    if (GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing &&
        (GRaceStatus::Get().GetRaceParameters() == nullptr || !GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace())) {
        useRaceHeatNow = true;
    }

    bool raceEventStatusChanged = useRaceHeatNow != this->mWasInRaceEventLastHeatUpdate;
    this->mWasInRaceEventLastHeatUpdate = useRaceHeatNow;

    if (now != current || raceEventStatusChanged) {
        delete this->mPursuitLevelAttrib;
        this->mPursuitLevelAttrib = nullptr;

        delete this->mPursuitSupportAttrib;
        this->mPursuitSupportAttrib = nullptr;
    }

    if (this->mPursuitLevelAttrib == nullptr) {
        int idx = now - 1;

        if (useRaceHeatNow) {
            this->mPursuitLevelAttrib = new Attrib::Gen::pursuitlevels(this->mPursuitEscalationAttrib->racetable(idx), 0, nullptr);
            this->mPursuitSupportAttrib = new Attrib::Gen::pursuitsupport(this->mPursuitEscalationAttrib->supportracetable(idx), 0, nullptr);
        } else {
            this->mPursuitLevelAttrib = new Attrib::Gen::pursuitlevels(this->mPursuitEscalationAttrib->heattable(idx), 0, nullptr);
            this->mPursuitSupportAttrib = new Attrib::Gen::pursuitsupport(this->mPursuitEscalationAttrib->supporttable(idx), 0, nullptr);
        }
    }

    this->mHeat = heat;
}

static const float Tweak_RacerSkill = 0.0f;
static const bool Tweak_OverrideRacerSkill = false;

static const float Tweak_CatchupCheatSkill[3] = {0.5f, 0.5f, 0.5f};
Table CatchupCheatTable(Tweak_CatchupCheatSkill, 3, 0.0f, 1.0f);

float AIPerpVehicle::GetSkill() const {
    return bClamp(this->fBaseSkill + this->fGlueSkill, 0.0f, 1.0f);
}

float AIPerpVehicle::GetCatchupCheat() const {
    return UMath::Clamp(this->fBaseSkill + this->fGlueSkill - 1.0f, 0.0f, 1.0f) * CatchupCheatTable.GetValue(this->fBaseSkill);
}

float AIPerpVehicle::GetHeat() const {
    return this->mHeat;
}

static const int kCostToStatePerHeatBump = 4000;

void AIPerpVehicle::AddCostToState(int cost) {
    if (!GRaceStatus::Exists()) {
        return;
    }
    IPursuit *ip = this->GetPursuit();
    if (ip != nullptr) {
        bool challengeRace = false;
        GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
        if (parms != nullptr) {
            challengeRace = parms->GetRaceType() == GRace::kRaceType_Challenge;
        }
        if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career || challengeRace) {
            this->mCostToState += cost;
            ip->NotifyPropertyDamaged(cost);
        }
    }
}

void AIPerpVehicle::AddToPendingRepPointsNormal(int amount) {
    if (GRaceStatus::Exists()) {
        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming || GRaceStatus::IsChallengeRace()) {
            this->mPendingRepPointsNormal += amount;
        }
    }
}

void AIPerpVehicle::AddToPendingRepPointsFromCopDestruction(int amount) {
    if (GRaceStatus::Exists()) {
        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming || GRaceStatus::IsChallengeRace()) {
            this->mPendingRepPointsFromCopDestruction += amount;
        }
    }
}

bool AIPerpVehicle::IsRacing() const {
    if (this->GetRacerInfo() != nullptr && GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
        AITarget *target = this->GetTarget();
        return target != nullptr && target->IsValid();
    }
    return false;
}

float AIPerpVehicle::GetPercentRaceComplete() const {
    if (this->pRacerInfo != nullptr) {
        return this->pRacerInfo->GetPctRaceComplete();
    }
    return 0.0f;
}

bool AIPerpVehicle::IsBeingPursued() const {
    ISimable *mysimobj = this->GetSimable();
    const IPursuit::List &Pursuits = IPursuit::GetList();

    for (IPursuit::List::const_iterator Pusuit_iter = Pursuits.begin(); Pusuit_iter != Pursuits.end(); ++Pusuit_iter) {
        IPursuit *curpursuit = *Pusuit_iter;
        AITarget *curtarget = curpursuit->GetTarget();
        if (curtarget != nullptr) {
            const ISimable *simobj = curtarget->GetSimable();
            if (simobj == mysimobj) {
                return true;
            }
        }
    }
    return false;
}

void AIPerpVehicle::OnCausedExplosion(IExplosion *explosion, ISimable *to) {
    float chain_start_time = explosion->GetCausalityTime();
    float sim_time = Sim::GetTime();
    int cost_to_state = 0;

    if (sim_time - chain_start_time <= Tweak_MaxAICausalityTime) {
        SimableType type = to->GetSimableType();
        IModel *model = to->GetModel();
        bool bIsRootModel = false;
        if (model != nullptr && model->IsRootModel()) {
            bIsRootModel = true;
        }

        if (type == SIMABLE_SMACKABLE && bIsRootModel) {
            Attrib::Gen::smackable attribs(to->GetAttributes());
            cost_to_state = attribs.COST_TO_STATE();
        } else if (type == SIMABLE_VEHICLE) {
            IVehicle *ivehicle;
            to->QueryInterface(&ivehicle);
            IPursuitAI *ipursuitVehicle;
            if (!ivehicle->IsDestroyed() && to->QueryInterface(&ipursuitVehicle)) {
                cost_to_state = Tweak_CostToStateHittingCop_Explosion;
            }
        }

        if (cost_to_state != 0 && GetPursuit() != nullptr) {
            AddCostToState(cost_to_state);
        }
    }

    to->SetCausality(static_cast<ICause *>(this)->GetInstanceHandle(), chain_start_time);
}

bool AIPerpVehicle::OnClearCausality(float start_time) {
    return false;
}

void AIPerpVehicle::OnCausedCollision(const Sim::Collision::Info &cinfo, ISimable *from, ISimable *to) {
    const float sim_time = Sim::GetTime();
    const bool directhit = UTL::COM::ComparePtr(this->GetOwner(), from);
    const SimableType type = to->GetSimableType();
    const float chain_start_time = directhit ? sim_time : from->GetCausalityTime();
    bool break_chain = false;
    int cost_to_state = 0;
    bool intentionalhit = false;

    if (type == SIMABLE_SMACKABLE &&
        ((to->GetInstanceHandle() == cinfo.objA && cinfo.objAImmobile) || (to->GetInstanceHandle() == cinfo.objB && cinfo.objBImmobile))) {
        return;
    }

    if (sim_time - chain_start_time <= Tweak_MaxAICausalityTime) {
        IPursuit *ipursuit = this->GetPursuit();

        if (type == SIMABLE_SMACKABLE) {
            intentionalhit = directhit;

            Attrib::Gen::smackable attribs(to->GetAttributes());
            cost_to_state = attribs.COST_TO_STATE();

            IModel *model = to->GetModel();
            if (model != nullptr && model->IsRootModel() && directhit && ipursuit != nullptr && ipursuit->IsPerpInSight() &&
                ipursuit->IsPlayerPursuit() && ipursuit->GetMinDistanceToTarget() < 25.0f) {
                GInfractionManager::Get().ReportDamageToProperty();
            }

            IPlayer *player = this->GetOwner()->GetPlayer();
            if (player != nullptr) {
                player->ChargeGameBreaker(Tweak_CTS_GameBreakerRecharge);
            }

            if (this->GetEngine() != nullptr) {
                this->GetEngine()->ChargeNOS(Tweak_CTS_NOSRecharge);
            }
        } else if (type == SIMABLE_VEHICLE) {
            float closing_speed = UMath::Length(cinfo.closingVel);
            bool causalityhit = closing_speed > Tweak_MinAICausalityHitSpeed;

            if (directhit) {
                bool i_am_a = from->GetOwnerHandle() == cinfo.objA;
                float normal_dir = i_am_a ? 1.0f : -1.0f;
                const UMath::Vector3 &my_vel = i_am_a ? cinfo.objAVel : cinfo.objBVel;
                const UMath::Vector3 &his_vel = i_am_a ? cinfo.objBVel : cinfo.objAVel;
                float his_closing_speed = normal_dir * UMath::Dot(his_vel, cinfo.normal);
                float my_closing_speed = normal_dir * -UMath::Dot(my_vel, cinfo.normal);
                intentionalhit = my_closing_speed > his_closing_speed;
            }

            IVehicle *ivehicle;
            to->QueryInterface(&ivehicle);

            IPursuitAI *ipursuitVehicle;
            if (to->QueryInterface(&ipursuitVehicle)) {
                bool wasDamagedByPerp = ipursuitVehicle->GetDamagedByPerp();

                if (!wasDamagedByPerp && directhit && causalityhit) {
                    ipursuitVehicle->SetDamagedByPerp(true);

                    if (ipursuit != nullptr) {
                        ipursuit->NotifyCopDamaged(ivehicle);

                        if (ipursuit->IsPlayerPursuit() && intentionalhit) {
                            GInfractionManager::Get().ReportAssaultingPoliceOfficer();
                        }
                    }
                }

                if (!ivehicle->IsDestroyed()) {
                    if (intentionalhit) {
                        cost_to_state = Tweak_CostToStateHittingCop_Direct;
                    } else if (!directhit) {
                        cost_to_state = Tweak_CostToStateHittingCop_Indirect;
                    }

                    if (cost_to_state != 0) {
                        float amount = UMath::Ramp(closing_speed, Tweak_MinAICausalityHitSpeed, Tweak_MaxAICausalityHitSpeed);
                        cost_to_state = static_cast<int>(static_cast<float>(cost_to_state / 50) * amount) * 50;
                        cost_to_state = UMath::Max(50, cost_to_state);
                    }

                    IVehicleAI *ivehicleai;
                    if (directhit && !ipursuitVehicle->GetInPursuit() && to->QueryInterface(&ivehicleai) && !ivehicleai->GetTarget()->IsValid()) {
                        DriverClass driverclass = this->GetVehicle()->GetDriverClass();

                        if (driverclass == DRIVER_HUMAN || driverclass == DRIVER_REMOTE || (ICopMgr::Exists() && ICopMgr::Get()->CanPursueRacers())) {
                            if (intentionalhit) {
                                ivehicleai->GetTarget()->Aquire(from);
                            }
                        }
                    }
                }
            }

            ITrafficAI *itrafficVehicle;
            if (causalityhit && intentionalhit && to->QueryInterface(&itrafficVehicle)) {
                this->LastTrafficHitTime = sim_time;
                GManager::Get().IncValue("insurance_claims");

                if (ipursuit != nullptr) {
                    ipursuit->NotifyTrafficCarHit();
                }

                if (GRaceStatus::Exists()) {
                    GRacerInfo *racerInfo = GRaceStatus::Get().GetRacerInfo(from);
                    if (racerInfo != nullptr) {
                        racerInfo->NotifyTrafficCollision();
                    }
                }

                if (intentionalhit && ipursuit != nullptr && ipursuit->IsPerpInSight() && ipursuit->IsPlayerPursuit() &&
                    ipursuit->GetMinDistanceToTarget() < 25.0f) {
                    GInfractionManager::Get().ReportHitAndRun();
                }
            }

            break_chain = !intentionalhit;
        }

        if (cost_to_state != 0 && ipursuit != nullptr) {
            this->AddCostToState(cost_to_state);
        }
    }

    to->SetCausality(static_cast<ICause *>(this)->GetInstanceHandle(), break_chain ? 0.0f : chain_start_time);
}

float AIPerpVehicle::GetLastTrafficHitTime() const {
    return this->LastTrafficHitTime;
}
