#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/AI/AISteer.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// TODO are these two maybe in AIAction.h?
// total size: 0x1
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

    enum { sample_count = 16 };
};

template <typename T> class time_delay_filter {
    typedef T traits_type;
    typedef typename T::value_type value_type; // TODO use these
    typedef typename T::value_arg value_arg;
    typedef typename T::time_type time_type;

  public:
    void reset(float init);

    const float &at(unsigned int off) const {
        return samples[(cursor + 16 - off) & 15];
    }
    inline float resolution() const {
        return traits.maximum_delay() * (1.0f / 16.0f);
    }

    float get_sample(time_type delay) const;
    void add_sample(float sample, time_type dt);

    time_delay_filter() {
        offset = 0.0f;
        cursor = 0;
    }

  private:
    static const size_t sample_count;

    traits_type traits;  // offset 0x0, size 0x1
    float samples[16];   // offset 0x4, size 0x40
    float lastsample;    // offset 0x44, size 0x4
    float offset;        // offset 0x48, size 0x4
    unsigned int cursor; // offset 0x4C, size 0x4
};

template <typename T> void time_delay_filter<T>::reset(float init) {
    cursor = 0;
    lastsample = init;
    offset = 0.0f;
    for (unsigned int i = 0; i < 16; i++) {
        samples[i] = init;
    }
}

template <typename T> float time_delay_filter<T>::get_sample(time_type delay) const {
    if (delay <= offset) {

        if (offset < 0.0001f) {
            return lastsample;
        }
        return traits.lerp(at(0), lastsample, (offset - delay) / offset);
    }
    unsigned int o = static_cast<unsigned int>(bFloor((delay - offset) / resolution())) + 1;
    if (o > 15) {
        return at(15);
    }
    return traits.lerp(at(o), at(o - 1), (static_cast<float>(o) * resolution() - delay + offset) / resolution());
}

template <typename T> void time_delay_filter<T>::add_sample(float sample, time_type dt) {
    unsigned int count = static_cast<unsigned int>(bFloor((dt + offset) / resolution()));

    for (unsigned int i = 0; i < count; i++) {

        float frac = (static_cast<float>(i + 1) * resolution() - offset) / dt;
        cursor = (cursor + 1) & 15;
        samples[cursor] = traits.lerp(lastsample, sample, frac);
    }
    offset = offset + (dt - static_cast<float>(count) * resolution());
    lastsample = sample;
}

#include "Speed/Indep/Src/World/Common/WGridNode.h" // 0x8003CD70
template void time_delay_filter<speed_delay_traits>::reset(float); // 0x8003CDE8
template float time_delay_filter<speed_delay_traits>::get_sample(float) const;
template void time_delay_filter<speed_delay_traits>::add_sample(float, float);

// total size: 0x48
class AIActionPursuitOffRoad : public AIAction, public Debugable {
  public:
    typedef struct time_delay_filter<speed_delay_traits> speed_delay_filter;

    static AIAction *Construct(struct AIActionParams *params);

    AIActionPursuitOffRoad(AIActionParams *params, float score);

  private:
    bool ShouldDoIt();
    void UpdateRoadAffinity(UMath::Vector3 &affinity);
    void UpdateSeparation(UMath::Vector3 &separation);
    void UpdateAvoidWalls(UMath::Vector3 &avoid);
    void UpdateSeek(UMath::Vector3 &seek);

  public:

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
    MakeDebugable(DBG_AI);
    params->mOwner->QueryInterface(&mIVehicleAI);
    params->mOwner->QueryInterface(&mIPursuitAI);
    params->mOwner->QueryInterface(&mIVehicle);
    params->mOwner->QueryInterface(&mITransmission);
    params->mOwner->QueryInterface(&mIInput);
    mIRigidBody = params->mOwner->GetRigidBody();

    static int brakeLeft = 0;
    brakeLeft++;

    mNOSCountDown = -1.0f;
    mBrakeLeft = brakeLeft & 1;
    mUserNOSLastTime = false;
}

