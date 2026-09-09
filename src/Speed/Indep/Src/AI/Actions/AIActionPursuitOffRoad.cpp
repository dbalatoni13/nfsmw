#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/AI/AISteer.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

static const float kNOSDelay = 1.5f; // Decl: 25

static const float CopRoadAffinityStaticScale = 0.5f;  // Decl: 216
static const float CopRoadAffinityDynamicScale = 2.0f; // Decl: 217

static const float CopAvoidableStaticScale = 2.7f;  // Decl: 249
static const float CopAvoidableDynamicScale = 5.3f; // Decl: 250

// Decl: 690
template <typename T> class time_delay_filter {
    typedef T traits_type;
    typedef typename T::value_type value_type; // TODO use these
    typedef typename T::value_arg value_arg;
    typedef typename T::time_type time_type;

  public:
    time_delay_filter() {
        this->offset = 0.0f;
        this->cursor = 0;
    }

    void reset(float init) {
        this->cursor = 0;
        this->offset = 0.0f;
        this->lastsample = init;
        for (unsigned int i = 0; i < sample_count; i++) {
            this->samples[i] = init;
        }
    }

    void add_sample(float sample, time_type dt) {
        unsigned int count = static_cast<unsigned int>(bFloor((dt + this->offset) / this->resolution()));

        for (unsigned int i = 0; i < count; i++) {
            this->cursor = (this->cursor + 1) & 0xF;
            float l = (static_cast<float>(i + 1) * this->resolution() - this->offset) / dt;
            this->samples[this->cursor] = this->traits.lerp(this->lastsample, sample, l);
        }

        this->offset += dt - static_cast<float>(count) * this->resolution();
        this->lastsample = sample;
    }

    float get_sample(time_type delay) const {
        if (delay <= this->offset) {
            if (this->offset < 0.0001f) {
                return this->lastsample;
            } else {
                return this->traits.lerp(this->at(0), this->lastsample, (this->offset - delay) / this->offset);
            }
        }

        unsigned int o = static_cast<unsigned int>(bFloor((delay - this->offset) / this->resolution()));

        if (o + 1 >= sample_count) {
            return this->at(sample_count - 1);
        }

        return this->traits.lerp(this->at(o + 1), this->at(o),
                                 (static_cast<float>(o + 1) * this->resolution() - delay + this->offset) / this->resolution());
    }

  private:
    const float &at(unsigned int off) const {
        return this->samples[(this->cursor + sample_count - off) & 0xF];
    }

    inline float resolution() const {
        return traits.maximum_delay() / sample_count;
    }

    static const size_t sample_count = T::sample_count;

    traits_type traits;          // offset 0x0, size 0x1
    float samples[sample_count]; // offset 0x4, size 0x40
    float lastsample;            // offset 0x44, size 0x4
    float offset;                // offset 0x48, size 0x4
    size_t cursor;               // offset 0x4C, size 0x4
};

// total size: 0x1
// Decl: 766
class speed_delay_traits {
  public:
    typedef float time_type;
    typedef float value_type;
    typedef float value_arg;

    time_type maximum_delay() const {
        return 2.0f;
    }

    value_type lerp(value_type a, value_type b, value_type l) const {
        return a + l * (b - a);
    }

    static const size_t sample_count = 16;
};

// TODO are these actually needed here to make it not inline?
template void time_delay_filter<speed_delay_traits>::reset(float);
template float time_delay_filter<speed_delay_traits>::get_sample(time_type) const;
template void time_delay_filter<speed_delay_traits>::add_sample(float, time_type);

// total size: 0x48
// Decl: 781
class AIActionPursuitOffRoad : public AIAction, public Debugable {
  public:
    typedef time_delay_filter<speed_delay_traits> speed_delay_filter;

    AIActionPursuitOffRoad(AIActionParams *params, float score);
    ~AIActionPursuitOffRoad() override {}

    static AIAction *Construct(AIActionParams *params);

    // AIAction
    bool CanBeAttempted(float dT) override;
    bool IsFinished() override;
    void BeginAction(float dT) override;
    void FinishAction(float dT) override;
    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    virtual void OnDebugDraw();

  private:
    void UpdateAvoidWalls(UMath::Vector3 &avoid);
    void UpdateSeek(UMath::Vector3 &seek);
    void UpdateSeparation(UMath::Vector3 &separation);
    void UpdateRoadAffinity(UMath::Vector3 &affinity);
    bool ShouldDoIt();

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

BIND_AIACTION_FACTORY(AIActionPursuitOffRoad);

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

