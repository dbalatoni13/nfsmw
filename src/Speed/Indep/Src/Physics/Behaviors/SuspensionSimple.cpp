
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
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
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

        void Stop() {
            this->mAV = 0.0f;
            this->mSlip = 0.0f;
            this->mRoadSpeed = 0.0f;
            this->mSlipAngle = 0.0f;
        }

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
    float GetWheelTraction(unsigned int index) const override {
        return mTires[index]->GetTraction();
    }
    float GetWheelAngularVelocity(int index) const override {
        return mTires[index]->GetAngularVelocity();
    }
    float GetWheelRadius(unsigned int index) const override {
        return mTires[index]->GetRadius();
    }
    float GetWheelSlip(unsigned int idx) const override {
        return mTires[idx]->GetCurrentSlip();
    }
    Mps GetToleratedSlip(unsigned int idx) const override {
        return mTires[idx]->GetToleratedSlip();
    }
    Newtons GetWheelLoad(unsigned int i) const override {
        return mTires[i]->GetLoad();
    }
    float GetWheelSkid(unsigned int idx) const override {
        return mTires[idx]->GetLateralSpeed();
    }
    void SetWheelAngularVelocity(int wheel, float w) override {
        mTires[wheel]->SetAngularVelocity(w);
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
    void ApplyVehicleEntryForces(bool enteringVehicle, const UMath::Vector3 &pos, bool calledfromEvent) override {}
    const float GetWheelRoadHeight(unsigned int i) const override {
        return mTires[i]->GetNormal().w;
    }
    float GetCompression(unsigned int i) const override {
        return mTires[i]->GetCompression();
    }
    const UMath::Vector4 &GetWheelRoadNormal(unsigned int i) const override {
        return mTires[i]->GetNormal();
    }
    bool IsWheelOnGround(unsigned int i) const override {
        return mTires[i]->IsOnGround();
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
    float GetWheelSteer(unsigned int wheel) const override {
        if (wheel < 2) {
            return RAD2ANGLE(mWheelSteer[wheel]);
        }
        return 0.0f;
    }
    float GetMaxSteering() const override {
        return DEG2ANGLE(mMaxSteering);
    }
    Angle GetWheelSlipAngle(unsigned int idx) const override {
        return mTires[idx]->GetSlipAngle();
    }

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

SuspensionSimple::Tire::Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes)
    : Wheel(1),                          //
      mRadius(UMath::Max(radius, 0.1f)), //
      mBrake(0.0f),                      //
      mEBrake(0.0f),                     //
      mAV(0.0f),                         //
      mLoad(0.0f),                       //
      mLateralForce(0.0f),               //
      mLongitudeForce(0.0f),             //
      mAppliedTorque(0.0f),              //
      mTractionBoost(1.0f),              //
      mSlip(0.0f),                       //
      mLateralSpeed(0.0f),               //
      mWheelIndex(index),                //
      mRoadSpeed(0.0f),                  //
      mSpecs(specs),                     //
      mBrakes(brakes),                   //
      mSlipAngle(0.0f),                  //
      mAxleIndex(index >> 1),            //
      mBrakeLocked(false),               //
      mAllowSlip(false),                 //
      mLastTorque(0.0f),                 //
      mAngularAcc(0.0f),                 //
      mMaxSlip(0.5f),                    //
      mGripBoost(1.0f) {}

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
static const float Tweak_SimpleSuspensionRollingFriction = 1.5f;
static const float Tweak_SimpleSuspensionWheelIntertia = 10.0f;
// static const float Tweak_SimplePhysLoadFactor;
static const float Tweak_SimpleBrakeScale = 1.0f;
static const float Tweak_SimpleEBrakeScale = 1.0f;
static const float Tweak_SimpleStagingTraction = 0.25f;
static const float Tweak_SimpleBrakeLockSkidRatio = 1.5f;
static const float Tweak_ExtraBrakeGrip = 1.0f;
static const float Tweak_SimpleBrakeSpeed = 5.0f;

// TODO value and use
static const float Tweak_AITurningDragReductionMin = 0.23f;
static const float Tweak_AITurningDragReductionMax = 0.3f;

// UNSOLVED, float math
float SuspensionSimple::Tire::UpdateLoaded(float lat_vel, float fwd_vel, float body_speed, float load, float dT, float drag_reduction) {
    if (this->mLoad <= 0.0f && !this->mBrakeLocked) {
        this->mAV = fwd_vel / this->mRadius;
    }

    float foward_acc = (fwd_vel - this->mRoadSpeed) / dT;

    this->mRoadSpeed = fwd_vel;
    this->mLateralSpeed = lat_vel;
    this->mLoad = UMath::Max(load, 0.0f);
    this->mTraction = 1.0f;
    this->mBrakeLocked = this->mEBrake > 0.0f && this->mAxleIndex == 1;

    bool braking = this->mBrake + this->mEBrake > 0.0f;
    if (braking) {
        float abs_fwd = UMath::Abs(fwd_vel);

        if (abs_fwd < 1.0f) {
            float bt = (-this->mBrake * load * fwd_vel) / this->mRadius;
            float ebt = (-this->mEBrake * load * fwd_vel) / this->mRadius;
            float total_brake = bt + ebt;
            this->mAppliedTorque = this->mAppliedTorque + total_brake;
        } else {
            float bt = this->mBrake * FTLB2NM(this->mBrakes->BRAKES().At(this->mAxleIndex));
            float ebt = this->mEBrake * FTLB2NM(this->mBrakes->EBRAKE());
            float total_brake = bt * 10.0f + ebt * 10.0f;

            this->mAppliedTorque += this->mAV <= 0.0f ? total_brake : -total_brake;
        }
    }

    this->mSlipAngle = UMath::Atan2a(lat_vel, UMath::Abs(fwd_vel));

    if (this->mBrakeLocked || this->mAllowSlip) {
        this->mSlip = this->mAV * this->mRadius - fwd_vel;
    } else {
        this->mSlip = 0.0f;
    }

    float catchupfriction; // TODO use
    bool is_slipping = false;
    float slip_ratio = 1.0f;
    float abs_slip = UMath::Abs(this->mSlip);

    if (this->mAllowSlip && abs_slip > this->mMaxSlip) {
        slip_ratio = this->mMaxSlip / abs_slip;
        is_slipping = true;
        this->mTraction = this->mTraction * slip_ratio;
    }

    float skid_speed = UMath::Sqrt(this->mSlip * this->mSlip + lat_vel * lat_vel);
    float angle_range = UMath::Ramp(UMath::Abs(this->mSlipAngle), 0.0f, DEG2ANGLE(12.0f));

    this->mLateralForce =
        this->mGripBoost * angle_range * 1.54f * this->mLoad * this->mSpecs->GRIP_SCALE().At(this->mAxleIndex) * this->mTractionBoost;

    if (this->mSlipAngle > 0.0f) {
        this->mLateralForce = -this->mLateralForce;
    }

    if (this->mBrakeLocked && skid_speed > 1.0f) {
        float friction = this->mLoad * this->mTractionBoost * this->mSpecs->DYNAMIC_GRIP().At(this->mAxleIndex);
        this->mLongitudeForce = (this->mSlip * friction) / (skid_speed * Tweak_SimpleBrakeLockSkidRatio);
        this->mLateralForce = ((-lat_vel * friction) * Tweak_SimpleBrakeLockSkidRatio) / skid_speed;
    } else if (is_slipping) {
        float PilotFactor = 0.85f;
        float speed_factor = UMath::Ramp(body_speed, MPH2MPS(30.0f), MPH2MPS(50.0f));
        float pilot_factor = speed_factor * (1.0f - PilotFactor) + PilotFactor;
        float dynamicfriction =
            ((this->mSpecs->DYNAMIC_GRIP().At(this->mAxleIndex) * this->mTractionBoost * pilot_factor * this->mLoad) / skid_speed) * this->mSlip;
        float groundfriction = this->mAppliedTorque / this->mRadius;

        this->mLongitudeForce = dynamicfriction;
        this->mLongitudeForce = UMath::Limit(this->mLongitudeForce, groundfriction);
    } else {
        this->mLongitudeForce = this->mAppliedTorque / this->mRadius;
    }

    if (this->mAllowSlip && !is_slipping && !this->mBrakeLocked) {
        float acc_diff = this->mAngularAcc * this->GetRadius() - foward_acc;
        this->mLongitudeForce += (acc_diff * Tweak_SimpleSuspensionWheelIntertia) / this->mRadius;
    }

    float total_force_sq = this->mLongitudeForce * this->mLongitudeForce + this->mLateralForce * this->mLateralForce;
    float max_force = (this->mLoad * this->mSpecs->STATIC_GRIP().At(this->mAxleIndex) * this->mTractionBoost) * (this->mBrake * 2.5f + 1.0f);

    if (total_force_sq > max_force * max_force && total_force_sq > UMath::Epsilon) {
        float ratio = max_force / UMath::Sqrt(total_force_sq);
        this->mTraction = this->mTraction * ratio;
        this->mLateralForce = this->mLateralForce * ratio;
        this->mLongitudeForce = this->mLongitudeForce * ratio;
        slip_ratio *= ratio * ratio;
    }

    if (UMath::Abs(fwd_vel) > 1.0f) {
        this->mLongitudeForce = this->mLongitudeForce - (UMath::Sina(this->mSlipAngle) * this->mLateralForce * drag_reduction) /
                                                            this->mSpecs->GRIP_SCALE().At(this->mAxleIndex);
    } else {
        this->mLateralForce = this->mLateralForce * UMath::Min(UMath::Abs(lat_vel), 1.0f);
    }

    if (!braking && IsRear(this->mWheelIndex)) {
        this->mLongitudeForce = this->mLongitudeForce * this->GetSurface().DRIVE_GRIP();
        this->mLateralForce = this->mLateralForce * this->GetSurface().LATERAL_GRIP();
    }

    if (this->mBrakeLocked) {
        this->mAngularAcc = 0.0f;
        this->mAV = 0.0f;
    } else if (this->mAllowSlip && slip_ratio < 1.0f) {
        float torque = ((this->mAppliedTorque - this->mLongitudeForce * this->mRadius) + this->mLastTorque) * 0.5f;

        this->mLastTorque = torque;
        float effective_torque = torque - (this->mAV * 1.5f) * this->GetSurface().ROLLING_RESISTANCE();

        this->mAngularAcc =
            UMath::Lerp(effective_torque * 0.1f - (slip_ratio * this->mSlip) / (this->mRadius * dT), foward_acc / this->mRadius, slip_ratio);
        this->mAV = this->mAngularAcc * dT + this->mAV;
    } else {
        this->mAV = this->mRoadSpeed / this->mRadius;
    }

    return this->mLateralForce;
}

BIND_BEHAVIOR_FACTORY(SuspensionSimple);

Behavior *SuspensionSimple::Construct(const BehaviorParams &params) {
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(UCRC32_BASE)));
    return new SuspensionSimple(params, sp);
}

