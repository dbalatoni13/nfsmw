
#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Wheel.h"
#include <cmath>

// total size: 0x198
class SuspensionSpline : public Chassis, public INISCarControl {
  public:
    // total size: 0x124
    class Tire : public Wheel {
      public:
        Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes);
        void BeginFrame();
        void EndFrame(float dT);
        void UpdateFree(float dT);
        void UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT);

        void MatchSpeed(float speed) {
            mLocked = false;
            mSlip = 0.0f;
            mRoadSpeed = speed;
            mSlipAngle = 0.0f;
            mLateralSpeed = 0.0f;
            mAV = speed / mRadius;
        }

        float GetRadius() const {
            return mRadius;
        }

        // TODO do these exist?
        // void ApplyTorque(float torque) {
        //     if (!mBrakeLocked) {
        //         mAppliedTorque += torque;
        //     }
        // }

        // void ScaleTractionBoost(float scale) {
        //     mTractionBoost *= scale;
        // }

        // void AllowSlip(bool b) {
        //     mAllowSlip = b;
        // }

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

    static Behavior *Construct(const BehaviorParams &params);

    SuspensionSpline(const struct BehaviorParams &bp, const SuspensionParams &sp);
    void DoSteering(State &state, UMath::Vector3 &right, UMath::Vector3 &left);
    void GetWheelBase(float *width, float *length);

    // Overrides
    // IUnknown
    ~SuspensionSpline() override;

    // ISuspension
    void MatchSpeed(float speed) override;
    UMath::Vector3 GetWheelCenterPos(unsigned int i) const override;

    // Behavior
    void Reset() override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // INISCarControl
    void RestoreState() override;

    Tire &GetWheel(unsigned int i) {
        return *mTires[i];
    }

  private:
    BehaviorSpecsPtr<Attrib::Gen::tires> mTireInfo;              // offset 0x9C, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::chassis> mSuspensionInfo;      // offset 0xB0, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::transmission> mDrivetrainInfo; // offset 0xC4, size 0x14
    IRigidBody *mRB;                                             // offset 0xD8, size 0x4
    ICollisionBody *mCollisionBody;                              // offset 0xDC, size 0x4
    IInput *mInput;                                              // offset 0xE0, size 0x4
    IEngine *mIEngine;                                           // offset 0xE4, size 0x4
    unsigned int mNumWheelsOnGround;                             // offset 0xE8, size 0x4
    const float mMaxSteering;                                    // offset 0xEC, size 0x4
    float mSteering;                                             // offset 0xF0, size 0x4
    float mNISSteering;                                          // offset 0xF4, size 0x4
    float mNISSteeringWeight;                                    // offset 0xF8, size 0x4
    float mTimeInAir;                                            // offset 0xFC, size 0x4
    float mBurnout;                                              // offset 0x100, size 0x4
    bool mBrakeLockFront;                                        // offset 0x104, size 0x1
    bool mBrakeLockRear;                                         // offset 0x108, size 0x1
    float mConstraint;                                           // offset 0x10C, size 0x4
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

void SuspensionSpline::Tire::BeginFrame() {
    SetForce(UMath::Vector3::kZero);
    mLoad = 0.0f;
}

void SuspensionSpline::Tire::EndFrame(float dT) {}

void SuspensionSpline::Tire::UpdateFree(float dT) {
    mSlip = 0.0f;
    mSlipAngle = 0.0f;
    if (mLocked) {
        mAV = 0.0f;
    }
}

Behavior *SuspensionSpline::Construct(const BehaviorParams &params) {
    // "BASE"
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(0xa6b47fac)));
    return new SuspensionSpline(params, sp);
}

void SuspensionSpline::Tire::UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT) {
    mLoad = load;
    mRoadSpeed = fwd_vel;
    mLateralSpeed = lat_vel;
    mSlipAngle = UMath::Atan2a(lat_vel, UMath::Abs(fwd_vel));
    if (mLocked) {
        mAV = 0.0f;
    } else {
        mAV = (fwd_vel + mBurnout) / mRadius;
    }
    mSlip = mAV * mRadius - fwd_vel;
}

void SuspensionSpline::OnBehaviorChange(const UCrc32 &mechanic) {
    Chassis::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mInput);
    } else if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mCollisionBody);
        GetOwner()->QueryInterface(&mRB);
    } else if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        GetOwner()->QueryInterface(&mIEngine);
    }
}

void SuspensionSpline::RestoreState() {
    mInput->ClearInput();

    mBrakeLockFront = false;
    mBrakeLockRear = false;
    mConstraint = 60.0f;
    mBurnout = 0.0f;
    mNISSteering = 0.0f;
    mNISSteeringWeight = 0.0f;

    INISCarEngine *iengine;
    if (GetOwner()->QueryInterface(&iengine)) {
        iengine->RestoreState();
    }
}

void SuspensionSpline::GetWheelBase(float *width, float *length) {
    UMath::Vector3 dimension;
    GetOwner()->GetRigidBody()->GetDimension(dimension);
    if (width) {
        *width = dimension.x;
    }
    if (length) {
        *length = dimension.z;
    }
}

UMath::Vector3 SuspensionSpline::GetWheelCenterPos(unsigned int i) const {
    UMath::Vector3 pos = mTires[i]->GetPosition();
    if (!mCollisionBody) {
        return pos;
    } else {
        UMath::ScaleAdd(mCollisionBody->GetUpVector(), GetWheelRadius(i), pos, pos);
        return pos;
    }
}

void SuspensionSpline::MatchSpeed(float speed) {
    for (int i = 0; i < 4; ++i) {
        mTires[i]->MatchSpeed(speed);
    }
}

void SuspensionSpline::Reset() {
    ISimable *owner = GetOwner();
    IRigidBody &rigidBody = *owner->GetRigidBody();
    UMath::Vector3 vUp;
    rigidBody.GetUpVector(vUp);

    unsigned int numonground = 0;
    for (unsigned int i = 0; i < GetNumWheels(); ++i) {
        Tire &wheel = GetWheel(i);
        wheel.Reset();
        if (wheel.InitPosition(rigidBody, wheel.GetRadius())) {
            const UMath::Vector3 &groundNormal = UMath::Vector4To3(wheel.GetNormal());
            float upness = UMath::Clamp(UMath::Dot(groundNormal, vUp), 0.0f, 1.0f);
            float newCompression = GetRideHeight(i) * upness + (wheel.GetNormal()).w;
            if (newCompression < 0.0f) {
                newCompression = 0.0f;
            }
            wheel.SetCompression(newCompression);
            if (newCompression > 0.0f) {
                numonground++;
            }
        }
    }
    mNumWheelsOnGround = numonground;
    mTimeInAir = 0.0f;
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
    mSteering = (1.0f - mNISSteeringWeight) * steering + mNISSteeringWeight * mNISSteering;
    mSteering = UMath::Clamp(mSteering, -mMaxSteering, mMaxSteering);

    float ca = UMath::Cosa(mSteering);
    float sa = UMath::Sina(mSteering);
    right.z = ca;
    right.y = 0.0f;
    right.x = sa;
    UMath::Rotate(right, state.matrix, right);
    left = right;
}
