#ifndef PHYSICS_PHYSICSTUNINGS_H
#define PHYSICS_PHYSICSTUNINGS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Physics {

struct Tunings {
    enum Path {
        STEERING = 0,
        HANDLING = 1,
        BRAKES = 2,
        RIDEHEIGHT = 3,
        AERODYNAMICS = 4,
        NOS = 5,
        INDUCTION = 6,
        MAX_TUNINGS = 7,
    };
    float Value[7]; // offset 0x0, size 0x1C

    static float LowerLimit(Path path);
    static float UpperLimit(Path path);
};

} // namespace Physics

#endif
