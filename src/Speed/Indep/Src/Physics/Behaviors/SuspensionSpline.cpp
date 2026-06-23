
#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Wheel.h"

#define EPSILON 0.000001f

// static const float Tweak_DefaultNISCarConstraint; // TODO value and use
static const bool Tweak_UseDefaultConstraint = false; // TODO use
// static const float MaxNISConstraint; // TODO value and use

#define MAKEATTRIB const

// total size: 0x198
class SuspensionSpline : public Chassis, public INISCarControl {
  public:
    // total size: 0x124
    class Tire : public Wheel {
      public:
        Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes);

        void SetLocked(bool locked) {
            this->mLocked = locked;
        }
        void SetBurnout(float speed) {
            this->mBurnout = speed;
        }

        float GetLoad() const {
            return this->mLoad;
        }
        float GetRadius() const {
            return this->mRadius;
        }

        float GetAngularVelocity() const {
            return this->mAV;
        }
        void SetAngularVelocity(float av) {
            this->mAV = av;
        }

        float GetCurrentSlip() const {
            return this->mSlip;
        }
        float GetTraction() const {}

        void BeginFrame();
        void EndFrame(float dT);

        float GetLateralSpeed() const {
            return this->mLateralSpeed;
        }
        float GetRoadSpeed() const {
            return this->mRoadSpeed;
        }

        void MatchSpeed(Mps speed) {
            this->mLocked = false;
            this->mSlip = 0.0f;
            this->mRoadSpeed = speed;
            this->mSlipAngle = 0.0f;
            this->mLateralSpeed = 0.0f;
            this->mAV = speed / mRadius;
        }

