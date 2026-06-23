
#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/IAttributeable.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICheater.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Wheel.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// total size: 0x144
class SuspensionSimple : public Chassis, public Sim::Collision::IListener, public IAttributeable, public Debugable {
  public:
    // total size: 0x124
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

        float GetTraction() const {
            return this->mTraction;
        }
        float GetRadius() const {
            return this->mRadius;
        }
        Radians GetAngularVelocity() const {
            return this->mAV;
        }
        Radians GetAngularAcceleration() const {
            return this->mAngularAcc;
        }
        void SetAngularVelocity(Radians w) {
            this->mAV = w;
        }
        Newtons GetLoad() const {
            return this->mLoad;
        }

        Mps GetToleratedSlip() const {
            return this->mMaxSlip;
        }
        Mps GetCurrentSlip() const {
            return this->mSlip;
        }

        void SetTractionBoost(float boost) {
            this->mTractionBoost = boost;
        }
        void ScaleTractionBoost(float scale) {
            this->mTractionBoost *= scale;
        }

        void Stop() {}

        void BeginFrame(float max_slip, float grip_scale, float traction_boost);
        void EndFrame(float dT);

        void AllowSlip(bool b) {
            this->mAllowSlip = b;
        }

        void ApplyTorque(float torque) {
            if (!this->mBrakeLocked) {
                this->mAppliedTorque += torque;
            }
        }

        float GetLateralForce() const {
            return this->mLateralForce;
        }
        float GetLongitudeForce() const {
            return this->mLongitudeForce;
        }
        float GetLateralSpeed() const {
            return this->mLateralSpeed;
        }

        bool IsSteeringWheel() const {
            return this->mWheelIndex < 2; // TODO correct?
        }

        float UpdateLoaded(float lat_vel, float fwd_vel, float body_speed, float load, float dT, float drag_reduction);
        void UpdateFree(float dT);

        Nm GetTorque() const {
            return this->mLastTorque;
        }

        void Reset() {
            Wheel::Reset();
            this->mLoad = 0.0f;
            this->mBrake = 0.0f;
            this->mEBrake = 0.0f;
            this->mAV = 0.0f;
            this->mLateralForce = 0.0f;
            this->mLongitudeForce = 0.0f;
            this->mAppliedTorque = 0.0f;
            this->mTractionBoost = 1.0f;
            this->mSlip = 0.0f;
            this->mLateralSpeed = 0.0f;
            this->mRoadSpeed = 0.0f;
            this->mTraction = 1.0f;
            this->mBrakeLocked = false;
            this->mAllowSlip = false;
            this->mLastTorque = 0.0f;
            this->mAngularAcc = 0.0f;
            this->mMaxSlip = 0.5f;
        }

        void MatchSpeed(Mps speed) {
            this->mAV = speed / mRadius;
            this->mRoadSpeed = speed;
            this->mTraction = 1.0f;
            this->mBrakeLocked = false;
            this->mSlip = 0.0f;
            this->mAngularAcc = 0.0f;
            this->mLastTorque = 0.0f;
            this->mBrake = 0.0f;
            this->mEBrake = 0.0f;
        }

        Angle GetSlipAngle() const {
            return mSlipAngle;
        }

