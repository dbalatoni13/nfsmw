#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

float Chassis::ComputeMaxSlip(const Chassis::State &state) const {
    float ramp = UMath::Ramp(state.speed, 10.0f, 71.0f);
    float result = ramp + 0.5f;
    if (state.gear == G_REVERSE)
        result = 71.0f;
    return result;
}

float GripVsSpeed[] = {0.833f, 0.958f, 1.008f, 1.0167f, 1.033f, 1.033f, 1.033f, 1.0167f, 1.0f, 1.0f};
Table GripRangeTable(GripVsSpeed, 10, 0.0f, 1.0f);

// Credits: Brawltendo
float Chassis::ComputeLateralGripScale(const Chassis::State &state) const {
    // lateral grip is tripled when in a drag race
    if (state.driver_style == STYLE_DRAG) {
        return 3.0f;
    }

    float ratio = UMath::Ramp(state.speed, 0.0f, MPH2MPS(85.0f));
    return GripRangeTable.GetValue(ratio) * 1.2f;
}

float TractionVsSpeed[] = {0.90899998f, 1.045f, 1.09f, 1.09f, 1.09f, 1.09f, 1.09f, 1.045f, 1.0f, 1.0f};
Table TractionRangeTable(TractionVsSpeed, 10, 0.0f, 1.0f);

// Credits: Brawltendo
// TODO doesn't match on GC yet
float Chassis::ComputeTractionScale(const Chassis::State &state) const {
    float result;
    if (state.driver_style == STYLE_DRAG) {
        result = 1.1f;
    } else {
        float ratio = UMath::Ramp(state.speed, 0.0f, MPH2MPS(85.0f));
        result = TractionRangeTable.GetValue(ratio) * 1.1f;
    }

    // traction is doubled when in reverse
    if (state.gear == G_REVERSE) {
        result = 2.0f;
    }

    return result;
}

// Credits:: Brawltendo
// UNSOLVED and TODO variable names aren't dwarf-matching
void Chassis::ComputeAckerman(const float steering, const Chassis::State &state, UMath::Vector4 *left, UMath::Vector4 *right) const {
    int going_right = true;
    float wheel_base = mAttributes.WHEEL_BASE();
    float track_width_front = mAttributes.TRACK_WIDTH().Front;
    float steering_angle_radians = steering * (float)M_TWOPI;

    // clamp steering angle <= 180 degrees
    if (steering_angle_radians > (float)M_PI)
        steering_angle_radians -= (float)M_TWOPI;

    // negative steering angle indicates a left turn
    if (steering_angle_radians < 0.0f) {
        going_right = false;
        steering_angle_radians = -steering_angle_radians;
    }

    // Ackermann steering geometry causes the outside wheel to have a smaller turning angle than the inside wheel
    // this is determined by the distance of the wheel to the center of the rear axle
    // this equation is a modified version of 1/tan(L/(R+T/2)), where L is the wheelbase, R is the steering radius, and T is the track width
    float steer_left;
    float steer_right;
    float steer_outside = (steering_angle_radians * wheel_base) / (steering_angle_radians * track_width_front + wheel_base);
    if (going_right) {
        steer_left = steer_outside;
        steer_right = steering_angle_radians;
    } else {
        steer_left = -steering_angle_radians;
        steer_right = -steer_outside;
    }

    // calculate forward vector for front wheels
    UMath::Vector3 r;
    r.z = cosf(steer_right);
    r.x = sinf(steer_right);
    r.y = 0.0f;
    UMath::Rotate(r, state.matrix, r);
    *right = UMath::Vector4Make(r, steer_right);

    UMath::Vector3 l;
    l.z = cosf(steer_left);
    l.x = sinf(steer_left);
    l.y = 0.0f;
    UMath::Rotate(l, state.matrix, l);
    *left = UMath::Vector4Make(l, steer_left);
}

static float AeroDropOff = 0.5f;
static float AeroDropOffMin = 0.4f;
static float OffThrottleDragFactor = 2.0f;
static float OffThrottleDragCenterHeight = -0.1f;