SuspensionSimple::SuspensionSimple(const BehaviorParams &bp, const SuspensionParams &sp)
    : Chassis(bp),                 //
      mBrakeInfo(this, 0),         //
      mTireInfo(this, 0),          //
      mSuspensionInfo(this, 0),    //
      mDrivetrainInfo(this, 0),    //
      mRB(nullptr),                //
      mRBComplex(nullptr),         //
      mInput(nullptr),             //
      mFrictionBoost(1.0f),        //
      mDraft(1.0f),                //
      mPowerSliding(false),        //
      mYawControlMultiplier(1.0f), //
      mNumWheelsOnGround(0),       //
      mAgainstWall(0.0f),          //
      mMaxSteering(45.0f),         //
      mTimeInAir(0.0f),            //
      mSleepTime(0.0f),            //
      mDriftPhysics(false) {
    this->EnableProfile("SuspensionSimple");

    IAttributeable::Register(this, Attrib::ClassName::brakes);
    IAttributeable::Register(this, Attrib::ClassName::tires);
    IAttributeable::Register(this, Attrib::ClassName::chassis);

    this->MakeDebugable(DBG_PHYSICS_RACERS);

    this->mWheelSteer[0] = this->mWheelSteer[1] = 0.0f;

    this->GetOwner()->QueryInterface(&this->mRB);
    this->GetOwner()->QueryInterface(&this->mRBComplex);
    this->GetOwner()->QueryInterface(&this->mInput);
    this->GetOwner()->QueryInterface(&this->mTrany);
    this->GetOwner()->QueryInterface(&this->mCheater);
    Sim::Collision::AddListener(this, this->GetOwner(), "SuspensionSimple");

    for (int i = 0; i < 4; ++i) {
        this->mTires[i] = nullptr;
    }

    this->CreateTires();
}

