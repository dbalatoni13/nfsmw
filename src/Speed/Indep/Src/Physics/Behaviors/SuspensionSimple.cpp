
#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
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
        void BeginFrame(float max_slip, float grip_scale, float traction_boost);
        void EndFrame(float dT);
        void UpdateFree(float dT);
        float UpdateLoaded(float lat_vel, float fwd_vel, float body_speed, float load, float dT, float drag_reduction);

        void MatchSpeed(float speed) {
            mAV = speed / mRadius;
            mRoadSpeed = speed;
            mTraction = 1.0f;
            mBrakeLocked = false;
            mSlip = 0.0f;
            mAngularAcc = 0.0f;
            mLastTorque = 0.0f;
            mBrake = 0.0f;
            mEBrake = 0.0f;
        }

        void Reset() {
            Wheel::Reset();
            mLoad = 0.0f;
            mBrake = 0.0f;
            mEBrake = 0.0f;
            mAV = 0.0f;
            mLateralForce = 0.0f;
            mLongitudeForce = 0.0f;
            mAppliedTorque = 0.0f;
            mTractionBoost = 1.0f;
            mSlip = 0.0f;
            mLateralSpeed = 0.0f;
            mRoadSpeed = 0.0f;
            mTraction = 1.0f;
            mBrakeLocked = false;
            mAllowSlip = false;
            mLastTorque = 0.0f;
            mAngularAcc = 0.0f;
            mMaxSlip = 0.5f;
        }

        float GetRadius() const {
            return mRadius;
        }

        void ApplyTorque(float torque) {
            if (!mBrakeLocked) {
                mAppliedTorque += torque;
            }
        }

        void ScaleTractionBoost(float scale) {
            mTractionBoost *= scale;
        }

        void AllowSlip(bool b) {
            mAllowSlip = b;
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
        float mSlipAngle;                   // offset 0x100, size 0x4
        const int mAxleIndex;               // offset 0x104, size 0x4
        float mTraction;                    // offset 0x108, size 0x4
        bool mBrakeLocked;                  // offset 0x10C, size 0x1
        bool mAllowSlip;                    // offset 0x110, size 0x1
        float mLastTorque;                  // offset 0x114, size 0x4
        float mAngularAcc;                  // offset 0x118, size 0x4
        float mMaxSlip;                     // offset 0x11C, size 0x4
        float mGripBoost;                   // offset 0x120, size 0x4
    };

    static Behavior *Construct(const BehaviorParams &params);

    SuspensionSimple(const struct BehaviorParams &bp, const SuspensionParams &sp);
    void DoAerobatics(State &state);
    void DoSteering(State &state, UMath::Vector3 &right, UMath::Vector3 &left);
    void DoWallSteer(State &state);
    void DoDriveForces(State &state);

    // Overrides
    // IUnknown
    ~SuspensionSimple() override;

    // IAttributeable
    void OnAttributeChange(const Attrib::Collection *aspec, unsigned int attribkey) override;

    // ISuspension
    void MatchSpeed(float speed) override;
    UMath::Vector3 GetWheelCenterPos(unsigned int i) const override;

    // Behavior
    void Reset() override;

    // IListener
    virtual void OnCollision(const COLLISION_INFO &cinfo);

    Tire &GetWheel(unsigned int i) {
        return *mTires[i];
    }

  private:
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
    float mMaxSteering;                                          // offset 0x124, size 0x4
    float mTimeInAir;                                            // offset 0x128, size 0x4
    float mSleepTime;                                            // offset 0x12C, size 0x4
    bool mDriftPhysics;                                          // offset 0x130, size 0x1
    Tire *mTires[4];                                             // offset 0x134, size 0x10
};

void SuspensionSimple::Tire::BeginFrame(float max_slip, float grip_scale, float traction_boost) {
    mMaxSlip = max_slip;
    mAppliedTorque = 0.0f;
    SetForce(UMath::Vector3::kZero);
    mLateralForce = 0.0f;
    mLongitudeForce = 0.0f;
    mTractionBoost = traction_boost;
    mGripBoost = grip_scale;
    mAllowSlip = false;
}

void SuspensionSimple::Tire::EndFrame(float dT) {}

// TODO move
extern float WheelMomentOfInertia;

