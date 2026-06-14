#ifndef PHYSICSUPGRADES_HPP
#define PHYSICSUPGRADES_HPP

#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"

namespace Physics {

namespace Upgrades {

enum Type {
    PUT_TIRES = 0,
    PUT_BRAKES = 1,
    PUT_CHASSIS = 2,
    PUT_TRANSMISSION = 3,
    PUT_ENGINE = 4,
    PUT_INDUCTION = 5,
    PUT_NOS = 6,
    PUT_MAX = 7,
};

// total size: 0x20
struct Package {
    int Part[7]; // offset 0x0, size 0x1C
    int Junkman; // offset 0x1C, size 0x4
};

int GetLevel(const Attrib::Gen::pvehicle &vehicle, Type type);
int GetMaxLevel(const Attrib::Gen::pvehicle &vehicle, Type type);

}; // namespace Upgrades

}; // namespace Physics

#endif
