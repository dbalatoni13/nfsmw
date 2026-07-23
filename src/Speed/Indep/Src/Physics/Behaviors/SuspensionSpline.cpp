
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
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/Wheel.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

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
        Tire(float radius);

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
        float GetTraction() const {
            return mLocked ? 0.0f : 1.0f;
        }

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
    void SetBurnout(float speed) override {
        this->mBurnout = speed;
    }
    float GetBurnout() const override {
        return this->mBurnout;
    }
    void SetBrakeLock(bool front, bool rear) override {
        this->mBrakeLockFront = front;
        this->mBrakeLockRear = rear;
    }
    bool SetNISPosition(const struct UMath::Matrix4 &position, bool initial, float dT) override;
    void RestoreState() override;
    void SetConstraintAngle(Degrees angle) override {
        this->mConstraint = UMath::Clamp(angle, 0.0f, 80.0f);
    }
    void SetSteering(Angle angle, float weight) override {
        this->mNISSteering = UMath::Clamp(angle, -this->mMaxSteering, this->mMaxSteering);
        this->mNISSteeringWeight = UMath::Clamp(weight, 0.0f, 1.0f);
    }

    virtual void NISCarTweaks(float dT);

    float AngleToAIAngle(float x) {
        return x / 32768.0f;
    }

    // Overrides: INISCarControl
    void SetAnimPitch(float dip, float time) override {
        this->mAnimatedPitchLength = time;
        this->mAnimatedPitchTime = 0.0f;
        this->mAnimatedPitch = 0.0f;

        float length;
        this->GetWheelBase(nullptr, &length);
        if (dip < 0.0f) {
            this->mAnimatedPitchDelta = -AngleToAIAngle(static_cast<float>(bATan(length * 0.5f, -dip)));
        } else {
            this->mAnimatedPitchDelta = AngleToAIAngle(static_cast<float>(bATan(length * 0.5f, dip)));
        }
    }

    float GetAnimPitch() const override {
        return this->mAnimatedPitch;
    }
    void SetAnimRoll(float dip, float time) override {
        this->mAnimatedRollLength = time;
        this->mAnimatedRollTime = 0.0f;
        this->mAnimatedRoll = 0.0f;

        float width;
        this->GetWheelBase(&width, nullptr);
        if (dip < 0.0f) {
            this->mAnimatedRollDelta = -AngleToAIAngle(static_cast<float>(bATan(width * 0.5f, -dip)));
        } else {
            this->mAnimatedRollDelta = AngleToAIAngle(static_cast<float>(bATan(width * 0.5f, dip)));
        }
    }

    float GetAnimRoll() const override {
        return mAnimatedRoll;
    }
    void SetAnimShake(float dip, float cycleRate, float cycleRamp, float time) override {
        this->mAnimatedShakeLength = time;
        this->mAnimatedShakeTime = 0.0f;
        this->mAnimatedShake = 0.0f;
        this->mAnimatedShakeCycleRate = cycleRate;
        this->mAnimatedShakeRamp = cycleRamp;

        float width;
        this->GetWheelBase(&width, nullptr);
        if (dip < 0.0f) {
            this->mAnimatedShakeDelta = -AngleToAIAngle(static_cast<float>(bATan(width * 0.5f, -dip)));
        } else {
            this->mAnimatedShakeDelta = AngleToAIAngle(static_cast<float>(bATan(width * 0.5f, dip)));
        }
    }

    float GetAnimShake() const override {
        return this->mAnimatedShake;
    }

    // Overrides: ISuspension
    void MatchSpeed(float speed) override;
    float GetWheelAngularVelocity(int index) const override {
        return this->mTires[index]->GetAngularVelocity();
    }
    float GetWheelRadius(unsigned int index) const override {
        return this->mTires[index]->GetRadius();
    }
    float GetWheelSlip(unsigned int idx) const override {
        return this->mTires[idx]->GetCurrentSlip();
    }
    Mps GetToleratedSlip(unsigned int idx) const override {
        return 0.5f;
    }
    float GetWheelSkid(unsigned int idx) const override {
        return this->mTires[idx]->GetLateralSpeed();
    }
    Newtons GetWheelLoad(unsigned int i) const override {
        return this->mTires[i]->GetLoad();
    }
    float GetWheelTraction(unsigned int index) const override {
        return this->mTires[index]->GetTraction();
    }
    void SetWheelAngularVelocity(int wheel, float w) override {}
    unsigned int GetNumWheels() const override {
        return 4;
    }
    const UMath::Vector3 &GetWheelPos(unsigned int i) const override {
        return this->mTires[i]->GetPosition();
    }
    const UMath::Vector3 &GetWheelLocalPos(unsigned int i) const override {
        return this->mTires[i]->GetLocalArm();
    }
    UMath::Vector3 GetWheelCenterPos(unsigned int i) const override;
    void ApplyVehicleEntryForces(bool enteringVehicle, const UMath::Vector3 &pos, bool calledfromEvent) override {}

    // Overrides: ISuspension
    const float GetWheelRoadHeight(unsigned int i) const override {
        return this->mTires[i]->GetNormal().w;
    }
    float GetCompression(unsigned int i) const override {
        return this->mTires[i]->GetCompression();
    }
    const UMath::Vector4 &GetWheelRoadNormal(unsigned int i) const override {
        return this->mTires[i]->GetNormal();
    }
    bool IsWheelOnGround(unsigned int i) const override {
        return this->mTires[i]->IsOnGround();
    }
    const SimSurface &GetWheelRoadSurface(unsigned int i) const override {
        return this->mTires[i]->GetSurface();
    }
    const UMath::Vector3 &GetWheelVelocity(unsigned int i) const override {
        return this->mTires[i]->GetVelocity();
    }
    int GetNumWheelsOnGround() const override {
        return this->mNumWheelsOnGround;
    }
    Angle GetWheelSteer(unsigned int wheel) const override {
        return wheel < 2 ? this->mSteering : 0.0f;
    }
    Angle GetMaxSteering() const override {
        return this->mMaxSteering;
    }
    Angle GetWheelSlipAngle(unsigned int idx) const override {
        return this->mTires[idx]->GetSlipAngle();
    }

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