SuspensionSimple::~SuspensionSimple() {
    for (int i = 0; i < 4; ++i) {
        delete this->mTires[i];
    }
    Sim::Collision::RemoveListener(this);
}

void SuspensionSimple::OnAttributeChange(const Attrib::Collection *aspec, Attrib::Key attribkey) {}

void SuspensionSimple::CreateTires() {
    for (int i = 0; i < 4; ++i) {
        delete this->mTires[i];

        bool is_front = i < 2;
        float diameter = Physics::Info::WheelDiameter(this->mTireInfo, is_front);
        this->mTires[i] = new Tire(diameter * 0.5f, i, this->mTireInfo, this->mBrakeInfo);
    }

    UMath::Vector3 dimension;
    this->GetOwner()->GetRigidBody()->GetDimension(dimension);

    float wheelbase = this->mSuspensionInfo.WHEEL_BASE();
    float axle_width_f = this->mSuspensionInfo->TRACK_WIDTH().Front - this->mTireInfo->SECTION_WIDTH().Front * 0.001f;
    float axle_width_r = this->mSuspensionInfo->TRACK_WIDTH().Rear - this->mTireInfo->SECTION_WIDTH().Rear * 0.001f;
    float front_axle = this->mSuspensionInfo.FRONT_AXLE();

    UVector3 fl(-axle_width_f * 0.5f, -dimension.y, front_axle);
    UVector3 fr(axle_width_f * 0.5f, -dimension.y, front_axle);
    UVector3 rl(-axle_width_r * 0.5f, -dimension.y, front_axle - wheelbase);
    UVector3 rr(axle_width_r * 0.5f, -dimension.y, front_axle - wheelbase);

    this->GetWheel(0).SetLocalArm(fl);
    this->GetWheel(1).SetLocalArm(fr);
    this->GetWheel(2).SetLocalArm(rl);
    this->GetWheel(3).SetLocalArm(rr);
}

