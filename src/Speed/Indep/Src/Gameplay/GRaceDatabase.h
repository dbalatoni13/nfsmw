#ifndef GAMEPLAY_GRACEDATABASE_H
#define GAMEPLAY_GRACEDATABASE_H

#include "GRaceStatus.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

enum Context {
    kRaceContext_Count = 3,
    kRaceContext_Career = 2,
    kRaceContext_Online = 1,
    kRaceContext_QuickRace = 0,
};

struct GRaceDatabase {
    // total size: 0x40
    enum ScoreFlags {
        kUnlocked_Online = 16,
        kUnlocked_Career = 8,
        kUnlocked_QuickRace = 4,
        kCompleted_ContextAny = 3,
        kCompleted_ContextCareer = 2,
        kCompleted_ContextQuickRace = 1,
    };

    GRaceCustom *GetStartupRace();

    unsigned int mRaceCountStatic; // offset 0x0, size 0x4
    unsigned int mRaceCountDynamic; // offset 0x4, size 0x4
    struct GRaceIndexData * mRaceIndex; // offset 0x8, size 0x4
    struct GRaceParameters * mRaceParameters; // offset 0xC, size 0x4
    struct GRaceCustom * mRaceCustom[4]; // offset 0x10, size 0x10
    unsigned int mBinCount; // offset 0x20, size 0x4
    struct GRaceBin * mBins; // offset 0x24, size 0x4
    struct Class * mGameplayClass; // offset 0x28, size 0x4
    struct GRaceCustom * mStartupRace; // offset 0x2C, size 0x4
    Context mStartupRaceContext; // offset 0x30, size 0x4
    unsigned int mNumInitialUnlocks; // offset 0x34, size 0x4
    unsigned int * mInitialUnlockHash; // offset 0x38, size 0x4
    struct GRaceSaveInfo * mRaceScoreInfo; // offset 0x3C, size 0x4

    static GRaceDatabase *mObj;
};

#endif
