#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aivehicle.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

const char *GetCaffeineLayerName(int driver_class);

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
    mDestSegment = -1;
    mReverseOverrideTimer = 0.0f;
    mReverseOverrideSteer = 0.0f;
    mCanRespawn = false;
    mDrivableToNav = false;
    mLastSpawnTime = 0.0f;
    mReverseOverrideDirection = false;
    mDrivableToTargetPos = false;
    mReversingSpeed = false;
    mSteeringBehind = false;
}
