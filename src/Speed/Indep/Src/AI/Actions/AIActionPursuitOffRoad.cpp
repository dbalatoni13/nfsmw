#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/AI/AISteer.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// total size: 0x1
// Decl: 766 TODO move there
class speed_delay_traits {
  public:
    typedef float time_type;
    typedef float value_type;
    typedef float value_arg;

    time_type maximum_delay() const {
        return 1.0f;
    }

    value_type lerp(value_type a, value_type b, value_type l) const {
        return a + l * (b - a);
    }

    // Static members
    static const size_t sample_count;
};

template <typename T> class time_delay_filter {
    typedef T traits_type;
    typedef typename T::value_type value_type; // TODO use these
    typedef typename T::value_arg value_arg;
    typedef typename T::time_type time_type;

  public:
    void reset(float init);

    const float &at(unsigned int off) const {
        return samples[(cursor - off) & 0xF];
    }
    inline float resolution() const {
        return traits.maximum_delay() / 16;
    }

    float get_sample(time_type delay) const;
    void add_sample(float sample, time_type dt);

    time_delay_filter() {
        this->offset = 0.0f;
        this->cursor = 0;
    }

  private:
    static const size_t sample_count;

    traits_type traits;  // offset 0x0, size 0x1
    float samples[16];   // offset 0x4, size 0x40
    float lastsample;    // offset 0x44, size 0x4
    float offset;        // offset 0x48, size 0x4
    unsigned int cursor; // offset 0x4C, size 0x4
};

// total size: 0x48
class AIActionPursuitOffRoad : public AIAction, public Debugable {
  public:
    typedef struct time_delay_filter<speed_delay_traits> speed_delay_filter;

    static AIAction *Construct(struct AIActionParams *params);

    AIActionPursuitOffRoad(AIActionParams *params, float score);
    bool ShouldDoIt();
    void UpdateRoadAffinity(UMath::Vector3 &affinity);
    void UpdateSeparation(UMath::Vector3 &separation);
    void UpdateAvoidWalls(UMath::Vector3 &avoid);
    void UpdateSeek(UMath::Vector3 &seek);

    // Virtual functions
    virtual void OnDebugDraw();

    // Virtual overrides
    // IUnknown
    ~AIActionPursuitOffRoad() override {}

    // AIAction
    bool CanBeAttempted(float dT) override;
    void BeginAction(float dT) override;
    bool IsFinished() override;
    void FinishAction(float dT) override;
    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    IVehicleAI *mIVehicleAI;        // offset 0x4C, size 0x4
    IRigidBody *mIRigidBody;        // offset 0x50, size 0x4
    IVehicle *mIVehicle;            // offset 0x54, size 0x4
    IPursuitAI *mIPursuitAI;        // offset 0x58, size 0x4
    ITransmission *mITransmission;  // offset 0x5C, size 0x4
    IInput *mIInput;                // offset 0x60, size 0x4
    speed_delay_filter mSpeedDelay; // offset 0x64, size 0x50
    performance_limiter mLimiter;   // offset 0xB4, size 0x4
    bool mBrakeLeft;                // offset 0xB8, size 0x1
    float mNOSCountDown;            // offset 0xBC, size 0x4
    bool mUserNOSLastTime;          // offset 0xC0, size 0x1
};

AIActionPursuitOffRoad::AIActionPursuitOffRoad(AIActionParams *params, float score) : AIAction(params, score) {
    this->MakeDebugable(DBG_AI);
    params->mOwner->QueryInterface(&this->mIVehicleAI);
    params->mOwner->QueryInterface(&this->mIPursuitAI);
    params->mOwner->QueryInterface(&this->mIVehicle);
    params->mOwner->QueryInterface(&this->mITransmission);
    params->mOwner->QueryInterface(&this->mIInput);
    this->mIRigidBody = params->mOwner->GetRigidBody();

    static int brakeLeft = 0;
    brakeLeft++;

    this->mNOSCountDown = -1.0f;
    this->mBrakeLeft = (brakeLeft & 1) != 0;
    this->mUserNOSLastTime = false;
}

void AIActionPursuitOffRoad::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mIRigidBody);
    }
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        this->GetOwner()->QueryInterface(&this->mITransmission);
    }
}

AIAction *AIActionPursuitOffRoad::Construct(AIActionParams *params) {
    return new AIActionPursuitOffRoad(params, 0.01f);
}

bool AIActionPursuitOffRoad::ShouldDoIt() {
    AITarget *target = this->mIVehicleAI->GetTarget();
    UMath::Vector3 targetPosition = target->GetPosition();
    UMath::Vector3 carPosition = this->mIRigidBody->GetPosition();
    float distanceToTarget = UMath::Distance(carPosition, targetPosition);
    float distancelimit = 60.0f;

    IVehicleAI *targetvehicleai;
    if (target->QueryInterface(&targetvehicleai)) {
        distancelimit = UMath::Distance(targetPosition, targetvehicleai->GetCurrentRoad()->GetPosition()) + distancelimit;
    }

    if (distanceToTarget > distancelimit) {
        return false;
    }
    if (UMath::Distance(target->GetLinearVelocity(), this->mIRigidBody->GetLinearVelocity()) > KPH2MPS(140.0f)) {
        return false;
    }
    if (!this->mIVehicleAI->GetDrivableToTargetPos()) {
        return false;
    }
    if (!this->mIVehicleAI->GetDrivableToDriveToNav()) {
        return false;
    }
    return true;
}

