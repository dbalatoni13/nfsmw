#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

Chassis::Chassis(const BehaviorParams &bp)
    : VehicleBehavior(bp, 0), ISuspension(bp.fowner), mAttributes(this, 0), mJumpTime(0.0f), mJumpAlititude(0.0f), mTireHeat(0.0f) {
    this->GetOwner()->QueryInterface(&this->mRBComplex);
    this->GetOwner()->QueryInterface(&this->mInput);
    this->GetOwner()->QueryInterface(&this->mEngine);
    this->GetOwner()->QueryInterface(&this->mTransmission);
    this->GetOwner()->QueryInterface(&this->mDragTrany);
    this->GetOwner()->QueryInterface(&this->mEngineDamage);
    this->GetOwner()->QueryInterface(&this->mSpikeDamage);
}

Meters Chassis::GuessCompression(unsigned int id, Newtons downforce) const {
    float compression = 0.0f;
    if (downforce < 0.0f) {
        unsigned int axle = id / 2;
        float spring_weight = LBIN2NM(this->mAttributes.SPRING_STIFFNESS().At(axle));
        downforce *= 0.25f;
        compression = -downforce / spring_weight;
    }
    return compression;
}

float Chassis::GetRenderMotion() const {
    return this->mAttributes.RENDER_MOTION();
}

Meters Chassis::GetRideHeight(unsigned int idx) const {
    return INCH2METERS(this->mAttributes.RIDE_HEIGHT().At(idx / 2));
}

static const float MinForwardSpeedForUndersteer = 1.0f;
static const float MinForwardSpeedForOversteer = 1.0f;

float Chassis::CalculateUndersteerFactor() const {
    float magnitude = 0.0f;
    float slip_avg = (this->GetWheelSkid(0) + this->GetWheelSkid(1)) / 2.0f;
    float steer = (this->GetWheelSteer(0) + this->GetWheelSteer(1)) / 2.0f;
    float speed = this->GetOwner()->GetRigidBody()->GetSpeed();
    if ((this->GetVehicle()->GetSpeed() > 0.0f) && (speed > MinForwardSpeedForUndersteer) && (steer * slip_avg < 0.0f)) {
        magnitude = UMath::Abs(slip_avg) / speed;
    }
    return UMath::Min(magnitude, 1.0f);
}

static const float Tweak_TireHeatTime = 3.0f;
static const float Tweak_TireUnHeatTime = 6.0f;
static const float Tweak_TireHeatToSlip = 80.0f;

static const float Tweak_MaxStaticSlip = 1.5f;
static const float Tweak_MinStaticSlip = 0.5f;
static const float Tweak_MaxStaticSlipSpeed = 71.0f;
static const float Tweak_MinStaticSlipSpeed = 10.0f;

Mps Chassis::ComputeMaxSlip(const Chassis::State &state) const {
    float ramp = UMath::Ramp(state.speed, Tweak_MinStaticSlipSpeed, Tweak_MaxStaticSlipSpeed);
    float result = ramp + Tweak_MinStaticSlip;
    if (state.gear == G_REVERSE)
        result = Tweak_MaxStaticSlipSpeed;
    return result;
}

void Chassis::DoTireHeat(const Chassis::State &state) {
    if (state.flags & 1) {
        for (unsigned int i = 0; i < this->GetNumWheels(); ++i) {
            if (this->GetWheelSlip(i) > Tweak_MinStaticSlip) {
                this->mTireHeat += state.time / Tweak_TireHeatTime;
                this->mTireHeat = UMath::Min(this->mTireHeat, 1.0f);
                return;
            }
        }
    } else {
        if (this->mTireHeat > 0.0f) {
            this->mTireHeat -= state.time / Tweak_TireUnHeatTime;
            this->mTireHeat = UMath::Max(this->mTireHeat, 0.0f);
        }
    }
}

float Chassis::CalculateOversteerFactor() const {
    float speed = this->GetOwner()->GetRigidBody()->GetSpeed();
    float magnitude = 0.0f;
    if ((this->GetVehicle()->GetSpeed() > 0.0f) && (speed > MinForwardSpeedForOversteer)) {
        magnitude = UMath::Abs((this->GetWheelSkid(3) + this->GetWheelSkid(2)) * 0.5f) / speed;
    }
    return UMath::Min(magnitude, 1.0f);
}

