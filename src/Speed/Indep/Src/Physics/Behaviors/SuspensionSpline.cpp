
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
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/Wheel.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <cmath>

extern float gNIS_AeroDynamics;

// total size: 0x198
class SuspensionSpline : public Chassis, public INISCarControl {
  public:
    // total size: 0x124
    class Tire : public Wheel {
      public:
        Tire(float radius);
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

        float GetAngularVelocity() const {
            return mAV;
        }

        float GetCurrentSlip() const {
            return mSlip;
        }

        float GetLateralSpeed() const {
            return mLateralSpeed;
        }

        float GetLoad() const {
            return mLoad;
        }

        float GetSlipAngle() const {
            return mSlipAngle;
        }

        float GetTraction() const {
            return mLocked ? 0.0f : 1.0f;
        }

        void SetAngularVelocity(float av) {
            mAV = av;
        }

        void SetLocked(bool locked) {
            mLocked = locked;
        }

        void SetBurnout(float speed) {
            mBurnout = speed;
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

    static Behavior *Construct(const BehaviorParams &params);

    SuspensionSpline(const struct BehaviorParams &bp, const SuspensionParams &sp);
    void DoSteering(State &state, UMath::Vector3 &right, UMath::Vector3 &left);
    void GetWheelBase(float *width, float *length);
    void DoWheelForces(State &state);

    // Overrides
    // IUnknown
    ~SuspensionSpline() override;

    // ISuspension
    float GetWheelTraction(unsigned int index) const override;
    unsigned int GetNumWheels() const override;
    const UMath::Vector3 &GetWheelPos(unsigned int i) const override;
    const UMath::Vector3 &GetWheelLocalPos(unsigned int i) const override;
    void MatchSpeed(float speed) override;
    UMath::Vector3 GetWheelCenterPos(unsigned int i) const override;
    float GetWheelLoad(unsigned int i) const override;
    const float GetWheelRoadHeight(unsigned int i) const override;
    float GetCompression(unsigned int i) const override;
    float GetWheelSlip(unsigned int idx) const override;
    float GetToleratedSlip(unsigned int idx) const override;
    float GetWheelSkid(unsigned int idx) const override;
    float GetWheelSlipAngle(unsigned int idx) const override;
    const UMath::Vector4 &GetWheelRoadNormal(unsigned int i) const override;
    const SimSurface &GetWheelRoadSurface(unsigned int i) const override;
    const UMath::Vector3 &GetWheelVelocity(unsigned int i) const override;
    int GetNumWheelsOnGround() const override;
    Radians GetWheelAngularVelocity(int index) const override;
    void SetWheelAngularVelocity(int wheel, float w) override;
    float GetWheelSteer(unsigned int wheel) const override;
    float GetWheelRadius(unsigned int index) const override;
    float GetMaxSteering() const override;

    // Behavior
    void OnTaskSimulate(float dT) override;
    void Reset() override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // INISCarControl
    float GetBurnout() const override;
    void SetBurnout(float speed) override;
    void SetBrakeLock(bool front, bool rear) override;
    void SetConstraintAngle(float angle) override;
    void SetSteering(float angle, float weight) override;
    bool SetNISPosition(const UMath::Matrix4 &position, bool initial, float dT) override;
    void RestoreState() override;
    void SetAnimPitch(float dip, float time) override;
    void SetAnimRoll(float dip, float time) override;
    void SetAnimShake(float dip, float cycleRate, float cycleRamp, float time) override;
    float GetAnimPitch() const override;
    float GetAnimRoll() const override;
    float GetAnimShake() const override;

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

SuspensionSpline::SuspensionSpline(const BehaviorParams &bp, const SuspensionParams &sp)
    : Chassis(bp),              //
      INISCarControl(this),     //
      mTireInfo(this, 0),       //
      mSuspensionInfo(this, 0), //
      mDrivetrainInfo(this, 0), //
      mRB(0),                   //
      mCollisionBody(0),        //
      mInput(0),                //
      mIEngine(0),              //
      mNumWheelsOnGround(0),    //
      mMaxSteering(0.125f),     //
      mSteering(0.0f),          //
      mNISSteering(0.0f),       //
      mNISSteeringWeight(0.0f), //
      mTimeInAir(0.0f),         //
      mBurnout(0.0f),           //
      mBrakeLockFront(false),   //
      mBrakeLockRear(false),    //
      mConstraint(60.0f),       //
      mAnimatedPitchLength(0.0f), //
      mAnimatedPitch(0.0f),       //
      mAnimatedPitchDelta(0.0f),  //
      mAnimatedPitchTime(0.0f),   //
      mAnimatedRollLength(0.0f),  //
      mAnimatedRoll(0.0f),        //
      mAnimatedRollDelta(0.0f),   //
      mAnimatedRollTime(0.0f),    //
      mAnimatedShakeLength(0.0f), //
      mAnimatedShakeCycleRate(0.0f), //
      mAnimatedShakeRamp(0.0f),      //
      mAnimatedShake(0.0f),          //
      mAnimatedShakeDelta(0.0f),     //
      mAnimatedShakeTime(0.0f) {
    mNISPosition = UMath::Matrix4::kIdentity;

    GetOwner()->QueryInterface(&mRB);
    GetOwner()->QueryInterface(&mCollisionBody);
    GetOwner()->QueryInterface(&mInput);
    GetOwner()->QueryInterface(&mIEngine);

    for (int i = 0; i < 4; ++i) {
        float diameter = Physics::Info::WheelDiameter(mTireInfo, i < 2);
        mTires[i] = new Tire(diameter * 0.5f);
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

SuspensionSpline::~SuspensionSpline() {
    for (int i = 0; i < 4; ++i) {
        delete mTires[i];
    }
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

void SuspensionSpline::OnTaskSimulate(float dT) {
    if (!mInput || !mCollisionBody || !mRB) {
        return;
    }

    SetCOG(0.0f, 0.0f);

    State state;
    ComputeState(dT, state);

    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->BeginFrame();
        if (i > 1) {
            mTires[i]->SetLocked(mBrakeLockRear);
            if (mDrivetrainInfo->TORQUE_SPLIT() != 1.0f) {
                mTires[i]->SetBurnout(mBurnout);
            }
        } else {
            mTires[i]->SetLocked(mBrakeLockFront);
            if (mDrivetrainInfo->TORQUE_SPLIT() != 0.0f) {
                mTires[i]->SetBurnout(mBurnout);
            }
        }
    }

    if (mBurnout != 0.0f) {
        mInput->SetControlGas(1.0f);
    } else {
        mInput->SetControlGas(0.5f);
    }

    if (mBrakeLockFront) {
        if (mBrakeLockRear) {
            mInput->SetControlBrake(1.0f);
            mInput->SetControlHandBrake(0.0f);
        } else {
            mInput->SetControlBrake(0.0f);
            mInput->SetControlHandBrake(0.0f);
        }
    } else {
        if (mBrakeLockRear) {
            mInput->SetControlHandBrake(1.0f);
            mInput->SetControlBrake(0.0f);
        } else {
            mInput->SetControlBrake(0.0f);
            mInput->SetControlHandBrake(0.0f);
        }
    }

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

    if (gNIS_AeroDynamics > 0.0f) {
        DoAerodynamics(state, 0.0f, gNIS_AeroDynamics, GetWheel(0).GetLocalArm().z, GetWheel(2).GetLocalArm().z, 0);
    }

    Chassis::OnTaskSimulate(dT);
}

void SuspensionSpline::RestoreState() {
    mInput->ClearInput();

    mBurnout = 0.0f;
    mBrakeLockFront = false;
    mBrakeLockRear = false;
    mConstraint = 60.0f;
    mNISSteering = 0.0f;
    mNISSteeringWeight = 0.0f;

    INISCarEngine *iengine;
    if (GetOwner()->QueryInterface(&iengine)) {
        iengine->RestoreState();
    }
}

float SuspensionSpline::GetBurnout() const {
    return mBurnout;
}

void SuspensionSpline::SetBurnout(float speed) {
    mBurnout = speed;
}

void SuspensionSpline::SetBrakeLock(bool front, bool rear) {
    mBrakeLockFront = front;
    mBrakeLockRear = rear;
}

void SuspensionSpline::SetConstraintAngle(float angle) {
    mConstraint = UMath::Clamp(angle, 0.0f, 80.0f);
}

void SuspensionSpline::SetSteering(float angle, float weight) {
    mNISSteering = UMath::Clamp(angle, -mMaxSteering, mMaxSteering);
    mNISSteeringWeight = UMath::Clamp(weight, 0.0f, 1.0f);
}

void SuspensionSpline::SetAnimPitch(float dip, float time) {
    mAnimatedPitchLength = time;
    mAnimatedPitchTime = 0.0f;
    mAnimatedPitch = 0.0f;

    float length;
    GetWheelBase(0, &length);
    if (dip < 0.0f) {
        mAnimatedPitchDelta = -static_cast<float>(bATan(length * 0.5f, -dip)) * 3.0517578e-05f;
    } else {
        mAnimatedPitchDelta = static_cast<float>(bATan(length * 0.5f, dip)) * 3.0517578e-05f;
    }
}

float SuspensionSpline::GetAnimPitch() const {
    return mAnimatedPitch;
}

void SuspensionSpline::SetAnimRoll(float dip, float time) {
    mAnimatedRollLength = time;
    mAnimatedRollTime = 0.0f;
    mAnimatedRoll = 0.0f;

    float width;
    GetWheelBase(&width, 0);
    if (dip < 0.0f) {
        mAnimatedRollDelta = -static_cast<float>(bATan(width * 0.5f, -dip)) * 3.0517578e-05f;
    } else {
        mAnimatedRollDelta = static_cast<float>(bATan(width * 0.5f, dip)) * 3.0517578e-05f;
    }
}

float SuspensionSpline::GetAnimRoll() const {
    return mAnimatedRoll;
}

void SuspensionSpline::SetAnimShake(float dip, float cycleRate, float cycleRamp, float time) {
    mAnimatedShakeLength = time;
    mAnimatedShakeTime = 0.0f;
    mAnimatedShake = 0.0f;
    mAnimatedShakeCycleRate = cycleRate;
    mAnimatedShakeRamp = cycleRamp;

    float width;
    GetWheelBase(&width, 0);
    if (dip < 0.0f) {
        mAnimatedShakeDelta = -static_cast<float>(bATan(width * 0.5f, -dip)) * 3.0517578e-05f;
    } else {
        mAnimatedShakeDelta = static_cast<float>(bATan(width * 0.5f, dip)) * 3.0517578e-05f;
    }
}

float SuspensionSpline::GetAnimShake() const {
    return mAnimatedShake;
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

float SuspensionSpline::GetWheelTraction(unsigned int index) const {
    return mTires[index]->GetTraction();
}

unsigned int SuspensionSpline::GetNumWheels() const {
    return 4;
}

const UMath::Vector3 &SuspensionSpline::GetWheelPos(unsigned int i) const {
    return mTires[i]->GetPosition();
}

const UMath::Vector3 &SuspensionSpline::GetWheelLocalPos(unsigned int i) const {
    return mTires[i]->GetLocalArm();
}

float SuspensionSpline::GetWheelLoad(unsigned int i) const {
    return mTires[i]->GetLoad();
}

const float SuspensionSpline::GetWheelRoadHeight(unsigned int i) const {
    return mTires[i]->GetNormal().w;
}

float SuspensionSpline::GetCompression(unsigned int i) const {
    return mTires[i]->GetCompression();
}

void SuspensionSpline::MatchSpeed(float speed) {
    for (int i = 0; i < 4; ++i) {
        mTires[i]->MatchSpeed(speed);
    }
}

float SuspensionSpline::GetWheelSlip(unsigned int idx) const {
    return mTires[idx]->GetCurrentSlip();
}

float SuspensionSpline::GetToleratedSlip(unsigned int idx) const {
    return 0.0f;
}

float SuspensionSpline::GetWheelSkid(unsigned int idx) const {
    return mTires[idx]->GetLateralSpeed();
}

float SuspensionSpline::GetWheelSlipAngle(unsigned int idx) const {
    return mTires[idx]->GetSlipAngle();
}

const UMath::Vector4 &SuspensionSpline::GetWheelRoadNormal(unsigned int i) const {
    return mTires[i]->GetNormal();
}

const SimSurface &SuspensionSpline::GetWheelRoadSurface(unsigned int i) const {
    return mTires[i]->GetSurface();
}

const UMath::Vector3 &SuspensionSpline::GetWheelVelocity(unsigned int i) const {
    return mTires[i]->GetVelocity();
}

int SuspensionSpline::GetNumWheelsOnGround() const {
    return mNumWheelsOnGround;
}

Radians SuspensionSpline::GetWheelAngularVelocity(int index) const {
    return mTires[index]->GetAngularVelocity();
}

void SuspensionSpline::SetWheelAngularVelocity(int wheel, float w) {
    mTires[wheel]->SetAngularVelocity(w);
}

float SuspensionSpline::GetWheelSteer(unsigned int wheel) const {
    return wheel < 2 ? mSteering : 0.0f;
}

float SuspensionSpline::GetWheelRadius(unsigned int index) const {
    return mTires[index]->GetRadius();
}

float SuspensionSpline::GetMaxSteering() const {
    return mMaxSteering;
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
            float newCompression = wheel.GetNormal().w + GetRideHeight(i) * upness;
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