void SuspensionSimple::OnBehaviorChange(const UCrc32 &mechanic) {
    Chassis::OnBehaviorChange(mechanic);

    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mInput);
    } else if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mRBComplex);
        this->GetOwner()->QueryInterface(&this->mRB);
    } else if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        this->GetOwner()->QueryInterface(&this->mTrany);
    } else if (mechanic == BEHAVIOR_MECHANIC_AI) {
        this->GetOwner()->QueryInterface(&this->mCheater);
    }
}

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
static const float Tweak_SimpleMaxOverSteerAngle = 35.0f;
static const float Tweak_SimpleMaxOverSteerSpeed = 20.0f;
static const float Tweak_OverSteerGripCorrection = 0.15f;
// static const float Tweak_MaxAIAeroCheat;
// static const float Tweak_MaxAIDragCheat;

void SuspensionSimple::OnTaskSimulate(float dT) {
    if ((this->mInput == nullptr) || (this->mRBComplex == nullptr) || (this->mRB == nullptr)) {
        return;
    }
    if (!this->mRBComplex->IsModeling()) {
        return;
    }

    ISimable *owner = this->GetOwner();

    float extra_ride = 0.0f;
    if (this->mNumWheelsOnGround > 1) {
        extra_ride = 2.0f;
    } else if (this->mNumWheelsOnGround == 0) {
        extra_ride = -10.0f;
    }
    this->SetCOG(0.0f, extra_ride);

    State state;
    this->ComputeState(dT, state);

    if (this->mSleepTime > 3.0f && this->DoSleep(state) == SS_ALL) {
        return;
    }

    float max_slip = this->ComputeMaxSlip(state);
    float grip_scale = this->ComputeLateralGripScale(state);
    float traction_scale = this->ComputeTractionScale(state);
    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->BeginFrame(max_slip, grip_scale, traction_scale);
    }

    float oversteerangle = UMath::Abs(this->GetVehicle()->GetSlipAngle() - RAD2ANGLE(this->mRB->GetAngularVelocity().y * 0.25f));
    float grip_factor = UMath::Ramp(oversteerangle, 0.0f, DEG2ANGLE(Tweak_SimpleMaxOverSteerAngle));
    grip_factor *= UMath::Ramp(state.speed, 0.0f, Tweak_SimpleMaxOverSteerSpeed);
    grip_factor *= Tweak_OverSteerGripCorrection;
