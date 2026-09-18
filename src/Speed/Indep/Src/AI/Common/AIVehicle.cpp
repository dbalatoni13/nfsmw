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
#include "Speed/Indep/Src/Gameplay/GCharacter.h"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aivehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/collisionreactions.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Generated/Events/EEnableAIPhysics.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/PVehicle.h"
extern Table HumanDragNavLookAheadTable;
extern Table HumanNavLookAheadTable;
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
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WRoadElem.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <algorithm>
#include <cfloat>
#include <set>
#include <vector>

#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"

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

BIND_BEHAVIOR_FACTORY(AIVehicleEmpty)

AIVehicleHuman::AIVehicleHuman(const BehaviorParams &bp) : AIVehicleRacecar(bp), IHumanAI(bp.fowner) {
    MakeDebugable(DBG_AI);
    fMomentRadius = 0.0f;
    bAiControl = false;
    mWrongWay = false;
}

Behavior *AIVehicleHuman::Construct(const BehaviorParams &bp) {
    return new AIVehicleHuman(bp);
}

BIND_BEHAVIOR_FACTORY(AIVehicleHuman)

AIVehicleHuman::~AIVehicleHuman() {
    int player_num = 0;
    IPerpetrator *ip;
    if (GetSimable()->QueryInterface(&ip)) {
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
                if (stable) {
                    FECarRecord *fe_car = stable->GetCarByIndex(career->GetCurrentCar());
                    if (fe_car) {
                        FECareerRecord *fe_career = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
                        if (fe_career) {
                            fe_career->SetVehicleHeat(Heat);
                        }
                    }
                }
            }
        }
    }
}

void AIVehicleHuman::UpdateWrongWay() {
    mWrongWay = false;
    IVehicle *vehicle = GetVehicle();
    if (vehicle->GetPhysicsMode() != PHYSICS_MODE_SIMULATED || vehicle->IsAnimating() || vehicle->IsStaging()) {
        return;
    }
    if (!GRaceStatus::Exists()) {
        return;
    }
    if (GRaceStatus::Get().GetPlayMode() != GRaceStatus::kPlayMode_Racing || !GRaceStatus::Get().GetActivelyRacing()) {
        return;
    }
    WRoadNav *road_nav = GetDriveToNav();
    if (!road_nav) {
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
        if (seg && seg->IsInRace()) {
            UMath::Vector3 fwd = nav.GetForwardVector();
            if (UMath::Dot(fwd, drive_dir) < 0.0f) {
                mWrongWay = true;
            }
        }
    }
}

void AIVehicleHuman::SetAiControl(bool ai_control) {
    if (bAiControl != ai_control) {
        ClearGoal();
        SetGoal("AIGoalRacer");
        bAiControl = ai_control;
        new EEnableAIPhysics(reinterpret_cast<uintptr_t>(GetOwner()->GetInstanceHandle()), GetVehicle()->GetSpeed(), ai_control ? 1 : 0);
    }
}

bool AIVehicleHuman::IsDragRacing() {
    return GetVehicle()->GetDriverStyle() == STYLE_DRAG;
}

bool AIVehicleHuman::IsDragSteering() {
    if (!IsDragRacing()) {
        return false;
    }
    if (GetVehicle()->GetSpeed() < 1.0f) {
        return false;
    }
    IPlayer *player = GetOwner()->GetPlayer();
    if (player && player->InGameBreaker()) {
        return false;
    }
    return mWrongWay == false;
}

// AIVehicleHuman::IsPlayerSteering esta EN LA CLASE (AIVehicle.h): el objetivo
// la emite con las inline de finish_file, no aqui.

void AIVehicleHuman::ChangeDragLanes(bool left) {
    if (!IsDragSteering()) {
        return;
    }
    WRoadNav *road_nav = GetDriveToNav();
    if (!road_nav) {
        return;
    }
    road_nav->ChangeDragLanes(left ? -1 : 1);
}

void AIVehicleHuman::OnDebugDraw() {}

bool bToggleAiControl = 0;

void AIVehicleHuman::Update(float dT) {
    ProfileNode profile_node;

    if (bToggleAiControl) {
        SetAiControl(!GetAiControl());
        bToggleAiControl = false;
    }

    UpdateWrongWay();

    if (GetAiControl()) {
        InputControls controls = GetInput()->GetControls();
        AIVehicleRacecar::Update(dT);
        return;
    }

    AIPerpVehicle::Update(dT);

    UMath::Vector3 car_forward_vector;
    GetVehicle()->ComputeHeading(&car_forward_vector);

    IRigidBody *rigid_body = GetSimable()->GetRigidBody();
    float current_speed = rigid_body->GetSpeed();
    Table &nav_look_ahead_table = IsDragRacing() ? HumanDragNavLookAheadTable : HumanNavLookAheadTable;

    bool reset_nav = false;
    WRoadNav *road_nav = GetDriveToNav();
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

        road_nav->UpdateOccludedPosition(!IsDragRacing());
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
        ResetDriveToNav(SELECT_VALID_LANE);
        road_nav->SetNavType(WRoadNav::kTypeDirection);
        float look_ahead = nav_look_ahead_table.GetValue(current_speed);
        road_nav->IncNavPosition(look_ahead, car_forward_vector, 0.0f);
        road_nav->UpdateOccludedPosition(true);
    }

    if (GRaceStatus::Exists()) {
        if ((GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming || GRaceStatus::Get().GetActivelyRacing()) &&
            road_nav->GetNavType() != WRoadNav::kTypePath && !road_nav->FindingPath() && road_nav->IsValid()) {
            AITarget *target = GetTarget();
            if (target->IsValid()) {
                road_nav->FindPath(&target->GetPosition(), &target->GetDirection(), nullptr);
            }
        }
    }

    if (IsDragRacing()) {
        road_nav->SetLaneType(WRoadNav::kLaneDrag);
        if (IsDragSteering()) {
            DoSteering();
            SetDriveTarget(road_nav->GetPosition());
        } else {
            road_nav->DetermineDragLane();
        }
    } else {
        road_nav->SetLaneType(WRoadNav::kLaneRacing);
    }

    if (GetPursuit() && GetPursuit()->IsPerpInSight() && GetPursuit()->IsPlayerPursuit()) {
        if (!IsOnLegalRoad()) {
            if (GetPursuit()->GetMinDistanceToTarget() < 25.0f) {
                GInfractionManager::Get().ReportDrivingOffRoadWay();
            }
        }
    }
}

Behavior *AIVehicle::Construct(const BehaviorParams &bp) {
    return new AIVehicle(bp, 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE);
}

BIND_BEHAVIOR_FACTORY(AIVehicle)

AIVehicle::AIVehicle(const BehaviorParams &bp, float update_rate, float stagger, Sim::TaskMode taskmode)
    : VehicleBehavior(bp, 0),                                   //
      IVehicleAI(bp.fowner),                                    //
      AIAvoidable(GetOwner()),                                  //
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
    EnableProfile("AIVehicle");
    GetOwner()->QueryInterface(&mCollisionBody);

    const Attrib::Gen::pvehicle &pvehicle = GetVehicle()->GetVehicleAttributes();
    bMemSet(mAccelData, 0, sizeof(mAccelData));
    Physics::Info::ComputeAccelerationTable(pvehicle, mTopSpeed, mAccelData, sizeof(mAccelData) / sizeof(mAccelData[0]));

    mAttributes = new Attrib::Gen::aivehicle(pvehicle.aivehicle(), 0, nullptr);
    mDriveToNav = new WRoadNav();
    mDrivableToNav = false;

    ISimable *isimable = GetSimable();
    IRigidBody *irigidbody = isimable->GetRigidBody();
    const UMath::Vector3 &rigidBodyPos = irigidbody->GetPosition();
    SetDriveTarget(rigidBodyPos);

    bool cookie_trail = false;
    bool decision_filter = false;
    IVehicle *v = GetVehicle();
    WRoadNav::EPathType path_type = WRoadNav::kPathNone;

    if (v->GetVehicleClass() == VehicleClass::CHOPPER) {
        path_type = WRoadNav::kPathChopper;
    } else {
        switch (GetVehicle()->GetDriverClass()) {
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
    mDriveToNav->SetVehicle(this);
    mDriveToNav->SetPathType(path_type);
    mDriveToNav->SetCookieTrail(cookie_trail);
    mDriveToNav->SetDecisionFilter(decision_filter);

    mTarget = new AITarget(GetSimable());
    mCollNav = new WRoadNav();

    MakeDebugable(DBG_AI);
    ResetInternals();
    mThinkTask = AddTask("AIVehicle", update_rate, stagger, taskmode);
    Sim::ProfileTask(mThinkTask, "AI Think");

    const char *layer_name = GetCaffeineLayerName(GetVehicle()->GetDriverClass());
}

AIVehicle::~AIVehicle() {
    delete mDriveToNav;
    delete mTarget;
    delete mCollNav;
    delete mCurrentGoal;
    if (mThinkTask) {
        RemoveTask(mThinkTask);
    }
    delete mAttributes;
}

void AIVehicle::ResetInternals() {
    GetOwner()->QueryInterface(&mITransmission);
    GetOwner()->QueryInterface(&mISuspension);
    GetOwner()->QueryInterface(&mIEngine);
    GetOwner()->QueryInterface(&mIInput);
    ClearGoal();
    mLastSpawnTime = 0.0f;
    mCanRespawn = false;
    mDrivableToNav = false;
    mReverseOverrideTimer = 0.0f;
    mReverseOverrideSteer = 0.0f;
    mReverseOverrideDirection = false;
    mDrivableToTargetPos = false;
    mDestSegment = -1;
    mReversingSpeed = false;
    mSteeringBehind = false;
}

void AIVehicle::OnTaskSimulate(float dT) {
    if (GetVehicle()->IsActive() && mDriveFlags != 0) {
        OnDriving(dT);
    }
}

bool AIVehicle::OnClearCausality(float start_time) {
    return Sim::GetTime() - start_time > 2.0f;
}

float AIVehicle::GetAcceleration(float at) const {
    if (mTopSpeed > 0.0f) {
        Table table(mAccelData, 10, 0.0f, mTopSpeed);
        return table.GetValue(UMath::Abs(at));
    } else {
        return 0.0f;
    }
}

bool AIVehicle::OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) {
    if (mAvoidableRadius > 0.0f && GetVehicle()->IsActive()) {
        IRigidBody *rb = GetOwner()->GetRigidBody();
        if (rb) {
            sweep = UMath::Max(mAvoidableRadius, 2.0f * rb->GetSpeed());
            pos = rb->GetPosition();
            return true;
        }
    }
    return false;
}

