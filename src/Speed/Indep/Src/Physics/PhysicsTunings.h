#ifndef PHYSICS_TUNINGS_H
#define PHYSICS_TUNINGS_H

#include "Speed/Indep/bWare/Inc/bWare.hpp"

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

    static float LowerLimit(Path path);
    static float UpperLimit(Path path);

    Tunings() {
        Default();
    }

    void Default() {
        bMemSet(this, 0, sizeof(*this));
    }

    float Value[7]; // offset 0x0, size 0x1C
};

} // namespace Physics

#endif
