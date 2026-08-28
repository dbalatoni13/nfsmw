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
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aivehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/collisionreactions.h"
#include "Speed/Indep/Src/Generated/Events/EEnableAIPhysics.hpp"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
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
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Src/World/WRoadElem.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

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

AIVehicleEmpty::AIVehicleEmpty(const BehaviorParams &bp) : AIVehicle(bp, 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE) {}

Behavior *AIVehicleEmpty::Construct(const BehaviorParams &bp) {
    return new AIVehicleEmpty(bp);
}

AIVehicleHuman::AIVehicleHuman(const BehaviorParams &bp) : AIVehicleRacecar(bp), IHumanAI(bp.fowner) {
    this->MakeDebugable(DBG_AI);
    this->fMomentRadius = 0.0f;
    this->bAiControl = false;
    this->mWrongWay = false;
}

Behavior *AIVehicleHuman::Construct(const BehaviorParams &bp) {
    return new AIVehicleHuman(bp);
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
        this->SetGoal("AIGoalRacer");
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

bool bToggleAiControl;

float aHumanNavLookAheadData[2] = {50.0f, 60.0f};
Table HumanNavLookAheadTable(aHumanNavLookAheadData, 2, 0.0f, 100.0f);
float aHumanDragNavLookAheadData[2] = {8.0f, 40.0f};
Table HumanDragNavLookAheadTable(aHumanDragNavLookAheadData, 2, 0.0f, 100.0f);

void AIVehicleHuman::Update(float dT) {
    ProfileNode profile_node("TODO", 0);

    if (bToggleAiControl) {
        this->SetAiControl(!this->GetAiControl());
        bToggleAiControl = false;
    }

    this->UpdateWrongWay();

    if (this->GetAiControl()) {
        InputControls controls = this->GetInput()->GetControls();
        this->AIVehicle::Update(dT);
        return;
    }

    this->AIVehicle::Update(dT);

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
        } else if (distance_to_nav > 70.0f) {
            reset_nav = true;
        }

        road_nav->UpdateOccludedPosition(!this->IsDragRacing());
    }

    if (!reset_nav) {
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
      mAvoidableRadius(20.0f),                                  //
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
            case DRIVER_TRAFFIC:
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
    this->mThinkTask = this->AddTask("AIVehicle", update_rate, stagger, taskmode);
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
    return Sim::GetTime() - start_time > 2.0f;
}

