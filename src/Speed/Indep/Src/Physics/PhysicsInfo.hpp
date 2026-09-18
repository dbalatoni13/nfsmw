#ifndef PHYSICSINFO_HPP
#define PHYSICSINFO_HPP

#include "PhysicsTunings.h"
#include "PhysicsTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/induction.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/nos.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"

namespace Physics {
namespace Info {

enum eInductionType {
    INDUCTION_SUPER_CHARGER = 2,
    INDUCTION_TURBO_CHARGER = 1,
    INDUCTION_NONE = 0,
};

struct Performance {
    Performance() {
        Default();
    }

    Performance(float topspeed, float handling, float accel) {
        TopSpeed = topspeed;
        Handling = handling;
        Acceleration = accel;
    }

    void Default() {
        TopSpeed = 0.0f;
        Handling = 0.0f;
        Acceleration = 0.0f;
    }

    void Maximize(const Performance &other) {
        TopSpeed = UMath::Max(TopSpeed, other.TopSpeed);
        Handling = UMath::Max(Handling, other.Handling);
        Acceleration = UMath::Max(Acceleration, other.Acceleration);
    }

    // inline en el original (DWARF de Game_FindPerformanceCandidates); suma
    // amount a cada componente y lo acota a [0, 1]
    void Grow(float amount) {
        TopSpeed = UMath::Clamp(TopSpeed + amount, 0.0f, 1.0f);
        Handling = UMath::Clamp(Handling + amount, 0.0f, 1.0f);
        Acceleration = UMath::Clamp(Acceleration + amount, 0.0f, 1.0f);
    }

    float TopSpeed;
    float Handling;
    float Acceleration;
};

void Init();
bool ComputePerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf);

float AerodynamicDownforce(const Attrib::Gen::chassis &chassis, const float speed);
float EngineInertia(const Attrib::Gen::engine &engine, const bool loaded);
eInductionType InductionType(const Attrib::Gen::induction &induction);
eInductionType InductionType(const Attrib::Gen::pvehicle &pvehicle);
bool HasNos(const Attrib::Gen::pvehicle &pvehicle);
bool HasRunflatTires(const Attrib::Gen::pvehicle &pvehicle);
float NosBoost(const Attrib::Gen::nos &nos, const Tunings *tunings);
float NosCapacity(const Attrib::Gen::nos &nos, const Tunings *tunings);
float InductionRPM(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, const Tunings *tunings);
float InductionBoost(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, float rpm, float spool, const Tunings *tunings,
                     float *psi);
FtLbs MaxTorque(const Attrib::Gen::engine &engine, Rpm &atrpm);
Rpm Redline(const Attrib::Gen::engine &engine);
Rpm Redline(const Attrib::Gen::pvehicle &pvehicle);
float Torque(const Attrib::Gen::engine &engine, float rpm);
Meters WheelDiameter(const Attrib::Gen::tires &tires, bool front);
Meters WheelDiameter(const Attrib::Gen::pvehicle &pvehicle, bool front);
float MaxInductedPower(const Attrib::Gen::pvehicle &pvehicle, const Tunings *tunings);
FtLbs AvgInductedTorque(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, const Attrib::Gen::transmission &transmission,
                        bool from_peak, const Tunings *tunings);
FtLbs MaxInductedTorque(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, float &atrpm, const Tunings *tunings);
FtLbs AvgInductedTorque(const Attrib::Gen::pvehicle &pvehicle, bool from_peak);
FtLbs MaxInductedTorque(const Attrib::Gen::pvehicle &pvehicle, Rpm &atrpm, const Tunings *tunings);
bool ShiftPoints(const Attrib::Gen::transmission &transmission, const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction,
                 float *shift_up, float *shift_down, unsigned int numpts);
Mps Speedometer(const Attrib::Gen::transmission &transmission, const Attrib::Gen::engine &engine, const Attrib::Gen::tires &tires, Rpm rpm,
                GearID gear, const Tunings *tunings);
unsigned int NumFowardGears(const Attrib::Gen::transmission &transmission);
unsigned int NumFowardGears(const Attrib::Gen::pvehicle &pvehicle);
bool HasPerformanceRatings(const Attrib::Gen::pvehicle &pvehicle);
// El ELF mangla EstimatePerformance__Q27Physics4InfoRCQ36Attrib3Gen8pvehicleRQ37Physics4Info11Performance:
// toma el pvehicle y el Performance. La declaracion de un solo parametro emitia un simbolo inexistente.
bool EstimatePerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf);
bool ComputeAccelerationTable(const Attrib::Gen::pvehicle &pvehicle, float &top_speed, float *table, int num_entries);
bool ComputePerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf);
bool GetStockPerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf);
bool GetMaximumPerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf);


} // namespace Info
} // namespace Physics

#endif
