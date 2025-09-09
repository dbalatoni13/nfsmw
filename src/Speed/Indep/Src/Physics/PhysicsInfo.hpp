#ifndef PHYSICS_PHYSICSINFO_H
#define PHYSICS_PHYSICSINFO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "PhysicsTunings.h"
#include "PhysicsTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/chassis.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/induction.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/nos.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"

using namespace Attrib::Gen;

namespace Physics {
namespace Info {

enum eInductionType {
    INDUCTION_SUPER_CHARGER = 2,
    INDUCTION_TURBO_CHARGER = 1,
    INDUCTION_NONE = 0,
};

float AerodynamicDownforce(const chassis &chassis, const float speed);
float EngineInertia(const engine &engine, const bool loaded);
eInductionType InductionType(const induction &induction);
float NosBoost(const nos &nos, const Tunings *tunings);
float NosCapacity(const nos &nos, const Tunings *tunings);
float InductionRPM(const engine &engine, const induction &induction, const Tunings *tunings);
float InductionBoost(const engine &engine, const induction &induction, float rpm, float spool, const Tunings *tunings, float *psi);
float WheelDiameter(const tires &tires, bool front);
float Speedometer(const transmission &transmission, const engine &engine, const tires &tires, float rpm, GearID gear, const Tunings *tunings);
float Torque(const engine &engine, float rpm);
bool ShiftPoints(const transmission &transmission, const engine &engine, const induction &induction, float *shift_up, float *shift_down,
                 unsigned int numpts);

struct Performance {
    Performance(float topspeed, float handling, float accel) {
        TopSpeed = topspeed;
        Handling = handling;
        Acceleration = accel;
    }
    void Default();

    float TopSpeed;
    float Handling;
    float Acceleration;
};

extern Performance PerformanceWeights[7];

bool EstimatePerformance(Performance &perf);

} // namespace Info
} // namespace Physics

#endif
