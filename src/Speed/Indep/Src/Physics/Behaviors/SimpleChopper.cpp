#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chopperspecs.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/rigidbodyspecs.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IHelicopter.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"

// total size: 0xD8
class SimpleChopper : protected VehicleBehavior, protected ISimpleChopper {
  public:
    virtual float GetPower() const;
    virtual float GetRPM() const;
    virtual float GetMaxRPM() const;
    virtual float GetRedline() const;
    virtual float GetMinRPM() const;
    virtual float GetMinGearRPM(int gear) const;
    virtual void MatchSpeed(float speed);
    virtual float GetNOSCapacity() const;
    virtual bool IsNOSEngaged() const;
    virtual bool HasNOS() const;

    void SetDesiredVelocity(const UMath::Vector3 &vel) override;
    void GetDesiredVelocity(UMath::Vector3 &vel) override;
    void MaxDeceleration(bool t) override;
    void SetDesiredFacingVector(const UMath::Vector3 &facingDir) override;
    void GetDesiredFacingVector(UMath::Vector3 &facingDir) override;

    static Behavior *Construct(const BehaviorParams &params);

    SimpleChopper(const BehaviorParams &bp, const EngineParams &ep);
    ~SimpleChopper() override;
    void Reset() override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void SetTorqueToMatchPitchAndRoll(UMath::Vector3 &localXZAccel, UMath::Vector3 &angVelOut);
    void OnTaskSimulate(float dT) override;

  private:
    UMath::Vector3 mLastBodyOffset;                       // offset 0x58, size 0xC
    UMath::Vector3 mLastAngVelocity;                     // offset 0x64, size 0xC
    UMath::Vector3 mLastAccelVector;                     // offset 0x70, size 0xC
    UMath::Vector3 mDesiredVelocity;                     // offset 0x7C, size 0xC
    UMath::Vector3 mPreviousVelocity;                    // offset 0x88, size 0xC
    UMath::Vector3 mDesiredFacingVector;                 // offset 0x94, size 0xC
    BehaviorSpecsPtr<Attrib::Gen::chopperspecs> mChopperSpecs; // offset 0xA0, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::pvehicle> mVehicleSpecs;      // offset 0xB4, size 0x14
    bool mMaxDecelFlag;                                  // offset 0xC8, size 0x1
    IRigidBody *mIrigidBody;                             // offset 0xCC, size 0x4
    ICollisionBody *mIrbComplex;                         // offset 0xD0, size 0x4
    IDamageable *mIdamage;                               // offset 0xD4, size 0x4
};

float SimpleChopper::GetPower() const {
    return 1.0f;
}

float SimpleChopper::GetRPM() const {
    return 1000.0f;
}

float SimpleChopper::GetMaxRPM() const {
    return 4500.0f;
}

float SimpleChopper::GetRedline() const {
    return 3500.0f;
}

float SimpleChopper::GetMinRPM() const {
    return 0.0f;
}

float SimpleChopper::GetMinGearRPM(int gear) const {
    return 0.0f;
}

void SimpleChopper::MatchSpeed(float speed) {}

float SimpleChopper::GetNOSCapacity() const {
    return 0.0f;
}

bool SimpleChopper::IsNOSEngaged() const {
    return false;
}

bool SimpleChopper::HasNOS() const {
    return false;
}

void SimpleChopper::SetDesiredVelocity(const UMath::Vector3 &vel) {
    mDesiredVelocity.x = vel.x;
    mDesiredVelocity.z = vel.z;
    mDesiredVelocity.y = vel.y;
}

void SimpleChopper::GetDesiredVelocity(UMath::Vector3 &vel) {
    vel.x = mDesiredVelocity.x;
    vel.z = mDesiredVelocity.z;
    vel.y = mDesiredVelocity.y;
}

void SimpleChopper::MaxDeceleration(bool t) {
    mMaxDecelFlag = t;
}

void SimpleChopper::SetDesiredFacingVector(const UMath::Vector3 &facingDir) {
    mDesiredFacingVector.x = facingDir.x;
    mDesiredFacingVector.z = facingDir.z;
    mDesiredFacingVector.y = facingDir.y;
}

void SimpleChopper::GetDesiredFacingVector(UMath::Vector3 &facingDir) {
    facingDir.x = mDesiredFacingVector.x;
    facingDir.z = mDesiredFacingVector.z;
    facingDir.y = mDesiredFacingVector.y;
}

Behavior *SimpleChopper::Construct(const BehaviorParams &params) {
    const EngineParams ep(params.fparams.Fetch<EngineParams>(UCrc32(0xa6b47fac)));
    return new SimpleChopper(params, ep);
}

