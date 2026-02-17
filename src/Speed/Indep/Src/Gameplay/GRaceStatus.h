#ifndef GAMEPLAY_GRACE_STATUS_H
#define GAMEPLAY_GRACE_STATUS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GCharacter.h"
#include "GRace.h"
#include "GRaceDatabase.h"
#include "GTimer.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/Simables/isimable.h"
#include "Speed/Indep/Src/Misc/Table.hpp"

#include <types.h>

// total size: 0x1A8
struct GRacerInfo {
  public:
    GCharacter *GetGameCharacter() const {
        return mGameCharacter;
    }

    float GetPctRaceComplete() const {
        return mPctRaceComplete;
    }

  private:
    HSIMABLE mhSimable;              // offset 0x0, size 0x4
    GCharacter *mGameCharacter;      // offset 0x4, size 0x4
    const char *mName;               // offset 0x8, size 0x4
    int mIndex;                      // offset 0xC, size 0x4
    int mRanking;                    // offset 0x10, size 0x4
    int mAiRanking;                  // offset 0x14, size 0x4
    float mPctRaceComplete;          // offset 0x18, size 0x4
    bool mKnockedOut;                // offset 0x1C, size 0x1
    bool mTotalled;                  // offset 0x20, size 0x1
    bool mEngineBlown;               // offset 0x24, size 0x1
    bool mBusted;                    // offset 0x28, size 0x1
    bool mChallengeComplete;         // offset 0x2C, size 0x1
    bool mFinishedRacing;            // offset 0x30, size 0x1
    bool mCameraDetached;            // offset 0x34, size 0x1
    float mPctLapComplete;           // offset 0x38, size 0x4
    int mLapsCompleted;              // offset 0x3C, size 0x4
    int mCheckpointsHitThisLap;      // offset 0x40, size 0x4
    int mTollboothsCrossed;          // offset 0x44, size 0x4
    float mTimeRemainingToBooth[16]; // offset 0x48, size 0x40
    int mSpeedTrapsCrossed;          // offset 0x88, size 0x4
    float mSpeedTrapSpeed[16];       // offset 0x8C, size 0x40
    int mSpeedTrapPosition[16];      // offset 0xCC, size 0x40
    float mDistToNextCheckpoint;     // offset 0x10C, size 0x4
    float mDistanceDriven;           // offset 0x110, size 0x4
    float mTopSpeed;                 // offset 0x114, size 0x4
    float mFinishingSpeed;           // offset 0x118, size 0x4
    float mPoundsNOSUsed;            // offset 0x11C, size 0x4
    float mTimeCrossedLastCheck;     // offset 0x120, size 0x4
    float mTotalUpdateTime;          // offset 0x124, size 0x4
    int mNumPerfectShifts;           // offset 0x128, size 0x4
    int mNumTrafficCarsHit;          // offset 0x12C, size 0x4
    float mSpeedBreakerTime;         // offset 0x130, size 0x4
    float mPointTotal;               // offset 0x134, size 0x4
    float mZeroToSixtyTime;          // offset 0x138, size 0x4
    float mQuarterMileTime;          // offset 0x13C, size 0x4
#ifndef EA_BUILD_A124
    float mSplitTimes[4];  // offset 0x140, size 0x10
    int mSplitRankings[4]; // offset 0x150, size 0x10
#endif
    GTimer mRaceTimer;                       // offset 0x160, size 0xC
    GTimer mLapTimer;                        // offset 0x16C, size 0xC
    GTimer mCheckTimer;                      // offset 0x178, size 0xC
    ALIGN_16 UMath::Vector3 mSavedPosition;  // offset 0x184, size 0xC
    float mSavedHeatLevel;                   // offset 0x190, size 0x4
    ALIGN_16 UMath::Vector3 mSavedDirection; // offset 0x194, size 0xC
    float mSavedSpeed;                       // offset 0x1A0, size 0x4
#ifndef EA_BUILD_A124
    bool mDNF; // offset 0x1A4, size 0x1
#endif
};