        void UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT);

        void UpdateFree(float dT);

        Angle GetSlipAngle() const {
            return this->mSlipAngle;
        }

      private:
        bool mLocked;        // offset 0xC4, size 0x1
        float mBurnout;      // offset 0xC8, size 0x4
        const float mRadius; // offset 0xCC, size 0x4
        float mAV;           // offset 0xD0, size 0x4
        float mSlip;         // offset 0xD4, size 0x4
        float mRoadSpeed;    // offset 0xD8, size 0x4
        float mSlipAngle;    // offset 0xDC, size 0x4
        float mLateralSpeed; // offset 0xE0, size 0x4
        float mLoad;         // offset 0xE4, size 0x4
    };

    SuspensionSpline(const BehaviorParams &bp, const SuspensionParams &sp);
    ~SuspensionSpline() override;
    static Behavior *Construct(const BehaviorParams &params);

    // Overrides: INISCarControl
    void SetBurnout(float speed) override {}
    float GetBurnout() const override {}
    void SetBrakeLock(bool front, bool rear) override {}
    void RestoreState() override;
    void SetConstraintAngle(Degrees angle) override {}
    void SetSteering(Angle angle, float weight) override {}

    virtual void NISCarTweaks(float dT);

    float AngleToAIAngle(float x) {}

    // Overrides: INISCarControl
    void SetAnimPitch(float dip, float time) override {}

    float GetAnimPitch() const override {}
    void SetAnimRoll(float dip, float time) override {}

    float GetAnimRoll() const override {}
    void SetAnimShake(float dip, float cycleRate, float cycleRamp, float time) override {}

    float GetAnimShake() const override {}

    // Overrides: ISuspension
    void MatchSpeed(float speed) override;
    float GetWheelAngularVelocity(int index) const override {}
    float GetWheelRadius(unsigned int index) const override {}
    float GetWheelSlip(unsigned int idx) const override {}
    Mps GetToleratedSlip(unsigned int idx) const override {}
    float GetWheelSkid(unsigned int idx) const override {}
    Newtons GetWheelLoad(unsigned int i) const override {}
    float GetWheelTraction(unsigned int index) const override {}
    void SetWheelAngularVelocity(int wheel, float w) override {}
    unsigned int GetNumWheels() const override {}
    const UMath::Vector3 &GetWheelPos(unsigned int i) const override {}
    const UMath::Vector3 &GetWheelLocalPos(unsigned int i) const override {}
    UMath::Vector3 GetWheelCenterPos(unsigned int i) const override;
    void ApplyVehicleEntryForces(bool enteringVehicle, const UMath::Vector3 &pos, bool calledfromEvent) override {}

    // Overrides: ISuspension
    const float GetWheelRoadHeight(unsigned int i) const override {}
    float GetCompression(unsigned int i) const override {}
    const UMath::Vector4 &GetWheelRoadNormal(unsigned int i) const override {}
    bool IsWheelOnGround(unsigned int i) const override {}
    const SimSurface &GetWheelRoadSurface(unsigned int i) const override {}
    const UMath::Vector3 &GetWheelVelocity(unsigned int i) const override {}
    int GetNumWheelsOnGround() const override {}
    Angle GetWheelSteer(unsigned int wheel) const override {}
    Angle GetMaxSteering() const override {}
    Angle GetWheelSlipAngle(unsigned int idx) const override {}

    // Behavior
    void OnTaskSimulate(float dT) override;
    void Reset() override;

  protected:
    void DoWheelForces(State &state);
    void DoSteering(State &state, UMath::Vector3 &right, UMath::Vector3 &left);

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    Tire &GetWheel(unsigned int i) {
        return *this->mTires[i];
    }

    const Tire &GetWheel(unsigned int i) const {
        return *this->mTires[i];
    }

  private:
    void GetWheelBase(float *width, float *length);

    BehaviorSpecsPtr<Attrib::Gen::tires> mTireInfo;              // offset 0x9C, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::chassis> mSuspensionInfo;      // offset 0xB0, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::transmission> mDrivetrainInfo; // offset 0xC4, size 0x14
    IRigidBody *mRB;                                             // offset 0xD8, size 0x4
    ICollisionBody *mCollisionBody;                              // offset 0xDC, size 0x4
    IInput *mInput;                                              // offset 0xE0, size 0x4
    IEngine *mIEngine;                                           // offset 0xE4, size 0x4
    unsigned int mNumWheelsOnGround;                             // offset 0xE8, size 0x4
    const Angle mMaxSteering;                                    // offset 0xEC, size 0x4
    Angle mSteering;                                             // offset 0xF0, size 0x4
    Angle mNISSteering;                                          // offset 0xF4, size 0x4
    float mNISSteeringWeight;                                    // offset 0xF8, size 0x4
    float mTimeInAir;                                            // offset 0xFC, size 0x4
    float mBurnout;                                              // offset 0x100, size 0x4
    bool mBrakeLockFront;                                        // offset 0x104, size 0x1
    bool mBrakeLockRear;                                         // offset 0x108, size 0x1
    Degrees mConstraint;                                         // offset 0x10C, size 0x4
    UMath::Matrix4 mNISPosition;                                 // offset 0x110, size 0x40
    float mAnimatedPitchLength;                                  // offset 0x150, size 0x4
    float mAnimatedPitch;                                        // offset 0x154, size 0x4
    float mAnimatedPitchDelta;                                   // offset 0x158, size 0x4
    float mAnimatedPitchTime;                                    // offset 0x15C, size 0x4
    float mAnimatedRollLength;                                   // offset 0x160, size 0x4
    float mAnimatedRoll;                                         // offset 0x164, size 0x4
    float mAnimatedRollDelta;                                    // offset 0x168, size 0x4
    float mAnimatedRollTime;                                     // offset 0x16C, size 0x4
    float mAnimatedShakeLength;                                  // offset 0x170, size 0x4
    float mAnimatedShakeCycleRate;                               // offset 0x174, size 0x4
    float mAnimatedShakeRamp;                                    // offset 0x178, size 0x4
    float mAnimatedShake;                                        // offset 0x17C, size 0x4
    float mAnimatedShakeDelta;                                   // offset 0x180, size 0x4
    float mAnimatedShakeTime;                                    // offset 0x184, size 0x4
    Tire *mTires[4];                                             // offset 0x188, size 0x10
};

BIND_BEHAVIOR_FACTORY(SuspensionSpline);

void SuspensionSpline::Tire::BeginFrame() {
    this->SetForce(UMath::Vector3::kZero);
    this->mLoad = 0.0f;
}

void SuspensionSpline::Tire::EndFrame(float dT) {}

void SuspensionSpline::Tire::UpdateFree(float dT) {
    this->mSlip = 0.0f;
    this->mSlipAngle = 0.0f;
    if (this->mLocked) {
        this->mAV = 0.0f;
    }
}

Behavior *SuspensionSpline::Construct(const BehaviorParams &params) {
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(UCRC32_BASE)));
    return new SuspensionSpline(params, sp);
}

