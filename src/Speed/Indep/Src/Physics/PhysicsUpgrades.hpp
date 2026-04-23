#ifndef PHYSICS_PHYSICSUPGRADES_H
#define PHYSICS_PHYSICSUPGRADES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Attrib { namespace Gen { struct pvehicle; } }

namespace Physics {

namespace Upgrades {

enum Type {
    kType_Engine = 0,
    kType_Transmission = 1,
    kType_Chassis = 2,
    kType_Nitrous = 3,
    kType_Tires = 4,
    kType_Brakes = 5,
    kType_Induction = 6,
    kType_Count = 7,
};

// total size: 0x20
struct Package {
    int Part[7]; // offset 0x0, size 0x1C
    int Junkman; // offset 0x1C, size 0x4
};

int GetMaxLevel(const Attrib::Gen::pvehicle &vehicle, Type type);
int GetLevel(const Attrib::Gen::pvehicle &vehicle, Type type);
void SetLevel(Attrib::Gen::pvehicle &vehicle, Type type, int level);

}; // namespace Upgrades

}; // namespace Physics

#endif