void SuspensionSimple::Tire::UpdateFree(float dT) {
    mTraction = 0.0f;
    mLoad = 0.0f;
    mSlip = 0.0f;
    mSlipAngle = 0.0f;

    mBrakeLocked = mEBrake > 0.0f && mAxleIndex == 1;
    if (mBrakeLocked) {
        mAV = 0.0f;
    } else {
        float angularacc = mAppliedTorque / WheelMomentOfInertia * dT;
        mAV += angularacc;
    }
    mLateralForce = 0.0f;
    mLongitudeForce = 0.0f;
    mLastTorque = 0.0f;
    mAngularAcc = 0.0f;
}

float SuspensionSimple::Tire::UpdateLoaded(float lat_vel, float fwd_vel, float body_speed, float load, float dT, float drag_reduction) {
    if (mLoad <= 0.0f && !mBrakeLocked) {
        mAV = fwd_vel / mRadius;
    }

    const float fwd_acc = (fwd_vel - mRoadSpeed) / dT;
    mRoadSpeed = fwd_vel;
    mLateralSpeed = lat_vel;
    mLoad = UMath::Max(load, 0.0f);
    mTraction = 1.0f;

    const bool noBrake = mBrake + mEBrake <= 0.0f;
    if (!noBrake) {
        const float abs_fwd = UMath::Abs(fwd_vel);
        if (abs_fwd >= 1.0f) {
            float brake_torque = mBrake * FTLB2NM(mBrakes->BRAKES().At(mAxleIndex)) * 10.0f +
                                 mEBrake * FTLB2NM(mBrakes->EBRAKE()) * 10.0f;
            if (mAV <= 0.0f) {
                brake_torque = mAppliedTorque + brake_torque;
            } else {
                brake_torque = mAppliedTorque - brake_torque;
            }
            mAppliedTorque = brake_torque;
        } else {
            mAppliedTorque = mAppliedTorque + ((-mBrake * load) * fwd_vel) / mRadius + ((-mEBrake * load) * fwd_vel) / mRadius;
        }
    }

    mSlipAngle = UMath::Atan2a(lat_vel, UMath::Abs(fwd_vel));
    if (!mBrakeLocked && !mAllowSlip) {
        mSlip = 0.0f;
    } else {
        mSlip = mAV * mRadius - fwd_vel;
    }

    const float abs_slip = UMath::Abs(mSlip);
    bool clippedSlip = false;
    float slipRatio = 1.0f;
    if (mAllowSlip && mMaxSlip < abs_slip) {
        slipRatio = mMaxSlip / abs_slip;
        clippedSlip = true;
        mTraction = mTraction * slipRatio;
    }

    const float skid_speed = UMath::Sqrt(mSlip * mSlip + lat_vel * lat_vel);
    float slip_scale = UMath::Abs(mSlipAngle) / (1.54f * 1.5f);
    if (slip_scale > 1.0f) {
        slip_scale = 1.0f;
    } else if (slip_scale < 0.0f) {
        slip_scale = 0.0f;
    }

    float lateralForce = ((mGripBoost * slip_scale) * mLoad) * 0.85f * mSpecs->GRIP_SCALE().At(mAxleIndex) * mTractionBoost;
    mLateralForce = lateralForce;
    if (mSlipAngle > 0.0f) {
        mLateralForce = -lateralForce;
    }

    if (mBrakeLocked && skid_speed > 1.0f) {
        const float dynamicGrip = mSpecs->DYNAMIC_GRIP().At(mAxleIndex);
        mLongitudeForce = ((mSlip * mLoad) * mTractionBoost * dynamicGrip) / (skid_speed * 1.5f);
        mLateralForce = (((-lat_vel * mLoad) * mTractionBoost) * dynamicGrip * 1.5f) / skid_speed;
    } else {
        float longitudeForce = mAppliedTorque / mRadius;
        if (clippedSlip) {
            float speedRatio = (body_speed - 30.0f * 0.44703f) / (50.0f * 0.44703f - 30.0f * 0.44703f);
            if (speedRatio > 1.0f) {
                speedRatio = 1.0f;
            } else if (speedRatio < 0.0f) {
                speedRatio = 0.0f;
            }

            float dynamicGrip =
                ((((mLoad * mSpecs->DYNAMIC_GRIP().At(mAxleIndex)) * mTractionBoost) * (speedRatio * 0.5f + 0.5f)) / skid_speed) * mSlip;
            mLongitudeForce = dynamicGrip;
            if (dynamicGrip * longitudeForce <= 0.0f) {
                longitudeForce = dynamicGrip;
            } else if (dynamicGrip <= 0.0f) {
                if (longitudeForce < dynamicGrip) {
                    longitudeForce = dynamicGrip;
                }
            } else if (dynamicGrip < longitudeForce) {
                longitudeForce = dynamicGrip;
            }
        }
        mLongitudeForce = longitudeForce;
    }

    if (mAllowSlip && !clippedSlip && !mBrakeLocked) {
        mLongitudeForce = mLongitudeForce + (((mLastTorque * mRadius - fwd_acc) * 10.0f) / mRadius);
    }

    const float max_force =
        ((mLoad * mSpecs->STATIC_GRIP().At(mAxleIndex)) * mTractionBoost) * (mBrake * 0.5f + 1.0f);
    const float force_sq = mLongitudeForce * mLongitudeForce + mLateralForce * mLateralForce;
    if (max_force * max_force < force_sq && 1.0e-6f < force_sq) {
        const float forceScale = max_force / UMath::Sqrt(force_sq);
        mTraction = mTraction * forceScale;
        mLateralForce = mLateralForce * forceScale;
        mLongitudeForce = mLongitudeForce * forceScale;
        slipRatio = slipRatio * forceScale * forceScale;
    }

    if (UMath::Abs(fwd_vel) <= 1.0f) {
        float lateralScale = 1.0f;
        const float abs_lat = UMath::Abs(lat_vel);
        if (abs_lat < 1.0f) {
            lateralScale = abs_lat;
        }
        mLateralForce = mLateralForce * lateralScale;
    } else {
        mLongitudeForce =
            mLongitudeForce - ((sinf(mSlipAngle * 6.2831855f) * mLateralForce) * drag_reduction) / mSpecs->GRIP_SCALE().At(mAxleIndex);
    }

    if (noBrake && mWheelIndex > 1) {
        mLongitudeForce = mLongitudeForce * mSurface.DRIVE_GRIP();
        mLateralForce = mLateralForce * mSurface.LATERAL_GRIP();
    }

    if (!mBrakeLocked) {
        if (!mAllowSlip || slipRatio >= 1.0f) {
            mAV = mRoadSpeed / mRadius;
        } else {
            const float radius = mRadius;
            float lastTorque = ((mAppliedTorque - mLongitudeForce * radius) + mLastTorque) * 0.5f;
            mLastTorque = lastTorque;
            float angularAcc =
                ((lastTorque - mAV * (mSurface.ROLLING_RESISTANCE() * 2.5f)) * 50.0f) - (slipRatio * mSlip) / (radius * dT);
            angularAcc = ((fwd_acc / radius) - angularAcc) * slipRatio + angularAcc;
            mAngularAcc = angularAcc;
            mAV = mAV + angularAcc * dT;
        }
    } else {
        mAV = 0.0f;
        mAngularAcc = 0.0f;
    }

    return mLateralForce;
}