void AIActionPursuitOffRoad::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mIRigidBody);
    }
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        GetOwner()->QueryInterface(&mITransmission);
    }
}

UTL::COM::Factory<AIActionParams *, AIAction, UCrc32>::Prototype _AIActionPursuitOffRoad("AIActionPursuitOffRoad", AIActionPursuitOffRoad::Construct);

AIAction *AIActionPursuitOffRoad::Construct(AIActionParams *params) {
    return new AIActionPursuitOffRoad(params, 0.01f);
}

bool AIActionPursuitOffRoad::ShouldDoIt() {
    AITarget *target = mIVehicleAI->GetTarget();
    UMath::Vector3 targetPosition = target->GetPosition();
    UMath::Vector3 carPosition = mIRigidBody->GetPosition();
    float distanceToTarget = UMath::Distance(carPosition, targetPosition);
    float distancelimit = 60.0f;

    IVehicleAI *targetvehicleai;
    if (target->QueryInterface(&targetvehicleai)) {
        distancelimit += UMath::Distance(targetPosition, targetvehicleai->GetCurrentRoad()->GetPosition());
    }

    if (distanceToTarget > distancelimit) {
        return false;
    }
    if (UMath::Distance(target->GetLinearVelocity(), mIRigidBody->GetLinearVelocity()) > KPH2MPS(140.0f)) {
        return false;
    }
    if (!mIVehicleAI->GetDrivableToTargetPos()) {
        return false;
    }
    if (!mIVehicleAI->GetDrivableToDriveToNav()) {
        return false;
    }
    return true;
}

bool AIActionPursuitOffRoad::CanBeAttempted(float dT) {
    if (mIVehicleAI && mIPursuitAI && mITransmission && mIRigidBody) {
        if (mIPursuitAI->GetChicken()) {
            return false;
        }
        if (!ShouldDoIt()) {
            return false;
        }
        return true;
    }
    return false;
}

bool AIActionPursuitOffRoad::IsFinished() {
    if (mIVehicleAI->GetTarget()->IsValid()) {
        return ShouldDoIt() == false;
    }
    return true;
}

void AIActionPursuitOffRoad::BeginAction(float dT) {
    if (mIVehicleAI->GetLastSpawnTime() <= 0.0f) {
        float maxSpeed = MPH2MPS(60.0f);
        mIVehicle->SetSpeed(maxSpeed);
    }
    WRoadNav *road_nav = mIVehicleAI->GetDriveToNav();
    road_nav->SetNavType(WRoadNav::kTypeDirection);
    road_nav->SetLaneType(WRoadNav::kLaneCop);
    road_nav->SetCookieTrail(true);
    road_nav->ResetCookieTrail();

    mIVehicleAI->ResetDriveToNav(SELECT_CURRENT_LANE);
    mSpeedDelay.reset(mIVehicleAI->GetTarget()->GetSpeed());
    mLimiter.init(mIVehicle->GetSpeed());
}

void AIActionPursuitOffRoad::FinishAction(float dT) {}

void AIActionPursuitOffRoad::UpdateRoadAffinity(UMath::Vector3 &affinity) {
    WRoadNav *drivenav = mIVehicleAI->GetDriveToNav();
    const NavCookie &cookie = drivenav->GetCurrentCookie();
    const UMath::Vector3 &position = mIRigidBody->GetPosition();
    const UMath::Vector3 &velocity = mIRigidBody->GetLinearVelocity();

    UMath::Vector2 side = UMath::Vector2Make(cookie.Forward.y, -cookie.Forward.x);
    UMath::Vector2 loff = UMath::Vector2Make(position.x - cookie.Left.x, position.z - cookie.Left.y);
    UMath::Vector2 roff = UMath::Vector2Make(position.x - cookie.Right.x, position.z - cookie.Right.y);

    float llen = UMath::Dot(loff, side);
    float rlen = -UMath::Dot(roff, side);
    float sidev = UMath::Dot(UMath::Vector2Make(velocity.x, velocity.z), side);
    float lden = bMax(0.1f, llen + 1.0f);
    float rden = bMax(0.1f, rlen + 1.0f);
    float linvtime = bMax(0.0f, -sidev) / lden;
    float rinvtime = bMax(0.0f, sidev) / rden;
    float lscale = 0.5f / (lden * lden);
    float rscale = 0.5f / (rden * rden);
    lscale = 2.0f * linvtime * linvtime + lscale;
    rscale = 2.0f * rinvtime * rinvtime + rscale;
    lscale = bMin(KPH2MPS(100.0f), lscale);
    rscale = bMin(KPH2MPS(100.0f), rscale);

    UMath::Vector3 side3 = UMath::Vector3Make(side.x, 0.0f, side.y);
    UMath::Scale(side3, lscale, affinity);
    UMath::ScaleAdd(side3, -rscale, affinity, affinity);
}

