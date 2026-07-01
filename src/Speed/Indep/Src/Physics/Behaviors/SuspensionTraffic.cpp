#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
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
        float GetTraction() const {
            return mEBrake > 0.0f ? 0.0f : 1.0f;
        }

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

        float GetSlipAngle() const {
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
        return 1.0f;
    }
    float GetWheelSkid(unsigned int idx) const override {
        return mTires[idx]->GetLateralSpeed();
    }
    Newtons GetWheelLoad(unsigned int i) const override {
        return mTires[i]->GetLoad();
    }
    float GetWheelTraction(unsigned int index) const override {
        return mTires[index]->GetTraction();
    }
    void SetWheelAngularVelocity(int wheel, float w) override {}
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
        return DEG2ANGLE(mLastSteer);
    }
    float GetMaxSteering() const override {
        return DEG2ANGLE(mMaxSteering);
    }
    Angle GetWheelSlipAngle(unsigned int idx) const override {
        return mTires[idx]->GetSlipAngle();
    }

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

extern float BrakingTorque;
extern float EBrakingTorque;

// UNSOLVED, stack issues due to scheduling
void SuspensionTraffic::Tire::UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT) {
    float slip_speed;
    float catchupfriction;
    float skid_speed;

    if (this->mLoad <= 0.0f) {
        this->mAV = fwd_vel / this->mRadius;
    }

    this->mRoadSpeed = fwd_vel;
    this->mLateralSpeed = lat_vel;
    this->mLoad = UMath::Max(load, 0.0f);

    if (0.0f < this->mBrake) {
        const float brake_spec = this->mBrakes->BRAKES().At(this->mAxleIndex);
        float bt = this->mBrake * (FTLB2NM(brake_spec) * BrakingTorque);
        if (0.0f < this->mAV) {
            bt = -bt;
        }
        this->ApplyTorque(bt);
    }

    if (0.0f < this->mEBrake) {
        const float ebrake_spec = this->mBrakes->EBRAKE();
        float ebt = this->mEBrake * (FTLB2NM(ebrake_spec) * EBrakingTorque);
        if (0.0f < this->mAV) {
            ebt = -ebt;
        }
        this->ApplyTorque(ebt);
    }

    this->mSlipAngle = UMath::Atan2a(lat_vel, UMath::Abs(fwd_vel));
    slip_speed = this->mAV * this->mRadius - fwd_vel;

    if (0.0f < this->mEBrake && 1.0f < UMath::Abs(fwd_vel)) {
        this->mSlip = slip_speed;
    } else {
        this->mSlip = 0.0f;
    }

    skid_speed = UMath::Sqrt(slip_speed * slip_speed + lat_vel * lat_vel);
    if (this->mEBrake > 0.5f && skid_speed > 1.0f) {
        this->mSlipping = true;
        this->mLongitudeForce = ((-fwd_vel * 2) * this->mLoad * this->mSpecs->GRIP_SCALE().At(this->mAxleIndex)) / skid_speed;
    } else {
        this->mLongitudeForce = this->mAppliedTorque / this->mRadius;
    }

    this->mLateralForce = (-lat_vel * 2) * this->mLoad * this->mSpecs->GRIP_SCALE().At(this->mAxleIndex);
    if (1.0f < skid_speed) {
        this->mLateralForce /= skid_speed;
    }

    if (1.0f < UMath::Abs(fwd_vel)) {
        this->mLongitudeForce -= UMath::Sina(this->mSlipAngle) * this->mLateralForce * 0.5f;
    } else {
        catchupfriction = UMath::Min(UMath::Abs(lat_vel), 1.0f);
        this->mLateralForce *= catchupfriction;
    }

    this->mAV = ((1.0f - this->mEBrake) * fwd_vel) / this->mRadius;
}

Behavior *SuspensionTraffic::Construct(const BehaviorParams &params) {
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(UCRC32_BASE)));
    return new SuspensionTraffic(params, sp);
}

