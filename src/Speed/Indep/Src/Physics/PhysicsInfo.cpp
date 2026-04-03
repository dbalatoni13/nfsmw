#include "PhysicsInfo.hpp"
#include "PhysicsTunings.h"
#include "PhysicsUpgrades.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Sim/UTil.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"


static PerfStats top_stats;
static PerfStats bottom_stats;
static PerformanceMaps TheStockCars;
static int Physics_Info_initialized;
Physics::Info::Performance Physics::Info::PerformanceWeights[7];

void Physics::Info::Performance::Default() {
    TopSpeed = 0.0f;
    Handling = 0.0f;
    Acceleration = 0.0f;
}

float Physics::Info::AerodynamicDownforce(const Attrib::Gen::chassis &chassis, float speed) {
    return speed * 2 * chassis.AERO_COEFFICIENT() * 1000.0f;
}

float Physics::Info::EngineInertia(const Attrib::Gen::engine &engine, bool loaded) {
    float scale;
    if (loaded)
        scale = 1.f;
    else
        scale = 0.35f;
    return scale * (engine.FLYWHEEL_MASS() * 0.025f + 0.25f);
}

Physics::Info::eInductionType Physics::Info::InductionType(const Attrib::Gen::pvehicle &pvehicle) {
    const Attrib::Gen::induction ind(pvehicle.induction(0), 0, nullptr);
    return InductionType(ind);
}

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

float Physics::Info::NosBoost(const Attrib::Gen::nos &nos, const Tunings *tunings) {
    float torque_scale = 1.0f;
    float boost = nos.TORQUE_BOOST();
    if (tunings) {
        boost += boost * tunings->Value[Physics::Tunings::NOS] * 0.25f;
    }
    return boost + torque_scale;
}

float Physics::Info::NosCapacity(const Attrib::Gen::nos &nos, const Tunings *tunings) {
    float capacity = nos.NOS_CAPACITY();
    if (tunings) {
        capacity -= capacity * tunings->Value[Physics::Tunings::NOS] * 0.25f;
    }
    return capacity;
}

float Physics::Info::InductionRPM(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, const Tunings *tunings) {
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

float Physics::Info::InductionBoost(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, float rpm, float spool, const Tunings *tunings, float *psi) {
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
            low_boost -= low_boost * value * 0.25f;
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
        float step = engine.TORQUE(secondIndex) - power;
        float torque = power + step * ratio;
        return torque;
    }

    return 0.0f;
}

float Physics::Info::WheelDiameter(const Attrib::Gen::tires &tires, bool front) {
    int axle = front ? 0 : 1;
    float diameter = INCH2METERS(tires.RIM_SIZE().At(axle));
    return diameter + tires.SECTION_WIDTH().At(axle) * 0.001f * 2.0f * (tires.ASPECT_RATIO().At(axle) * 0.01f);
}

float Physics::Info::WheelDiameter(const Attrib::Gen::pvehicle &pvehicle, bool front) {
    const Attrib::Gen::tires t(pvehicle.tires(0), 0, nullptr);
    return WheelDiameter(t, front);
}

// NON_MATCHING: register allocation swap f31/f30 for rpm/converter_ratio
bool Physics::Info::ShiftPoints(const Attrib::Gen::transmission &transmission, const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, float *shift_up,
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
        float rpm = (engine.IDLE() + redline) * 0.5f;
        float max = rpm;
        int flag = 1;

        if (rpm < redline) {
            while (flag) {
                float currenttorque = Torque(engine, max) * (InductionBoost(engine, induction, 1.0f, max, NULL, NULL) + 1.0f);
                float shiftuptorque;
                if (UMath::Abs(g1) > 0.00001f) {
                    float ratio = g2 / g1;
                    float next_rpm = ratio * max;
                    shiftuptorque = Torque(engine, next_rpm) * g2 / g1 * (InductionBoost(engine, induction, 1.0f, next_rpm, NULL, NULL) + 1.0f);
                } else {
                    shiftuptorque = 0.0f;
                }

                if (shiftuptorque > currenttorque) {
                    shift_up[j] = max;
                    flag = 0;
                    break;
                }

                max += 50.0f;
                if (max >= redline) break;
            }
        }
        if (flag) {
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

float Physics::Info::AvgInductedTorque(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, const Attrib::Gen::transmission &transmission, bool from_peak, const Tunings *tunings) {
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
            torque = engine.TORQUE(i) * (InductionBoost(engine, induction, rpm, 1.0f, tunings, nullptr) + 1.0f);
            float torque_pt = torque * converter_ratio;
            total_torque += torque_pt;
            count += 1.0f;
        }
        rpm += delta_rpm;
        if (rpm >= engine.RED_LINE()) break;
    }

    if (count > 0.0f) {
        return total_torque / count;
    }
    return 0.0f;
}

