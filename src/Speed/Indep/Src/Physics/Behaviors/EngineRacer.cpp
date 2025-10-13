#include "EngineRacer.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/Events/EEngineBlown.hpp"
#include "Speed/Indep/Src/Generated/Events/EPlayerShift.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Sim/UTil.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

Behavior *EngineRacer::Construct(const BehaviorParams &params) {
    return new EngineRacer(params);
}

float EngineRacer::GetHorsePower() const {
    float engine_torque = GetEngineTorque(mRPM);
    return NM2HP(engine_torque * mThrottle, mRPM);
}

void EngineRacer::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mSuspension);
    }
    if (mechanic == BEHAVIOR_MECHANIC_AI) {
        GetOwner()->QueryInterface(&mCheater);
    }
}

void EngineRacer::Sabotage(float time) {
    if ((mSabotage <= 0.0f) && (time > FLOAT_EPSILON) && !IsBlown()) {
        mSabotage = Sim::GetTime() + time;
    }
}

bool EngineRacer::Blow() {
    if (!mBlown) {
        mBlown = true;
        mSabotage = 0.0f;
        new EEngineBlown(GetOwner()->GetInstanceHandle());
        return true;
    }
    return false;
}

void EngineRacer::OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey) {}

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

// Credits: Brawltendo
float EngineRacer::GetDriveWheelSlippage() const {
    float retval = 0.0f;
    int drivewheels = 0;
    if (RearWheelDrive()) {
        drivewheels += 2;
        retval += mSuspension->GetWheelSlip(TIRE_RR) + mSuspension->GetWheelSlip(TIRE_RL);
    }
    if (FrontWheelDrive()) {
        drivewheels += 2;
        retval += mSuspension->GetWheelSlip(TIRE_FL) + mSuspension->GetWheelSlip(TIRE_FR);
    }

    return retval / drivewheels;
}

// Credits: Brawltendo
void EngineRacer::SetDifferentialAngularVelocity(float w) {
    float current = GetDifferentialAngularVelocity(0);
    float diff = w - current;
    float speed = MPS2MPH(GetVehicle()->GetAbsoluteSpeed());
    int lockdiff = speed < 40.0f;
    if (RearWheelDrive()) {
        if (!mSuspension->IsWheelOnGround(2) && !mSuspension->IsWheelOnGround(3))
            lockdiff = 1;

        float w1 = mSuspension->GetWheelAngularVelocity(2);
        float w2 = mSuspension->GetWheelAngularVelocity(3);
        if (lockdiff)
            w2 = w1 = (w1 + w2) * 0.5f;

        mSuspension->SetWheelAngularVelocity(2, w1 + diff);
        mSuspension->SetWheelAngularVelocity(3, w2 + diff);
    }

    lockdiff = speed < 40.0f;
    if (FrontWheelDrive()) {
        if (!mSuspension->IsWheelOnGround(0) && !mSuspension->IsWheelOnGround(1))
            lockdiff = 1;

        float w1 = mSuspension->GetWheelAngularVelocity(0);
        float w2 = mSuspension->GetWheelAngularVelocity(1);
        if (lockdiff)
            w2 = w1 = (w1 + w2) * 0.5f;

        mSuspension->SetWheelAngularVelocity(0, w1 + diff);
        mSuspension->SetWheelAngularVelocity(1, w2 + diff);
    }
}

// Credits: Brawltendo
float EngineRacer::CalcSpeedometer(float rpm, unsigned int gear) const {
    Physics::Tunings *tunings = GetVehicle()->GetTunings();
    return Physics::Info::Speedometer(mTranyInfo, mEngineInfo, mTireInfo, rpm, (GearID)gear, tunings);
}

// Credits: Brawltendo
float EngineRacer::GetMaxSpeedometer() const {
    unsigned int num_ratios = GetNumGearRatios();
    if (num_ratios > 0) {
        float limiter = MPH2MPS(mEngineInfo.SPEED_LIMITER(0));
        float max_speedometer = CalcSpeedometer(mEngineInfo.RED_LINE(), num_ratios - 1);
        if (limiter > 0.0f) {
            return UMath::Min(max_speedometer, limiter);
        } else {
            return max_speedometer;
        }
    } else {
        return 0.0f;
    }
}

