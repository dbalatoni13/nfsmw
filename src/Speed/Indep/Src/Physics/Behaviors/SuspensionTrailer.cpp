
#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/Wheel.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

#define EPSILON 0.000001f

#define MAKEATTRIB const

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
            return mRadius;
        }

        float GetAngularVelocity() const {
            return this->mAV;
        }
        void SetAngularVelocity(float av) {
            this->mAV = av;
        }

        float GetLoad() const {
            return this->mLoad;
        }

        float GetCurrentSlip() const {
            return this->mSlip;
        }

        float GetTraction() const {
            return mEBrake > 0.0f ? 0.0f : 1.0f;
        }

        void BeginFrame();
        void EndFrame(float dT);

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
        const int mAxleIndex;               // offset 0xFC, size 0x4
        bool mSlipping;                     // offset 0x100, size 0x1
        float mLateralSpeed;                // offset 0x104, size 0x4
        LastRotationSign mLastSign;         // offset 0x108, size 0x4
    };

    SuspensionTrailer(const BehaviorParams &bp, const SuspensionParams &sp);
    ~SuspensionTrailer() override;
    static Behavior *Construct(const BehaviorParams &params);

    // ISuspension
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
        return 1.0f;
    }
    Newtons GetWheelLoad(unsigned int i) const override {
        return this->mTires[i]->GetLoad();
    }
    float GetWheelSkid(unsigned int idx) const override {
        return this->mTires[idx]->GetLateralSpeed();
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
    float GetWheelSteer(unsigned int wheel) const override {
        return 0.0f;
    }
    float GetMaxSteering() const override {
        return 0.0f;
    }
    Angle GetWheelSlipAngle(unsigned int idx) const override {
        return this->GetVehicle()->GetSlipAngle();
    }

    // Behavior
    void OnTaskSimulate(float dT) override;
    void Reset() override;

  protected:
    void DoSimpleAero(State &state);
    void DoWheelForces(State &state);
#ifdef EA_BUILD_A124
    void DoWallSteer();
#endif

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    void ComputeState(float dT, State &state) const;

    Tire &GetWheel(unsigned int i) {
        return *mTires[i];
    }

    const Tire &GetWheel(unsigned int i) const {
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
    : Wheel(0),                          //
      mRadius(UMath::Max(radius, 0.1f)), //
      mBrake(0.0f),                      //
      mEBrake(0.0f),                     //
      mAV(0.0f),                         //
      mLoad(0.0f),                       //
      mLateralForce(0.0f),               //
      mLongitudeForce(0.0f),             //
      mAppliedTorque(0.0f),              //
      mSlip(0.0f),                       //
      mLastTorque(0.0f),                 //
      mWheelIndex(index),                //
      mRoadSpeed(0.0f),                  //
      mSpecs(specs),                     //
      mBrakes(brakes),                   //
      mAxleIndex(index >> 1),            //
      mSlipping(false),                  //
      mLateralSpeed(0.0f),               //
      mLastSign(WAS_ZERO) {}

void SuspensionTrailer::Tire::BeginFrame() {
    this->mAppliedTorque = 0.0f;
    this->SetForce(UMath::Vector3::kZero);
    this->mLateralForce = 0.0f;
    this->mLongitudeForce = 0.0f;
}

void SuspensionTrailer::Tire::EndFrame(float dT) {}

void SuspensionTrailer::Tire::UpdateFree(float dT) {
    this->mLoad = 0.0f;
    this->mSlip = 0.0f;
    this->mSlipping = false;
    if (this->mEBrake > 0.0f || this->mBrake > 0.0f) {
        this->mAV = 0.0f;
    }
    this->mLateralForce = 0.0f;
    this->mLongitudeForce = 0.0f;
}

float TrailerCorneringForceTable[7] = {0.0f, 0.4f, 0.8f, 1.2f, 1.6f, 1.75f, 1.65f};
Table TrailerCorneringForce(TrailerCorneringForceTable, 7, 0.0f, 1.0f / 3.0f);

void SuspensionTrailer::Tire::UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT) {
    const float brake_spec = FTLB2NM(this->mBrakes->BRAKES().At(this->mAxleIndex)) * 4.0f;
    const float ebrake_spec = FTLB2NM(this->mBrakes->EBRAKE()) * 10.0f;
    const float dynamicgrip_spec = this->mSpecs->DYNAMIC_GRIP().At(this->mAxleIndex);
    const float staticgrip_spec = this->mSpecs->STATIC_GRIP().At(this->mAxleIndex);

    this->mRoadSpeed = fwd_vel;
    this->mLateralSpeed = lat_vel;
    this->mLoad = UMath::Max(load, 0.0f);

    float bt = this->mBrake * brake_spec;
    float ebt = this->mEBrake * ebrake_spec;

    if (0.0f < fwd_vel) {
        bt = -bt;
    }
    this->ApplyTorque(bt);

    if (0.0f < fwd_vel) {
        ebt = -ebt;
    }
    this->ApplyTorque(ebt);

    float slip_angle = UMath::Atan2a(lat_vel, UMath::Abs(fwd_vel));
    this->mLongitudeForce = this->mAppliedTorque / this->mRadius;
    this->mSlip = this->mAV * this->mRadius - fwd_vel;

    this->mLateralForce = (this->mLoad * this->mSpecs->GRIP_SCALE().At(this->mAxleIndex)) * TrailerCorneringForce.GetValue(UMath::Abs(slip_angle));
    if (0.0f < lat_vel) {
        this->mLateralForce = -this->mLateralForce;
    }

    if (0.5f < this->mEBrake) {
        if (this->mAxleIndex == 1) {
            this->mLateralForce *= 0.75f;
        }
        this->mAV = 0.0f;
        this->mSlipping = true;
    } else {
        if (this->mAxleIndex == 1) {
            this->mLateralForce *= 1.5f;
        }
        this->mAV = fwd_vel / this->mRadius;
    }

    float len_force = UMath::Sqrt(this->mLateralForce * this->mLateralForce + this->mLongitudeForce * this->mLongitudeForce);
    float max_force = this->mLoad * staticgrip_spec;
    if (len_force > max_force && len_force > 0.001f) {
        float max_dynamic_force = this->mLoad * dynamicgrip_spec;
        float ratio = max_dynamic_force / len_force;
        this->mLateralForce *= ratio;
        this->mLongitudeForce *= ratio;
    }

    if (UMath::Abs(fwd_vel) > 1.0f) {
        this->mLongitudeForce -= UMath::Sina(slip_angle) * this->mLateralForce * 0.5f;
    } else {
        this->mLateralForce *= UMath::Min(UMath::Abs(lat_vel), 1.0f);
    }

    if (0.0f < this->mEBrake) {
        this->mAV = 0.0f;
        this->mSlip = -fwd_vel;
    }
}

BIND_BEHAVIOR_FACTORY(SuspensionTrailer);

Behavior *SuspensionTrailer::Construct(const BehaviorParams &params) {
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(UCRC32_BASE)));
    return new SuspensionTrailer(params, sp);
}

