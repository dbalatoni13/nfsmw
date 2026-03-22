
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
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
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

        void SetBrake(float brake) {
            mBrake = brake;
        }

        void SetEBrake(float ebrake) {
            mEBrake = ebrake;
        }

        float GetAngularVelocity() const {
            return mAV;
        }

        float GetLoad() const {
            return mLoad;
        }

        float GetTraction() const {
            return mTraction;
        }

        float GetSlip() const {
            return mSlip;
        }

        float GetLateralSpeed() const {
            return mLateralSpeed;
        }

        float GetSlipAngle() const {
            return mSlipAngle;
        }

        float GetToleratedSlip() const {
            return mMaxSlip;
        }

        void SetAngularVelocity(float av) {
            mAV = av;
        }

        float GetLongitudeForce() const {
            return mLongitudeForce;
        }

        void ApplyTorque(float torque) {
            if (!mBrakeLocked) {
                mAppliedTorque += torque;
            }
        }

        void ScaleTractionBoost(float scale) {
            mTractionBoost *= scale;
        }

        void SetTractionBoost(float boost) {
            mTractionBoost = boost;
        }

        void AllowSlip(bool b) {
            mAllowSlip = b;
        }

        void Stop() {
            mAV = 0.0f;
            mSlip = 0.0f;
            mRoadSpeed = 0.0f;
            mSlipAngle = 0.0f;
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
    void CreateTires();
    void DoAerobatics(State &state);
    void DoSteering(State &state, UMath::Vector3 &right, UMath::Vector3 &left);
    void DoWallSteer(State &state);
    void DoDriveForces(State &state);
    void DoWheelForces(State &state);

    // Overrides
    // IUnknown
    ~SuspensionSimple() override;

    // IAttributeable
    void OnAttributeChange(const Attrib::Collection *aspec, unsigned int attribkey) override;

    // ISuspension
    float GetWheelTraction(unsigned int index) const override {
        return mTires[index]->GetTraction();
    }
    unsigned int GetNumWheels() const override {
        return 4;
    }
    const UMath::Vector3 &GetWheelPos(unsigned int i) const override {
        return mTires[i]->GetPosition();
    }
    const UMath::Vector3 &GetWheelLocalPos(unsigned int i) const override {
        return mTires[i]->GetLocalArm();
    }
    UMath::Vector3 GetWheelCenterPos(unsigned int i) const override;
    float GetWheelLoad(unsigned int i) const override {
        return mTires[i]->GetLoad();
    }
    void ApplyVehicleEntryForces(bool enteringVehicle, const UMath::Vector3 &pos, bool calledfromEvent) override {}
    const float GetWheelRoadHeight(unsigned int i) const override {
        return mTires[i]->GetNormal().w;
    }
    bool IsWheelOnGround(unsigned int i) const override {
        return mTires[i]->IsOnGround();
    }
    float GetCompression(unsigned int i) const override {
        return mTires[i]->GetCompression();
    }
    float GetWheelSlip(unsigned int idx) const override {
        return mTires[idx]->GetSlip();
    }
    float GetToleratedSlip(unsigned int idx) const override {
        return mTires[idx]->GetToleratedSlip();
    }
    float GetWheelSkid(unsigned int idx) const override {
        return mTires[idx]->GetLateralSpeed();
    }
    float GetWheelSlipAngle(unsigned int idx) const override {
        return mTires[idx]->GetSlipAngle();
    }
    const UMath::Vector4 &GetWheelRoadNormal(unsigned int i) const override {
        return mTires[i]->GetNormal();
    }
    const SimSurface &GetWheelRoadSurface(unsigned int i) const override {
        return mTires[i]->GetSurface();
    }
    const UMath::Vector3 &GetWheelVelocity(unsigned int i) const override {
        return mTires[i]->GetVelocity();
    }
    int GetNumWheelsOnGround() const override {
        return mNumWheelsOnGround;
    }
    Radians GetWheelAngularVelocity(int index) const override {
        return mTires[index]->GetAngularVelocity();
    }
    void SetWheelAngularVelocity(int wheel, float w) override {
        mTires[wheel]->SetAngularVelocity(w);
    }
    float GetWheelSteer(unsigned int wheel) const override {
        if (wheel < 2) {
            return RAD2ANGLE(mWheelSteer[wheel]);
        }
        return 0.0f;
    }
    float GetWheelRadius(unsigned int index) const override {
        return mTires[index]->GetRadius();
    }
    float GetMaxSteering() const override {
        return DEG2ANGLE(mMaxSteering);
    }
    void MatchSpeed(float speed) override;

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void OnTaskSimulate(float dT) override;
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

SuspensionSimple::Tire::Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes)
    : Wheel(1), //
      mRadius(UMath::Max(radius, 0.1f)), //
      mBrake(0.0f), //
      mEBrake(0.0f), //
      mAV(0.0f), //
      mLoad(0.0f), //
      mLateralForce(0.0f), //
      mLongitudeForce(0.0f), //
      mAppliedTorque(0.0f), //
      mTractionBoost(1.0f), //
      mSlip(0.0f), //
      mLateralSpeed(0.0f), //
      mWheelIndex(index), //
      mRoadSpeed(0.0f), //
      mSpecs(specs), //
      mBrakes(brakes), //
      mSlipAngle(0.0f), //
      mAxleIndex(index >> 1), //
      mBrakeLocked(false), //
      mAllowSlip(false), //
      mLastTorque(0.0f), //
      mAngularAcc(0.0f), //
      mMaxSlip(0.5f), //
      mGripBoost(1.0f) {}

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
    mBrakeLocked = mEBrake > 0.0f && mAxleIndex == 1;

    const bool noBrake = mBrake + mEBrake <= 0.0f;
    if (!noBrake) {
        const float abs_fwd = UMath::Abs(fwd_vel);
        if (abs_fwd < 1.0f) {
            mAppliedTorque = mAppliedTorque + ((-mBrake * load) * fwd_vel) / mRadius + ((-mEBrake * load) * fwd_vel) / mRadius;
        } else {
            float brake_torque = mBrake * FTLB2NM(mBrakes->BRAKES().At(mAxleIndex)) * 10.0f +
                                 mEBrake * FTLB2NM(mBrakes->EBRAKE()) * 10.0f;
            if (mAV <= 0.0f) {
                brake_torque = mAppliedTorque + brake_torque;
            } else {
                brake_torque = mAppliedTorque - brake_torque;
            }
            mAppliedTorque = brake_torque;
        }
    }

    mSlipAngle = UMath::Atan2a(lat_vel, UMath::Abs(fwd_vel));
    if (!mBrakeLocked) {
        if (!mAllowSlip) {
            mSlip = 0.0f;
        } else {
            mSlip = mAV * mRadius - fwd_vel;
        }
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
    float slip_scale = 1.0f;
    const float slip_angle_scale = UMath::Abs(mSlipAngle) / (1.54f * 1.5f);
    if (slip_angle_scale < slip_scale) {
        slip_scale = slip_angle_scale;
    }
    if (slip_scale < 0.0f) {
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
    } else if (!clippedSlip) {
        mLongitudeForce = mAppliedTorque / mRadius;
    } else {
        float longitudeForce = mAppliedTorque / mRadius;
        float speedRatio = 1.0f;
        const float bodyRatio = (body_speed - 30.0f * 0.44703f) / (50.0f * 0.44703f - 30.0f * 0.44703f);
        if (bodyRatio < speedRatio) {
            speedRatio = bodyRatio;
        }
        if (speedRatio < 0.0f) {
            speedRatio = 0.0f;
        }

        float dynamicGrip =
            ((((mLoad * mSpecs->DYNAMIC_GRIP().At(mAxleIndex)) * mTractionBoost) * (speedRatio * 0.5f + 0.5f)) / skid_speed) * mSlip;
        mLongitudeForce = dynamicGrip;
        if (dynamicGrip * longitudeForce <= 0.0f) {
            longitudeForce = dynamicGrip;
        } else if (0.0f < dynamicGrip) {
            if (dynamicGrip < longitudeForce) {
                longitudeForce = dynamicGrip;
            }
        } else if (longitudeForce < dynamicGrip) {
            longitudeForce = dynamicGrip;
        }
        mLongitudeForce = longitudeForce;
    }

    if (mAllowSlip && !clippedSlip && !mBrakeLocked) {
        mLongitudeForce = mLongitudeForce + (((mAngularAcc * mRadius - fwd_acc) * 10.0f) / mRadius);
    }

    const float max_force =
        ((mLoad * mSpecs->STATIC_GRIP().At(mAxleIndex)) * mTractionBoost) * (mBrake * 0.5f + 1.0f);
    const float force_sq = mLongitudeForce * mLongitudeForce + mLateralForce * mLateralForce;
    if (max_force * max_force < force_sq && 1.0e-6f < force_sq) {
        const float forceScale = max_force / UMath::Sqrt(force_sq);
        mTraction = mTraction * forceScale;
        mLateralForce = mLateralForce * forceScale;
        mLongitudeForce = mLongitudeForce * forceScale;
        slipRatio = slipRatio * forceScale;
        slipRatio = slipRatio * forceScale;
    }

    if (UMath::Abs(fwd_vel) > 1.0f) {
        const float gripScale = mSpecs->GRIP_SCALE().At(mAxleIndex);
        mLongitudeForce = mLongitudeForce - ((sinf(mSlipAngle * 6.2831855f) * mLateralForce) * drag_reduction) / gripScale;
    } else {
        float lateralScale = 1.0f;
        const float abs_lat = UMath::Abs(lat_vel);
        if (abs_lat < 1.0f) {
            lateralScale = abs_lat;
        }
        mLateralForce = mLateralForce * lateralScale;
    }

    if (noBrake && mWheelIndex > 1) {
        mLongitudeForce = mLongitudeForce * mSurface.DRIVE_GRIP();
        mLateralForce = mLateralForce * mSurface.LATERAL_GRIP();
    }

    if (mBrakeLocked) {
        mAV = 0.0f;
        mAngularAcc = 0.0f;
    } else if (!mAllowSlip || slipRatio >= 1.0f) {
        mAV = mRoadSpeed / mRadius;
    } else {
        const float radius = mRadius;
        float lastTorque = ((mAppliedTorque - mLongitudeForce * radius) + mLastTorque) * 0.5f;
        mLastTorque = lastTorque;
        float angularAcc = ((lastTorque - (mAV * 2.5f) * mSurface.ROLLING_RESISTANCE()) * 50.0f) - (slipRatio * mSlip) / (radius * dT);
        angularAcc = ((fwd_acc / radius) - angularAcc) * slipRatio + angularAcc;
        mAngularAcc = angularAcc;
        mAV = angularAcc * dT + mAV;
    }

    return mLateralForce;
}