// Credits: Brawltendo
float EngineRacer::GetSpeedometer() const {
    return CalcSpeedometer(RPS2RPM(mTransmissionVelocity), mGear);
}

// Credits: Brawltendo
void EngineRacer::LimitFreeWheels(float w) {
    unsigned int numwheels = mSuspension->GetNumWheels();
    for (unsigned int i = 0; i < numwheels; ++i) {
        if (!mSuspension->IsWheelOnGround(i)) {
            if (i < 2) {
                if (!FrontWheelDrive())
                    continue;
            } else if (!RearWheelDrive())
                continue;

            float ww = mSuspension->GetWheelAngularVelocity(i);
            if (ww * w < 0.0f)
                ww = 0.0f;
            else if (ww > 0.0f) {
                ww = UMath::Min(ww, w);
            } else if (ww < 0.0f) {
                ww = UMath::Max(ww, w);
            }
            mSuspension->SetWheelAngularVelocity(i, ww);
        }
    }
}

float SmoothRPMDecel[] = {2.5f, 15.0f};

// Credits: Brawltendo
// TODO not matching on GC yet
float Engine_SmoothRPM(bool is_shifting, GearID gear, float dT, float old_rpm, float new_rpm, float engine_inertia) {
    bool fast_shifting = is_shifting && gear > G_FIRST || gear == G_NEUTRAL;
    // this ternary is dumb but that's what makes it match
    float max_rpm_decel = -SmoothRPMDecel[fast_shifting ? 1 : 0];
    float rpm = new_rpm;
    float rpm_decel = max_rpm_decel / engine_inertia * 1000.0f;
    if (dT > 0.0f && (new_rpm - old_rpm) / dT < rpm_decel) {
        float newrpm = rpm_decel * dT + old_rpm;
        if (!(newrpm < new_rpm)) {
            rpm = newrpm;
        }
    }
    return rpm * 0.55f + old_rpm * 0.45f;
}

// Credits: Brawltendo
void EngineRacer::DoECU() {
    if (GetGear() <= G_NEUTRAL) {
        return;
    }
    // the speed at which the limiter starts to kick in
    float limiter = MPH2MPS(mEngineInfo.SPEED_LIMITER(0));
    if (limiter > 0.0f) {
        // the speed for the limiter to take full effect
        float cutoff = MPH2MPS(mEngineInfo.SPEED_LIMITER(1));
        if (cutoff > 0.0f) {
            float speedometer = GetSpeedometer();
            if (speedometer > limiter) {
                float limiter_range = speedometer - limiter;
                mThrottle *= (1.0f - UMath::Clamp(limiter_range / cutoff, 0.f, 1.0f));
            }
        }
    }
}

bool Tweak_InfiniteNOS = false;

