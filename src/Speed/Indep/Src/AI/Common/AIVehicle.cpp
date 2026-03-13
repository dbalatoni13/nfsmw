#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/AI/path_spot.h"
#include "Speed/Indep/Src/AI/road_walker.h"
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
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Generated/Events/EEnableAIPhysics.hpp"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICause.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICheater.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WRoadElem.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/Generated/Messages/MForcePursuitStart.h"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include <algorithm>

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
    MakeDebugable(DBG_AI);
    fMomentRadius = 0.0f;
    bAiControl = false;
    mWrongWay = false;
}

Behavior *AIVehicleHuman::Construct(const BehaviorParams &bp) {
    return new AIVehicleHuman(bp);
}

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

bool bToggleAiControl;

float aHumanNavLookAheadData[2] = {50.0f, 60.0f};
Table HumanNavLookAheadTable(aHumanNavLookAheadData, 2, 0.0f, 100.0f);
float aHumanDragNavLookAheadData[2] = {8.0f, 40.0f};
Table HumanDragNavLookAheadTable(aHumanDragNavLookAheadData, 2, 0.0f, 100.0f);

void AIVehicleHuman::Update(float dT) {
    ProfileNode profile_node("TODO", 0);

    if (bToggleAiControl) {
        SetAiControl(!GetAiControl());
        bToggleAiControl = false;
    }

    UpdateWrongWay();

    if (GetAiControl()) {
        InputControls controls = GetInput()->GetControls();
        AIVehicle::Update(dT);
        return;
    }

    AIVehicle::Update(dT);

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
    ProfileNode profile_node("TODO", 0);
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
    mCurrentGoal = UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::CreateInstance(name, GetOwner());
    DriverClass driverclass = GetVehicle()->GetDriverClass();
    // TODO, this means the first 4
    if (driverclass - 1U < 3) {
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

void AIVehicle::Update(float dT) {
    IRigidBody *rb = GetOwner()->GetRigidBody();
    UMath::Vector3 vfwd;
    rb->GetForwardVector(vfwd);

    float yaw = UMath::Atan2r(vfwd.x, vfwd.z);
    mDampedAngularVel.Integrate(rb->GetAngularVelocity().y, dT);
    if ((yaw < -1.5707964f && mDampedAngle.GetPosition() > 1.5707964f) || (yaw > 1.5707964f && mDampedAngle.GetPosition() < -1.5707964f)) {
        mDampedAngle.SetPosition(yaw);
    } else {
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

// UNSOLVED
void AIVehicle::OnGasBrake(float dT) {
    if ((mDriveFlags & 2) == 0 || !GetInput()) {
        return;
    }

    bool reversing = false;
    GetInput()->SetControlGas(0.0f);
    GetInput()->SetControlBrake(0.0f);
    GetInput()->SetControlHandBrake(0.0f);
    GetInput()->SetControlSteeringVertical(0.0f);

    if (mITransmission) {
        if (mITransmission->IsReversing()) {
            reversing = true;
        }

        if (mITransmission && GetVehicle()->GetDriverClass() == DRIVER_TRAFFIC) {
            bool in_shock = GetVehicle()->InShock();
            GearID drive_gear = reversing ? G_REVERSE : G_FIRST;
            bool in_neutral = mITransmission->GetGear() == G_NEUTRAL;

            // TODO
            if (in_neutral && !in_shock || in_shock) {
                mITransmission->Shift(in_shock ? G_NEUTRAL : drive_gear);
            }

            if (in_shock) {
                return;
            }
        }
    }

    float currentSpeed = GetVehicle()->GetSpeed();
    float desiredSpeed = mDriveSpeed;
    float steer;

    if (!mReversingSpeed && mSteeringBehind) {
        GetInput()->SetControlGas(1.0f);
        GetInput()->SetControlHandBrake(1.0f);
        return;
    }
    GetInput()->GetControls();
    if (desiredSpeed < 0.5f) {
        GetInput()->SetControlBrake(1.0f);
        return;
    }
    if (reversing) {
        if (currentSpeed > 1.0f) {
            GetInput()->SetControlBrake(1.0f);
        } else {
            GetInput()->SetControlGas(1.0f);
        }
        return;
    }
    if (currentSpeed < -1.0f) {
        GetInput()->SetControlBrake(1.0f);
        return;
    }

    if (desiredSpeed < currentSpeed) {
        if (UMath::Abs(desiredSpeed - currentSpeed) > 2.5f || desiredSpeed < 5.0f) {
            GetInput()->SetControlBrake(1.0f);
        }
        return;
    }

    GetInput()->SetControlGas(1.0f);
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

inline void AIVehicle::SetReverseOverride(float time) {
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
    ProfileNode profile_node("TODO", 0);
    mDrivableToTargetPos = !WorldCollision(GetPosition(), mTarget->GetPosition());
}

bool AIVehicle::WorldCollision(const UMath::Vector3 &pos, const UMath::Vector3 &dest) {
    if (UMath::DistanceSquare(pos, dest) > 90000.0f) {
        return true;
    }

    UMath::Vector4 posToDest[2];
    posToDest[0] = UMath::Vector4Make(pos, 1.0f);
    posToDest[0].y = pos.y + 1.0f;
    posToDest[1] = UMath::Vector4Make(dest, 1.0f);
    posToDest[1].y = dest.y + 1.0f;

    WCollisionMgr::WorldCollisionInfo cInfo;
    WCollisionMgr collMgr(0, 0);
    int hit = collMgr.CheckHitWorld(posToDest, cInfo, 3);
    if (hit != 0) {
        if (1.0f < UMath::DistanceSquarexyz(posToDest[1], cInfo.fCollidePt)) {
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

    bool foundCollision = false;
    ISimable *isimable = GetSimable();
    IRigidBody *irb = isimable->GetRigidBody();
    const UMath::Vector3 &position = irb->GetPosition();
    UMath::Vector3 forwardVector;
    irb->GetForwardVector(forwardVector);
    UMath::Vector3 rightVector;
    irb->GetRightVector(rightVector);
    UMath::Vector3 dimension;
    irb->GetDimension(dimension);

    leftCollNormal = UMath::Vector3::kZero;
    rightCollNormal = UMath::Vector3::kZero;

    float kRightStart = -3.0f;
    float kTimeScale = 1.5f;
    float kSpeedOffset = 5.0f;
    float kRightEnd = 3.0f;
    float kRightStep = 6.0f;

    for (float i = kRightStart; i <= kRightEnd; i += kRightStep) {
        UMath::Vector3 collVec;
        UMath::Scale(rightVector, i, collVec);
        UMath::Vector3 boundPos;
        UMath::ScaleAdd(collVec, dimension.x, position, boundPos);
        UMath::ScaleAdd(forwardVector, dimension.z, boundPos, boundPos);

        UMath::Scale(rightVector, i * kTimeScale, collVec);
        UMath::Add(forwardVector, collVec, collVec);
        UMath::Unit(collVec, collVec);

        float minDistance = UMath::Max(irb->GetRadius() + kSpeedOffset, irb->GetSpeedXZ() * kTimeScale + irb->GetRadius());

        UMath::Vector3 collPos;
        UMath::ScaleAdd(collVec, minDistance, position, collPos);

        UMath::Vector4 posToDest[2];
        posToDest[0] = UMath::Vector4Make(boundPos, kRightEnd);
        posToDest[1] = UMath::Vector4Make(collPos, kRightEnd);

        WCollisionMgr::WorldCollisionInfo cInfo;
        WCollisionMgr collMgr(0, 3);

        if (collMgr.CheckHitWorld(posToDest, cInfo, 2)) {
            foundCollision = true;
            if (i < 0.0f) {
                rightCollNormal = Vector4To3(cInfo.fNormal);
            } else {
                leftCollNormal = Vector4To3(cInfo.fNormal);
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

void AIPerpVehicle::SetRacerInfo(GRacerInfo *info) {
    pRacerInfo = info;
    ComputeSkill();
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

static const float Tweak_CatchupCheatSkill[3] = {0.5f, 0.5f, 0.5f};
Table CatchupCheatTable(Tweak_CatchupCheatSkill, 3, 0.0f, 1.0f);

static const float Tweak_CatchupGlue = 1.0f;
Table CatchupGlueTable(&Tweak_CatchupGlue, 1, 0.0f, 1.0f);

static const float Tweak_SlowDownGlue = 1.0f;
Table SlowDownGlueTable(&Tweak_SlowDownGlue, 1, 0.0f, 1.0f);

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

bool AIPerpVehicle::OnClearCausality(float start_time) {
    return false;
}

float AIPerpVehicle::GetLastTrafficHitTime() const {
    return LastTrafficHitTime;
}

static const float Tweak_AdaptiveSkillUp[] = {0.0f, 0.3f, 0.5f};
static const float Tweak_AdaptiveSkillDown[] = {0.0f, 0.3f, 0.5f};
static const float Tweak_QuickRaceSkills[] = {0.0f, 0.5f, 1.0f};
static const float Tweak_QuickRaceSkillsNoGlue[] = {0.0f, 0.5f, 1.0f};

static Table AdaptiveSkillUpTable(Tweak_AdaptiveSkillUp, 3, 0.0f, 1.0f);
static Table AdaptiveSkillDownTable(Tweak_AdaptiveSkillDown, 3, 0.0f, 1.0f);

void AIPerpVehicle::ComputeSkill() {
    fBaseSkill = 0.0f;
    if (GRaceStatus::Exists()) {
        GRace::Context context = GRaceStatus::Get().GetRaceContext();
        if (context == GRace::kRaceContext_QuickRace) {
            GRaceParameters *params = GRaceStatus::Get().GetRaceParameters();
            if (params == nullptr || GRaceStatus::Get().GetRaceParameters()->GetCatchUp()) {
                GRace::Difficulty difficulty = GRaceStatus::Get().GetRaceParameters()->GetDifficulty();
                fBaseSkill = Tweak_QuickRaceSkills[difficulty];
            } else {
                GRace::Difficulty difficulty = GRaceStatus::Get().GetRaceParameters()->GetDifficulty();
                fBaseSkill = Tweak_QuickRaceSkillsNoGlue[difficulty];
            }
        } else if (context == GRace::kRaceContext_Career) {
            if (pRacerInfo != nullptr) {
                GCharacter *character = pRacerInfo->GetGameCharacter();
                if (character != nullptr) {
                    float character_skill = static_cast< float >(character->SkillLevel(0)) * 0.01f;
                    character_skill = UMath::Clamp(character_skill, 0.0f, 1.0f);
                    float difficulty = GRaceStatus::Get().GetAdaptiveDifficutly();
                    Table *table;
                    if (difficulty > 0.0f) {
                        table = &AdaptiveSkillUpTable;
                    } else {
                        table = &AdaptiveSkillDownTable;
                    }
                    float adjustment = table->GetValue(character_skill);
                    character_skill = character_skill + difficulty * adjustment;
                    fBaseSkill = UMath::Clamp(character_skill, 0.0f, 1.0f);
                }
            }
        }
    }
}

void AIPerpVehicle::OnCausedExplosion(IExplosion *explosion, ISimable *to) {
    float chain_start_time = explosion->GetCausalityTime();
    float sim_time = Sim::GetTime();
    int cost_to_state = 0;

    if (sim_time - chain_start_time <= 0.5f) {
        SimableType type = to->GetSimableType();
        IModel *model = to->GetModel();
        bool bIsRootModel = false;
        if (model != nullptr && model->IsRootModel()) {
            bIsRootModel = true;
        }

        if (type == SIMABLE_SMACKABLE && bIsRootModel) {
            Attrib::Instance attribs(to->GetAttributes());
            const unsigned int *resultptr =
                reinterpret_cast< const unsigned int * >(attribs.GetAttributePointer(0x6db7d192, 0));
            if (!resultptr) {
                resultptr = reinterpret_cast< const unsigned int * >(Attrib::DefaultDataArea(sizeof(unsigned int)));
            }
            cost_to_state = *resultptr;
        } else if (type == SIMABLE_VEHICLE) {
            IVehicle *ivehicle;
            to->QueryInterface(&ivehicle);
            if (!ivehicle->IsDestroyed()) {
                IPursuitAI *ipursuitVehicle;
                if (to->QueryInterface(&ipursuitVehicle)) {
                    cost_to_state = 2000;
                }
            }
        }

        if (cost_to_state != 0 && GetPursuit() != nullptr) {
            AddCostToState(cost_to_state);
        }
    }

    to->SetCausality(static_cast< ICause * >(this)->GetInstanceHandle(), chain_start_time);
}

void AIPerpVehicle::OnCausedCollision(const Sim::Collision::Info &cinfo, ISimable *from, ISimable *to) {
    float sim_time = Sim::GetTime();
    bool directhit = UTL::COM::ComparePtr(GetOwner(), from);
    SimableType type = to->GetSimableType();
    float chain_start_time;
    if (directhit) {
        chain_start_time = sim_time;
    } else {
        chain_start_time = from->GetCausalityTime();
    }
    bool break_chain = false;
    int cost_to_state = 0;
    bool intentionalhit = false;

    if (type == SIMABLE_SMACKABLE) {
        HSIMABLE toHandle = to->GetInstanceHandle();
        if ((toHandle == cinfo.objA && cinfo.objAImmobile) ||
            (toHandle == cinfo.objB && cinfo.objBImmobile)) {
            break_chain = true;
        }
    }

    if (!break_chain && sim_time - chain_start_time <= 0.5f) {
        IPursuit *ipursuit = GetPursuit();

        if (type == SIMABLE_SMACKABLE) {
            Attrib::Instance attribs(to->GetAttributes());
            const unsigned int *resultptr =
                reinterpret_cast< const unsigned int * >(attribs.GetAttributePointer(0x6db7d192, 0));
            if (!resultptr) {
                resultptr = reinterpret_cast< const unsigned int * >(Attrib::DefaultDataArea(sizeof(unsigned int)));
            }
            cost_to_state = *resultptr;

            IModel *model = to->GetModel();
            if (model != nullptr && model->IsRootModel() && directhit &&
                ipursuit != nullptr && ipursuit->IsPerpInSight() &&
                ipursuit->IsPlayerPursuit() &&
                ipursuit->TimeToFinisherAttempt() < 30.0f) {
                GInfractionManager::Get().ReportDamageToProperty();
            }
        } else if (type == SIMABLE_VEHICLE) {
            float closing_speed = UMath::Length(cinfo.closingVel);
            bool causalityhit = !directhit;

            {
                bool i_am_a = cinfo.objA == GetOwner()->GetInstanceHandle();
                float normal_dir = i_am_a ? 1.0f : -1.0f;
                const UMath::Vector3 &my_vel = i_am_a ? cinfo.objAVel : cinfo.objBVel;
                const UMath::Vector3 &his_vel = i_am_a ? cinfo.objBVel : cinfo.objAVel;
                float my_closing_speed = UMath::Dot(my_vel, cinfo.normal);
                float his_closing_speed = UMath::Dot(his_vel, cinfo.normal);
                intentionalhit = normal_dir * my_closing_speed < normal_dir * -his_closing_speed;
            }

            IVehicle *ivehicle;
            to->QueryInterface(&ivehicle);
            IPursuitAI *ipursuitVehicle;
            to->QueryInterface(&ipursuitVehicle);

            if (ipursuitVehicle != nullptr) {
                bool wasDamagedByPerp = ipursuitVehicle->GetDamagedByPerp();
                if (!wasDamagedByPerp && !directhit && 5.0f < closing_speed) {
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
                        cost_to_state = 2000;
                    } else if (directhit) {
                        cost_to_state = 500;
                    }

                    if (cost_to_state != 0) {
                        float amount = UMath::Ramp(closing_speed, 5.0f, 40.0f);
                        cost_to_state = UMath::Max(static_cast< int >(static_cast< float >(cost_to_state / 50) * amount) * 50, 50);
                    }

                    if (!directhit && !ipursuitVehicle->GetInPursuit()) {
                        IVehicleAI *ivehicleai;
                        to->QueryInterface(&ivehicleai);
                        if (ivehicleai != nullptr && !ivehicleai->GetTarget()->IsValid()) {
                            IVehicle *myVehicle = GetVehicle();
                            DriverClass driverclass = myVehicle->GetDriverClass();
                            if (driverclass == DRIVER_HUMAN || driverclass == DRIVER_REMOTE ||
                                (ICopMgr::Exists() && ICopMgr::Get()->CanPursueRacers())) {
                                if (intentionalhit) {
                                    ivehicleai->GetTarget()->Aquire(from);
                                }
                            }
                        }
                    }
                }
            }

            ITrafficAI *itrafficVehicle;
            if (5.0f < closing_speed && intentionalhit && to->QueryInterface(&itrafficVehicle)) {
                LastTrafficHitTime = sim_time;
                GManager::Get().IncValue("TrafficCarsHit");
                if (ipursuit != nullptr) {
                    ipursuit->NotifyTrafficCarHit();
                }
                if (GRaceStatus::Exists()) {
                    GRacerInfo *racerInfo = GRaceStatus::Get().GetRacerInfo(from);
                    if (racerInfo != nullptr) {
                        racerInfo->NotifyTrafficCollision();
                    }
                }

                if (intentionalhit && ipursuit != nullptr &&
                    ipursuit->IsPerpInSight() &&
                    ipursuit->IsPlayerPursuit() &&
                    ipursuit->TimeToFinisherAttempt() < 30.0f) {
                    GInfractionManager::Get().ReportHitAndRun();
                }
            }
            break_chain = !intentionalhit;
        }

        if (cost_to_state != 0 && ipursuit != nullptr) {
            AddCostToState(cost_to_state);
        }
    }

    to->SetCausality(static_cast< ICause * >(this)->GetInstanceHandle(), chain_start_time);
}

struct FindAvgComplete {
    float mTotal;
    float mCount;

    FindAvgComplete() : mTotal(0.0f), mCount(0.0f) {}

    void operator()(IVehicle *vehicle) {
        IPerpetrator *ai;
        if (vehicle->QueryInterface(&ai)) {
            GRacerInfo *info = ai->GetRacerInfo();
            if (info != nullptr) {
                mTotal += info->GetPctRaceComplete();
                mCount += 1.0f;
            }
        }
    }

    float Result() const {
        if (mCount > 0.0f) {
            return mTotal / mCount;
        }
        return 0.0f;
    }
};

void AIPerpVehicle::Update(float dT) {
    ProfileNode profile_node("AIPerpVehicle::Update", 0);

    static int car_hash = bStringHash("car");
    static int heli_hash = bStringHash("heli");

    m911CallTimer -= dT;
    AIVehicle::Update(dT);

    mDriveToNav->SetRaceFilter(IsRacing() && WRoadNetwork::Get().IsRaceFilterValid());

    fGlueSkill = 0.0f;
    fGlueOutput = 0.0f;

    bool off_world = false;
    if (IsRacing()) {
        off_world = !GetVehicle()->IsStaging();
    }

    GRacerInfo *racer_info = GetRacerInfo();

    if (off_world && racer_info != nullptr && !GetOwner()->IsPlayer()) {
        IVehicle *player = IVehicle::First(VEHICLE_PLAYERS);

        fGlueTimer += dT;

        if (fGlueTimer > 1.0f && player != nullptr) {
            float percent_complete = racer_info->GetPctRaceComplete();
            FindAvgComplete avg;
            avg = IVehicle::ForEach(VEHICLE_AI, avg);
            float average_complete = avg.Result();

            float glue_error = GRaceStatus::Get().GetRaceLength() * (average_complete - percent_complete) * 10.0f;

            Physics::Info::Performance perf;
            if (player->GetPerformance(perf)) {
                glue_error *= (1.0f - perf.TopSpeed) * 0.5f + 1.0f;
            }

            pGlueError->Record(glue_error, fGlueTimer, false, false);
            fGlueTimer -= 1.0f;
        }

        bool simple = IsSimplePhysicsActive();
        bool catchup = GRaceStatus::Get().ComputeCatchUpSkill(racer_info, pGlueError, &fGlueOutput, &fGlueSkill, simple);

        if (catchup) {
            if (!simple) {
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
            fGlueSkill = 0.0f;
            fGlueOutput = 0.0f;
        }
    }

    IRigidBody *rigid_body = GetSimable()->GetRigidBody();
    const UMath::Vector3 &bodyPos = rigid_body->GetPosition();
    UMath::Vector3 dim = rigid_body->GetDimension();

    bVector3 nfspos;
    eSwizzleWorldVector(reinterpret_cast< const bVector3 & >(bodyPos), nfspos);

    UMath::Vector3 myPos;
    myPos.x = nfspos.x;
    myPos.y = nfspos.y - dim.y;
    myPos.z = nfspos.z;

    mHiddenFromCars = false;
    mHiddenFromHelicopters = false;

    bVector2 pos2(nfspos.x, nfspos.z);

    IPursuit *ip = GetPursuit();
    bool NotSeenRightNow = true;

    if (ip == nullptr) {
        mPursuitZoneCheck--;
        if (mPursuitZoneCheck < 0) {
            mPursuitZoneCheck = 10;
            if (ICopMgr::Exists() && ICopMgr::Get()->VehicleSpawningEnabled(false) &&
                (!GRaceStatus::Exists() || GRaceStatus::Get().GetPlayMode() == 0)) {
                TrackPathZone *azone = TheTrackPathManager.FindZone(&pos2, TRACK_PATH_ZONE_PURSUIT_START, nullptr);
                if (azone != nullptr) {
                    ICopMgr::Get()->LockoutCops(false);
                    MForcePursuitStart msg(static_cast< int >(GetHeat()));
                    UCrc32 port("gameplay");
                    msg.Post(port);
                }
            }
        }
    } else {
        NotSeenRightNow = ip->GetEvadeLevel() >= 0.5f;

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
    }

    int zoneCount = 0;
    TrackPathZone *azone = TheTrackPathManager.FindZone(&pos2, TRACK_PATH_ZONE_HIDDEN, nullptr);
    while (azone != nullptr) {
        float elevation = azone->GetElevation();

        if (elevation == 0.0f || UMath::Abs(myPos.y - elevation) < 10.0f) {
            if (!mWasInZoneLastUpdate) {
                mWasInZoneLastUpdate = true;
                if (NotSeenRightNow) {
                    mHiddenZoneLatchTime = 0.5f;
                } else {
                    mHiddenZoneLatchTime = 2.0f;
                }
            } else if (mHiddenZoneLatchTime <= 5.0f) {
                mHiddenZoneTimer += dT;
            } else {
                mHiddenZoneTimer = 0.0f;
            }

            if (mHiddenZoneLatchTime < mHiddenZoneTimer) {
                int data = azone->GetData(0);
                if (data != car_hash) {
                    if (data == heli_hash) {
                        mHiddenFromHelicopters = true;
                    } else {
                        mHiddenFromHelicopters = true;
                        mHiddenFromCars = true;
                    }
                } else {
                    mHiddenFromCars = true;
                }
            }

            zoneCount++;
        }

        azone = TheTrackPathManager.FindZone(&pos2, TRACK_PATH_ZONE_HIDDEN, azone);
    }

    if (zoneCount == 0) {
        mWasInZoneLastUpdate = false;
        mHiddenZoneTimer = 0.0f;
    }
}

// Range: 0x80020438 -> 0x80020604
void path_spot::init_nav(WRoadNav &nav) const {
    WRoadNetwork &roadnetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadnetwork.GetSegment(segmentindex);
    float tparam = param;
    UMath::Vector3 dir;
    UMath::Vector3 point;

    roadnetwork.GetSegmentForwardVector(*segment, dir);
    if (nodeind == 0) {
        UMath::Negate(dir);
        tparam = 1.0f - param;
    }

    segment->GetStartControl(point);

    nav.SetPathType(WRoadNav::kPathCop);
    nav.SetNavType(WRoadNav::kTypeDirection);
    nav.SetLaneType(WRoadNav::kLaneCop);
    nav.InitAtSegment(segmentindex, tparam, point, dir, true);
    nav.ChangeLanes(laneoffset, 0.0f);
}

// Range: 0x80020604 -> 0x800206F8
void path_spot::init_nav(WRoadNav &nav, const UMath::Vector3 &point) const {
    WRoadNetwork &roadnetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadnetwork.GetSegment(segmentindex);
    float tparam = param;
    UMath::Vector3 dir;

    roadnetwork.GetSegmentForwardVector(*segment, dir);
    if (nodeind == 0) {
        UMath::Negate(dir);
        tparam = 1.0f - param;
    }

    nav.SetPathType(WRoadNav::kPathCop);
    nav.SetNavType(WRoadNav::kTypeDirection);
    nav.SetLaneType(WRoadNav::kLaneCop);
    nav.InitAtSegment(segmentindex, tparam, point, dir, false);
    float snapped = nav.SnapToSelectableLane(nav.GetLaneOffset());
    nav.ChangeLanes(snapped, 0.0f);
}

road_walker::start_record::start_record(float s, WRoadNav &nav)
    : score(s) {
    spot.segmentindex = nav.GetSegmentInd();
    spot.nodeind = nav.GetNodeInd();
    spot.param = nav.GetSegmentTime();
    spot.laneoffset = nav.GetLaneOffset();
    point = nav.GetPosition();
}

const int road_walker::walk_limit = 32;
const int road_walker::evaluate_limit = 10;

float TotalWalkPathTime;

// Range: 0x800206F8 -> 0x800212C8
bool road_walker::walk_road(const UMath::Vector3 &start, const UMath::Vector3 &dir,
                            float futuredist, float targetdist, short prevfuture,
                            int prevnodeind) {
    ProfileNode profile_node;
    unsigned int ticks = bGetTicker();
    const WGrid &grid = WGrid::Get();
    WRoadNetwork &roadnetwork = WRoadNetwork::Get();
    UTL::Std::set<short, _type_set> segments;
    UTL::FastVector<unsigned int, 16> nodes;
    WRoadNav startnav;
    start_vector sortedstarts;

    direction = dir;
    previousfutures[1] = -1;
    previousfutures[0] = prevfuture;
    futurescale = futuredist / targetdist;

    if (prevfuture > -1) {
        const WRoadNode *node =
            roadnetwork.GetNode(roadnetwork.GetSegment(prevfuture)->fNodeIndex[prevnodeind]);
        const WRoadSegment *checksegment = GetAttachedDirectionalSegment(node, prevfuture);
        if (checksegment) {
            previousfutures[1] = checksegment->fIndex;
        }
    }

    UMath::Normalize(direction);
    UMath::ScaleAdd(dir, futuredist, start, futurepoint);
    UMath::ScaleAdd(dir, targetdist, start, targetpoint);

    grid.FindNodes(start, node_find_radius(), nodes);

    for (unsigned int *it = nodes.begin(); it != nodes.end(); ++it) {
        WGridNode *gridnode = grid.GetNode(*it);
        if (gridnode) {
            int count = gridnode->GetElemTypeCount(WGrid_kRoadSegment);
            for (int j = 0; j < count; j++) {
                short segInd =
                    static_cast<short>(gridnode->GetElemType(j, WGrid_kRoadSegment));
                if (!raceroutes || roadnetwork.GetSegment(segInd)->IsInRace()) {
                    segments.insert(segInd);
                }
            }
        }
    }

    startnav.SetPathType(WRoadNav::kPathCop);
    startnav.SetNavType(WRoadNav::kTypeDirection);
    startnav.SetLaneType(WRoadNav::kLaneCop);

    int reservesize = 33;
    if (static_cast<int>(segments.size()) > reservesize) {
        reservesize = segments.size();
    }
    sortedstarts.reserve(reservesize);

    for (UTL::Std::set<short, _type_set>::iterator sit = segments.begin();
         sit != segments.end(); ++sit) {
        if (*sit < static_cast<short>(roadnetwork.GetNumSegments())) {
            startnav.InitAtSegment(*sit, start, direction, false);

            if ((roadnetwork.GetSegment(startnav.GetSegmentInd())->fFlags & 0x40) &&
                startnav.GetNodeInd() == 0) {
                startnav.Reverse();
            }

            float snapped = startnav.SnapToSelectableLane(startnav.GetLaneOffset());
            if (snapped != startnav.GetLaneOffset()) {
                startnav.ChangeLanes(snapped, 0.0f);
            }

            float dx = start.x - startnav.GetPosition().x;
            float dz = start.z - startnav.GetPosition().z;
            float horizDist = UMath::Sqrt(dx * dx + dz * dz) - 10.0f;
            if (horizDist < 0.0f) {
                horizDist = 0.0f;
            }

            float heightDiff = bAbs(start.y - startnav.GetPosition().y) - 10.0f;
            if (heightDiff < 0.0f) {
                heightDiff = 0.0f;
            }

            UMath::Vector3 fwd = startnav.GetForwardVector();
            UMath::Normalize(fwd);
            float dot = UMath::Dot(direction, fwd);
            float dirScore = 1.0f - dot;

            float score = horizDist + heightDiff * 5.0f + dirScore + dirScore;

            start_record rec(score, startnav);
            sortedstarts.push_back(rec);
        }
    }

    if (sortedstarts.begin() == sortedstarts.end()) {
        TotalWalkPathTime += bGetTickerDifference(ticks);
        return false;
    }

    std::sort(sortedstarts.begin(), sortedstarts.end());

    float oldscore = bestscore;
    bestscore = 1e30f;
    numevaluates = 0;
    numwalkallpaths = 0;

    for (start_record *it = sortedstarts.begin();
         numevaluates < evaluate_limit && numwalkallpaths < walk_limit &&
         it != sortedstarts.end() && (startscore = it->score, it->score < bestscore);
         ++it) {
        startspot = it->spot;
        startpoint = it->point;
        walk_all_paths(it->spot, futuredist, targetdist, false);
    }

    TotalWalkPathTime += bGetTickerDifference(ticks);
    return bestscore < oldscore;
}

// Range: 0x800212C8 -> 0x80021620
void road_walker::walk_all_paths(const path_spot &start, float futuredist, float targetdist,
                                 bool coppenalty) {
    WRoadNetwork &roadnetwork = WRoadNetwork::Get();
    numwalkallpaths++;

    short segInd = start.segmentindex;
    int nodeind = start.nodeind;
    const WRoadSegment *segment = roadnetwork.GetSegment(segInd);
    float param = start.param;
    bool first = true;

    for (;;) {
        unsigned short flags = segment->fFlags;

        if ((flags & 0x40) && nodeind == 0) {
            return;
        }

        float segdist = segment->GetLength();

        if (first) {
            first = false;
            if (!coppenalty) {
                coppenalty = !segment->ShouldCopsConsider();
            }
        } else {
            coppenalty = true;
        }

        if (futuredist > 0.0f) {
            float futparam = param + futuredist / segdist;
            if (futparam <= 1.0f) {
                futurespot.segmentindex = segInd;
                futurespot.nodeind = nodeind;
                futurespot.param = futparam;
                futurespot.laneoffset = 0.0f;
            }
        }

        float tgtparam = param + targetdist / segdist;
        if (tgtparam <= 1.0f) {
            path_spot targetspot;
            targetspot.segmentindex = segInd;
            targetspot.nodeind = nodeind;
            targetspot.param = tgtparam;
            targetspot.laneoffset = 0.0f;
            evaluate_end(targetspot, coppenalty);
            return;
        }

        float remaining = segdist * (1.0f - param);
        targetdist -= remaining;
        futuredist -= remaining;

        const WRoadNode *nextNode =
            roadnetwork.GetNode(segment->fNodeIndex[nodeind]);
        const WRoadSegment *nextSeg =
            GetAttachedDirectionalSegment(nextNode, segInd);
        param = 0.0f;

        if (nextSeg == nullptr) {
            int i = 0;
            if (numevaluates < evaluate_limit && numwalkallpaths < walk_limit &&
                nextNode->fNumSegments > 0) {
                do {
                    unsigned short branchSegInd = nextNode->fSegmentIndex[i];
                    if (static_cast<short>(branchSegInd) != segInd) {
                        unsigned short bflags =
                            roadnetwork.GetSegment(branchSegInd)->fFlags;
                        if (!(bflags & 0x1000) && !((bflags >> 13) & 1)) {
                            if (!raceroutes || (static_cast<short>(bflags) < 0)) {
                                path_spot branchStart;
                                branchStart.segmentindex = branchSegInd;
                                branchStart.nodeind =
                                    (roadnetwork.GetNode(
                                         roadnetwork.GetSegment(branchSegInd)
                                             ->fNodeIndex[0]) ==
                                     nextNode)
                                        ? 1
                                        : 0;
                                branchStart.param = 0.0f;
                                branchStart.laneoffset = 0.0f;
                                walk_all_paths(branchStart, futuredist, targetdist,
                                               coppenalty);
                            }
                        }
                    }
                    i++;
                } while (numevaluates < evaluate_limit &&
                         numwalkallpaths < walk_limit &&
                         i < static_cast<int>(nextNode->fNumSegments));
            }
            return;
        }

        segInd = nextSeg->fIndex;
        nodeind = (nextNode == roadnetwork.GetNode(nextSeg->fNodeIndex[0])) ? 1 : 0;
        segment = nextSeg;
    }
}

// Range: 0x80021620 -> 0x80021A28
void road_walker::evaluate_end(const path_spot &targetspot, bool coppenalty) {
    numevaluates++;

    float score = startscore;
    if (coppenalty) {
        score += 3.0f;
    }

    if (score < bestscore) {
        WRoadNav nav1;
        targetspot.init_nav(nav1, targetpoint);

        UMath::Vector3 toStart;
        UMath::Sub(nav1.GetPosition(), startpoint, toStart);
        UMath::Vector3 normToStart;
        normToStart = toStart;
        normToStart.y = 0.0f;
        float len = UMath::Normalize(normToStart);
        if (len != 0.0f) {
            // already normalized
        }
        float dot = UMath::Dot(normToStart, direction);
        score += (1.0f - dot) * 0.5f;

        if (score < bestscore) {
            float s = futurescale;
            UMath::Vector3 interpPos;
            interpPos.x =
                ((nav1.GetPosition().x - startpoint.x) * s + startpoint.x - futurepoint.x) * s +
                futurepoint.x;
            interpPos.y =
                ((nav1.GetPosition().y - startpoint.y) * s + startpoint.y - futurepoint.y) * s +
                futurepoint.y;
            interpPos.z =
                ((nav1.GetPosition().z - startpoint.z) * s + startpoint.z - futurepoint.z) * s +
                futurepoint.z;

            WRoadNav nav2;
            futurespot.init_nav(nav2, interpPos);

            UMath::Vector3 toStart2;
            UMath::Sub(nav2.GetPosition(), startpoint, toStart2);
            UMath::Vector3 normToStart2;
            normToStart2 = toStart2;
            normToStart2.y = 0.0f;
            float len2 = UMath::Normalize(normToStart2);
            if (len2 != 0.0f) {
            }

            float dot2 = UMath::Dot(normToStart2, direction);
            float futureScore = (1.0f - dot2) * 1.0f;

            UMath::Vector3 fwd;
            fwd = nav1.GetForwardVector();
            fwd.y = 0.0f;
            float lenFwd = UMath::Normalize(fwd);
            if (lenFwd != 0.0f) {
            }
            float dot3 = UMath::Dot(fwd, direction);
            score += futureScore + (1.0f - dot3) + (1.0f - dot3);

            if (nav2.GetSegmentInd() != previousfutures[0] &&
                nav2.GetSegmentInd() != previousfutures[1]) {
                score += 5.0f;
            }

            if (score < bestscore) {
                beststartspot = startspot;
                bestfuturespot.segmentindex = nav2.GetSegmentInd();
                bestfuturespot.nodeind = nav2.GetNodeInd();
                bestfuturespot.param = nav2.GetSegmentTime();
                bestfuturespot.laneoffset = nav2.GetLaneOffset();
                besttargetspot.segmentindex = nav1.GetSegmentInd();
                besttargetspot.nodeind = nav1.GetNodeInd();
                besttargetspot.param = nav1.GetSegmentTime();
                besttargetspot.laneoffset = nav1.GetLaneOffset();
                bestscore = score;
            }
        }
    }
}