Behavior *SuspensionSimple::Construct(const BehaviorParams &params) {
    // "BASE"
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(0xa6b47fac)));
    return new SuspensionSimple(params, sp);
}

SuspensionSimple::SuspensionSimple(const BehaviorParams &bp, const SuspensionParams &sp)
    : Chassis(bp),                //
      mBrakeInfo(this, 0),        //
      mTireInfo(this, 0),         //
      mSuspensionInfo(this, 0),   //
      mDrivetrainInfo(this, 0),   //
      mRB(0),                     //
      mRBComplex(0),              //
      mInput(0),                  //
      mTrany(0),                  //
      mCheater(0),                //
      mFrictionBoost(1.0f),       //
      mDraft(1.0f),               //
      mPowerSliding(false),       //
      mYawControlMultiplier(1.0f), //
      mNumWheelsOnGround(0),       //
      mAgainstWall(0.0f),          //
      mMaxSteering(45.0f),         //
      mTimeInAir(0.0f),            //
      mSleepTime(0.0f),            //
      mDriftPhysics(false) {
    GetOwner()->QueryInterface(&mRB);
    GetOwner()->QueryInterface(&mRBComplex);
    GetOwner()->QueryInterface(&mInput);
    GetOwner()->QueryInterface(&mTrany);
    GetOwner()->QueryInterface(&mCheater);
    Sim::Collision::AddListener(this, GetOwner(), "SuspensionSimple");

    mWheelSteer[0] = 0.0f;
    mWheelSteer[1] = 0.0f;

    for (int i = 0; i < 4; ++i) {
        mTires[i] = 0;
    }

    CreateTires();
}

