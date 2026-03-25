
#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/Wheel.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

extern Table TrailerCorneringForce;

// total size: 0x108
class SuspensionTrailer : public Chassis {
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
        void BeginFrame();
        void EndFrame(float dT);
        void UpdateFree(float dT);
        void UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT);

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

        float GetRoadSpeed() const {
            return mRoadSpeed;
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

      private:
        const float mRadius;          // offset 0xC4, size 0x4
        float mBrake;                 // offset 0xC8, size 0x4
        float mEBrake;                // offset 0xCC, size 0x4
        float mAV;                    // offset 0xD0, size 0x4
        float mLoad;                  // offset 0xD4, size 0x4
        float mLateralForce;          // offset 0xD8, size 0x4
        float mLongitudeForce;        // offset 0xDC, size 0x4
        float mAppliedTorque;         // offset 0xE0, size 0x4
        float mSlip;                  // offset 0xE4, size 0x4
        float mLastTorque;            // offset 0xE8, size 0x4
        const int mWheelIndex;        // offset 0xEC, size 0x4
        float mRoadSpeed;             // offset 0xF0, size 0x4
        const Attrib::Gen::tires *mSpecs;   // offset 0xF4, size 0x4
        const Attrib::Gen::brakes *mBrakes; // offset 0xF8, size 0x4
        const int mAxleIndex;         // offset 0xFC, size 0x4
        bool mSlipping;               // offset 0x100, size 0x1
        float mLateralSpeed;          // offset 0x104, size 0x4
        LastRotationSign mLastSign;   // offset 0x108, size 0x4
    };

    static Behavior *Construct(const BehaviorParams &params);

    SuspensionTrailer(const struct BehaviorParams &bp, const SuspensionParams &sp);
    void ComputeState(float dT, State &state) const;
    void DoSimpleAero(State &state);
    void DoWheelForces(State &state);

    // Overrides
    // IUnknown
    ~SuspensionTrailer() override;

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void OnTaskSimulate(float dT) override;
    void Reset() override;

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
        return 1.0f;
    }
    float GetWheelSkid(unsigned int idx) const override {
        return mTires[idx]->GetLateralSpeed();
    }
    float GetWheelSlipAngle(unsigned int idx) const override {
        return GetVehicle()->GetSlipAngle();
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
    void SetWheelAngularVelocity(int wheel, float w) override {}
    float GetWheelSteer(unsigned int wheel) const override {
        return 0.0f;
    }
    float GetWheelRadius(unsigned int index) const override {
        return mTires[index]->GetRadius();
    }
    float GetMaxSteering() const override {
        return 0.0f;
    }
    void MatchSpeed(float speed) override;

    Tire &GetWheel(unsigned int i) {
        return *mTires[i];
    }

  private:
    BehaviorSpecsPtr<Attrib::Gen::tires> mTireInfo;         // offset 0x94, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::brakes> mBrakeInfo;       // offset 0xA8, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::chassis> mSuspensionInfo; // offset 0xBC, size 0x14
    IRigidBody *mRB;                                        // offset 0xD0, size 0x4
    IDynamicsEntity *mIDynamicsEntity;                      // offset 0xD4, size 0x4
    ICollisionBody *mRBComplex;                             // offset 0xD8, size 0x4
    IDamageable *mIDamage;                                  // offset 0xDC, size 0x4
    bool mPowerSliding;                                     // offset 0xE0, size 0x1
    unsigned int mNumWheelsOnGround;                        // offset 0xE4, size 0x4
    float mMotionDampingFactor;                             // offset 0xE8, size 0x4
    const float mSteeringControl;                           // offset 0xEC, size 0x4
    float mTimeInAir;                                       // offset 0xF0, size 0x4
    bool mDriftPhysics;                                     // offset 0xF4, size 0x1
    Tire *mTires[4];                                        // offset 0xF8, size 0x10
};

SuspensionTrailer::Tire::Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes)
    : Wheel(0), //
      mRadius(UMath::Max(radius, 0.1f)), //
      mBrake(0.0f), //
      mEBrake(0.0f), //
      mAV(0.0f), //
      mLoad(0.0f), //
      mLateralForce(0.0f), //
      mLongitudeForce(0.0f), //
      mAppliedTorque(0.0f), //
      mSlip(0.0f), //
      mLastTorque(0.0f), //
      mWheelIndex(index), //
      mRoadSpeed(0.0f), //
      mSpecs(specs), //
      mBrakes(brakes), //
      mAxleIndex(index >> 1), //
      mSlipping(false), //
      mLateralSpeed(0.0f), //
      mLastSign(WAS_ZERO) {}

void SuspensionTrailer::Tire::BeginFrame() {
    mAppliedTorque = 0.0f;
    SetForce(UMath::Vector3::kZero);
    mLateralForce = 0.0f;
    mLongitudeForce = 0.0f;
}

void SuspensionTrailer::Tire::EndFrame(float dT) {}

void SuspensionTrailer::Tire::UpdateFree(float dT) {
    mLoad = 0.0f;
    mSlip = 0.0f;
    mSlipping = false;
    if (mEBrake > 0.0f || mBrake > 0.0f) {
        mAV = 0.0f;
    }
    mLateralForce = 0.0f;
    mLongitudeForce = 0.0f;
}

