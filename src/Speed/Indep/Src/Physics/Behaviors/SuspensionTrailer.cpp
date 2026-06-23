
#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/Src/Physics/Wheel.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

#define EPSILON 0.000001f

#define MAKEATTRIB const

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

        void SetBrake(float brake) {
            this->mBrake = brake;
        }
        void SetEBrake(float ebrake) {
            this->mEBrake = ebrake;
        }
        float GetEBrake() const {
            return this->mEBrake;
        }

        float GetRadius() const {
            return mRadius;
        }

        float GetAngularVelocity() const {
            return this->mAV;
        }
        void SetAngularVelocity(float av) {
            this->mAV = av;
        }

        float GetLoad() const {
            return this->mLoad;
        }

        float GetCurrentSlip() const {
            return this->mSlip;
        }

        float GetTraction() const {}

        void BeginFrame();
        void EndFrame(float dT);

        void ApplyTorque(float torque) {}

        float GetLateralForce() const {
            return this->mLateralForce;
        }
        float GetLongitudeForce() const {
            return this->mLongitudeForce;
        }

        float GetLateralSpeed() const {
            return this->mLateralSpeed;
        }

        void UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT);
        void UpdateFree(float dT);

      private:
        const float mRadius;                // offset 0xC4, size 0x4
        float mBrake;                       // offset 0xC8, size 0x4
        float mEBrake;                      // offset 0xCC, size 0x4
        float mAV;                          // offset 0xD0, size 0x4
        float mLoad;                        // offset 0xD4, size 0x4
        float mLateralForce;                // offset 0xD8, size 0x4
        float mLongitudeForce;              // offset 0xDC, size 0x4
        float mAppliedTorque;               // offset 0xE0, size 0x4
        float mSlip;                        // offset 0xE4, size 0x4
        float mLastTorque;                  // offset 0xE8, size 0x4
        const int mWheelIndex;              // offset 0xEC, size 0x4
        float mRoadSpeed;                   // offset 0xF0, size 0x4
        const Attrib::Gen::tires *mSpecs;   // offset 0xF4, size 0x4
        const Attrib::Gen::brakes *mBrakes; // offset 0xF8, size 0x4
        const int mAxleIndex;               // offset 0xFC, size 0x4
        bool mSlipping;                     // offset 0x100, size 0x1
        float mLateralSpeed;                // offset 0x104, size 0x4
        LastRotationSign mLastSign;         // offset 0x108, size 0x4
    };

    SuspensionTrailer(const BehaviorParams &bp, const SuspensionParams &sp);
    ~SuspensionTrailer() override;
    static Behavior *Construct(const BehaviorParams &params);

    // ISuspension
    void MatchSpeed(float speed) override;
    float GetWheelAngularVelocity(int index) const override {}
    float GetWheelRadius(unsigned int index) const override {}
    float GetWheelSlip(unsigned int idx) const override {}
    Mps GetToleratedSlip(unsigned int idx) const override {}
    Newtons GetWheelLoad(unsigned int i) const override {}
    float GetWheelSkid(unsigned int idx) const override {}
    float GetWheelTraction(unsigned int index) const override {}
    void SetWheelAngularVelocity(int wheel, float w) override {}
    unsigned int GetNumWheels() const override {}
    const UMath::Vector3 &GetWheelPos(unsigned int i) const override {}
    const UMath::Vector3 &GetWheelLocalPos(unsigned int i) const override {}
    UMath::Vector3 GetWheelCenterPos(unsigned int i) const override;
    void ApplyVehicleEntryForces(bool enteringVehicle, const UMath::Vector3 &pos, bool calledfromEvent) override {}
    const float GetWheelRoadHeight(unsigned int i) const override {}
    float GetCompression(unsigned int i) const override {}
    const UMath::Vector4 &GetWheelRoadNormal(unsigned int i) const override {}
    bool IsWheelOnGround(unsigned int i) const override {}
    const SimSurface &GetWheelRoadSurface(unsigned int i) const override {}
    const UMath::Vector3 &GetWheelVelocity(unsigned int i) const override {}
    int GetNumWheelsOnGround() const override {}
    float GetWheelSteer(unsigned int wheel) const override {}
    float GetMaxSteering() const override {}
    Angle GetWheelSlipAngle(unsigned int idx) const override {}

    // Behavior
    void OnTaskSimulate(float dT) override;
    void Reset() override;

  protected:
    void DoSimpleAero(State &state);
    void DoWheelForces(State &state);
#ifdef EA_BUILD_A124
    void DoWallSteer();
