#ifndef PHYSICSUPGRADES_HPP
#define PHYSICSUPGRADES_HPP

#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/presetride.h"
#include "types.h"

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
    int Part[7];   // offset 0x0, size 0x1C
    int32 Junkman; // offset 0x1C, size 0x4
};

int GetMaxLevel(const Attrib::Gen::pvehicle &vehicle, Type type);
int GetLevel(const Attrib::Gen::pvehicle &vehicle, Type type);
void SetLevel(Attrib::Gen::pvehicle &vehicle, Type type, int level);

bool ApplyPreset(Attrib::Gen::pvehicle &vehicle, const Attrib::Gen::presetride &preset);
void Clear(Attrib::Gen::pvehicle &vehicle);
void Flush();
bool SetPackage(Attrib::Gen::pvehicle &vehicle, const Package &package);
void GetPackage(const Attrib::Gen::pvehicle &vehicle, Package &package);

bool CanInstallJunkman(const Attrib::Gen::pvehicle &pvehicle, Type type);
bool SetJunkman(Attrib::Gen::pvehicle &vehicle, Type type);

void SetLevel(Attrib::Gen::pvehicle &pvehicle, Type type, int level);

float GetHeat(Attrib::Gen::pvehicle pvehicle, Type type, int level);

}; // namespace Upgrades

}; // namespace Physics

#endif
