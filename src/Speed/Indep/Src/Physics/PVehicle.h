#ifndef PHYSICS_PVEHICLE_H
#define PHYSICS_PVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

// total size: 0x30
struct VehicleParams : public Sim::Param {
    DriverClass carClass;                       // offset 0x10, size 0x4
    unsigned int carType;                       // offset 0x14, size 0x4
    const UMath::Vector3 &initialVec;           // offset 0x18, size 0x4
    const UMath::Vector3 &initialPos;           // offset 0x1C, size 0x4
    const FECustomizationRecord *customization; // offset 0x20, size 0x4
    IVehicleCache *VehicleCache;                // offset 0x24, size 0x4
    const Physics::Info::Performance *matched;  // offset 0x28, size 0x4
    unsigned int Flags;                         // offset 0x2C, size 0x4

    VehicleParams(IVehicleCache *cache, DriverClass driver, unsigned int type, const UMath::Vector3 &dir, const UMath::Vector3 &pos,
                  unsigned int flags, const FECustomizationRecord *upgrades, const Physics::Info::Performance *match)
        : Sim::Param(UCrc32(0xa6b47fac), this), //
          carClass(driver),                     //
          carType(type),                        //
          initialVec(dir),                      //
          initialPos(pos),                      //
          customization(upgrades),              //
          VehicleCache(cache),                  //
          matched(match),                       //
          Flags(flags) {}

    static UCrc32 TypeName() {
        static UCrc32 value = UCrc32("VehicleParams");
        return value;
    }
};

#endif
