#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Generated/Messages/MSetTrafficSpeed.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Sim/UTil.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
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

BIND_AIACTION_FACTORY(AIActionTraffic);

AIActionTraffic::AIActionTraffic(AIActionParams *params, float score)
    : AIAction(params, score),               //
      mFixedSpeed(false),                    //
      mStopSign(false),                      //
      mClearIntersection(false),             //
      mNavMatrix(UMath::Matrix4::kIdentity), //
      mAccidentTimer(0.0f),                  //
      mAccident(ACCIDENT_NONE) {
    this->MakeDebugable(DBG_AI);

    mIsTractor = VehicleClass::TRACTOR == GetVehicle()->GetVehicleClass();
    AddListener(this, this->GetOwner(), "AIActionTraffic");

    this->mRigidBody = params->mOwner->GetRigidBody();
    params->mOwner->QueryInterface(&this->mInput);

    this->mTargetSpeedDefault = MPH2MPS(35.0f);
    this->mTargetSpeedHighway = MPH2MPS(55.0f);

    this->mSetSpeedHandler = Hermes::Handler::Create<MSetTrafficSpeed, AIActionTraffic, AIActionTraffic>(
        this, &AIActionTraffic::MessageSetSpeed, UCrc32("AIAction"), this->GetVehicle()->GetSimable()->GetWorldID());

    this->mDefaultPursuitLevelAttrib = new Attrib::Gen::pursuitlevels(Attrib::key_default, 0, nullptr);
    this->nPullOverState = eNO_PULL_OVER;
}

AIActionTraffic::~AIActionTraffic() {
    Sim::Collision::RemoveListener(this);
    if (this->mSetSpeedHandler != nullptr) {
        Hermes::Handler::Destroy(this->mSetSpeedHandler);
        this->mSetSpeedHandler = nullptr;
    }
    delete this->mDefaultPursuitLevelAttrib;
}

AIAction *AIActionTraffic::Construct(AIActionParams *params) {
    return new AIActionTraffic(params, AIACTION_SCORE_HIGH);
}

void AIActionTraffic::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mRigidBody);
    } else if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mInput);
    }
}

void AIActionTraffic::OnAccident(HSIMABLE hobject, const UMath::Vector3 &speed, const UMath::Vector3 &position) {
    if (hobject == GetOwner()->GetInstanceHandle() || Sim::DistanceToCamera(position) > 80.0f) {
        return;
    }
    ISimable *object = ISimable::FindInstance(hobject);
    IVehicle *vehicle;
    if (object != nullptr && object->QueryInterface(&vehicle) && (vehicle->GetAbsoluteSpeed() >= MPH2MPS(5.0f) || mAccident != ACCIDENT_OVER)) {
        switch (vehicle->GetDriverClass()) {
            case DRIVER_HUMAN:
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
            default:
                break;
        }
    }
}

void AIActionTraffic::OnCollision(const COLLISION_INFO &cinfo) {
    if (this->GetVehicle()->GetDriverClass() != DRIVER_COP && cinfo.type == COLLISION_INFO::OBJECT) {
        this->OnAccident(cinfo.objA, cinfo.closingVel, cinfo.position);
        this->OnAccident(cinfo.objB, cinfo.closingVel, cinfo.position);
    }
}

bool AIActionTraffic::IsFinished() {
    if (this->GetAI() != nullptr && this->GetAI()->GetPursuit() != nullptr) {
        if (this->GetAI()->GetPursuit()->IsPerpInSight()) {
            return true;
        }
    }
    return false;
}

bool AIActionTraffic::CanBeAttempted(float dT) {
    if (this->mRigidBody != nullptr && this->GetAI() != nullptr && this->GetVehicle() != nullptr && this->mInput != nullptr) {
        if (this->GetAI()->GetPursuit() != nullptr) {
            return this->GetAI()->GetPursuit()->IsPerpInSight() == false;
        }

        WRoadNav test_nav;
        const float dir_weight = 1.0f;
        const bool force_centre_lane = true;
        UMath::Vector3 forwardVector;

        this->mRigidBody->GetForwardVector(forwardVector);
        test_nav.SetNavType(WRoadNav::kTypeTraffic);
        test_nav.SetLaneType(WRoadNav::kLaneTraffic);
        test_nav.InitAtPoint(this->mRigidBody->GetPosition(), forwardVector, force_centre_lane, dir_weight);
        if (test_nav.IsValid()) {
            return true;
        }
    }
    return false;
}