SuspensionSpline::Tire::Tire(float radius)
    : Wheel(0),                          //
      mLocked(false),                    //
      mBurnout(0.0f),                    //
      mRadius(UMath::Max(radius, 0.1f)), //
      mAV(0.0f),                         //
      mSlip(0.0f),                       //
      mRoadSpeed(0.0f),                  //
      mSlipAngle(0.0f),                  //
      mLateralSpeed(0.0f),               //
      mLoad(0.0f) {}

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

SuspensionSpline::SuspensionSpline(const BehaviorParams &bp, const SuspensionParams &sp)
    : Chassis(bp),               //
      INISCarControl(bp.fowner), //
      mTireInfo(this, 0),        //
      mSuspensionInfo(this, 0),  //
      mDrivetrainInfo(this, 0),  //
      mRB(nullptr),              //
      mCollisionBody(nullptr),   //
      mInput(nullptr),           //
      mNumWheelsOnGround(0),     //
      mMaxSteering(0.125f),      //
      mSteering(0.0f),           //
      mNISSteering(0.0f),        //
      mNISSteeringWeight(0.0f),  //
      mTimeInAir(0.0f),          //
      mBurnout(0.0f),            //
      mBrakeLockFront(false),    //
      mBrakeLockRear(false),     //
      mConstraint(60.0f) {
    this->mNISPosition = UMath::Matrix4::kIdentity;

    this->mAnimatedPitchLength = 0.0f;
    this->mAnimatedPitch = 0.0f;
    this->mAnimatedPitchDelta = 0.0f;
    this->mAnimatedPitchTime = 0.0f;
    this->mAnimatedRollLength = 0.0f;
    this->mAnimatedRoll = 0.0f;
    this->mAnimatedRollDelta = 0.0f;
    this->mAnimatedRollTime = 0.0f;
    this->mAnimatedShakeLength = 0.0f;
    this->mAnimatedShakeCycleRate = 0.0f;
    this->mAnimatedShakeRamp = 0.0f;
    this->mAnimatedShake = 0.0f;
    this->mAnimatedShakeDelta = 0.0f;
    this->mAnimatedShakeTime = 0.0f;

    this->EnableProfile("SuspensionSpline");

    this->GetOwner()->QueryInterface(&this->mRB);
    this->GetOwner()->QueryInterface(&this->mCollisionBody);
    this->GetOwner()->QueryInterface(&this->mInput);
    this->GetOwner()->QueryInterface(&this->mIEngine);

    for (int i = 0; i < 4; ++i) {
        bool is_front = i < 2;
        float diameter = Physics::Info::WheelDiameter(this->mTireInfo, is_front);
        this->mTires[i] = new Tire(diameter * 0.5f);
    }

    UMath::Vector3 dimension;
    this->GetOwner()->GetRigidBody()->GetDimension(dimension);

    float wheelbase = this->mSuspensionInfo->WHEEL_BASE();
    float axle_width_f = this->mSuspensionInfo->TRACK_WIDTH().Front - this->mTireInfo->SECTION_WIDTH().Front * 0.001f;
    float axle_width_r = this->mSuspensionInfo->TRACK_WIDTH().Rear - this->mTireInfo->SECTION_WIDTH().Rear * 0.001f;
    float front_axle = this->mSuspensionInfo->FRONT_AXLE();

    UVector3 fl(-axle_width_f * 0.5f, -dimension.y, front_axle);
    UVector3 fr(axle_width_f * 0.5f, -dimension.y, front_axle);
    UVector3 rl(-axle_width_r * 0.5f, -dimension.y, front_axle - wheelbase);
    UVector3 rr(axle_width_r * 0.5f, -dimension.y, front_axle - wheelbase);

    this->GetWheel(0).SetLocalArm(fl);
    this->GetWheel(1).SetLocalArm(fr);
    this->GetWheel(2).SetLocalArm(rl);
    this->GetWheel(3).SetLocalArm(rr);
}

