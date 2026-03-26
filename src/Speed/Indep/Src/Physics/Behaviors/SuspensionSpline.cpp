
#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UEALibs.hpp"
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
void OrthoInverse(UMath::Matrix4 &m);

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

        float GetRoadSpeed() const {
            return mRoadSpeed;
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
    void NISCarTweaks(float dT);

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
    void ApplyVehicleEntryForces(bool enteringVehicle, const UMath::Vector3 &pos, bool calledfromEvent) override;
    const float GetWheelRoadHeight(unsigned int i) const override;
    bool IsWheelOnGround(unsigned int i) const override;
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

void SuspensionSpline::DoWheelForces(State &state) {
    UMath::Vector3 steerR;
    UMath::Vector3 steerL;
    DoSteering(state, steerR, steerL);

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
        shock_specs[i] = LBIN2NM(mSuspensionInfo->SHOCK_STIFFNESS().At(i));
        shock_ext_specs[i] = LBIN2NM(mSuspensionInfo->SHOCK_EXT_STIFFNESS().At(i));
        shock_valving[i] = INCH2METERS(mSuspensionInfo->SHOCK_VALVING().At(i));
        shock_digression[i] = 1.0f - mSuspensionInfo->SHOCK_DIGRESSION().At(i);
        spring_specs[i] = LBIN2NM(mSuspensionInfo->SPRING_STIFFNESS().At(i));
        sway_specs[i] = LBIN2NM(mSuspensionInfo->SWAYBAR_STIFFNESS().At(i));
        travel_specs[i] = INCH2METERS(mSuspensionInfo->TRAVEL().At(i));
        rideheight_specs[i] = INCH2METERS(mSuspensionInfo->RIDE_HEIGHT().At(i));
        progression[i] = mSuspensionInfo->SPRING_PROGRESSION().At(i);
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

    UMath::Vector3 world_cog;
    UMath::Rotate(state.cog, state.matrix, world_cog);

    const UMath::Vector3 &vUp = state.GetUpVector();
    const float max_shock_force = mSuspensionInfo->SHOCK_BLOWOUT() * state.mass * 9.81f;

    unsigned int wheels_on_ground = 0;
    float max_delta = 0.0f;
    bool resolve = false;
    UMath::Vector3 total_force = UMath::Vector3::kZero;
    UMath::Vector3 total_torque = UMath::Vector3::kZero;

    for (unsigned int i = 0; i < 4; ++i) {
        const unsigned int axle = i / 2;
        Tire &wheel = GetWheel(i);
        wheel.UpdatePosition(state.angular_vel, state.linear_vel, state.matrix, world_cog, state.time, wheel.GetRadius(), true, state.collider,
                             state.dimension.y * 2.0f);

        const UMath::Vector3 &ground_normal = UMath::Vector4To3(wheel.GetNormal());
        const UMath::Vector3 &forward_normal = steering_normals[i];
        UMath::Vector3 lateral_normal;
        UMath::UnitCross(ground_normal, forward_normal, lateral_normal);

        const float upness = UMath::Clamp(UMath::Dot(ground_normal, vUp), 0.0f, 1.0f);
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

            float rise = (new_compression - old_compression) / state.time;
            if (0.001f < shock_valving[axle] && shock_digression[axle] < 1.0f) {
                float abs_rise = UMath::Abs(rise);
                if (shock_valving[axle] < abs_rise) {
                    float digressed = shock_valving[axle] * UMath::Pow(abs_rise / shock_valving[axle], shock_digression[axle]);
                    rise = rise <= 0.0f ? -digressed : digressed;
                }
            }

            float damp = rise * (rise <= 0.0f ? shock_ext_specs[axle] : shock_specs[axle]);
            if (max_shock_force < damp) {
                damp = 0.0f;
            }

            float vertical_load =
                damp + (new_compression * spring_specs[axle]) * (new_compression * progression[axle] + 1.0f) + sway_stiffness[i];
            vertical_load = UMath::Max(vertical_load, 0.0f);

            UMath::Vector3 vertical_force;
            UMath::Scale(vUp, vertical_load, vertical_force);

            UMath::Vector3 up_cross_forward;
            UMath::Cross(forward_normal, ground_normal, up_cross_forward);
            UMath::Vector3 projected_forward;
            UMath::Cross(up_cross_forward, forward_normal, projected_forward);

            const float load_scale = UMath::Max(0.25f, UMath::Dot(projected_forward, ground_normal) * 4.0f - 3.0f);
            const float scaled_load = load_scale * vertical_load;

            const UMath::Vector3 &point_velocity = wheel.GetVelocity();
            const float lat_speed = UMath::Dot(point_velocity, lateral_normal);
            const float fwd_speed = UMath::Dot(point_velocity, forward_normal);

            UMath::Vector3 tire_accel;
            tire_accel.x = (lat_speed - wheel.GetLateralSpeed()) / state.time;
            tire_accel.y = 0.0f;
            tire_accel.z = (fwd_speed - wheel.GetRoadSpeed()) / state.time;

            wheel.UpdateLoaded(lat_speed, fwd_speed, scaled_load, state.time);

            UMath::Vector3 force;
            UMath::Scale(lateral_normal, -(tire_accel.x * (state.mass * 0.25f)), force);
            UMath::ScaleAdd(forward_normal, tire_accel.z * (state.mass * 0.25f), force, force);

            const float force_mag = UMath::Length(force);
            if (mTireInfo->STATIC_GRIP().At(axle) * scaled_load < force_mag) {
                UMath::Scale(force, (mTireInfo->DYNAMIC_GRIP().At(axle) * scaled_load) / force_mag, force);
            }

            UMath::Vector3 wheel_force;
            UMath::Add(force, vertical_force, wheel_force);

            UMath::Vector3 p = wheel.GetLocalArm();
            p.y = (p.y + new_compression) - rideheight_specs[axle];
            UMath::RotateTranslate(p, state.matrix, p);
            wheel.SetPosition(p);

            UMath::Vector3 r;
            UMath::Sub(p, world_cog, r);

            UMath::Vector3 torque;
            UMath::Cross(r, wheel_force, torque);
            UMath::Add(total_torque, torque, total_torque);
            UMath::Add(total_force, wheel_force, total_force);

            wheel.SetForce(wheel_force);
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
        mRB->Resolve(total_force, total_torque);
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

bool SuspensionSpline::SetNISPosition(const UMath::Matrix4 &position, bool initial, float dT) {
    bool success = true;

    GetVehicle()->Activate();

    if (initial) {
        success = GetVehicle()->SetVehicleOnGround(UMath::ExtractAxis(position, 3), UMath::ExtractAxis(position, 2));
        GetVehicle()->SetSpeed(0.0f);
        mNISPosition = position;
        mRB->SetLinearVelocity(UMath::Vector3::kZero);
        mRB->SetAngularVelocity(UMath::Vector3::kZero);
        mInput->ClearInput();
    } else {
        UMath::Vector3 linear_velocity_xz = UMath::Vector3::kZero;
        float angular_velocity_y = 0.0f;

        if (dT > 1.0e-6f) {
            UMath::Sub(UMath::ExtractAxis(position, 3), UMath::ExtractAxis(mNISPosition, 3), linear_velocity_xz);
            UMath::Scale(linear_velocity_xz, 1.0f / dT, linear_velocity_xz);

            const float last_rotation = UMath::Atan2r(UMath::ExtractAxis(mNISPosition, 2).z, -UMath::ExtractAxis(mNISPosition, 2).x);
            const float new_rotation = UMath::Atan2r(UMath::ExtractAxis(position, 2).z, -UMath::ExtractAxis(position, 2).x);
            angular_velocity_y = (new_rotation - last_rotation) / dT;
        }

        mNISPosition = position;

        UMath::Vector3 pos = UMath::ExtractAxis(position, 3);
        pos.y = mRB->GetPosition().y;
        mRB->SetPosition(pos);

        UMath::Vector3 vel = mRB->GetLinearVelocity();
        vel.x = linear_velocity_xz.x;
        vel.z = linear_velocity_xz.z;
        mRB->SetLinearVelocity(vel);

        UMath::Vector3 avel = mRB->GetAngularVelocity();
        avel.y = angular_velocity_y;
        mRB->SetAngularVelocity(avel);

        UMath::Matrix4 matrix;
        mRB->GetMatrix4(matrix);

        UMath::Vector3 cross;
        UMath::Cross(UMath::ExtractAxis(matrix, 1), UMath::ExtractAxis(mNISPosition, 1), cross);

        const float pitch = asinf(UMath::Clamp(cross.x, -1.0f, 1.0f)) * 0.15915494f;
        const float roll = asinf(UMath::Clamp(cross.z, -1.0f, 1.0f)) * 0.15915494f;
        const float max_angle = mConstraint * 0.5f * 0.0027777778f;
        const float newroll = UMath::Clamp(roll, -max_angle, max_angle);
        const float newpitch = UMath::Clamp(pitch, -max_angle, max_angle);

        if (newroll != roll) {
            MATRIX4_multzrot(&matrix, roll - newroll, &matrix);
        }

        if (newpitch != pitch) {
            MATRIX4_multxrot(&matrix, pitch - newpitch, &matrix);
        }

        UMath::Matrix4 invorient;
        UMath::Matrix4 localmatrix;
        invorient = matrix;
        OrthoInverse(invorient);
        UMath::Mult(invorient, mNISPosition, localmatrix);

        const float newdelta = UMath::Atan2a(UMath::ExtractAxis(localmatrix, 2).z, -UMath::ExtractAxis(localmatrix, 2).x);
        UMath::SetYRot(localmatrix, newdelta);
        UMath::Mult(localmatrix, matrix, matrix);
        mRB->SetOrientation(matrix);
    }

    return success;
}

void SuspensionSpline::SetAnimPitch(float dip, float time) {
    mAnimatedPitchLength = time;
    mAnimatedPitchTime = 0.0f;
    mAnimatedPitch = 0.0f;

    float length;
    GetWheelBase(0, &length);
    if (dip < 0.0f) {
        mAnimatedPitchDelta = -(static_cast<float>(bATan(length * 0.5f, -dip)) * 3.0517578e-05f);
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
        mAnimatedRollDelta = -(static_cast<float>(bATan(width * 0.5f, -dip)) * 3.0517578e-05f);
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
        mAnimatedShakeDelta = -(static_cast<float>(bATan(width * 0.5f, -dip)) * 3.0517578e-05f);
    } else {
        mAnimatedShakeDelta = static_cast<float>(bATan(width * 0.5f, dip)) * 3.0517578e-05f;
    }
}

float SuspensionSpline::GetAnimShake() const {
    return mAnimatedShake;
}

void SuspensionSpline::NISCarTweaks(float dT) {
    if (mAnimatedPitchLength != 0.0f) {
        float time = mAnimatedPitchTime + dT;
        mAnimatedPitchTime = time;
        if (mAnimatedPitchLength <= time) {
            mAnimatedPitchLength = 0.0f;
            mAnimatedPitch = 0.0f;
        } else {
            float halfLength = mAnimatedPitchLength * 0.5f;
            if (halfLength < time) {
                time = halfLength - time * 0.5f;
            } else {
                time *= 0.5f;
            }
            mAnimatedPitch = (time / halfLength) * mAnimatedPitchDelta;
        }
    }

    if (mAnimatedRollLength != 0.0f) {
        float time = mAnimatedRollTime + dT;
        mAnimatedRollTime = time;
        if (mAnimatedRollLength <= time) {
            mAnimatedRollLength = 0.0f;
            mAnimatedRoll = 0.0f;
        } else {
            float halfLength = mAnimatedRollLength * 0.5f;
            if (halfLength < time) {
                time = halfLength - time * 0.5f;
            } else {
                time *= 0.5f;
            }
            mAnimatedRoll = (time / halfLength) * mAnimatedRollDelta;
        }
    }

    if (mAnimatedShakeLength != 0.0f && mAnimatedShakeCycleRate != 0.0f) {
        float time = mAnimatedShakeTime + dT;
        mAnimatedShakeTime = time;
        if (mAnimatedShakeLength <= time) {
            mAnimatedShakeLength = 0.0f;
            mAnimatedShake = 0.0f;
            mAnimatedShakeDelta = 0.0f;
            mAnimatedShakeRamp = 0.0f;
        } else {
            float totalCycles = static_cast<float>(static_cast<int>(mAnimatedShakeLength / mAnimatedShakeCycleRate));
            float cycle = static_cast<float>(static_cast<int>(time / mAnimatedShakeCycleRate));
            float ramp = static_cast<float>(static_cast<int>(mAnimatedShakeRamp / mAnimatedShakeCycleRate + 0.99f));
            float scale = 1.0f;

            if (ramp > 0.0f) {
                if (ramp <= cycle) {
                    if (totalCycles - ramp < cycle) {
                        scale = (totalCycles - cycle) / ramp;
                    }
                } else {
                    scale = cycle / ramp;
                }
            }

            unsigned short angle =
                static_cast<unsigned short>(static_cast<int>(((time - mAnimatedShakeCycleRate * cycle) / mAnimatedShakeCycleRate) * 360.0f * 65536.0f) / 0x168);
            mAnimatedShake = mAnimatedShakeDelta * bCos(angle) * scale;
        }
    }
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

void SuspensionSpline::ApplyVehicleEntryForces(bool enteringVehicle, const UMath::Vector3 &pos, bool calledfromEvent) {}

float SuspensionSpline::GetWheelLoad(unsigned int i) const {
    return mTires[i]->GetLoad();
}

const float SuspensionSpline::GetWheelRoadHeight(unsigned int i) const {
    return mTires[i]->GetNormal().w;
}

float SuspensionSpline::GetCompression(unsigned int i) const {
    return mTires[i]->GetCompression();
}

bool SuspensionSpline::IsWheelOnGround(unsigned int i) const {
    return mTires[i]->IsOnGround();
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