void AIActionTraffic::BeginAction(float dT) {
    this->GetAI()->GetDriveToNav()->SetNavType(WRoadNav::kTypeTraffic);
    this->GetAI()->GetDriveToNav()->SetLaneType(WRoadNav::kLaneTraffic);
    this->GetAI()->ResetDriveToNav(SELECT_VALID_LANE);
    this->mAccidentTimer = 0.0f;
    this->mAccident = ACCIDENT_NONE;
    this->UpdateNavPos(30.0f);
    if (this->GetAI()->GetLastSpawnTime() > 0.0f && this->GetAI() != nullptr && this->GetAI()->GetPursuit() != nullptr &&
        !this->GetAI()->GetPursuit()->IsPerpInSight()) {
        this->mTargetSpeedDefault = MPH2MPS(this->mDefaultPursuitLevelAttrib->SearchModeCityMPH());
        this->mTargetSpeedHighway = MPH2MPS(this->mDefaultPursuitLevelAttrib->SearchModeHwyMPH());
    }
}

void AIActionTraffic::FinishAction(float dT) {}

float aAIStoppingDist[2] = {3.0f, 50.0f};
Table aAIStoppingDistTable(aAIStoppingDist, 2, 0.0f, 1.0f);

float GetSpeedLimitForCurvature(float friction, float curvature, float top_speed);

float AIActionTraffic::ComputeSpeed(float current_speed, float dT) {
    WRoadNav *road_nav = this->GetAI()->GetDriveToNav();

    if (road_nav->HitDeadEnd()) {
        return 0.0f;
    }

    road_nav = this->GetAI()->GetDriveToNav();
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(road_nav->GetSegmentInd());
    bool is_cop = this->GetVehicle()->GetDriverClass() == DRIVER_COP;

    const float posted_speed = roadNetwork.GetSegmentNumTrafficLanes(*segment) >= 4 ? this->mTargetSpeedHighway : this->mTargetSpeedDefault;
    float desired_speed = posted_speed;

    if (!this->mFixedSpeed) {
        float curvature =
            this->GetAI()->GetDriveToNav()->CookieTrailCurvature(this->mRigidBody->GetPosition(), this->mRigidBody->GetLinearVelocity());
        float speed_limit = GetSpeedLimitForCurvature(is_cop ? 1.6f : 0.6f, curvature, posted_speed);
        desired_speed = bMin(posted_speed, speed_limit);

        if (road_nav->IsOccludedByAvoidable() && !road_nav->IsOccludedFromBehind()) {
            float mass = this->mRigidBody->GetMass();
            if (this->mIsTractor) {
                mass = mass + mass;
            }

            float my_length = this->mRigidBody->GetRadius();
            my_length = my_length + my_length;

            float dist_to_occlusion = UMath::Max(UMath::Distance(road_nav->GetApexPosition(), this->mRigidBody->GetPosition()) - my_length, 0.0f);

            float stopping_distance = aAIStoppingDistTable.GetValue(current_speed * UMath::Max(mass * 0.0005f, 1.0f));

            if (dist_to_occlusion < stopping_distance) {
                desired_speed = bClamp(road_nav->GetOccludingTrailSpeed() * dist_to_occlusion / stopping_distance, 0.0f, desired_speed);
                desired_speed = bMin(desired_speed, current_speed);
            }
        }
    }

    if (!is_cop) {
        desired_speed = bMin(desired_speed, current_speed + (dT + dT));
    }

    return desired_speed;
}

void AIActionTraffic::UpdateNavPos(float lookAheadDistance) {
    WRoadNav *nav_point = this->GetAI()->GetDriveToNav();
    bool pull_over = this->nPullOverState != 0;

    if (!pull_over) {
        if (!nav_point->HitDeadEnd()) {
            UMath::Vector3 navPos = nav_point->GetPosition();
            UMath::Vector3 carPosition = this->mRigidBody->GetPosition();
            UMath::Vector3 carToNav;
            UMath::Vector3 navForwardVector = nav_point->GetForwardVector();

            UMath::Unit(navForwardVector);
            UMath::Sub(navPos, carPosition, carToNav);

            float length = UMath::Normalize(carToNav);
            float distToNav = length * UMath::Dot(navForwardVector, carToNav);

            UMath::Vector3 targetVec = UMath::Vector3::kZero;
            AITarget *aitarget = this->GetAI()->GetTarget();

            if (aitarget != nullptr && aitarget->IsValid()) {
                UMath::Vector3 targetPos = aitarget->GetPosition();
                UMath::Sub(targetPos, carPosition, targetVec);
                UMath::Normalize(targetVec);
            }

            if (distToNav < lookAheadDistance) {
                nav_point->IncNavPosition(lookAheadDistance - distToNav, targetVec, lookAheadDistance);
            }
        }

        nav_point->UpdateOccludedPosition(true);
    }

    UMath::Copy(UMath::Matrix4::kIdentity, this->mNavMatrix);

    UMath::Vector3 nav_direction = nav_point->GetForwardVector();

    if (UMath::Normalize(nav_direction) > 0.01f) {
        this->mNavMatrix = Util_GenerateMatrix(nav_direction, nullptr);

        this->mNavMatrix.v3 = UMath::Vector4Make(!pull_over ? nav_point->GetOccludedPosition() : nav_point->GetPosition(), 1.0f);
    }
}

