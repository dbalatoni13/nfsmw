#ifndef GAMEPLAY_GRACEDATABASE_H
#define GAMEPLAY_GRACEDATABASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/Src/Gameplay/GActivity.h"
#include "GRace.h"
#include "GMarker.h"
#include "Speed/Indep/Src/Gameplay/GCharacter.h"
#include "Speed/Indep/Src/Misc/PackedDecimal.h"

enum Context {
    kRaceContext_QuickRace = 0,
    kRaceContext_Online = 1,
    kRaceContext_Career = 2,
    kRaceContext_Count = 3,
};

class GVault;
class GRaceCustom;

typedef Attrib::Gen::gameplay GameplayObj;

typedef union { // 0x4
    /* 0x0 */ float mBestTime;
    /* 0x0 */ float mBestCash;
    /* 0x0 */ uint32 mBestPoints;
    /* 0x0 */ float mBestSpeed;
} GHighScoresUnion;

// total size: 0x30
struct GRaceIndexData {
    Attrib::Key mKey;         // offset 0x0, size 0x4
    char mEventID[10];        // offset 0x4, size 0xA
    decimal16 mChallengeGoal; // offset 0xE, size 0x2
    uint32 mChallengeType;    // offset 0x10, size 0x4
    uint32 mRaceHash;         // offset 0x14, size 0x4
    uint32 mFlags;            // offset 0x18, size 0x4
    float mLength;            // offset 0x1C, size 0x4
    short mLocalizationTag;   // offset 0x20, size 0x2
    decimal16 mCash;          // offset 0x22, size 0x2
    decimal16 mReputation;    // offset 0x24, size 0x2
    udecfix16 mRivalBest;     // offset 0x26, size 0x2
    uint8 mNumLaps;           // offset 0x28, size 0x1
    uint8 mRegion;            // offset 0x29, size 0x1
    int8 mCopDensity;         // offset 0x2A, size 0x1
    int8 mRaceType;           // offset 0x2B, size 0x1
    uint8 mMapX1;             // offset 0x2C, size 0x1
    uint8 mMapY1;             // offset 0x2D, size 0x1
    uint8 mMapX2;             // offset 0x2E, size 0x1
    uint8 mMapY2;             // offset 0x2F, size 0x1
};

// total size: 0x10
struct GRaceSaveInfo {
    uint32 mRaceHash;             // offset 0x0, size 0x4
    uint32 mFlags;                // offset 0x4, size 0x4
    GHighScoresUnion mHighScores; // offset 0x8, size 0x4
    udecfix16 mTopSpeed;          // offset 0xC, size 0x2
    udecfix16 mAverageSpeed;      // offset 0xE, size 0x2

    GRaceSaveInfo() : mTopSpeed(0), mAverageSpeed(0) {}
};