float AIVehicle::GetAcceleration(float at) const {
    if (this->mTopSpeed > 0.0f) {
        Table table(this->mAccelData, 10, 0.0f, this->mTopSpeed);
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
            Attrib::Gen::collisionreactions reactions((Attrib::Collection *)nullptr, 0, nullptr);

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
    if ((yaw < -1.5707964f && this->mDampedAngle.GetPosition() > 1.5707964f) ||
        (yaw > 1.5707964f && this->mDampedAngle.GetPosition() < -1.5707964f)) {
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
    UMath::ScaleAdd(this->mCollisionBody->GetLinearVelocity(), 0.65f, this->mCollisionBody->GetPosition(), position);
    this->mDrivableToNav = !this->WorldCollision(position, pdrivetonav->GetPosition());
    UMath::Sub(pdrivetonav->GetPosition(), this->mCollisionBody->GetPosition(), this->mDirToNav);
    UMath::Unit(this->mDirToNav, this->mDirToNav);
}

void AIVehicle::OnReverse(float dT) {
    if (!(this->mDriveFlags & 4) || this->GetReverseOverride() || this->mITransmission == nullptr) {
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
    UMath::Sub(this->mDest, this->mCollisionBody->GetPosition(), dirVector);
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

    if (this->mDriveSpeed != 0.0f || currentSpeed >= 1.0f) {
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
}

// UNSOLVED
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

            // TODO
            if (in_neutral && !in_shock || in_shock) {
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

inline void AIVehicle::SetReverseOverride(float time) {
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

void AIVehicle::OnCollision(const COLLISION_INFO &cinfo) {}

WRoadNav *AIVehicle::GetCollNav(const UMath::Vector3 &forwardVector, float predictTime) {
    this->mCollNav->SetNavType(WRoadNav::kTypeDirection);

    if (predictTime > 0.0f) {
        UMath::Matrix4 orientMat = this->GetOrientation();
        UMath::Vector3 predictionresult;

        AI::Math::PredictPosition(predictTime, this->GetPosition(), orientMat, this->GetLinearVelocity(), this->GetAngularVelocity(),
                                  predictionresult);
        this->mCollNav->InitAtPoint(predictionresult, forwardVector, false, 0.0f);
    } else {
        this->mCollNav->InitAtPoint(this->mCollisionBody->GetPosition(), forwardVector, false, 0.0f);
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
        ipai->SetSupportGoal((const char *)nullptr);
    }
}

bool AIVehicle::CanRespawn(bool respawnAvailable) {
    if (!respawnAvailable) {
        this->mCanRespawn = true;
    }
    bool rv = false;
    if (this->mCanRespawn) {
        rv = this->mLastSpawnTime > 8.0f;
    }
    if (!rv && respawnAvailable && this->mLastSpawnTime > 10.0f) {
        rv = true;
    }
    return rv;
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

AIPerpVehicle::AIPerpVehicle(const BehaviorParams &bp)
    : AIVehiclePid(bp, 0.5f, mStagger, Sim::TASK_FRAME_FIXED), //
      IPerpetrator(bp.fowner),                                 //
      ICause(bp.fowner),                                       //
      ICheater(bp.fowner),                                     //
      LastTrafficHitTime(-1.0f),                               //
      mHeat(0.0f),                                             //
      mCostToState(0),                                         //
      mPendingRepPointsNormal(0),                              //
      mPendingRepPointsFromCopDestruction(0),                  //
      mHiddenFromCars(false),                                  //
      mHiddenFromHelicopters(false),                           //
      mWasInRaceEventLastHeatUpdate(false),                    //
      mHiddenZoneTimer(0.0f),                                  //
      mWasInZoneLastUpdate(false),                             //
      mPursuitZoneCheck(0),                                    //
      pRacerInfo(nullptr),                                     //
      fBaseSkill(0.5f),                                        //
      fGlueSkill(0.0f),                                        //
      fGlueOutput(0.0f),                                       //
      m911CallTimer(0.0f) {
    mStagger += 0.5f;
    if (mStagger > 1.0f) {
        mStagger = 0.0f;
    }
    // default
    this->mPursuitEscalationAttrib = new Attrib::Gen::pursuitescalation(0xeec2271a, 0, nullptr);
    this->mPursuitLevelAttrib = nullptr;
    this->mPursuitSupportAttrib = nullptr;
    this->SetHeat(1.0f);
    this->GetOwner()->SetCausality(this->GetInstanceHandle(), 0.0f);
    this->mHiddenZoneLatchTime = 0.05f;
    this->pGlueError = new (nullptr) PidError(10, 5, 1.0f); // TODO name
    this->fGlueTimer = bRandom(1.0f);
}

AIPerpVehicle::~AIPerpVehicle() {
    delete this->mPursuitLevelAttrib;
    delete this->mPursuitSupportAttrib;
    delete this->mPursuitEscalationAttrib;
    delete this->pGlueError;
}

void AIPerpVehicle::SetRacerInfo(GRacerInfo *info) {
    this->pRacerInfo = info;
    this->ComputeSkill();
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

float AIPerpVehicle::GetSkill() const {
    return bClamp(this->fBaseSkill + this->fGlueSkill, 0.0f, 1.0f);
}

static const float Tweak_CatchupCheatSkill[3] = {0.5f, 0.5f, 0.5f};
Table CatchupCheatTable(Tweak_CatchupCheatSkill, 3, 0.0f, 1.0f);

float AIPerpVehicle::GetCatchupCheat() const {
    return UMath::Clamp(this->fBaseSkill + this->fGlueSkill - 1.0f, 0.0f, 1.0f) * CatchupCheatTable.GetValue(this->fBaseSkill);
}

float AIPerpVehicle::GetHeat() const {
    return this->mHeat;
}

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

bool AIPerpVehicle::OnClearCausality(float start_time) {
    return false;
}

float AIPerpVehicle::GetLastTrafficHitTime() const {
    return this->LastTrafficHitTime;
}