SuspensionTrailer::SuspensionTrailer(const BehaviorParams &bp, const SuspensionParams &sp)
    : Chassis(bp),                //
      mTireInfo(this, 0),         //
      mBrakeInfo(this, 0),        //
      mSuspensionInfo(this, 0),   //
      mRB(nullptr),               //
      mIDynamicsEntity(nullptr),  //
      mRBComplex(nullptr),        //
      mPowerSliding(false),       //
      mNumWheelsOnGround(0),      //
      mMotionDampingFactor(0.0f), //
      mSteeringControl(1.1f),     //
      mTimeInAir(0.0f) {
    mDriftPhysics = false;
    this->EnableProfile("SuspensionTrailer");

    this->GetOwner()->QueryInterface(&this->mRB);
    this->GetOwner()->QueryInterface(&this->mRBComplex);
    this->GetOwner()->QueryInterface(&this->mIDynamicsEntity);
    this->GetOwner()->QueryInterface(&this->mIDamage);

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

SuspensionTrailer::~SuspensionTrailer() {
    for (int i = 0; i < 4; ++i) {
        delete mTires[i];
    }
}

void SuspensionTrailer::OnBehaviorChange(const UCrc32 &mechanic) {
    Chassis::OnBehaviorChange(mechanic);
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mIDynamicsEntity);
        this->GetOwner()->QueryInterface(&this->mRBComplex);
        this->GetOwner()->QueryInterface(&this->mRB);
    } else if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        this->GetOwner()->QueryInterface(&this->mIDamage);
    }
}