// class GRaceParameters { // 0x14
//   public:
//     GRaceParameters();
//     Attrib::Key GetCollectionKey();
//     GameplayObj *GetGameplayObj();
//     GActivity *GetActivity();
//     GVault *GetChildVault();
//     GVault *GetParentVault();
//     char *GetEventID();
//     unsigned int GetEventHash();
//     bool GetIsAvailable();
//     bool GetIsLoaded();
//     void BlockUntilLoaded();
//     int GetLocalizationTag();
//     unsigned int GetChallengeType();
//     float GetChallengeGoal();
//     GRace::Type GetRaceType();
//     Region GetRegion();
//     int GetNumLaps();
//     int GetReputation();
//     float GetCashValue();
//     float GetRaceLengthMeters();
//     bool GetCopsEnabled();
//     CopDensity GetCopDensity();
//     bool GetScriptedCopsInRace();
//     float GetRivalBestTime();
//     bool GetCanBeReversed();
//     bool GetIsDDayRace();
//     bool GetIsBossRace();
//     bool GetIsMarkerRace();
//     bool GetIsPursuitRace();
//     bool GetIsLoopingRace();
//     bool GetRankPlayersByPoints();
//     bool GetRankPlayersByDistance();
//     bool GetNeverInQuickRace();
//     bool GetIsChallengeSeriesRace();
//     void GetBoundingBox();
//     float GetTimeLimit();
//     int GetBustedLives();
//     int GetKnockoutsPerLap();
//     float GetTimedKnockout();
//     int GetTrafficDensity();
//     GRace::Difficulty GetDifficulty();
//     char *GetTrafficPattern();
//     float GetMaxHeatLevel();
//     float GetInitialPlayerSpeed();
//     bool GetIsRollingStart();
//     char *GetPlayerCarType();
//     float GetPlayerCarPerformance();
//     bool GetUseWorldHeatInRace();
//     float GetForceHeatLevel();
//     float GetMaxRaceHeatLevel();
//     bool GetCatchUp();
//     bool GetCatchUpOverride();
//     char *GetCatchUpSkill();
//     char *GetCatchUpSpread();
//     float GetCatchUpIntegral();
//     float GetCatchUpDerivative();
//     bool GetPhotofinish();
//     int GetNumOpponents();
//     GCharacter *GetOpponentChar();
//     unsigned int GetNumCheckpoints();
//     /* vtable[1] */ virtual void GetCheckpointPosition();
//     /* vtable[2] */ virtual void GetCheckpointDirection();
//     bool GetCheckpointsVisible();
//     unsigned int GetNumShortcuts();
//     GMarker *GetShortcut();
//     unsigned int GetNumBarrierExemptions();
//     GMarker *GetBarrierExemption();
//     bool HasFinishLine();
//     void GetStartPosition();
//     void GetStartDirection();
//     Angle GetStartAngle();
//     void GetFinishPosition();
//     void GetFinishDirection();
//     unsigned int GetBarrierCount();
//     char *GetBarrierName();
//     unsigned int GetBarrierHash();
//     bool GetBarrierIsFlipped();
//     bool GetNoPostRaceScreen();
//     char *GetPhotoFinishCamera();
//     char *GetPhotoFinishTexture();
//     float GetTimeOfDay();
//     bool GetIsSunsetRace();
//     float GetStartTime();
//     float GetStartPercent();
//     char *GetSpeedTrapCamera();
//     bool SetupTimeOfDay();

//   protected:
//     // GRaceParameters();
//     /* vtable[3] */ virtual ~GRaceParameters();
//     bool GetInitiallyUnlockedQuickRace();
//     bool GetInitiallyUnlockedOnline();
//     bool GetInitiallyUnlockedChallenge();
//     void GenerateIndex();
//     void EnsureLoaded();
//     void NotifyParentVaultUnloading();
//     void NotifyParentVaultLoaded();
//     void ExtractPosition();
//     void ExtractDirection();

//     /* 0x00 */ GRaceIndexData *mIndex;
//     /* 0x04 */ Attrib::Gen::gameplay *mRaceRecord;
//     /* 0x08 */ GVault *mParentVault;
//     /* 0x0c */ GVault *mChildVault;
// };

// total size: 0x14
class GRaceParameters {
  public:
    unsigned int GetCollectionKey() const;

    float GetRaceLengthMeters() const;

    int GetReputation() const;

    float GetCashValue() const;

    int GetLocalizationTag() const;

    int GetNumLaps() const;

    const char *GetEventID() const;

    float GetRivalBestTime() const;

    float GetChallengeGoal() const;

    bool GetInitiallyUnlockedQuickRace() const;

    bool GetInitiallyUnlockedOnline() const;

    bool GetInitiallyUnlockedChallenge() const;

    bool GetIsDDayRace() const;

    bool GetIsBossRace() const;

    bool GetIsMarkerRace() const;

    bool GetIsPursuitRace() const;

    bool GetIsLoopingRace() const;

    bool GetRankPlayersByPoints() const;

    bool GetRankPlayersByDistance() const;