DECLARE_VECTOR_TYPE(ID_GRaceStatusTriggerList);

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

    GRaceParameters(unsigned int collectionKey, struct GRaceIndexData *index);

    virtual ~GRaceParameters();

    void GenerateIndex(struct GRaceIndexData *index);

    void NotifyParentVaultUnloading();

    void NotifyParentVaultLoaded();

    const Attrib::Gen::gameplay *GetGameplayObj() const;

    struct GActivity *GetActivity() const;

    struct GVault *GetChildVault() const;

    struct GVault *GetParentVault() const;

    void GetBoundingBox(struct Vector2 &topLeft, struct Vector2 &botRight) const;

    unsigned int GetChallengeType() const;

    GRace::Type GetRaceType() const;

    // enum Region GetRegion() const;

    void ExtractPosition(Attrib::Gen::gameplay &collection, UMath::Vector3 &pos) const;

    void ExtractDirection(Attrib::Gen::gameplay &collection, UMath::Vector3 &dir, float rotate) const;

    unsigned int GetEventHash() const;

    // bool GetIsAvailable(enum Context context) const;

    bool GetIsSunsetRace() const;

    bool GetIsMiddayRace() const;

    void SetupTimeOfDay();

    int GetTrafficDensity() const;

    // enum Difficulty GetDifficulty() const;

    // enum CopDensity GetCopDensity() const;

    bool GetCanBeReversed() const;

    struct GCharacter *GetOpponentChar(unsigned int index) const;

    int GetNumOpponents() const;

    void GetStartPosition(UMath::Vector3 &pos) const;

    void GetStartDirection(UMath::Vector3 &dir) const;

    float GetStartAngle() const;

    bool HasFinishLine() const;

    void GetFinishPosition(UMath::Vector3 &pos) const;

    void GetFinishDirection(UMath::Vector3 &dir) const;

    virtual void GetCheckpointPosition(unsigned int index, UMath::Vector3 &pos) const;

    virtual void GetCheckpointDirection(unsigned int index, UMath::Vector3 &dir) const;

    struct GMarker *GetShortcut(unsigned int index) const;

    struct GMarker *GetBarrierExemption(unsigned int index) const;

    const char *GetBarrierName(unsigned int index) const;

    unsigned int GetBarrierHash(unsigned int index) const;

    bool GetBarrierIsFlipped(unsigned int index) const;

  protected:
    struct GRaceIndexData *mIndex;      // offset 0x0, size 0x4
    Attrib::Gen::gameplay *mRaceRecord; // offset 0x4, size 0x4
    struct GVault *mParentVault;        // offset 0x8, size 0x4
    struct GVault *mChildVault;         // offset 0xC, size 0x4
};

// total size: 0x46AC
class GRaceStatus : public UTL::COM::Object, public IVehicleCache {
  public:
    enum PlayMode {
        kPlayMode_Roaming = 0,
        kPlayMode_Racing = 1,
    };

    typedef UTL::Std::vector<struct GTrigger *, _type_ID_GRaceStatusTriggerList> TriggerList;

    static void Init();
    static void Shutdown();

    void EnableBinBarriers();

    void DisableBinBarriers();

    void RefreshBinWhileInGame();

    void EnterBin(unsigned int binNumber);

    void CalculateRankings();

    void SortCheckPointRankings();

    void Update(float dT);

    bool CanUnspawnRoamer(const IVehicle *roamer) const;

    // Overrides: IVehicleCache
    enum eVehicleCacheResult OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const override;

    // Overrides: IVehicleCache
    void OnRemovedVehicleCache(IVehicle *ivehicle) override;

    void SetRaceContext(GRace::Context context);

    GRacerInfo &GetRacerInfo(int index);

    GRacerInfo *GetRacerInfo(ISimable *isim);

    GRacerInfo *GetWinningPlayerInfo();

    int GetRacerCount() const;

    void StartMasterTimer();

    void StopMasterTimer();

    float GetRaceTimeElapsed() const;

    float GetRaceTimeRemaining() const;

    void SkipToEndOfRaceForRacer(ISimable *thisPlayer, int index, float time);

    void SkipToEndOfRaceInPlace(int place, float finishTime);

    void ClearRacers();

    GRacerInfo &AddSimablePlayer(ISimable *isim);

    void AddRacer(GRuntimeInstance *racer);

    void SetRaceActivity(GActivity *activity);

    void EnableBarriers();

    void DisableBarriers();

    void SetRoaming();

    void SetRacing();

    void NotifyScriptWhenLoaded();

    void AddAvailableEventToMap(GRuntimeInstance *trigger, GRuntimeInstance *activity);

    void AddSpeedTrapToMap(GRuntimeInstance *trigger);

    void AwardBonusTime(float seconds);

    void ClearCheckpoints();

    void AddCheckpoint(GRuntimeInstance *trigger);

    static void NotifyEmitterGroupDelete(void *obj, EmitterGroup *group);

    void SetNextCheckpointPos(GRuntimeInstance *trigger);

    float DetermineRaceSegmentLength(const UMath::Vector4 *positions, const UMath::Vector4 *directions, int start, int end);

    void DetermineRaceLength();

    void ParseCatchUpData(const char *skill, const char *spread);

    float GetAdaptiveDifficutly() const;

    void SyncronizeAdaptiveBonus();

    // void UpdateAdaptiveDifficulty(enum eAdaptiveGainReason reason, struct ISimable *who);