void Chassis::OnTaskSimulate(float dT) {}

static const float Tweak_DragGripBoost = 3.0f;
static const float Tweak_DragTractionBoost = 1.1f;
float TractionVsSpeed[] = {0.909f, 1.045f, 1.09f, 1.09f, 1.09f, 1.09f, 1.09f, 1.045f, 1.0f, 1.0f};

Table TractionRangeTable(TractionVsSpeed, 10, 0.0f, 1.0f);

float GripVsSpeed[] = {0.833f, 0.958f, 1.008f, 1.0167f, 1.033f, 1.033f, 1.033f, 1.0167f, 1.0f, 1.0f};

Table GripRangeTable(GripVsSpeed, 10, 0.0f, 1.0f);

static const float Traction_RangeMaxSpeedMPH = 85.0f;

static const float Tweak_GlobalGripScale = 1.2f;
static const float Tweak_GlobalTractionScale = 1.1f;
static const float Tweak_ReverseTractionScale = 2.0f;

// Credits: Brawltendo
float Chassis::ComputeLateralGripScale(const Chassis::State &state) const {
    // lateral grip is multiplied when in a drag race
    if (state.driver_style == STYLE_DRAG) {
        return Tweak_DragGripBoost;
    }

    float ratio = UMath::Ramp(state.speed, 0.0f, MPH2MPS(Traction_RangeMaxSpeedMPH));
    return GripRangeTable.GetValue(ratio) * Tweak_GlobalGripScale;
}

// Credits: Brawltendo
float Chassis::ComputeTractionScale(const Chassis::State &state) const {
    float result = 1.0f;

    if (state.driver_style == STYLE_DRAG) {
        result = Tweak_DragTractionBoost;
    } else {
        float ratio = UMath::Ramp(state.speed, 0.0f, MPH2MPS(Traction_RangeMaxSpeedMPH));
        result = TractionRangeTable.GetValue(ratio) * Tweak_GlobalTractionScale;
    }

    if (state.gear == G_REVERSE) {
        result = Tweak_ReverseTractionScale;
    }

    return result;
}

// UNSOLVED small regswaps
Chassis::SleepState Chassis::DoSleep(const Chassis::State &state) {
    if (state.flags & 1) {
        return SS_NONE;
    }
    IRigidBody *irb = this->GetOwner()->GetRigidBody();
    if (state.speed < 0.5f) {
        if ((this->GetNumWheelsOnGround() == this->GetNumWheels()) && (state.brake_input + state.ebrake_input > 0.0f) && (state.gas_input == 0.0f)) {
            if ((UMath::Length(state.angular_vel) < 0.25f) && (!this->mRBComplex->HasHadCollision())) {
                if (state.speed < UMath::Epsilon) {
                    this->mRBComplex->Damp(1.0f);
                } else {
                    this->mRBComplex->Damp(1.0f - state.speed);
                }
                for (unsigned int i = 0; i < this->GetNumWheels(); ++i) {
                    this->SetWheelAngularVelocity(i, 0.0f);
                }
                return SS_ALL;
            }
        }
    }
    if (state.speed < 1.0f) {
        if ((UMath::Length(state.angular_vel) < 0.25f) && (state.gas_input <= 0.0f)) {
            UMath::Vector3 v = state.local_vel;
            UMath::Vector3 w = state.local_angular_vel;
            UMath::Vector3 f = this->mRBComplex->GetForce();
            UMath::Vector3 t = this->mRBComplex->GetTorque();
            irb->ConvertWorldToLocal(f, false);
            irb->ConvertWorldToLocal(t, false);

            v.x *= state.speed;
            w.y *= state.speed;
            f.x = -f.x * (1.0f - state.speed);
            t.y = -t.y * (1.0f - state.speed);

            UMath::Rotate(v, state.matrix, v);
            UMath::Rotate(w, state.matrix, w);
            UMath::Rotate(t, state.matrix, t);
            UMath::Rotate(f, state.matrix, f);

            irb->Resolve(f, t);
            irb->SetLinearVelocity(v);
            irb->SetAngularVelocity(w);
            return SS_LATERAL;
        }
    }
    return SS_NONE;
}

