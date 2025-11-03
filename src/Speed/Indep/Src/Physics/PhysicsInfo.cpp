#include "PhysicsInfo.hpp"
#include "PhysicsTunings.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Sim/UTil.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

using namespace Attrib::Gen;

// Credits: Brawltendo
float Physics::Info::AerodynamicDownforce(const chassis &chassis, const float speed) {
    return speed * 2 * chassis.AERO_COEFFICIENT() * 1000.0f;
}

// Credits: Brawltendo
float Physics::Info::EngineInertia(const engine &engine, const bool loaded) {
    float scale;
    if (loaded)
        scale = 1.f;
    else
        scale = 0.35f;
    return scale * (engine.FLYWHEEL_MASS() * 0.025f + 0.25f);
}

// Credits: Brawltendo
Physics::Info::eInductionType Physics::Info::InductionType(const induction &induction) {
    if (induction.HIGH_BOOST() > 0.0f || induction.LOW_BOOST() > 0.0f) {
        // turbochargers don't produce significant boost until above the boost threshold (the lowest engine RPM at which it will spool up)
        // meanwhile superchargers apply boost proportionally to the engine RPM, so this param isn't needed there
        if (induction.SPOOL() > 0.0f) {
            return INDUCTION_TURBO_CHARGER;
        } else {
            return INDUCTION_SUPER_CHARGER;
        }
    } else {
        return INDUCTION_NONE;
    }
}

// Credits: Brawltendo
float Physics::Info::NosBoost(const nos &nos, const Tunings *tunings) {
    float torque_scale = 1.0f;
    float boost = nos.TORQUE_BOOST();
    if (tunings) {
        boost += boost * tunings->Value[Physics::Tunings::NOS] * 0.25f;
    }
    return boost + torque_scale;
}

// Credits: Brawltendo
float Physics::Info::NosCapacity(const nos &nos, const Tunings *tunings) {
    float capacity = nos.NOS_CAPACITY();
    if (tunings) {
        capacity -= capacity * tunings->Value[Physics::Tunings::NOS] * 0.25f;
    }
    return capacity;
}

// Credits: Brawltendo
float Physics::Info::InductionRPM(const engine &engine, const induction &induction, const Tunings *tunings) {
    float spool = induction.SPOOL();

    // tune the (normalized) RPM at which forced induction kicks in
    if (tunings && spool > 0.0f) {
        float range;
        float value = tunings->Value[Physics::Tunings::INDUCTION];
        if (value < 0.0f) {
            range = spool * 0.25f;
        } else {
            range = (1.0f - spool) * 0.25f;
        }
        spool += range * value;
    }

    // return the unnormalized RPM
    return spool * (engine.RED_LINE() - engine.IDLE()) + engine.IDLE();
}

// Credits: Brawltendo
float Physics::Info::InductionBoost(const engine &engine, const induction &induction, float rpm, float spool, const Tunings *tunings, float *psi) {
    if (psi) {
        *psi = 0.0f;
    }

    spool = UMath::Clamp(spool, 0.0f, 1.0f);
    float rpm_min = engine.IDLE();
    float rpm_max = engine.RED_LINE();
    float induction_boost = 0.f;
    float spool_rpm = InductionRPM(engine, induction, tunings);
    float high_boost = induction.HIGH_BOOST();
    float low_boost = induction.LOW_BOOST();
    float drag = induction.VACUUM();

    if (high_boost > 0.0f || low_boost > 0.0f) {
        // tuning slider adjusts the induction boost bias
        // -tuning produces more low end boost, while +tuning produces more high end boost
        if (tunings) {
            float value = tunings->Value[Physics::Tunings::INDUCTION];
            low_boost  -= low_boost  * value * 0.25f;
            high_boost += high_boost * value * 0.25f;
        }

        if (rpm >= spool_rpm) {
            float induction_ratio = UMath::Ramp(rpm, spool_rpm, rpm_max);
            induction_boost = induction_ratio * high_boost + (1.0f - induction_ratio) * low_boost;
            if (psi) {
                *psi = spool * induction.PSI() * UMath::Ramp(induction_boost, 0.0f, UMath::Max(high_boost, low_boost));
            }
        } else if (drag < 0.0f) {
            // apply vacuum effect when not in boost
            float drag_ratio = UMath::Ramp(rpm, rpm_min, spool_rpm);
            induction_boost = drag_ratio * drag;
            if (psi) {
                *psi = drag_ratio * -induction.PSI() * UMath::Ramp(-induction_boost, 0.0f, UMath::Max(high_boost, low_boost));
            }
        }
    }
    
    return induction_boost * spool;
}