SimpleChopper::SimpleChopper(const BehaviorParams &bp, const EngineParams &ep)
    : VehicleBehavior(bp, 0), //
      ISimpleChopper(bp.fowner), //
      mChopperSpecs(this, 0), //
      mVehicleSpecs(this, 0) {
    (void)ep;

    mIrigidBody = nullptr;
    mIrbComplex = nullptr;
    mIdamage = nullptr;
    *reinterpret_cast<uint32 *>(reinterpret_cast<char *>(this) + 0x48) = 0;

    GetOwner()->QueryInterface(&mIrigidBody);
    GetOwner()->QueryInterface(&mIrbComplex);
    GetOwner()->QueryInterface(&mIdamage);

    *reinterpret_cast<uint32 *>(&mDesiredFacingVector.x) = 0;
    *reinterpret_cast<uint32 *>(&mDesiredFacingVector.y) = 0;
    *reinterpret_cast<uint32 *>(&mDesiredFacingVector.z) = 0;
    UMath::Clear(mLastBodyOffset);
    UMath::Clear(mLastAngVelocity);
    UMath::Clear(mLastAccelVector);
    UMath::Clear(mDesiredVelocity);
    UMath::Clear(mPreviousVelocity);
}

void SimpleChopper::Reset() {}

void SimpleChopper::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mIrigidBody);
        GetOwner()->QueryInterface(&mIrbComplex);
    }
    if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        GetOwner()->QueryInterface(&mIdamage);
    }
}

SimpleChopper::~SimpleChopper() {}

extern float Max_Chopper_Accel;
extern float Min_Chopper_Accel;
extern float Chopper_Ratio;

void SimpleChopper::SetTorqueToMatchPitchAndRoll(UMath::Vector3 &localXZAccel, UMath::Vector3 &angVelOut) {
    float adjustedForwardAccel;
    float pitchAng;
    UMath::Vector3 idealForwardV;
    UMath::Vector3 actualF;
    UMath::Vector3 totalTorque;
    UMath::Vector3 angVel;
    float adjustedSideAccel;
    float rollAng;
    UMath::Vector3 rightVec;
    UMath::Vector3 idealRightV;

    mIrigidBody->ConvertWorldToLocal(localXZAccel, false);

    UMath::Scale(mLastAccelVector, 4.0f, mLastAccelVector);
    UMath::AddScale(localXZAccel, mLastAccelVector, 0.2f, localXZAccel);

    adjustedForwardAccel = -(localXZAccel.z * (9.0f / Max_Chopper_Accel)) * mChopperSpecs->PITCH_ANG();
    if (0.0f < localXZAccel.y && 15.0f < mIrigidBody->GetSpeed() && 0.0f < mIrigidBody->GetLinearVelocity().y) {
        adjustedForwardAccel += localXZAccel.y * 0.035f;
    }

    pitchAng = UMath::Clamp(adjustedForwardAccel, -0.1f, 0.1f);
    mIrigidBody->GetForwardVector(idealForwardV);
    actualF = idealForwardV;
    idealForwardV.y = UMath::Sina(pitchAng);
    UMath::Unit(idealForwardV, idealForwardV);
    UMath::Cross(idealForwardV, actualF, totalTorque);

    angVel = mIrigidBody->GetAngularVelocity();
    mIrigidBody->ConvertWorldToLocal(angVel, false);
    angVel.y = 0.0f;
    angVel.z = 0.0f;
    mIrigidBody->ConvertWorldToLocal(totalTorque, false);
    angVel.x = UMath::Clamp(-totalTorque.x * mChopperSpecs->PITCH_ALIGN_SCALE(), -1.0f, 1.0f);

    adjustedSideAccel = -(localXZAccel.x * (12.0f / Max_Chopper_Accel)) * mChopperSpecs->ROLL_ANG();
    rollAng = UMath::Clamp(adjustedSideAccel, -0.1f, 0.1f);
    mIrigidBody->GetRightVector(rightVec);
    idealRightV = rightVec;
    idealRightV.y = UMath::Sina(rollAng);
    UMath::Unit(idealRightV, idealRightV);
    UMath::Cross(idealRightV, rightVec, totalTorque);
    mIrigidBody->ConvertWorldToLocal(totalTorque, false);
    angVel.z = UMath::Clamp(-totalTorque.z * mChopperSpecs->ROLL_ALIGN_SCALE(), -1.0f, 1.0f);

    mIrigidBody->ConvertLocalToWorld(angVel, false);
    angVelOut = angVel;
    mIrigidBody->ResolveTorque(UMath::Vector3::kZero);
}

