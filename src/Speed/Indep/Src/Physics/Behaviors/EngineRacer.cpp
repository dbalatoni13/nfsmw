#include "EngineRacer.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Sim/UTil.h"

// Credits: Brawltendo
// UNSOLVED
float EngineRacer::GetBrakingTorque(float engine_torque, float rpm) const {
    unsigned int numpts = mEngineInfo.Num_ENGINE_BRAKING();
    if (numpts > 1) {
        float rpm_min = mEngineInfo.IDLE();
        float rpm_max = mEngineInfo.MAX_RPM();
        float ratio;
        // TODO
        unsigned int index =
            UTIL_InterprolateIndex(numpts - 1, UMath::Clamp(rpm, mEngineInfo.IDLE(), mEngineInfo.RED_LINE()), rpm_min, rpm_max, ratio);

        float base = mEngineInfo.ENGINE_BRAKING(index);
        unsigned int secondIndex = index + 1;
        float step = mEngineInfo.ENGINE_BRAKING(UMath::Min(secondIndex, numpts - 1));
        float load_pct = (step - base) * ratio + base;
        return -engine_torque * UMath::Clamp(load_pct, 0.f, 1.f);
    } else {
        return -engine_torque * mEngineInfo.ENGINE_BRAKING(0);
    }
}

// Credits: Brawltendo
// TODO not matching on GC yet
ShiftPotential EngineRacer::FindShiftPotential(GearID gear, float rpm) const {
    if (gear <= G_NEUTRAL)
        return SHIFT_POTENTIAL_NONE;

    float shift_up_rpm = GetShiftUpRPM(gear);
    float shift_down_rpm = GetShiftDownRPM(gear);
    float lower_gear_ratio = GetGearRatio(gear - 1);

    // is able to shift down
    if (gear != G_FIRST && lower_gear_ratio > 0.0f) {
        float lower_gear_shift_up_rpm = GetShiftUpRPM(gear - 1);
        lower_gear_shift_up_rpm = GetGearRatio(gear) * (lower_gear_shift_up_rpm / lower_gear_ratio) - 200.0f;
        // the RPM to shift down is lowered when the throttle isn't pressed
        float off_throttle_rpm = UMath::Lerp(UMath::Lerp(mEngineInfo.IDLE(), shift_down_rpm, 0.65f), shift_down_rpm, mThrottle);
        shift_down_rpm = UMath::Min(lower_gear_shift_up_rpm, off_throttle_rpm);
    }

    // shifting up
    if (rpm >= shift_up_rpm && gear < GetTopGear()) {
        return SHIFT_POTENTIAL_UP;
    }
    if (rpm <= shift_down_rpm && gear > G_FIRST) {
        return SHIFT_POTENTIAL_DOWN;
    }
    return SHIFT_POTENTIAL_NONE;
}

// Credits: Brawltendo
// UNSOLVED
ShiftStatus EngineRacer::OnGearChange(GearID gear) {
    if (gear >= GetNumGearRatios())
        return SHIFT_STATUS_NONE;
    // new gear can't be the same as the old one
    if (gear == mGear || gear < G_REVERSE)
        return SHIFT_STATUS_NONE;

    mGearShiftTimer = gear < mGear ? (GetShiftDelay(gear) * 0.25f) : GetShiftDelay(gear);
    mGear = gear;
    mClutch.Disengage();
    return SHIFT_STATUS_NORMAL;
}

// Credits: Brawltendo
float EngineRacer::GetDifferentialAngularVelocity(bool locked) const {
    float into_gearbox = 0.0f;
    bool in_reverse = GetGear() == G_REVERSE;

    if (FrontWheelDrive()) {
        float w_vel = (mSuspension->GetWheelAngularVelocity(0) + mSuspension->GetWheelAngularVelocity(1)) * 0.5f;
        if (!locked) {
            if (UMath::Abs(w_vel) > 0.0f)
                into_gearbox = w_vel;
        } else {
            into_gearbox = in_reverse ? UMath::Min(w_vel, 0.0f) : UMath::Max(w_vel, 0.0f);
        }
    }

    if (RearWheelDrive()) {
        float w_vel = (mSuspension->GetWheelAngularVelocity(2) + mSuspension->GetWheelAngularVelocity(3)) * 0.5f;
        if (!locked) {
            if (UMath::Abs(w_vel) > UMath::Abs(into_gearbox))
                into_gearbox = w_vel;
        } else {
            into_gearbox = in_reverse ? UMath::Min(w_vel, into_gearbox) : UMath::Max(w_vel, into_gearbox);
        }
    }

    return into_gearbox;
}
