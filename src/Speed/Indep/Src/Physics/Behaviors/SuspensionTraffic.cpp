#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/Wheel.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// total size: 0x110
class SuspensionTraffic : public Chassis {
  public:
    // total size: 0x10C
    class Tire : public Wheel {
      public:
        enum LastRotationSign {
            WAS_NEGATIVE = 2,
            WAS_ZERO = 1,
            WAS_POSITIVE = 0,
        };

        Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes);
        void BeginFrame();
        void EndFrame(float dT);
        void UpdateFree(float dT);

        void Stop() {
            mAV = 0.0f;
            mSlip = 0.0f;
            mRoadSpeed = 0.0f;
            mSlipAngle = 0.0f;
        }

        float GetRadius() const {
            return mRadius;
        }

        float GetAngularVelocity() const {
            return mAV;
        }

        void SetAngularVelocity(float av) {
            mAV = av;
        }

        void ApplyTorque(float torque) {
            mAppliedTorque += torque;
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
        float mSlip;                        // offset 0xE4, size 0x4
        float mLastTorque;                  // offset 0xE8, size 0x4
        const int mWheelIndex;              // offset 0xEC, size 0x4
        float mRoadSpeed;                   // offset 0xF0, size 0x4
        const Attrib::Gen::tires *mSpecs;   // offset 0xF4, size 0x4
        const Attrib::Gen::brakes *mBrakes; // offset 0xF8, size 0x4
        float mSlipAngle;                   // offset 0xFC, size 0x4
        const int mAxleIndex;               // offset 0x100, size 0x4
        bool mSlipping;                     // offset 0x104, size 0x1
        float mLateralSpeed;                // offset 0x108, size 0x4
    };

    static Behavior *Construct(const BehaviorParams &params);

    SuspensionTraffic(const struct BehaviorParams &bp, const SuspensionParams &sp);
    void DoSimpleAero(State &state);
    void DoDriveForces(State &state);
    void DoWheelForces(State &state);
    float DoHP2Steering(State &state);
    void DoSteering(State &state, UMath::Vector3 &right, UMath::Vector3 &left);

    // Overrides
    // IUnknown
    ~SuspensionTraffic() override;

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void OnTaskSimulate(float dT) override;
    void Reset() override;

    // ISuspension
    UMath::Vector3 GetWheelCenterPos(unsigned int i) const override;
    Radians GetWheelAngularVelocity(int index) const override;
    float GetWheelRadius(unsigned int index) const override;
    void MatchSpeed(float speed) override;

    Tire &GetWheel(unsigned int i) {
        return *mTires[i];
    }

  private:
    BehaviorSpecsPtr<Attrib::Gen::tires> mTireInfo;              // offset 0x94, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::brakes> mBrakeInfo;            // offset 0xA8, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::chassis> mSuspensionInfo;      // offset 0xBC, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::transmission> mDrivetrainInfo; // offset 0xD0, size 0x14
    IRigidBody *mRB;                                             // offset 0xE4, size 0x4
    ICollisionBody *mRBComplex;                                  // offset 0xE8, size 0x4
    IInput *mInput;                                              // offset 0xEC, size 0x4
    ITransmission *mTransmission;                                // offset 0xF0, size 0x4
    float mLastSteer;                                            // offset 0xF4, size 0x4
    unsigned int mNumWheelsOnGround;                             // offset 0xF8, size 0x4
    float mMaxSteering;                                          // offset 0xFC, size 0x4
    Tire *mTires[4];                                             // offset 0x100, size 0x10
};

SuspensionTraffic::Tire::Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes)
    : Wheel(0),                          //
      mRadius(UMath::Max(radius, 0.1f)), //
      mWheelIndex(index),                //
      mAxleIndex(index >> 1),            //
      mSpecs(specs),                     //
      mBrakes(brakes),                   //
      mSlipping(false),                  //
      mLateralSpeed(0.0f),               //
      mBrake(0.0f),                      //
      mEBrake(0.0f),                     //
      mAV(0.0f),                         //
      mLoad(0.0f),                       //
      mLateralForce(0.0f),               //
      mLongitudeForce(0.0f),             //
      mAppliedTorque(0.0f),              //
      mSlip(0.0f),                       //
      mLastTorque(0.0f),                 //
      mRoadSpeed(0.0f),                  //
      mSlipAngle(0.0f) {}

void SuspensionTraffic::Tire::BeginFrame() {
    mAppliedTorque = 0.0f;
    SetForce(UMath::Vector3::kZero);
    mLateralForce = 0.0f;
    mLongitudeForce = 0.0f;
}

void SuspensionTraffic::Tire::EndFrame(float dT) {}

void SuspensionTraffic::Tire::UpdateFree(float dT) {
    mSlipping = 0;
    mLoad = 0.0f;
    mSlip = 0.0f;
    mSlipAngle = 0.0f;
    if ((mEBrake > 0.0f) || (mBrake > 0.0f)) {
        mAV = 0.0f;
    }
    mLateralForce = 0.0f;
    mLongitudeForce = 0.0f;
}

Behavior *SuspensionTraffic::Construct(const BehaviorParams &params) {
    // "BASE"
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(0xa6b47fac)));
    return new SuspensionTraffic(params, sp);
}

