#ifndef GAMEPLAY_GRACE_H
#define GAMEPLAY_GRACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

namespace GRace {

enum Difficulty {
    kRaceDifficulty_Easy = 0,
    kRaceDifficulty_Medium = 1,
    kRaceDifficulty_Hard = 2,
    kRaceDifficulty_NumDifficulties = 3,
};

enum Context {
    kRaceContext_QuickRace,
    kRaceContext_TimeTrial,
#if ONLINE_SUPPORT
    kRaceContext_Online,
#endif
    kRaceContext_Career,
    kRaceContext_Count,
};

enum Type {
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
    kRaceType_NumTypes = 11,
};

}; // namespace GRace

#endif