    bool GetCopsEnabled() const;

    bool GetScriptedCopsInRace() const;

    bool GetNeverInQuickRace() const;

    bool GetIsChallengeSeriesRace() const;

    bool GetIsCollectorsEditionRace() const;

    float GetTimeLimit() const;

    float GetMaxHeatLevel() const;

    bool GetNoPostRaceScreen() const;

    bool GetUseWorldHeatInRace() const;

    float GetForceHeatLevel() const;

    float GetMaxRaceHeatLevel() const;

    float GetInitialPlayerSpeed() const;

    bool GetIsRollingStart() const;

    bool GetIsEpicPursuitRace() const;

    const char *GetPlayerCarType() const;

    float GetPlayerCarPerformance() const;

    int GetBustedLives() const;

    int GetKnockoutsPerLap() const;

    float GetTimedKnockout() const;

    bool GetCatchUp() const;

    bool GetCatchUpOverride() const;

    const char *GetCatchUpSkill() const;

    const char *GetCatchUpSpread() const;

    float GetCatchUpIntegral() const;

    float GetCatchUpDerivative() const;

    bool GetPhotofinish() const;

    unsigned int GetNumCheckpoints() const;

    bool GetCheckpointsVisible() const;

    unsigned int GetNumShortcuts() const;

    unsigned int GetNumBarrierExemptions() const;

    unsigned int GetBarrierCount() const;

    const char *GetTrafficPattern() const;

    const char *GetPhotoFinishCamera() const;

    const char *GetPhotoFinishTexture() const;

    float GetTimeOfDay() const;

    float GetStartTime() const;

    float GetStartPercent() const;

    const char *GetSpeedTrapCamera() const;

    inline void EnsureLoaded() const {}

    void BlockUntilLoaded();

    bool GetIsLoaded() const;

    GRaceParameters(unsigned int collectionKey, GRaceIndexData *index);

    virtual ~GRaceParameters();

    void GenerateIndex(GRaceIndexData *index);

    void NotifyParentVaultUnloading();

    void NotifyParentVaultLoaded();

    const Attrib::Gen::gameplay *GetGameplayObj() const;

    GActivity *GetActivity() const;

    GVault *GetChildVault() const;

    GVault *GetParentVault() const;

    void GetBoundingBox(UMath::Vector2 &topLeft, UMath::Vector2 &botRight) const;

    unsigned int GetChallengeType() const;

    GRace::Type GetRaceType() const;

    // enum Region GetRegion() const;
    GRace::Region GetRegion() const;

    void ExtractPosition(Attrib::Gen::gameplay &collection, UMath::Vector3 &pos) const;

    void ExtractDirection(Attrib::Gen::gameplay &collection, UMath::Vector3 &dir, float rotate) const;

    unsigned int GetEventHash() const;

    bool GetIsAvailable(GRace::Context context) const;

    bool GetIsSunsetRace() const;

    bool GetIsMiddayRace() const;

    void SetupTimeOfDay();

    int GetTrafficDensity() const;

    GRace::Difficulty GetDifficulty() const;

    GRace::CopDensity GetCopDensity() const;

    bool GetCanBeReversed() const;

    GCharacter *GetOpponentChar(unsigned int index) const;

    int GetNumOpponents() const;

    void GetStartPosition(UMath::Vector3 &pos) const;

    void GetStartDirection(UMath::Vector3 &dir) const;

    float GetStartAngle() const;

    bool HasFinishLine() const;

    void GetFinishPosition(UMath::Vector3 &pos) const;

    void GetFinishDirection(UMath::Vector3 &dir) const;

    virtual void GetCheckpointPosition(unsigned int index, UMath::Vector3 &pos) const;

    virtual void GetCheckpointDirection(unsigned int index, UMath::Vector3 &dir) const;

    GMarker *GetShortcut(unsigned int index) const;

    GMarker *GetBarrierExemption(unsigned int index) const;