void SuspensionSpline::Tire::UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT) {
    this->mLoad = load;
    this->mRoadSpeed = fwd_vel;
    this->mLateralSpeed = lat_vel;
    this->mSlipAngle = UMath::Atan2a(lat_vel, UMath::Abs(fwd_vel));
    if (this->mLocked) {
        this->mAV = 0.0f;
    } else {
        this->mAV = (fwd_vel + this->mBurnout) / this->mRadius;
    }
    this->mSlip = this->mAV * this->mRadius - fwd_vel;
}

void SuspensionSpline::OnBehaviorChange(const UCrc32 &mechanic) {
    Chassis::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mInput);
    } else if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mCollisionBody);
        this->GetOwner()->QueryInterface(&this->mRB);
    } else if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        this->GetOwner()->QueryInterface(&this->mIEngine);
    }
}

float gNIS_AeroDynamics;
static const float Tweak_AnimDampSpeed = 0.25f; // TODO use

void SuspensionSpline::RestoreState() {
    this->mInput->ClearInput();

    this->mBurnout = 0.0f;
    this->mBrakeLockFront = false;
    this->mBrakeLockRear = false;
    this->mConstraint = 60.0f;
    this->mNISSteering = 0.0f;
    this->mNISSteeringWeight = 0.0f;

    INISCarEngine *iengine;
    if (this->GetOwner()->QueryInterface(&iengine)) {
        iengine->RestoreState();
    }
}

void SuspensionSpline::GetWheelBase(float *width, float *length) {
    UMath::Vector3 dimension;
    this->GetOwner()->GetRigidBody()->GetDimension(dimension);
    if (width != nullptr) {
        *width = dimension.x;
    }
    if (length != nullptr) {
        *length = dimension.z;
    }
}

UMath::Vector3 SuspensionSpline::GetWheelCenterPos(unsigned int i) const {
    UMath::Vector3 pos = this->mTires[i]->GetPosition();
    if (this->mCollisionBody == nullptr) {
        return pos;
    } else {
        UMath::ScaleAdd(this->mCollisionBody->GetUpVector(), this->GetWheelRadius(i), pos, pos);
        return pos;
    }
}

void SuspensionSpline::MatchSpeed(float speed) {
    for (int i = 0; i < 4; ++i) {
        this->mTires[i]->MatchSpeed(speed);
    }
}

void SuspensionSpline::Reset() {
    ISimable *owner = this->GetOwner();
    IRigidBody &rigidBody = *owner->GetRigidBody();
    UMath::Vector3 vUp;
    rigidBody.GetUpVector(vUp);

    unsigned int numonground = 0;
    for (unsigned int i = 0; i < this->GetNumWheels(); ++i) {
        Tire &wheel = this->GetWheel(i);
        wheel.Reset();
        if (wheel.InitPosition(rigidBody, wheel.GetRadius())) {
            const UMath::Vector3 &groundNormal = UMath::Vector4To3(wheel.GetNormal());
            float upness = UMath::Clamp(UMath::Dot(groundNormal, vUp), 0.0f, 1.0f);
            float newCompression = wheel.GetNormal().w + this->GetRideHeight(i) * upness;
            if (newCompression < 0.0f) {
                newCompression = 0.0f;
            }
            wheel.SetCompression(newCompression);
            if (newCompression > 0.0f) {
                numonground++;
            }
        }
    }
    this->mNumWheelsOnGround = numonground;
    this->mTimeInAir = 0.0f;
}

void SuspensionSpline::DoSteering(State &state, UMath::Vector3 &right, UMath::Vector3 &left) {
    float steering = 0.0f;
    if (state.speed > 1.0f) {
        steering = UMath::Atan2a(state.local_vel.x, UMath::Abs(state.local_vel.z));

        if (state.local_vel.z < 0.0f) {
            steering = -steering;
        }
    }

    steering *= 1.0f - UMath::Abs(state.local_vel.z * 0.02f) * 0.8f;
    this->mSteering = (1.0f - this->mNISSteeringWeight) * steering + this->mNISSteeringWeight * this->mNISSteering;
    this->mSteering = UMath::Clamp(this->mSteering, -this->mMaxSteering, this->mMaxSteering);

    float ca = UMath::Cosa(this->mSteering);
    float sa = UMath::Sina(this->mSteering);
    right.z = ca;
    right.y = 0.0f;
    right.x = sa;
    UMath::Rotate(right, state.matrix, right);
    left = right;
}