    bool ComputeCatchUpSkill(GRacerInfo *racer_info, PidError *pid, float *output, float *skill, bool off_world);

    void MakeDefaultCatchUpData();

    void MakeCatchUpData();

    int GetCheckpointCount();

    GTrigger *GetCheckpoint(int index);

    GTrigger *GetNextCheckpoint();

    void ClearTimes();

    void SetLapTime(int lapIndex, int racerIndex, float time);

    float GetLapTime(int lapIndex, int racerIndex, bool bCumulativeTimeAtLap);

    void SetCheckpointTime(int lapIndex, int checkIndex, int racerIndex, float time);

    float GetCheckpointTime(int lapIndex, int checkIndex, int racerIndex, bool bCumulative);

    int GetLapPosition(int lapIndex, int racerIndex, bool bOverallPosition);

    int GetCheckpointPosition(int lapIndex, int checkIndex, int racerIndex);

    float GetFinishTimeBehind(int racerIndex);

    float GetBestLapTime(int racerIndex);

    float GetWorstLapTime(int racerIndex);

    int GetLapsLed(int racerIndex);

    float GetRaceSpeedTrapSpeed(int trapIndex, int racerIndex);

    int GetRaceSpeedTrapPosition(int trapIndex, int racerIndex);

    float GetBestSpeedTrapSpeed(int racerIndex);

    float GetWorstSpeedTrapSpeed(int racerIndex);

    float GetRaceTollboothTime(int boothIndex, int racerIndex);

    void RaceAbandoned();

    void EndStopAll();

    void FinalizeRaceStats();

    bool IsAudioLoading();

    bool IsModelsLoading();

    bool IsLoading();

    float GetSegmentLength(int segment, int lap);

    GRaceStatus();

    GRaceParameters *GetRaceParameters() const {
        return mRaceParms;
    }

    static GRaceStatus &Get() {
        return *fObj;
    }

    static bool Exists() {
        return fObj != nullptr;
    }

    GRace::Type GetRaceType() const {
        return mRaceParms ? mRaceParms->GetRaceType() : GRace::kRaceType_None;
    }

    static bool IsChallengeRace() {
        return Exists() && Get().GetRaceType() == GRace::kRaceType_Challenge;
    }

    PlayMode GetPlayMode() {
        return mPlayMode;
    }

    unsigned int GetTrafficPattern() const {
        return mTrafficPattern;
    }

    bool GetActivelyRacing() const {
        return mActivelyRacing;
    }

    GRace::Context GetRaceContext() const {
        return mRaceContext;
    }

    static bool IsFinalEpicPursuit() {
        return GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() && GRaceStatus::Get().GetRaceParameters()->GetIsEpicPursuitRace();
    }

    float GetBinBaseHeat() const {
        return mRaceBin->GetBaseOpenWorldHeat();
    }

    float GRaceStatusGetBinMaxHeat() const {
        return mRaceBin->GetMaxOpenWorldHeat();
    }

    float GetBinHeatScale() const {
        return mRaceBin->GetScaleOpenWorldHeat();
    }

  private:
    static struct GRaceStatus *fObj;

