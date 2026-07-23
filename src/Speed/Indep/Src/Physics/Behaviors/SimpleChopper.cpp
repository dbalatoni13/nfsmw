#include "Speed/Indep/Src/Generated/AttribSys/Classes/chopperspecs.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/rigidbodyspecs.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IHelicopter.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"

// total size: 0xD8
class SimpleChopper : protected VehicleBehavior, protected ISimpleChopper {
  public:
    static Behavior *Construct(const BehaviorParams &params);

  protected:
    virtual float GetPower() const {
        return 1.0f;
    }
    virtual float GetRPM() const {
        return 1000.0f;
    }
    virtual float GetMaxRPM() const {
        return 4500.0f;
    }
    virtual float GetRedline() const {
        return 3500.0f;
    }
    virtual float GetMinRPM() const {
        return 0.0f;
    }
    virtual float GetMinGearRPM(int gear) const {
        return 0.0f;
    }
    virtual void MatchSpeed(float speed) {}
    virtual float GetNOSCapacity() const {
        return 0.0f;
    }
    virtual bool IsNOSEngaged() const {
        return false;
    }
    virtual bool HasNOS() const {
        return false;
    }

    // Overrides: Behavior
    void OnTaskSimulate(float dT) override;
    void Reset() override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // Overrides: ISimpleChopper
    void SetDesiredVelocity(const UMath::Vector3 &vel) override {
        this->mDesiredVelocity = vel;
    }
    void GetDesiredVelocity(UMath::Vector3 &vel) override {
        vel = this->mDesiredVelocity;
    }
    void MaxDeceleration(bool t) override {
        this->mMaxDecelFlag = t;
    }
    void SetDesiredFacingVector(const UMath::Vector3 &facingDir) override {
        this->mDesiredFacingVector = facingDir;
    }
    void GetDesiredFacingVector(UMath::Vector3 &facingDir) override {
        facingDir = this->mDesiredFacingVector;
    }

    SimpleChopper(const BehaviorParams &bp, const EngineParams &ep);
    ~SimpleChopper() override;

  private:
    void SetTorqueToMatchPitchAndRoll(UMath::Vector3 &localXZAccel, UMath::Vector3 &angVelOut);

    UMath::Vector3 mLastBodyOffset;                            // offset 0x58, size 0xC
    UMath::Vector3 mLastAngVelocity;                           // offset 0x64, size 0xC
    UMath::Vector3 mLastAccelVector;                           // offset 0x70, size 0xC
    UMath::Vector3 mDesiredVelocity;                           // offset 0x7C, size 0xC
    UMath::Vector3 mPreviousVelocity;                          // offset 0x88, size 0xC
    UMath::Vector3 mDesiredFacingVector;                       // offset 0x94, size 0xC
    BehaviorSpecsPtr<Attrib::Gen::chopperspecs> mChopperSpecs; // offset 0xA0, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::pvehicle> mVehicleSpecs;     // offset 0xB4, size 0x14
    bool mMaxDecelFlag;                                        // offset 0xC8, size 0x1
    IRigidBody *mIrigidBody;                                   // offset 0xCC, size 0x4
    ICollisionBody *mIrbComplex;                               // offset 0xD0, size 0x4
    IDamageable *mIdamage;                                     // offset 0xD4, size 0x4
};

BIND_BEHAVIOR_FACTORY(SimpleChopper);

Behavior *SimpleChopper::Construct(const BehaviorParams &params) {
    const EngineParams ep(params.fparams.Fetch<EngineParams>(UCrc32(0xa6b47fac)));
    return new SimpleChopper(params, ep);
}

