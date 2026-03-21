#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chopperspecs.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
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