SuspensionSimple::~SuspensionSimple() {
    for (int i = 0; i < 4; ++i) {
        delete mTires[i];
    }
    Sim::Collision::RemoveListener(this);
}

void SuspensionSimple::OnAttributeChange(const Attrib::Collection *aspec, unsigned int attribkey) {}

void SuspensionSimple::OnBehaviorChange(const UCrc32 &mechanic) {
    Chassis::OnBehaviorChange(mechanic);

    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mInput);
    } else if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mRBComplex);
        GetOwner()->QueryInterface(&mRB);
    } else if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        GetOwner()->QueryInterface(&mTrany);
    } else if (mechanic == BEHAVIOR_MECHANIC_AI) {
        GetOwner()->QueryInterface(&mCheater);
    }
}

void SuspensionSimple::CreateTires() {
    for (int i = 0; i < 4; ++i) {
        delete mTires[i];
        float diameter = Physics::Info::WheelDiameter(mTireInfo, i < 2);
        mTires[i] = new Tire(diameter * 0.5f, i, mTireInfo, mBrakeInfo);
    }

    UMath::Vector3 dimension;
    GetOwner()->GetRigidBody()->GetDimension(dimension);

    float wheelbase = mSuspensionInfo.WHEEL_BASE();
    float axle_width_f = mSuspensionInfo.TRACK_WIDTH().At(0) - mTireInfo.SECTION_WIDTH().At(0) * 0.001f;
    float axle_width_r = mSuspensionInfo.TRACK_WIDTH().At(1) - mTireInfo.SECTION_WIDTH().At(1) * 0.001f;
    float front_axle = mSuspensionInfo.FRONT_AXLE();

    UVector3 fl(-axle_width_f * 0.5f, -dimension.y, front_axle);
    UVector3 fr(axle_width_f * 0.5f, -dimension.y, front_axle);
    UVector3 rl(-axle_width_r * 0.5f, -dimension.y, front_axle - wheelbase);
    UVector3 rr(axle_width_r * 0.5f, -dimension.y, front_axle - wheelbase);

    GetWheel(0).SetLocalArm(fl);
    GetWheel(1).SetLocalArm(fr);
    GetWheel(2).SetLocalArm(rl);
    GetWheel(3).SetLocalArm(rr);
}

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

