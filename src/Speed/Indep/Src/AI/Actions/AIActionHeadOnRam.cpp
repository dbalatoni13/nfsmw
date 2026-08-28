#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/AI/AISteer.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// total size: 0x48
class AIActionHeadOnRam : public AIAction, public Debugable {
  public:
    static AIAction *Construct(struct AIActionParams *params);

    AIActionHeadOnRam(AIActionParams *params, float score);
    void GetSeekPosition(UMath::Vector3 &seekPosition);
    float GetDesiredSpeed(UMath::Vector3 &seekPosition);
    void UpdateSeek(UMath::Vector3 &seek, UMath::Vector3 &seekPosition, float &distToSeekPos);

    // Virtual functions
    virtual void OnDebugDraw();

    // Virtual overrides
    // IUnknown
    ~AIActionHeadOnRam() override {}

    // AIAction
    bool CanBeAttempted(float dT) override;
    void BeginAction(float dT) override;
    bool IsFinished() override;
    void FinishAction(float dT) override;
    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    bool ShouldRestartWhenFinished() override {
        return true;
    }

  private:
    IVehicleAI *mIVehicleAI;       // offset 0x4C, size 0x4
    IRigidBody *mIRigidBody;       // offset 0x50, size 0x4
    IVehicle *mIVehicle;           // offset 0x54, size 0x4
    IPursuitAI *mIPursuitAI;       // offset 0x58, size 0x4
    ITransmission *mITransmission; // offset 0x5C, size 0x4
    bool mBrakeLeft;               // offset 0x60, size 0x1
};

AIActionHeadOnRam::AIActionHeadOnRam(AIActionParams *params, float score)
    : AIAction(params, score) //
{
    this->MakeDebugable(DBG_AI);
    params->mOwner->QueryInterface(&this->mIVehicleAI);
    params->mOwner->QueryInterface(&this->mIPursuitAI);
    params->mOwner->QueryInterface(&this->mIVehicle);
    params->mOwner->QueryInterface(&this->mITransmission);
    this->mIRigidBody = params->mOwner->GetRigidBody();

    static int brakeLeft = 0;
    brakeLeft++;
    this->mBrakeLeft = (brakeLeft & 1) != 0;
}

void AIActionHeadOnRam::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mIRigidBody);
    }
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        this->GetOwner()->QueryInterface(&this->mITransmission);
    }
}

AIAction *AIActionHeadOnRam::Construct(AIActionParams *params) {
    return new AIActionHeadOnRam(params, 0.1f);
}

bool AIActionHeadOnRam::CanBeAttempted(float dT) {
    if (this->mIVehicleAI != nullptr && this->mIPursuitAI != nullptr && this->mITransmission != nullptr && this->mIRigidBody != nullptr) {
        if (!this->mIVehicleAI->GetDrivableToTargetPos()) {
            return false;
        }
        if (this->mIVehicleAI->GetDrivableToDriveToNav()) {
            return this->mIPursuitAI->GetChicken() == false;
        }
    }
    return false;
}

bool AIActionHeadOnRam::IsFinished() {
    return !this->mIVehicleAI->GetTarget()->IsValid();
}

void AIActionHeadOnRam::BeginAction(float dT) {
    if (this->mIVehicleAI->GetLastSpawnTime() <= 0.0f) {
        float maxSpeed = MPH2MPS(60.0f);
        UMath::Vector3 seekPosition;
        this->GetSeekPosition(seekPosition);
        this->mIVehicle->SetSpeed(UMath::Min(maxSpeed, this->GetDesiredSpeed(seekPosition)));
    }
}

void AIActionHeadOnRam::FinishAction(float dT) {}

void AIActionHeadOnRam::GetSeekPosition(UMath::Vector3 &seekPosition) {
    AITarget *target = this->mIVehicleAI->GetTarget();
    UMath::Vector3 targetPosition = target->GetPosition();
    UMath::Vector3 targetVelocity = target->GetLinearVelocity();
    UMath::Vector3 targetForward;
    if (target->GetSpeed() < KPH2MPS(5.0f)) {
        target->GetForwardVector(targetForward);
    } else {
        targetForward = targetVelocity;
    }
    UMath::Normalize(targetForward);
    UMath::Vector3 targetSide = UMath::Vector3Make(targetForward.z, 0.0f, -targetForward.x);
    UMath::Normalize(targetSide);

    UMath::Vector3 offset = this->mIPursuitAI->GetInPositionOffset();
    UMath::Scale(targetForward, offset.z, seekPosition);
    UMath::ScaleAdd(targetSide, offset.x, seekPosition, seekPosition);
    UMath::Add(seekPosition, targetPosition, seekPosition);
}

float AIActionHeadOnRam::GetDesiredSpeed(UMath::Vector3 &seekPosition) {
    float desiredSpeed = 0.0f;
    UMath::Vector3 carPosition = this->mIRigidBody->GetPosition();

    UMath::Vector3 forwardVector;
    this->mIRigidBody->GetForwardVector(forwardVector);

    AITarget *target = this->mIVehicleAI->GetTarget();
    UMath::Vector3 targetForwardVec;
    target->GetForwardVector(targetForwardVec);

    float targetSpeed = target->GetSpeed();
    UMath::Vector3 dirToTargetPos;
    UMath::Sub(seekPosition, carPosition, dirToTargetPos);

    float length = UMath::Normalize(dirToTargetPos);
    float distToTarget = length * UMath::Dot(targetForwardVec, dirToTargetPos);

    desiredSpeed = AISteer::GetDesiredSpeedToTarget(distToTarget, targetSpeed);

    return desiredSpeed;
}

void AIActionHeadOnRam::Update(float dT) {
    UMath::Vector3 steer = UMath::Vector3::kZero;
    UMath::Vector3 seek = UMath::Vector3::kZero;
    UMath::Vector3 seekPosition;
    this->GetSeekPosition(seekPosition);

    AITarget *target = this->mIVehicleAI->GetTarget();
    AISteer::Ram(seek, this->mIRigidBody->GetPosition(), this->mIRigidBody->GetSpeed(), target->GetPosition(), target->GetLinearVelocity());
    UMath::Add(seek, steer, steer);

    this->mIVehicleAI->SetDriveSpeed(KPH2MPS(100.0f));

    UMath::Add(this->mIRigidBody->GetPosition(), steer, steer);
    this->mIVehicleAI->SetDriveTarget(steer);
    this->mIVehicleAI->DoDriving(7);
}

void AIActionHeadOnRam::OnDebugDraw() {}