    GRacerInfo mRacerInfo[16];   // offset 0x1C, size 0x1A80
    int mRacerCount;             // offset 0x1A9C, size 0x4
    bool mIsLoading;             // offset 0x1AA0, size 0x1
    PlayMode mPlayMode;          // offset 0x1AA4, size 0x4
    GRace::Context mRaceContext; // offset 0x1AA8, size 0x4
    GRaceParameters *mRaceParms; // offset 0x1AAC, size 0x4
    GRaceBin *mRaceBin;          // offset 0x1AB0, size 0x4
    GTimer mRaceMasterTimer;     // offset 0x1AB4, size 0xC
#ifndef EA_BUILD_A124
    bool mPlayerPursuitInCooldown; // offset 0x1AC0, size 0x1
#endif
    float mBonusTime;                    // offset 0x1AC4, size 0x4
    float mTaskTime;                     // offset 0x1AC8, size 0x4
    bool mSuddenDeathMode;               // offset 0x1ACC, size 0x1
    bool mTimeExpiredMsgSent;            // offset 0x1AD0, size 0x1
    bool mActivelyRacing;                // offset 0x1AD4, size 0x1
    int mLastSecondTickSent;             // offset 0x1AD8, size 0x4
    struct WorldModel *mCheckpointModel; // offset 0x1ADC, size 0x4
    EmitterGroup *mCheckpointEmitter;    // offset 0x1AE0, size 0x4
    bool mQueueBinChange;                // offset 0x1AE4, size 0x1
#ifndef EA_BUILD_A124
    unsigned int mWarpWhenInFreeRoam; // offset 0x1AE8, size 0x4
#endif
    int mNumTollbooths;         // offset 0x1AEC, size 0x4
    bool mScriptWaitingForLoad; // offset 0x1AF0, size 0x1
#ifndef EA_BUILD_A124
    bool mRefreshBinAfterRace; // offset 0x1AF4, size 0x1
#endif
    TriggerList mCheckpoints;         // offset 0x1AF8, size 0x10
    struct GTrigger *mNextCheckpoint; // offset 0x1B08, size 0x4
    float mLapTimes[10][16];          // offset 0x1B0C, size 0x280
    float mCheckTimes[10][16][16];    // offset 0x1D8C, size 0x2800
    float mSegmentLengths[18];        // offset 0x458C, size 0x48
    float fRaceLength;                // offset 0x45D4, size 0x4
    float fFirstLapLength;            // offset 0x45D8, size 0x4
    float fSubsequentLapLength;       // offset 0x45DC, size 0x4
#ifndef EA_BUILD_A124
    bool mCaluclatedAdaptiveGain; // offset 0x45E0, size 0x1
#endif
    float fCatchUpIntegral;           // offset 0x45E4, size 0x4
    float fCatchUpDerivative;         // offset 0x45E8, size 0x4
    float fCatchUpAdaptiveBonus;      // offset 0x45EC, size 0x4
    float fAveragePercentComplete;    // offset 0x45F0, size 0x4
    int nCatchUpSkillEntries;         // offset 0x45F4, size 0x4
    float aCatchUpSkillData[11];      // offset 0x45F8, size 0x2C
    int nCatchUpSpreadEntries;        // offset 0x4624, size 0x4
    float aCatchUpSpreadData[11];     // offset 0x4628, size 0x2C
    int nSpeedTraps;                  // offset 0x4654, size 0x4
    struct GTrigger *aSpeedTraps[16]; // offset 0x4658, size 0x40
    bool mVehicleCacheLocked;         // offset 0x4698, size 0x1
    bool bRaceRouteError;             // offset 0x469C, size 0x1
    int mTrafficDensity;              // offset 0x46A0, size 0x4
    unsigned int mTrafficPattern;     // offset 0x46A4, size 0x4
    bool mHasBeenWon;                 // offset 0x46A8, size 0x1
};

// total size: 0x28
class GRaceCustom : public GRaceParameters {
  public:
    // void *operator new(unsigned int size, void *ptr) {}

    // void operator delete(void *mem, void *ptr) {}

    // void *operator new(unsigned int size) {}

    // void operator delete(void *mem, unsigned int size) {}

    // void *operator new(unsigned int size, const char *name) {}

    // void operator delete(void *mem, const char *name) {}

    // void operator delete(void *mem, unsigned int size, const char *name) {}

    GRaceCustom(const struct GRaceParameters &other);

    // Overrides: GRaceParameters
    ~GRaceCustom() override;

    void CreateRaceActivity();

    GActivity *GetRaceActivity() const;

    // Overrides: GRaceParameters
    void GetCheckpointPosition(unsigned int index, UMath::Vector3 &pos) const override;

    // Overrides: GRaceParameters
    void GetCheckpointDirection(unsigned int index, UMath::Vector3 &dir) const override;

    void SetReversed(bool isReverseDir);

    void SetPositionalKnockout(bool enabled, int knockoutsPerLap);

    void SetTimedKnockout(bool enabled, float secondsBehindLeader);

    void SetNumLaps(int numLaps);

    void SetTrafficDensity(int density);

    void SetNumOpponents(int numOpponents);

    void SetDifficulty(GRace::Difficulty difficulty);

    void SetCatchUp(bool catchUpEnabled);

    void SetCopsEnabled(bool copsEnabled);

    void SetBustedLives(int bustedLives);

    void SetForceHeatLevel(int level);

    void SetAttribute(unsigned int key, const int &value, unsigned int index);

    void SetAttribute(unsigned int key, const float &value, unsigned int index);

    void SetAttribute(unsigned int key, const bool &value, unsigned int index);

    void SetHeatLevel(int level) {
        mHeatLevel = level;
    }

    void SetFreedByOwner() {
        mFreedByOwner = true;
    }

    bool GetFreedByOwner() {
        return mFreedByOwner;
    }

  private:
    GActivity *mRaceActivity;   // offset 0x14, size 0x4
    unsigned int mNumOpponents; // offset 0x18, size 0x4
    bool mReversed;             // offset 0x1C, size 0x1
    bool mFreedByOwner;         // offset 0x20, size 0x1
    int mHeatLevel;             // offset 0x24, size 0x4
};

#endif