SuspensionSpline::~SuspensionSpline() {
    for (int i = 0; i < 4; ++i) {
        delete this->mTires[i];
    }
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

float gNIS_AeroDynamics = 0.0f;
static const float Tweak_AnimDampSpeed = 0.25f; // TODO use

void SuspensionSpline::OnTaskSimulate(float dT) {
    if ((this->mInput == nullptr) || (this->mCollisionBody == nullptr) || (this->mRB == nullptr)) {
        return;
    }

    this->SetCOG(0.0f, 0.0f);

    ISimable *owner = this->GetOwner();
    State state;
    this->ComputeState(dT, state);

    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->BeginFrame();

        if (i > 1) {
            this->mTires[i]->SetLocked(this->mBrakeLockRear);
        } else {
            this->mTires[i]->SetLocked(this->mBrakeLockFront);
        }

        if (i <= 1) {
            if (this->mDrivetrainInfo->TORQUE_SPLIT() != 0.0f) {
                this->mTires[i]->SetBurnout(this->mBurnout);
            }
        } else {
            if (this->mDrivetrainInfo->TORQUE_SPLIT() != 1.0f) {
                this->mTires[i]->SetBurnout(this->mBurnout);
            }
        }
    }

    if (this->mBurnout != 0.0f) {
        this->mInput->SetControlGas(1.0f);
    } else {
        this->mInput->SetControlGas(0.5f);
    }

    if (this->mBrakeLockFront && this->mBrakeLockRear) {
        this->mInput->SetControlBrake(1.0f);
        this->mInput->SetControlHandBrake(0.0f);
    } else if (this->mBrakeLockRear) {
        this->mInput->SetControlHandBrake(1.0f);
        this->mInput->SetControlBrake(0.0f);
    } else {
        this->mInput->SetControlBrake(0.0f);
        this->mInput->SetControlHandBrake(0.0f);
    }

    this->DoWheelForces(state);

    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->UpdateTime(dT);
    }

    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->EndFrame(dT);
    }

    if (this->GetNumWheelsOnGround() != 0) {
        this->mTimeInAir = 0.0f;
    } else {
        this->mTimeInAir += dT;
    }

    if (gNIS_AeroDynamics > 0.0f) {
        this->DoAerodynamics(state, 0.0f, gNIS_AeroDynamics, this->GetWheel(0).GetLocalArm().z, this->GetWheel(2).GetLocalArm().z, nullptr);
    }

    Chassis::OnTaskSimulate(dT);
}

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