float Physics::Info::MaxInductedTorque(const Attrib::Gen::engine &eng, const Attrib::Gen::induction &ind, float &atrpm, const Tunings *tunings) {
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

float Physics::Info::MaxInductedTorque(const Attrib::Gen::pvehicle &pvehicle, float &atrpm, const Tunings *tunings) {
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

bool PerfStats::Fetch(const Attrib::Gen::pvehicle &vehicle, bVector2 *graph_data, int *num_data) {
    Time0To100 = 0.0f;
    TopSpeed = 0.0f;
    HandlingRating = 0.0f;

    Attrib::Gen::engine eng(vehicle.engine(0), 0, nullptr);
    Attrib::Gen::induction ind(vehicle.induction(0), 0, nullptr);
    Attrib::Gen::transmission trans(vehicle.transmission(0), 0, nullptr);
    Attrib::Gen::chassis chas(vehicle.chassis(0), 0, nullptr);
    Attrib::Gen::tires tir(vehicle.tires(0), 0, nullptr);
    Attrib::Gen::brakes bra(vehicle.brakes(0).GetCollection(), 0, nullptr);
    Attrib::Gen::nos n(vehicle.nos(0), 0, nullptr);

    float max_torque_rpm;
    Physics::Info::MaxTorque(eng, max_torque_rpm);
    float wheel_diameter = Physics::Info::WheelDiameter(vehicle, false);
    float idle_rpm = eng.IDLE();
    float redline_rpm = eng.RED_LINE();
    float min_w = RPM2RPS(idle_rpm);
    float max_w = RPM2RPS(redline_rpm);
    float wheel_radius = wheel_diameter * 0.5f;
    float final_gear = trans.FINAL_GEAR();
    float speed_limiter = MPH2MPS(eng.SPEED_LIMITER(0));

    float shift_up[12];
    float shift_down[12];

    if (!Physics::Info::ShiftPoints(trans, eng, ind, shift_up, shift_down, 12) ||
        wheel_radius <= 0.0f || final_gear <= 0.0f) {
        return false;
    }

    float speed = 0.0f;
    unsigned int gear = 0;
    float time = 0.0f;
    float mass = vehicle.MASS();
    float dT = 0.125f;
    int data_index = 0;
    if (graph_data != nullptr) {
        dT = 1.0f;
    }
    int max_data_index = 0;
    if (num_data != nullptr) {
        max_data_index = *num_data;
    }
    float power_range = max_w - min_w;
    unsigned int num_gears = Physics::Info::NumFowardGears(vehicle);
    unsigned int last_gear = num_gears - 1;

    do {
        float total_gear_ratio = trans.GEAR_RATIO(gear + G_FIRST) * final_gear;
        float differential_rpm = RPS2RPM(min_w + (speed / wheel_radius) * total_gear_ratio * (power_range / max_w));
        float rpm = UMath::Min(differential_rpm, redline_rpm);
        rpm = UMath::Max(rpm, idle_rpm);

        if (gear == 0) {
            rpm = UMath::Max(rpm, max_torque_rpm);
        }

        float torque = Physics::Info::Torque(eng, rpm) * FTLB2NM(1.0f);
        float boost = Physics::Info::InductionBoost(eng, ind, rpm, 1.0f, nullptr, nullptr);
        float nos_capacity = Physics::Info::NosCapacity(n, nullptr);
        float force = torque * (boost + 1.0f) * total_gear_ratio;
        if (time < nos_capacity && speed > 5.0f) {
            float nos_boost = Physics::Info::NosBoost(n, nullptr);
            force *= nos_boost;
        }
        float accel = (force / wheel_radius) / mass;

        if (graph_data != nullptr) {
            int idx = data_index;
            if (max_data_index < idx) {
                idx = max_data_index;
            }
            graph_data[idx].y = accel;
            graph_data[idx].x = speed;
            data_index++;
        }

        float drag = UMath::Abs(speed * speed * chas.DRAG_COEFFICIENT()) / mass;
        speed = (speed + accel * dT) - drag * dT;

        if (speed >= MPH2MPS(100.0f) && Time0To100 <= 0.0f) {
            Time0To100 = time;
            dT = 1.0f;
        }

        if (speed_limiter <= 0.0f || speed < speed_limiter) {
            if (gear == last_gear &&
                (accel < drag || redline_rpm <= rpm) &&
                TopSpeed <= 0.0f) {
                TopSpeed = speed;
            }
        } else {
            TopSpeed = speed_limiter;
        }

        time += dT;

        if (TopSpeed > 0.0f) {
            if (Time0To100 > 0.0f) break;
        }

        if (rpm >= shift_up[gear + G_FIRST]) {
            unsigned int next_gear = gear + 1;
            gear = last_gear;
            if (next_gear < last_gear) {
                gear = next_gear;
            }
        }
    } while (time < 120.0f);

    if (gear == last_gear || TopSpeed <= 0.0f) {
        TopSpeed = speed;
    }

    float base_handling = vehicle.HandlingRating(0);
    float top_handling = vehicle.HandlingRating(1);
    float handling_sum = 0.0f;
    float diff = top_handling - base_handling;
    float *pw = &Physics::Info::PerformanceWeights[0].Handling;
    float weight_sum = 0.0f;
    Physics::Upgrades::Type type = Physics::Upgrades::PUT_TIRES;
    do {
        Physics::Upgrades::Type next = static_cast<Physics::Upgrades::Type>(type + Physics::Upgrades::PUT_BRAKES);
        weight_sum += *pw;
        float pct = Physics::Upgrades::GetPercent(vehicle, type);
        float w = *pw;
        pw += 3;
        handling_sum += pct * w;
        type = next;
    } while (static_cast<int>(type) < 7);
    if (weight_sum > 1e-6f) {
        handling_sum /= weight_sum;
    }
    HandlingRating = UMath::Lerp(base_handling, top_handling, handling_sum);

    if (num_data != nullptr) {
        *num_data = data_index;
    }

    bool success = false;
    if (TopSpeed > 0.0f) {
        success = Time0To100 > 0.0f;
    }
    return success;
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
    bMemSet(&temp, 0, sizeof(PerfStats));
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
            }
        }
        Physics::Upgrades::Flush();
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
        }
        Physics::Upgrades::Flush();
    }

    int count = 0;
    for (PerformanceMaps::iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); iter++) {
        count++;
    }

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
            if (p.Key == (*iter2).Key) {
                p.Upgraded = (*iter2).Stock;
                break;
            }
        }
    }

    Physics_Info_initialized = 1;
}

