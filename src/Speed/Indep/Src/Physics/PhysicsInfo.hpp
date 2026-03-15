#ifndef PHYSICS_PHYSICSINFO_H
#define PHYSICS_PHYSICSINFO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "PhysicsTunings.h"
#include "PhysicsTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/induction.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/nos.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

class _type_list;
DECLARE_CONTAINER_TYPE(PerformanceMaps);

namespace Physics {
namespace Info {

enum eInductionType {
    INDUCTION_SUPER_CHARGER = 2,
    INDUCTION_TURBO_CHARGER = 1,
    INDUCTION_NONE = 0,
};

struct Performance {
    Performance() {}

    Performance(float topspeed, float handling, float accel)
        : TopSpeed(topspeed) //
        , Handling(handling) //
        , Acceleration(accel) {}

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

} // namespace Info
} // namespace Physics

struct PerfStats {
    bool Fetch(const Attrib::Gen::pvehicle &pvehicle, bVector2 *graph_data, int *num_data);

    float Time0To100;
    float TopSpeed;
    float HandlingRating;
};

struct PerfLevel {
    PerfLevel(unsigned int key) //
        : Stats() //
        , Stock() //
        , Upgraded() //
        , Key(key) //
        , Analyzed(false) {}

    bool Analyze(const Attrib::Gen::pvehicle &pvehicle);
    void Rate();
    void Print(const char * = nullptr);

    PerfStats Stats;
    Physics::Info::Performance Stock;
    Physics::Info::Performance Upgraded;
    unsigned int Key;
    bool Analyzed;
};

struct PerformanceMaps : public UTL::Std::list<PerfLevel, _type_PerformanceMaps> {
    void FindLimits(float direction, PerfStats &out) const;
};

namespace Physics {
namespace Info {

void Init();

float AerodynamicDownforce(const Attrib::Gen::chassis &chassis, float speed);
float EngineInertia(const Attrib::Gen::engine &engine, bool loaded);
eInductionType InductionType(const Attrib::Gen::induction &induction);
bool HasNos(const Attrib::Gen::pvehicle &pvehicle);
bool HasRunflatTires(const Attrib::Gen::pvehicle &pvehicle);
float NosBoost(const Attrib::Gen::nos &nos, const Tunings *tunings);
float NosCapacity(const Attrib::Gen::nos &nos, const Tunings *tunings);
float InductionRPM(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, const Tunings *tunings);
float InductionBoost(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction, float rpm, float spool, const Tunings *tunings,
                     float *psi);
float Torque(const Attrib::Gen::engine &engine, float rpm);
float WheelDiameter(const Attrib::Gen::tires &tires, bool front);
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
bool EstimatePerformance(Performance &perf);
eInductionType InductionType(const Attrib::Gen::pvehicle &pvehicle);
float WheelDiameter(const Attrib::Gen::pvehicle &pvehicle, bool front);
float Redline(const Attrib::Gen::engine &engine);
float Redline(const Attrib::Gen::pvehicle &pvehicle);
unsigned int NumFowardGears(const Attrib::Gen::transmission &transmission);
unsigned int NumFowardGears(const Attrib::Gen::pvehicle &pvehicle);
float MaxInductedPower(const Attrib::Gen::pvehicle &pvehicle, const Tunings *tunings);
float AvgInductedTorque(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction,
                        const Attrib::Gen::transmission &transmission, bool loaded, const Tunings *tunings);
float MaxInductedTorque(const Attrib::Gen::engine &engine, const Attrib::Gen::induction &induction,
                        float &atrpm, const Tunings *tunings);
float MaxInductedTorque(const Attrib::Gen::pvehicle &pvehicle, float &atrpm, const Tunings *tunings);
float MaxTorque(const Attrib::Gen::engine &engine, float &atrpm);
void Init();
void FindPerformanceCandidates(const Performance &minimum_perf, const Performance &maximum_perf,
                               UTL::Std::list<unsigned int, _type_list> &candidates);
bool HasPerformanceRatings(const Attrib::Gen::pvehicle &pvehicle);
bool EstimatePerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf);
bool ComputePerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf);
bool GetStockPerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf);
bool GetMaximumPerformance(const Attrib::Gen::pvehicle &pvehicle, Performance &perf);
bool ComputeAccelerationTable(const Attrib::Gen::pvehicle &pvehicle, float &top_speed, float *table, int num_entries);

extern Performance PerformanceWeights[7];

} // namespace Info
} // namespace Physics

#endif