// Credits: Brawltendo
// TODO not matching on GC yet
void Chassis::DoAerodynamics(const Chassis::State &state, float drag_pct, float aero_pct, float aero_front_z, float aero_rear_z,
                             const Physics::Tunings *tunings) {
    IRigidBody *irb = this->GetOwner()->GetRigidBody();

    if (drag_pct > 0.0f) {
        const float dragcoef_spec = mAttributes.DRAG_COEFFICIENT();
        // drag increases relative to the car's speed
        // letting off the throttle will increase drag by OffThrottleDragFactor
        float drag = (dragcoef_spec * state.speed * drag_pct) * ((OffThrottleDragFactor - 1.0f) * (1.0f - state.gas_input) + 1.0f);
        if (tunings)
            drag *= tunings->aerodynamicsTuning * 0.25f + 1.0f;

        UVector3 drag_vector(state.linear_vel);
        drag_vector *= -drag;
        UVector3 drag_center(state.cog);

        // lower drag vertical pos based on off-throttle amount as long as 2 or more wheels are grounded
        if (state.ground_effect >= 0.5f)
            drag_center.y += OffThrottleDragCenterHeight * (1.0f - state.gas_input);

        UMath::RotateTranslate(drag_center, state.matrix, drag_center);
        irb->ResolveForceAtPoint(drag_vector, drag_center);
    }

    if (aero_pct > 0.0f) {
        // this should really be an inlined UMath::Max but for some reason it doesn't wanna generate correctly
        // so instead I manually inlined it
        float upness_temp = UMath::Max(state.GetUpVector().y, 0.0f);
        // scale downforce by the gradient when less than 2 wheels are grounded
        float upness = upness_temp;
        if (state.ground_effect >= 0.5f)
            upness = 1.0f;

        // in reverse, the car's forward vector is used as the movement direction
        UVector3 movement_dir(state.GetForwardVector());
        if (state.speed > 0.0001f) {
            // TODO
            // movement_dir = state.linear_vel;
            movement_dir.x = state.linear_vel.x;
            movement_dir.y = state.linear_vel.y;
            movement_dir.z = state.linear_vel.z;
            movement_dir *= 1.0f / state.speed;
        }

        float forwardness = UMath::Max(UMath::Dot(movement_dir, state.GetForwardVector()), 0.0f);
        float drop_off = UMath::Max(AeroDropOffMin, UMath::Pow(forwardness, AeroDropOff));
        float downforce = Physics::Info::AerodynamicDownforce(mAttributes, state.speed) * drop_off * upness * aero_pct;
        // lower downforce when car is in air
        if (state.ground_effect == 0.0f) {
            downforce *= 0.8f;
        }
        if (tunings) {
            downforce *= tunings->aerodynamicsTuning * 0.25f + 1.0f;
        }

        if (downforce > 0.0f) {
            UVector3 aero_center(state.cog.x, state.cog.y, state.cog.z);
            // when at least 1 wheel is grounded, change the downforce forward position using the aero CG and axle positions
            if (state.ground_effect != 0.0f) {
                // TODO change according to the dwarf
                aero_center.z = (aero_front_z - aero_rear_z) * (mAttributes.AERO_CG() * 0.01f) + aero_rear_z;
            }

            UVector3 force(0.0f, -downforce, 0.0f);
            UMath::RotateTranslate(aero_center, state.matrix, aero_center);
            UMath::Rotate(force, state.matrix, force);
            irb->ResolveForceAtPoint(force, aero_center);
        }
    }
}

static float LowSpeedSpeed = 0.0f;
static float HighSpeedSpeed = 30.0f;
static float MaxYawBonus = 0.35f;
static float LowSpeedYawBoost = 0.0f;
static float HighSpeedYawBoost = 1.0f;
static float YawEBrakeThreshold = 0.5f;
static float YawAngleThreshold = 20.0f;

// Credits: Brawltendo
float YawFrictionBoost(float yaw, float ebrake, float speed, float yawcontrol, float grade) {
    yaw = bAbs(yaw);
    float retval = 1.0f;
    retval += bAbs(grade);
    if (ebrake > YawEBrakeThreshold && yaw < bDegToRad(YawAngleThreshold))
        return retval;

    float speed_factor = (speed - LowSpeedSpeed) / (HighSpeedSpeed - LowSpeedSpeed);
    float boost = LowSpeedYawBoost + (HighSpeedYawBoost - LowSpeedYawBoost) * speed_factor;
    float bonus = yaw * yawcontrol * boost;
    if (bonus > MaxYawBonus)
        bonus = MaxYawBonus;
    return retval + bonus;
}