Behavior *SuspensionSimple::Construct(const BehaviorParams &params) {
    // "BASE"
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(0xa6b47fac)));
    return new SuspensionSimple(params, sp);
}

void SuspensionSimple::OnAttributeChange(const Attrib::Collection *aspec, unsigned int attribkey) {}

void SuspensionSimple::MatchSpeed(float speed) {
    for (int i = 0; i < 4; ++i) {
        mTires[i]->MatchSpeed(speed);
    }
}

void SuspensionSimple::DoAerobatics(State &state) {
    if (state.flags & 2) {
        return;
    }
    DoJumpStabilizer(state);
}

UMath::Vector3 SuspensionSimple::GetWheelCenterPos(unsigned int i) const {
    UMath::Vector3 pos = mTires[i]->GetPosition();
    if (!mRBComplex) {
        return pos;
    }
    UMath::ScaleAdd(mRBComplex->GetUpVector(), GetWheelRadius(i), pos, pos);
    return pos;
}

// TODO move?
static bool printit;

void SuspensionSimple::Reset() {
    ISimable *owner = GetOwner();
    unsigned int numonground = 0;
    IRigidBody *rigidBody = owner->GetRigidBody();
    for (unsigned int i = 0; i < 4; ++i) {
        Tire &wheel = GetWheel(i);
        wheel.Reset();

        if (wheel.InitPosition(*rigidBody, wheel.GetRadius())) {
            float newCompression = wheel.GetNormal().w + GetRideHeight(i);
            if (newCompression <= FLOAT_EPSILON) {
                newCompression = 0.0f;
            }
            wheel.SetCompression(newCompression);
            if (newCompression > 0.0f) {
                numonground++;
            }
        }
    };
    mNumWheelsOnGround = numonground;
    mAgainstWall = 0.0f;
    mTimeInAir = 0.0f;
    mSleepTime = 0.0f;
    printit = 1;
}