// TODO
#ifndef EA_BUILD_A124
    if (state.driver_class == 6) {
        grip_factor = 0.0f;
    }
#endif
    this->mTires[2]->SetTractionBoost(grip_factor + 1.0f);
    this->mTires[3]->SetTractionBoost(grip_factor + 1.0f);

    float aero_pct = 1.0f;
    float drag_pct = 1.0f;
    if (this->mCheater != nullptr) {
        float cheat = this->mCheater->GetCatchupCheat();
        drag_pct = UMath::Lerp(drag_pct, 0.0f, cheat);
        aero_pct = UMath::Lerp(aero_pct, 1.5f, cheat);
    }
    if (state.driver_style == 1) {
        aero_pct = UMath::Max(aero_pct, 1.5f);
    }

    this->DoAerodynamics(state, drag_pct, aero_pct, this->GetWheel(0).GetLocalArm().z, this->GetWheel(2).GetLocalArm().z, nullptr);
    this->DoDriveForces(state);
    this->DoWheelForces(state);

    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->UpdateTime(dT);
    }

    this->mAgainstWall = 0.0f;

    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->EndFrame(dT);
    }

    if (this->GetNumWheelsOnGround() != 0) {
        this->mTimeInAir = 0.0f;
    } else {
        this->mTimeInAir += dT;
    }

    this->DoAerobatics(state);
    if (this->DoSleep(state) == SS_ALL) {
        for (unsigned int i = 0; i < 4; ++i) {
            this->mTires[i]->Stop();
        }
        this->mSleepTime += dT;
    } else {
        this->mSleepTime = 0.0f;
    }

    Chassis::OnTaskSimulate(dT);
}

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
        traction_boost *= Tweak_SimpleStagingTraction;
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