SuspensionTraffic::SuspensionTraffic(const BehaviorParams &bp, const SuspensionParams &sp)
    : Chassis(bp),              //
      mTireInfo(this, 0),       //
      mBrakeInfo(this, 0),      //
      mSuspensionInfo(this, 0), //
      mDrivetrainInfo(this, 0), //
      mLastSteer(0.0f),         //
      mMaxSteering(45.0f) {
    this->mRB = nullptr;
    this->mRBComplex = nullptr;
    this->mInput = nullptr;
    this->mNumWheelsOnGround = 0;

    this->EnableProfile("TODO");

    this->GetOwner()->QueryInterface(&this->mRB);
    this->GetOwner()->QueryInterface(&this->mRBComplex);
    this->GetOwner()->QueryInterface(&this->mInput);
    this->GetOwner()->QueryInterface(&this->mTransmission);

    for (int i = 0; i < 4; ++i) {
        bool is_front = i < 2;
        float diameter = Physics::Info::WheelDiameter(this->mTireInfo, is_front);
        this->mTires[i] = new Tire(diameter * 0.5f, i, this->mTireInfo, this->mBrakeInfo);
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

SuspensionTraffic::~SuspensionTraffic() {
    for (int i = 0; i < 4; ++i) {
        delete mTires[i];
    }
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

// UNSOLVED, float math
void SuspensionTraffic::DoWheelForces(State &state) {
    const float dT = state.time;
    UVector3 steerR;
    UVector3 steerL;

    this->DoSteering(state, steerR, steerL);

    for (int i = 0; i < 4; ++i) {
        this->mTires[i]->SetBrake(state.brake_input);
        if (i > 1) {
            this->mTires[i]->SetEBrake(state.ebrake_input);
        }
    }

    unsigned int wheelsOnGround = 0;
    float maxDelta = 0.0f;
    const UMath::Vector3 &vFwd = state.GetForwardVector();
    const UMath::Vector3 &vUp = state.GetUpVector();
    const float mass = state.mass;

    UMath::Vector3 cog;
    UMath::Rotate(state.cog, state.matrix, cog);

    float shock_specs[2];
    float spring_specs[2];
    float sway_specs[2];
    float travel_specs[2];
    float rideheight_specs[2];
    float progression[2];

    for (unsigned int i = 0; i < 2; ++i) {
        shock_specs[i] = LBIN2NM(this->mSuspensionInfo->SHOCK_STIFFNESS().At(i));
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
        int axle = i / 2;
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
            const float rise = diff / dT;
            float spring = newCompression * spring_specs[axle];
            float damp = rise * shock_specs[axle];

            springForce = spring * (newCompression * progression[axle] + 1.0f);

            if (damp > this->mSuspensionInfo.SHOCK_BLOWOUT() * (mass * 9.81f)) {
                damp = 0.0f;
            }

            springForce = damp + springForce;
            springForce += sway_stiffness[i];
            springForce = UMath::Max(springForce, 0.0f);

            UVector3 verticalForce = vUp * springForce;
            UVector3 driveForce;
            UVector3 lateralForce;
            UVector3 c;

            UMath::Cross(forwardNormal, groundNormal, c);
            UMath::Cross(c, forwardNormal, c);

            float d2 = UMath::Dot(c, groundNormal);
            float load = UMath::Max(d2 * 4.0f - 3.0f, 0.0f) * springForce;

            const UMath::Vector3 &pointVelocity = wheel.GetVelocity();
            UVector3 vNorm(pointVelocity);
            float speed = vNorm.Normalize();
            float vdot = UMath::Dot(vNorm, lateralNormal);
            float xspeed = UMath::Dot(pointVelocity, lateralNormal);
            float zspeed = UMath::Dot(pointVelocity, forwardNormal);

            wheel.UpdateLoaded(xspeed, zspeed, load, state.time);

            float traction_force = wheel.GetLateralForce();
            float max_traction = UMath::Abs((xspeed / dT) * (0.25f * mass));
            traction_force = UMath::Clamp(traction_force, -max_traction, max_traction);

            lateralForce = lateralNormal * traction_force;
            UMath::UnitCross(lateralNormal, groundNormal, driveForce);
            UMath::Scale(driveForce, wheel.GetLongitudeForce());

            UMath::Vector3 force;
            UMath::Add(lateralForce, driveForce, force);
            UMath::Add(force, verticalForce, force);

            wheel.SetForce(force);
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
        UMath::Vector3 cg;
        UMath::RotateTranslate(state.cog, state.matrix, cg);

        for (unsigned int i = 0; i < this->GetNumWheels(); ++i) {
            Tire &wheel = this->GetWheel(i);
            UVector3 p(wheel.GetLocalArm());

            p.y += wheel.GetCompression();
            const UMath::Vector3 &force = wheel.GetForce();
            p.y = p.y - rideheight_specs[i / 2];

            UMath::RotateTranslate(p, state.matrix, p);
            wheel.SetPosition(p);

            UMath::Vector3 torque;
            UMath::Vector3 r;
            UMath::Sub(p, cg, r);
            r.y *= 0.5f;

            UMath::Cross(r, force, torque);
            this->mRB->Resolve(force, torque);
        }
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