void SuspensionSpline::NISCarTweaks(float dT) {
    if (this->mAnimatedPitchLength != 0.0f) {
        this->mAnimatedPitchTime += dT;

        if (this->mAnimatedPitchTime < this->mAnimatedPitchLength) {
            float halfLength = this->mAnimatedPitchLength * 0.5f;

            float timePos;
            if (this->mAnimatedPitchTime <= halfLength) {
                timePos = this->mAnimatedPitchTime * 0.5f;
            } else {
                timePos = halfLength - this->mAnimatedPitchTime * 0.5f;
            }

            float ratio = timePos / halfLength;
            this->mAnimatedPitch = ratio * this->mAnimatedPitchDelta;
        } else {
            this->mAnimatedPitch = 0.0f;
            this->mAnimatedPitchLength = 0.0f;
        }
    }

    if (this->mAnimatedRollLength != 0.0f) {
        this->mAnimatedRollTime += dT;

        if (this->mAnimatedRollTime < this->mAnimatedRollLength) {
            float halfLength = this->mAnimatedRollLength * 0.5f;

            float timePos;
            if (this->mAnimatedRollTime <= halfLength) {
                timePos = this->mAnimatedRollTime * 0.5f;
            } else {
                timePos = halfLength - this->mAnimatedRollTime * 0.5f;
            }

            float ratio = timePos / halfLength;
            this->mAnimatedRoll = ratio * this->mAnimatedRollDelta;
        } else {
            this->mAnimatedRoll = 0.0f;
            this->mAnimatedRollLength = 0.0f;
        }
    }

    if (this->mAnimatedShakeLength != 0.0f && this->mAnimatedShakeCycleRate != 0.0f) {
        this->mAnimatedShakeTime += dT;

        if (this->mAnimatedShakeTime < this->mAnimatedShakeLength) {
            float totalCycles = static_cast<float>(static_cast<int>(this->mAnimatedShakeLength / this->mAnimatedShakeCycleRate));
            float cycleCount = static_cast<float>(static_cast<int>(this->mAnimatedShakeTime / this->mAnimatedShakeCycleRate));
            float rampCycles = static_cast<float>(static_cast<int>(this->mAnimatedShakeRamp / this->mAnimatedShakeCycleRate + 0.99f));
            float cycleTime = this->mAnimatedShakeTime - this->mAnimatedShakeCycleRate * cycleCount;
            float ramp = 1.0f;

            if (rampCycles > 0.0f) {
                if (cycleCount < rampCycles) {
                    ramp = cycleCount / rampCycles;
                } else if (cycleCount > totalCycles - rampCycles) {
                    ramp = (totalCycles - cycleCount) / rampCycles;
                }
            }

            float ratio = cycleTime / this->mAnimatedShakeCycleRate;
            this->mAnimatedShake = this->mAnimatedShakeDelta * bCos(bDegToAng(ratio * 360.0f)) * ramp;
        } else {
            this->mAnimatedShake = 0.0f;
            this->mAnimatedShakeDelta = 0.0f;
            this->mAnimatedShakeRamp = 0.0f;
            this->mAnimatedShakeLength = 0.0f;
        }
    }
}