void AIVehicle::DoNOS() {
    bool wantnos = false;
    bool isnos = GetInput()->GetControls().fNOS;
    float myspeed = GetVehicle()->GetSpeed();
    float desiredspeed = GetDriveSpeed();

    if (desiredspeed > (isnos ? KPH2MPS(90.0f) : KPH2MPS(120.0f))) {
        if (myspeed > (isnos ? KPH2MPS(50.0f) : KPH2MPS(80.0f))) {
            if ((desiredspeed - myspeed) > (isnos ? KPH2MPS(10.0f) : KPH2MPS(60.0f))) {
                IRigidBody *ibody;
                if (GetVehicle()->QueryInterface(&ibody)) {
                    UMath::Vector3 driveoff = GetDriveTarget() - GetVehicle()->GetPosition();
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
    GetInput()->SetControlNOS(wantnos);
}

bool AIVehicle::OnTask(HSIMTASK hTask, float dT) {
    ProfileNode profile_node;
    if (hTask == mThinkTask) {
        if (IsPaused() || TheOnlineManager.GetState() == OLS_RACE_END) {
            return true;
        }
        ClearDriveFlags();
        if (GetVehicle()->IsActive()) {
            Update(dT);
        }
        HCAUSE hcause = GetOwner()->GetCausality();
        if (hcause) {
            float start_time = GetOwner()->GetCausalityTime();
            if (OnClearCausality(start_time)) {
                GetOwner()->SetCausality(nullptr, 0.0f);
            }
        }
        return true;
    } else {
        Sim::Object::OnTask(hTask, dT);
        return false;
    }
}

void AIVehicle::OnOwnerAttached(IAttachable *pOther) {
    IPursuit *ipursuit;
    if (!mPursuit && pOther->QueryInterface(&ipursuit)) {
        mPursuit = ipursuit;
    }
    IRoadBlock *iroadblock;
    if (!mRoadBlock && pOther->QueryInterface(&iroadblock)) {
        mRoadBlock = iroadblock;
    }
    Behavior::OnOwnerAttached(pOther);
}

void AIVehicle::OnOwnerDetached(IAttachable *pOther) {
    if (UTL::COM::ComparePtr(pOther, mPursuit)) {
        mPursuit = nullptr;
    }
    if (UTL::COM::ComparePtr(pOther, mRoadBlock)) {
        mRoadBlock = nullptr;
    }
    Behavior::OnOwnerDetached(pOther);
}

void AIVehicle::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        GetOwner()->QueryInterface(&mIEngine);
        GetOwner()->QueryInterface(&mITransmission);
    }
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mISuspension);
    }
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mCollisionBody);
    }
    if (mCurrentGoal) {
        mCurrentGoal->OnBehaviorChange(mechanic);
    }
    Behavior::OnBehaviorChange(mechanic);
}

void AIVehicle::ClearGoal() {
    if (mCurrentGoal) {
        delete mCurrentGoal;
        mCurrentGoal = nullptr;
    }
    mGoalName = UCrc32::kNull;
}

