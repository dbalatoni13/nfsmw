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
        void UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT);

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

        float GetLoad() const {
            return mLoad;
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

        float GetTraction() const {
            return mEBrake > 0.0f ? 0.0f : 1.0f;
        }

        void SetAngularVelocity(float av) {
            mAV = av;
        }

        void SetBrake(float brake) {
            mBrake = brake;
        }

        void SetEBrake(float ebrake) {
            mEBrake = ebrake;
        }

        float GetLongitudeForce() const {
            return mLongitudeForce;
        }

        float GetLateralForce() const {
            return mLateralForce;
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
    float GetWheelTraction(unsigned int index) const override {
        return mTires[index]->GetTraction();
    }
    float GetWheelSlip(unsigned int idx) const override {
        return mTires[idx]->GetSlip();
    }
    float GetToleratedSlip(unsigned int idx) const override {
        return 1.0f;
    }
    float GetWheelSkid(unsigned int idx) const override {
        return mTires[idx]->GetLateralSpeed();
    }
    float GetWheelLoad(unsigned int i) const override {
        return mTires[i]->GetLoad();
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
    Radians GetWheelAngularVelocity(int index) const override;
    float GetWheelSteer(unsigned int wheel) const override {
        return RAD2ANGLE(mLastSteer);
    }
    float GetWheelRadius(unsigned int index) const override;
    float GetMaxSteering() const override {
        return DEG2ANGLE(mMaxSteering);
    }
    float GetWheelSlipAngle(unsigned int idx) const override {
        return mTires[idx]->GetSlipAngle();
    }
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

void SuspensionTraffic::Tire::UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT) {
    const Attrib::Gen::brakes::_LayoutStruct *brakes =
        reinterpret_cast<Attrib::Gen::brakes::_LayoutStruct *>(mBrakes->GetLayoutPointer());
    const Attrib::Gen::tires::_LayoutStruct *tires =
        reinterpret_cast<Attrib::Gen::tires::_LayoutStruct *>(mSpecs->GetLayoutPointer());

    if (mLoad <= 0.0f) {
        mAV = fwd_vel / mRadius;
    }

    mRoadSpeed = fwd_vel;
    mLateralSpeed = lat_vel;
    mLoad = UMath::Max(load, 0.0f);

    if (0.0f < mBrake) {
        float brake = mBrake * (FTLB2NM(brakes->BRAKES.At(mAxleIndex)) * BrakingTorque);
        if (0.0f < mAV) {
            brake = -brake;
        }
        mAppliedTorque += brake;
    }

    if (0.0f < mEBrake) {
        float ebrake = mEBrake * FTLB2NM(brakes->EBRAKE) * EBrakingTorque;
        if (0.0f < mAV) {
            ebrake = -ebrake;
        }
        mAppliedTorque += ebrake;
    }

    const float slip_angle = UMath::Atan2a(lat_vel, UMath::Abs(fwd_vel));
    const float slip = mAV * mRadius - fwd_vel;
    mSlipAngle = slip_angle;

    if (0.0f < mEBrake && 1.0f < UMath::Abs(fwd_vel)) {
        mSlip = slip;
    } else {
        mSlip = 0.0f;
    }

    const float slip_mag = UMath::Sqrt(slip * slip + lat_vel * lat_vel);
    if (mEBrake <= 0.5f || slip_mag <= 1.0f) {
        mLongitudeForce = mAppliedTorque / mRadius;
    } else {
        mSlipping = true;
        mLongitudeForce = ((-fwd_vel + -fwd_vel) * mLoad * tires->GRIP_SCALE.At(mAxleIndex)) / slip_mag;
    }

    mLateralForce = (-lat_vel + -lat_vel) * mLoad * tires->GRIP_SCALE.At(mAxleIndex);
    if (1.0f < slip_mag) {
        mLateralForce /= slip_mag;
    }

    if (UMath::Abs(fwd_vel) <= 1.0f) {
        float speed_scale = 1.0f;
        if (UMath::Abs(lat_vel) < 1.0f) {
            speed_scale = UMath::Abs(lat_vel);
        }
        mLateralForce *= speed_scale;
    } else {
        mLongitudeForce -= UMath::Sina(mSlipAngle * 6.2831855f) * mLateralForce * 0.5f;
    }

    mAV = ((1.0f - mEBrake) * fwd_vel) / mRadius;
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
      mLastSteer(0.0f),         //
      mMaxSteering(45.0f) {
    mRB = nullptr;
    mRBComplex = nullptr;
    mInput = nullptr;
    mNumWheelsOnGround = 0;
    *reinterpret_cast<uint32 *>(reinterpret_cast<char *>(this) + 0x48) = 0;

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

void SuspensionTraffic::DoWheelForces(State &state) {
    UMath::Vector3 steerR;
    UMath::Vector3 steerL;
    DoSteering(state, steerR, steerL);

    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->SetBrake(state.brake_input);
        if (i > 1) {
            mTires[i]->SetEBrake(state.ebrake_input);
        }
    }

    UMath::Vector3 world_cog;
    UMath::Rotate(state.cog, state.matrix, world_cog);

    float shock_specs[2];
    float spring_specs[2];
    float sway_specs[2];
    float travel_specs[2];
    float rideheight_specs[2];
    float progression[2];

    for (unsigned int i = 0; i < 2; ++i) {
        shock_specs[i] = LBIN2NM(mSuspensionInfo.SHOCK_STIFFNESS().At(i));
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

    const UMath::Vector3 &vUp = state.GetUpVector();
    unsigned int wheels_on_ground = 0;
    float max_delta = 0.0f;
    bool resolve = false;

    for (unsigned int i = 0; i < 4; ++i) {
        const unsigned int axle = i / 2;
        Tire &wheel = GetWheel(i);
        wheel.UpdatePosition(state.angular_vel, state.linear_vel, state.matrix, world_cog, state.time, wheel.GetRadius(), true, state.collider,
                             state.dimension.y * 2.0f);

        const UMath::Vector3 &ground_normal = UMath::Vector4To3(wheel.GetNormal());
        const UMath::Vector3 &forward_normal = steering_normals[i];
        UMath::Vector3 lateral_normal;
        UMath::UnitCross(ground_normal, forward_normal, lateral_normal);

        const float upness = UMath::Clamp(UMath::Dot(vUp, ground_normal), 0.0f, 1.0f);
        const float old_compression = wheel.GetCompression();
        float new_compression = rideheight_specs[axle] * upness + wheel.GetNormal().w;
        const float max_compression = travel_specs[axle];

        if (old_compression == 0.0f) {
            max_delta = UMath::Max(max_delta, new_compression - max_compression);
        }

        new_compression = UMath::Max(new_compression, 0.0f);
        if (max_compression < new_compression) {
            max_delta = UMath::Max(max_delta, new_compression - max_compression);
            new_compression = max_compression;
        }

        if (new_compression <= 0.0f || upness <= VehicleSystem::ENABLE_ROLL_STOPS_THRESHOLD) {
            wheel.SetForce(UMath::Vector3::kZero);
            wheel.UpdateFree(state.time);
        } else {
            ++wheels_on_ground;

            float damp = ((new_compression - old_compression) / state.time) * shock_specs[axle];
            if (mSuspensionInfo.SHOCK_BLOWOUT() * state.mass * 9.81f < damp) {
                damp = 0.0f;
            }

            float vertical_load =
                damp + (new_compression * spring_specs[axle]) * (new_compression * progression[axle] + 1.0f) + sway_stiffness[i];
            vertical_load = UMath::Max(vertical_load, 0.0f);

            const float load_scale = UMath::Max(0.3f, upness * 4.0f - 3.0f);
            const UMath::Vector3 &point_velocity = wheel.GetVelocity();
            const float lat_speed = UMath::Dot(point_velocity, lateral_normal);
            const float fwd_speed = UMath::Dot(point_velocity, forward_normal);
            wheel.UpdateLoaded(lat_speed, fwd_speed, load_scale * vertical_load, state.time);

            float max_lateral = (lat_speed / state.time) * (0.25f * state.mass);
            if (max_lateral < 0.0f) {
                max_lateral = -max_lateral;
            }

            const float lateral_force = UMath::Clamp(wheel.GetLateralForce(), -max_lateral, max_lateral);

            UMath::Vector3 force;
            UMath::Scale(lateral_normal, lateral_force, force);

            UMath::Vector3 drive_force;
            UMath::UnitCross(lateral_normal, ground_normal, drive_force);
            UMath::ScaleAdd(drive_force, wheel.GetLongitudeForce(), force, force);
            UMath::ScaleAdd(vUp, vertical_load, force, force);

            wheel.SetForce(force);
            resolve = true;
        }

        if (new_compression == 0.0f) {
            wheel.IncAirTime(state.time);
        } else {
            wheel.SetAirTime(0.0f);
        }
        wheel.SetCompression(new_compression);
    }

    if (resolve) {
        for (unsigned int i = 0; i < 4; ++i) {
            Tire &wheel = GetWheel(i);
            UMath::Vector3 p = wheel.GetLocalArm();
            p.y = (p.y + wheel.GetCompression()) - rideheight_specs[i / 2];
            UMath::RotateTranslate(p, state.matrix, p);
            wheel.SetPosition(p);

            UMath::Vector3 r;
            UMath::Sub(p, world_cog, r);
            r.y *= 0.5f;

            UMath::Vector3 torque;
            UMath::Cross(r, wheel.GetForce(), torque);
            mRB->Resolve(wheel.GetForce(), torque);
        }
    }

    if (0.0f < max_delta) {
        for (unsigned int i = 0; i < 4; ++i) {
            Tire &wheel = GetWheel(i);
            wheel.SetY(wheel.GetPosition().y + max_delta);
        }
        mRB->ModifyYPos(max_delta);
    }

    mNumWheelsOnGround = wheels_on_ground;
}
