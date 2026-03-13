#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Generated/Messages/MSetTrafficSpeed.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Sim/Util.h"
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

UTL::COM::Factory<AIActionParams *, AIAction, UCrc32>::Prototype _AIActionTraffic("AIActionTraffic", AIActionTraffic::Construct);

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

float GetSpeedLimitForCurvature(float friction, float curvature, float top_speed);
extern Table aAIStoppingDistTable;

float AIActionTraffic::ComputeSpeed(float current_speed, float dT) {
    WRoadNav *road_nav = GetAI()->GetDriveToNav();

    if (road_nav->HitDeadEnd()) {
        return 0.0f;
    }

    road_nav = GetAI()->GetDriveToNav();
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(road_nav->GetSegmentInd());
    bool is_cop = GetVehicle()->GetDriverClass() == DRIVER_COP;

    int numLanes = roadNetwork.GetSegmentNumTrafficLanes(*segment);
    float posted_speed;
    if (numLanes >= 4) {
        posted_speed = mTargetSpeedHighway;
    } else {
        posted_speed = mTargetSpeedDefault;
    }
    float desired_speed = posted_speed;

    if (!mFixedSpeed) {
        float curvature = GetAI()->GetDriveToNav()->CookieTrailCurvature(
            mRigidBody->GetPosition(), mRigidBody->GetLinearVelocity());
        float friction = 0.6f;
        if (is_cop) {
            friction = 1.6f;
        }
        float speed_limit = GetSpeedLimitForCurvature(friction, curvature, posted_speed);
        desired_speed = bMin(posted_speed, speed_limit);

        if (road_nav->IsOccludedByAvoidable() && !road_nav->IsOccludedFromBehind()) {
            float mass = mRigidBody->GetMass();
            if (mIsTractor) {
                mass = mass + mass;
            }
            float my_length = mRigidBody->GetRadius();
            my_length = my_length + my_length;
            float dist_to_occlusion = UMath::Distance(road_nav->GetApexPosition(),
                                                       mRigidBody->GetPosition());
            dist_to_occlusion = UMath::Max(0.0f, dist_to_occlusion - my_length);
            float speed_factor = UMath::Max(mass * 0.0005f, 1.0f);
            float stopping_distance = aAIStoppingDistTable.GetValue(current_speed * speed_factor);

            if (dist_to_occlusion < stopping_distance) {
                float ratio = road_nav->GetOccludingTrailSpeed() * dist_to_occlusion /
                              stopping_distance;
                ratio = bClamp(ratio, 0.0f, desired_speed);
                desired_speed = bMin(ratio, current_speed);
            }
        }
    }

    if (!is_cop) {
        desired_speed = bMin(desired_speed, current_speed + (dT + dT));
    }

    return desired_speed;
}

void AIActionTraffic::UpdateNavPos(float lookAheadDistance) {
    WRoadNav *nav_point = GetAI()->GetDriveToNav();
    bool pull_over = nPullOverState == 0;

    if (pull_over) {
        if (!nav_point->HitDeadEnd()) {
            UMath::Vector3 navPos = nav_point->GetPosition();
            UMath::Vector3 carPosition = mRigidBody->GetPosition();
            UMath::Vector3 carToNav;
            UMath::Vector3 navForwardVector = nav_point->GetForwardVector();
            UMath::Unit(navForwardVector, navForwardVector);
            UMath::Sub(navPos, carPosition, carToNav);
            float distToNav = UMath::Normalize(carToNav);
            float dot = UMath::Dot(navForwardVector, carToNav);

            UMath::Vector3 targetVec = UMath::Vector3::kZero;
            AITarget *aitarget = GetAI()->GetTarget();
            if (aitarget != nullptr && aitarget->IsValid()) {
                UMath::Vector3 targetPos = aitarget->GetPosition();
                UMath::Sub(targetPos, carPosition, targetVec);
                UMath::Normalize(targetVec);
            }

            if (distToNav * dot < lookAheadDistance) {
                nav_point->IncNavPosition(lookAheadDistance - distToNav * dot, targetVec,
                                          lookAheadDistance);
            }
        }
        nav_point->UpdateOccludedPosition(true);
    }

    UMath::Copy(UMath::Matrix4::kIdentity, mNavMatrix);

    UMath::Vector3 nav_direction = nav_point->GetForwardVector();
    float length = UMath::Normalize(nav_direction);

    if (length > 0.01f) {
        UMath::Matrix4 orientMat = Util_GenerateMatrix(nav_direction, nullptr);
        UMath::Copy(orientMat, mNavMatrix);

        const UMath::Vector3 &pos = pull_over
            ? nav_point->GetOccludedPosition()
            : nav_point->GetPosition();
        mNavMatrix.v3 = UMath::Vector4Make(pos, 1.0f);
    }
}

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