      private:
        const float mRadius;                // offset 0xC4, size 0x4
        float mBrake;                       // offset 0xC8, size 0x4
        float mEBrake;                      // offset 0xCC, size 0x4
        float mAV;                          // offset 0xD0, size 0x4
        float mLoad;                        // offset 0xD4, size 0x4
        float mLateralForce;                // offset 0xD8, size 0x4
        float mLongitudeForce;              // offset 0xDC, size 0x4
        float mAppliedTorque;               // offset 0xE0, size 0x4
        float mTractionBoost;               // offset 0xE4, size 0x4
        float mSlip;                        // offset 0xE8, size 0x4
        float mLateralSpeed;                // offset 0xEC, size 0x4
        const int mWheelIndex;              // offset 0xF0, size 0x4
        float mRoadSpeed;                   // offset 0xF4, size 0x4
        const Attrib::Gen::tires *mSpecs;   // offset 0xF8, size 0x4
        const Attrib::Gen::brakes *mBrakes; // offset 0xFC, size 0x4
        Angle mSlipAngle;                   // offset 0x100, size 0x4
        const int mAxleIndex;               // offset 0x104, size 0x4
        float mTraction;                    // offset 0x108, size 0x4
        bool mBrakeLocked;                  // offset 0x10C, size 0x1
        bool mAllowSlip;                    // offset 0x110, size 0x1
        Nm mLastTorque;                     // offset 0x114, size 0x4
        Radians mAngularAcc;                // offset 0x118, size 0x4
        float mMaxSlip;                     // offset 0x11C, size 0x4
        float mGripBoost;                   // offset 0x120, size 0x4
    };

    SuspensionSimple(const struct BehaviorParams &bp, const SuspensionParams &sp);
    static Behavior *Construct(const BehaviorParams &params);
    ~SuspensionSimple() override;

    // IAttributeable
    void OnAttributeChange(const Attrib::Collection *aspec, Attrib::Key attribkey) override;

    void OnDebugDraw();

    // ISuspension
    void MatchSpeed(float speed) override;
    float GetWheelTraction(unsigned int index) const override {}
    float GetWheelAngularVelocity(int index) const override {}
    float GetWheelRadius(unsigned int index) const override {}
    float GetWheelSlip(unsigned int idx) const override {}
    Mps GetToleratedSlip(unsigned int idx) const override {}
    Newtons GetWheelLoad(unsigned int i) const override {}
    float GetWheelSkid(unsigned int idx) const override {}
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

    // IListener
    void OnCollision(const COLLISION_INFO &cinfo) override;

    // Behavior
    void OnTaskSimulate(float dT) override;
    void Reset() override;

  protected:
    void DoWheelForces(struct State &state);
    void DoDriveForces(State &state);
    void DoSteering(State &state, UMath::Vector3 &right, UMath::Vector3 &left);
    void DoWallSteer(State &state);
    void DoAerobatics(State &state);

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    Tire &GetWheel(unsigned int i) {
        return *mTires[i];
    }

    const Tire &GetWheel(unsigned int i) const {
        return *mTires[i];
    }

  private:
    void CreateTires();

    BehaviorSpecsPtr<Attrib::Gen::brakes> mBrakeInfo;            // offset 0xA0, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::tires> mTireInfo;              // offset 0xB4, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::chassis> mSuspensionInfo;      // offset 0xC8, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::transmission> mDrivetrainInfo; // offset 0xDC, size 0x14
    IRigidBody *mRB;                                             // offset 0xF0, size 0x4
    ICollisionBody *mRBComplex;                                  // offset 0xF4, size 0x4
    IInput *mInput;                                              // offset 0xF8, size 0x4
    ITransmission *mTrany;                                       // offset 0xFC, size 0x4
    ICheater *mCheater;                                          // offset 0x100, size 0x4
    float mFrictionBoost;                                        // offset 0x104, size 0x4
    float mDraft;                                                // offset 0x108, size 0x4
    bool mPowerSliding;                                          // offset 0x10C, size 0x1
    float mWheelSteer[2];                                        // offset 0x110, size 0x8
    float mYawControlMultiplier;                                 // offset 0x118, size 0x4
    unsigned int mNumWheelsOnGround;                             // offset 0x11C, size 0x4
    float mAgainstWall;                                          // offset 0x120, size 0x4
    Degrees mMaxSteering;                                        // offset 0x124, size 0x4
    float mTimeInAir;                                            // offset 0x128, size 0x4
    float mSleepTime;                                            // offset 0x12C, size 0x4
    bool mDriftPhysics;                                          // offset 0x130, size 0x1
    Tire *mTires[4];                                             // offset 0x134, size 0x10
};

void SuspensionSimple::Tire::BeginFrame(float max_slip, float grip_scale, float traction_boost) {
    this->mMaxSlip = max_slip;
    this->mAppliedTorque = 0.0f;
    this->SetForce(UMath::Vector3::kZero);
    this->mLateralForce = 0.0f;
    this->mLongitudeForce = 0.0f;
    this->mTractionBoost = traction_boost;
    this->mGripBoost = grip_scale;
    this->mAllowSlip = false;
}

