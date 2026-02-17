#ifndef GAMEPLAY_GRACEDATABASE_H
#define GAMEPLAY_GRACEDATABASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

enum Context {
    kRaceContext_QuickRace = 0,
    kRaceContext_Online = 1,
    kRaceContext_Career = 2,
    kRaceContext_Count = 3,
};

class GVault;
class GRaceCustom;
class GRaceParameters;

// total size: 0x1C
class GRaceBin {
  public:
    // total size: 0x4
    struct BinStats {
        uint16 mChallengesCompleted; // offset 0x0, size 0x2
        uint16 mRacesWon;            // offset 0x2, size 0x2
    };

    GRaceBin(unsigned int collectionKey);

    ~GRaceBin() {}

    unsigned int GetCollectionKey() const;

    const Attrib::Gen::gameplay *GetGameplayObj() const;

    GVault *GetChildVault() const;

    int GetBinNumber() const;

    int GetBossReputation() const;

    float GetBaseOpenWorldHeat() const;

    float GetMaxOpenWorldHeat() const;

    float GetScaleOpenWorldHeat() const;

    unsigned int GetBossKey() const;

    unsigned int GetBossRaceCount() const;

    unsigned int GetBossRaceHash(unsigned int index) const;

    unsigned int GetWorldRaceCount() const;

    unsigned int GetWorldRaceHash(unsigned int index) const;

    unsigned int GetJumpRaceCount() const;

    unsigned int GetJumpRaceHash(unsigned int index) const;

    unsigned int GetBaselineUnlockCount() const;

    unsigned int GetBaselineUnlock(unsigned int index) const;

    unsigned int GetBarrierCount() const;

    const char *GetBarrierName(unsigned int index) const;

    unsigned int GetBarrierHash(unsigned int index) const;

    bool GetBarrierIsFlipped(unsigned int index) const;

    void EnableBarriers();

    void DisableBarriers();

    int GetRequiredBounty() const;

    int GetRequiredChallenges() const;

    int GetRequiredRaceWins() const;

    int GetCompletedChallenges() const;

    int GetAwardedRaceWins() const;

    void RefreshProgress();

  protected:
    unsigned int Serialize(unsigned char *dest);

    unsigned int Deserialize(unsigned char *src);

    void SetCompletedChallenges(int numChallenges);

    void SetRacesWon(int numRaces);

    Attrib::Gen::gameplay mBinRecord; // offset 0x0, size 0x14
    GVault *mChildVault;              // offset 0x14, size 0x4
    BinStats mStats;                  // offset 0x18, size 0x4
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

    static void Init();

    GRaceCustom *GetStartupRace();
    void SetStartupRace(GRaceCustom *custom, Context context);
    void FreeCustomRace(GRaceCustom *custom);
    GRaceParameters *GetRaceFromHash(unsigned int hash);
    GRaceCustom *AllocCustomRace(GRaceParameters *parms);

    static GRaceDatabase &Get() {
        return *mObj;
    }

    static bool Exists() {
        return mObj != nullptr;
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
    GRaceBin *mBins;                         // offset 0x24, size 0x4
    Attrib::Class *mGameplayClass;           // offset 0x28, size 0x4
    struct GRaceCustom *mStartupRace;        // offset 0x2C, size 0x4
    Context mStartupRaceContext;             // offset 0x30, size 0x4
    unsigned int mNumInitialUnlocks;         // offset 0x34, size 0x4
    unsigned int *mInitialUnlockHash;        // offset 0x38, size 0x4
    struct GRaceSaveInfo *mRaceScoreInfo;    // offset 0x3C, size 0x4

    static GRaceDatabase *mObj;
};

#endif