void Chassis::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        this->GetOwner()->QueryInterface(&this->mTransmission);
        this->GetOwner()->QueryInterface(&this->mEngine);
        this->GetOwner()->QueryInterface(&this->mDragTrany);
        this->GetOwner()->QueryInterface(&this->mEngineDamage);
    } else if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mInput);
    } else if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mRBComplex);
    } else if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        this->GetOwner()->QueryInterface(&this->mSpikeDamage);
    }
}

// Credits: Brawltendo
void Chassis::ComputeAckerman(float steering, const Chassis::State &state, UMath::Vector4 *left, UMath::Vector4 *right) const {
    int going_right = TRUE;
    float wheelbase = this->mAttributes.WHEEL_BASE();
    float wheeltrack = this->mAttributes.TRACK_WIDTH().Front;
    float steer_inside = ANGLE2RAD(steering);

    // clamp steering angle <= 180 degrees
    if (steer_inside > UMath::PI)
        steer_inside -= UMath::TWOPI;

    // negative steering angle indicates a left turn
    if (steer_inside < 0.0f) {
        going_right = FALSE;
        steer_inside = -steer_inside;
    }

    // Ackermann steering geometry causes the outside wheel to have a smaller turning angle than the inside wheel
    // this is determined by the distance of the wheel to the center of the rear axle
    // this equation is a modified version of 1/tan(L/(R+T/2)), where L is the wheelbase, R is the steering radius, and T is the track width
    float steer_outside = (wheelbase * steer_inside) / (wheeltrack * steer_inside + wheelbase);
    float steer0, steer1; // 0 - right, 1 - left
    if (going_right) {
        steer0 = steer_inside;
        steer1 = steer_outside;
    } else {
        steer1 = -steer_inside;
        steer0 = -steer_outside;
    }

    float ca, sa;
    // calculate forward vector for front wheels
    UMath::Vector3 r;
    ca = cosf(steer0);
    sa = sinf(steer0);
    r.z = ca;
    r.x = sa;
    r.y = 0.0f;
    UMath::Rotate(r, state.matrix, r);
    *right = UMath::Vector4Make(r, steer0);

    UMath::Vector3 l;
    ca = cosf(steer1);
    sa = sinf(steer1);
    l.z = ca;
    l.x = sa;
    l.y = 0.0f;
    UMath::Rotate(l, state.matrix, l);
    *left = UMath::Vector4Make(l, steer1);
}

void Chassis::SetCOG(float extra_bias, float extra_ride) {
    float front_z = this->mAttributes.FRONT_AXLE();
    float rear_z = front_z - this->mAttributes.WHEEL_BASE();
    IRigidBody *irb = this->GetOwner()->GetRigidBody();
    float dim_y = irb->GetDimension().y;

    float fwbias = (this->mAttributes.FRONT_WEIGHT_BIAS() + extra_bias) * 0.01f;
    if (this->GetNumWheelsOnGround() == 0) {
        fwbias = 0.5f;
    }
    float cg_z = (front_z - rear_z) * fwbias + rear_z;
    float cg_y = INCH2METERS(this->mAttributes.ROLL_CENTER()) - (dim_y + UMath::Max(INCH2METERS(this->mAttributes.RIDE_HEIGHT().At(0) + extra_ride),
                                                                                    INCH2METERS(this->mAttributes.RIDE_HEIGHT().At(1) + extra_ride)));
    UVector3 cog(0.0f, cg_y, cg_z);
    this->mRBComplex->SetCenterOfGravity(cog);
    return;
}