// Credits: Brawltendo
float Physics::Info::Torque(const Attrib::Gen::engine &engine, float rpm) {
    float rpm_min = engine.IDLE();
    float rpm_max = engine.MAX_RPM();
    rpm = UMath::Clamp(rpm, engine.IDLE(), engine.RED_LINE());
    unsigned int numpts = engine.Num_TORQUE();
    if (numpts > 1) {
        float ratio;
        unsigned int index = UTIL_InterprolateIndex(numpts - 1, rpm, rpm_min, rpm_max, ratio);
        float power = engine.TORQUE(index);
        unsigned int secondIndex = UMath::Min(numpts - 1, index + 1);
        return UMath::Lerp(power, engine.TORQUE(secondIndex), ratio);
    }

    return 0.0f;
}

// Credits: Brawltendo
float Physics::Info::WheelDiameter(const tires &tires, bool front) {
    int axle = front ? 0 : 1;
    float diameter = INCH2METERS(tires.RIM_SIZE().At(axle));
    return diameter + tires.SECTION_WIDTH().At(axle) * 0.001f * 2.0f * (tires.ASPECT_RATIO().At(axle) * 0.01f);
}

// float Physics::Info::MaxInductedTorque(const Attrib::Gen::pvehicle &pvehicle, float &atrpm, const Tunings *tunings) {
// 	Attrib::Gen::engine engine(pvehicle.engine(), 0, NULL);
// 	Attrib::Gen::induction induction(pvehicle.induction());
// 	return MaxInductedTorque(engine, induction, atrpm, tunings);
// }

// Credits: Brawltendo
// TODO not matching on GC yet
bool Physics::Info::ShiftPoints(const transmission &transmission, const engine &engine, const induction &induction, float *shift_up,
                                float *shift_down, unsigned int numpts) {
    for (int i = 0; i < numpts; ++i) {
        shift_up[i] = 0.0f;
        shift_down[i] = 0.0f;
    }

    unsigned int num_gear_ratios = transmission.Num_GEAR_RATIO();
    if (numpts < num_gear_ratios)
        return false;

    float redline = engine.RED_LINE();
    int topgear = num_gear_ratios - 1;
    int j;
    for (j = G_FIRST; j < topgear; ++j) {
        float g1 = transmission.GEAR_RATIO(j);
        float g2 = transmission.GEAR_RATIO(j + 1);
        float rpm = (redline + engine.IDLE()) * 0.5f;
        float max = rpm;
        int flag = 0;

        if (rpm < redline) {
            // find the upshift RPM for this gear using predicted engine torque
            while (!flag) {
                // seems like the rpm and spool params are swapped in both instances
                // so either it's a mistake that was copy-pasted or it was a deliberate choice
                float currenttorque = Torque(engine, max) * (InductionBoost(engine, induction, 1.0f, max, NULL, NULL) + 1.0f);
                float shiftuptorque;
                if (UMath::Abs(g1) > 0.00001f) {
                    float ratio = g2 / g1;
                    float next_rpm = ratio * max;
                    shiftuptorque = Torque(engine, next_rpm) * (InductionBoost(engine, induction, 1.0f, next_rpm, NULL, NULL) + 1.0f) * g2 / g1;
                } else {
                    shiftuptorque = 0.0f;
                }

                // set the upshift RPM to the current max
                if (shiftuptorque > currenttorque)
                    break;

                max += 50.0f;
                // set the upshift RPM to the redline RPM
                flag = !(max < redline);
            }
            if (!flag) {
                shift_up[j] = max;
            }
        } else {
            flag = 1;
        }
        if (flag) {
            shift_up[j] = redline - 100.0f;
        }

        // calculate downshift RPM for the next gear
        if (UMath::Abs(g1) > 0.00001f) {
            shift_down[j + 1] = (g2 / g1) * shift_up[j];
        } else {
            shift_down[j + 1] = 0.0f;
        }
    }

    shift_up[topgear] = engine.RED_LINE();
    return true;
}

// Credits: Brawltendo
float Physics::Info::Speedometer(const Attrib::Gen::transmission &transmission, const Attrib::Gen::engine &engine, const Attrib::Gen::tires &tires,
                                 float rpm, GearID gear, const Tunings *tunings) {
    float speed = 0.0f;
    float gear_ratio = transmission.GEAR_RATIO(gear) * transmission.FINAL_GEAR();
    float power_range = engine.RED_LINE() - engine.IDLE();
    gear_ratio = UMath::Abs(gear_ratio);
    if (gear_ratio > 0.0f && power_range > 0.0f) {
        float wheelrear = WheelDiameter(tires, false) * 0.5f;
        float wheelfront = WheelDiameter(tires, true) * 0.5f;
        float avg_wheel_radius = (wheelrear + wheelfront) * 0.5f;
        float clutch_rpm = (rpm - engine.IDLE()) / gear_ratio / power_range * engine.RED_LINE();
        speed = RPM2RPS(clutch_rpm) * avg_wheel_radius;
    }

    float limiter = MPH2MPS(engine.SPEED_LIMITER(0));
    if (limiter > 0.0f) {
        speed = UMath::Min(speed, limiter);
    }

    return speed;
}