    float lscale = CopRoadAffinityStaticScale / (lden * lden);
    lscale += CopRoadAffinityDynamicScale * linvtime * linvtime;
    lscale = bMin(KPH2MPS(100.0f), lscale);

    float rscale = CopRoadAffinityStaticScale / (rden * rden);
    rscale += CopRoadAffinityDynamicScale * rinvtime * rinvtime;
    rscale = bMin(KPH2MPS(100.0f), rscale);

    UMath::Vector3 side3 = UMath::Vector3Make(side.x, 0.0f, side.y);
    UMath::Scale(side3, lscale, affinity);
    UMath::ScaleAdd(side3, -rscale, affinity, affinity);
}

void AIActionPursuitOffRoad::UpdateSeparation(UMath::Vector3 &separation) {
    const AvoidableList &avoidList = this->mIVehicleAI->GetAvoidableList();
    AISteer::VehicleSeperation(separation, this->mIVehicle, avoidList, CopAvoidableStaticScale, CopAvoidableDynamicScale);
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

void AIActionPursuitOffRoad::UpdateSeek(UMath::Vector3 &seek) {
    UMath::Vector3 position = this->mIRigidBody->GetPosition();
    UMath::Vector3 forwardVector;
    this->mIRigidBody->GetForwardVector(forwardVector);

    AITarget *target = this->mIVehicleAI->GetTarget();
    UMath::Vector3 targetPosition = target->GetPosition();
    UMath::Vector3 targetVelocity = target->GetLinearVelocity();
    UMath::Vector3 targetDirection = targetVelocity;

    if (target->GetSpeed() < 1.0f) {
        target->GetForwardVector(targetDirection);
    } else {
        UMath::Normalize(targetDirection);
    }

    IPerpetrator *iperp;
    target->QueryInterface(&iperp);

    IVehicleAI *itargetai;
    target->QueryInterface(&itargetai);

    UMath::Vector3 seekcenter;
    UMath::Vector3 seekdirection;
    UMath::Vector3 seekfuturecenter;

    WRoadNav *startnav = itargetai->GetCurrentRoad();
    WRoadNav *endnav = itargetai->GetFutureRoad();

    if (startnav->IsValid() && endnav->IsValid()) {
        seekcenter = startnav->GetPosition();
        seekfuturecenter = endnav->GetPosition();
        seekdirection = UVector3(seekfuturecenter) - seekcenter;
    } else {
        seekcenter = targetPosition;
        seekdirection = targetDirection;
        UMath::Add(targetPosition, targetVelocity, seekfuturecenter);
    }

    UMath::Vector3 offset = this->mIPursuitAI->GetPursuitOffset();
    UMath::Vector3 seekside = UMath::Vector3Make(seekdirection.z, 0.0f, -seekdirection.x);

    UMath::Normalize(seekside);
    UMath::Normalize(seekdirection);

    UMath::Vector3 seekPosition;
    UMath::ScaleAdd(seekdirection, offset.z, seekcenter, seekPosition);
    UMath::ScaleAdd(seekside, offset.x, seekPosition, seekPosition);

    UMath::Vector3 seekFuturePosition;
    UMath::ScaleAdd(seekdirection, offset.z, seekfuturecenter, seekFuturePosition);
    UMath::ScaleAdd(seekside, offset.x, seekFuturePosition, seekFuturePosition);

    float delay_time = iperp->GetPursuitLevelAttrib()->SpeedReactionTime();
    float delay_speed = this->mSpeedDelay.get_sample(delay_time);

    UMath::Vector3 roadoff;
    UMath::Sub(seekFuturePosition, seekPosition, roadoff);
    float roadlen = UMath::Length(roadoff);

    if (0.00001f < roadlen) {
        UMath::Scale(roadoff, delay_speed / roadlen);
        roadlen = delay_speed;
    }

    UMath::Vector3 simpleoff = targetVelocity;
    float simplelen = UMath::Length(targetVelocity);

    if (0.00001f < simplelen) {
        UMath::Scale(simpleoff, delay_speed / simplelen);
    }

    UMath::Vector3 seekoff;
    UMath::Sub(seekPosition, position, seekoff);
    float seeklen = UMath::Length(seekoff);

    if (0.00001f < seeklen) {
        float seekdir = UMath::Dot(seekoff, seekdirection);
        seekdir /= seeklen;

        float rscale = bMax(KPH2MPS(20.0f), delay_speed * 0.2f);
        rscale = bMin(1.0f, rscale / seeklen);

        float fscale;
        float slide = bClamp(seekdir * 10.0f, -1.0f, 1.0f);
        slide = slide * 0.5f + 0.5f;

        fscale = 1.8f;
        float seekscale = slide * fscale + (1.0f - slide) * rscale;

        if (0.00001f <= roadlen) {
            UMath::Vector3 paraseek;
            UMath::Scale(roadoff, UMath::Dot(seekoff, roadoff) / (roadlen * roadlen), paraseek);

            UMath::Vector3 perpseek = UVector3(seekoff) - paraseek;
            UMath::ScaleAdd(paraseek, seekscale, perpseek, seekoff);
        }
    }

    float tether_weight = this->mIVehicleAI->GetAttributes().TETHER_WEIGHT() * 0.01f;

    UMath::Vector3 blendedpos;
    Lerp(roadoff, simpleoff, tether_weight, blendedpos);
    UMath::Add(blendedpos, seekoff);
    UMath::Add(blendedpos, position);

    bool resetdrivenav;
    resetdrivenav = true;

    WRoadNav *drivenav = this->mIVehicleAI->GetDriveToNav();
    drivenav->SetRaceFilter(iperp->IsRacing());

    if (drivenav->IsValid()) {
        drivenav->SetLaneType(iperp->GetHeat() >= 3.0f ? WRoadNav::kLaneCopReckless : WRoadNav::kLaneCop);

        UMath::Vector3 drivepos = drivenav->GetPosition();
        UMath::Vector3 drivedir = drivenav->GetForwardVector();
        UMath::Normalize(drivedir);

        UMath::Vector3 endoff;
        UMath::Sub(blendedpos, drivepos, endoff);
        float incdelta = UMath::Dot(endoff, drivedir);

        if (incdelta >= -5.0f) {
            UMath::Vector3 enddir = seekdirection;
            UMath::Normalize(enddir);

            const float inc_quantum = 5.0f;

            while (inc_quantum < incdelta) {
                drivenav->IncNavPosition(inc_quantum, enddir, 0.0f);

                drivepos = drivenav->GetPosition();
                drivedir = drivenav->GetForwardVector();
                UMath::Normalize(drivedir);

                UMath::Sub(blendedpos, drivepos, endoff);
                incdelta = UMath::Dot(endoff, drivedir);
            }

            incdelta = UMath::Max(0.1f, incdelta);

            UMath::Vector3 driveside;
            driveside = UMath::Vector3Make(drivedir.z, 0.0f, -drivedir.x);
            UMath::Normalize(driveside);

            float lanedelta = UMath::Dot(endoff, driveside);
            float laneoffset = lanedelta + drivenav->GetLaneOffset();
            float snaplaneoffset = drivenav->SnapToSelectableLane(laneoffset);

            if (bAbs(snaplaneoffset - laneoffset) < 4.0f) {
                drivenav->ChangeLanes(snaplaneoffset, 0.0f);
                resetdrivenav = false;

                drivenav->IncNavPosition(incdelta, enddir, 0.0f);
                drivenav->UpdateOccludedPosition(true);

                UMath::Vector3 occludedpos = drivenav->GetOccludedPosition();
                UMath::Vector3 occludedoff;
                UMath::Sub(occludedpos, position, occludedoff);

                float occludedlength = UMath::Length(occludedoff);

                if (0.00001f < occludedlength) {
                    UMath::Scale(occludedoff, UMath::Distance(blendedpos, position) / occludedlength);
                }

                UMath::Add(occludedoff, position, blendedpos);
            }
        }
    }

    UMath::Sub(blendedpos, position, seek);

    if (resetdrivenav) {
        drivenav->SetNavType(WRoadNav::kTypeDirection);
        drivenav->SetLaneType(WRoadNav::kLaneCop);
        drivenav->SetCookieTrail(true);
        drivenav->ResetCookieTrail();
        this->mIVehicleAI->ResetDriveToNav(SELECT_VALID_LANE);
    }
}

void AIActionPursuitOffRoad::Update(float dT) {
    UMath::Vector3 steer = UMath::Vector3::kZero;
    UMath::Vector3 separation = UMath::Vector3::kZero;
    UMath::Vector3 affinity = UMath::Vector3::kZero;
    UMath::Vector3 seek = UMath::Vector3::kZero;

    AITarget *target = this->mIVehicleAI->GetTarget();
    this->mSpeedDelay.add_sample(target->GetSpeed(), dT);

    bool bUserNOSLastTime = this->mUserNOSLastTime;

    IInput *targetIInput;
    if (target->QueryInterface(&targetIInput)) {
        this->mUserNOSLastTime = targetIInput->GetControls().fNOS;
    }

    UMath::Vector3 avoid = UMath::Vector3::kZero;
    this->UpdateAvoidWalls(avoid);
    this->UpdateSeek(seek);

    float seek_speed = UMath::Length(seek);
    float max_accel = 0.0f;
    float max_speed = KPH2MPS(this->mIVehicleAI->GetAttributes().MAXIMUM_AI_SPEED());

    IVehicleAI *targetai;
    if (this->mIVehicleAI->GetPursuit()->GetTarget()->QueryInterface(&targetai)) {
        float speed = this->mIVehicle->GetSpeed();
        float speedmult = this->mIVehicleAI->GetAttributes().TopSpeedMultiplier();
        float accelmult = this->mIVehicleAI->GetAttributes().AccelerationMultiplier();

        if (this->mIVehicleAI->GetPursuit() != nullptr) {
            if (this->mIVehicleAI->GetPursuit()->GetIsAJerk()) {
                speedmult *= 1.2f;
                accelmult *= 1.5f;
                max_speed *= 1.1f;
            }
        }

        max_speed = bMin(max_speed, targetai->GetTopSpeed() * speedmult);
        max_accel = targetai->GetAcceleration(speed) * accelmult;

        UMath::Vector3 forward;
        this->mIRigidBody->GetForwardVector(forward);
        const float Gravity = -9.81f;
        const float grade = forward.y;
        max_accel = max_accel + grade * Gravity;

        IEngine *targetengine;
        if (targetai->QueryInterface(&targetengine) && targetengine->IsNOSEngaged()) {
            max_accel *= 0.2f;
        }

        this->mLimiter.update(speed, max_speed, max_accel, dT);
        max_speed = this->mLimiter.get_speed_limit();
    }

    if (seek_speed > max_speed) {
        UMath::Scale(seek, max_speed / seek_speed);
    }

    this->UpdateSeparation(separation);
    this->UpdateRoadAffinity(affinity);

    UMath::Add(avoid, separation, steer);
    UMath::Add(affinity, steer, steer);

    float steerdotseek = UMath::Dot(steer, seek);
    float steercounterseek = -steerdotseek / UMath::Length(seek);

    if (steercounterseek > 0.0001f) {
        float longweight = bClamp((KPH2MPS(35.0f) - steercounterseek) / KPH2MPS(25.0f), 0.0f, 1.0f);

        UMath::Vector3 seeklong;
        UMath::Vector3 seeklat;
        float steerlength2 = UMath::Dot(steer, steer);

        UMath::Scale(steer, steerdotseek / steerlength2, seeklong);
        UMath::Sub(seek, seeklong, seeklat);
        UMath::Scale(seeklong, longweight, seek);
        UMath::Add(seek, seeklat);
    }

    UMath::Add(steer, seek);

    {
        const UMath::Vector3 &v = this->mIRigidBody->GetLinearVelocity();
        float lv = UMath::Length(v);
        float ls = UMath::Length(steer);

        if (lv > 0.1f && ls > 0.1f) {
            float dot = UMath::Dot(v, steer) / (lv * ls);
            float dist = UMath::Length(steer);
            float curvature = (1.0f - dot) * lv / dist;
            float limit = bMax(KPH2MPS(40.0f), KPH2MPS(450.0f) - curvature * KPH2MPS(950.0f));

            if (limit < ls) {
                UMath::Scale(steer, limit / ls);
            }
        }
    }

    float desired_speed = UMath::Length(steer);
    this->mIVehicleAI->SetDriveSpeed(desired_speed);

    UMath::Add(this->mIRigidBody->GetPosition(), steer, steer);
    this->mIVehicleAI->SetDriveTarget(steer);
    this->mIVehicleAI->DoDriving(7);

    if (this->mNOSCountDown > 0.0f) {
        this->mNOSCountDown -= dT;
        this->mIInput->SetControlNOS(false);
    } else {
        bool isnos = this->mIInput->GetControls().fNOS;
        if (!isnos && this->mUserNOSLastTime == true && bUserNOSLastTime == false) {
            this->mNOSCountDown = kNOSDelay;
        } else {
            this->mIVehicleAI->DoNOS();
        }
    }
}

void AIActionPursuitOffRoad::OnDebugDraw() {}
