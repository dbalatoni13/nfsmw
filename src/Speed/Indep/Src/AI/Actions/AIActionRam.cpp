#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/Common/AISteer.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/World/WWorldMath.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// total size: 0x48
class AIActionRam : public AIAction, public Debugable {
  public:
    static AIAction *Construct(struct AIActionParams *params);

    AIActionRam(AIActionParams *params, float score);
    bool ShouldDoIt();
    void GetSeekPosition(UMath::Vector3 &seekPosition, bool avoid);
    void UpdateSeek(UMath::Vector3 &seek, UMath::Vector3 &seekPosition, bool pull_over);

    // Virtual functions
    virtual void OnDebugDraw();

    // Virtual overrides
    // IUnknown
    ~AIActionRam() override {}

    // AIAction
    bool CanBeAttempted(float dT) override;
    void BeginAction(float dT) override;
    bool IsFinished() override;
    void FinishAction(float dT) override;
    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    IVehicleAI *mIVehicleAI;       // offset 0x4C, size 0x4
    IRigidBody *mIRigidBody;       // offset 0x50, size 0x4
    IVehicle *mIVehicle;           // offset 0x54, size 0x4
    IPursuitAI *mIPursuitAI;       // offset 0x58, size 0x4
    ITransmission *mITransmission; // offset 0x5C, size 0x4
    IInput *mIInput;               // offset 0x60, size 0x4
    bool mBrakeLeft;               // offset 0x64, size 0x1
    performance_limiter mLimiter;  // offset 0x68, size 0x4
};

AIActionRam::AIActionRam(AIActionParams *params, float score)
    : AIAction(params, score) //
{
    MakeDebugable(DBG_AI);
    params->mOwner->QueryInterface(&mIInput);
    params->mOwner->QueryInterface(&mIVehicleAI);
    params->mOwner->QueryInterface(&mIPursuitAI);
    params->mOwner->QueryInterface(&mIVehicle);
    params->mOwner->QueryInterface(&mITransmission);
    mIRigidBody = params->mOwner->GetRigidBody();

    static int brakeLeft = 0;
    brakeLeft++;
    mBrakeLeft = brakeLeft & 1;
}

void AIActionRam::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mIRigidBody);
    }
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        GetOwner()->QueryInterface(&mITransmission);
    }
}

AIAction *AIActionRam::Construct(AIActionParams *params) {
    return new AIActionRam(params, 0.1f);
}

bool AIActionRam::ShouldDoIt() {
    if (!mIVehicleAI->GetDrivableToTargetPos()) {
        return false;
    }
    if (!mIVehicleAI->GetDrivableToDriveToNav()) {
        return false;
    }
    return true;
}