void AIVehicle::SetGoal(const UCrc32 &name) {
    if (mGoalName == name) {
        return;
    }
    delete mCurrentGoal;
    mGoalName = name;
    mCurrentGoal = AIGoal::CreateInstance(name, GetOwner());
    DriverClass driverclass = GetVehicle()->GetDriverClass();
    // TODO, this means the first 4
    if (driverclass >= DRIVER_TRAFFIC && driverclass <= DRIVER_RACER) {
        IRBVehicle *ivehiclebody;
        if (GetOwner()->QueryInterface(&ivehiclebody)) {
            const Attrib::Gen::aivehicle &attributes = GetAttributes();
            unsigned int num_reactions = attributes.Num_PlayerCollisions();
            bool found = false;
            Attrib::Gen::collisionreactions reactions((Attrib::Collection *)nullptr, 0, nullptr);

            for (size_t i = 0; i < num_reactions; i++) {
                const AICollisionReactionRecord &record = attributes.PlayerCollisions(i);
                if (record.Goal == mGoalName.GetValue()) {
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

// No es un `||`: el mapa de lineas del original pone la segunda condicion CUATRO
// lineas debajo de la primera (1084 y 1088), o sea una cadena `else if` con el
// cuerpo REPETIDO. El binario es el mismo -- el cross-jumping de la pasada
// `jump2` (posterior a reload) funde los dos `SetPosition` --, pero el reparto de
// registros NO: con el cuerpo repetido `yaw` pasa de 5 a 6 referencias y
// local-alloc le da f31 en vez de f30. Con el `||` la funcion se quedaba en
// 99,33% con f30/f31 cruzados.
void AIVehicle::Update(float dT) {
    IRigidBody *rb = GetOwner()->GetRigidBody();
    UMath::Vector3 vfwd;
    rb->GetForwardVector(vfwd);

    float yaw = UMath::Atan2r(vfwd.x, vfwd.z);
    mDampedAngularVel.Integrate(rb->GetAngularVelocity().y, dT);
    if (yaw < -1.5707964f && mDampedAngle.GetPosition() > 1.5707964f) {
        mDampedAngle.SetPosition(yaw);
    }

    else if (yaw > 1.5707964f && mDampedAngle.GetPosition() < -1.5707964f) {
        mDampedAngle.SetPosition(yaw);
    }
    else {
        mDampedAngle.Integrate(yaw, dT);
    }

    UpdateSimplePhysics(dT);
}

void AIVehicle::ResetDriveToNav(eLaneSelection lane_selection) {
    bool force_center_lane = lane_selection == SELECT_CENTER_LANE;
    UMath::Vector3 forwardVector;
    GetVehicle()->ComputeHeading(&forwardVector);

    IRigidBody *irb = GetSimable()->GetRigidBody();

    const float dir_weight = 1.0f;
    WRoadNav *road_nav = GetDriveToNav();
    road_nav->InitAtPoint(irb->GetPosition(), forwardVector, force_center_lane, dir_weight);
    road_nav->CancelPathFinding();

    if (lane_selection == SELECT_VALID_LANE) {
        road_nav->SnapToSelectableLane();
    }
    UpdateRoadNavInfo();
}

bool AIVehicle::ResetVehicleToRoadNav(WRoadNav *other_nav) {
    WRoadNav *road_nav = GetDriveToNav();
    road_nav->InitFromOtherNav(other_nav, false);
    bool success = GetVehicle()->SetVehicleOnGround(road_nav->GetPosition(), road_nav->GetForwardVector());

    UpdateRoadNavInfo();
    return success;
}

bool AIVehicle::ResetVehicleToRoadNav(short segInd, char laneInd, float timeStep) {
    WRoadNav *pdrivetonav = GetDriveToNav();
    pdrivetonav->InitAtSegment(segInd, laneInd, timeStep);
    bool success = GetVehicle()->SetVehicleOnGround(pdrivetonav->GetPosition(), pdrivetonav->GetForwardVector());

    UpdateRoadNavInfo();
    return success;
}

bool AIVehicle::ResetVehicleToRoadPos(const UMath::Vector3 &position, const UMath::Vector3 &forwardVector) {
    const float dir_weight = 1.0f;
    const bool force_centre_lane = false;

    GetDriveToNav()->InitAtPoint(position, forwardVector, force_centre_lane, dir_weight);
    bool success = GetVehicle()->SetVehicleOnGround(position, forwardVector);

    UpdateRoadNavInfo();
    return success;
}

void AIVehicle::UpdateRoadNavInfo() {
    WRoadNav *pdrivetonav = GetDriveToNav();

    if (!pdrivetonav->IsValid()) {
        return;
    }

    UMath::Vector3 position;
    UMath::ScaleAdd(mCollisionBody->GetLinearVelocity(), 0.65f, mCollisionBody->GetPosition(), position);
    mDrivableToNav = !WorldCollision(position, pdrivetonav->GetPosition());
    UMath::Sub(pdrivetonav->GetPosition(), mCollisionBody->GetPosition(), mDirToNav);
    UMath::Unit(mDirToNav, mDirToNav);
}

void AIVehicle::OnReverse(float dT) {
    if (!(mDriveFlags & 4) || GetReverseOverride() || !mITransmission) {
        return;
    }
    bool wasReversing = mITransmission->IsReversing();

    if (!wasReversing) {
        if (GetVehicle()->GetSpeed() >= 15.0f) {
            mReversingSpeed = false;
            return;
        }
    }

    mReversingSpeed = true;

    UMath::Vector3 dirVector;
    UMath::Sub(mDest, mCollisionBody->GetPosition(), dirVector);
    UMath::Unit(dirVector, dirVector);

    const UMath::Vector3 &forwardVector = GetForwardVector();
    float facingDot = UMath::Dot(forwardVector, dirVector);

    if (mITransmission->IsReversing() && facingDot > 0.0f) {
        mITransmission->Shift(G_FIRST);
    } else {
        if (!mITransmission->IsReversing() && facingDot < -0.707f) {
            mITransmission->Shift(G_REVERSE);
        }
    }
}

float AIVehicle::GetOverSteerCorrection(float steer) {
    // TODO the dwarf suggests something much more complicated
    return 0.0f;
}

void AIVehicle::OnSteering(float dT) {
    if ((mDriveFlags & 1) == 0 || !GetInput()) {
        return;
    }

    GetInput()->SetControlSteering(0.0f);
    GetInput()->SetControlSteeringVertical(0.0f);

    float currentSpeed = GetSimable()->GetRigidBody()->GetSpeedXZ();

    if (mDriveSpeed != 0.0f || currentSpeed >= 1.0f) {
        if (GetVehicle()->GetDriverClass() == 1) {
            WRoadNav *road_nav = GetDriveToNav();
            road_nav->UpdateOccludedPosition(true);
            SetDriveTarget(road_nav->GetOccludedPosition());
        }

        UMath::Vector3 dirVector;
        UMath::Sub(mDest, GetPosition(), dirVector);
        dirVector.y = 0.0f;
        UMath::Unit(dirVector, dirVector);

        UMath::Vector3 forwardVector;
        forwardVector = GetForwardVector();
        forwardVector.y = 0.0f;
        UMath::Unit(forwardVector, forwardVector);

        UMath::Vector3 steerProd;
        UMath::Cross(forwardVector, dirVector, steerProd);
        steerProd.y = UMath::Clamp(steerProd.y, -1.0f, 1.0f);
        float steer = asinf(steerProd.y);

        steer /= ANGLE2RAD(GetSuspension()->GetMaxSteering());
        float steerCorrection = GetOverSteerCorrection(steer);

        mSteeringBehind = false;
        if (mITransmission && mITransmission->IsReversing()) {
            steer = -steer;
        } else {
            if (UMath::Dot(dirVector, forwardVector) < -0.2f) {
                steer = (steer < 0.0f) ? -1.0f : 1.0f;
                mSteeringBehind = true;
            } else {
                if (GetInput()->GetControls().fHandBrake == 0.0f) {
                    steer += steerCorrection;
                }
            }
        }

        steer = UMath::Clamp(steer, -1.0f, 1.0f);
        GetInput()->SetControlSteering(steer);
    }
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
    OnReverse(dT);
    OnSteering(dT);
    OnGasBrake(dT);
}

float AIVehicle::GetPathDistanceRemaining() {
    float distance = 0.0f;
    WRoadNav *road_nav = GetDriveToNav();
    if (road_nav) {
        const UMath::Vector3 &car_position = GetSimable()->GetRigidBody()->GetPosition();
        float path_distance_remaining = road_nav->GetPathDistanceRemaining();

        if (path_distance_remaining == 0.0f) {
            AITarget *target = GetTarget();
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
    mSteeringBehind = false;
    mReversingSpeed = false;
    mReverseOverrideTimer = 0.0f;
}

void AIVehicle::SetReverseOverride(float time) {
    if (mITransmission) {
        mReverseOverrideTimer = time;
        if (mITransmission->IsReversing()) {
            mITransmission->Shift(G_FIRST);
        } else {
            mITransmission->Shift(G_REVERSE);
        }
    }
}

void AIVehicle::UpdateReverseOverride(float dT) {
    if (mReverseOverrideTimer > 0.0f && (mReverseOverrideTimer -= dT) <= 0.0f) {
        mReverseOverrideTimer = 0.0f;
        mSteeringBehind = false;
        if (mITransmission && mITransmission->IsReversing()) {
            mITransmission->Shift(G_FIRST);
        }
    }
}

float AIVehicle::GetLookAhead() {
    IRigidBody *irigidbody = GetSimable()->GetRigidBody();

    UMath::Vector3 dimension;
    irigidbody->GetDimension(dimension);

    float currentSpeed = irigidbody->GetSpeedXZ();
    float lookAhead = UMath::Min(15.0f, UMath::Max(dimension.z + 7.5f, currentSpeed * 0.5f));

    return lookAhead;
}

void AIVehicle::UpdateTargeting() {
    if (!mTarget->IsValid()) {
        return;
    }
    ProfileNode profile_node;
    mDrivableToTargetPos = !WorldCollision(GetPosition(), mTarget->GetPosition());
}

bool AIVehicle::WorldCollision(const UMath::Vector3 &pos, const UMath::Vector3 &dest) {
    if (UMath::DistanceSquare(pos, dest) > 40000.0f) {
        return true;
    }

    UMath::Vector4 segs[2];
    segs[0] = UMath::Vector4Make(pos, 1.0f);
    segs[0].y += 0.5f;
    segs[1] = UMath::Vector4Make(dest, 1.0f);
    segs[1].y += 0.5f;

    WCollisionMgr::WorldCollisionInfo cinfo;
    if (WCollisionMgr(0, 3).CheckHitWorld(segs, cinfo, 2)) {
        if (UMath::DistanceSquarexyz(segs[1], cinfo.fCollidePt) > 0.5f) {
            return true;
        }
    }
    return false;
}

void AIVehicle::OnCollision(const COLLISION_INFO &cinfo) {}

bool AIVehicle::GetWorldAvoidanceInfo(float dT, UMath::Vector3 &leftCollNormal, UMath::Vector3 &rightCollNormal) const {
    if (mITransmission && mITransmission->IsReversing()) {
        return false;
    }

    IRigidBody *irb = GetSimable()->GetRigidBody();
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
        float collisionScale = irb->GetRadius() + irb->GetSpeedXZ() * 0.25f;
        UMath::ScaleAdd(collVec, UMath::Max(collisionScale, minDistance), position, collPos);

        UMath::Vector4 posToDest[2];
        posToDest[0] = UMath::Vector4Make(boundPos, 1.0f);
        posToDest[1] = UMath::Vector4Make(collPos, 1.0f);

        WCollisionMgr::WorldCollisionInfo cInfo;
        if (WCollisionMgr(0, 3).CheckHitWorld(posToDest, cInfo, 2)) {
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
    mCollNav->SetNavType(WRoadNav::kTypeDirection);

    if (predictTime > 0.0f) {
        UMath::Matrix4 orientMat = GetOrientation();
        UMath::Vector3 predictionresult;

        AI::Math::PredictPosition(predictTime, GetPosition(), orientMat, GetLinearVelocity(), GetAngularVelocity(), predictionresult);
        mCollNav->InitAtPoint(predictionresult, forwardVector, false, 0.0f);
    } else {
        mCollNav->InitAtPoint(mCollisionBody->GetPosition(), forwardVector, false, 0.0f);
    }

    return mCollNav;
}

void AIVehicle::SetSpawned() {
    ResetInternals();
    IDamageable *idamage;
    if (GetSimable()->QueryInterface(&idamage)) {
        idamage->ResetDamage();
    }
    EventSequencer::IEngine *ievents = GetOwner()->GetEventSequencer();
    if (ievents) {
        ievents->Reset(Sim::GetTime());
    }
    IArticulatedVehicle *iarticulation;
    if (GetOwner()->QueryInterface(&iarticulation)) {
        IVehicle *itrailer = iarticulation->GetTrailer();
        IVehicleAI *iai;
        if (itrailer && itrailer->QueryInterface(&iai)) {
            iai->SetSpawned();
        }
    }
    IAIHelicopter *ih;
    if (GetOwner()->QueryInterface(&ih)) {
        ih->SetFuelFull();
    }
    mCanRespawn = false;
}

void AIVehicle::UnSpawn() {
    IAIHelicopter *ih;
    if (GetOwner()->QueryInterface(&ih)) {
        gHeliVehicle = nullptr;
    }
    if (IsSimplePhysicsActive()) {
        DisableSimplePhysics();
    }
    ClearGoal();
    GetVehicle()->Deactivate();

    IPursuitAI *ipai;
    if (GetOwner()->QueryInterface(&ipai)) {
        ipai->SetSupportGoal((const char *)nullptr);
    }
}

bool AIVehicle::CanRespawn(bool respawnAvailable) {
    if (!respawnAvailable) {
        mCanRespawn = true;
    }
    bool rv = false;
    if (mCanRespawn) {
        rv = mLastSpawnTime > 8.0f;
    }
    if (!rv && respawnAvailable && mLastSpawnTime > 10.0f) {
        rv = true;
    }
    return rv;
}

static const float Tweak_OffWorldAccel[2] = {0.5f, 1.0f};
static const float Tweak_OffWorldSpeed[2] = {0.75f, 1.0f};

void AIVehicle::UpdateSimplePhysics(float dT) {
    if (!IsSimplePhysicsActive()) {
        return;
    }

    ISimable *isimable = GetSimable();
    IVehicle *ivehicle = GetVehicle();
    IRigidBody *irigidbody = isimable->GetRigidBody();

    const UMath::Vector3 &position = irigidbody->GetPosition();

    UMath::Vector3 newPosition = position;
    UMath::Matrix4 vehicleMat;
    UMath::Init(vehicleMat);

    UMath::Vector3 destPos = mDest;
    destPos.y += 1.0f;

    UMath::Vector3 dirVector = UVector3(destPos) - position;
    UMath::Unit(dirVector, dirVector);

    float skill = GetSkill();
    float currentSpeed = irigidbody->GetSpeed();
    float driveSpeed = currentSpeed;
    if (driveSpeed > mDriveSpeed) {
        driveSpeed -= dT * 30.0f;
        driveSpeed = UMath::Max(driveSpeed, mDriveSpeed);
    } else {
        driveSpeed += GetAcceleration(driveSpeed) * dT * UMath::Lerp(Tweak_OffWorldAccel[0], Tweak_OffWorldAccel[1], skill);
        driveSpeed = UMath::Min(driveSpeed, mDriveSpeed);
    }
    float top_speed = GetTopSpeed();
    driveSpeed = UMath::Min(top_speed * UMath::Lerp(Tweak_OffWorldSpeed[0], Tweak_OffWorldSpeed[1], skill), UMath::Max(0.0f, driveSpeed));
    if (mITransmission->IsReversing()) {
        driveSpeed *= -0.5f;
    }

    UMath::ScaleAdd(dirVector, driveSpeed * dT, newPosition, newPosition);

    WWorldPos &wpos = isimable->GetWPos();
    wpos.FindClosestFace(position, true);

    bool up_valid = false;
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
    WRoadNav *road_nav = GetDriveToNav();
    if (road_nav != NULL && road_nav->HasCookieTrail()) {
        elevation = road_nav->GetCurrentCookie().Centre.y + 1.0f;
    }
    if (!ivehicle->IsOffWorld()) {
        elevation = wpos.HeightAtPoint(position);
    }

    float rideheight = 0.0f;
    UMath::Vector3 dimension;
    irigidbody->GetDimension(dimension);
    if (GetSuspension()) {
        for (int i = 0; i < 4; i++) {
            rideheight = UMath::Max(rideheight, GetSuspension()->GetRideHeight(i));
        }
    }
    elevation += rideheight + dimension.y;
    newPosition.y = elevation;

    irigidbody->SetPosition(newPosition);
    irigidbody->SetOrientation(vehicleMat);

    UMath::Vector3 linearVelocity;
    UMath::Scale(dirVector, driveSpeed, linearVelocity);
    irigidbody->SetLinearVelocity(linearVelocity);
    irigidbody->SetAngularVelocity(UMath::Vector3::kZero);
}

void AIVehicle::EnableSimplePhysics() {
    if (IsSimplePhysicsActive()) {
        return;
    }
    IVehicle *vehicle = GetVehicle();
    vehicle->SetPhysicsMode(PHYSICS_MODE_EMULATED);
}

void AIVehicle::DisableSimplePhysics() {
    if (!IsSimplePhysicsActive()) {
        return;
    }
    IVehicle *vehicle = GetVehicle();
    if (vehicle->GetPhysicsMode() == PHYSICS_MODE_EMULATED) {
        vehicle->SetPhysicsMode(PHYSICS_MODE_SIMULATED);
    }

    UMath::Vector3 forward;
    IRigidBody *irigidbody = GetSimable()->GetRigidBody();
    UMath::Vector3 angular_velocity = irigidbody->GetAngularVelocity();
    irigidbody->GetForwardVector(forward);
    float speed = irigidbody->GetSpeed();

    vehicle->SetVehicleOnGround(irigidbody->GetPosition(), forward);
    irigidbody->SetAngularVelocity(angular_velocity);
    vehicle->SetSpeed(speed);

    IRBVehicle *rigid_body_vehicle;
    if (GetOwner()->QueryInterface(&rigid_body_vehicle)) {
        rigid_body_vehicle->SetInvulnerability(INVULNERABLE_FROM_PHYSICS_SWITCH, 1.0f);
    }
}

bool AIVehicle::IsSimplePhysicsActive() {
    return GetVehicle()->GetPhysicsMode() == PHYSICS_MODE_EMULATED;
}

struct path_spot {
    path_spot() {}

    path_spot(short s, int i, float p, float l) : segmentindex(s), nodeind(i), param(p), laneoffset(l) {}

    path_spot(const WRoadNav &nav)
        : segmentindex(nav.GetSegmentInd()), nodeind(nav.GetNodeInd()), param(nav.GetSegmentTime()), laneoffset(nav.GetLaneOffset()) {}

    void init_nav(WRoadNav &nav) const;
    void init_nav(WRoadNav &nav, const UMath::Vector3 &point) const;

    short segmentindex;
    int nodeind;
    float param;
    float laneoffset;
};

void path_spot::init_nav(WRoadNav &nav) const {
    WRoadNetwork &roadnetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadnetwork.GetSegment(segmentindex);
    float tparam = param;
    UMath::Vector3 dir;
    UMath::Vector3 point;

    roadnetwork.GetSegmentForwardVector(*segment, dir);
    if (nodeind == 0) {
        tparam = 1.0f - tparam;
        UMath::Negate(dir);
    }
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
    float tparam = param;
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

float TotalWalkPathTime = 0.0f;

struct road_walker {
    struct start_record {
        start_record(float s, WRoadNav &nav) : score(s), spot(nav), point(nav.GetPosition()) {}

        bool operator<(const start_record &o) const {
            return score < o.score;
        }

        float score;
        path_spot spot;
        UMath::Vector3 point;
    };

    typedef UTL::Std::set<short, _type_set> segment_set;
    typedef std::vector<start_record, std::allocator<start_record> > start_vector;

    road_walker() {}

    void set_race_routes(bool on) {
        raceroutes = on;
    }

    const path_spot &get_best_start_spot() {
        return beststartspot;
    }

    const path_spot &get_best_future_spot() {
        return bestfuturespot;
    }

    const path_spot &get_best_target_spot() {
        return besttargetspot;
    }

    float node_find_radius() const {
        return 25.0f;
    }

    bool walk_road(const UMath::Vector3 &start, const UMath::Vector3 &dir, float futuredist, float targetdist, short prevfuture, int prevnodeind);
    void walk_all_paths(const path_spot &start, float futuredist, float targetdist, bool coppenalty);
    void evaluate_end(const path_spot &targetspot, bool coppenalty);

    static const int walk_limit = 32;
    static const int evaluate_limit = 10;

    bool raceroutes;
    float bestscore;
    path_spot beststartspot;
    path_spot bestfuturespot;
    path_spot besttargetspot;
    UMath::Vector3 direction;
    UMath::Vector3 futurepoint;
    UMath::Vector3 targetpoint;
    short previousfutures[2];
    float futurescale;
    path_spot futurespot;
    path_spot startspot;
    UMath::Vector3 startpoint;
    float startscore;
    int numwalkallpaths;
    int numevaluates;
};

bool road_walker::walk_road(const UMath::Vector3 &start, const UMath::Vector3 &dir, float futuredist, float targetdist, short prevfuture,
                            int prevnodeind) {
    ProfileNode profile_node;
    unsigned int ticker = bGetTicker();

    const WGrid &grid = WGrid::Get();
    WRoadNetwork &roadnetwork = WRoadNetwork::Get();

    direction = dir;
    futurescale = futuredist / targetdist;

    previousfutures[0] = prevfuture;
    previousfutures[1] = -1;
    if (prevfuture >= 0) {
        const WRoadNode *node = roadnetwork.GetNode(roadnetwork.GetSegment(prevfuture)->fNodeIndex[prevnodeind]);
        const WRoadSegment *prevseg = GetAttachedDirectionalSegment(node, prevfuture);
        if (prevseg) {
            previousfutures[1] = prevseg->fIndex;
        }
    }

    UMath::Normalize(direction);
    UMath::ScaleAdd(direction, futuredist, start, futurepoint);
    UMath::ScaleAdd(direction, targetdist, start, targetpoint);

    segment_set segments;
    UTL::FastVector<unsigned int, 16> nodeinds;
    nodeinds.reserve(64);
    WGrid::Get().FindNodes(start, node_find_radius(), nodeinds);

    for (UTL::FastVector<unsigned int, 16>::iterator iter = nodeinds.begin(); iter != nodeinds.end(); ++iter) {
        WGridNode *gridnode = grid.fNodes[*iter];
        if (gridnode != NULL) {
            int numsegs = gridnode->GetElemTypeCount(WGrid_kRoadSegment);
            for (int i = 0; i < numsegs; i++) {
                short segind = static_cast<short>(gridnode->GetElemType(i, WGrid_kRoadSegment));
                if (!raceroutes || roadnetwork.GetSegment(segind)->IsInRace()) {
                    segments.insert(segind);
                }
            }
        }
    }

    WRoadNav nav;
    nav.SetPathType(WRoadNav::kPathCop);
    nav.SetLaneType(WRoadNav::kLaneCop);
    nav.SetNavType(WRoadNav::kTypeDirection);

    start_vector startrecords;
    startrecords.reserve(UMath::Max(33, static_cast<int>(segments.size())));

    for (segment_set::iterator it = segments.begin(); it != segments.end(); ++it) {
        short segind = *it;
        if (segind >= static_cast<int>(roadnetwork.GetNumSegments())) {
            continue;
        }

        nav.InitAtSegment(segind, start, direction, false);
        if (nav.GetSegment()->IsOneWay() && nav.GetNodeInd() == 0) {
            nav.Reverse();
        }
        float lane = nav.SnapToSelectableLane(nav.GetLaneOffset());
        if (lane != nav.GetLaneOffset()) {
            nav.ChangeLanes(lane, 0.0f);
        }

        float score = 0.0f;
        score += UMath::Max(0.0f, UMath::Distancexz(nav.GetPosition(), start) - 2.0f);
        score += bMax(0.0f, bAbs(start.y - nav.GetPosition().y) - 2.0f) * 3.0f;

        UMath::Vector3 fwd = nav.GetForwardVector();
        fwd.y = 0.0f;
        UMath::Normalize(fwd);
        score += (1.0f - UMath::Dot(direction, fwd)) * 2.0f;

        startrecords.push_back(start_record(score, nav));
    }

    if (startrecords.empty()) {
        return false;
    }
    std::sort(startrecords.begin(), startrecords.end());

    numwalkallpaths = 0;
    numevaluates = 0;

    bestscore = FLT_MAX;
    for (start_vector::iterator sit = startrecords.begin();
         numevaluates < evaluate_limit && numwalkallpaths < walk_limit && sit != startrecords.end(); ++sit) {
        startscore = sit->score;
        if (startscore >= bestscore) {
            break;
        }
        startspot = sit->spot;
        startpoint = sit->point;
        walk_all_paths(startspot, futuredist, targetdist, false);
    }
    TotalWalkPathTime += bGetTickerDifference(ticker);

    return bestscore < FLT_MAX;
}

void road_walker::walk_all_paths(const path_spot &start, float futuredist, float targetdist, bool coppenalty) {
    numwalkallpaths++;

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
                futurespot = path_spot(segmentindex, nodeind, futurefraction, 0.0f);
            }
        }

        float targetfraction = param + targetdist / segmentlength;
        if (targetfraction <= 1.0f) {
            path_spot endspot(segmentindex, nodeind, targetfraction, 0.0f);
            evaluate_end(endspot, coppenalty);
            return;
        }

        const WRoadNode *node = roadnetwork.GetNode(segment->fNodeIndex[nodeind]);
        futuredist -= segmentlength * (1.0f - param);
        targetdist -= segmentlength * (1.0f - param);

        const WRoadSegment *checksegment = GetAttachedDirectionalSegment(node, segmentindex);
        if (checksegment == NULL) {
            break;
        }
        segmentindex = checksegment->fIndex;
        nodeind = node == roadnetwork.GetNode(checksegment->fNodeIndex[0]);
        segment = checksegment;
        param = 0.0f;
    }

    node = roadnetwork.GetNode(segment->fNodeIndex[nodeind]);
    for (int i = 0; numevaluates < evaluate_limit && numwalkallpaths < walk_limit && i < node->fNumSegments; i++) {
        short newsegmentindex = node->fSegmentIndex[i];
        if (newsegmentindex == segmentindex) {
            continue;
        }
        const WRoadSegment *newsegment = roadnetwork.GetSegment(node->fSegmentIndex[i]);
        if (newsegment->CrossesBarrier() | newsegment->CrossesDriveThroughBarrier()) {
            continue;
        }
        if (raceroutes && !newsegment->IsInRace()) {
            continue;
        }
        int newnodeind = roadnetwork.GetNode(newsegment->fNodeIndex[0]) == node;
        path_spot newspot(newsegmentindex, newnodeind, 0.0f, 0.0f);
        walk_all_paths(newspot, futuredist, targetdist, coppenalty);
    }
}

void road_walker::evaluate_end(const path_spot &targetspot, bool coppenalty) {
    numevaluates++;

    float score = startscore;
    if (coppenalty) {
        score += 5.0f;
    }

    if (score >= bestscore) {
        return;
    }

    WRoadNav nav;
    targetspot.init_nav(nav, targetpoint);

    UMath::Vector3 dir = nav.GetPosition() - startpoint;
    dir.y = 0.0f;
    UMath::Normalize(dir);
    score += (1.0f - UMath::Dot(dir, direction)) * 16.0f;

    if (score >= bestscore) {
        return;
    }

    UMath::Vector3 mid;
    UMath::Lerp(startpoint, nav.GetPosition(), futurescale, mid);
    UMath::Lerp(futurepoint, mid, futurescale, mid);

    WRoadNav futurenav;
    futurespot.init_nav(futurenav, mid);

    UMath::Vector3 futuredir = futurenav.GetPosition() - startpoint;
    futuredir.y = 0.0f;
    UMath::Normalize(futuredir);
    score += (1.0f - UMath::Dot(futuredir, direction)) * 24.0f;

    UMath::Vector3 fwd = nav.GetForwardVector();
    fwd.y = 0.0f;
    UMath::Normalize(fwd);
    score += (1.0f - UMath::Dot(fwd, direction)) * 2.0f;

    if (futurenav.GetSegmentInd() != previousfutures[0] && futurenav.GetSegmentInd() != previousfutures[1]) {
        score += 2.0f;
    }

    if (score < bestscore) {
        beststartspot = startspot;
        bestfuturespot = path_spot(futurenav);
        besttargetspot = path_spot(nav);
        bestscore = score;
    }
}

void AIVehicle::UpdateRoads() {
    ICollisionBody *ibody;
    GetOwner()->QueryInterface(&ibody);

    UMath::Vector3 currentoff;
    UMath::Sub(ibody->GetPosition(), mCurrentRoad.GetPosition(), currentoff);
    bool isvalid = UMath::Length(currentoff) < 20.0f && mCurrentRoad.IsValid() && mFutureRoad.IsValid();

    float timeSinceIncrement = Sim::GetTime() - mRoadIncrementTimer;
    if (isvalid && timeSinceIncrement < 0.02f) {
        return;
    }

    UMath::Vector3 velocity;
    GetSimable()->GetLinearVelocity(velocity);
    float speed = UMath::Length(velocity);

    IPerpetrator *iperp;
    bool bRaceRouteOnly = false;
    if (GetOwner()->QueryInterface(&iperp) && iperp->IsRacing()) {
        bRaceRouteOnly = true;
    }
    mCurrentRoad.SetRaceFilter(bRaceRouteOnly);
    mFutureRoad.SetRaceFilter(bRaceRouteOnly);

    float timeSinceUpdate = Sim::GetTime() - mRoadUpdateTimer;
    if (isvalid && timeSinceUpdate < 0.33f) {
        mRoadIncrementTimer = Sim::GetTime();

        UMath::Vector3 road_direction;
        UMath::Unit(mCurrentRoad.GetForwardVector(), road_direction);
        UMath::Vector3 road_side = UMath::Vector3Make(road_direction.z, 0.0f, -road_direction.x);
        UMath::Normalize(road_side);

        float lanedelta = UMath::Dot(road_side, currentoff);
        float dist = UMath::Dot(road_direction, currentoff);

        if (dist > 0.05f) {
            float laneoffset = mCurrentRoad.SnapToSelectableLane(mCurrentRoad.GetLaneOffset() + lanedelta);
            mCurrentRoad.ChangeLanes(laneoffset, 0.0f);
            mCurrentRoad.IncNavPosition(dist, mCurrentRoad.GetForwardVector(), 0.0f);
        }

        dist = UMath::Length(velocity) - UMath::Distance(mFutureRoad.GetPosition(), mCurrentRoad.GetPosition());
        if (dist > 0.05f) {
            UMath::Vector3 incdir = mFarFuturePosition - mFutureRoad.GetPosition();
            UMath::Normalize(incdir);

            UMath::Unit(mFutureRoad.GetForwardVector(), road_direction);
            road_side = UMath::Vector3Make(road_direction.z, 0.0f, -road_direction.x);

            float lanedelta = UMath::Dot(velocity, road_side) * dist / speed;
            float laneoffset = mFutureRoad.SnapToSelectableLane(mFutureRoad.GetLaneOffset() + lanedelta);
            dist = dist * UMath::Dot(incdir, road_direction);
            mFutureRoad.ChangeLanes(laneoffset, 0.0f);
            mFutureRoad.IncNavPosition(dist, incdir, 0.0f);
        }
        return;
    }

    mRoadUpdateTimer = Sim::GetTime();
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
    if (walker.walk_road(position, direction, futuredistance, targetdistance, mLastFutureSegment, mLastFutureNodeInd)) {
        mCurrentRoad.SetRaceFilter(bRaceRouteOnly);
        mCurrentRoad.SetTrafficFilter(false);
        mCurrentRoad.SetCopFilter(false);
        mFutureRoad.SetRaceFilter(bRaceRouteOnly);
        mFutureRoad.SetTrafficFilter(false);
        mFutureRoad.SetCopFilter(false);

        walker.get_best_start_spot().init_nav(mCurrentRoad);
        walker.get_best_future_spot().init_nav(mFutureRoad);
        mLastFutureSegment = mFutureRoad.GetSegmentInd();
        mLastFutureNodeInd = mFutureRoad.GetNodeInd();

        WRoadNav targetnav;
        walker.get_best_target_spot().init_nav(targetnav);
        mFarFuturePosition = targetnav.GetPosition();
        UMath::Unit(targetnav.GetForwardVector(), mFarFutureDirection);
    }
}

WRoadNav *AIVehicle::GetCurrentRoad() {
    UpdateRoads();
    return &mCurrentRoad;
}

WRoadNav *AIVehicle::GetFutureRoad() {
    UpdateRoads();
    return &mFutureRoad;
}

const UMath::Vector3 &AIVehicle::GetFarFuturePosition() {
    UpdateRoads();
    return mFarFuturePosition;
}

const UMath::Vector3 &AIVehicle::GetFarFutureDirection() {
    UpdateRoads();
    return mFarFutureDirection;
}

const UMath::Vector3 &AIVehicle::GetSeekAheadPosition() {
    UpdateRoads();

    float timeSinceThink = Sim::GetTime() - mSeekAheadTimer;
    if (timeSinceThink > 0.33f) {
        IPerpetrator *iperp;
        bool bRaceRouteOnly = false;

        if (GetOwner()->QueryInterface(&iperp) && iperp->IsRacing()) {
            bRaceRouteOnly = true;
        }

        WRoadNav nav;
        nav.InitFromOtherNav(GetCurrentRoad(), false);

        nav.SetRaceFilter(bRaceRouteOnly);
        nav.SetLaneType(WRoadNav::kLaneCop);
        nav.SetCopFilter(!bRaceRouteOnly && mCurrentRoad.GetSegment()->ShouldCopsConsider());
        nav.SetPathType(WRoadNav::kPathCop);
        nav.SetNavType(WRoadNav::kTypeDirection);

        if (nav.IsValid()) {
            const float kSeekAheadTime = 7.8f;
            const float kSeekAheadOffset = 0.4f;
            mSeekAheadTimer = Sim::GetTime();
            float seekaheadtime = kSeekAheadTime;
            if (mPursuit && mPursuit->GetIsAJerk()) {
                seekaheadtime *= kSeekAheadOffset;
            }

            UMath::Vector3 velocity;
            GetSimable()->GetLinearVelocity(velocity);
            float speed = UMath::Length(velocity);
            float inc_distance = UMath::Min(500.0f, speed * seekaheadtime + 8.0f);

            nav.IncNavPosition(inc_distance, UMath::Vector3::kZero, 0.0f);
            mSeekAheadPosition = nav.GetPosition();
        }
    }
    return mSeekAheadPosition;
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
    mPursuitEscalationAttrib = new Attrib::Gen::pursuitescalation(0xeec2271a, 0, nullptr);
    mPursuitLevelAttrib = nullptr;
    mPursuitSupportAttrib = nullptr;
    SetHeat(1.0f);
    GetOwner()->SetCausality(GetInstanceHandle(), 0.0f);
    mHiddenZoneLatchTime = 0.05f;
    pGlueError = new (nullptr) PidError(10, 5, 1.0f); // TODO name
    fGlueTimer = bRandom(1.0f);
}

AIPerpVehicle::~AIPerpVehicle() {
    delete mPursuitLevelAttrib;
    delete mPursuitSupportAttrib;
    delete mPursuitEscalationAttrib;
    delete pGlueError;
}

static const float Tweak_QuickRaceSkills[3] = {0.15f, 0.4f, 1.0f};
static const float Tweak_QuickRaceSkillsNoGlue[3] = {0.15f, 0.4f, 0.8f};
extern Table AdaptiveSkillUpTable;
extern Table AdaptiveSkillDownTable;

void AIPerpVehicle::ComputeSkill() {
    fBaseSkill = 0.0f;
    if (!GRaceStatus::Exists()) {
        return;
    }
    if (GetOwner()->IsPlayer()) {
        return;
    }

    if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_QuickRace) {
        GRaceParameters *params = GRaceStatus::Get().GetRaceParameters();
        if (params && !params->GetCatchUp()) {
            fBaseSkill = Tweak_QuickRaceSkillsNoGlue[GRaceStatus::Get().GetRaceParameters()->GetDifficulty()];
        } else {
            fBaseSkill = Tweak_QuickRaceSkills[GRaceStatus::Get().GetRaceParameters()->GetDifficulty()];
        }
    } else if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        if (pRacerInfo && pRacerInfo->GetGameCharacter()) {
            float character_skill =
                UMath::Clamp(static_cast<float>(pRacerInfo->GetGameCharacter()->SkillLevel()) * 0.01f, 0.0f, 1.0f);
            float difficulty = GRaceStatus::Get().GetAdaptiveDifficutly();
            if (difficulty > 0.0f) {
                difficulty *= AdaptiveSkillUpTable.GetValue(character_skill);
            } else {
                difficulty *= AdaptiveSkillDownTable.GetValue(character_skill);
            }
            fBaseSkill = UMath::Clamp(character_skill + difficulty, 0.0f, 1.0f);
        }
    }
}

void AIPerpVehicle::SetRacerInfo(GRacerInfo *info) {
    pRacerInfo = info;
    ComputeSkill();
}

// definidas mas abajo, junto a las demas tablas de tweak
extern Table CatchupGlueTable;
extern Table SlowDownGlueTable;

struct FindAvgComplete {
    FindAvgComplete() : total(0.0f), count(0.0f) {}

    void operator()(IVehicle *vehicle) {
        IPerpetrator *ai;
        if (vehicle->QueryInterface(&ai)) {
            GRacerInfo *info = ai->GetRacerInfo();
            if (info) {
                total += info->GetPctRaceComplete();
                count += 1.0f;
            }
        }
    }

    float Result() const {
        return count > 0.0f ? total / count : 0.0f;
    }

    float total;
    float count;
};

float AIPerpVehicle::mStagger = 0.0f;

void AIPerpVehicle::Update(float dT) {
    ProfileNode profile_node;
    static const unsigned int car_hash = bStringHash("Car"), heli_hash = bStringHash("Heli");

    m911CallTimer -= dT;

    AIVehicle::Update(dT);

    mDriveToNav->SetRaceFilter(IsRacing());

    fGlueSkill = 0.0f;
    fGlueOutput = 0.0f;
    bool catchup = IsRacing() && (GetVehicle()->IsStaging() == false);
    GRacerInfo *racer_info = GetRacerInfo();
    if (catchup && racer_info && !GetOwner()->IsPlayer()) {

        IVehicle *player = IVehicle::First(VEHICLE_PLAYERS);

        fGlueTimer += dT;
        if ((fGlueTimer > 1.0f) && player) {

            float percent_complete = racer_info->GetPctRaceComplete();
            float average_complete = IVehicle::ForEach(VEHICLE_PLAYERS, FindAvgComplete()).Result();

            // Partido a proposito: en una sola sentencia, el `associate` de fold
            // (activo por los flags de coma flotante de la unidad) saca el 0,01f
            // del parentesis y emite `(GetRaceLength() * 0,01f) * (avg - pct)`.
            // El objetivo hace `(avg - pct) * 0,01f` primero y multiplica por la
            // longitud despues. El DWARF del original no lista esta intermedia
            // --como tampoco lista `average_complete`, que tampoco tiene sitio--,
            // pero sin ella la funcion se queda en 99,28%.
            float glue_pct = (average_complete - percent_complete) * 0.01f;
            float glue_error = GRaceStatus::Get().GetRaceLength() * glue_pct;

            Physics::Info::Performance perf;

            if (player->GetPerformance(perf)) {

                float performance_ratio = 1.0f - perf.TopSpeed;
                glue_error *= performance_ratio * 0.5f + 1.0f;
            }

            pGlueError->Record(glue_error, fGlueTimer, false, false);
            fGlueTimer -= 1.0f;
        }

        bool off_world = IsSimplePhysicsActive();
        if (GRaceStatus::Get().ComputeCatchUpSkill(racer_info, pGlueError, &fGlueOutput, &fGlueSkill, off_world)) {

            // NO hay `return` aqui: el `bne` del objetivo salta al MISMO destino
            // que el `beq` de `if (catchup...)` -- la salida del bloque --, no al
            // epilogo. Con el `return` toda la cola de la funcion deja de estar
            // en el post-dominio del bloque, el hoisting de gcse no puede subir
            // &myPos, &pos2, el temporal de GetDimension, this+0x50 ni
            // TheTrackPathManager@ha, y GCC salva CUATRO registros menos
            // (`stmw r18` contra `stmw r14`, marco 0xd8 contra 0xe8): 94,11%.
            if (!off_world) {
                if (fGlueSkill > 0.0f) {

                    if (GRaceStatus::IsSpeedTrapRace()) {
                        fGlueSkill *= 0.5f;
                    } else {
                        fGlueSkill *= CatchupGlueTable.GetValue(fBaseSkill);
                    }
                } else if (fGlueSkill < 0.0f) {

                    if (GRaceStatus::IsSpeedTrapRace()) {
                        fGlueSkill *= 0.5f;
                    } else {
                        fGlueSkill *= SlowDownGlueTable.GetValue(fBaseSkill);
                    }
                }
            }

        } else {

            fGlueOutput = 0.0f;
            fGlueSkill = 0.0f;
        }
    }

    IRigidBody *rigid_body = GetSimable()->GetRigidBody();
    UMath::Vector3 myPos = rigid_body->GetPosition();
    bVector3 nfspos;
    myPos.y -= rigid_body->GetDimension().y;

    eSwizzleWorldVector(*reinterpret_cast<const bVector3 *>(&myPos), nfspos);
    bVector2 pos2(nfspos.x, nfspos.y);

    mHiddenFromCars = false;
    mHiddenFromHelicopters = false;

    bool NotSeenRightNow = true;

    IPursuit *ip = GetPursuit();
    if (ip) {

        NotSeenRightNow = ip->GetEvadeLevel() >= 0.05f;

        if (ip->IsPerpBusted()) {

            IInput *ii;
            if (GetOwner()->QueryInterface(&ii)) {

                ii->SetControlGas(0.0f);
                ii->SetControlBrake(1.0f);
                ii->SetControlSteering(0.0f);
                ii->SetControlSteeringVertical(0.0f);
                ii->SetControlHandBrake(1.0f);
                ii->SetControlNOS(false);
            }
        }
    } else {

        mPursuitZoneCheck--;
        if (mPursuitZoneCheck < 0) {
            mPursuitZoneCheck = 10;

#ifndef EA_BUILD_A124
            if (ICopMgr::Exists() && ICopMgr::Get()->VehicleSpawningEnabled(false)) {

                if (!GRaceStatus::Exists() || (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming)) {

                    TrackPathZone *azone = TheTrackPathManager.FindZone(&pos2, TRACK_PATH_ZONE_PURSUIT_START, 0);
                    if (azone) {

                        ICopMgr::Get()->LockoutCops(false);
                        MForcePursuitStart(static_cast<int>(GetHeat())).Post(UCrc32("AICopManager"));
                    }
                }
            }
#endif
        }
    }

    int zoneCount = 0;

    {
    TrackPathZone *azone = TheTrackPathManager.FindZone(&pos2, TRACK_PATH_ZONE_HIDDEN, 0);
    while (azone) {

        float elevation = azone->GetElevation();
        if ((elevation == 0.0f) || (UMath::Abs(myPos.y - elevation) < 1.25f)) {

            if (!mWasInZoneLastUpdate) {

                mWasInZoneLastUpdate = true;

                if (NotSeenRightNow) {

                    mHiddenZoneLatchTime = 0.05f;

                } else {

                    mHiddenZoneLatchTime = 99999.0f;
                }
            } else {

                if (mHiddenZoneLatchTime > 999.0f) {

                    mHiddenZoneTimer = 0.0f;

                } else {

                    mHiddenZoneTimer += dT;
                }
            }

            bool inZoneLongEnough = mHiddenZoneTimer > mHiddenZoneLatchTime;
            dT = 0.0f;

            if (inZoneLongEnough) {

                if (azone->GetData(0) == static_cast<int>(car_hash)) {
                    mHiddenFromCars = true;
                } else if (azone->GetData(0) == static_cast<int>(heli_hash)) {
                    mHiddenFromHelicopters = true;
                } else {
                    mHiddenFromHelicopters = true;
                    mHiddenFromCars = true;
                }
            }

            zoneCount++;
        }

        azone = TheTrackPathManager.FindZone(&pos2, TRACK_PATH_ZONE_HIDDEN, azone);
    }
    }

    if (zoneCount == 0) {

        mWasInZoneLastUpdate = false;
        mHiddenZoneTimer = 0.0f;
    }
}

void AIPerpVehicle::Set911CallTime(float time) {
    m911CallTimer = bMax(time, m911CallTimer);
}

void AIPerpVehicle::OnBehaviorChange(const UCrc32 &mechanic) {
    AIVehicle::OnBehaviorChange(mechanic);
}

bool AIPerpVehicle::IsPartiallyHidden(float &HowHidden) const {
    if (mHiddenZoneTimer > 0.07f) {
        HowHidden = UMath::Min(1.0f, mHiddenZoneTimer / mHiddenZoneLatchTime);
        return true;
    }
    HowHidden = 0.0f;
    return false;
}

void AIPerpVehicle::SetCostToState(int cts) {
    mCostToState = cts;
}

int AIPerpVehicle::GetCostToState() const {
    return mCostToState;
}

void AIPerpVehicle::SetHeat(float heat) {
    int current = static_cast<int>(mHeat);
    int now = static_cast<int>(heat);
    bool useRaceHeatNow = false;

    if (GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing &&
        (!GRaceStatus::Get().GetRaceParameters() || !GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace())) {
        useRaceHeatNow = true;
    }

    bool raceEventStatusChanged = useRaceHeatNow != mWasInRaceEventLastHeatUpdate;
    mWasInRaceEventLastHeatUpdate = useRaceHeatNow;

    if (now != current || raceEventStatusChanged) {
        delete mPursuitLevelAttrib;
        mPursuitLevelAttrib = nullptr;

        delete mPursuitSupportAttrib;
        mPursuitSupportAttrib = nullptr;
    }

    if (!mPursuitLevelAttrib) {
        int idx = now - 1;

        if (useRaceHeatNow) {
            mPursuitLevelAttrib = new Attrib::Gen::pursuitlevels(mPursuitEscalationAttrib->racetable(idx), 0, nullptr);
            mPursuitSupportAttrib = new Attrib::Gen::pursuitsupport(mPursuitEscalationAttrib->supportracetable(idx), 0, nullptr);
        } else {
            mPursuitLevelAttrib = new Attrib::Gen::pursuitlevels(mPursuitEscalationAttrib->heattable(idx), 0, nullptr);
            mPursuitSupportAttrib = new Attrib::Gen::pursuitsupport(mPursuitEscalationAttrib->supporttable(idx), 0, nullptr);
        }
    }

    mHeat = heat;
}

float AIPerpVehicle::GetSkill() const {
    return bClamp(fBaseSkill + fGlueSkill, 0.0f, 1.0f);
}

static const float Tweak_AdaptiveSkillUp[3] = {0.5f, 0.75f, 1.0f};
Table AdaptiveSkillUpTable(Tweak_AdaptiveSkillUp, 3, 0.0f, 1.0f);
static const float Tweak_AdaptiveSkillDown[3] = {0.5f, 0.375f, 0.25f};
Table AdaptiveSkillDownTable(Tweak_AdaptiveSkillDown, 3, 0.0f, 1.0f);
static const float Tweak_CatchupGlueSkill[3] = {0.33f, 0.66f, 1.0f};
Table CatchupGlueTable(Tweak_CatchupGlueSkill, 3, 0.0f, 1.0f);
static const float Tweak_SlowDownGlueSkill[3] = {1.0f, 1.0f, 0.66f};
Table SlowDownGlueTable(Tweak_SlowDownGlueSkill, 3, 0.0f, 1.0f);

static const float Tweak_CatchupCheatSkill[3] = {0.5f, 0.5f, 0.5f};
Table CatchupCheatTable(Tweak_CatchupCheatSkill, 3, 0.0f, 1.0f);

float AIPerpVehicle::GetCatchupCheat() const {
    return UMath::Clamp(fBaseSkill + fGlueSkill - 1.0f, 0.0f, 1.0f) * CatchupCheatTable.GetValue(fBaseSkill);
}

float AIPerpVehicle::GetHeat() const {
    return mHeat;
}

void AIPerpVehicle::AddCostToState(int cost) {
    if (!GRaceStatus::Exists()) {
        return;
    }
    IPursuit *ip = GetPursuit();
    if (ip) {
        bool challengeRace = false;
        GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
        if (parms) {
            challengeRace = parms->GetRaceType() == GRace::kRaceType_Challenge;
        }
        if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career || challengeRace) {
            mCostToState += cost;
            ip->NotifyPropertyDamaged(cost);
        }
    }
}

void AIPerpVehicle::AddToPendingRepPointsNormal(int amount) {
    if (GRaceStatus::Exists()) {
        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming || GRaceStatus::IsChallengeRace()) {
            mPendingRepPointsNormal += amount;
        }
    }
}

void AIPerpVehicle::AddToPendingRepPointsFromCopDestruction(int amount) {
    if (GRaceStatus::Exists()) {
        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming || GRaceStatus::IsChallengeRace()) {
            mPendingRepPointsFromCopDestruction += amount;
        }
    }
}

bool AIPerpVehicle::IsRacing() const {
    if (GetRacerInfo() && GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
        AITarget *target = GetTarget();
        return target && target->IsValid();
    }
    return false;
}

float AIPerpVehicle::GetPercentRaceComplete() const {
    if (pRacerInfo) {
        return pRacerInfo->GetPctRaceComplete();
    }
    return 0.0f;
}

bool AIPerpVehicle::IsBeingPursued() const {
    ISimable *mysimobj = GetSimable();
    const IPursuit::List &Pursuits = IPursuit::GetList();

    for (IPursuit::List::const_iterator Pusuit_iter = Pursuits.begin(); Pusuit_iter != Pursuits.end(); ++Pusuit_iter) {
        IPursuit *curpursuit = *Pusuit_iter;
        AITarget *curtarget = curpursuit->GetTarget();
        if (curtarget) {
            const ISimable *simobj = curtarget->GetSimable();
            if (simobj == mysimobj) {
                return true;
            }
        }
    }
    return false;
}

void AIPerpVehicle::OnCausedExplosion(IExplosion *explosion, ISimable *to) {
    int cost = 0;
    float cause_time = explosion->GetCausalityTime();
    if (Sim::GetTime() - cause_time <= 2.0f) {
        SimableType type = to->GetSimableType();
        IModel *model = to->GetModel();
        bool is_root = model && model->IsRootModel();

        if (type == SIMABLE_SMACKABLE && is_root) {
            Attrib::Gen::smackable attrs(to->GetAttributes());
            cost = attrs.COST_TO_STATE();
        } else if (type == SIMABLE_VEHICLE) {
            IVehicle *ivehicle;
            to->QueryInterface(&ivehicle);
            if (!ivehicle->IsDestroyed()) {
                IPursuitAI *ipursuitai;
                if (to->QueryInterface(&ipursuitai)) {
                    cost = 2000;
                }
            }
        }

        if (cost != 0) {
            if (GetPursuit()) {
                AddCostToState(cost);
            }
        }
    }
    to->SetCausality(GetInstanceHandle(), cause_time);
}

bool AIPerpVehicle::OnClearCausality(float start_time) {
    return false;
}

void AIPerpVehicle::OnCausedCollision(const COLLISION_INFO &cinfo, ISimable *from, ISimable *to) {

    const float sim_time = Sim::GetTime();
    const bool directhit = UTL::COM::ComparePtr(GetOwner(), from);
    const SimableType type = to->GetSimableType();
    const float chain_start_time = directhit ? sim_time : from->GetCausalityTime();
    bool break_chain = false;
    int cost_to_state = 0;
    bool intentionalhit = false;

    // avoid the spurious hits against immobile scenery
    if (type == SIMABLE_SMACKABLE) {
        if ((to->GetInstanceHandle() == cinfo.objA) && cinfo.objAImmobile) {
            return;
        }

        if ((to->GetInstanceHandle() == cinfo.objB) && cinfo.objBImmobile) {
            return;
        }
    }

    if (sim_time - chain_start_time <= 2.0f) {
        IPursuit *ipursuit = GetPursuit();

        if (type == SIMABLE_SMACKABLE) {

            intentionalhit = directhit;

            Attrib::Gen::smackable attribs(to->GetAttributes());
            cost_to_state = attribs.COST_TO_STATE();

            IModel *model = to->GetModel();
            if (model && model->IsRootModel()) {

                if (intentionalhit && ipursuit && ipursuit->IsPerpInSight() && ipursuit->IsPlayerPursuit() &&
                    (ipursuit->GetMinDistanceToTarget() < 25.0f)) {
                    GInfractionManager::Get().ReportDamageToProperty();
                }
            }

            IPlayer *player = GetOwner()->GetPlayer();
            if (player) {

                player->ChargeGameBreaker(0.0f);
            }
            if (GetEngine()) {

                GetEngine()->ChargeNOS(0.0f);
            }

        } else if (type == SIMABLE_VEHICLE) {

            float closing_speed = UMath::Length(cinfo.closingVel);
            bool causalityhit = closing_speed > 4.0f;

            if (directhit) {

                bool i_am_a = (from->GetOwnerHandle() == cinfo.objA);
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
            ITrafficAI *itrafficVehicle;

            if (to->QueryInterface(&ipursuitVehicle)) {

                bool wasDamagedByPerp = ipursuitVehicle->GetDamagedByPerp();
                if (!wasDamagedByPerp && directhit && causalityhit) {

                    ipursuitVehicle->SetDamagedByPerp(true);

                    if (ipursuit) {

                        ipursuit->NotifyCopDamaged(ivehicle);

                        if (ipursuit->IsPlayerPursuit() && intentionalhit) {
                            GInfractionManager::Get().ReportAssaultingPoliceOfficer();
                        }
                    }
                }

                if (!ivehicle->IsDestroyed()) {

                    IVehicleAI *ivehicleai;

                    if (intentionalhit) {
                        cost_to_state = 2000;
                    } else if (!directhit) {
                        cost_to_state = 500;
                    }

                    if (cost_to_state) {
                        float amount = UMath::Ramp(closing_speed, 4.0f, 30.0f);
                        cost_to_state = static_cast<int>((cost_to_state / 50) * amount) * 50;
                        cost_to_state = UMath::Max(50, cost_to_state);
                    }

                    if (directhit && !ipursuitVehicle->GetInPursuit() && to->QueryInterface(&ivehicleai) &&
                        !ivehicleai->GetTarget()->IsValid()) {

                        DriverClass driverclass = GetVehicle()->GetDriverClass();

                        if ((driverclass == DRIVER_HUMAN) || (driverclass == DRIVER_REMOTE) ||
                            (ICopMgr::Exists() && ICopMgr::Get()->CanPursueRacers())) {

                            if (intentionalhit) {
                                ivehicleai->GetTarget()->Aquire(from);
                            }
                        }
                    }
                }
            }

            if (causalityhit && intentionalhit && to->QueryInterface(&itrafficVehicle)) {

                LastTrafficHitTime = sim_time;

                GManager::Get().IncValue("insurance_claims");
                if (ipursuit) {
                    ipursuit->NotifyTrafficCarHit();
                }

                if (GRaceStatus::Exists()) {

                    GRacerInfo *racerInfo = GRaceStatus::Get().GetRacerInfo(from);
                    if (racerInfo) {
                        racerInfo->NotifyTrafficCollision();
                    }
                }

                if (intentionalhit && ipursuit && ipursuit->IsPerpInSight() && ipursuit->IsPlayerPursuit() &&
                    (ipursuit->GetMinDistanceToTarget() < 25.0f)) {
                    GInfractionManager::Get().ReportHitAndRun();
                }
            }

            break_chain = !intentionalhit;
        }

        if (cost_to_state && ipursuit) {

            AddCostToState(cost_to_state);
        }
    }

    to->SetCausality(GetInstanceHandle(), break_chain ? 0.0f : chain_start_time);
}

float AIPerpVehicle::GetLastTrafficHitTime() const {
    return LastTrafficHitTime;
}