bool Physics::Info::ComputeAccelerationTable(const Attrib::Gen::pvehicle &vehicle, float &top_speed, float *table, int num_entries) {
    Attrib::Gen::transmission trans(vehicle.transmission(0), 0, nullptr);
    Attrib::Gen::tires tir(vehicle.tires(0), 0, nullptr);
    Attrib::Gen::chassis chas(vehicle.chassis(0), 0, nullptr);
    Attrib::Gen::engine eng(vehicle.engine(0), 0, nullptr);
    Attrib::Gen::induction ind(vehicle.induction(0), 0, nullptr);

    float avg_torque = AvgInductedTorque(eng, ind, trans, true, nullptr);
    avg_torque = avg_torque * FTLB2NM(1.0f);

    if (avg_torque <= 0.0f || num_entries < 2 || table == nullptr) {
        return false;
    }

    bVector2 graph_data[10];

    unsigned int num_gears = NumFowardGears(trans);
    if (num_gears == 0) {
        return false;
    }

    float mass = vehicle.MASS();
    float final_gear = trans.FINAL_GEAR();
    float wheel_radius = WheelDiameter(tir, false) * 0.5f;

    if (wheel_radius <= 0.001f) {
        return false;
    }

    top_speed = 0.0f;
    int graph_max = 0;
    float prev_accel = 0.0f;
    float prev_speed = 0.0f;

    for (unsigned int foward_gear = 0; foward_gear < num_gears; foward_gear++) {
        float gear_ratio = trans.GEAR_RATIO(foward_gear + G_FIRST) * final_gear;
        float gear_eff = trans.GEAR_EFFICIENCY(foward_gear + G_FIRST);

        if (gear_ratio <= 0.0f) break;

        float speed = (eng.RED_LINE() * RPM2RPS(1.0f) * wheel_radius) / gear_ratio;
        float force = (avg_torque * gear_ratio * gear_eff) / wheel_radius;
        float drag = speed * speed * chas.DRAG_COEFFICIENT();
        float accel = (force - drag) / mass;

        if (accel <= 0.0f) {
            if (prev_accel <= 0.0f) break;
            speed = UMath::Lerp(prev_speed, speed, prev_accel / (prev_accel - accel));
            accel = 0.0f;
        }

        top_speed = UMath::Max(top_speed, speed);

        graph_data[graph_max].x = speed;
        graph_data[graph_max].y = accel;
        graph_max++;
        prev_accel = accel;
        prev_speed = speed;
    }

    if (graph_max == 0) {
        return false;
    }

    Graph accel_graph(graph_data, graph_max);
    float max_speed = top_speed;

    if (max_speed <= 0.0f) {
        return false;
    }

    float inc = max_speed / static_cast<float>(num_entries - 1);
    for (int i = 0; i < num_entries; i++) {
        table[i] = accel_graph.GetValue(inc * static_cast<float>(i));
    }

    return true;
}