// Credits: Brawltendo
// UNSOLVED
float EngineRacer::DoNos(const Physics::Tunings *tunings, float dT, bool engaged) {
    if (!HasNOS())
        return 1.0f;

    float speed_mph = MPS2MPH(GetVehicle()->GetAbsoluteSpeed());
    float recharge_rate = 0.0f;
    IPlayer *player = GetOwner()->GetPlayer();

    if (!player || player->CanRechargeNOS()) {
        float min_speed = mNOSInfo.RECHARGE_MIN_SPEED();
        float max_speed = mNOSInfo.RECHARGE_MAX_SPEED();
        if (speed_mph >= min_speed && mGear >= G_FIRST) {
            float t = UMath::Ramp(speed_mph, min_speed, max_speed);
            recharge_rate = UMath::Lerp(mNOSInfo.RECHARGE_MIN(), mNOSInfo.RECHARGE_MAX(), t);
        }
    }

    if (mGear < G_FIRST || mThrottle <= 0.0f || IsBlown())
        engaged = false;
    if (speed_mph < 10.0f && !IsNOSEngaged() || speed_mph < 5.0f && IsNOSEngaged())
        engaged = false;

    float nos_discharge = Physics::Info::NosCapacity(mNOSInfo, tunings);
    float nos_torque_scale = 1.0f;
    if (nos_discharge > 0.0f) {
        float nos_disengage = mNOSInfo.NOS_DISENGAGE();
        if (engaged && mNOSCapacity > 0.0f) {
            float discharge = dT / nos_discharge;
            // don't deplete nitrous
            if (Tweak_InfiniteNOS || GetVehicle()->GetDriverClass() == DRIVER_REMOTE)
                discharge = 0.0f;
            // GetCatchupCheat returns 0.0 for human racers, but AI racers get hax
            if (mCheater)
                discharge *= UMath::Lerp(1.0f, 0.5f, mCheater->GetCatchupCheat());
            mNOSCapacity -= discharge;
            nos_torque_scale = Physics::Info::NosBoost(mNOSInfo, tunings);
            mNOSEngaged = 1.0f;
            // mNOSCapacity = UMath::Max(mNOSCapacity - discharge, 0.f);
            mNOSCapacity = UMath::Max(mNOSCapacity, 0.0f);
        } else if (mNOSEngaged > 0.0f && nos_disengage > 0.0f) {
            // nitrous can't start recharging until the disengage timer runs out
            // it takes [NOS_DISENGAGE] seconds for it to reach zero

            mNOSEngaged -= dT / nos_disengage;
            mNOSEngaged = UMath::Max(mNOSEngaged, 0.0f);
        } else {
            if (mNOSCapacity < 1.0f && recharge_rate > 0.0f) {
                float recharge = dT / recharge_rate;
                // GetCatchupCheat returns 0.0 for human racers, but AI racers get hax
                if (mCheater)
                    recharge *= UMath::Lerp(1.0f, 2.0f, mCheater->GetCatchupCheat());
                mNOSCapacity = UMath::Min(recharge + mNOSCapacity, 1.0f);
            }
            mNOSEngaged = 0.0f;
        }

    } else {
        // fallback in case someone sets the nitrous capacity <= 0.0 by uncapping tuning limits

        mNOSCapacity = 0.0f;
        mNOSEngaged = 0.0f;
    }
    return nos_torque_scale;
}

// Credits: Brawltendo
// TODO not matching on GC yet
void EngineRacer::DoInduction(const Physics::Tunings *tunings, float dT) {
    Physics::Info::eInductionType type = Physics::Info::InductionType(mInductionInfo);
    if (type == Physics::Info::INDUCTION_NONE) {
        mSpool = 0.0f;
        mInductionBoost = 0.0f;
        mPSI = 0.0f;
        return;
    }

    float desired_spool = UMath::Ramp(mThrottle, 0.0f, 0.5f);
    float rpm = RPS2RPM(mAngularVelocity);

    if (IsGearChanging())
        desired_spool = 0.0f;
    // turbocharger can't start spooling up until the engine rpm is >= the boost threshold
    if (type == Physics::Info::INDUCTION_TURBO_CHARGER && rpm < Physics::Info::InductionRPM(mEngineInfo, mInductionInfo, tunings)) {
        desired_spool = 0.0f;
    }

    if (mSpool > desired_spool) {
        float spool_time = mInductionInfo.SPOOL_TIME_DOWN();
        if (spool_time > FLOAT_EPSILON) {
            mSpool -= dT / spool_time;
            mSpool = UMath::Max(mSpool, desired_spool);
        } else {
            mSpool = desired_spool;
        }
    } else if (mSpool < desired_spool) {
        float spool_time = mInductionInfo.SPOOL_TIME_UP();
        if (spool_time > FLOAT_EPSILON) {
            mSpool += dT / spool_time;
            mSpool = UMath::Min(mSpool, desired_spool);
        } else {
            mSpool = desired_spool;
        }
    }

    float target_psi;
    mSpool = UMath::Clamp(mSpool, 0.0f, 1.0f);
    mInductionBoost = Physics::Info::InductionBoost(mEngineInfo, mInductionInfo, rpm, mSpool, tunings, &target_psi);
    if (mPSI > target_psi) {
        mPSI = UMath::Max(mPSI - dT * 20.0f, target_psi);
    } else if (mPSI < target_psi) {
        mPSI = UMath::Min(mPSI + dT * 20.0f, target_psi);
    }
}