SimpleChopper::SimpleChopper(const BehaviorParams &bp, const EngineParams &ep)
    : VehicleBehavior(bp, 0),    //
      ISimpleChopper(bp.fowner), //
      mChopperSpecs(this, 0),    //
      mVehicleSpecs(this, 0) {
    this->mIrigidBody = nullptr;
    this->mIrbComplex = nullptr;
    this->mIdamage = nullptr;

    this->EnableProfile("SimpleChopper");

    this->GetOwner()->QueryInterface(&this->mIrigidBody);
    this->GetOwner()->QueryInterface(&this->mIrbComplex);
    this->GetOwner()->QueryInterface(&this->mIdamage);

    UMath::Clear(this->mLastBodyOffset);
    UMath::Clear(this->mLastAngVelocity);
    UMath::Clear(this->mLastAccelVector);
    UMath::Clear(this->mDesiredVelocity);
    UMath::Clear(this->mPreviousVelocity);
    UMath::Clear(this->mDesiredFacingVector);
}

SimpleChopper::~SimpleChopper() {}

void SimpleChopper::Reset() {}

void SimpleChopper::OnBehaviorChange(const UCrc32 &mechanic) {
    Behavior::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mIrigidBody);
        this->GetOwner()->QueryInterface(&this->mIrbComplex);
    }
    if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        this->GetOwner()->QueryInterface(&this->mIdamage);
    }
}

float Max_Chopper_Accel = 80.0f;

// UNSOLVED, probably identical
void SimpleChopper::SetTorqueToMatchPitchAndRoll(UMath::Vector3 &localXZAccel, UMath::Vector3 &angVelOut) {
    this->mIrigidBody->ConvertWorldToLocal(localXZAccel, false);

    UMath::Scale(this->mLastAccelVector, 4.0f, this->mLastAccelVector);
    UMath::AddScale(localXZAccel, this->mLastAccelVector, 0.2f, localXZAccel);

    float adjustedForwardAccel = -(localXZAccel.z * (9.0f / Max_Chopper_Accel)) * this->mChopperSpecs->PITCH_ANG();

    if (0.0f < localXZAccel.y && 15.0f < this->mIrigidBody->GetSpeed() && 0.0f < this->mIrigidBody->GetLinearVelocity().y) {
        float pAdj = localXZAccel.y * 0.035f;
        adjustedForwardAccel += pAdj;
    }

    float pitchAng = UMath::Clamp(adjustedForwardAccel, -0.1f, 0.1f);

    UMath::Vector3 idealForwardV;
    this->mIrigidBody->GetForwardVector(idealForwardV);

    UMath::Vector3 actualF = idealForwardV;
    idealForwardV.y = UMath::Sina(pitchAng);
    UMath::Unit(idealForwardV, idealForwardV);

    UMath::Vector3 totalTorque;
    UMath::Cross(idealForwardV, actualF, totalTorque);

    UMath::Vector3 angVel = this->mIrigidBody->GetAngularVelocity();
    this->mIrigidBody->ConvertWorldToLocal(angVel, false);
    angVel.y = 0.0f;
    angVel.z = 0.0f;

    this->mIrigidBody->ConvertWorldToLocal(totalTorque, false);
    angVel.x = -totalTorque.x * this->mChopperSpecs->PITCH_ALIGN_SCALE();
    angVel.x = UMath::Clamp(angVel.x, -1.0f, 1.0f);

    float adjustedSideAccel = -(localXZAccel.x * (12.0f / Max_Chopper_Accel)) * this->mChopperSpecs->ROLL_ANG();
    float rollAng = UMath::Clamp(adjustedSideAccel, -0.1f, 0.1f);

    UMath::Vector3 rightVec;
    this->mIrigidBody->GetRightVector(rightVec);

    UMath::Vector3 idealRightV = rightVec;
    idealRightV.y = UMath::Sina(rollAng);
    UMath::Unit(idealRightV, idealRightV);
    UMath::Cross(idealRightV, rightVec, totalTorque);

    this->mIrigidBody->ConvertWorldToLocal(totalTorque, false);
    angVel.z = -totalTorque.z * this->mChopperSpecs->ROLL_ALIGN_SCALE();
    angVel.z = UMath::Clamp(angVel.z, -1.0f, 1.0f);

    this->mIrigidBody->ConvertLocalToWorld(angVel, false);
    angVelOut = angVel;
    this->mIrigidBody->ResolveTorque(UMath::Vector3::kZero);
}

float Min_Chopper_Accel = 30.0f;
float Chopper_Ratio = 2.0f;