bool AIActionTraffic::ShouldPullOver(const UMath::Vector3 &my_position, WRoadNav *road_nav) {
    return false;
}

void AIActionTraffic::Update(float dT) {
    WRoadNav *road_nav = this->GetAI()->GetDriveToNav();
    IRigidBody *irb = this->GetOwner()->GetRigidBody();

    float current_speed = irb->GetSpeed();
    float t = UMath::Ramp(current_speed, 0.0f, 25.0f);
    if (road_nav->IsOccludedByAvoidable() != 0) {
        t = 1.0f;
    }
    float lookAheadDistance = UMath::Lerp(10.0f, 30.0f, t);
    lookAheadDistance += current_speed * dT;
    lookAheadDistance += irb->GetRadius();

    bool do_driving = true;

    switch (this->nPullOverState) {
        case eNO_PULL_OVER:
            if (this->ShouldPullOver(irb->GetPosition(), road_nav)) {
                this->nPullOverState = ePULLING_OVER;
                road_nav->PullOver();
            }
            break;
        case ePULLING_OVER: {
            UMath::Vector3 nav_forward;
            UMath::Unit(road_nav->GetForwardVector(), nav_forward);
            UMath::Vector3 nav_to_car = irb->GetPosition() - road_nav->GetPosition();
            UMath::Normalize(nav_to_car);
            if (UMath::Dot(nav_to_car, nav_forward) > (-dT * current_speed) * 0.5f) {
                this->nPullOverState = ePULLED_OVER;
            }
            break;
        }
        case ePULLED_OVER:
            if (!this->ShouldPullOver(irb->GetPosition(), road_nav)) {
                this->nPullOverState = eNO_PULL_OVER;
                this->GetAI()->ResetDriveToNav(SELECT_CURRENT_LANE);
            } else {
                if (this->mInput != nullptr) {
                    this->mInput->SetControlGas(0.0f);
                    this->mInput->SetControlBrake(1.0f);
                    this->mInput->SetControlSteering(0.0f);
                }
                do_driving = false;
            }
            break;
    }

    this->UpdateNavPos(lookAheadDistance);
    this->GetAI()->SetAvoidableRadius(lookAheadDistance);
    float desired_speed = this->ComputeSpeed(current_speed, dT);

    if (road_nav->IsValid()) {
        this->GetAI()->SetDriveTarget(Vector4To3(this->mNavMatrix.v3));
        this->GetAI()->SetDriveSpeed(desired_speed);
    } else {
        this->GetAI()->SetDriveSpeed(0.0f);
    }

    if (this->mAccident == ACCIDENT_INPROGRESS) {
        this->mAccidentTimer = this->mAccidentTimer - 1.0f;
        if (this->mAccidentTimer <= 0.0f) {
            this->mAccidentTimer = 0.0f;
            this->mAccident = ACCIDENT_OVER;
        }
        if (this->mInput != nullptr) {
            this->mInput->SetControlGas(0.0f);
            this->mInput->SetControlBrake(0.0f);
            this->mInput->SetControlHandBrake(0.0f);
            if (!this->mIsTractor) {
                this->mInput->SetControlSteering(1.0f);
            }
        }
        do_driving = false;
    } else if (this->mAccident == ACCIDENT_OVER) {
        if (this->mInput != nullptr) {
            this->mInput->SetControlGas(0.0f);
            this->mInput->SetControlBrake(1.0f);
            if (!this->mIsTractor) {
                this->mInput->SetControlSteering(1.0f);
            }
        }
        do_driving = false;
    }

    if (do_driving) {
        this->GetAI()->DoDriving(3);
    }
}

void AIActionTraffic::OnDebugDraw() {}

void AIActionTraffic::MessageSetSpeed(const MSetTrafficSpeed &message) {
    if (this->GetVehicle() != nullptr && message.GetID() == this->GetVehicle()->GetSimable()->GetWorldID()) {
        this->mTargetSpeedDefault = MPH2MPS(message.GetSpeedDefault());
        this->mTargetSpeedHighway = MPH2MPS(message.GetSpeedHighway());
        this->mFixedSpeed = message.GetFixSpeed() != 0;
    }
}
