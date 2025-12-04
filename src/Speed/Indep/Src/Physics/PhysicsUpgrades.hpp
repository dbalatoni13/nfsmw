#ifndef PHYSICS_PHYSICSUPGRADES_H
#define PHYSICS_PHYSICSUPGRADES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Physics {

namespace Upgrades {

// total size: 0x20
struct Package {
    int Part[7]; // offset 0x0, size 0x1C
    int Junkman; // offset 0x1C, size 0x4
};

}; // namespace Upgrades

}; // namespace Physics

#endif