void Chassis::ComputeState(float dT, Chassis::State &state) const {
    IRigidBody *irb = this->GetOwner()->GetRigidBody();
    state.time = dT;
    state.flags = 0;
    state.collider = irb->GetWCollider();
    state.inertia = this->mRBComplex->GetInertiaTensor();
    state.dimension = irb->GetDimension();

    irb->GetMatrix4(state.matrix);
    state.matrix.v3 = UMath::Vector4Make(irb->GetPosition(), 1.0f);

    state.local_vel = irb->GetLinearVelocity();
    state.linear_vel = state.local_vel;
    irb->ConvertWorldToLocal(state.local_vel, false);
    state.angular_vel = irb->GetAngularVelocity();
    state.local_angular_vel = state.angular_vel;
    irb->ConvertWorldToLocal(state.local_angular_vel, false);

    state.speed = UMath::Length(state.linear_vel);
    if (state.local_vel.z < 1.0f) {
        state.slipangle = 0.0f;
    } else {
        state.slipangle = UMath::Atan2a(state.local_vel.x, state.local_vel.z);
    }

    const InputControls &controls = this->mInput->GetControls();
    state.gas_input = UMath::Clamp(controls.fGas, 0.0f, 1.0f);
    state.brake_input = UMath::Clamp(controls.fBrake, 0.0f, 1.0f);
    state.ebrake_input = controls.fHandBrake;
    state.steer_input = UMath::Clamp(controls.fSteering, -1.0f, 1.0f);

    state.cog = this->mRBComplex->GetCenterOfGravity();
    state.ground_effect = this->GetNumWheelsOnGround() * 0.25f;
    state.mass = irb->GetMass();
    state.driver_style = this->GetVehicle()->GetDriverStyle();
#ifndef EA_BUILD_A124
    state.driver_class = this->GetVehicle()->GetDriverClass();
#endif

    if (this->GetVehicle()->IsStaging()) {
        state.flags |= State::IS_STAGING;
    }

    if (this->mEngine != nullptr) {
        state.nos_boost = this->mEngine->GetNOSBoost();
    } else {
        state.nos_boost = 1.0f;
    }
    if (this->mTransmission != nullptr) {
        state.gear = mTransmission->GetGear();
    } else {
        state.gear = G_NEUTRAL;
    }
    if (this->mDragTrany != nullptr) {
        state.shift_boost = this->mDragTrany->GetShiftBoost();
    } else {
        state.shift_boost = 1.0f;
    }

    if ((this->mEngineDamage != nullptr) && this->mEngineDamage->IsBlown() || GetVehicle()->IsDestroyed()) {
        state.brake_input = 1.0f;
        state.gas_input = 0.0f;
        state.ebrake_input = 1.0f;
    }

    UMath::Rotate(state.cog, state.matrix, state.world_cog);

    state.blown_tires = 0;
    if (this->mSpikeDamage != nullptr) {
        unsigned int num_wheels = this->GetNumWheels();
        for (unsigned int i = 0; i < num_wheels; ++i) {
            if (this->mSpikeDamage->GetTireDamage(i) == TIRE_DAMAGE_BLOWN) {
                state.blown_tires |= (1 << i);
            }
        }
    }

    if (this->GetVehicle()->IsDestroyed()) {
        state.flags |= State::IS_DESTROYED;
    }
}

static float AeroDropOff = 0.5f;
static float AeroDropOffMin = 0.4f;
static float OffThrottleDragFactor = 2.0f;
static float OffThrottleDragCenterHeight = -0.1f;
static const float Tweak_TuningAero_Drag = 0.25f;
static const float Tweak_TuningAero_DownForce = 0.25f;
static const float Tweak_PlaneDynamics = 0.0f;