void SuspensionSimple::OnCollision(const COLLISION_INFO &cinfo) {
    if (cinfo.type != Sim::Collision::Info::WORLD) {
        return;
    }
    if ((UMath::Abs(cinfo.normal.y) < 0.1f) && (mAgainstWall == 0.0f) && mRBComplex) {
        if (UMath::Length(cinfo.closingVel) < 7.5f) {
            const UMath::Vector3 &vFoward = mRBComplex->GetForwardVector();
            const UMath::Vector3 &vRight = mRBComplex->GetRightVector();
            float dirdot = UMath::Dot(cinfo.normal, vFoward);
            if (dirdot <= 0.0f) {
                UMath::Vector3 rpos;
                UMath::Sub(cinfo.position, mRB->GetPosition(), rpos);
                dirdot = UMath::Dot(rpos, vFoward);
                if (dirdot > mRB->GetDimension().z * 0.75f) {
                    float dot = UMath::Dot(cinfo.normal, vRight);
                    mAgainstWall = (dot > 0.0f ? -1.0f : 1.0f) - dot;
                }
            }
        }
    }
}

void SuspensionSimple::DoSteering(Chassis::State &state, UMath::Vector3 &right, UMath::Vector3 &left) {
    float steer_coeff = UMath::Max(state.ebrake_input, state.brake_input);
    mMaxSteering = UMath::Lerp(45.0f, 60.0f, UMath::Max(steer_coeff, 1.0f - state.gas_input));
    float steer_input = state.steer_input * mMaxSteering;
    float trueesteer;
    if (state.driver_style != 1) {
        steer_input *= mTireInfo.STEERING();
    }
    UMath::Vector4 r4;
    UMath::Vector4 l4;
    ComputeAckerman(steer_input * DEG2ANGLE(1.0f), state, &l4, &r4);

    right = UMath::Vector4To3(r4);
    left = UMath::Vector4To3(l4);
    mWheelSteer[0] = l4.w;
    mWheelSteer[1] = r4.w;
    DoWallSteer(state);
}

void SuspensionSimple::DoWallSteer(State &state) {
    float wall = mAgainstWall;
    if ((wall != 0.0f) && (GetNumWheelsOnGround() > 2) && (state.gas_input > 0.0f)) {
        float dW = state.steer_input * state.gas_input * 0.125f;
        if (wall * dW < 0.0f) {
            return;
        }
        dW *= UMath::Abs(wall);
        UMath::Vector3 chg = {};
        chg.y = dW;
        UMath::Rotate(chg, state.matrix, chg);
        UMath::Add(state.angular_vel, chg, chg);
        mRB->SetAngularVelocity(chg);
    }
}

void SuspensionSimple::DoDriveForces(State &state) {
    if (!mTrany) {
        return;
    }
    float drive_torque = mTrany->GetDriveTorque();
    if (UMath::Abs(drive_torque) <= FLOAT_EPSILON) {
        return;
    }
    float split = mDrivetrainInfo->TORQUE_SPLIT();
    float front_drive = drive_torque * split;
    bool slip_tires = false;
    bool is_staging = state.flags & 1;
    if (is_staging || (state.gear < 4 && (state.gas_input > 0.5f))) {
        slip_tires = true;
    }
    float traction_boost = state.nos_boost * state.shift_boost;
    if (is_staging) {
        traction_boost *= 0.25f;
    }
    if (front_drive != 0.0f) {
        for (unsigned int i = 0; i < 2; ++i) {
            Tire *tire = mTires[i];
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
            Tire *tire = mTires[i];
            if (tire->IsOnGround()) {
                tire->ApplyTorque(rear_drive * 0.5f);
                tire->AllowSlip(slip_tires);
                tire->ScaleTractionBoost(traction_boost);
            }
        }
    }
}
