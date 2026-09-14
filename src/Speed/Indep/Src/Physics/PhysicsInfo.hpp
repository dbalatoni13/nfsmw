#ifndef PHYSICSINFO_HPP
#define PHYSICSINFO_HPP

#include "PhysicsTunings.h"
#include "PhysicsTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/induction.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/nos.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

DECLARE_CONTAINER_TYPE(PerformanceMaps);

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

    void Default();

    void Maximize(const Performance &other) {
        TopSpeed = UMath::Max(TopSpeed, other.TopSpeed);
        Handling = UMath::Max(Handling, other.Handling);
        Acceleration = UMath::Max(Acceleration, other.Acceleration);
    }

    float TopSpeed;
    float Handling;
    float Acceleration;
};

inline void Performance::Default() {
    TopSpeed = 0.0f;
    Handling = 0.0f;
    Acceleration = 0.0f;
}

} // namespace Info
} // namespace Physics

// total size: 0xC
struct PerfStats {
    PerfStats() {
        bMemSet(this, 0, sizeof(PerfStats));
    }

    bool Fetch(const Attrib::Gen::pvehicle &pvehicle, bVector2 *graph_data, int *num_data);

    float Time0To100;      // offset 0x0, size 0x4
    float TopSpeed;        // offset 0x4, size 0x4
    float HandlingRating;  // offset 0x8, size 0x4
};

// total size: 0x2C
struct PerfLevel {
    PerfLevel(unsigned int key)
        : Stats(),       //
          Stock(),       //
          Upgraded(),    //
          Key(key),      //
          Analyzed(false) {}

    bool Analyze(const Attrib::Gen::pvehicle &pvehicle);
    void Rate();
    void Print(const char * = nullptr);

    PerfStats Stats;                     // offset 0x0, size 0xC
    Physics::Info::Performance Stock;    // offset 0xC, size 0xC
    Physics::Info::Performance Upgraded; // offset 0x18, size 0xC
    unsigned int Key;                    // offset 0x24, size 0x4
    bool Analyzed;                       // offset 0x28, size 0x4
};

struct PerformanceMaps : public UTL::Std::list<PerfLevel, _type_PerformanceMaps> {
    void FindLimits(float direction, PerfStats &out) const;
};

namespace Physics {
namespace Info {

void Init();

float AerodynamicDownforce(const Attrib::Gen::chassis &chassis, const float speed);
float EngineInertia(const Attrib::Gen::engine &engine, const bool loaded);
eInductionType InductionType(const Attrib::Gen::pvehicle &pvehicle);
eInductionType InductionType(const Attrib::Gen::induction &induction);
eInductionType InductionType(const Attrib::Gen::pvehicle &pvehicle);
bool HasNos(const Attrib::Gen::pvehicle &pvehicle);
bool HasRunflatTires(const Attrib::Gen::pvehicle &pvehicle);
float NosBoost(const Attrib::Gen::nos &nos, const Tunings *tunings);
float NosCapacity(const Attrib::Gen::nos &nos, const Tunings *tunings);
float InductionRPM(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, const Tunings *tunings);
float InductionBoost(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, float rpm, float spool, const Tunings *tunings,
                     float *psi);
float Torque(const Attrib::Gen::engine &engine, float rpm);
float MaxTorque(const Attrib::Gen::engine &engine, float &atrpm);
Meters WheelDiameter(const Attrib::Gen::tires &tires, bool front);
Meters WheelDiameter(const Attrib::Gen::pvehicle &pvehicle, bool front);
float Redline(const Attrib::Gen::engine &engine);
float Redline(const Attrib::Gen::pvehicle &pvehicle);
unsigned int NumFowardGears(const Attrib::Gen::transmission &transmission);
unsigned int NumFowardGears(const Attrib::Gen::pvehicle &pvehicle);
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
bool HasPerformanceRatings(const Attrib::Gen::pvehicle &pvehicle);
bool EstimatePerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf);
bool EstimatePerformance(Performance &perf);
bool ComputePerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf);
bool GetStockPerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf);
bool GetMaximumPerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf);
bool ComputeAccelerationTable(const Attrib::Gen::pvehicle &pvehicle, float &top_speed, float *table, int num_entries);
void FindPerformanceCandidates(const Performance &minimum_perf, const Performance &maximum_perf, UTL::Std::list<unsigned int, _type_list> &candidates);

extern Performance PerformanceWeights[7];

} // namespace Info
} // namespace Physics

#endif
