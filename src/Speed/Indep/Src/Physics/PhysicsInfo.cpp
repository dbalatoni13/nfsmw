#include "PhysicsInfo.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "PhysicsTunings.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "PhysicsUpgrades.hpp"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Sim/UTil.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"

using namespace Attrib::Gen;

static Physics::Info::Performance PerformanceWeights[7] = {
    Physics::Info::Performance(0.25f, 1.5f, 0.25f),
    Physics::Info::Performance(0.0f, 0.5f, 0.0f),
    Physics::Info::Performance(0.25f, 1.0f, 0.2f),
    Physics::Info::Performance(1.0f, 0.0f, 0.75f),
    Physics::Info::Performance(0.5f, 0.0f, 1.0f),
    Physics::Info::Performance(0.25f, 0.0f, 1.25f),
    Physics::Info::Performance(0.25f, 0.0f, 1.5f),
};

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
    Attrib::Gen::induction induction(pvehicle.induction(0), 0, nullptr);

    return InductionType(induction);
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
    Attrib::Gen::nos nos(pvehicle.nos(0), 0, nullptr);

    return nos.TORQUE_BOOST() > 0.0f && nos.NOS_CAPACITY() > 0.0f;
}

bool Physics::Info::HasRunflatTires(const Attrib::Gen::pvehicle &pvehicle) {
    // runflats were never implemented
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

Meters Physics::Info::WheelDiameter(const Attrib::Gen::pvehicle &pvehicle, bool front) {
    Attrib::Gen::tires tires(pvehicle.tires(0), 0, nullptr);
    return WheelDiameter(tires, front);
}

float Physics::Info::MaxInductedPower(const Attrib::Gen::pvehicle &pvehicle, const Tunings *tunings) {
    Attrib::Gen::engine engine(pvehicle.engine(0), 0, nullptr);
    Attrib::Gen::induction induction(pvehicle.induction(0), 0, nullptr);

    // a torque curve needs at least two points to be sampled

    if (engine.Num_TORQUE() <= 1) {
        // no curve, no power
        return 0.0f;
    }

    // walk the whole rev range and keep the best figure

    float maxpower = 0.0f;
    float rpm = engine.IDLE();
    float rpm_step = (engine.MAX_RPM() - engine.IDLE()) / (float)(engine.Num_TORQUE() - 1);
    for (unsigned int i = 0; i < engine.Num_TORQUE(); ++i) {
        // seems like the rpm and spool params are swapped here too
        float power = engine.TORQUE(i) * (InductionBoost(engine, induction, rpm, 1.0f, tunings, nullptr) + 1.0f) * rpm * (1.0f / 5252.0f);

        if (power > maxpower) {
            // new peak
            maxpower = power;
        }

        rpm += rpm_step;
    }

    // the instances go away with the scope

    return maxpower;
}

FtLbs Physics::Info::AvgInductedTorque(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction,
                                       const Attrib::Gen::transmission &transmission, bool from_peak, const Tunings *tunings) {
    // a torque curve needs at least two points to be sampled
    if (engine.Num_TORQUE() <= 1) {
        // no curve, no torque
        return 0.0f;
    }

    Rpm peakrpm;
    FtLbs peaktorque = MaxInductedTorque(engine, induction, peakrpm, tunings);
    if (peaktorque <= 0.0f) {
        return 0.0f;
    }
    float converter = transmission.TORQUE_CONVERTER();

    float rpm = engine.IDLE();
    float totaltorque = 0.0f;
    float numpts = 0.0f;
    float rpm_step = (engine.MAX_RPM() - engine.IDLE()) / (float)(engine.Num_TORQUE() - 1);
    for (unsigned int i = 0; i < engine.Num_TORQUE(); ++i) {
        // when asked for the average past the peak, skip everything below it
        if (!from_peak || rpm >= peakrpm) {
            // the torque converter multiplies the low end
            float scale = converter * (1.0f - UMath::Ramp(rpm, engine.IDLE(), peakrpm)) + 1.0f;

            float torque = scale * engine.TORQUE(i) * (InductionBoost(engine, induction, rpm, 1.0f, tunings, nullptr) + 1.0f);
            totaltorque += torque;
            numpts += 1.0f;
        }

        rpm += rpm_step;
        if (rpm >= engine.RED_LINE())
            break;
    }

    if (numpts > 0.0f) {
        return totaltorque / numpts;
    }

    return 0.0f;
}

FtLbs Physics::Info::MaxInductedTorque(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, float &atrpm,
                                       const Tunings *tunings) {
    // a torque curve needs at least two points to be sampled
    if (engine.Num_TORQUE() <= 1) {
        // no curve, the idle RPM is the best we can say
        atrpm = engine.IDLE();
        return 0.0f;
    }

    float maxtorque = 0.0f;
    atrpm = engine.IDLE();
    float rpm = engine.IDLE();

    float rpm_step = (engine.MAX_RPM() - engine.IDLE()) / (float)(engine.Num_TORQUE() - 1);
    for (unsigned int i = 0; i < engine.Num_TORQUE(); ++i) {
        // seems like the rpm and spool params are swapped here too
        float torque = engine.TORQUE(i) * (InductionBoost(engine, induction, rpm, 1.0f, tunings, nullptr) + 1.0f);
        if (torque > maxtorque) {
            maxtorque = torque;
            atrpm = rpm;
        }

        rpm += rpm_step;
    }

    // never report a peak outside the usable band
    atrpm = UMath::Clamp(atrpm, engine.IDLE(), engine.RED_LINE());

    return maxtorque;
}

FtLbs Physics::Info::MaxInductedTorque(const Attrib::Gen::pvehicle &pvehicle, Rpm &atrpm, const Tunings *tunings) {
    Attrib::Gen::engine engine(pvehicle.engine(0), 0, nullptr);
    Attrib::Gen::induction induction(pvehicle.induction(0), 0, nullptr);

    return MaxInductedTorque(engine, induction, atrpm, tunings);
}

FtLbs Physics::Info::MaxTorque(const Attrib::Gen::engine &engine, Rpm &atrpm) {
    float maxtorque = 0.0f;
    int maxindex = 0;

    unsigned int numpts = engine.Num_TORQUE();
    if (numpts == 0) {
        atrpm = maxtorque;
        return maxtorque;
    }

    for (unsigned int i = 0; i < engine.Num_TORQUE(); ++i) {
        float torque = engine.TORQUE(i);
        if (torque > maxtorque) {
            maxtorque = torque;
            maxindex = i;
        }
    }

    atrpm = engine.IDLE();
    if (numpts > 1) {
        float ratio = (float)maxindex / (float)(numpts - 1);
        atrpm += ratio * (engine.MAX_RPM() - engine.IDLE());
    }

    atrpm = UMath::Clamp(atrpm, engine.IDLE(), engine.RED_LINE());

    return maxtorque;
}

Rpm Physics::Info::Redline(const Attrib::Gen::engine &engine) {
    return engine.RED_LINE();
}

Rpm Physics::Info::Redline(const Attrib::Gen::pvehicle &pvehicle) {
    Attrib::Gen::engine engine(pvehicle.engine(0), 0, nullptr);
    return Redline(engine);
}

// Credits: Brawltendo
// TODO not matching on GC yet
bool Physics::Info::ShiftPoints(const Attrib::Gen::transmission &transmission, const Attrib::Gen::engine &engine,
                                const Attrib::Gen::induction &induction, float *shift_up, float *shift_down, unsigned int numpts) {
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
        int flag = 1;
        float rpm = (engine.IDLE() + redline) * 0.5f;
        float max = rpm;

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

                // set the upshift RPM to the current max
                if (shiftuptorque > currenttorque) {
                    shift_up[j] = max;
                    flag = 0;
                    break;
                }

                max += 50.0f;
                // set the upshift RPM to the redline RPM
                if (max >= redline)
                    break;
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

// Credits: Brawltendo
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
    unsigned int numgears = transmission.Num_GEAR_RATIO();
    if (numgears > 2) {
        // discount reverse and neutral
        return numgears - 2;
    }
    return 0;
}

unsigned int Physics::Info::NumFowardGears(const Attrib::Gen::pvehicle &pvehicle) {
    Attrib::Gen::transmission transmission(pvehicle.transmission(0), 0, nullptr);
    return NumFowardGears(transmission);
}

DECLARE_CONTAINER_TYPE(PerformanceMaps);

struct PerfStats {
    PerfStats() {
        bMemSet(this, 0, sizeof(PerfStats));
    }

    bool Fetch(const Attrib::Gen::pvehicle &pvehicle, struct bVector2 *graph_data, int *num_data);

    float Time0To100;
    float TopSpeed;
    float HandlingRating;
};

struct PerfLevel {
    PerfLevel(unsigned int key) : Key(key) {
        Analyzed = false;
    }

    void Print(const char *prefix);
    void Rate();
    bool Analyze(const Attrib::Gen::pvehicle &pvehicle);

    PerfStats Stats;
    Physics::Info::Performance Stock;
    Physics::Info::Performance Upgraded;
    const unsigned int Key;
    bool Analyzed;
};

struct PerformanceMaps : public UTL::Std::list<PerfLevel, _type_PerformanceMaps> {
    void FindLimits(float direction, PerfStats &out) const;
};

static PerfStats top_stats;
static PerfStats bottom_stats;
static PerformanceMaps TheStockCars;

bool PerfStats::Fetch(const Attrib::Gen::pvehicle &pvehicle, struct bVector2 *graph_data, int *num_data) {
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

    // without the shift points there is nothing to drive

    if (!Physics::Info::ShiftPoints(transmission, engine, induction, shift_up, shift_down, 12)) {
        return false;
    }

    if (wheel_radius <= 0.0f || final_gear <= 0.0f) {
        return false;
    }

    unsigned int gear = 0;
    float speed = 0.0f;
    float time = 0.0f;
    int data_index = 0;
    float mass = pvehicle.MASS();
    float dT = graph_data ? 1.0f : 0.125f;
    int max_data_index = num_data ? *num_data : 0;
    unsigned int topgear = Physics::Info::NumFowardGears(pvehicle) - 1;

    while (time < 120.0f) {

        float total_gear_ratio = transmission.GEAR_RATIO(gear + 2) * final_gear;
        float differential_w = speed / wheel_radius;
        float power_range = (max_w - min_w) / max_w;
        float w = differential_w * total_gear_ratio * power_range + min_w;
        float rpm = RPS2RPM(w);
        rpm = UMath::Clamp(rpm, idle, redline);

        if (gear == 0) {
            // off the line the engine sits at the torque peak
            rpm = UMath::Max(rpm, max_torque_rpm);
        }

        float torque = FTLB2NM(Physics::Info::Torque(engine, rpm));

        torque *= 1.0f + Physics::Info::InductionBoost(engine, induction, rpm, 1.0f, nullptr, nullptr);
        torque *= total_gear_ratio;

        if (time < Physics::Info::NosCapacity(nos, nullptr) && speed > 5.0f) {
            // the bottle is still full and the car is rolling
            torque *= Physics::Info::NosBoost(nos, nullptr);
        }

        float force = torque / wheel_radius;

        float acc = force / mass;
        if (graph_data) {
            graph_data[bMin(data_index++, max_data_index)] = bVector2(speed, acc);
        }
        const float dragcoef_spec = chassis.DRAG_COEFFICIENT();
        float drag = speed * dragcoef_spec;
        float dec = UMath::Abs(speed * drag) / mass;

        speed += acc * dT;
        speed -= dec * dT;

        if (speed >= MPH2MPS(100.0f) && Time0To100 <= 0.0f) {
            // past the benchmark the step can get coarse
            Time0To100 = time;
            dT = 1.0f;
        }

        if (limiter > 0.0f && speed >= limiter) {
            TopSpeed = limiter;
        } else if (gear == topgear && (dec > acc || rpm >= redline) && TopSpeed <= 0.0f) {
            // the drag ate the last of the power
            TopSpeed = speed;
        }

        time += dT;

        if (TopSpeed > 0.0f && Time0To100 > 0.0f) {
            // both numbers are in
            break;
        }

        if (rpm >= shift_up[gear + 2]) {
            gear = UMath::Min(topgear, ++gear);
        }
    }

    if (gear == topgear || TopSpeed <= 0.0f) {
        TopSpeed = speed;
    }

    float base_handling = pvehicle.HandlingRating(0);
    float top_handling = pvehicle.HandlingRating(1);

    // the handling rating is the weighted average of the upgrade paths

    float ratio = 0.0f;
    float weights = 0.0f;

    for (int i = 0; i < 7; i++) {

        Physics::Upgrades::Type path = (Physics::Upgrades::Type)i;
        weights += PerformanceWeights[i].Handling;
        ratio += Physics::Upgrades::GetPercent(pvehicle, path) * PerformanceWeights[i].Handling;
    }

    if (weights > UMath::Epsilon) {
        ratio /= weights;
    }
    HandlingRating = UMath::Lerp(base_handling, top_handling, ratio);

    if (num_data) *num_data = data_index;
    return TopSpeed > 0.0f && Time0To100 > 0.0f;
}

void PerfLevel::Print(const char *prefix) {
}

void PerfLevel::Rate() {
    Stock.Handling = UMath::Ramp(Stats.HandlingRating, bottom_stats.HandlingRating, top_stats.HandlingRating);
    Stock.Acceleration = 1.0f - UMath::Ramp(Stats.Time0To100, bottom_stats.Time0To100, top_stats.Time0To100);
    Stock.TopSpeed = UMath::Ramp(Stats.TopSpeed, bottom_stats.TopSpeed, top_stats.TopSpeed);
}

bool PerfLevel::Analyze(const Attrib::Gen::pvehicle &pvehicle) {
    Analyzed = false;

    // the graph is only needed when somebody asks for it

    if (!Stats.Fetch(pvehicle, nullptr, nullptr)) {
        // the car could not be measured
        return false;
    }

    Analyzed = true;
    return true;
}

void PerformanceMaps::FindLimits(float direction, PerfStats &out) const {
    out = PerfStats();

    for (PerformanceMaps::const_iterator iter = begin(); iter != end(); ++iter) {
        const PerfLevel &p = *iter;

        if (iter == begin()) {
            // the first entry sets the baseline
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

static bool Physics_Info_initialized = false;

void Physics::Info::Init() {

    // every stock car gets measured once, at boot

    const Attrib::Class *aclass = Attrib::Database::Get().GetClass(0x4a97ec8f);
    unsigned int key = aclass->GetFirstCollection();

    // the working lists only live as long as this function

    PerformanceMaps all_cars;
    PerformanceMaps upgraded_cars;

    while (key != 0) {

        Attrib::Gen::pvehicle pvehicle(key, 0, nullptr);

        if (pvehicle.MODEL().GetHash32() != UCrc32::kNull.GetValue() && !pvehicle.IsDynamic()) {

            // a car without ratings cannot be placed on the scale

            if (HasPerformanceRatings(pvehicle)) {

                // measure it the way it leaves the dealer

                PerfLevel performance(key);
                if (performance.Analyze(pvehicle)) {
                    TheStockCars.push_back(performance);
                    all_cars.push_back(performance);
                }

                Physics::Upgrades::Flush();
            }
        }

        key = aclass->GetNextCollection(key);
    }

    // and once more with every upgrade fitted

    for (PerformanceMaps::iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); iter++) {
        PerfLevel &p = *iter;

        Attrib::Gen::pvehicle pvehicle(p.Key, 0, nullptr);

        if (Physics::Upgrades::SetMaximum(pvehicle)) {

            // the upgraded car is a level of its own

            PerfLevel performance(p.Key);
            if (performance.Analyze(pvehicle)) {
                upgraded_cars.push_back(performance);
                all_cars.push_back(performance);
            }

            Physics::Upgrades::Flush();
        }
    }

    if (TheStockCars.size() == 0) {
        // nothing was measured, there is no scale to build
        return;
    }

    all_cars.FindLimits(-1.0f, bottom_stats);
    all_cars.FindLimits(1.0f, top_stats);

    // now the raw figures can be turned into ratings

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

    // hand every stock car the figures of its upgraded twin

    for (PerformanceMaps::iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); iter++) {
        PerfLevel &p_stock = *iter;

        p_stock.Upgraded = p_stock.Stock;

        for (PerformanceMaps::iterator iter2 = upgraded_cars.begin(); iter2 != upgraded_cars.end(); iter2++) {
            PerfLevel &p_upgraded = *iter2;

            if (p_stock.Key == p_upgraded.Key) {
                // the collection key is what ties the two together
                p_stock.Upgraded = p_upgraded.Stock;
                break;
            }
        }
    }

    Physics_Info_initialized = true;
}

bool Physics::Info::HasPerformanceRatings(const Attrib::Gen::pvehicle &pvehicle) {
    float low = pvehicle.HandlingRating(0);
    float high = pvehicle.HandlingRating(1);
    return low < high && high > 0.0f;
}

bool Physics::Info::ComputeAccelerationTable(const Attrib::Gen::pvehicle &pvehicle, float &top_speed, float *table, int num_entries) {

    Attrib::Gen::transmission transmission(pvehicle.transmission(0), 0, nullptr);
    Attrib::Gen::tires tires(pvehicle.tires(0), 0, nullptr);
    Attrib::Gen::chassis chassis(pvehicle.chassis(0), 0, nullptr);
    Attrib::Gen::engine engine(pvehicle.engine(0), 0, nullptr);
    Attrib::Gen::induction induction(pvehicle.induction(0), 0, nullptr);

    float ft_lbs = AvgInductedTorque(engine, induction, transmission, true, nullptr);
    float avg_torque = FTLB2NM(ft_lbs);

    if (avg_torque <= 0.0f || num_entries <= 1 || table == nullptr) {
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

    if (wheel_radius <= UMath::Epsilon) {
        return false;
    }

    // walk up through the gears and note where the curve breaks

    top_speed = 0.0f;
    unsigned int graph_max = 0;
    float prev_accel = 0.0f;
    float prev_speed = 0.0f;
    for (unsigned int i = 0; i < num_gears; i++) {

        // reverse and neutral sit in front of the forward gears

        unsigned int gear = i + 2;
        float gear_ratio = transmission.GEAR_RATIO(gear) * final_gear;
        float torque = avg_torque * gear_ratio * transmission.GEAR_EFFICIENCY(gear);
        float force = torque / wheel_radius;

        if (gear_ratio <= 0.0f) {
            return false;
        }

        float speed = RPM2RPS(engine.RED_LINE()) * wheel_radius / gear_ratio;

        // the drag grows with the square of the speed

        float drag = speed * speed * chassis.DRAG_COEFFICIENT();

        float accel = (force - drag) / mass;

        if (accel <= 0.0f) {

            if (prev_accel <= 0.0f) {
                break;
            }

            float blend = 1.0f - prev_accel / (prev_accel - accel);
            speed = UMath::Lerp(prev_speed, speed, blend);
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

        float step = top_speed / (num_entries - 1);
        for (int i = 0; i < num_entries; i++) {

            table[i] = accel_graph.GetValue(step * i);
        }

        return true;
    }

    return false;
}

bool Physics::Info::EstimatePerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf) {
    Performance stock;
    Performance upgraded;

    // the ratings only make sense against the stock and fully upgraded ends

    if (!GetStockPerformance(pvehicle, stock)) {
        return false;
    }

    if (!GetMaximumPerformance(pvehicle, upgraded)) {
        return false;
    }

    // accumulate the weighted contribution of every upgrade path

    perf.Acceleration = 0.0f;
    perf.Handling = 0.0f;
    perf.TopSpeed = 0.0f;

    Performance weights;
    Performance junk;
    Performance junk_weights;

    for (int i = 0; i < 7; i++) {
        Physics::Upgrades::Type path = (Physics::Upgrades::Type)i;

        float value = Physics::Upgrades::GetPercent(pvehicle, path);

        junk_weights.Handling += PerformanceWeights[i].Handling;
        junk_weights.Acceleration += PerformanceWeights[i].Acceleration;
        junk_weights.TopSpeed += PerformanceWeights[i].TopSpeed;

        if (Physics::Upgrades::GetJunkman(pvehicle, path)) {
            // junkman parts get counted apart, they go past the stock maximum

            junk.Handling += PerformanceWeights[i].Handling;
            junk.Acceleration += PerformanceWeights[i].Acceleration;
            junk.TopSpeed += PerformanceWeights[i].TopSpeed;
        }

        weights.Handling += PerformanceWeights[i].Handling;
        perf.Handling += PerformanceWeights[i].Handling * value;

        weights.Acceleration += PerformanceWeights[i].Acceleration;
        perf.Acceleration += PerformanceWeights[i].Acceleration * value;

        weights.TopSpeed += PerformanceWeights[i].TopSpeed;
        perf.TopSpeed += PerformanceWeights[i].TopSpeed * value;
    }

    if (weights.Handling > UMath::Epsilon)
        perf.Handling /= weights.Handling;

    if (weights.Acceleration > UMath::Epsilon)
        perf.Acceleration /= weights.Acceleration;

    if (weights.TopSpeed > UMath::Epsilon)
        perf.TopSpeed /= weights.TopSpeed;

    if (junk_weights.Handling > UMath::Epsilon) {
        // junkman parts push the upgraded end past 100%
        junk.Handling /= junk_weights.Handling;
        upgraded.Handling = UMath::Lerp(upgraded.Handling, 1.0f, junk.Handling);
        stock.Handling = UMath::Lerp(stock.Handling, upgraded.Handling, junk.Handling * 0.33f);
        perf.Handling *= junk.Handling * 0.33f + 1.0f;
        perf.Handling = UMath::Min(perf.Handling, 1.0f);
    }

    if (junk_weights.Acceleration > UMath::Epsilon) {
        // junkman parts push the upgraded end past 100%
        junk.Acceleration /= junk_weights.Acceleration;
        upgraded.Acceleration = UMath::Lerp(upgraded.Acceleration, 1.0f, junk.Acceleration);
        stock.Acceleration = UMath::Lerp(stock.Acceleration, upgraded.Acceleration, junk.Acceleration * 0.33f);
        perf.Acceleration *= junk.Acceleration * 0.33f + 1.0f;
        perf.Acceleration = UMath::Min(perf.Acceleration, 1.0f);
    }

    if (junk_weights.TopSpeed > UMath::Epsilon) {
        // junkman parts push the upgraded end past 100%
        junk.TopSpeed /= junk_weights.TopSpeed;
        upgraded.TopSpeed = UMath::Lerp(upgraded.TopSpeed, 1.0f, junk.TopSpeed);
        stock.TopSpeed = UMath::Lerp(stock.TopSpeed, upgraded.TopSpeed, junk.TopSpeed * 0.33f);
        perf.TopSpeed *= junk.TopSpeed * 0.33f + 1.0f;
        perf.TopSpeed = UMath::Min(perf.TopSpeed, 1.0f);
    }

    perf.Handling = UMath::Lerp(stock.Handling, upgraded.Handling, perf.Handling);
    perf.Acceleration = UMath::Lerp(stock.Acceleration, upgraded.Acceleration, perf.Acceleration);
    perf.TopSpeed = UMath::Lerp(stock.TopSpeed, upgraded.TopSpeed, perf.TopSpeed);
    return true;
}

bool Physics::Info::ComputePerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf) {
    // without ratings there is nothing to interpolate between
    if (!HasPerformanceRatings(pvehicle)) {
        return false;
    }

    // the stock cars were already measured at boot

    for (PerformanceMaps::iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); iter++) {
        // the collection key identifies the car

        PerfLevel &p_stock = *iter;
        if (p_stock.Key == pvehicle.GetCollection()) {
            // already known
            perf = p_stock.Stock;
            return true;
        }
    }

    // not a stock car, measure it now
    PerfLevel perf_level(pvehicle.GetCollection());
    if (!perf_level.Analyze(pvehicle)) {
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

    for (PerformanceMaps::const_iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); ++iter) {
        const PerfLevel &p = *iter;
        if (p.Key == key) {
            // already known
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

    for (PerformanceMaps::const_iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); ++iter) {
        const PerfLevel &p = *iter;
        if (p.Key == key) {
            // already known
            perf = p.Upgraded;
            return true;
        }
    }
    return false;
}
namespace Physics {
namespace Info {

void FindPerformanceCandidates(const Performance &minimum_perf, const Performance &maximum_perf, UTL::Std::list<unsigned int, _type_list> &vlist) {
    // start from an empty result

    vlist.clear();
    for (PerformanceMaps::const_iterator iter = TheStockCars.begin(); iter != TheStockCars.end(); ++iter) {
        const PerfLevel &p = *iter;

        // a car whose stock figures are already past the ceiling can never fit

        if (p.Stock.TopSpeed > maximum_perf.TopSpeed || p.Stock.Acceleration > maximum_perf.Acceleration || p.Stock.Handling > maximum_perf.Handling)
            continue;

        // and one whose fully upgraded figures stay under the floor cannot either

        if (p.Upgraded.TopSpeed < minimum_perf.TopSpeed || p.Upgraded.Acceleration < minimum_perf.Acceleration ||
            p.Upgraded.Handling < minimum_perf.Handling)
            continue;

        vlist.push_back(p.Key);
    }
}

} // namespace Info
} // namespace Physics