void SuspensionTrailer::MatchSpeed(float speed) {
    for (int i = 0; i < 4; ++i) {
        float w = this->mTires[i]->GetRadius();
        this->mTires[i]->SetAngularVelocity(speed / w);
    }
}

void SuspensionTrailer::ComputeState(float dT, State &state) const {
    Chassis::ComputeState(dT, state);
    if ((this->mIDynamicsEntity != nullptr) && !Dynamics::Articulation::IsJoined(this->mIDynamicsEntity)) {
        state.ebrake_input = 0.0f;
        state.brake_input = 0.0f;
        state.gas_input = 0.0f;
        state.cog = UMath::Vector3::kZero;
        this->mRBComplex->SetCenterOfGravity(state.cog);
    } else {
        float fwbias = this->mSuspensionInfo->FRONT_WEIGHT_BIAS();
        float cg_z = 0.0f;
        float cg_y = INCH2METERS(this->mSuspensionInfo->ROLL_CENTER());
        float ride_height = UMath::Max(this->GetRideHeight(0), this->GetRideHeight(2));
        state.cog = UVector3(0.0f, cg_y - (state.dimension.y + ride_height), cg_z);
        this->mRBComplex->SetCenterOfGravity(state.cog);
    }
}

void SuspensionTrailer::OnTaskSimulate(float dT) {
    if ((this->mRB == nullptr) || (this->mRBComplex == nullptr)) {
        return;
    }
    ISimable *owner = GetOwner();
    State state;
    this->ComputeState(dT, state);
    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->BeginFrame();
    }
    this->DoSimpleAero(state);
    this->DoWheelForces(state);
    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->UpdateTime(dT);
    }
    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->EndFrame(dT);
    }
    if (GetNumWheelsOnGround() != 0) {
        this->mTimeInAir = 0.0f;
    } else {
        this->mTimeInAir += dT;
    }
    Chassis::OnTaskSimulate(dT);
}

UMath::Vector3 SuspensionTrailer::GetWheelCenterPos(unsigned int i) const {
    UMath::Vector3 pos = this->mTires[i]->GetPosition();
    if (this->mRBComplex == nullptr) {
        return pos;
    } else {
        UMath::ScaleAdd(this->mRBComplex->GetUpVector(), this->GetWheelRadius(i), pos, pos);
        return pos;
    }
}

