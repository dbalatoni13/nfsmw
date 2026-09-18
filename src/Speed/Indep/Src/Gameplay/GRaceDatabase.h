#ifndef GAMEPLAY_GRACEDATABASE_H
#define GAMEPLAY_GRACEDATABASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/FixedPoint.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Src/Gameplay/GRace.h"
class GActivity;
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

enum Context {
    kRaceContext_QuickRace = 0,
    kRaceContext_Online = 1,
    kRaceContext_Career = 2,
    kRaceContext_Count = 3,
};

#include "GVault.h"

class GRaceCustom;
class GRaceParameters;

typedef union { // 0x4
    float mBestTime;
    float mBestCash;
    unsigned int mBestPoints;
    float mBestSpeed;
} GHighScoresUnion;

// total size: 0x10
struct GRaceSaveInfo {
    unsigned int mRaceHash;       // offset 0x0, size 0x4
    unsigned int mFlags;          // offset 0x4, size 0x4
    GHighScoresUnion mHighScores; // offset 0x8, size 0x4
    FixedPoint<unsigned short, 10, 2> mTopSpeed;     // offset 0xC, size 0x2
    FixedPoint<unsigned short, 10, 2> mAverageSpeed; // offset 0xE, size 0x2

    GRaceSaveInfo() {}
};

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
    friend class GRaceDatabase;

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
    bool IsOnlineRaceUnlocked(unsigned int eventHash) {
        return CheckRaceScoreFlags(eventHash, kUnlocked_Online);
    }

    const char *GetFinalEpicChaseRace() const {
        return "1.8.1";
    }

    enum ScoreFlags {
        kCompleted_ContextQuickRace = 1 << 0,
        kCompleted_ContextCareer = 1 << 1,
        kCompleted_ContextAny = 3,
        kUnlocked_QuickRace = 1 << 2,
        kUnlocked_Career = 1 << 3,
        kUnlocked_Online = 1 << 4,
    };

    static void Init();

  private:
    GRaceDatabase();

    void BuildBinList();
    unsigned int StoreBinList(GRaceBin *list);
    void BuildRaceList();
    unsigned int StoreRaceList(GRaceParameters *list);
    void BuildScoreList();
    friend class GManager;
    unsigned int SerializeBins(unsigned char *dest);
    unsigned int DeserializeBins(unsigned char *src);
    void RefreshBinProgress();
    bool CollectionIsRaceActivity(Attrib::Gen::gameplay &instanceObj);
    bool CollectionIsRaceBin(Attrib::Gen::gameplay &instanceObj);
    void DestroyCustomRace(GRaceCustom *custom);

  public:
    void UpdateRaceScore(bool setComplete);

    GRaceCustom *GetStartupRace();
    void SetStartupRace(GRaceCustom *custom, GRace::Context context);
    void ClearStartupRace(); // ClearStartupRace__13GRaceDatabase @ 0x801A45BC
    void FreeCustomRace(GRaceCustom *custom);
    GRaceParameters *GetRaceFromHash(unsigned int hash);
    GRaceParameters *GetRaceFromActivity(GActivity *activity);
    GRaceCustom *AllocCustomRace(GRaceParameters *parms);
    GRaceBin *GetBinNumber(int number);
    bool CheckRaceScoreFlags(unsigned int eventHash, ScoreFlags mask);
    const char *GetNextDDayRace();
    GRaceSaveInfo *GetScoreInfo(unsigned int eventHash);
    void LoadBestScores(GRaceSaveInfo *entries, unsigned int count);
    void SimulateDDayComplete();
    void ClearRaceScores();
    void NotifyVaultLoaded(GVault *vault);
    void NotifyVaultUnloading(GVault *vault);
    GRaceParameters *GetRaceFromKey(unsigned int collectionKey);
    unsigned int GetBinCount();
    GRaceBin *GetBin(unsigned int index);
    Context GetStartupRaceContext();
    void ResetCareerCompleteFlag(unsigned int eventHash);
    unsigned int GetRaceCount();
    GRaceParameters *GetRaceParameters(unsigned int index);

    bool IsCareerRaceComplete(unsigned int eventHash) {
        return CheckRaceScoreFlags(eventHash, kCompleted_ContextCareer);
    }

    bool IsQuickRaceComplete(unsigned int eventHash) {
        return CheckRaceScoreFlags(eventHash, kCompleted_ContextQuickRace);
    }

    bool IsCareerRaceUnlocked(unsigned int eventHash) {
        return CheckRaceScoreFlags(eventHash, kUnlocked_Career);
    }

    bool IsQuickRaceUnlocked(unsigned int eventHash) {
        return CheckRaceScoreFlags(eventHash, kUnlocked_QuickRace);
    }

    const char *GetBurgerKingRace() const {
        return "19.8.31";
    }

    const char *GetDDayStartRace() const {
        return sDDayRaces[0];
    }

    const char *GetDDayEndRace() const {
        return "16.2.1";
    }

    const char *GetFinalBossRace() const {
        return "1.2.3";
    }

    GRaceSaveInfo *GetScoreInfo() {
        return mRaceScoreInfo;
    }

    unsigned int GetScoreInfoCount() {
        return mRaceCountStatic;
    }

    static const char sDDayRaces[5][8];

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