void SuspensionSimple::DoWheelForces(State &state) {
    const float dT = state.time;

    UVector3 steerR;
    UVector3 steerL;
    this->DoSteering(state, steerR, steerL);

    for (unsigned int i = 2; i < 4; ++i) {
        this->mTires[i]->SetEBrake(state.ebrake_input);
    }

    for (unsigned int i = 0; i < 4; ++i) {
        float speedmph = MPS2MPH(state.speed);

        this->mTires[i]->SetBrake(state.brake_input);
    }

    float steerdrag_reduction = 0.5f;
    if (state.driver_class == 3 && state.driver_style != 1) {
        float cheat;
        if (this->mCheater == nullptr) {
            cheat = 1.0f;
        } else {
            cheat = this->mCheater->GetCatchupCheat();
        }
        steerdrag_reduction = UMath::Lerp(0.23f, 0.5f, cheat);
    }

    unsigned int wheelsOnGround = 0;
    float maxDelta = 0.0f;
    const UMath::Vector3 &vFwd = state.GetForwardVector();
    const UMath::Vector3 &vUp = state.GetUpVector();
    const float mass = state.mass * 0.25f;

    float shock_specs[2];
    float spring_specs[2];
    float sway_specs[2];
    float travel_specs[2];
    float rideheight_specs[2];
    float shock_ext_specs[2];
    float shock_valving[2];
    float shock_digression[2];
    float progression[2];

    for (unsigned int i = 0; i < 2; ++i) {
        shock_specs[i] = LBIN2NM(this->mSuspensionInfo->SHOCK_STIFFNESS().At(i));
        shock_ext_specs[i] = LBIN2NM(this->mSuspensionInfo->SHOCK_EXT_STIFFNESS().At(i));
        shock_valving[i] = INCH2METERS(this->mSuspensionInfo->SHOCK_VALVING().At(i));
        shock_digression[i] = 1.0f - this->mSuspensionInfo->SHOCK_DIGRESSION().At(i);
        spring_specs[i] = LBIN2NM(this->mSuspensionInfo->SPRING_STIFFNESS().At(i));
        sway_specs[i] = LBIN2NM(this->mSuspensionInfo->SWAYBAR_STIFFNESS().At(i));
        travel_specs[i] = INCH2METERS(this->mSuspensionInfo->TRAVEL().At(i));
        rideheight_specs[i] = INCH2METERS(this->mSuspensionInfo->RIDE_HEIGHT().At(i));
        progression[i] = this->mSuspensionInfo->SPRING_PROGRESSION().At(i);
    }

    float sway_stiffness[4];
    sway_stiffness[0] = (this->mTires[0]->GetCompression() - this->mTires[1]->GetCompression()) * sway_specs[0];
    sway_stiffness[1] = -sway_stiffness[0];
    sway_stiffness[2] = (this->mTires[2]->GetCompression() - this->mTires[3]->GetCompression()) * sway_specs[1];
    sway_stiffness[3] = -sway_stiffness[2];

    UMath::Vector4 steering_normals[4];
    steering_normals[0] = UMath::Vector4Make(steerL, 0.0f);
    steering_normals[1] = UMath::Vector4Make(steerR, 0.0f);
    steering_normals[2] = UMath::Vector4Make(vFwd, 0.0f);
    steering_normals[3] = UMath::Vector4Make(vFwd, 0.0f);

    bool resolve = false;

    for (unsigned int i = 0; i < 4; ++i) {
        int axle = i >> 1;
        Tire &wheel = this->GetWheel(i);
        wheel.UpdatePosition(state.angular_vel, state.linear_vel, state.matrix, state.world_cog, state.time, wheel.GetRadius(), true, state.collider,
                             state.dimension.y * 2.0f);

        const UMath::Vector3 &groundNormal = UMath::Vector4To3(wheel.GetNormal());
        const UMath::Vector3 &forwardNormal = UMath::Vector4To3(steering_normals[i]);
        UMath::Vector3 lateralNormal;
        UMath::UnitCross(groundNormal, forwardNormal, lateralNormal);

        float penetration = wheel.GetNormal().w;
        float upness = UMath::Clamp(UMath::Dot(groundNormal, vUp), 0.0f, 1.0f);
        const float oldCompression = wheel.GetCompression();
        float newCompression = rideheight_specs[axle] * upness + penetration;
        float max_compression = travel_specs[axle];

        if (oldCompression == 0.0f) {
            float delta = newCompression - max_compression;
            maxDelta = UMath::Max(maxDelta, delta);
        }

        newCompression = UMath::Max(newCompression, 0.0f);
        if (max_compression < newCompression) {
            maxDelta = UMath::Max(maxDelta, newCompression - max_compression);
            newCompression = max_compression;
        }

        if (newCompression <= UMath::Epsilon || upness <= VehicleSystem::ENABLE_ROLL_STOPS_THRESHOLD) {
            wheel.SetForce(UMath::Vector3::kZero);
            wheel.UpdateFree(dT);
        } else {
            ++wheelsOnGround;

            float springForce = newCompression * spring_specs[axle];
            const float diff = newCompression - oldCompression;
            float rise = diff / dT;
            float spring = springForce * (newCompression * progression[axle] + 1.0f);
            float damp;

            if (UMath::Epsilon < shock_valving[axle] && shock_digression[axle] < 1.0f) {
                float abs_rise = UMath::Abs(rise);
                float valving = shock_valving[axle];
                if (valving < abs_rise) {
                    float digression = valving * UMath::Pow(abs_rise / valving, shock_digression[axle]);
                    if (rise < 0.0f) {
                        rise = -digression;
                    } else {
                        rise = digression;
                    }
                }
            }

            if (rise <= 0.0f) {
                damp = rise * shock_ext_specs[axle];
            } else {
                damp = rise * shock_specs[axle];
            }

            if ((this->mSuspensionInfo->SHOCK_BLOWOUT() * mass * 9.81f) < damp) {
                damp = 0.0f;
            }

            float load = damp + spring + sway_stiffness[i];
            load = UMath::Max(load, 0.0f);

            const UMath::Vector3 &pointVelocity = wheel.GetVelocity();
            float xspeed = UMath::Dot(pointVelocity, lateralNormal);
            float zspeed = UMath::Dot(pointVelocity, forwardNormal);
            float traction_force = wheel.UpdateLoaded(xspeed, state.local_vel.z, state.local_vel.z, load, dT, steerdrag_reduction);

            float max_traction = UMath::Abs((xspeed / dT) * 9.81f * mass);
            traction_force = UMath::Clamp(traction_force, -max_traction, max_traction);

            UMath::Vector3 lateralForce;
            UMath::Scale(lateralNormal, traction_force, lateralForce);

            UMath::Vector3 driveForce;
            UMath::UnitCross(lateralNormal, groundNormal, driveForce);
            UMath::Scale(driveForce, wheel.GetLongitudeForce());

            UMath::Vector3 verticalForce;
            UMath::Scale(vUp, load, verticalForce);

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
        UMath::Vector3 total_torque = UMath::Vector3::kZero;
        UMath::Vector3 total_force = UMath::Vector3::kZero;

        for (unsigned int i = 0; i < 4; ++i) {
            Tire &wheel = this->GetWheel(i);
            UVector3 p = wheel.GetLocalArm();
            p.y += wheel.GetCompression();
            p.y -= rideheight_specs[i >> 1];
            UMath::RotateTranslate(p, state.matrix, p);
            wheel.SetPosition(p);

            const UMath::Vector3 &force = wheel.GetForce();

            UMath::Vector3 torque;
            UMath::Vector3 r;
            UMath::Sub(p, state.world_cog, r);
            UMath::Sub(r, state.GetPosition(), r);
            UMath::Cross(r, force, torque);

            UMath::Add(total_force, force, total_force);
            UMath::Add(total_torque, torque, total_torque);
        }

        float yaw = UMath::Dot(vUp, total_torque);
        float counter_yaw = yaw * this->mTireInfo->YAW_SPEED() - yaw;
        UMath::ScaleAdd(vUp, counter_yaw, total_torque, total_torque);
        this->mRB->Resolve(total_force, total_torque);
    }

    if (0.0f < maxDelta) {
        for (unsigned int i = 0; i < 4; ++i) {
            Wheel &wheel = this->GetWheel(i);
            wheel.SetY(wheel.GetPosition().y + maxDelta);
        }
        this->mRB->ModifyYPos(maxDelta);
    }

    if (wheelsOnGround != 0 && !this->mNumWheelsOnGround) {
        state.local_angular_vel.y *= 0.5f;
        UMath::Rotate(state.local_angular_vel, state.matrix, state.angular_vel);
        this->mRB->SetAngularVelocity(state.angular_vel);
    }

    this->mNumWheelsOnGround = wheelsOnGround;
}