// Credits: Brawltendo
// TODO stack frame is slightly off
void Chassis::DoAerodynamics(const Chassis::State &state, float drag_pct, float aero_pct, float aero_front_z, float aero_rear_z,
                             const Physics::Tunings *tunings) {
    IRigidBody *irb = this->GetOwner()->GetRigidBody();

    if (drag_pct > 0.0f) {
        const float dragcoef_spec = this->mAttributes.DRAG_COEFFICIENT();
        // drag increases relative to the car's speed
        // letting off the throttle will increase drag by OffThrottleDragFactor
        float drag = state.speed * drag_pct * dragcoef_spec;
        drag += drag * (OffThrottleDragFactor - 1.0f) * (1.0f - state.gas_input);
        if (tunings != nullptr) {
            drag += drag * Tweak_TuningAero_Drag * tunings->Value[Physics::Tunings::AERODYNAMICS];
        }

        UVector3 drag_vector(state.linear_vel);
        drag_vector *= -drag;
        UVector3 drag_center(state.cog);

        // manipulate drag height based on off-throttle amount when 2 or more wheels are grounded
        if (state.ground_effect >= 0.5f)
            drag_center.y += OffThrottleDragCenterHeight * (1.0f - state.gas_input);

        UMath::RotateTranslate(drag_center, state.matrix, drag_center);
        irb->ResolveForce(drag_vector, drag_center);
    }

    if (aero_pct > 0.0f) {
        // scale downforce by the gradient when less than 2 wheels are grounded
        float upness = UMath::Max(state.GetUpVector().y, 0.0f);
        if (state.ground_effect >= 0.5f)
            upness = 1.0f;

        // in reverse, the car's forward vector is used as the movement direction
        UVector3 movement_dir(state.GetForwardVector());
        if (state.speed > 0.0001f) {
            movement_dir = state.linear_vel;
            movement_dir *= 1.0f / state.speed;
        }

        float forwardness = UMath::Max(UMath::Dot(movement_dir, state.GetForwardVector()), 0.0f);
        forwardness = UMath::Max(AeroDropOffMin, UMath::Pow(forwardness, AeroDropOff));
        float downforce = aero_pct * upness * forwardness * Physics::Info::AerodynamicDownforce(this->mAttributes, state.speed);
        // lower downforce when car is in air
        if (state.ground_effect == 0.0f) {
            downforce *= 0.8f;
        }
        if (tunings != nullptr) {
            downforce += downforce * Tweak_TuningAero_DownForce * tunings->Value[Physics::Tunings::AERODYNAMICS];
        }

        if (downforce > 0.0f) {
            UVector3 aero_center(state.cog.x, state.cog.y, state.cog.z);
            // when at least 1 wheel is grounded, change the downforce forward position using the aero CG and axle positions
            if (state.ground_effect != 0.0f) {
                aero_center.z = (aero_front_z - aero_rear_z) * (this->mAttributes.AERO_CG() * 0.01f) + aero_rear_z;
            }

            if (Tweak_PlaneDynamics != 0.0f) {
                // just some random nonsense because the DWARF says there was a block here
                float pitch = UMath::Atan2a(UMath::Abs(state.matrix.v2.z), state.matrix.v2.x);
                aero_center.z *= pitch;
            }

            UVector3 force(0.0f, -downforce, 0.0f);
            UMath::RotateTranslate(aero_center, state.matrix, aero_center);
            UMath::Rotate(force, state.matrix, force);
            irb->ResolveForce(force, aero_center);
        }
    }
}

static const int bJumpStabilizer = 1;

static const int bPassiveStabilizer = 1;
bVector2 JumpStabilizationGraph[] = {bVector2(0.0f, 0.0f), bVector2(0.4f, 0.15f), bVector2(2.0f, 5.0f)};
Graph JumpStabilization(JumpStabilizationGraph, 3);

static const int bActiveStabilizer = 1;
static const float fPitchStabilizerAction = 40.0f;
static const float fRollStabilizerAction = 20.0f;
static const float fStablizationAltitude = 15.0f;
static const float fStabilizerUp = 0.8f;
static const float fStabilizerSpeed = 5.0f;
static const float fStabilizerMaxAngVel = 0.0f;
static const int bDoLandingGravity = 1;
static const float fExtraLandingGravity = 3.0f;
static const float fLandingGravitySpeed = 20.0f;
static const float fLandingGravityUpThreshold = 0.96f;
static const float fLandingGravityMinTime = 1.0f;
static const float fLandingGravityMinAltitude = 2.0f;
static const float fLandingGravityMaxAltitude = 6.0f;

