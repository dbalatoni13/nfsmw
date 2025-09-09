#ifndef PHYSICS_PHYSICSTUNINGS_H
#define PHYSICS_PHYSICSTUNINGS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Physics {

struct Tunings {
    float steeringTuning;
    float handlingTuning;
    float brakesTuning;
    float rideHeightTuning;
    float aerodynamicsTuning;
    float nitrousTuning;
    float turboTuning;

    // float LowerLimit(Path path);
    // float UpperLimit(Path path);
};

} // namespace Physics

#endif
