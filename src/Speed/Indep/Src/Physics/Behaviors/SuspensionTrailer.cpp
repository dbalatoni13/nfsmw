
#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/Src/Physics/Wheel.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// total size: 0x108
class SuspensionTrailer : public Chassis {
  public:
    // total size: 0x10C
    class Tire : public Wheel {
      public:
        enum LastRotationSign {
            WAS_POSITIVE = 0,
            WAS_ZERO = 1,
            WAS_NEGATIVE = 2,
        };

        Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes);
        void BeginFrame();
        void EndFrame(float dT);
        void UpdateFree(float dT);
        void UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT);

        float GetRadius() const {
            return mRadius;
        }

        void SetAngularVelocity(float av) {
            mAV = av;
        }

      private:
        const float mRadius;          // offset 0xC4, size 0x4
        float mBrake;                 // offset 0xC8, size 0x4
        float mEBrake;                // offset 0xCC, size 0x4
        float mAV;                    // offset 0xD0, size 0x4
        float mLoad;                  // offset 0xD4, size 0x4
        float mLateralForce;          // offset 0xD8, size 0x4
        float mLongitudeForce;        // offset 0xDC, size 0x4
        float mAppliedTorque;         // offset 0xE0, size 0x4
        float mSlip;                  // offset 0xE4, size 0x4
        float mLastTorque;            // offset 0xE8, size 0x4
        const int mWheelIndex;        // offset 0xEC, size 0x4
        float mRoadSpeed;             // offset 0xF0, size 0x4
        const struct tires *mSpecs;   // offset 0xF4, size 0x4
        const struct brakes *mBrakes; // offset 0xF8, size 0x4
        const int mAxleIndex;         // offset 0xFC, size 0x4
        bool mSlipping;               // offset 0x100, size 0x1
        float mLateralSpeed;          // offset 0x104, size 0x4
        LastRotationSign mLastSign;   // offset 0x108, size 0x4
    };

    static Behavior *Construct(const BehaviorParams &params);

    SuspensionTrailer(const struct BehaviorParams &bp, const SuspensionParams &sp);
    void ComputeState(float dT, State &state) const;
    void DoSimpleAero(State &state);
    void DoWheelForces(State &state);

    // Overrides
    // IUnknown
    override virtual ~SuspensionTrailer();

    // Behavior
    override virtual void OnBehaviorChange(const UCrc32 &mechanic);
    override virtual void OnTaskSimulate(float dT);
    override virtual void Reset();

    // ISuspension
    override virtual void MatchSpeed(float speed);
    override virtual UMath::Vector3 GetWheelCenterPos(unsigned int i) const;

    Tire &GetWheel(unsigned int i) {
        return *mTires[i];
    }

  private:
    BehaviorSpecsPtr<Attrib::Gen::tires> mTireInfo;         // offset 0x94, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::brakes> mBrakeInfo;       // offset 0xA8, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::chassis> mSuspensionInfo; // offset 0xBC, size 0x14
    IRigidBody *mRB;                                        // offset 0xD0, size 0x4
    IDynamicsEntity *mIDynamicsEntity;                      // offset 0xD4, size 0x4
    ICollisionBody *mRBComplex;                             // offset 0xD8, size 0x4
    IDamageable *mIDamage;                                  // offset 0xDC, size 0x4
    bool mPowerSliding;                                     // offset 0xE0, size 0x1
    unsigned int mNumWheelsOnGround;                        // offset 0xE4, size 0x4
    float mMotionDampingFactor;                             // offset 0xE8, size 0x4
    const float mSteeringControl;                           // offset 0xEC, size 0x4
    float mTimeInAir;                                       // offset 0xF0, size 0x4
    bool mDriftPhysics;                                     // offset 0xF4, size 0x1
    Tire *mTires[4];                                        // offset 0xF8, size 0x10
};

void SuspensionTrailer::Tire::BeginFrame() {
    mAppliedTorque = 0.0f;
    SetForce(UMath::Vector3::kZero);
    mLateralForce = 0.0f;
    mLongitudeForce = 0.0f;
}