void SimpleChopper::OnTaskSimulate(float dT) {
    this->mIrigidBody->ModifyXPos(-this->mLastBodyOffset.x);
    this->mIrigidBody->ModifyYPos(-this->mLastBodyOffset.y);
    this->mIrigidBody->ModifyZPos(-this->mLastBodyOffset.z);

    UMath::Vector3 v = {0.0f, -this->mIrigidBody->GetDimension().y, 0.0f};
    UMath::Rotate(v, this->mIrbComplex->GetMatrix4(), this->mLastBodyOffset);

    this->mIrigidBody->ModifyXPos(this->mLastBodyOffset.x);
    this->mIrigidBody->ModifyYPos(this->mLastBodyOffset.y);
    this->mIrigidBody->ModifyZPos(this->mLastBodyOffset.z);

    float desiredSpeed = UMath::Length(this->mDesiredVelocity);
    float maxSpeed = this->mChopperSpecs->MAX_SPEED_MPS();
    if (desiredSpeed > maxSpeed) {
        UMath::Scale(this->mDesiredVelocity, maxSpeed / desiredSpeed, this->mDesiredVelocity);
    }

    float currentLinearSpeed = this->mIrigidBody->GetSpeed();
    float maxAccel = currentLinearSpeed * 0.6f + Min_Chopper_Accel;
    if (maxAccel > Max_Chopper_Accel) {
        maxAccel = Max_Chopper_Accel;
    }
    if (this->mMaxDecelFlag) {
        maxAccel = Max_Chopper_Accel;
    }

    UMath::Vector3 deltaVel;
    UMath::Sub(this->mDesiredVelocity, this->mIrigidBody->GetLinearVelocity(), deltaVel);

    UMath::Vector3 accelVector;
    float fAccel = UMath::Length(deltaVel) * Chopper_Ratio;
    if (fAccel > 1.0f && (fAccel > maxAccel || this->mMaxDecelFlag)) {
        UMath::Scale(deltaVel, maxAccel / fAccel, accelVector);
    } else {
        UMath::Scale(deltaVel, Chopper_Ratio, accelVector);
    }

    UMath::Vector3 totalForce;
    UMath::Scale(accelVector, this->mIrigidBody->GetMass(), totalForce);

    float gravity = Attrib::Gen::rigidbodyspecs(Attrib::Gen::pvehicle(this->GetOwner()->GetAttributes()).rigidbodyspecs(), 0, nullptr).GRAVITY();

    float RotorUp = this->mIrigidBody->GetMass() * gravity;
    totalForce.y -= RotorUp;
    this->mIrigidBody->ResolveForce(totalForce);

    UMath::Vector3 workingAngVel = UMath::Vector3::kZero;
    if (dT > 0.005f) {
        UMath::Vector3 calculatedAccel;
        UMath::Sub(this->mIrigidBody->GetLinearVelocity(), this->mPreviousVelocity, calculatedAccel);
        UMath::Scale(calculatedAccel, 1.0f / dT);
        this->SetTorqueToMatchPitchAndRoll(calculatedAccel, workingAngVel);
    }

    UMath::Vector3 idealForwardV = this->mDesiredFacingVector;

    UMath::Vector3 actualF;
    this->mIrigidBody->GetForwardVector(actualF);

    actualF.y = 0.0f;
    idealForwardV.y = 0.0f;
    UMath::Unit(idealForwardV);

    UMath::Vector3 totalTorque;
    UMath::Cross(idealForwardV, actualF, totalTorque);

    UMath::Vector3 angVel = workingAngVel;
    angVel.y = -totalTorque.y * 8.0f;
    angVel.y = UMath::Clamp(angVel.y, -1.3f, 1.3f);

    UMath::Scale(this->mLastAngVelocity, 7.0f, this->mLastAngVelocity);
    UMath::AddScale(angVel, this->mLastAngVelocity, 0.125f, angVel);

    this->mIrigidBody->SetAngularVelocity(angVel);
    this->mLastAngVelocity = angVel;
    this->mPreviousVelocity = this->mIrigidBody->GetLinearVelocity();
}
