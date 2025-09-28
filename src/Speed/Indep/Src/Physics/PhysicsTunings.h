#ifndef PHYSICS_PHYSICSTUNINGS_H
#define PHYSICS_PHYSICSTUNINGS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Physics {

struct Tunings {
    float steeringTuning;     // offset 0x0, size 0x4
    float handlingTuning;     // offset 0x4, size 0x4
    float brakesTuning;       // offset 0x8, size 0x4
    float rideHeightTuning;   // offset 0xC, size 0x4
    float aerodynamicsTuning; // offset 0x10, size 0x4
    float nitrousTuning;      // offset 0x14, size 0x4
    float turboTuning;        // offset 0x18, size 0x4

    // float LowerLimit(Path path);
    // float UpperLimit(Path path);
};

} // namespace Physics

#endif