bool Physics::Info::EstimatePerformance(const Attrib::Gen::pvehicle &vehicle, Performance &perf) {
    Performance stock;
    Performance upgraded;

    bool success = GetStockPerformance(vehicle, stock);
    bool result = false;
    if (success) {
        success = GetMaximumPerformance(vehicle, upgraded);
        if (success) {
            perf.TopSpeed = 0.0f;
            perf.Acceleration = 0.0f;
            perf.Handling = 0.0f;

            Performance weights;
            Performance junk;
            Performance junk_weights;

            Physics::Info::Performance *pw = PerformanceWeights;
            Physics::Upgrades::Type type = Physics::Upgrades::PUT_TIRES;
            do {
                float value = Physics::Upgrades::GetPercent(vehicle, type);

                junk_weights.Handling += pw->Handling;
                junk_weights.Acceleration += pw->Acceleration;
                junk_weights.TopSpeed += pw->TopSpeed;

                if (Physics::Upgrades::GetJunkman(vehicle, type)) {
                    junk.Handling += pw->Handling;
                    junk.Acceleration += pw->Acceleration;
                    junk.TopSpeed += pw->TopSpeed;
                }

                float hw = pw->Handling;
                type = static_cast<Physics::Upgrades::Type>(type + Physics::Upgrades::PUT_BRAKES);
                float handling = hw * value + perf.Handling;
                weights.Handling += hw;
                perf.Handling = handling;
                weights.Acceleration += pw->Acceleration;
                perf.Acceleration += pw->Acceleration * value;
                weights.TopSpeed += pw->TopSpeed;
                perf.TopSpeed += pw->TopSpeed * value;
                pw++;
            } while (static_cast<int>(type) < 7);

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
                float junk_ratio = junk.Handling / junk_weights.Handling;
                upgraded.Handling = UMath::Lerp(upgraded.Handling, 1.0f, junk_ratio);
                float bonus = junk_ratio * 0.33f;
                float h = perf.Handling * (bonus + 1.0f);
                perf.Handling = h;
                stock.Handling = UMath::Lerp(stock.Handling, upgraded.Handling, bonus);
                perf.Handling = UMath::Min(h, 1.0f);
            }

            if (junk_weights.Acceleration > 1e-6f) {
                float junk_ratio = junk.Acceleration / junk_weights.Acceleration;
                upgraded.Acceleration = UMath::Lerp(upgraded.Acceleration, 1.0f, junk_ratio);
                float bonus = junk_ratio * 0.33f;
                float a = perf.Acceleration * (bonus + 1.0f);
                perf.Acceleration = a;
                stock.Acceleration = UMath::Lerp(stock.Acceleration, upgraded.Acceleration, bonus);
                perf.Acceleration = UMath::Min(a, 1.0f);
            }

            if (junk_weights.TopSpeed > 1e-6f) {
                float junk_ratio = junk.TopSpeed / junk_weights.TopSpeed;
                upgraded.TopSpeed = UMath::Lerp(upgraded.TopSpeed, 1.0f, junk_ratio);
                float bonus = junk_ratio * 0.33f;
                float t = perf.TopSpeed * (bonus + 1.0f);
                perf.TopSpeed = t;
                stock.TopSpeed = UMath::Lerp(stock.TopSpeed, upgraded.TopSpeed, bonus);
                perf.TopSpeed = UMath::Min(t, 1.0f);
            }

            result = true;
            perf.Handling = UMath::Lerp(stock.Handling, upgraded.Handling, perf.Handling);
            perf.Acceleration = UMath::Lerp(stock.Acceleration, upgraded.Acceleration, perf.Acceleration);
            perf.TopSpeed = UMath::Lerp(stock.TopSpeed, upgraded.TopSpeed, perf.TopSpeed);
        } else {
            result = false;
        }
    }
    return result;
}

bool Physics::Info::ComputePerformance(const Attrib::Gen::pvehicle &vehicle, Performance &perf) {
    if (!HasPerformanceRatings(vehicle)) {
        return false;
    }

    for (PerformanceMaps::iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); iter++) {
        if ((*iter).Key == vehicle.GetCollection()) {
            perf = (*iter).Stock;
            return true;
        }
    }

    unsigned int coll_key = vehicle.GetCollection();
    PerfLevel perf_level(coll_key);
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