void Chassis::DoJumpStabilizer(const Chassis::State &state) {
    if (!bJumpStabilizer || (this->mRBComplex == nullptr)) {
        return;
    }

    int nTouching = this->GetNumWheelsOnGround();
    bool resolve = false;
    UMath::Vector4 ground_normal = this->mRBComplex->GetGroundNormal();
    float altitude = -ground_normal.w;
    float ground_dot = UMath::Dot(state.GetUpVector(), UMath::Vector4To3(ground_normal));

    UMath::Vector3 damping_torque = UMath::Vector3::kZero;
    UMath::Vector3 damping_force = UMath::Vector3::kZero;

    if (!nTouching) {
        this->mJumpTime += state.time;
        this->mJumpAlititude = UMath::Max(this->mJumpAlititude, altitude);

        if (bDoLandingGravity) {
            float accel = fExtraLandingGravity;
            // apply more downforce when the car has been airborne for a long time
            if (this->mJumpTime > fLandingGravityMinTime && ground_dot > fLandingGravityUpThreshold && this->mJumpAlititude > fLandingGravityMinAltitude &&
                state.linear_vel.y < 0.0f && altitude < fLandingGravityMaxAltitude) {
                float alt_ratio = 1.0f - UMath::Ramp(altitude, 0.0f, fLandingGravityMaxAltitude);
                float speed_ratio = UMath::Ramp(state.speed, 0.0f, fLandingGravitySpeed);
                accel += alt_ratio * 10.0f * speed_ratio;
            }

            UVector3 df_extra(0.0f, -state.mass * accel, 0.0f);
            UMath::Add(damping_force, df_extra, damping_force);
            resolve = true;
        }
    } else {
        mJumpTime = 0.0f;
        mJumpAlititude = 0.0f;
    }

    if (bJumpStabilizer && nTouching < 2 && state.GetUpVector().y > fStabilizerUp && !this->mRBComplex->IsInGroundContact()) {
        float speed_ramp = UMath::Ramp(state.speed, 0.0f, fStabilizerSpeed);
        float avelmag = UMath::Length(state.local_angular_vel);
        float damping = speed_ramp * JumpStabilization.GetValue(avelmag);

        UMath::Vector3 damping_moment;
        UMath::Scale(state.local_angular_vel, state.inertia, damping_moment);
        UMath::Scale(damping_moment, -damping, damping_moment);
        damping_moment.y = 0.0f;
        UMath::Rotate(damping_moment, state.matrix, damping_moment);
        UMath::Add(damping_moment, damping_torque, damping_torque);
        resolve = true;
    }

    if (bActiveStabilizer && nTouching == 0 && ground_normal.y > 0.9f && state.GetUpVector().y > 0.1f && ground_dot > 0.8f &&
        altitude > UMath::Epsilon) {
        float altitude_ramp = 1.0f - UMath::Ramp(altitude, 0.0f, fStablizationAltitude);
        float speed_ramp = UMath::Ramp(state.speed, 0.0f, fStabilizerSpeed);

        UMath::Vector3 vMoment;
        UMath::Vector3 vFlatFwd;
        UMath::UnitCross(state.GetRightVector(), UMath::Vector4To3(ground_normal), vFlatFwd);
        float dot = UMath::Dot(vFlatFwd, state.GetForwardVector());
        if (dot < 0.99f) {
            UMath::UnitCross(vFlatFwd, state.GetForwardVector(), vMoment);
            float fMag = state.mass * speed_ramp * altitude_ramp * fPitchStabilizerAction * (dot - 1.0f);
            UMath::ScaleAdd(vMoment, fMag, damping_torque, damping_torque);
            resolve = true;
        }

        UMath::Vector3 vFlatRight;
        UMath::UnitCross(UMath::Vector4To3(ground_normal), state.GetForwardVector(), vFlatRight);
        dot = UMath::Dot(vFlatRight, state.GetRightVector());
        if (dot < 0.99f) {
            UMath::UnitCross(vFlatRight, state.GetRightVector(), vMoment);
            float fMag = state.mass * speed_ramp * fRollStabilizerAction * altitude_ramp * (dot - 1.0f);
            UMath::ScaleAdd(vMoment, fMag, damping_torque, damping_torque);
            resolve = true;
        }
    }

    if (resolve) {
        IRigidBody *irb = GetOwner()->GetRigidBody();
        irb->Resolve(damping_force, damping_torque);
    }
}