void AIActionPursuitOffRoad::UpdateSeparation(UMath::Vector3 &separation) {
    const AvoidableList &avoidList = mIVehicleAI->GetAvoidableList();
    AISteer::VehicleSeperation(separation, mIVehicle, avoidList, 2.7f, 5.3f);
}

void AIActionPursuitOffRoad::UpdateAvoidWalls(UMath::Vector3 &avoid) {
    UMath::Vector3 velocity = mIRigidBody->GetLinearVelocity();
    float speed = Length(velocity);
    UMath::Vector3 position = mIRigidBody->GetPosition();

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
    if (!WCollisionMgr(0, 3).CheckHitWorld(posToDest, cinfo, 2)) {
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
    UMath::Vector3 position = mIRigidBody->GetPosition();
    UMath::Vector3 forwardVector;
    mIRigidBody->GetForwardVector(forwardVector);
    AITarget *target = mIVehicleAI->GetTarget();
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
    WRoadNav *startnav = itargetai->GetCurrentRoad();
    WRoadNav *endnav = itargetai->GetFutureRoad();

    UMath::Vector3 seekcenter;
    UMath::Vector3 seekdirection;
    UMath::Vector3 seekfuturecenter;
    if (startnav->IsValid() && endnav->IsValid()) {
        seekcenter = startnav->GetPosition();
        seekfuturecenter = endnav->GetPosition();
        seekdirection = UVector3(seekfuturecenter) - seekcenter;
    } else {
        seekcenter = targetPosition;
        seekdirection = targetDirection;
        UMath::Add(targetPosition, targetVelocity, seekfuturecenter);
    }

    UMath::Vector3 offset = mIPursuitAI->GetPursuitOffset();
    UMath::Vector3 seekside = UMath::Vector3Make(seekdirection.z, 0.0f, -seekdirection.x);
    UMath::Vector3 seekPosition;
    UMath::Vector3 seekFuturePosition;
    UMath::Normalize(seekside);
    UMath::Normalize(seekdirection);
    UMath::ScaleAdd(seekdirection, offset.z, seekcenter, seekPosition);
    UMath::ScaleAdd(seekside, offset.x, seekPosition, seekPosition);
    UMath::ScaleAdd(seekdirection, offset.z, seekfuturecenter, seekFuturePosition);
    UMath::ScaleAdd(seekside, offset.x, seekFuturePosition, seekFuturePosition);

    float delay_time = iperp->GetPursuitLevelAttrib()->SpeedReactionTime();
    float delay_speed = mSpeedDelay.get_sample(delay_time);
    UMath::Vector3 roadoff;
    UMath::Sub(seekFuturePosition, seekPosition, roadoff);
    float roadlen = UMath::Length(roadoff);
    if (roadlen > 1e-05f) {
        UMath::Scale(roadoff, delay_speed / roadlen, roadoff);
        roadlen = delay_speed;
    }

    UMath::Vector3 simpleoff = targetVelocity;
    float simplelen = UMath::Length(targetVelocity);
    if (simplelen > 1e-05f) {
        UMath::Scale(simpleoff, delay_speed / simplelen, simpleoff);
    }

    UMath::Vector3 seekoff;
    UMath::Sub(seekPosition, position, seekoff);
    float seeklen = UMath::Length(seekoff);
    if (seeklen > 1e-05f) {
        float seekdir = UMath::Dot(seekoff, seekdirection) / seeklen;
        float rscale = bMin(1.0f, bMax(KPH2MPS(20.0f), delay_speed * 0.2f) / seeklen);
        float fscale = bMin(1.0f, bMax(seekdir * 10.0f, -1.0f));
        float slide = fscale * 0.5f + 0.5f;
        float seekscale = slide * 1.8f + (1.0f - slide) * rscale;
        if (roadlen >= 1e-05f) {
            UMath::Vector3 paraseek;
            UMath::Scale(roadoff, UMath::Dot(seekoff, roadoff) / (roadlen * roadlen), paraseek);
            UMath::Vector3 perpseek = UVector3(seekoff) - paraseek;
            UMath::ScaleAdd(paraseek, seekscale, perpseek, seekoff);
        }
    }

    float tether_weight = mIVehicleAI->GetAttributes().TETHER_WEIGHT() * 0.01f;
    UMath::Vector3 blendedpos;
    UMath::Lerp(roadoff, simpleoff, tether_weight, blendedpos);
    UMath::Add(blendedpos, seekoff, blendedpos);
    UMath::Add(blendedpos, position, blendedpos);

    bool resetdrivenav = true;
    WRoadNav *drivenav = mIVehicleAI->GetDriveToNav();
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
            UMath::Vector3 driveside;
            UMath::Normalize(enddir);
            const float inc_quantum = 5.0f;
            if (incdelta > inc_quantum) {
                do {
                    drivenav->IncNavPosition(inc_quantum, enddir, 0.0f);
                    drivepos = drivenav->GetPosition();
                    drivedir = drivenav->GetForwardVector();
                    UMath::Normalize(drivedir);
                    UMath::Sub(blendedpos, drivepos, endoff);
                    incdelta = UMath::Dot(endoff, drivedir);
                } while (incdelta > inc_quantum);
            }
            float lanedelta = UMath::Max(0.1f, incdelta);
            driveside = UMath::Vector3Make(drivedir.z, 0.0f, -drivedir.x);
            UMath::Normalize(driveside);
            float laneoffset = UMath::Dot(endoff, driveside) + drivenav->GetLaneOffset();
            float snaplaneoffset = drivenav->SnapToSelectableLane(laneoffset);
            if (bAbs(snaplaneoffset - laneoffset) < 4.0f) {
                drivenav->ChangeLanes(snaplaneoffset, 0.0f);
                resetdrivenav = false;
                drivenav->IncNavPosition(lanedelta, enddir, 0.0f);
                drivenav->UpdateOccludedPosition(true);
                UMath::Vector3 occludedpos = drivenav->GetOccludedPosition();
                UMath::Vector3 occludedoff;
                UMath::Sub(occludedpos, position, occludedoff);
                float occludedlength = UMath::Length(occludedoff);
                if (occludedlength > 1e-05f) {
                    UMath::Scale(occludedoff, UMath::Distance(blendedpos, position) / occludedlength, occludedoff);
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
        mIVehicleAI->ResetDriveToNav(SELECT_VALID_LANE);
    }
}

void AIActionPursuitOffRoad::Update(float dT) {
    UMath::Vector3 steer = UMath::Vector3::kZero;
    UMath::Vector3 separation = UMath::Vector3::kZero;
    UMath::Vector3 affinity = UMath::Vector3::kZero;
    UMath::Vector3 seek = UMath::Vector3::kZero;

    AITarget *target = mIVehicleAI->GetTarget();
    mSpeedDelay.add_sample(target->GetSpeed(), dT);

    bool bUserNOSLastTime = mUserNOSLastTime;

    IInput *targetIInput;
    if (target->QueryInterface(&targetIInput)) {
        mUserNOSLastTime = targetIInput->GetControls().fNOS;
    }

    UMath::Vector3 avoid = UMath::Vector3::kZero;
    UpdateAvoidWalls(avoid);

    UpdateSeek(seek);

    float seek_speed = UMath::Length(seek);
    float max_accel = 0.0f;
    float max_speed = KPH2MPS(mIVehicleAI->GetAttributes().MAXIMUM_AI_SPEED());

    IVehicleAI *targetai;
    if (mIVehicleAI->GetPursuit()->GetTarget()->QueryInterface(&targetai)) {
        float speed = mIVehicle->GetSpeed();
        float speedmult = mIVehicleAI->GetAttributes().TopSpeedMultiplier();
        float accelmult = mIVehicleAI->GetAttributes().AccelerationMultiplier();
        if (mIVehicleAI->GetPursuit() && mIVehicleAI->GetPursuit()->GetIsAJerk()) {
            speedmult *= 1.2f;
            accelmult *= 1.5f;
            max_speed *= 1.1f;
        }
        max_speed = bMin(max_speed, targetai->GetTopSpeed() * speedmult);
        max_accel = targetai->GetAcceleration(speed) * accelmult;

        UMath::Vector3 forward;
        mIRigidBody->GetForwardVector(forward);
        const float Gravity = -9.81f;
        const float grade = forward.y;
        max_accel += grade * Gravity;

        IEngine *targetengine;
        if (targetai->QueryInterface(&targetengine) && targetengine->IsNOSEngaged()) {
            max_accel *= 0.2f;
        }

        mLimiter.update(speed, max_speed, max_accel, dT);
        max_speed = mLimiter.get_speed_limit();
    }

    if (seek_speed > max_speed) {
        UMath::Scale(seek, max_speed / seek_speed);
    }

    UpdateSeparation(separation);
    UpdateRoadAffinity(affinity);

    UMath::Add(avoid, separation, steer);
    UMath::Add(affinity, steer, steer);

    float steerdotseek = UMath::Dot(steer, seek);
    float steercounterseek = (-steerdotseek) / UMath::Length(seek);

    if (steercounterseek > 0.0001f) {
        float longweight = bClamp((KPH2MPS(35.0f) - steercounterseek) / KPH2MPS(25.0f), 0.0f, 1.0f);
        UMath::Vector3 seeklong;
        UMath::Vector3 seeklat;

        float steerlength2 = UMath::Dot(steer, steer);
        UMath::Scale(steer, steerdotseek / steerlength2, seeklong);
        UMath::Sub(seek, seeklong, seeklat);
        UMath::Scale(seeklong, longweight, seek);
        UMath::Add(seek, seeklat, seek);
    }

    UMath::Add(steer, seek, steer);

    {
        const UMath::Vector3 &v = mIRigidBody->GetLinearVelocity();
        float lv = UMath::Length(v);
        float ls = UMath::Length(steer);
        if (lv > 0.1f && ls > 0.1f) {
            float dot = UMath::Dot(v, steer) / (lv * ls);
            float dist = UMath::Length(steer);
            float curvature = (1.0f - dot) * lv / dist;
            float curvmax = KPH2MPS(40.0f);
            float limit = bMax(curvmax, KPH2MPS(450.0f) - curvature * KPH2MPS(950.0f));
            if (limit < ls) {
                UMath::Scale(steer, limit / ls);
            }
        }
    }

    float desired_speed = UMath::Length(steer);
    mIVehicleAI->SetDriveSpeed(desired_speed);
    UMath::Add(mIRigidBody->GetPosition(), steer, steer);
    mIVehicleAI->SetDriveTarget(steer);

    mIVehicleAI->DoDriving(7);

    if (mNOSCountDown > 0.0f) {
        mNOSCountDown -= dT;
        mIInput->SetControlNOS(false);
    } else {
        bool isnos = mIInput->GetControls().fNOS;
        if (!isnos && mUserNOSLastTime == true && !bUserNOSLastTime) {
            mNOSCountDown = 1.5f;
        } else {
            mIVehicleAI->DoNOS();
        }
    }
}

void AIActionPursuitOffRoad::OnDebugDraw() {}