void SuspensionSimple::Tire::EndFrame(float dT) {}

extern float WheelMomentOfInertia;

void SuspensionSimple::Tire::UpdateFree(float dT) {
    this->mTraction = 0.0f;
    this->mLoad = 0.0f;
    this->mSlip = 0.0f;
    this->mSlipAngle = 0.0f;

    this->mBrakeLocked = this->mEBrake > 0.0f && this->mAxleIndex == 1;
    if (this->mBrakeLocked) {
        this->mAV = 0.0f;
    } else {
        float angularacc = this->mAppliedTorque / WheelMomentOfInertia * dT;
        this->mAV += angularacc;
    }
    this->mLateralForce = 0.0f;
    this->mLongitudeForce = 0.0f;
    this->mLastTorque = 0.0f;
    this->mAngularAcc = 0.0f;
}

// TODO value and use
// static const float Tweak_SimpleSuspensionRollingFriction;
// static const float Tweak_SimpleSuspensionWheelIntertia;
// static const float Tweak_SimplePhysLoadFactor;
// static const float Tweak_SimpleBrakeScale;
// static const float Tweak_SimpleEBrakeScale;
// static const float Tweak_SimpleStagingTraction;
// static const float Tweak_SimpleBrakeLockSkidRatio;
// static const float Tweak_ExtraBrakeGrip;
static const float Tweak_SimpleBrakeSpeed = 5.0f;

// TODO value and use
// static const float Tweak_AITurningDragReductionMin;
// static const float Tweak_AITurningDragReductionMax;

BIND_BEHAVIOR_FACTORY(SuspensionSimple);

Behavior *SuspensionSimple::Construct(const BehaviorParams &params) {
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(UCRC32_BASE)));
    return new SuspensionSimple(params, sp);
}

void SuspensionSimple::OnAttributeChange(const Attrib::Collection *aspec, Attrib::Key attribkey) {}

void SuspensionSimple::MatchSpeed(float speed) {
    for (int i = 0; i < 4; ++i) {
        this->mTires[i]->MatchSpeed(speed);
    }
}

void SuspensionSimple::DoAerobatics(State &state) {
    if (state.flags & 2) {
        return;
    }
    this->DoJumpStabilizer(state);
}

// TODO value and use
// static const float Tweak_SimpleMaxOverSteerAngle;
// static const float Tweak_SimpleMaxOverSteerSpeed;
// static const float Tweak_OverSteerGripCorrection;
// static const float Tweak_MaxAIAeroCheat;
// static const float Tweak_MaxAIDragCheat;

UMath::Vector3 SuspensionSimple::GetWheelCenterPos(unsigned int i) const {
    UMath::Vector3 pos = this->mTires[i]->GetPosition();
    if (this->mRBComplex == nullptr) {
        return pos;
    }
    UMath::ScaleAdd(this->mRBComplex->GetUpVector(), this->GetWheelRadius(i), pos, pos);
    return pos;
}

static bool printit = false;

void SuspensionSimple::Reset() {
    ISimable *owner = this->GetOwner();
    unsigned int numonground = 0;
    IRigidBody *rigidBody = owner->GetRigidBody();
    for (unsigned int i = 0; i < 4; ++i) {
        Tire &wheel = this->GetWheel(i);
        wheel.Reset();

        if (wheel.InitPosition(*rigidBody, wheel.GetRadius())) {
            float newCompression = wheel.GetNormal().w + this->GetRideHeight(i);
            if (newCompression <= UMath::Epsilon) {
                newCompression = 0.0f;
            }
            wheel.SetCompression(newCompression);
            if (newCompression > 0.0f) {
                numonground++;
            }
        }
    };
    this->mNumWheelsOnGround = numonground;
    this->mAgainstWall = 0.0f;
    this->mTimeInAir = 0.0f;
    this->mSleepTime = 0.0f;
    printit = true;
}

