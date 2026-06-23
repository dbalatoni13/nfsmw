#ifndef PHYSICS_TUNINGS_H
#define PHYSICS_TUNINGS_H

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

    float Value[7]; // offset 0x0, size 0x1C
};

} // namespace Physics

#endif