// Credits: Brawltendo
float EngineRacer::DoThrottle() {
    if (IsBlown() || !mIInput) {
        // cut the throttle when the engine is blown
        return 0.0f;
    }
    return mIInput->GetControls().fGas;
}

// Credits: Brawltendo
float EngineRacer::GetShiftPoint(GearID from_gear, GearID to_gear) const {
    if (from_gear < G_NEUTRAL) {
        return 0.0f;
    }
    if (to_gear <= G_NEUTRAL) {
        return 0.0f;
    }
    if (to_gear > from_gear) {
        return mShiftUpRPM[from_gear];
    }
    if (to_gear < from_gear) {
        return mShiftDownRPM[from_gear];
    }
    return 0.0f;
}

// Credits: Brawltendo
bool EngineRacer::DoGearChange(GearID gear, bool automatic) {
    // can't shift past top gear
    if (gear > GetTopGear()) {
        return false;
    }
    // can't shift below reverse gear
    if (gear < G_REVERSE) {
        return false;
    }

    GearID previous = (GearID)mGear;
    ShiftStatus status = OnGearChange(gear);
    // has shifted
    if (status != SHIFT_STATUS_NONE) {
        mShiftStatus = status;
        mShiftPotential = SHIFT_POTENTIAL_NONE;
        ISimable *owner = GetOwner();
        // player shifted
        if (owner->IsPlayer()) {
            // dispatch shift event
            new EPlayerShift(owner->GetInstanceHandle(), status, automatic, previous, gear);
        }
        // AI shifted
        return true;
    }
    // didn't shift
    return false;
}

// Credits: Brawltendo
// TODO not matching on GC yet
void EngineRacer::AutoShift() {
    if (mGear == G_REVERSE || mGearShiftTimer > 0.0f || GetVehicle()->IsStaging() || mSportShifting > 0.0f)
        return;

    // skip neutral when using auto transmission
    if (mGear == G_NEUTRAL) {
        mGear = G_FIRST;
        return;
    }

    switch (mShiftPotential) {
        case SHIFT_POTENTIAL_DOWN: {
            int next_gear = mGear - 1;
            if (next_gear > G_FIRST) {
                float current_rpm = RPS2RPM(mTransmissionVelocity);
                float rpm = GetRatioChange(next_gear, mGear) * current_rpm;
                do {
                    if (FindShiftPotential((GearID)next_gear, rpm) != SHIFT_POTENTIAL_DOWN) {
                        break;
                    }
                    rpm = GetRatioChange(--next_gear, mGear) * current_rpm;
                } while (next_gear > G_FIRST);
            }
            DoGearChange((GearID)next_gear, true);
            break;
        }

        case SHIFT_POTENTIAL_UP:
        case SHIFT_POTENTIAL_PERFECT:
        case SHIFT_POTENTIAL_MISS: {
            int have_traction = 1;
            for (int i = 0; i < 4; ++i) {
                have_traction &= mSuspension->IsWheelOnGround(i) && mSuspension->GetWheelSlip(i) < 4.f;
            }
            if (have_traction) {
                DoGearChange((GearID)(mGear + 1), true);
            }
            break;
        }

        default:
            return;
    }
}

// Credits: Brawltendo
void EngineRacer::DoShifting(float dT) {
    if (mIInput && mIInput->IsAutomaticShift()) {
        AutoShift();
    }

    if (mGearShiftTimer > 0.0f) {
        mGearShiftTimer -= dT;
        if (mGearShiftTimer <= 0.0f) {
            mGearShiftTimer = 0.0f;
        }
    }

    if (mSportShifting > 0.0f && mShiftPotential) {
        if (mIInput) {
            float gas = mIInput->GetControls().fGas;
            mSportShifting = UMath::Max(mSportShifting - dT * (2.0f - gas), 0.0f);
        } else {
            mSportShifting = 0.0f;
        }
    }
}