bool AIActionPursuitOffRoad::CanBeAttempted(float dT) {
    if (this->mIVehicleAI != nullptr && this->mIPursuitAI != nullptr && this->mITransmission != nullptr && this->mIRigidBody != nullptr) {
        if (this->mIPursuitAI->GetChicken()) {
            return false;
        }
        if (!this->ShouldDoIt()) {
            return false;
        }
        return true;
    }
    return false;
}

bool AIActionPursuitOffRoad::IsFinished() {
    if (this->mIVehicleAI->GetTarget()->IsValid()) {
        return this->ShouldDoIt() == false;
    }
    return true;
}

void AIActionPursuitOffRoad::BeginAction(float dT) {
    if (this->mIVehicleAI->GetLastSpawnTime() <= 0.0f) {
        float maxSpeed = MPH2MPS(60.0f);
        this->mIVehicle->SetSpeed(maxSpeed);
    }
    WRoadNav *road_nav = this->mIVehicleAI->GetDriveToNav();
    road_nav->SetNavType(WRoadNav::kTypeDirection);
    road_nav->SetLaneType(WRoadNav::kLaneCop);
    road_nav->SetCookieTrail(true);
    road_nav->ResetCookieTrail();

    this->mIVehicleAI->ResetDriveToNav(SELECT_CURRENT_LANE);
    this->mSpeedDelay.reset(this->mIVehicleAI->GetTarget()->GetSpeed());
    this->mLimiter.init(this->mIVehicle->GetSpeed());
}

void AIActionPursuitOffRoad::FinishAction(float dT) {}

void AIActionPursuitOffRoad::UpdateRoadAffinity(UMath::Vector3 &affinity) {
    WRoadNav *drivenav = this->mIVehicleAI->GetDriveToNav();
    const NavCookie &cookie = drivenav->GetCurrentCookie();
    const UMath::Vector3 &position = this->mIRigidBody->GetPosition();
    const UMath::Vector3 &velocity = this->mIRigidBody->GetLinearVelocity();

    UMath::Vector2 side = UMath::Vector2Make(cookie.Forward.y, -cookie.Forward.x);
    UMath::Vector2 loff = UMath::Vector2Make(position.x - cookie.Left.x, position.z - cookie.Left.y);
    UMath::Vector2 roff = UMath::Vector2Make(position.x - cookie.Right.x, position.z - cookie.Right.y);

    float llen = UMath::Dot(loff, side);
    float rlen = -UMath::Dot(roff, side);
    float sidev = UMath::Dot(UMath::Vector2Make(velocity.x, velocity.z), side);
    llen += 1.0f;
    rlen += 1.0f;

    float lden = bMax(0.1f, llen);
    float rden = bMax(0.1f, rlen);
    float linvtime = bMax(0.0f, -sidev) / lden;
    float rinvtime = bMax(0.0f, sidev) / rden;

    float lscale = 0.5f / (lden * lden);
    lscale += 2.0f * linvtime * linvtime;
    lscale = bMin(KPH2MPS(100.0f), lscale);

    float rscale = 0.5f / (rden * rden);
    rscale += 2.0f * rinvtime * rinvtime;
    rscale = bMin(KPH2MPS(100.0f), rscale);

    UMath::Vector3 side3 = UMath::Vector3Make(side.x, 0.0f, side.y);
    UMath::Scale(side3, lscale, affinity);
    UMath::ScaleAdd(side3, -rscale, affinity, affinity);
}

void AIActionPursuitOffRoad::UpdateSeparation(UMath::Vector3 &separation) {
    const AvoidableList &avoidList = this->mIVehicleAI->GetAvoidableList();
    AISteer::VehicleSeperation(separation, this->mIVehicle, avoidList, 2.7f, 5.3f);
}

void AIActionPursuitOffRoad::UpdateAvoidWalls(UMath::Vector3 &avoid) {
    UMath::Vector3 velocity = this->mIRigidBody->GetLinearVelocity();
    float speed = Length(velocity);
    UMath::Vector3 position = this->mIRigidBody->GetPosition();

    if (speed < 2.0f) {
        return;
    }
    float length = bMin(80.0f, speed + 10.0f);
    UMath::Vector3 target;
    UMath::ScaleAdd(velocity, length / speed, position, target);

    UMath::Vector4 posToDest[2];
    posToDest[0] = UMath::Vector4Make(position, 1.0f);
    posToDest[1] = UMath::Vector4Make(target, 1.0f);

    WCollisionMgr::WorldCollisionInfo cinfo;
    if (WCollisionMgr(0, 3).CheckHitWorld(posToDest, cinfo, 2) == 0) {
        return;
    }
    UMath::Vector3 collidepoint = Vector4To3(cinfo.fCollidePt);
    UMath::Vector3 collidenormal = Vector4To3(cinfo.fNormal);

    float collidedist = UMath::Distance(collidepoint, position);
    float collidetime = collidedist / speed;
    float collidedot = UMath::Dot(velocity, collidenormal) / speed;

    if (collidedot >= 0.0f) {
        return;
    }
    float strength = (collidedot * collidedot * KPH2MPS(10.0f)) / (collidetime * collidetime);
    Scale(collidenormal, strength, avoid);
}

void AIActionPursuitOffRoad::OnDebugDraw() {}