void SuspensionTrailer::Tire::UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT) {
    const Attrib::Gen::brakes::_LayoutStruct *brakes =
        reinterpret_cast<Attrib::Gen::brakes::_LayoutStruct *>(mBrakes->GetLayoutPointer());
    const Attrib::Gen::tires::_LayoutStruct *tires =
        reinterpret_cast<Attrib::Gen::tires::_LayoutStruct *>(mSpecs->GetLayoutPointer());

    const float brake_torque = brakes->BRAKES.At(mAxleIndex) * FTLB2NM(4.0f);
    const float ebrake_torque = brakes->EBRAKE * FTLB2NM(10.0f);
    const float dynamic_grip = tires->DYNAMIC_GRIP.At(mAxleIndex);
    const float static_grip = tires->STATIC_GRIP.At(mAxleIndex);
    const float grip_scale = tires->GRIP_SCALE.At(mAxleIndex);

    mRoadSpeed = fwd_vel;
    mLateralSpeed = lat_vel;
    mLoad = UMath::Max(load, 0.0f);

    float brake = mBrake * brake_torque;
    float ebrake = mEBrake * ebrake_torque;
    if (0.0f < fwd_vel) {
        brake = -brake;
        ebrake = -ebrake;
    }

    mAppliedTorque += brake;
    mAppliedTorque += ebrake;

    const float slip_angle = UMath::Atan2a(lat_vel, UMath::Abs(fwd_vel));
    mLongitudeForce = mAppliedTorque / mRadius;
    mSlip = mAV * mRadius - fwd_vel;

    float lateral_force = (mLoad * grip_scale) * TrailerCorneringForce.GetValue(UMath::Abs(slip_angle));
    if (0.0f < lat_vel) {
        lateral_force = -lateral_force;
    }
    mLateralForce = lateral_force;

    if (mEBrake <= 0.5f) {
        if (mAxleIndex == 1) {
            mLateralForce *= 1.5f;
        }
        mAV = fwd_vel / mRadius;
    } else {
        if (mAxleIndex == 1) {
            mLateralForce *= 0.75f;
        }
        mAV = 0.0f;
        mSlipping = true;
    }

    const float grip_mag = UMath::Sqrt(mLateralForce * mLateralForce + mLongitudeForce * mLongitudeForce);
    if (mLoad * static_grip < grip_mag && 0.001f < grip_mag) {
        const float grip_ratio = (mLoad * dynamic_grip) / grip_mag;
        mLateralForce *= grip_ratio;
        mLongitudeForce *= grip_ratio;
    }

    if (UMath::Abs(fwd_vel) <= 1.0f) {
        float speed_scale = 1.0f;
        if (UMath::Abs(lat_vel) < 1.0f) {
            speed_scale = UMath::Abs(lat_vel);
        }
        mLateralForce *= speed_scale;
    } else {
        mLongitudeForce -= UMath::Sina(slip_angle * 6.2831855f) * mLateralForce * 0.5f;
    }

    if (0.0f < mEBrake) {
        mAV = 0.0f;
        mSlip = -fwd_vel;
    }
}

Behavior *SuspensionTrailer::Construct(const BehaviorParams &params) {
    // "BASE"
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(0xa6b47fac)));
    return new SuspensionTrailer(params, sp);
}

SuspensionTrailer::SuspensionTrailer(const BehaviorParams &bp, const SuspensionParams &sp)
    : Chassis(bp),              //
      mTireInfo(this, 0),       //
      mBrakeInfo(this, 0),      //
      mSuspensionInfo(this, 0), //
      mRB(0),                   //
      mIDynamicsEntity(0),      //
      mRBComplex(0),            //
      mPowerSliding(false),     //
      mNumWheelsOnGround(0),    //
      mMotionDampingFactor(0.0f), //
      mSteeringControl(1.1f),     //
      mTimeInAir(0.0f) {
    *reinterpret_cast<uint32 *>(reinterpret_cast<char *>(this) + 0x48) = 0;
    mDriftPhysics = false;

    GetOwner()->QueryInterface(&mRB);
    GetOwner()->QueryInterface(&mRBComplex);
    GetOwner()->QueryInterface(&mIDynamicsEntity);
    GetOwner()->QueryInterface(&mIDamage);

    for (int i = 0; i < 4; ++i) {
        float diameter = Physics::Info::WheelDiameter(mTireInfo, i < 2);
        mTires[i] = new Tire(diameter * 0.5f, i, mTireInfo, mBrakeInfo);
    }

    UMath::Vector3 dimension;
    GetOwner()->GetRigidBody()->GetDimension(dimension);

    float axle_width_f = mSuspensionInfo.TRACK_WIDTH().At(0) - INCH2METERS(mTireInfo.SECTION_WIDTH().At(0));
    float axle_width_r = mSuspensionInfo.TRACK_WIDTH().At(1) - INCH2METERS(mTireInfo.SECTION_WIDTH().At(1));
    float front_axle = mSuspensionInfo.FRONT_AXLE();
    float rear_axle = front_axle - mSuspensionInfo.WHEEL_BASE();

    UVector3 fl(-axle_width_f * 0.5f, -dimension.y, front_axle);
    UVector3 fr(axle_width_f * 0.5f, -dimension.y, front_axle);
    UVector3 rl(-axle_width_r * 0.5f, -dimension.y, rear_axle);
    UVector3 rr(axle_width_r * 0.5f, -dimension.y, rear_axle);

    GetWheel(0).SetLocalArm(fl);
    GetWheel(1).SetLocalArm(fr);
    GetWheel(2).SetLocalArm(rl);
    GetWheel(3).SetLocalArm(rr);
}

