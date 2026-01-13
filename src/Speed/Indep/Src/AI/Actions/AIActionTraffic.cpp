#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Generated/Messages/MSetTrafficSpeed.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// total size: 0x48
class AIActionTraffic : public AIAction, public Debugable, public Sim::Collision::IListener {
  public:
    enum eAccident {
        ACCIDENT_NONE = 0,
        ACCIDENT_INPROGRESS = 1,
        ACCIDENT_OVER = 2,
    };
    enum PullOverState {
        eNO_PULL_OVER = 0,
        ePULLING_OVER = 1,
        ePULLED_OVER = 2,
    };

    static AIAction *Construct(struct AIActionParams *params);

    AIActionTraffic(AIActionParams *params, float score);
    void MessageSetSpeed(const MSetTrafficSpeed &message);
    float ComputeSpeed(float current_speed, float dT);
    void UpdateNavPos(float lookAheadDistance);
    bool ShouldPullOver(const UMath::Vector3 &my_position, WRoadNav *road_nav);
    void OnAccident(HSIMABLE hobject, const UMath::Vector3 &speed, const UMath::Vector3 &position);

    // Virtual functions
    virtual void OnDebugDraw();

    // Virtual overrides
    // IUnknown
    ~AIActionTraffic() override;

    // AIAction
    bool CanBeAttempted(float dT) override;
    void BeginAction(float dT) override;
    bool IsFinished() override;
    void FinishAction(float dT) override;
    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // IListener
    void OnCollision(const COLLISION_INFO &cinfo) override;

  private:
    bool mStopSign;                                         // offset 0x50, size 0x1
    bool mClearIntersection;                                // offset 0x54, size 0x1
    bool mFixedSpeed;                                       // offset 0x58, size 0x1
    float mTargetSpeedDefault;                              // offset 0x5C, size 0x4
    float mTargetSpeedHighway;                              // offset 0x60, size 0x4
    bool mIsTractor;                                        // offset 0x64, size 0x1
    IRigidBody *mRigidBody;                                 // offset 0x68, size 0x4
    IInput *mInput;                                         // offset 0x6C, size 0x4
    Hermes::HHANDLER mSetSpeedHandler;                      // offset 0x70, size 0x4
    Attrib::Gen::pursuitlevels *mDefaultPursuitLevelAttrib; // offset 0x74, size 0x4
    UMath::Matrix4 mNavMatrix;                              // offset 0x78, size 0x40
    eAccident mAccident;                                    // offset 0xB8, size 0x4
    float mAccidentTimer;                                   // offset 0xBC, size 0x4
    PullOverState nPullOverState;                           // offset 0xC0, size 0x4
};

AIActionTraffic::AIActionTraffic(AIActionParams *params, float score)
    : AIAction(params, score),               //
      mFixedSpeed(false),                    //
      mStopSign(false),                      //
      mClearIntersection(false),             //
      mNavMatrix(UMath::Matrix4::kIdentity), //
      mAccidentTimer(0.0f),                  //
      mAccident(ACCIDENT_NONE) {
    MakeDebugable(DBG_AI);

    mIsTractor = VehicleClass::TRACTOR == GetVehicle()->GetVehicleClass();
    AddListener(this, GetOwner(), "AIActionTraffic");

    mRigidBody = params->mOwner->GetRigidBody();
    params->mOwner->QueryInterface(&mInput);

    mTargetSpeedDefault = MPH2MPS(35.0f);
    mTargetSpeedHighway = MPH2MPS(55.0f);

    mSetSpeedHandler = Hermes::Handler::Create<MSetTrafficSpeed, AIActionTraffic, AIActionTraffic>(
        this, &AIActionTraffic::MessageSetSpeed, "AIAction", GetVehicle()->GetSimable()->GetWorldID());

    // "default"
    mDefaultPursuitLevelAttrib = new Attrib::Gen::pursuitlevels(0xeec2271a, 0, nullptr);
    nPullOverState = eNO_PULL_OVER;
}

