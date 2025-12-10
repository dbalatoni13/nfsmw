#ifndef GAMEPLAY_GRACEDATABASE_H
#define GAMEPLAY_GRACEDATABASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GRaceStatus.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

enum Context {
    kRaceContext_QuickRace = 0,
    kRaceContext_Online = 1,
    kRaceContext_Career = 2,
    kRaceContext_Count = 3,
};

// total size: 0x40
class GRaceDatabase {
  public:
    enum ScoreFlags {
        kCompleted_ContextQuickRace = 1 << 0,
        kCompleted_ContextCareer = 1 << 1,
        kCompleted_ContextAny = 3,
        kUnlocked_QuickRace = 1 << 2,
        kUnlocked_Career = 1 << 3,
        kUnlocked_Online = 1 << 4,
    };

    GRaceCustom *GetStartupRace();
    void SetStartupRace(GRaceCustom *custom, Context context);
    void FreeCustomRace(GRaceCustom *custom);
    GRaceParameters *GetRaceFromHash(unsigned int hash);
    GRaceCustom *AllocCustomRace(GRaceParameters *parms);

    static GRaceDatabase &Get() {
        return *mObj;
    }

    GRaceParameters *GetRaceFromName(const char *name) {
        return GetRaceFromHash(Attrib::StringHash32(name));
    }

  private:
    unsigned int mRaceCountStatic;           // offset 0x0, size 0x4
    unsigned int mRaceCountDynamic;          // offset 0x4, size 0x4
    struct GRaceIndexData *mRaceIndex;       // offset 0x8, size 0x4
    struct GRaceParameters *mRaceParameters; // offset 0xC, size 0x4
    struct GRaceCustom *mRaceCustom[4];      // offset 0x10, size 0x10
    unsigned int mBinCount;                  // offset 0x20, size 0x4
    struct GRaceBin *mBins;                  // offset 0x24, size 0x4
    struct Class *mGameplayClass;            // offset 0x28, size 0x4
    struct GRaceCustom *mStartupRace;        // offset 0x2C, size 0x4
    Context mStartupRaceContext;             // offset 0x30, size 0x4
    unsigned int mNumInitialUnlocks;         // offset 0x34, size 0x4
    unsigned int *mInitialUnlockHash;        // offset 0x38, size 0x4
    struct GRaceSaveInfo *mRaceScoreInfo;    // offset 0x3C, size 0x4

    static GRaceDatabase *mObj;
};

#endif
