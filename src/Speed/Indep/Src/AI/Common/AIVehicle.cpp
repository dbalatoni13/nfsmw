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
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
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
#include "Speed/Indep/Src/World/WRoadElem.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
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
        // new EEnabled
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
        path_type = WRoadNav::kPathPathy;
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
    mDestSegment = -1;
    mCanRespawn = false;
    mDrivableToNav = false;
    mReverseOverrideTimer = 0.0f;
    mReverseOverrideDirection = false;
    mDrivableToTargetPos = false;
    mReversingSpeed = false;
    mReverseOverrideSteer = 0.0f;
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
                    UMath::Vector3 driveoff = UVector3(GetDriveTarget()) - GetVehicle()->GetPosition();
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

                    UMath::Vector3 car_to_target = UVector3(target_position) - car_position;
                    distance = bMax(0.0f, UMath::Dot(car_to_target, target_direction));
                }
            }
        } else {
            UMath::Vector3 nav_forward;
            UMath::Unit(road_nav->GetForwardVector(), nav_forward);

            UMath::Vector3 car_to_nav = UVector3(road_nav->GetPosition()) - car_position;
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

void AIVehicle::OnCollision(const COLLISION_INFO &cinfo) {}

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