void SuspensionSimple::DoWheelForces(State &state) {
    const float dT = state.time;

    UMath::Vector3 steerR;
    UMath::Vector3 steerL;
    DoSteering(state, steerR, steerL);

    for (unsigned int i = 2; i < 4; ++i) {
        mTires[i]->SetEBrake(state.ebrake_input);
    }

    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->SetBrake(state.brake_input);
    }

    float catchup_cheat = 0.5f;
    if (state.driver_style == 3 && state.driver_class != 1) {
        if (!mCheater) {
            catchup_cheat = 1.0f;
        } else {
            catchup_cheat = mCheater->GetCatchupCheat();
        }
        catchup_cheat = catchup_cheat * (0.5f - 0.23f) + 0.23f;
    }

    float shock_specs[2];
    float shock_ext_specs[2];
    float shock_valving[2];
    float shock_digression[2];
    float spring_specs[2];
    float sway_specs[2];
    float travel_specs[2];
    float rideheight_specs[2];
    float progression[2];

    for (unsigned int i = 0; i < 2; ++i) {
        shock_specs[i] = LBIN2NM(mSuspensionInfo.SHOCK_STIFFNESS().At(i));
        shock_ext_specs[i] = LBIN2NM(mSuspensionInfo.SHOCK_EXT_STIFFNESS().At(i));
        shock_valving[i] = INCH2METERS(mSuspensionInfo.SHOCK_VALVING().At(i));
        shock_digression[i] = 1.0f - mSuspensionInfo.SHOCK_DIGRESSION().At(i);
        spring_specs[i] = LBIN2NM(mSuspensionInfo.SPRING_STIFFNESS().At(i));
        sway_specs[i] = LBIN2NM(mSuspensionInfo.SWAYBAR_STIFFNESS().At(i));
        travel_specs[i] = INCH2METERS(mSuspensionInfo.TRAVEL().At(i));
        rideheight_specs[i] = INCH2METERS(mSuspensionInfo.RIDE_HEIGHT().At(i));
        progression[i] = mSuspensionInfo.SPRING_PROGRESSION().At(i);
    }

    float sway_stiffness[4];
    sway_stiffness[0] = (mTires[0]->GetCompression() - mTires[1]->GetCompression()) * sway_specs[0];
    sway_stiffness[1] = -sway_stiffness[0];
    sway_stiffness[2] = (mTires[2]->GetCompression() - mTires[3]->GetCompression()) * sway_specs[1];
    sway_stiffness[3] = -sway_stiffness[2];

    UMath::Vector3 steering_normals[4];
    steering_normals[0] = steerL;
    steering_normals[1] = steerR;
    steering_normals[2] = state.GetForwardVector();
    steering_normals[3] = state.GetForwardVector();

    unsigned int wheelsOnGround = 0;
    float maxDelta = 0.0f;
    bool resolve = false;

    const UMath::Vector3 &vUp = state.GetUpVector();
    const float maxShockForce = mSuspensionInfo.SHOCK_BLOWOUT() * state.mass * 0.25f * 9.81f;

    for (unsigned int i = 0; i < 4; ++i) {
        const unsigned int axle = i / 2;
        Tire &wheel = GetWheel(i);
        wheel.UpdatePosition(state.angular_vel, state.linear_vel, state.matrix, state.world_cog, state.time, wheel.GetRadius(), true, state.collider,
                             state.dimension.y * 2.0f);

        const UMath::Vector3 &groundNormal = UMath::Vector4To3(wheel.GetNormal());
        const UMath::Vector3 &forwardNormal = steering_normals[i];
        UMath::Vector3 lateralNormal;
        UMath::UnitCross(groundNormal, forwardNormal, lateralNormal);

        const float upness = UMath::Clamp(UMath::Dot(groundNormal, vUp), 0.0f, 1.0f);
        const float oldCompression = wheel.GetCompression();
        float newCompression = rideheight_specs[axle] * upness + wheel.GetNormal().w;
        const float maxCompression = travel_specs[axle];

        if (oldCompression == 0.0f) {
            maxDelta = UMath::Max(maxDelta, newCompression - maxCompression);
        }

        newCompression = UMath::Max(newCompression, 0.0f);
        if (maxCompression < newCompression) {
            maxDelta = UMath::Max(maxDelta, newCompression - maxCompression);
            newCompression = maxCompression;
        }

        if (newCompression <= FLOAT_EPSILON || upness <= VehicleSystem::ENABLE_ROLL_STOPS_THRESHOLD) {
            wheel.SetForce(UMath::Vector3::kZero);
            wheel.UpdateFree(dT);
        } else {
            ++wheelsOnGround;

            float rise = (newCompression - oldCompression) / dT;
            if (FLOAT_EPSILON < shock_valving[axle] && shock_digression[axle] < 1.0f) {
                const float abs_rise = UMath::Abs(rise);
                if (shock_valving[axle] < abs_rise) {
                    float digressed = shock_valving[axle] * UMath::Pow(abs_rise / shock_valving[axle], shock_digression[axle]);
                    rise = rise < 0.0f ? -digressed : digressed;
                }
            }

            float damp = rise * (rise <= 0.0f ? shock_ext_specs[axle] : shock_specs[axle]);
            if (maxShockForce < damp) {
                damp = 0.0f;
            }

            float verticalLoad = damp + (newCompression * spring_specs[axle]) * (newCompression * progression[axle] + 1.0f) + sway_stiffness[i];
            verticalLoad = UMath::Max(verticalLoad, 0.0f);

            const UMath::Vector3 &pointVelocity = wheel.GetVelocity();
            const float xspeed = UMath::Dot(pointVelocity, lateralNormal);
            const float zspeed = UMath::Dot(pointVelocity, forwardNormal);
            float traction_force = wheel.UpdateLoaded(xspeed, state.local_vel.z, state.local_vel.z, verticalLoad, dT, catchup_cheat);

            float max_traction = (xspeed / dT) * 9.81f * (state.mass * 0.25f);
            if (max_traction < 0.0f) {
                max_traction = -max_traction;
            }
            traction_force = UMath::Clamp(traction_force, -max_traction, max_traction);

            UMath::Vector3 lateralForce;
            UMath::Scale(lateralNormal, traction_force, lateralForce);

            UMath::Vector3 driveForce;
            UMath::UnitCross(lateralNormal, groundNormal, driveForce);
            UMath::Scale(driveForce, wheel.GetLongitudeForce(), driveForce);

            UMath::Vector3 verticalForce;
            UMath::Scale(vUp, verticalLoad, verticalForce);

            UMath::Vector3 force;
            UMath::Add(lateralForce, driveForce, force);
            UMath::Add(force, verticalForce, force);
            wheel.SetForce(force);
            resolve = true;
        }

        if (newCompression == 0.0f) {
            wheel.IncAirTime(dT);
        } else {
            wheel.SetAirTime(0.0f);
        }
        wheel.SetCompression(newCompression);
    }

    if (resolve) {
        UMath::Vector3 total_force = UMath::Vector3::kZero;
        UMath::Vector3 total_torque = UMath::Vector3::kZero;

        for (unsigned int i = 0; i < 4; ++i) {
            Tire &wheel = GetWheel(i);
            UMath::Vector3 p = wheel.GetLocalArm();
            p.y += wheel.GetCompression();
            p.y -= rideheight_specs[i / 2];
            UMath::RotateTranslate(p, state.matrix, p);
            wheel.SetPosition(p);

            UMath::Vector3 r;
            UMath::Sub(p, state.world_cog, r);
            UMath::Sub(r, state.GetPosition(), r);

            UMath::Vector3 torque;
            UMath::Cross(r, wheel.GetForce(), torque);
            UMath::Add(total_force, wheel.GetForce(), total_force);
            UMath::Add(total_torque, torque, total_torque);
        }

        const float yaw = UMath::Dot(vUp, total_torque);
        UMath::ScaleAdd(vUp, yaw * mYawControlMultiplier - yaw, total_torque, total_torque);
        mRB->Resolve(total_force, total_torque);
    }

    if (0.0f < maxDelta) {
        for (unsigned int i = 0; i < 4; ++i) {
            Tire &wheel = GetWheel(i);
            wheel.SetY(wheel.GetPosition().y + maxDelta);
        }
        mRB->ModifyYPos(maxDelta);
    }

    if (wheelsOnGround != 0 && !mNumWheelsOnGround) {
        state.local_angular_vel.y *= 0.5f;
        UMath::Rotate(state.local_angular_vel, state.matrix, state.angular_vel);
        mRB->SetAngularVelocity(state.angular_vel);
    }

    mNumWheelsOnGround = wheelsOnGround;
}