void SuspensionTrailer::Reset() {
    ISimable *owner = this->GetOwner();
    IRigidBody &rigidBody = *owner->GetRigidBody();
    unsigned int numonground = 0;
    for (unsigned int i = 0; i < this->GetNumWheels(); ++i) {
        Tire &wheel = this->GetWheel(i);
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
    this->mNumWheelsOnGround = numonground;
    this->mTimeInAir = 0.0f;
    this->mMotionDampingFactor = 0.0f;
}

void SuspensionTrailer::DoSimpleAero(State &state) {
    const float dragcoef_spec = this->mSuspensionInfo->DRAG_COEFFICIENT();
    float speed = state.speed;
    float drag = -(speed * dragcoef_spec);
    UVector3 drag_vector = state.linear_vel;
    drag_vector *= drag;
    this->mRB->ResolveForce(drag_vector);
}

// UNSOLVED, float math
void SuspensionTrailer::DoWheelForces(State &state) {
    const float dT = state.time;

    for (int i = 0; i < 4; ++i) {
        this->mTires[i]->SetBrake(state.brake_input);
        this->mTires[i]->SetEBrake(state.ebrake_input);
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

    bool resolve = false;

    for (unsigned int i = 0; i < 4; ++i) {
        int axle = i >> 1;
        Tire &wheel = this->GetWheel(i);
        UMath::Vector3 wp = wheel.GetWorldArm();

        wheel.UpdatePosition(state.angular_vel, state.linear_vel, state.matrix, cog, state.time, wheel.GetRadius(), true, state.collider,
                             state.dimension.y * 2.0f);

        const UVector3 groundNormal(wheel.GetNormal());
        const UVector3 forwardNormal(vFwd);
        UVector3 lateralNormal;
        UMath::UnitCross(groundNormal, forwardNormal, lateralNormal);

        float penetration = wheel.GetNormal().w;
        float upness = UMath::Clamp(UMath::Dot(vUp, groundNormal), 0.0f, 1.0f);
        const float oldCompression = wheel.GetCompression();
        float newCompression = rideheight_specs[axle] * upness + penetration;
        float max_compression = travel_specs[axle];

        if (wheel.GetCompression() == 0.0f) {
            float delta = newCompression - max_compression;
            maxDelta = UMath::Max(maxDelta, delta);
        }

        newCompression = UMath::Max(newCompression, 0.0f);
        if (newCompression > max_compression) {
            maxDelta = UMath::Max(maxDelta, newCompression - max_compression);
            newCompression = max_compression;
        }

        if (newCompression > 0.0f && upness > VehicleSystem::ENABLE_ROLL_STOPS_THRESHOLD) {
            ++wheelsOnGround;

            float springForce = newCompression * spring_specs[axle];
            const float diff = newCompression - wheel.GetCompression();
            const float rise = diff / dT;
            float spring = springForce * (newCompression * progression[axle] + 1.0f);
            float damp = rise * shock_specs[axle];

            if (damp > this->mSuspensionInfo->SHOCK_BLOWOUT() * (mass * 9.81f)) {
                damp = 0.0f;
            }

            float load = damp + spring + sway_stiffness[i];
            load = UMath::Max(load, 0.0f);

            UVector3 verticalForce = UVector3(vUp) * load;
            UVector3 driveForce;
            UVector3 lateralForce;

            float d2 = UMath::Max(0.3f, upness * 4.0f - 3.0f);
            load *= d2;

            const UMath::Vector3 &pointVelocity = wheel.GetVelocity();
            UVector3 vNorm(pointVelocity);
            float speed = vNorm.Normalize();
            float vdot = UMath::Dot(vNorm, lateralNormal);
            float xspeed = UMath::Dot(pointVelocity, lateralNormal);
            float zspeed = UMath::Dot(pointVelocity, forwardNormal);

            wheel.UpdateLoaded(xspeed, zspeed, load, state.time);

            float traction_force = wheel.GetLateralForce();
            float max_traction = xspeed / dT;
            max_traction *= 0.25f;
            max_traction *= mass;
            max_traction = UMath::Abs(max_traction);
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
            UVector3 p = wheel.GetLocalArm();
            p.y += wheel.GetCompression();
            p.y -= rideheight_specs[i >> 1];

            const UMath::Vector3 &force = wheel.GetForce();

            UMath::RotateTranslate(p, state.matrix, p);
            wheel.SetPosition(p);

            UMath::Vector3 torque;
            UMath::Vector3 r;
            UMath::Sub(p, cg, r);
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
