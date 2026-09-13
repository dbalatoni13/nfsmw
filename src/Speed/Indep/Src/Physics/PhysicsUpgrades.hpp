#ifndef PHYSICSUPGRADES_HPP
#define PHYSICSUPGRADES_HPP

#include "Speed/Indep/Src/Generated/AttribSys/Classes/presetride.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "types.h"

namespace Physics {

namespace Info {
struct Performance;
} // namespace Info

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
    Package() {}

    void Default() {
        bMemSet(this, 0, sizeof(*this));
        Junkman = 0;
    }

    int Part[7]; // offset 0x0, size 0x1C
    int Junkman; // offset 0x1C, size 0x4
};

float GetPercent(const Attrib::Gen::pvehicle &vehicle, Type type);
int GetLevel(const Attrib::Gen::pvehicle &vehicle, Type type);
void GetPackage(const Attrib::Gen::pvehicle &vehicle, Package &package);
bool SetPackage(Attrib::Gen::pvehicle &vehicle, const Package &package);
bool GetJunkman(const Attrib::Gen::pvehicle &vehicle, Type type);
bool CanInstallJunkman(const Attrib::Gen::pvehicle &vehicle, Type type);
bool SetJunkman(Attrib::Gen::pvehicle &vehicle, Type type);
bool ApplyPreset(Attrib::Gen::pvehicle &vehicle, const Attrib::Gen::presetride &presetride);
void RemovePart(Attrib::Gen::pvehicle &vehicle, Type type);
void RemoveJunkman(Attrib::Gen::pvehicle &vehicle, Type type);
bool Validate(const Attrib::Gen::pvehicle &vehicle, Type type);
bool Validate(const Attrib::Gen::pvehicle &vehicle);
int GetMaxLevel(const Attrib::Gen::pvehicle &vehicle, Type type);
bool SetMaximum(Attrib::Gen::pvehicle &pvehicle);
bool SetLevel(Attrib::Gen::pvehicle &vehicle, Type type, int level);
void Clear(Attrib::Gen::pvehicle &vehicle);
bool MatchPerformance(Attrib::Gen::pvehicle &vehicle, const Physics::Info::Performance &matched_performance);
void Flush();
float GetHeat(Attrib::Gen::pvehicle pvehicle, Type type, int level);

}; // namespace Upgrades

}; // namespace Physics

#endif