// UNSOLVED, looks like just regswaps + using a precalculated stack offset?
bool SuspensionSpline::SetNISPosition(const UMath::Matrix4 &position, bool initial, float dT) {
    bool success = true;

    this->NISCarTweaks(dT);

    if (initial) {
        success = this->GetVehicle()->SetVehicleOnGround(Vector4To3(position.v3), Vector4To3(position.v2));
        this->GetVehicle()->SetSpeed(0.0f);

        this->mNISPosition = position;

        this->mRB->SetLinearVelocity(UMath::Vector3::kZero);
        this->mRB->SetAngularVelocity(UMath::Vector3::kZero);
        this->RestoreState();
    } else {
        float car_angle = UMath::Atan2a(position.v2.z, -position.v2.x);
        float angular_velocity_y;
        UMath::Vector3 linear_velocity_xz;

        if (dT > UMath::Epsilon) {
            UMath::Sub(Vector4To3(position.v3), Vector4To3(this->mNISPosition.v3), linear_velocity_xz);
            UMath::Scale(linear_velocity_xz, 1.0f / dT);

            float lastRotation = UMath::Atan2r(this->mNISPosition.v2.z, -this->mNISPosition.v2.x);
            float newRotation = UMath::Atan2r(position.v2.z, -position.v2.x);
            float deltaRotation = newRotation - lastRotation;
            angular_velocity_y = deltaRotation / dT;
        } else {
            angular_velocity_y = 0.0f;
            UMath::Clear(linear_velocity_xz);
        }

        this->mNISPosition = position;

        UMath::Vector3 pos = this->mRB->GetPosition();
        pos.x = this->mNISPosition.v3.x;
        pos.z = this->mNISPosition.v3.z;
        this->mRB->SetPosition(pos);

        UMath::Vector3 vel = this->mRB->GetLinearVelocity();
        vel.x = linear_velocity_xz.x;
        vel.z = linear_velocity_xz.z;
        this->mRB->SetLinearVelocity(vel);

        UMath::Vector3 avel = this->mRB->GetAngularVelocity();
        avel.y = angular_velocity_y;
        this->mRB->SetAngularVelocity(avel);

        UMath::Matrix4 matrix;
        this->mRB->GetMatrix4(matrix);

        UMath::Vector4 cross;
        UMath::Crossxyz(matrix.v1, this->mNISPosition.v1, cross);

        float pitch = UMath::ASina(UMath::Bound(cross.x, 1.0f));
        float roll = UMath::ASina(UMath::Bound(cross.z, 1.0f));
        float max_angle = DEG2ANGLE(this->mConstraint * 0.5f);
        float newroll = UMath::Clamp(roll, -max_angle, max_angle);
        float newpitch = UMath::Clamp(pitch, -max_angle, max_angle);

        if (newroll != roll) {
            UMath::MultZRot(matrix, roll - newroll, matrix);
        }

        if (newpitch != pitch) {
            UMath::MultXRot(matrix, pitch - newpitch, matrix);
        }

        UMath::Matrix4 invorient;
        UMath::Matrix4 localmatrix;
        invorient = matrix;
        OrthoInverse(invorient);
        UMath::Mult(invorient, this->mNISPosition, localmatrix);

        float newdelta = UMath::Atan2a(localmatrix.v2.x, localmatrix.v2.z);
        UMath::SetYRot(localmatrix, newdelta);
        UMath::Mult(localmatrix, matrix, matrix);
        this->mRB->SetOrientation(matrix);
    }

    return success;
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

void SuspensionSpline::DoWheelForces(State &state) {
    const float dT = state.time;
    UVector3 steerR;
    UVector3 steerL;
    this->DoSteering(state, steerR, steerL);

    unsigned int wheelsOnGround = 0;
    float maxDelta = 0.0f;
    const UMath::Vector3 &vFwd = state.GetForwardVector();
    const UMath::Vector3 &vUp = state.GetUpVector();

    const float mass = state.mass;

    UMath::Vector3 cog;
    UMath::Rotate(state.cog, state.matrix, cog);

    float time = Sim::GetTime();

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
        shock_specs[i] = LBIN2NM(this->mSuspensionInfo.SHOCK_STIFFNESS().At(i));
        shock_ext_specs[i] = LBIN2NM(this->mSuspensionInfo.SHOCK_EXT_STIFFNESS().At(i));
        shock_valving[i] = INCH2METERS(this->mSuspensionInfo.SHOCK_VALVING().At(i));
        shock_digression[i] = 1.0f - this->mSuspensionInfo.SHOCK_DIGRESSION().At(i);
        spring_specs[i] = LBIN2NM(this->mSuspensionInfo.SPRING_STIFFNESS().At(i));
        sway_specs[i] = LBIN2NM(this->mSuspensionInfo.SWAYBAR_STIFFNESS().At(i));
        travel_specs[i] = INCH2METERS(this->mSuspensionInfo.TRAVEL().At(i));
        rideheight_specs[i] = INCH2METERS(this->mSuspensionInfo.RIDE_HEIGHT().At(i));
        progression[i] = this->mSuspensionInfo.SPRING_PROGRESSION().At(i);
    }

    float sway_stiffness[4];
    sway_stiffness[0] = (this->mTires[0]->GetCompression() - this->mTires[1]->GetCompression()) * sway_specs[0];
    sway_stiffness[1] = -sway_stiffness[0];
    sway_stiffness[2] = (this->mTires[2]->GetCompression() - this->mTires[3]->GetCompression()) * sway_specs[1];
    sway_stiffness[3] = -sway_stiffness[2];

    UMath::Vector4 steering_normals[4];
    steering_normals[0] = UMath::Vector4Make(steerL, 1.0f);
    steering_normals[1] = UMath::Vector4Make(steerR, 1.0f);
    steering_normals[2] = UMath::Vector4Make(vFwd, 1.0f);
    steering_normals[3] = UMath::Vector4Make(vFwd, 1.0f);

    UMath::Vector3 cg;
    UMath::RotateTranslate(state.cog, state.matrix, cg);

    UMath::Vector3 total_force = UMath::Vector3::kZero;
    UMath::Vector3 total_torque = UMath::Vector3::kZero;
    bool resolve = false;

    for (unsigned int i = 0; i < 4; ++i) {
        int axle = i >> 1;
        Tire &wheel = this->GetWheel(i);
        UMath::Vector3 wp = wheel.GetWorldArm();

        wheel.UpdatePosition(state.angular_vel, state.linear_vel, state.matrix, cog, state.time, wheel.GetRadius(), true, state.collider,
                             state.dimension.y * 2.0f);

        const UVector3 groundNormal(wheel.GetNormal());
        const UVector3 forwardNormal(steering_normals[i]);
        UVector3 lateralNormal;
        UMath::UnitCross(groundNormal, forwardNormal, lateralNormal);

        float penetration = wheel.GetNormal().w;
        float upness = UMath::Clamp(UMath::Dot(groundNormal, vUp), 0.0f, 1.0f);
        const float oldCompression = wheel.GetCompression();
        float newCompression = rideheight_specs[axle] * upness + penetration;
        float max_compression = travel_specs[axle];

        if (wheel.GetCompression() == 0.0f) {
            maxDelta = UMath::Max(maxDelta, newCompression - max_compression);
        }

        newCompression = UMath::Max(newCompression, 0.0f);
        if (newCompression > max_compression) {
            float delta = newCompression - max_compression;
            maxDelta = UMath::Max(maxDelta, delta);
            newCompression = max_compression;
        }

        if (newCompression > 0.0f && upness > VehicleSystem::ENABLE_ROLL_STOPS_THRESHOLD) {
            ++wheelsOnGround;

            float springForce;
            const float diff = newCompression - wheel.GetCompression();
            float rise = diff / dT;

            if (UMath::Epsilon < shock_valving[axle] && shock_digression[axle] < 1.0f) {
                float abs_rise = UMath::Abs(rise);
                float valving = shock_valving[axle];

                if (abs_rise > valving) {
                    float digression = valving * UMath::Pow(abs_rise / valving, shock_digression[axle]);
                    rise = rise > 0.0f ? digression : -digression;
                }
            }

            float spring = newCompression * spring_specs[axle] * (newCompression * progression[axle] + 1.0f);
            float damp = rise > 0.0f ? rise * shock_specs[axle] : rise * shock_ext_specs[axle];

            if (damp > this->mSuspensionInfo.SHOCK_BLOWOUT() * 9.81f * mass) {
                damp = 0.0f;
            }

            springForce = UMath::Max(damp + spring + sway_stiffness[i], 0.0f);

            UVector3 verticalForce = UVector3(vUp) * springForce;

            UVector3 c;
            UMath::Cross(forwardNormal, groundNormal, c);
            UMath::Cross(c, forwardNormal, c);

            float d2 = UMath::Max(UMath::Dot(c, groundNormal) * 4.0f - 3.0f, 0.3f);
            float load = d2 * springForce;

            const UMath::Vector3 &pointVelocity = wheel.GetVelocity();
            float xspeed = UMath::Dot(pointVelocity, lateralNormal);
            float zspeed = UMath::Dot(pointVelocity, forwardNormal);

            UMath::Vector3 local_acc;
            local_acc.x = (xspeed - wheel.GetLateralSpeed()) / state.time;
            local_acc.z = (zspeed - wheel.GetRoadSpeed()) / state.time;
            local_acc.y = 0.0f;

            wheel.UpdateLoaded(xspeed, zspeed, load, state.time);

            UMath::Vector3 local_force;
            UMath::Scale(local_acc, state.mass * 0.25f, local_force);

            UMath::Vector3 lateralForce;
            UMath::Scale(lateralNormal, -local_force.x, lateralForce);
            UMath::ScaleAdd(forwardNormal, local_force.z, lateralForce, lateralForce);

            float lforce = UMath::Length(local_force);
            float sgrip = this->mTireInfo->STATIC_GRIP().At(axle) * load;

            if (lforce > sgrip) {
                float dgrip = this->mTireInfo->DYNAMIC_GRIP().At(axle) * load;
                UMath::Scale(lateralForce, dgrip / lforce, lateralForce);
            }

            UMath::Vector3 force;
            UMath::Add(lateralForce, verticalForce, force);

            UVector3 p = wheel.GetLocalArm();
            p.y += wheel.GetCompression();
            p.y -= rideheight_specs[axle];
            UMath::RotateTranslate(p, state.matrix, p);
            wheel.SetPosition(p);

            UMath::Vector3 torque;
            UMath::Vector3 r;
            UMath::Sub(p, cg, r);
            UMath::Cross(r, force, torque);
            UMath::Add(torque, total_torque, total_torque);
            UMath::Add(total_force, verticalForce, total_force);

            resolve = true;
        } else {
            wheel.SetForce(UMath::Vector3::kZero);
            wheel.UpdateFree(dT);
        }

        if (newCompression == 0.0f) {
            wheel.IncAirTime(dT);
        } else {
            wheel.SetAirTime(0.0f);
        }

        wheel.SetCompression(newCompression);
    }

    if (resolve) {
        this->mRB->ConvertWorldToLocal(total_torque, false);
        this->mRB->ConvertWorldToLocal(total_force, false);

        total_torque.y = 0.0f;
        total_force.x = 0.0f;
        total_force.z = 0.0f;

        this->mRB->ConvertLocalToWorld(total_torque, false);
        this->mRB->ConvertLocalToWorld(total_force, false);

        this->mRB->Resolve(total_force, total_torque);
    }

    if (0.0f < maxDelta) {
        for (unsigned int i = 0; i < this->GetNumWheels(); ++i) {
            Wheel &wheel = this->GetWheel(i);
            wheel.SetY(wheel.GetPosition().y + maxDelta);
        }

        this->mRB->ModifyYPos(maxDelta);
    }

    this->mNumWheelsOnGround = wheelsOnGround;
}