bool AIActionRam::CanBeAttempted(float dT) {
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

bool AIActionRam::IsFinished() {
    if (!mIVehicleAI->GetTarget()->IsValid()) {
        return true;
    } else {
        return ShouldDoIt() == false;
    }
}

void AIActionRam::BeginAction(float dT) {
    if (mIVehicleAI->GetLastSpawnTime() <= 0.0f) {
        float maxSpeed = MPH2MPS(60.0f);
        mIVehicle->SetSpeed(maxSpeed);
    }
    mLimiter.init(mIVehicle->GetSpeed());
}

void AIActionRam::FinishAction(float dT) {}

void AIActionRam::GetSeekPosition(UMath::Vector3 &seekPosition, bool avoid) {
    AITarget *target = mIVehicleAI->GetTarget();
    UMath::Vector3 targetPosition = target->GetPosition();
    UMath::Vector3 targetVelocity = target->GetLinearVelocity();
    UMath::Vector3 myPosition = mIRigidBody->GetPosition();

    UMath::Vector3 targetForward;
    if (target->GetSpeed() < KPH2MPS(5.0f)) {
        target->GetForwardVector(targetForward);
    } else {
        targetForward = targetVelocity;
    }
    UMath::Normalize(targetForward);

    UMath::Vector3 targetSide = UMath::Vector3Make(targetForward.z, 0.0f, -targetForward.x);
    UMath::Normalize(targetSide);

    UMath::Vector3 offset = mIPursuitAI->GetInPositionOffset();
    UMath::Vector3 metotarget = UVector3(targetPosition) - myPosition;
    UMath::Vector3 targettoseek;

    UMath::Scale(targetForward, offset.z, targettoseek);
    UMath::ScaleAdd(targetSide, offset.x, targettoseek, targettoseek);

    UMath::Vector3 metoseek = UVector3(metotarget) + UVector3(targettoseek);

    float targettoseekradius = UMath::Lengthxz(targettoseek);
    float metotargetradius = UMath::Lengthxz(metotarget);
    float metoseekradius = UMath::Lengthxz(metoseek);
    float avoidradius = bMin(targettoseekradius, metotargetradius) - 0.2f;
    float u1;
    float u2;

    if (!avoid || avoidradius <= 0.1f ||
        !WWorldMath::IntersectCircle(-metotarget.x, -metotarget.z, targettoseek.x, targettoseek.z, 0.0f, 0.0f, avoidradius, u1, u2)) {
        UMath::Add(targettoseek, targetPosition, seekPosition);
        return;
    }

    UMath::Vector3 targettotangent = UMath::Vector3Make(-metotarget.z, 0.0f, metotarget.x);
    UMath::Scale(targettotangent, targettoseekradius * 2.1f / UMath::Length(targettotangent), targettotangent);

    UMath::Vector3 drive1 = UVector3(metotarget) + UVector3(targettotangent);
    UMath::Vector3 drive2 = UVector3(metotarget) - targettotangent;

    UMath::Scale(drive1, metoseekradius / UMath::Length(drive1));
    UMath::Scale(drive2, metoseekradius / UMath::Length(drive2));

    if (UMath::Distance(drive1, metoseek) > UMath::Distance(drive2, metoseek)) {
        drive1 = drive2;
    }

    UMath::Add(drive1, targetPosition, seekPosition);
}

void AIActionRam::UpdateSeek(UMath::Vector3 &seek, UMath::Vector3 &seekPosition, bool pull_over) {
    UMath::Vector3 position = mIRigidBody->GetPosition();
    UMath::Vector3 forwardVector;
    mIRigidBody->GetForwardVector(forwardVector);
    UMath::Vector3 velocity = mIRigidBody->GetLinearVelocity();

    AITarget *target = mIVehicleAI->GetTarget();
    UMath::Vector3 targetVelocity = target->GetLinearVelocity();

    if (UMath::Length(velocity) < 1.0f) {
        velocity = forwardVector;
    }
    float tvl = UMath::Length(targetVelocity);
    UMath::Vector3 newseek = seekPosition;
    if (tvl > 0.01f) {
        UMath::Vector3 seekoff;
        UMath::Sub(seekPosition, position, seekoff);
        float blah = UMath::Dot(seekoff, targetVelocity) / tvl * 0.7f;
        UMath::ScaleAdd(targetVelocity, blah / tvl, seekoff, seekoff);
        UMath::Add(position, seekoff, newseek);
    }
    AISteer::Pursuit(seek, position, velocity, newseek, targetVelocity);
}

void AIActionRam::Update(float dT) {
    bool ispullover = mIVehicleAI->GetGoalName() == "AIGoalPullOver";

    UMath::Vector3 seekPosition;
    GetSeekPosition(seekPosition, ispullover);
    UMath::Vector3 seek;
    UpdateSeek(seek, seekPosition, ispullover);

    UMath::Vector3 steer = seek;
    float aggression = 0.0f;
    UMath::Vector3 separation = UMath::Vector3Make(0.0f, 0.0f, 0.0f);

    if (ispullover) {
        IPerpetrator *iperp;
        AITarget *target = mIVehicleAI->GetTarget();
        if (target->QueryInterface(&iperp)) {
            const Attrib::Gen::pursuitlevels *pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
            aggression = pursuitLevelAttrib->CollapseAggression();
        }

        bool isajerk = false;
        if (mIVehicleAI->GetPursuit() && mIVehicleAI->GetPursuit()->GetIsAJerk()) {
            isajerk = true;
        }
        if (isajerk) {
            aggression = 1.0f;
        }

        float staticavoid = (1.0f - aggression) * 4.0f;
        float dynamicavoid = staticavoid + 1.5f;

        AISteer::VehicleSeperation(separation, mIVehicle, mIVehicleAI->GetAvoidableList(), staticavoid, dynamicavoid);

        float steerdotseparation = UMath::Dot(steer, separation);
        float steercounterseparation = (-steerdotseparation) / UMath::Length(steer);

        if (steercounterseparation > 0.0001f) {
            float longweight = (KPH2MPS(55.0f) - steercounterseparation) / KPH2MPS(55.0f);
            longweight = bClamp(longweight, 0.0f, 1.0f);
            UMath::Vector3 steerlong;
            UMath::Vector3 steerlat;

            float separationlength2 = UMath::Dot(separation, separation);
            UMath::Scale(separation, steerdotseparation / separationlength2, steerlong);
            UMath::Sub(steer, steerlong, steerlat);
            UMath::Scale(steerlong, longweight, steer);
            UMath::Add(steer, steerlat, steer);
        }

        UMath::Add(steer, separation, steer);
    }

    float desired_speed = UMath::Length(steer);
    IVehicleAI *targetai;

    if (mIVehicleAI->GetPursuit()->GetTarget()->QueryInterface(&targetai)) {
        float speed = mIVehicle->GetSpeed();
        float speedmult = mIVehicleAI->GetAttributes().TopSpeedMultiplier();
        float accelmult = mIVehicleAI->GetAttributes().AccelerationMultiplier();
        float max_speed = KPH2MPS(mIVehicleAI->GetAttributes().MAXIMUM_AI_SPEED());

        if (mIVehicleAI->GetPursuit()->GetIsAJerk()) {
            speedmult *= 1.2f;
            accelmult *= 1.5f;
            max_speed *= 1.1f;
        }

        max_speed = bMin(max_speed, targetai->GetTopSpeed() * speedmult);
        float max_accel = targetai->GetAcceleration(speed) * accelmult;

        mLimiter.update(speed, max_speed, max_accel, dT);
        desired_speed = bMin(desired_speed, mLimiter.get_speed_limit());
    }

    float closeenoughspeed = (1.0f - aggression) * KPH2MPS(0.2f) + KPH2MPS(0.5f);
    if (ispullover && UMath::Length(seek) < closeenoughspeed) {
        mIInput->SetControlGas(0.0f);
        mIInput->SetControlBrake(1.0f);
        mIInput->SetControlSteering(0.0f);
        mIInput->SetControlSteeringVertical(0.0f);
        mIInput->SetControlHandBrake(1.0f);
        mIInput->SetControlNOS(false);
        return;
    }

    mIVehicleAI->SetDriveSpeed(desired_speed);

    UMath::Add(mIRigidBody->GetPosition(), steer, steer);
    mIVehicleAI->SetDriveTarget(steer);
    mIVehicleAI->DoDriving(7);
    mIInput->SetControlNOS(false);
}

void AIActionRam::OnDebugDraw() {}