void SuspensionSimple::OnCollision(const COLLISION_INFO &cinfo) {
    if (cinfo.type != Sim::Collision::Info::WORLD) {
        return;
    }
    if ((UMath::Abs(cinfo.normal.y) < 0.1f) && (mAgainstWall == 0.0f) && (this->mRBComplex != nullptr)) {
        if (UMath::Length(cinfo.closingVel) < 7.5f) {
            const UMath::Vector3 &vFoward = this->mRBComplex->GetForwardVector();
            const UMath::Vector3 &vRight = this->mRBComplex->GetRightVector();
            float dirdot = UMath::Dot(cinfo.normal, vFoward);
            if (dirdot <= 0.0f) {
                UMath::Vector3 rpos;
                UMath::Sub(cinfo.position, this->mRB->GetPosition(), rpos);
                dirdot = UMath::Dot(rpos, vFoward);
                if (dirdot > this->mRB->GetDimension().z * 0.75f) {
                    float dot = UMath::Dot(cinfo.normal, vRight);
                    this->mAgainstWall = (dot > 0.0f ? 1.0f : -1.0f) - dot;
                }
            }
        }
    }
}

// TODO use and value
// static const float Tweak_NormalAISteer;
// static const float Tweak_BrakingAISteer;

void SuspensionSimple::DoSteering(Chassis::State &state, UMath::Vector3 &right, UMath::Vector3 &left) {
    float steer_coeff = UMath::Max(state.ebrake_input, state.brake_input);
    mMaxSteering = UMath::Lerp(45.0f, 60.0f, UMath::Max(steer_coeff, 1.0f - state.gas_input));
    float steer_input = state.steer_input * this->mMaxSteering;
    float trueesteer;
    if (state.driver_style != 1) {
        steer_input *= this->mTireInfo.STEERING();
    }
    UMath::Vector4 r4;
    UMath::Vector4 l4;
    this->ComputeAckerman(steer_input * DEG2ANGLE(1.0f), state, &l4, &r4);

    right = UMath::Vector4To3(r4);
    left = UMath::Vector4To3(l4);
    this->mWheelSteer[0] = l4.w;
    this->mWheelSteer[1] = r4.w;
    this->DoWallSteer(state);
}

void SuspensionSimple::DoWallSteer(State &state) {
    float wall = this->mAgainstWall;
    if ((wall != 0.0f) && (this->GetNumWheelsOnGround() > 2) && (state.gas_input > 0.0f)) {
        float dW = state.steer_input * state.gas_input * 0.125f;
        if (wall * dW < 0.0f) {
            return;
        }
        dW *= UMath::Abs(wall);
        UMath::Vector3 chg = {};
        chg.y = dW;
        UMath::Rotate(chg, state.matrix, chg);
        UMath::Add(state.angular_vel, chg, chg);
        this->mRB->SetAngularVelocity(chg);
    }
}

static const bool Tweak_AllowSimpleTireSlip = true;
// static const float Tweak_SimpleSlipThrottle; // TODO value and use

void SuspensionSimple::DoDriveForces(State &state) {
    if (this->mTrany == nullptr) {
        return;
    }
    float drive_torque = this->mTrany->GetDriveTorque();
    if (UMath::Abs(drive_torque) <= UMath::Epsilon) {
        return;
    }
    float split = this->mDrivetrainInfo->TORQUE_SPLIT();
    float front_drive = drive_torque * split;
    bool slip_tires = false;
    bool is_staging = (state.flags & 1) != 0;
    if (is_staging || (state.gear < 4 && (state.gas_input > 0.5f))) {
        slip_tires = true;
    }
    float traction_boost = state.nos_boost * state.shift_boost;
    if (is_staging) {
        traction_boost *= 0.25f;
    }
    if (front_drive != 0.0f) {
        for (unsigned int i = 0; i < 2; ++i) {
            Tire *tire = this->mTires[i];
            if (tire->IsOnGround()) {
                tire->ApplyTorque(front_drive * 0.5f);
                tire->AllowSlip(slip_tires);
                tire->ScaleTractionBoost(traction_boost);
            }
        };
    }
    float rear_drive = drive_torque * (1.0f - split);
    if (rear_drive != 0.0f) {
        for (unsigned int i = 2; i < 4; ++i) {
            Tire *tire = this->mTires[i];
            if (tire->IsOnGround()) {
                tire->ApplyTorque(rear_drive * 0.5f);
                tire->AllowSlip(slip_tires);
                tire->ScaleTractionBoost(traction_boost);
            }
        }
    }
}