SuspensionTraffic::SuspensionTraffic(const BehaviorParams &bp, const SuspensionParams &sp)
    : Chassis(bp),              //
      mTireInfo(this, 0),       //
      mBrakeInfo(this, 0),      //
      mSuspensionInfo(this, 0), //
      mDrivetrainInfo(this, 0), //
      mRB(0),                   //
      mRBComplex(0),            //
      mInput(0),                //
      mTransmission(0),         //
      mLastSteer(0.0f),         //
      mNumWheelsOnGround(0),    //
      mMaxSteering(45.0f) {
    for (int i = 0; i < 4; ++i) {
        mTires[i] = 0;
    }

    GetOwner()->QueryInterface(&mRB);
    GetOwner()->QueryInterface(&mRBComplex);
    GetOwner()->QueryInterface(&mInput);
    GetOwner()->QueryInterface(&mTransmission);

    for (int i = 0; i < 4; ++i) {
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

SuspensionTraffic::~SuspensionTraffic() {
    for (int i = 0; i < 4; ++i) {
        delete mTires[i];
    }
}

void SuspensionTraffic::OnBehaviorChange(const UCrc32 &mechanic) {
    Chassis::OnBehaviorChange(mechanic);

    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mInput);
    } else if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mRBComplex);
        GetOwner()->QueryInterface(&mRB);
    } else if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        GetOwner()->QueryInterface(&mTransmission);
    }
}

void SuspensionTraffic::OnTaskSimulate(float dT) {
    if (!mInput || !mRBComplex || !mRB) {
        return;
    }
    SetCOG(0.0f, 0.0f);

    ISimable *owner = GetOwner();
    State state;
    ComputeState(dT, state);
    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->BeginFrame();
    }

    DoSimpleAero(state);
    DoDriveForces(state);
    DoWheelForces(state);

    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->UpdateTime(dT);
    }
    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->EndFrame(dT);
    }
    if (DoSleep(state) == SS_ALL) {
        for (unsigned int i = 0; i < 4; ++i) {
            mTires[i]->Stop();
        }
    }
    Chassis::OnTaskSimulate(dT);
}

UMath::Vector3 SuspensionTraffic::GetWheelCenterPos(unsigned int i) const {
    UMath::Vector3 pos = mTires[i]->GetPosition();
    if (!mRBComplex) {
        return pos;
    }
    UMath::ScaleAdd(mRBComplex->GetUpVector(), GetWheelRadius(i), pos, pos);
    return pos;
}

Radians SuspensionTraffic::GetWheelAngularVelocity(int index) const {
    return mTires[index]->GetAngularVelocity();
}

float SuspensionTraffic::GetWheelRadius(unsigned int index) const {
    return mTires[index]->GetRadius();
}

void SuspensionTraffic::MatchSpeed(float speed) {
    for (int i = 0; i < 4; ++i) {
        float w = mTires[i]->GetRadius();
        mTires[i]->SetAngularVelocity(speed / w);
    }
}

void SuspensionTraffic::Reset() {
    ISimable *owner = GetOwner();
    IRigidBody *rigidBody = owner->GetRigidBody();
    unsigned int numonground = 0;

    for (int i = 0; i < GetNumWheels(); ++i) {
        Tire &wheel = GetWheel(i);
        if (wheel.InitPosition(*rigidBody, wheel.GetRadius())) {
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
}

void SuspensionTraffic::DoSimpleAero(State &state) {
    const float dragcoef_spec = mSuspensionInfo->DRAG_COEFFICIENT();
    float speed = state.speed;
    float drag = speed * dragcoef_spec;
    UVector3 drag_vector(state.linear_vel);

    drag_vector *= -drag;
    mRB->ResolveForce(drag_vector);
}

float SuspensionTraffic::DoHP2Steering(State &state) {
    float steer_input = state.steer_input;
    float newsteer = steer_input * mMaxSteering;

    mLastSteer = newsteer;
    return newsteer * DEG2ANGLE(1.0f);
}

void SuspensionTraffic::DoSteering(State &state, UMath::Vector3 &right, UMath::Vector3 &left) {
    float truesteer = DoHP2Steering(state);
    float steer1 = ANGLE2RAD(1.0f);
    float ca = cosf(truesteer * steer1);
    float sa = sinf(truesteer * steer1);

    right.x = sa;
    right.y = 0.0f;
    right.z = ca;
    UMath::Rotate(right, state.matrix, right);
    left = right;
}

void SuspensionTraffic::DoDriveForces(State &state) {
    if (!mTransmission) {
        return;
    }
    float drive_torque = mTransmission->GetDriveTorque();
    if (drive_torque == 0.0f) {
        return;
    }
    float torquesplit = mDrivetrainInfo->TORQUE_SPLIT();
    for (unsigned int tire = 0; tire < 4; ++tire) {
        if (mTires[tire]->IsOnGround()) {
            float torque = drive_torque;
            torque = tire < 2 ? torque * (1.0f - torquesplit) : (torque * torquesplit);

            if (UMath::Abs(torque) >= 0.0f) {
                mTires[tire]->ApplyTorque(torque * 0.5f);
            }
        }
    }
}
