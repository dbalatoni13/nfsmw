#ifndef GAMEPLAY_GRACE_H
#define GAMEPLAY_GRACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

namespace GRace {

typedef enum {
    kRaceContext_QuickRace,
    kRaceContext_TimeTrial,
#if ONLINE_SUPPORT
    kRaceContext_Online,
#endif
    kRaceContext_Career,
    kRaceContext_Count,
} Context;

typedef enum {
    kRaceType_None = -1,
    kRaceType_P2P = 0,
    kRaceType_Circuit = 1,
    kRaceType_Drag = 2,
    kRaceType_Knockout = 3,
    kRaceType_Tollbooth = 4,
    kRaceType_SpeedTrap = 5,
    kRaceType_Checkpoint = 6,
    kRaceType_CashGrab = 7,
    kRaceType_Challenge = 8,
    kRaceType_JumpToSpeedTrap = 9,
    kRaceType_JumpToMilestone = 10,
    kRaceType_NumTypes = 11
} Type;

typedef enum { kRaceRegion_None = -1, kRaceRegion_College = 0, kRaceRegion_Coastal = 1, kRaceRegion_City = 2, kRaceRegion_NumRegions = 3 } Region;

typedef enum { kRaceDifficulty_Easy = 0, kRaceDifficulty_Medium = 1, kRaceDifficulty_Hard = 2, kRaceDifficulty_NumDifficulties = 3 } Difficulty;

typedef enum {
    kRaceTraffic_Off = 0,
    kRaceTraffic_Light = 1,
    kRaceTraffic_Medium = 2,
    kRaceTraffic_Heavy = 3,
    kRaceTraffic_NumLevels = 4
} TrafficLevel;

typedef enum { kRaceCops_Off = 0, kRaceCops_Light = 1, kRaceCops_Medium = 2, kRaceCops_Heavy = 3, kRaceCops_NumDensities = 4 } CopDensity;

}; // namespace GRace

#endif