SuspensionTrailer::~SuspensionTrailer() {
    for (int i = 0; i < 4; ++i) {
        delete mTires[i];
    }
}

void SuspensionTrailer::OnBehaviorChange(const UCrc32 &mechanic) {
    Chassis::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mIDynamicsEntity);
        GetOwner()->QueryInterface(&mRBComplex);
        GetOwner()->QueryInterface(&mRB);
    } else if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        GetOwner()->QueryInterface(&mIDamage);
    }
}

void SuspensionTrailer::MatchSpeed(float speed) {
    for (int i = 0; i < 4; ++i) {
        float w = mTires[i]->GetRadius();
        mTires[i]->SetAngularVelocity(speed / w);
    }
}

void SuspensionTrailer::ComputeState(float dT, State &state) const {
    Chassis::ComputeState(dT, state);
    if (mIDynamicsEntity && !Dynamics::Articulation::IsJoined(mIDynamicsEntity)) {
        state.ebrake_input = 0.0f;
        state.brake_input = 0.0f;
        state.gas_input = 0.0f;
        state.cog = UMath::Vector3::kZero;
        mRBComplex->SetCenterOfGravity(state.cog);
    } else {
        float fwbias = mSuspensionInfo->FRONT_WEIGHT_BIAS();
        float cg_z = 0.0f;
        float cg_y = INCH2METERS(mSuspensionInfo->ROLL_CENTER());
        float ride_height = UMath::Max(GetRideHeight(0), GetRideHeight(2));
        state.cog = UVector3(0.0f, cg_y - (state.dimension.y + ride_height), cg_z);
        mRBComplex->SetCenterOfGravity(state.cog);
    }
}

void SuspensionTrailer::OnTaskSimulate(float dT) {
    if (!mRB || !mRBComplex) {
        return;
    }
    ISimable *owner = GetOwner();
    State state;
    ComputeState(dT, state);
    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->BeginFrame();
    }
    DoSimpleAero(state);
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
    Chassis::OnTaskSimulate(dT);
}

UMath::Vector3 SuspensionTrailer::GetWheelCenterPos(unsigned int i) const {
    UMath::Vector3 pos = mTires[i]->GetPosition();
    if (!mRBComplex) {
        return pos;
    } else {
        UMath::ScaleAdd(mRBComplex->GetUpVector(), GetWheelRadius(i), pos, pos);
        return pos;
    }
}

void SuspensionTrailer::Reset() {
    ISimable *owner = GetOwner();
    IRigidBody &rigidBody = *owner->GetRigidBody();
    unsigned int numonground = 0;
    for (unsigned int i = 0; i < GetNumWheels(); ++i) {
        Tire &wheel = GetWheel(i);
        if (wheel.InitPosition(rigidBody, wheel.GetRadius())) {
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
    mTimeInAir = 0.0f;
    mMotionDampingFactor = 0.0f;
}

void SuspensionTrailer::DoSimpleAero(State &state) {
    const float dragcoef_spec = mSuspensionInfo->DRAG_COEFFICIENT();
    float speed = state.speed;
    float drag = -(speed * dragcoef_spec);
    UVector3 drag_vector = state.linear_vel;
    drag_vector *= drag;
    mRB->ResolveForce(drag_vector);
}

void SuspensionTrailer::DoWheelForces(State &state) {
    for (unsigned int i = 0; i < 4; ++i) {
        mTires[i]->SetBrake(state.brake_input);
        mTires[i]->SetEBrake(state.ebrake_input);
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

    const UMath::Vector3 &vUp = state.GetUpVector();
    const UMath::Vector3 &forward = state.GetForwardVector();
    unsigned int wheels_on_ground = 0;
    float max_delta = 0.0f;
    bool resolve = false;

    for (unsigned int i = 0; i < 4; ++i) {
        const unsigned int axle = i / 2;
        Tire &wheel = GetWheel(i);
        wheel.UpdatePosition(state.angular_vel, state.linear_vel, state.matrix, world_cog, state.time, wheel.GetRadius(), true, state.collider,
                             state.dimension.y * 2.0f);

        const UMath::Vector3 &ground_normal = UMath::Vector4To3(wheel.GetNormal());
        UMath::Vector3 lateral_normal;
        UMath::UnitCross(ground_normal, forward, lateral_normal);

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
            const float fwd_speed = UMath::Dot(point_velocity, forward);
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