AIActionTraffic::~AIActionTraffic() {
    Sim::Collision::RemoveListener(this);
    if (mSetSpeedHandler) {
        Hermes::Handler::Destroy(mSetSpeedHandler);
        mSetSpeedHandler = nullptr;
    }
    delete mDefaultPursuitLevelAttrib;
}

AIAction *AIActionTraffic::Construct(AIActionParams *params) {
    return new AIActionTraffic(params, 0.1f);
}

void AIActionTraffic::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mRigidBody);
    } else if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mInput);
    }
}

void AIActionTraffic::OnAccident(HSIMABLE hobject, const UMath::Vector3 &speed, const UMath::Vector3 &position) {
    if (hobject == GetOwner()->GetInstanceHandle() || Sim::DistanceToCamera(position) > 80.0f) {
        return;
    }
    ISimable *object = ISimable::FindInstance(hobject);
    IVehicle *vehicle;
    if (object && object->QueryInterface(&vehicle) && (vehicle->GetAbsoluteSpeed() >= MPH2MPS(5.0f) || mAccident != ACCIDENT_OVER)) {
        switch (vehicle->GetDriverClass()) {
            // TODO
            case DRIVER_HUMAN:
            case DRIVER_TRAFFIC:
            case DRIVER_COP:
            case DRIVER_RACER:
                if (mIsTractor) {
                    mAccident = ACCIDENT_OVER;
                    mAccidentTimer = 0.0f;
                } else {
                    mAccident = ACCIDENT_INPROGRESS;
                    mAccidentTimer = 3.0f;
                }
                break;
            case DRIVER_NONE:
                break;
            case DRIVER_NIS:
                break;
            case DRIVER_REMOTE:
                break;
        }
    }
}

void AIActionTraffic::OnCollision(const COLLISION_INFO &cinfo) {
    if (GetVehicle()->GetDriverClass() != DRIVER_COP && cinfo.type == COLLISION_INFO::OBJECT) {
        OnAccident(cinfo.objA, cinfo.closingVel, cinfo.position);
        OnAccident(cinfo.objB, cinfo.closingVel, cinfo.position);
    }
}

bool AIActionTraffic::IsFinished() {
    if (GetAI() && GetAI()->GetPursuit()) {
        if (GetAI()->GetPursuit()->IsPerpInSight()) {
            return true;
        }
    }
    return false;
}

bool AIActionTraffic::CanBeAttempted(float dT) {
    if (mRigidBody && GetAI() && GetVehicle() && mInput) {
        if (GetAI()->GetPursuit()) {
            return GetAI()->GetPursuit()->IsPerpInSight() == false;
        }

        WRoadNav test_nav;
        const float dir_weight = 1.0f;
        const bool force_centre_lane = true;
        UMath::Vector3 forwardVector;

        mRigidBody->GetForwardVector(forwardVector);
        test_nav.SetNavType(WRoadNav::kTypeTraffic);
        test_nav.SetLaneType(WRoadNav::kLaneTraffic);
        test_nav.InitAtPoint(mRigidBody->GetPosition(), forwardVector, force_centre_lane, dir_weight);
        if (test_nav.IsValid()) {
            return true;
        }
    }
    return false;
}

void AIActionTraffic::BeginAction(float dT) {
    GetAI()->GetDriveToNav()->SetNavType(WRoadNav::kTypeTraffic);
    GetAI()->GetDriveToNav()->SetLaneType(WRoadNav::kLaneTraffic);
    GetAI()->ResetDriveToNav(SELECT_VALID_LANE);
    mAccidentTimer = 0.0f;
    mAccident = ACCIDENT_NONE;
    UpdateNavPos(30.0f);
    if (GetAI()->GetLastSpawnTime() > 0.0f && GetAI() && GetAI()->GetPursuit() && !GetAI()->GetPursuit()->IsPerpInSight()) {
        mTargetSpeedDefault = MPH2MPS(mDefaultPursuitLevelAttrib->SearchModeCityMPH());
        mTargetSpeedHighway = MPH2MPS(mDefaultPursuitLevelAttrib->SearchModeHwyMPH());
    }
}

