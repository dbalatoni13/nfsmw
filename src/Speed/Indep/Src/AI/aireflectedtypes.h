#ifndef _aireflectedtypes_h_
#define _aireflectedtypes_h_

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

enum FormationType {
    PIT = 1,
    BOX_IN = 2,
    ROLLING_BLOCK = 3,
    FOLLOW = 4,
    HELI_PURSUIT = 5,
    HERD = 6,
    STAGGER_FOLLOW = 7,
};

struct CopFormationRecord {
    FormationType Formation; // offset 0x0, size 0x4
    float Duration;          // offset 0x4, size 0x4
    float Frequency;         // offset 0x8, size 0x4
};

enum AirSupportStrategy {
    HI_PATROL = 0,
    PURSUIT = 1,
    SKID_HIT = 2,
    SPIKE_DROP = 3,
};

struct AirSupport {
    AirSupportStrategy HeliStrategy; // offset 0x0, size 0x4
    uint32 Chance;                   // offset 0x4, size 0x4
    float Duration;                  // offset 0x8, size 0x4
};

enum HeavySupportStrategy {
    E_BRAKE = 1,
    COORDINATED_E_BRAKE = 2,
    RAM = 3,
    HEAVY_ROADBLOCK = 4,
};

enum LeaderSupportStrategy {
    CROSS_FOLLOW = 5,
    CROSS_BRAKE = 6,
    CROSS_PLUS_V_BLOCK = 7,
};

struct HeavySupport {
    HeavySupportStrategy HeavyStrategy; // offset 0x0, size 0x4
    uint32_t Chance;                    // offset 0x4, size 0x4
    float Duration;                     // offset 0x8, size 0x4
    uint32_t ChanceBigSUV;              // offset 0xC, size 0x4
};

struct LeaderSupport {
    LeaderSupportStrategy LeaderStrategy; // offset 0x0, size 0x4
    uint32_t Chance;                      // offset 0x4, size 0x4
    float Duration;                       // offset 0x8, size 0x4
    uint32_t PriorityChance;              // offset 0xC, size 0x4
    float PriorityTime;                   // offset 0x10, size 0x4
};

// total size: 0x18
struct CopCountRecord {
    Attrib::StringKey CopType; // offset 0x0, size 0x10
    unsigned int Count;        // offset 0x10, size 0x4
    unsigned int Chance;       // offset 0x14, size 0x4

    CopCountRecord &operator=(const CopCountRecord &_ctor_arg) {}
};

// total size: 0x10
struct AICollisionReactionRecord {
    // AICollisionReactionRecord &operator=(const AICollisionReactionRecord &_ctor_arg) {}

    uint32 Goal;              // offset 0x0, size 0x4
    Attrib::RefSpec Reaction; // offset 0x4, size 0xC
};

#endif
