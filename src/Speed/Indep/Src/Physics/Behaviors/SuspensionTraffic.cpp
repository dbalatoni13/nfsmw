#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Wheel.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

#define EPSILON 0.000001f

#define MAKEATTRIB const

// total size: 0x110
class SuspensionTraffic : public Chassis {
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
            return this->mRadius;
        }
        float GetAngularVelocity() const {
            return this->mAV;
        }
        void SetAngularVelocity(float av) {
            this->mAV = av;
        }

        void Stop() {
            this->mAV = 0.0f;
            this->mSlip = 0.0f;
            this->mRoadSpeed = 0.0f;
            this->mSlipAngle = 0.0f;
        }

        float GetCurrentSlip() const {
            return this->mSlip;
        }
        float GetTraction() const {}

        void BeginFrame();
        void EndFrame(float dT);

        float GetLoad() const {
            return mLoad;
        }
        void ApplyTorque(float torque) {
            this->mAppliedTorque += torque;
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
        float mSlipAngle;                   // offset 0xFC, size 0x4
        const int mAxleIndex;               // offset 0x100, size 0x4
        bool mSlipping;                     // offset 0x104, size 0x1
        float mLateralSpeed;                // offset 0x108, size 0x4
    };

    SuspensionTraffic(const BehaviorParams &bp, const SuspensionParams &sp);
    ~SuspensionTraffic() override;
    static Behavior *Construct(const BehaviorParams &params);

    // ISuspension
    void MatchSpeed(float speed) override;
    float GetWheelAngularVelocity(int index) const override {}
    float GetWheelRadius(unsigned int index) const override {}
    float GetWheelSlip(unsigned int idx) const override {}
    Mps GetToleratedSlip(unsigned int idx) const override {}
    float GetWheelSkid(unsigned int idx) const override {}
    Newtons GetWheelLoad(unsigned int i) const override {}
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
    void DoDriveForces(State &state);
    void DoSteering(State &state, UMath::Vector3 &right, UMath::Vector3 &left);
    float DoHP2Steering(State &state);

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    Tire &GetWheel(unsigned int i) {
        return *this->mTires[i];
    }

    const Tire &GetWheel(unsigned int i) const {
        return *this->mTires[i];
    }

  private:
#ifdef EA_BUILD_A124
    void CreateTires();
#endif

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

BIND_BEHAVIOR_FACTORY(SuspensionTraffic);

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
    this->mAppliedTorque = 0.0f;
    this->SetForce(UMath::Vector3::kZero);
    this->mLateralForce = 0.0f;
    this->mLongitudeForce = 0.0f;
}

void SuspensionTraffic::Tire::EndFrame(float dT) {}

void SuspensionTraffic::Tire::UpdateFree(float dT) {
    this->mSlipping = false;
    this->mLoad = 0.0f;
    this->mSlip = 0.0f;
    this->mSlipAngle = 0.0f;
    if ((this->mEBrake > 0.0f) || (this->mBrake > 0.0f)) {
        this->mAV = 0.0f;
    }
    this->mLateralForce = 0.0f;
    this->mLongitudeForce = 0.0f;
}

Behavior *SuspensionTraffic::Construct(const BehaviorParams &params) {
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(UCRC32_BASE)));
    return new SuspensionTraffic(params, sp);
}

void SuspensionTraffic::OnBehaviorChange(const UCrc32 &mechanic) {
    Chassis::OnBehaviorChange(mechanic);

    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mInput);
    } else if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mRBComplex);
        this->GetOwner()->QueryInterface(&this->mRB);
    } else if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        this->GetOwner()->QueryInterface(&this->mTransmission);
    }
}

void SuspensionTraffic::OnTaskSimulate(float dT) {
    if ((this->mInput == nullptr) || (this->mRBComplex == nullptr) || (this->mRB == nullptr)) {
        return;
    }
    this->SetCOG(0.0f, 0.0f);

    ISimable *owner = this->GetOwner();
    State state;
    this->ComputeState(dT, state);
    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->BeginFrame();
    }

    this->DoSimpleAero(state);
    this->DoDriveForces(state);
    this->DoWheelForces(state);

    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->UpdateTime(dT);
    }
    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->EndFrame(dT);
    }
    if (this->DoSleep(state) == SS_ALL) {
        for (unsigned int i = 0; i < 4; ++i) {
            this->mTires[i]->Stop();
        }
    }
    Chassis::OnTaskSimulate(dT);
}

UMath::Vector3 SuspensionTraffic::GetWheelCenterPos(unsigned int i) const {
    UMath::Vector3 pos = this->mTires[i]->GetPosition();
    if (this->mRBComplex == nullptr) {
        return pos;
    }
    UMath::ScaleAdd(this->mRBComplex->GetUpVector(), this->GetWheelRadius(i), pos, pos);
    return pos;
}

void SuspensionTraffic::MatchSpeed(float speed) {
    for (int i = 0; i < 4; ++i) {
        float w = this->mTires[i]->GetRadius();
        this->mTires[i]->SetAngularVelocity(speed / w);
    }
}

void SuspensionTraffic::Reset() {
    ISimable *owner = this->GetOwner();
    IRigidBody *rigidBody = owner->GetRigidBody();
    unsigned int numonground = 0;

    for (int i = 0; i < this->GetNumWheels(); ++i) {
        Tire &wheel = this->GetWheel(i);
        if (wheel.InitPosition(*rigidBody, wheel.GetRadius())) {
            float newCompression = wheel.GetNormal().w + this->GetRideHeight(i);
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
}

void SuspensionTraffic::DoSimpleAero(State &state) {
    const float dragcoef_spec = this->mSuspensionInfo->DRAG_COEFFICIENT();
    float speed = state.speed;
    float drag = speed * dragcoef_spec;
    UVector3 drag_vector(state.linear_vel);

    drag_vector *= -drag;
    this->mRB->ResolveForce(drag_vector);
}

float SuspensionTraffic::DoHP2Steering(State &state) {
    float steer_input = state.steer_input;
    float newsteer = steer_input * this->mMaxSteering;

    this->mLastSteer = newsteer;
    return newsteer * DEG2ANGLE(1.0f);
}

void SuspensionTraffic::DoSteering(State &state, UMath::Vector3 &right, UMath::Vector3 &left) {
    float truesteer = this->DoHP2Steering(state);
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
    if (this->mTransmission == nullptr) {
        return;
    }
    float drive_torque = this->mTransmission->GetDriveTorque();
    if (drive_torque == 0.0f) {
        return;
    }
    float torquesplit = this->mDrivetrainInfo->TORQUE_SPLIT();
    for (unsigned int tire = 0; tire < 4; ++tire) {
        if (this->mTires[tire]->IsOnGround()) {
            float torque = drive_torque;
            torque = tire < 2 ? torque * (1.0f - torquesplit) : (torque * torquesplit);

            if (UMath::Abs(torque) >= 0.0f) {
                this->mTires[tire]->ApplyTorque(torque * 0.5f);
            }
        }
    }
}

static const float TrafficRollAdjust = 0.0f; // TODO value and use
