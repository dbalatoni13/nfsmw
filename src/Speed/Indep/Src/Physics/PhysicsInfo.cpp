#include "PhysicsInfo.hpp"
#include "PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "PhysicsTunings.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Sim/UTil.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

using namespace Attrib::Gen;

static Physics::Info::Performance PerformanceWeights[7] = {
    Physics::Info::Performance(0.25f, 1.5f, 0.25f), // PUT_TIRES
    Physics::Info::Performance(0.0f, 0.5f, 0.0f),   // PUT_BRAKES
    Physics::Info::Performance(0.25f, 1.0f, 0.2f),  // PUT_CHASSIS
    Physics::Info::Performance(1.0f, 0.0f, 0.75f),  // PUT_TRANSMISSION
    Physics::Info::Performance(0.5f, 0.0f, 1.0f),   // PUT_ENGINE
    Physics::Info::Performance(0.25f, 0.0f, 1.25f), // PUT_INDUCTION
    Physics::Info::Performance(0.25f, 0.0f, 1.5f),  // PUT_NOS
};
static PerfStats top_stats;
static PerfStats bottom_stats;
static PerformanceMaps TheStockCars;
static int Physics_Info_initialized;

// Credits: Brawltendo
float Physics::Info::AerodynamicDownforce(const Attrib::Gen::chassis &chassis, const float speed) {
    return speed * 2 * chassis.AERO_COEFFICIENT() * 1000.0f;
}

// Credits: Brawltendo
float Physics::Info::EngineInertia(const Attrib::Gen::engine &engine, const bool loaded) {
    float scale;
    if (loaded) {
        scale = 1.0f;
    } else {
        scale = 0.35f;
    }
    return scale * (engine.FLYWHEEL_MASS() * 0.025f + 0.25f);
}

Physics::Info::eInductionType Physics::Info::InductionType(const Attrib::Gen::pvehicle &pvehicle) {
    const Attrib::Gen::induction ind(pvehicle.induction(0), 0, nullptr);
    return InductionType(ind);
}