void SuspensionSimple::OnTaskSimulate(float dT) {
    if (!mInput || !mRBComplex || !mRB) {
        return;
    }
    if (!mRBComplex->IsModeling()) {
        return;
    }

    float ride_extra = 0.0f;
    if (mNumWheelsOnGround > 1) {
        ride_extra = 2.0f;
    } else if (mNumWheelsOnGround == 0) {
        ride_extra = -10.0f;
    }
    SetCOG(0.0f, ride_extra);

    State state;
    ComputeState(dT, state);

    if (mSleepTime > 3.0f && DoSleep(state) == SS_ALL) {
        return;
    }

    float max_slip = ComputeMaxSlip(state);
    float grip_scale = ComputeLateralGripScale(state);
    float traction_scale = ComputeTractionScale(state);
    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->BeginFrame(max_slip, grip_scale, traction_scale);
    }

    float traction_boost = UMath::Abs(GetVehicle()->GetSlipAngle() - mRB->GetDimension().y * 0.25f * RAD2ANGLE(1.0f));
    traction_boost = UMath::Clamp(traction_boost / DEG2ANGLE(35.0f), 0.0f, 1.0f);

    float speed_scale = UMath::Clamp(state.speed / 20.0f, 0.0f, 1.0f);
    traction_boost = traction_boost * speed_scale * 0.15f;
    if (state.gear == 6) {
        traction_boost = 0.0f;
    }
    mTires[2]->SetTractionBoost(traction_boost + 1.0f);
    mTires[3]->SetTractionBoost(traction_boost + 1.0f);

    float drag_pct = 1.0f;
    float aero_pct = 1.0f;
    if (mCheater) {
        float catchup = mCheater->GetCatchupCheat();
        drag_pct = 1.0f - catchup;
        aero_pct = catchup * (1.5f - aero_pct) + aero_pct;
    }
    if (state.driver_class == 1) {
        aero_pct = UMath::Max(1.5f, aero_pct);
    }

    DoAerodynamics(state, drag_pct, aero_pct, mTires[0]->GetLocalArm().z, mTires[2]->GetLocalArm().z, 0);
    DoDriveForces(state);
    DoWheelForces(state);

    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->UpdateTime(dT);
    }

    mAgainstWall = 0.0f;

    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->EndFrame(dT);
    }

    if (GetNumWheelsOnGround() != 0) {
        mTimeInAir = 0.0f;
    } else {
        mTimeInAir += dT;
    }

    DoAerobatics(state);
    if (DoSleep(state) == SS_ALL) {
        for (unsigned int i = 0; i < 4; ++i) {
            mTires[i]->Stop();
        }
        mSleepTime += dT;
    } else {
        mSleepTime = 0.0f;
    }

    Chassis::OnTaskSimulate(dT);
}