    const char *GetBarrierName(unsigned int index) const;

    unsigned int GetBarrierHash(unsigned int index) const;

    bool GetBarrierIsFlipped(unsigned int index) const;

  protected:
    GRaceIndexData *mIndex;             // offset 0x0, size 0x4
    Attrib::Gen::gameplay *mRaceRecord; // offset 0x4, size 0x4
    GVault *mParentVault;               // offset 0x8, size 0x4
    GVault *mChildVault;                // offset 0xC, size 0x4
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

    GRaceBin *GetBin(unsigned int index);
    int GetBinNumber() const;
    int GetBinNumber(int index);

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

class GActivity;

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
    void ClearStartupRace();
    void SetStartupRace(GRaceCustom *custom, Context context);
    void FreeCustomRace(GRaceCustom *custom);
    GRaceParameters *GetRaceFromHash(unsigned int hash);
    GRaceParameters *GetRaceFromActivity(GActivity *activity);
    GRaceCustom *AllocCustomRace(GRaceParameters *parms);

    unsigned int GetRaceCount() const;
    GRaceParameters *GetRaceParameters(unsigned int index) const;

    static GRaceDatabase &Get() {
        return *mObj;
    }

    static bool Exists() {
        return mObj != nullptr;
    }

    GRaceParameters *GetRaceFromName(const char *name) {
        return GetRaceFromHash(Attrib::StringHash32(name));
    }

    bool IsCareerRaceComplete(unsigned int eventHash) {
        return CheckRaceScoreFlags(eventHash, kCompleted_ContextCareer);
    }

    bool IsQuickRaceComplete(unsigned int eventHash) {
        return CheckRaceScoreFlags(eventHash, kCompleted_ContextQuickRace);
    }

    const char *GetBurgerKingRace() const {
        return "19.8.31";
    }

    const char *GetDDayStartRace() const {
        return sDDayRaces[0];
    }

    const char *GetDDayEndRace() const {
        return sDDayRaces[7];
    }

    const char *GetFinalBossRace() const {
        return sDDayRaces[4];
    }

    bool CheckRaceScoreFlags(unsigned int eventHash, ScoreFlags mask);
    const char *GetNextDDayRace();
    GRaceSaveInfo *GetScoreInfo(unsigned int eventHash);
    GRaceSaveInfo *GetScoreInfo() {
        return mRaceScoreInfo;
    };
    unsigned int GetScoreInfoCount() {
        return mRaceCountStatic;
    };
    void LoadBestScores(GRaceSaveInfo *entries, unsigned int count);
    void SimulateDDayComplete();
    void ClearRaceScores();

    unsigned int GetBinCount();
    GRaceBin *GetBin(unsigned int index);
    GRaceBin *GetBinNumber(int number);

    static const char sDDayRaces[8][5];

  private:
    unsigned int mRaceCountStatic;    // offset 0x0, size 0x4
    unsigned int mRaceCountDynamic;   // offset 0x4, size 0x4
    GRaceIndexData *mRaceIndex;       // offset 0x8, size 0x4
    GRaceParameters *mRaceParameters; // offset 0xC, size 0x4
    GRaceCustom *mRaceCustom[4];      // offset 0x10, size 0x10
    unsigned int mBinCount;           // offset 0x20, size 0x4
    GRaceBin *mBins;                  // offset 0x24, size 0x4
    Attrib::Class *mGameplayClass;    // offset 0x28, size 0x4
    GRaceCustom *mStartupRace;        // offset 0x2C, size 0x4
    Context mStartupRaceContext;      // offset 0x30, size 0x4
    unsigned int mNumInitialUnlocks;  // offset 0x34, size 0x4
    unsigned int *mInitialUnlockHash; // offset 0x38, size 0x4
    GRaceSaveInfo *mRaceScoreInfo;    // offset 0x3C, size 0x4

  public:
    static GRaceDatabase *mObj;
};

bool GetIsCollectorsEdition();

#endif