void SimpleChopper::OnTaskSimulate(float dT) {
    UMath::Vector3 v;
    float desiredSpeed;
    float maxSpeed;
    float currentLinearSpeed;
    float maxAccel;
    UMath::Vector3 deltaVel;
    UMath::Vector3 accelVector;
    float fAccel;
    UMath::Vector3 totalForce;
    float gravity;
    float RotorUp;
    UMath::Vector3 workingAngVel;
    UMath::Vector3 idealForwardV;
    UMath::Vector3 actualF;
    UMath::Vector3 totalTorque;
    UMath::Vector3 angVel;

    mIrigidBody->ModifyXPos(-mLastBodyOffset.x);
    mIrigidBody->ModifyYPos(-mLastBodyOffset.y);
    mIrigidBody->ModifyZPos(-mLastBodyOffset.z);

    UMath::Clear(v);
    mIrigidBody->GetDimension(v);
    v.x = 0.0f;
    v.y = -v.y;
    v.z = 0.0f;
    UMath::Rotate(v, mIrbComplex->GetMatrix4(), mLastBodyOffset);

    mIrigidBody->ModifyXPos(mLastBodyOffset.x);
    mIrigidBody->ModifyYPos(mLastBodyOffset.y);
    mIrigidBody->ModifyZPos(mLastBodyOffset.z);

    desiredSpeed = UMath::Length(mDesiredVelocity);
    maxSpeed = mChopperSpecs->MAX_SPEED_MPS();
    if (desiredSpeed > maxSpeed) {
        UMath::Scale(mDesiredVelocity, maxSpeed / desiredSpeed, mDesiredVelocity);
    }

    currentLinearSpeed = mIrigidBody->GetSpeed();
    maxAccel = currentLinearSpeed * 0.6f + Min_Chopper_Accel;
    if (maxAccel > Max_Chopper_Accel) {
        maxAccel = Max_Chopper_Accel;
    }
    if (mMaxDecelFlag) {
        maxAccel = Max_Chopper_Accel;
    }

    UMath::Sub(mDesiredVelocity, mIrigidBody->GetLinearVelocity(), deltaVel);
    fAccel = UMath::Length(deltaVel) * Chopper_Ratio;
    if (fAccel > 1.0f && (fAccel > maxAccel || mMaxDecelFlag)) {
        UMath::Scale(deltaVel, maxAccel / fAccel, accelVector);
    } else {
        UMath::Scale(deltaVel, Chopper_Ratio, accelVector);
    }

    UMath::Scale(accelVector, mIrigidBody->GetMass(), totalForce);
    Attrib::Gen::pvehicle vehicleSpecs(*mVehicleSpecs);
    Attrib::Gen::rigidbodyspecs rigidBodySpecs(vehicleSpecs.rigidbodyspecs(), 0, nullptr);
    gravity = rigidBodySpecs.GRAVITY();
    totalForce.y -= mIrigidBody->GetMass() * gravity;
    mIrigidBody->ResolveForce(totalForce);

    workingAngVel = UMath::Vector3::kZero;
    if (dT > 0.005f) {
        UMath::Vector3 calculatedAccel;

        UMath::Sub(mIrigidBody->GetLinearVelocity(), mPreviousVelocity, calculatedAccel);
        UMath::Scale(calculatedAccel, 1.0f / dT, calculatedAccel);
        SetTorqueToMatchPitchAndRoll(calculatedAccel, workingAngVel);
    }

    idealForwardV = mDesiredFacingVector;
    mIrigidBody->GetForwardVector(actualF);
    idealForwardV.y = 0.0f;
    actualF.y = 0.0f;
    UMath::Unit(idealForwardV, idealForwardV);
    UMath::Cross(idealForwardV, actualF, totalTorque);

    angVel.x = workingAngVel.x;
    angVel.z = workingAngVel.z;
    angVel.y = UMath::Clamp(-totalTorque.y * 8.0f, -1.3f, 1.3f);

    UMath::Scale(mLastAngVelocity, 7.0f, mLastAngVelocity);
    UMath::AddScale(angVel, mLastAngVelocity, 1.0f, angVel);
    UMath::Scale(angVel, 0.125f, angVel);

    mIrigidBody->SetAngularVelocity(angVel);
    mLastAngVelocity = angVel;
    mPreviousVelocity = mIrigidBody->GetLinearVelocity();
}