void SuspensionTrailer::Tire::EndFrame(float dT) {}

void SuspensionTrailer::Tire::UpdateFree(float dT) {
    mSlipping = false;
    mLoad = 0.0f;
    mSlip = 0.0f;
    if (mEBrake > 0.0f || mBrake > 0.0f) {
        mAV = 0.0f;
    }
    mLateralForce = 0.0f;
    mLongitudeForce = 0.0f;
}

Behavior *SuspensionTrailer::Construct(const BehaviorParams &params) {
    // "BASE"
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(0xa6b47fac)));
    return new SuspensionTrailer(params, sp);
}

void SuspensionTrailer::OnBehaviorChange(const UCrc32 &mechanic) {
    Chassis::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mIDynamicsEntity);
        GetOwner()->QueryInterface(&mRBComplex);
        GetOwner()->QueryInterface(&mRB);
    } else if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        GetOwner()->QueryInterface(&mIDamage);
    }
}

void SuspensionTrailer::MatchSpeed(float speed) {
    for (int i = 0; i < 4; ++i) {
        float w = mTires[i]->GetRadius();
        mTires[i]->SetAngularVelocity(speed / w);
    }
}

void SuspensionTrailer::ComputeState(float dT, State &state) const {
    Chassis::ComputeState(dT, state);
    if (mIDynamicsEntity && !Dynamics::Articulation::IsJoined(mIDynamicsEntity)) {
        state.ebrake_input = 0.0f;
        state.brake_input = 0.0f;
        state.gas_input = 0.0f;
        state.cog = UMath::Vector3::kZero;
        mRBComplex->SetCenterOfGravity(state.cog);
    } else {
        float fwbias = mSuspensionInfo->FRONT_WEIGHT_BIAS();
        float cg_z = 0.0f;
        float cg_y = INCH2METERS(mSuspensionInfo->ROLL_CENTER());
        float ride_height = UMath::Max(GetRideHeight(0), GetRideHeight(2));
        state.cog = UVector3(0.0f, cg_y - (state.dimension.y + ride_height), cg_z);
        mRBComplex->SetCenterOfGravity(state.cog);
    }
}

void SuspensionTrailer::OnTaskSimulate(float dT) {
    if (!mRB || !mRBComplex) {
        return;
    }
    ISimable *owner = GetOwner();
    State state;
    ComputeState(dT, state);
    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->BeginFrame();
    }
    DoSimpleAero(state);
    DoWheelForces(state);
    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->UpdateTime(dT);
    }
    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->EndFrame(dT);
    }
    if (GetNumWheelsOnGround() != 0) {
        mTimeInAir = 0.0f;
    } else {
        mTimeInAir += dT;
    }
    Chassis::OnTaskSimulate(dT);
}

UMath::Vector3 SuspensionTrailer::GetWheelCenterPos(unsigned int i) const {
    UMath::Vector3 pos = mTires[i]->GetPosition();
    if (!mRBComplex) {
        return pos;
    } else {
        UMath::ScaleAdd(mRBComplex->GetUpVector(), GetWheelRadius(i), pos, pos);
        return pos;
    }
}

void SuspensionTrailer::Reset() {
    ISimable *owner = GetOwner();
    IRigidBody &rigidBody = *owner->GetRigidBody();
    unsigned int numonground = 0;
    for (unsigned int i = 0; i < GetNumWheels(); ++i) {
        Tire &wheel = GetWheel(i);
        if (wheel.InitPosition(rigidBody, wheel.GetRadius())) {
            float newCompression = wheel.GetNormal().w + GetRideHeight(i);
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
    mMotionDampingFactor = 0.0f;
}

void SuspensionTrailer::DoSimpleAero(State &state) {
    const float dragcoef_spec = mSuspensionInfo->DRAG_COEFFICIENT();
    float speed = state.speed;
    float drag = -(speed * dragcoef_spec);
    UVector3 drag_vector = state.linear_vel;
    drag_vector *= drag;
    mRB->ResolveForce(drag_vector);
}