void AIActionTraffic::FinishAction(float dT) {}

bool AIActionTraffic::ShouldPullOver(const UMath::Vector3 &my_position, WRoadNav *road_nav) {
    return false;
}

void AIActionTraffic::Update(float dT) {
    WRoadNav *road_nav = GetAI()->GetDriveToNav();
    IRigidBody *irb = GetOwner()->GetRigidBody();

    float current_speed = irb->GetSpeed();
    float t = UMath::Ramp(current_speed, 0.0f, 25.0f);
    if (road_nav->IsOccludedByAvoidable() != 0) {
        t = 1.0f;
    }
    float lookAheadDistance = UMath::Lerp(10.0f, 30.0f, t);
    lookAheadDistance += current_speed * dT;
    lookAheadDistance += irb->GetRadius();

    bool do_driving = true;

    switch (nPullOverState) {
        case eNO_PULL_OVER:
            if (ShouldPullOver(irb->GetPosition(), road_nav)) {
                nPullOverState = ePULLING_OVER;
                road_nav->PullOver();
            }
            break;
        case ePULLING_OVER: {
            UMath::Vector3 nav_forward;
            UMath::Unit(road_nav->GetForwardVector(), nav_forward);
            UMath::Vector3 nav_to_car = irb->GetPosition() - road_nav->GetPosition();
            UMath::Normalize(nav_to_car);
            if (UMath::Dot(nav_to_car, nav_forward) > (-dT * current_speed) * 0.5f) {
                nPullOverState = ePULLED_OVER;
            }
            break;
        }
        case ePULLED_OVER:
            if (!ShouldPullOver(irb->GetPosition(), road_nav)) {
                nPullOverState = eNO_PULL_OVER;
                GetAI()->ResetDriveToNav(SELECT_CURRENT_LANE);
            } else {
                if (mInput) {
                    mInput->SetControlGas(0.0f);
                    mInput->SetControlBrake(1.0f);
                    mInput->SetControlSteering(0.0f);
                }
                do_driving = false;
            }
            break;
    }

    UpdateNavPos(lookAheadDistance);
    GetAI()->SetAvoidableRadius(lookAheadDistance);
    float desired_speed = ComputeSpeed(current_speed, dT);

    if (road_nav->IsValid()) {
        GetAI()->SetDriveTarget(Vector4To3(mNavMatrix.v3));
        GetAI()->SetDriveSpeed(desired_speed);
    } else {
        GetAI()->SetDriveSpeed(0.0f);
    }

    if (mAccident == ACCIDENT_INPROGRESS) {
        mAccidentTimer = mAccidentTimer - 1.0f;
        if (mAccidentTimer <= 0.0f) {
            mAccidentTimer = 0.0f;
            mAccident = ACCIDENT_OVER;
        }
        if (mInput) {
            mInput->SetControlGas(0.0f);
            mInput->SetControlBrake(0.0f);
            mInput->SetControlHandBrake(0.0f);
            if (!mIsTractor) {
                mInput->SetControlSteering(1.0f);
            }
        }
        do_driving = false;
    } else if (mAccident == ACCIDENT_OVER) {
        if (mInput) {
            mInput->SetControlGas(0.0f);
            mInput->SetControlBrake(1.0f);
            if (!mIsTractor) {
                mInput->SetControlSteering(1.0f);
            }
        }
        do_driving = false;
    }

    if (do_driving) {
        GetAI()->DoDriving(3);
    }
}

void AIActionTraffic::OnDebugDraw() {}

void AIActionTraffic::MessageSetSpeed(const MSetTrafficSpeed &message) {
    if (GetVehicle() && message.GetID() == GetVehicle()->GetSimable()->GetWorldID()) {
        mTargetSpeedDefault = MPH2MPS(message.GetSpeedDefault());
        mTargetSpeedHighway = MPH2MPS(message.GetSpeedHighway());
        mFixedSpeed = message.GetFixSpeed() != 0;
    }
}