// Credits: Brawltendo
Physics::Info::eInductionType Physics::Info::InductionType(const Attrib::Gen::induction &induction) {
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

bool Physics::Info::HasNos(const Attrib::Gen::pvehicle &pvehicle) {
    const Attrib::Gen::nos nos(pvehicle.nos(0), 0, nullptr);
    return nos.TORQUE_BOOST() > 0.0f && nos.NOS_CAPACITY() > 0.0f;
}

bool Physics::Info::HasRunflatTires(const Attrib::Gen::pvehicle &pvehicle) {
    return false;
}

// Credits: Brawltendo
float Physics::Info::NosBoost(const Attrib::Gen::nos &nos, const Tunings *tunings) {
    float torque_scale = 1.0f;
    float boost = nos.TORQUE_BOOST();
    if (tunings != nullptr) {
        boost += boost * tunings->Value[Physics::Tunings::NOS] * 0.25f;
    }
    return boost + torque_scale;
}

// Credits: Brawltendo
float Physics::Info::NosCapacity(const Attrib::Gen::nos &nos, const Tunings *tunings) {
    float capacity = nos.NOS_CAPACITY();
    if (tunings != nullptr) {
        capacity -= capacity * tunings->Value[Physics::Tunings::NOS] * 0.25f;
    }
    return capacity;
}

// Credits: Brawltendo
float Physics::Info::InductionRPM(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, const Tunings *tunings) {
    float spool = induction.SPOOL();

    // tune the (normalized) RPM at which forced induction kicks in
    if ((tunings != nullptr) && spool > 0.0f) {
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
float Physics::Info::InductionBoost(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, float rpm, float spool,
                                    const Tunings *tunings, float *psi) {
    if (psi != nullptr) {
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
        if (tunings != nullptr) {
            float value = tunings->Value[Physics::Tunings::INDUCTION];
            low_boost -= low_boost * value * 0.25f;
            high_boost += high_boost * value * 0.25f;
        }

        if (rpm >= spool_rpm) {
            float induction_ratio = UMath::Ramp(rpm, spool_rpm, rpm_max);
            induction_boost = induction_ratio * high_boost + (1.0f - induction_ratio) * low_boost;
            if (psi != nullptr) {
                *psi = spool * induction.PSI() * UMath::Ramp(induction_boost, 0.0f, UMath::Max(high_boost, low_boost));
            }
        } else if (drag < 0.0f) {
            // apply vacuum effect when not in boost
            float drag_ratio = UMath::Ramp(rpm, rpm_min, spool_rpm);
            induction_boost = drag_ratio * drag;
            if (psi != nullptr) {
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
Meters Physics::Info::WheelDiameter(const Attrib::Gen::tires &tires, bool front) {
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
Meters Physics::Info::WheelDiameter(const Attrib::Gen::pvehicle &pvehicle, bool front) {
    const Attrib::Gen::tires t(pvehicle.tires(0), 0, nullptr);
    return WheelDiameter(t, front);
}

float Physics::Info::MaxInductedPower(const Attrib::Gen::pvehicle &pvehicle, const Tunings *tunings) {
    Attrib::Gen::engine engine(pvehicle.engine(0), 0, nullptr);
    Attrib::Gen::induction induction(pvehicle.induction(0), 0, nullptr);
    unsigned int num_torque = engine.Num_TORQUE();

    if (num_torque < 2) {
        return 0.0f;
    }

    float result = 0.0f;
    float rpm = engine.IDLE();
    float delta_rpm = (engine.MAX_RPM() - engine.IDLE()) / static_cast<float>(engine.Num_TORQUE() - 1);

    for (unsigned int i = 0; i < engine.Num_TORQUE(); i++) {
        float pt_torque = engine.TORQUE(i) * (InductionBoost(engine, induction, rpm, 1.0f, tunings, nullptr) + 1.0f);
        float hp = FTLB2HP(pt_torque, rpm);
        if (hp > result) {
            result = hp;
        }
        rpm += delta_rpm;
    }

    return result;
}

FtLbs Physics::Info::AvgInductedTorque(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction,
                                       const Attrib::Gen::transmission &transmission, bool from_peak, const Tunings *tunings) {
    unsigned int num_torque = engine.Num_TORQUE();
    if (num_torque < 2) {
        return 0.0f;
    }

    float peak_torque_rpm;
    float peak_torque = MaxInductedTorque(engine, induction, peak_torque_rpm, tunings);
    if (!(peak_torque > 0.0f)) {
        return 0.0f;
    }

    float torque_converter = transmission.TORQUE_CONVERTER();
    float torque;
    float rpm = engine.IDLE();
    float total_torque = 0.0f;
    float count = 0.0f;
    float delta_rpm = (engine.MAX_RPM() - engine.IDLE()) / static_cast<float>(engine.Num_TORQUE() - 1);

    for (unsigned int i = 0; i < engine.Num_TORQUE(); i++) {
        if (!from_peak || rpm >= peak_torque_rpm) {
            float converter_ratio = 1.0f + torque_converter * (1.0f - UMath::Ramp(rpm, engine.IDLE(), peak_torque_rpm));
            torque = converter_ratio * engine.TORQUE(i) * (InductionBoost(engine, induction, rpm, 1.0f, tunings, nullptr) + 1.0f);
            float torque_pt = torque;
            total_torque += torque_pt;
            count += 1.0f;
        }
        rpm += delta_rpm;
        if (rpm >= engine.RED_LINE()) {
            break;
        }
    }

    if (count > 0.0f) {
        return total_torque / count;
    }
    return 0.0f;
}

FtLbs Physics::Info::MaxInductedTorque(const Attrib::Gen::engine &eng, const Attrib::Gen::induction &ind, float &atrpm, const Tunings *tunings) {
    if (eng.Num_TORQUE() < 2) {
        atrpm = eng.IDLE();
        return 0.0f;
    }

    float torque = 0.0f;
    atrpm = eng.IDLE();
    float rpm = eng.IDLE();
    float delta_rpm = (eng.MAX_RPM() - eng.IDLE()) / static_cast<float>(eng.Num_TORQUE() - 1);

    for (unsigned int i = 0; i < eng.Num_TORQUE(); i++) {
        float pt_torque = eng.TORQUE(i) * (InductionBoost(eng, ind, rpm, 1.0f, tunings, nullptr) + 1.0f);
        if (pt_torque > torque) {
            atrpm = rpm;
            torque = pt_torque;
        }
        rpm += delta_rpm;
    }

    atrpm = UMath::Clamp(atrpm, eng.IDLE(), eng.RED_LINE());
    return torque;
}

FtLbs Physics::Info::MaxInductedTorque(const Attrib::Gen::pvehicle &pvehicle, Rpm &atrpm, const Tunings *tunings) {
    const Attrib::Gen::engine eng(pvehicle.engine(0), 0, nullptr);
    const Attrib::Gen::induction ind(pvehicle.induction(0), 0, nullptr);
    return MaxInductedTorque(eng, ind, atrpm, tunings);
}

float Physics::Info::MaxTorque(const Attrib::Gen::engine &eng, float &atrpm) {
    float torque = 0.0f;
    int max_pt = 0;
    unsigned int num_torque = eng.Num_TORQUE();

    if (num_torque == 0) {
        atrpm = torque;
    } else {
        for (unsigned int i = 0; i < eng.Num_TORQUE(); i++) {
            float pt_torque = eng.TORQUE(i);
            if (pt_torque > torque) {
                max_pt = i;
                torque = pt_torque;
            }
        }

        atrpm = eng.IDLE();
        if (num_torque > 1) {
            float rpm_ratio = static_cast<float>(max_pt) / static_cast<float>(num_torque - 1);
            atrpm = rpm_ratio * (eng.MAX_RPM() - eng.IDLE()) + atrpm;
        }

        atrpm = UMath::Clamp(atrpm, eng.IDLE(), eng.RED_LINE());
    }
    return torque;
}

float Physics::Info::Redline(const Attrib::Gen::engine &engine) {
    return engine.RED_LINE();
}

float Physics::Info::Redline(const Attrib::Gen::pvehicle &pvehicle) {
    const Attrib::Gen::engine eng(pvehicle.engine(0), 0, nullptr);
    return Redline(eng);
}

bool Physics::Info::ShiftPoints(const Attrib::Gen::transmission &transmission, const Attrib::Gen::engine &engine,
                                const Attrib::Gen::induction &induction, float *shift_up, float *shift_down, unsigned int numpts) {
    for (unsigned int i = 0; i < numpts; ++i) {
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
        float rpm = (engine.IDLE() + redline) * 0.5f;
        float max = rpm;
        int flag = 1;

        if (rpm < redline) {
            // find the upshift RPM for this gear using predicted engine torque
            while (flag) {
                // seems like the rpm and spool params are swapped in both instances
                // so either it's a mistake that was copy-pasted or it was a deliberate choice
                float currenttorque = Torque(engine, max) * (InductionBoost(engine, induction, 1.0f, max, nullptr, nullptr) + 1.0f);
                float shiftuptorque;
                if (UMath::Abs(g1) > 0.00001f) {
                    float ratio = g2 / g1;
                    float next_rpm = ratio * max;
                    shiftuptorque = Torque(engine, next_rpm) * g2 / g1 * (InductionBoost(engine, induction, 1.0f, next_rpm, nullptr, nullptr) + 1.0f);
                } else {
                    shiftuptorque = 0.0f;
                }

                if (shiftuptorque > currenttorque) {
                    // set the upshift RPM to the current max
                    shift_up[j] = max;
                    flag = 0;
                    break;
                }

                max += 50.0f;
                if (!(max < redline)) {
                    break;
                }
            }
        }
        if (flag) {
            // set the upshift RPM to the redline RPM
            shift_up[j] = redline - 100.0f;
        }

        // calculate downshift RPM for the next gear
        if (UMath::Abs(g1) > 0.00001f) {
            shift_down[j + 1] = shift_up[j] * g2 / g1;
        } else {
            shift_down[j + 1] = 0.0f;
        }
    }

    shift_up[topgear] = engine.RED_LINE();
    return true;
}

Mps Physics::Info::Speedometer(const Attrib::Gen::transmission &transmission, const Attrib::Gen::engine &engine, const Attrib::Gen::tires &tires,
                               Rpm rpm, GearID gear, const Tunings *tunings) {
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

unsigned int Physics::Info::NumFowardGears(const Attrib::Gen::transmission &transmission) {
    unsigned int num_ratios = transmission.Num_GEAR_RATIO();
    if (num_ratios > 2) {
        return num_ratios - 2;
    }
    return 0;
}

unsigned int Physics::Info::NumFowardGears(const Attrib::Gen::pvehicle &pvehicle) {
    const Attrib::Gen::transmission trans(pvehicle.transmission(0), 0, nullptr);
    return NumFowardGears(trans);
}

bool Physics::Info::HasPerformanceRatings(const Attrib::Gen::pvehicle &pvehicle) {
    float base_handling = pvehicle.HandlingRating(0);
    float top_handling = pvehicle.HandlingRating(1);
    return base_handling < top_handling && 0.0f < top_handling;
}

bool PerfStats::Fetch(const Attrib::Gen::pvehicle &pvehicle, bVector2 *graph_data, int *num_data) {
    Time0To100 = 0.0f;
    TopSpeed = 0.0f;
    HandlingRating = 0.0f;

    Attrib::Gen::engine engine(pvehicle.engine(0), 0, nullptr);
    Attrib::Gen::induction induction(pvehicle.induction(0), 0, nullptr);
    Attrib::Gen::transmission transmission(pvehicle.transmission(0), 0, nullptr);
    Attrib::Gen::chassis chassis(pvehicle.chassis(0), 0, nullptr);
    Attrib::Gen::tires tires(pvehicle.tires(0), 0, nullptr);
    Attrib::Gen::brakes brakes(pvehicle.brakes(0), 0, nullptr);
    Attrib::Gen::nos nos(pvehicle.nos(0), 0, nullptr);

    float max_torque_rpm;
    float max_torque = Physics::Info::MaxTorque(engine, max_torque_rpm);
    float wheel_radius = Physics::Info::WheelDiameter(pvehicle, false) * 0.5f;
    float final_gear = transmission.FINAL_GEAR();

    float shift_up[12];
    float shift_down[12];

    float idle = engine.IDLE();
    float redline = engine.RED_LINE();
    float min_w = RPM2RPS(idle);
    float max_w = RPM2RPS(redline);
    float limiter = MPH2MPS(engine.SPEED_LIMITER(0));

    if (!Physics::Info::ShiftPoints(transmission, engine, induction, shift_up, shift_down, 12)) {
        return false;
    }
    if (wheel_radius <= 0.0f || final_gear <= 0.0f) {
        return false;
    }

    unsigned int gear = 0;
    float time = 0.0f;
    float speed = 0.0f;
    int data_index = 0;
    float mass = pvehicle.MASS();
    float dT;
    if (graph_data != nullptr) {
        dT = 1.0f;
    } else {
        dT = 0.125f;
    }
    int max_data_index = 0;
    if (num_data != nullptr) {
        max_data_index = *num_data;
    }
    unsigned int topgear = Physics::Info::NumFowardGears(pvehicle) - 1;

    while (time < 120.0f) {
        float total_gear_ratio = transmission.GEAR_RATIO(gear + G_FIRST) * final_gear;
        float differential_w = (speed / wheel_radius) * total_gear_ratio;
        float power_range = (max_w - min_w) / max_w;
        float w = min_w + differential_w * power_range;
        float rpm = UMath::Clamp(RPS2RPM(w), idle, redline);

        if (gear == 0) {
            rpm = UMath::Max(rpm, max_torque_rpm);
        }

        float torque = Physics::Info::Torque(engine, rpm);

        float force = FTLB2NM(torque) * (Physics::Info::InductionBoost(engine, induction, rpm, 1.0f, nullptr, nullptr) + 1.0f);
        force *= total_gear_ratio;

        if (time < Physics::Info::NosCapacity(nos, nullptr) && speed > 5.0f) {
            force *= Physics::Info::NosBoost(nos, nullptr);
        }
        float acc = (force / wheel_radius) / mass;

        if (graph_data != nullptr) {
            graph_data[bMin(max_data_index, data_index++)] = bVector2(speed, acc);
        }

        const float dragcoef_spec = chassis.DRAG_COEFFICIENT();
        float drag = speed * (speed * dragcoef_spec);
        float dec = UMath::Abs(drag) / mass;
        speed = (speed + acc * dT) - dec * dT;

        if (speed >= MPH2MPS(100.0f) && Time0To100 <= 0.0f) {
            Time0To100 = time;
            dT = 1.0f;
        }

        if (limiter > 0.0f && speed >= limiter) {
            TopSpeed = limiter;
        } else if (gear == topgear && (acc < dec || redline <= rpm) && TopSpeed <= 0.0f) {
            TopSpeed = speed;
        }

        time += dT;

        if (TopSpeed > 0.0f) {
            if (Time0To100 > 0.0f) break;
        }

        if (rpm >= shift_up[gear + G_FIRST]) {
            gear = UMath::Min(gear + 1, topgear);
        }
    }

    if (gear == topgear || TopSpeed <= 0.0f) {
        TopSpeed = speed;
    }

    float base_handling = pvehicle.HandlingRating(0);
    float top_handling = pvehicle.HandlingRating(1);
    float ratio = 0.0f;
    float weights = 0.0f;
    for (int i = 0; i < 7; i++) {
        Physics::Upgrades::Type path = static_cast<Physics::Upgrades::Type>(i);
        weights += PerformanceWeights[i].Handling;
        ratio += Physics::Upgrades::GetPercent(pvehicle, path) * PerformanceWeights[i].Handling;
    }
    if (weights > 1e-6f) {
        ratio /= weights;
    }
    HandlingRating = UMath::Lerp(base_handling, top_handling, ratio);

    if (num_data != nullptr) {
        *num_data = data_index;
    }

    return TopSpeed > 0.0f && Time0To100 > 0.0f;
}

void PerfLevel::Print(const char *) {
}

void PerfLevel::Rate() {
    Stock.Handling = UMath::Ramp(Stats.HandlingRating, bottom_stats.HandlingRating, top_stats.HandlingRating);
    Stock.Acceleration = 1.0f - UMath::Ramp(Stats.Time0To100, bottom_stats.Time0To100, top_stats.Time0To100);
    Stock.TopSpeed = UMath::Ramp(Stats.TopSpeed, bottom_stats.TopSpeed, top_stats.TopSpeed);
}

bool PerfLevel::Analyze(const Attrib::Gen::pvehicle &pvehicle) {
    Analyzed = false;
    if (!Stats.Fetch(pvehicle, nullptr, nullptr)) {
        return false;
    }
    Analyzed = true;
    return true;
}

void PerformanceMaps::FindLimits(float direction, PerfStats &out) const {
    PerfStats temp;
    out = temp;

    for (const_iterator iter = begin(); iter != end(); iter++) {
        const PerfLevel &p = *iter;
        if (iter == begin()) {
            out = p.Stats;
        } else {
            if (p.Stats.HandlingRating * direction > out.HandlingRating * direction) {
                out.HandlingRating = p.Stats.HandlingRating;
            }
            if (p.Stats.Time0To100 * direction > out.Time0To100 * direction) {
                out.Time0To100 = p.Stats.Time0To100;
            }
            if (p.Stats.TopSpeed * direction > out.TopSpeed * direction) {
                out.TopSpeed = p.Stats.TopSpeed;
            }
        }
    }
}

void Physics::Info::Init() {
    const Attrib::Class *aclass = Attrib::Database::Get().GetClass(Attrib::Gen::pvehicle::ClassKey());
    unsigned int key = aclass->GetFirstCollection();

    PerformanceMaps all_cars;
    PerformanceMaps upgraded_cars;

    while (key != 0) {
        Attrib::Gen::pvehicle vehicle(key, 0, nullptr);
        if (vehicle.MODEL().GetHash32() != UCrc32::kNull.GetValue() && !vehicle.IsDynamic()) {
            if (HasPerformanceRatings(vehicle)) {
                PerfLevel performance(key);
                if (performance.Analyze(vehicle)) {
                    TheStockCars.push_back(performance);
                    all_cars.push_back(performance);
                }
                Physics::Upgrades::Flush();
            }
        }
        key = aclass->GetNextCollection(key);
    }

    for (PerformanceMaps::iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); iter++) {
        PerfLevel &p = *iter;
        Attrib::Gen::pvehicle vehicle(p.Key, 0, nullptr);
        if (Physics::Upgrades::SetMaximum(vehicle)) {
            PerfLevel performance(p.Key);
            if (performance.Analyze(vehicle)) {
                upgraded_cars.push_back(performance);
                all_cars.push_back(performance);
            }
            Physics::Upgrades::Flush();
        }
    }

    int count = TheStockCars.size();

    if (count == 0) {
        return;
    }

    all_cars.FindLimits(-1.0f, bottom_stats);
    all_cars.FindLimits(1.0f, top_stats);

    for (PerformanceMaps::iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); iter++) {
        PerfLevel &p = *iter;
        p.Rate();
        p.Print("Stock Performance");
    }

    for (PerformanceMaps::iterator iter = upgraded_cars.begin(); iter != upgraded_cars.end(); iter++) {
        PerfLevel &p = *iter;
        p.Rate();
        p.Print("Upgraded Performance");
    }

    for (PerformanceMaps::iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); iter++) {
        PerfLevel &p = *iter;
        p.Upgraded = p.Stock;
        for (PerformanceMaps::iterator iter2 = upgraded_cars.begin(); iter2 != upgraded_cars.end(); iter2++) {
            PerfLevel &p2 = *iter2;
            if (p.Key == p2.Key) {
                p.Upgraded = p2.Stock;
                break;
            }
        }
    }

    Physics_Info_initialized = 1;
}

bool Physics::Info::ComputeAccelerationTable(const Attrib::Gen::pvehicle &pvehicle, float &top_speed, float *table, int num_entries) {
    Attrib::Gen::transmission transmission(pvehicle.transmission(0), 0, nullptr);
    Attrib::Gen::tires tires(pvehicle.tires(0), 0, nullptr);
    Attrib::Gen::chassis chassis(pvehicle.chassis(0), 0, nullptr);
    Attrib::Gen::engine engine(pvehicle.engine(0), 0, nullptr);
    Attrib::Gen::induction induction(pvehicle.induction(0), 0, nullptr);

    float ft_lbs = AvgInductedTorque(engine, induction, transmission, true, nullptr);
    float avg_torque = FTLB2NM(ft_lbs);

    if (avg_torque <= 0.0f || num_entries < 2 || table == nullptr) {
        return false;
    }

    bVector2 graph_data[10];

    unsigned int num_gears = NumFowardGears(transmission);
    if (num_gears == 0) {
        return false;
    }

    float final_gear = transmission.FINAL_GEAR();
    float mass = pvehicle.MASS();
    float wheel_radius = WheelDiameter(tires, false) * 0.5f;

    if (wheel_radius <= 0.001f) {
        return false;
    }

    top_speed = 0.0f;
    unsigned int graph_max = 0;
    float prev_accel = 0.0f;
    float prev_speed = 0.0f;

    for (unsigned int foward_gear = 0; foward_gear < num_gears; foward_gear++) {
        GearID gear = static_cast<GearID>(foward_gear + G_FIRST);
        float gear_ratio = transmission.GEAR_RATIO(gear) * final_gear;
        float drive_torque = avg_torque * gear_ratio * transmission.GEAR_EFFICIENCY(gear);
        float force = drive_torque / wheel_radius;

        if (gear_ratio <= 0.0f) {
            return false;
        }

        float speed = (engine.RED_LINE() * RPM2RPS(1.0f) * wheel_radius) / gear_ratio;
        float drag = speed * speed * chassis.DRAG_COEFFICIENT();
        float accel = (force - drag) / mass;

        if (accel <= 0.0f) {
            if (prev_accel <= 0.0f) break;
            float ratio = 1.0f - prev_accel / (prev_accel - accel);
            speed = UMath::Lerp(prev_speed, speed, ratio);
            accel = 0.0f;
        }

        top_speed = UMath::Max(top_speed, speed);

        graph_data[graph_max].x = speed;
        graph_data[graph_max].y = accel;
        prev_accel = accel;
        prev_speed = speed;
        graph_max++;
    }

    if (graph_max == 0) {
        return false;
    }

    Graph accel_graph(graph_data, graph_max);

    if (top_speed > 0.0f) {
        float inc = top_speed / static_cast<float>(num_entries - 1);
        for (int i = 0; i < num_entries; i++) {
            table[i] = accel_graph.GetValue(inc * static_cast<float>(i));
        }
        return true;
    }

    return false;
}

bool Physics::Info::EstimatePerformance(const Attrib::Gen::pvehicle &vehicle, Performance &perf) {
    Performance stock;
    Performance upgraded;

    if (!GetStockPerformance(vehicle, stock)) {
        return false;
    }
    if (!GetMaximumPerformance(vehicle, upgraded)) {
        return false;
    }
    {
        {
            perf.Acceleration = 0.0f;
            perf.Handling = 0.0f;
            perf.TopSpeed = 0.0f;

            Performance weights;
            Performance junk;
            Performance junk_weights;

            for (int type = 0; type < 7; type++) {
                float value = Physics::Upgrades::GetPercent(vehicle, static_cast<Physics::Upgrades::Type>(type));

                junk_weights.Handling += PerformanceWeights[type].Handling;
                junk_weights.Acceleration += PerformanceWeights[type].Acceleration;
                junk_weights.TopSpeed += PerformanceWeights[type].TopSpeed;

                if (Physics::Upgrades::GetJunkman(vehicle, static_cast<Physics::Upgrades::Type>(type))) {
                    junk.Handling += PerformanceWeights[type].Handling;
                    junk.Acceleration += PerformanceWeights[type].Acceleration;
                    junk.TopSpeed += PerformanceWeights[type].TopSpeed;
                }

                weights.Handling += PerformanceWeights[type].Handling;
                perf.Handling = PerformanceWeights[type].Handling * value + perf.Handling;

                weights.Acceleration += PerformanceWeights[type].Acceleration;
                perf.Acceleration = PerformanceWeights[type].Acceleration * value + perf.Acceleration;

                weights.TopSpeed += PerformanceWeights[type].TopSpeed;
                perf.TopSpeed = PerformanceWeights[type].TopSpeed * value + perf.TopSpeed;
            }

            if (weights.Handling > 1e-6f) {
                perf.Handling = perf.Handling / weights.Handling;
            }
            if (weights.Acceleration > 1e-6f) {
                perf.Acceleration = perf.Acceleration / weights.Acceleration;
            }
            if (weights.TopSpeed > 1e-6f) {
                perf.TopSpeed = perf.TopSpeed / weights.TopSpeed;
            }

            if (junk_weights.Handling > 1e-6f) {
                junk.Handling = junk.Handling / junk_weights.Handling;
                upgraded.Handling = UMath::Lerp(upgraded.Handling, 1.0f, junk.Handling);
                float bonus = junk.Handling * 0.33f;
                float h = perf.Handling * (bonus + 1.0f);
                perf.Handling = h;
                stock.Handling = UMath::Lerp(stock.Handling, upgraded.Handling, bonus);
                perf.Handling = UMath::Min(h, 1.0f);
            }

            if (junk_weights.Acceleration > 1e-6f) {
                junk.Acceleration = junk.Acceleration / junk_weights.Acceleration;
                upgraded.Acceleration = UMath::Lerp(upgraded.Acceleration, 1.0f, junk.Acceleration);
                float bonus = junk.Acceleration * 0.33f;
                float a = perf.Acceleration * (bonus + 1.0f);
                perf.Acceleration = a;
                stock.Acceleration = UMath::Lerp(stock.Acceleration, upgraded.Acceleration, bonus);
                perf.Acceleration = UMath::Min(a, 1.0f);
            }

            if (junk_weights.TopSpeed > 1e-6f) {
                junk.TopSpeed = junk.TopSpeed / junk_weights.TopSpeed;
                upgraded.TopSpeed = UMath::Lerp(upgraded.TopSpeed, 1.0f, junk.TopSpeed);
                float bonus = junk.TopSpeed * 0.33f;
                float t = perf.TopSpeed * (bonus + 1.0f);
                perf.TopSpeed = t;
                stock.TopSpeed = UMath::Lerp(stock.TopSpeed, upgraded.TopSpeed, bonus);
                perf.TopSpeed = UMath::Min(t, 1.0f);
            }

            perf.Handling = UMath::Lerp(stock.Handling, upgraded.Handling, perf.Handling);
            perf.Acceleration = UMath::Lerp(stock.Acceleration, upgraded.Acceleration, perf.Acceleration);
            perf.TopSpeed = UMath::Lerp(stock.TopSpeed, upgraded.TopSpeed, perf.TopSpeed);
        }
    }
    return true;
}

bool Physics::Info::ComputePerformance(const Attrib::Gen::pvehicle &vehicle, Performance &perf) {
    if (!HasPerformanceRatings(vehicle)) {
        return false;
    }

    for (PerformanceMaps::iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); iter++) {
        PerfLevel &p = *iter;
        if (p.Key == vehicle.GetCollection()) {
            perf = p.Stock;
            return true;
        }
    }

    PerfLevel perf_level(vehicle.GetCollection());
    if (!perf_level.Analyze(vehicle)) {
        return false;
    }
    perf_level.Rate();
    perf = perf_level.Stock;
    return true;
}

bool Physics::Info::GetStockPerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf) {
    if (!HasPerformanceRatings(pvehicle)) {
        return false;
    }

    unsigned int key = pvehicle.GetCollection();
    if (pvehicle.IsDynamic()) {
        key = pvehicle.GetParent();
    }

    for (PerformanceMaps::const_iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); iter++) {
        const PerfLevel &p = *iter;
        if (p.Key == key) {
            perf = p.Stock;
            return true;
        }
    }

    return false;
}

bool Physics::Info::GetMaximumPerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf) {
    if (!HasPerformanceRatings(pvehicle)) {
        return false;
    }

    unsigned int key = pvehicle.GetCollection();
    if (pvehicle.IsDynamic()) {
        key = pvehicle.GetParent();
    }

    for (PerformanceMaps::const_iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); iter++) {
        const PerfLevel &p = *iter;
        if (p.Key == key) {
            perf = p.Upgraded;
            return true;
        }
    }

    return false;
}

void Physics::Info::FindPerformanceCandidates(const Performance &minimum_perf, const Performance &maximum_perf,
                                              UTL::Std::list<unsigned int, _type_list> &vlist) {
    vlist.clear();

    for (PerformanceMaps::const_iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); iter++) {
        const PerfLevel &p = *iter;
        if (p.Stock.TopSpeed <= maximum_perf.TopSpeed &&
            p.Stock.Acceleration <= maximum_perf.Acceleration &&
            p.Stock.Handling <= maximum_perf.Handling &&
            p.Upgraded.TopSpeed >= minimum_perf.TopSpeed &&
            p.Upgraded.Acceleration >= minimum_perf.Acceleration &&
            p.Upgraded.Handling >= minimum_perf.Handling) {
            vlist.push_back(p.Key);
        }
    }
}