#endif

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    void ComputeState(float dT, State &state) const;

    Tire &GetWheel(unsigned int i) {
        return *mTires[i];
    }

    const Tire &GetWheel(unsigned int i) const {
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
    this->mAppliedTorque = 0.0f;
    this->SetForce(UMath::Vector3::kZero);
    this->mLateralForce = 0.0f;
    this->mLongitudeForce = 0.0f;
}

void SuspensionTrailer::Tire::EndFrame(float dT) {}

void SuspensionTrailer::Tire::UpdateFree(float dT) {
    this->mLoad = 0.0f;
    this->mSlip = 0.0f;
    this->mSlipping = false;
    if (this->mEBrake > 0.0f || this->mBrake > 0.0f) {
        this->mAV = 0.0f;
    }
    this->mLateralForce = 0.0f;
    this->mLongitudeForce = 0.0f;
}

float TrailerCorneringForceTable[7] = {0.0f, 0.4f, 0.8f, 1.2f, 1.6f, 1.75f, 1.65f};
Table TrailerCorneringForce(TrailerCorneringForceTable, 7, 0.0f, 1.0f / 3.0f);

BIND_BEHAVIOR_FACTORY(SuspensionTrailer);

Behavior *SuspensionTrailer::Construct(const BehaviorParams &params) {
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(UCRC32_BASE)));
    return new SuspensionTrailer(params, sp);
}

void SuspensionTrailer::OnBehaviorChange(const UCrc32 &mechanic) {
    Chassis::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mIDynamicsEntity);
        this->GetOwner()->QueryInterface(&this->mRBComplex);
        this->GetOwner()->QueryInterface(&this->mRB);
    } else if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        this->GetOwner()->QueryInterface(&this->mIDamage);
    }
}

void SuspensionTrailer::MatchSpeed(float speed) {
    for (int i = 0; i < 4; ++i) {
        float w = this->mTires[i]->GetRadius();
        this->mTires[i]->SetAngularVelocity(speed / w);
    }
}

void SuspensionTrailer::ComputeState(float dT, State &state) const {
    Chassis::ComputeState(dT, state);
    if ((this->mIDynamicsEntity != nullptr) && !Dynamics::Articulation::IsJoined(this->mIDynamicsEntity)) {
        state.ebrake_input = 0.0f;
        state.brake_input = 0.0f;
        state.gas_input = 0.0f;
        state.cog = UMath::Vector3::kZero;
        this->mRBComplex->SetCenterOfGravity(state.cog);
    } else {
        float fwbias = this->mSuspensionInfo->FRONT_WEIGHT_BIAS();
        float cg_z = 0.0f;
        float cg_y = INCH2METERS(this->mSuspensionInfo->ROLL_CENTER());
        float ride_height = UMath::Max(this->GetRideHeight(0), this->GetRideHeight(2));
        state.cog = UVector3(0.0f, cg_y - (state.dimension.y + ride_height), cg_z);
        this->mRBComplex->SetCenterOfGravity(state.cog);
    }
}

void SuspensionTrailer::OnTaskSimulate(float dT) {
    if ((this->mRB == nullptr) || (this->mRBComplex == nullptr)) {
        return;
    }
    ISimable *owner = GetOwner();
    State state;
    this->ComputeState(dT, state);
    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->BeginFrame();
    }
    this->DoSimpleAero(state);
    this->DoWheelForces(state);
    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->UpdateTime(dT);
    }
    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->EndFrame(dT);
    }
    if (GetNumWheelsOnGround() != 0) {
        this->mTimeInAir = 0.0f;
    } else {
        this->mTimeInAir += dT;
    }
    Chassis::OnTaskSimulate(dT);
}

UMath::Vector3 SuspensionTrailer::GetWheelCenterPos(unsigned int i) const {
    UMath::Vector3 pos = this->mTires[i]->GetPosition();
    if (this->mRBComplex == nullptr) {
        return pos;
    } else {
        UMath::ScaleAdd(this->mRBComplex->GetUpVector(), this->GetWheelRadius(i), pos, pos);
        return pos;
    }
}

void SuspensionTrailer::Reset() {
    ISimable *owner = this->GetOwner();
    IRigidBody &rigidBody = *owner->GetRigidBody();
    unsigned int numonground = 0;
    for (unsigned int i = 0; i < this->GetNumWheels(); ++i) {
        Tire &wheel = this->GetWheel(i);
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
    this->mNumWheelsOnGround = numonground;
    this->mTimeInAir = 0.0f;
    this->mMotionDampingFactor = 0.0f;
}

void SuspensionTrailer::DoSimpleAero(State &state) {
    const float dragcoef_spec = this->mSuspensionInfo->DRAG_COEFFICIENT();
    float speed = state.speed;
    float drag = -(speed * dragcoef_spec);
    UVector3 drag_vector = state.linear_vel;
    drag_vector *= drag;
    this->mRB->ResolveForce(drag_vector);
}
