#ifndef FRONTEND_DATABASE_VEHICLEDB_H
#define FRONTEND_DATABASE_VEHICLEDB_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"

// total size: 0x14
struct FECarRecord {
    unsigned int Handle;         // offset 0x0, size 0x4
    unsigned int FEKey;          // offset 0x4, size 0x4
    unsigned int VehicleKey;     // offset 0x8, size 0x4
    unsigned int FilterBits;     // offset 0xC, size 0x4
    unsigned char Customization; // offset 0x10, size 0x1
    unsigned char CareerHandle;  // offset 0x11, size 0x1
    unsigned short Padd;         // offset 0x12, size 0x2
};

// total size: 0x198
struct FECustomizationRecord {
    short InstalledPartIndices[139];             // offset 0x0, size 0x116
    Physics::Upgrades::Package InstalledPhysics; // offset 0x118, size 0x20
    Physics::Tunings Tunings[3];                 // offset 0x138, size 0x54
    Physics::eCustomTuningType ActiveTuning;     // offset 0x18C, size 0x4
    int Preset;                                  // offset 0x190, size 0x4
    unsigned char Handle;                        // offset 0x194, size 0x1
};

// total size: 0x8
struct FEImpoundData {
    enum eImpoundReasons {
        IMPOUND_REASON_NONE = 0,
        IMPOUND_REASON_BUSTED_BY_CROSS = 1,
        IMPOUND_REASON_STRIKE_LIMIT_REACHED = 2,
        IMPOUND_REASON_INSUFFICIENT_FUNDS = 3,
        IMPOUND_RELEASED = 4,
    };
    unsigned char MaxBusted;         // offset 0x0, size 0x1
    char TimesBusted;                // offset 0x1, size 0x1
    char ImpoundedState;             // offset 0x2, size 0x1
    unsigned char DaysBeforeRelease; // offset 0x3, size 0x1
    char EvadeCount;                 // offset 0x4, size 0x1
    char Pad1;                       // offset 0x5, size 0x1
    short Pad2;                      // offset 0x6, size 0x2
};

// total size: 0x10
struct FEInfractionsData {
    unsigned short Speeding;  // offset 0x0, size 0x2
    unsigned short Racing;    // offset 0x2, size 0x2
    unsigned short Reckless;  // offset 0x4, size 0x2
    unsigned short Assault;   // offset 0x6, size 0x2
    unsigned short HitAndRun; // offset 0x8, size 0x2
    unsigned short Damage;    // offset 0xA, size 0x2
    unsigned short Resist;    // offset 0xC, size 0x2
    unsigned short OffRoad;   // offset 0xE, size 0x2
};

// total size: 0x38
struct FECareerRecord {
    unsigned char Handle;                  // offset 0x0, size 0x1
    FEImpoundData TheImpoundData;          // offset 0x2, size 0x8
    float VehicleHeat;                     // offset 0xC, size 0x4
    unsigned int Bounty;                   // offset 0x10, size 0x4
    unsigned short NumEvadedPursuits;      // offset 0x14, size 0x2
    unsigned short NumBustedPursuits;      // offset 0x16, size 0x2
    FEInfractionsData UnservedInfractions; // offset 0x18, size 0x10
    FEInfractionsData ServedInfractions;   // offset 0x28, size 0x10
};

// total size: 0x8CC8
class FEPlayerCarDB {
  public:
    // total size: 0x4
    class MyCallback {};

  private:
    FECarRecord CarTable[200];                        // offset 0x0, size 0xFA0
    FECustomizationRecord Customizations[75];         // offset 0xFA0, size 0x7788
    FECareerRecord CareerRecords[25];                 // offset 0x8728, size 0x578
    unsigned int SoldHistoryBounty;                   // offset 0x8CA0, size 0x4
    unsigned short SoldHistoryNumEvadedPursuits;      // offset 0x8CA4, size 0x2
    unsigned short SoldHistoryNumBustedPursuits;      // offset 0x8CA6, size 0x2
    FEInfractionsData SoldHistoryUnservedInfractions; // offset 0x8CA8, size 0x10
    FEInfractionsData SoldHistoryServedInfractions;   // offset 0x8CB8, size 0x10
};

#endif
