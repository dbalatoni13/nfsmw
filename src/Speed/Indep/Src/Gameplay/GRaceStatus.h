#ifndef GAMEPLAY_GRACE_STATUS_H
#define GAMEPLAY_GRACE_STATUS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GCharacter.h"
#include "GRace.h"
#include "GRaceDatabase.h"
#include "GActivity.h"
#include "GTimer.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/World/WorldModel.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

#include <types.h>

// total size: 0x1A8
class GRacerInfo {
  public:
    GRacerInfo();
    ~GRacerInfo();
    ISimable *GetSimable() const {
        return ISimable::FindInstance(mhSimable);
    }
    GCharacter *GetGameCharacter() const {
        return mGameCharacter;
    }
    bool GetIsHuman() const {
        return !mGameCharacter;
    };
    const char *GetName() const {
        return mName;
    }
    int GetIndex() const {
        return mIndex;
    };
    int GetRanking() const {
        return mRanking;
    }
    int GetAiRanking() const {
        return mAiRanking;
    };
    float GetPctRaceComplete() const {
        return mPctRaceComplete;
    }
    float GetHudPctRaceComplete() const;
#ifndef EA_BUILD_A124
    void UpdateSplits();
    int GetSpeedTrapsCrossed() const {
        return mSpeedTrapsCrossed;
    }
#endif
    bool GetIsKnockedOut() const {
        return mKnockedOut;
    }
    bool GetIsTotalled() const {
        return mTotalled;
    }
    bool GetIsEngineBlown() const {
        return mEngineBlown;
    }
    bool GetIsBusted() const {
        return mBusted;
    }
    bool GetChallengeComplete() const {
        return mChallengeComplete;
    };
    bool GetCameraDetached() const {
        return mCameraDetached;
    };
    float GetPctLapComplete() const {
        return mPctLapComplete;
    };
    int GetLapsCompleted() const {
        return mLapsCompleted;
    };
    int GetChecksHitThisLap() const {
        return mCheckpointsHitThisLap;
    };
    int GetTollboothsCrossed() const {
        return mTollboothsCrossed;
    };
    float GetDistToNextCheck() const {
        return mDistToNextCheckpoint;
    };
    float GetDistDriven() const {
        return mDistanceDriven;
    };
    float GetTopSpeed() const {
        return mTopSpeed;
    };
    float GetFinishingSpeed() const {
        return mFinishingSpeed;
    }
    float GetPoundsNOSUsed() const {
        return mPoundsNOSUsed;
    };
    float GetRaceTime() const {
        return mRaceTimer.GetTime();
    };
    float GetLapTime() const {
        return mLapTimer.GetTime();
    };
    float GetCheckTime() const {
        return mCheckTimer.GetTime();
    };
    int GetPerfectShifts() const {
        return mNumPerfectShifts;
    };
    int GetTrafficCarsHit() const {
        return mNumTrafficCarsHit;
    };
    float GetSpeedBreakerTime() const {
        return mSpeedBreakerTime;
    };
    float GetPointTotal() const {
        return mPointTotal;
    }
    float GetZeroToSixtyTime() const {
        return mZeroToSixtyTime;
    };
    float GetQuarterMileTime() const {
        return mQuarterMileTime;
    };
#ifndef EA_BUILD_A124
    void SetRaceTime(float f) {}
    void SetLapsCompleted(int n) {
        mLapsCompleted = n;
    }
    void SetPctRaceComplete(float f) {
        mPctRaceComplete = f;
    }
    void SetDistDriven(float f) {
        mDistanceDriven = f;
    }
    void SetTopSpeed(float f) {
        mTopSpeed = f;
    }
    void SetPoundsNOSUsed(float f) {
        mPoundsNOSUsed = f;
    }
    void SetZeroToSixtyTime(float f) {
        mZeroToSixtyTime = f;
    }
    void SetQuarterMileTime(float f) {
        mQuarterMileTime = f;
    }
    void SetPerfectShifts(int i) {
        mNumPerfectShifts = i;
    }
    float GetSplitTime(int split) const {
        return mSplitTimes[split];
    }
    int GetSplitRanking(int split) const {
        return mSplitRankings[split];
    }
    void SetSplitTime(int split, float time) {
        mSplitTimes[split] = time;
    }
    void SetSplitRanking(int split, int rank) {
        mSplitRankings[split] = rank;
    }
    bool GetDNF() const {
        return mDNF;
    }
#endif
    bool IsLoading() const;
    void GetPosition(UMath::Vector3 &pos) const;
    void GetLinearVelocity(UMath::Vector3 &vel) const;
    float CalcAverageSpeed() const;
    void SetSimable(ISimable *simable);
    void SetName(char *name);
    void SetIndex(int index);
    void SetRanking(int ranking);
    void AddToPointTotal(float points);
    void KnockOut();
    void BlowEngine();
    void TotalVehicle();
    void Busted();
    void ChallengeComplete();
    void ForceStop();
    void EndForceStop();
    void Update(float dT);
    void SaveStartPosition();
    void RestoreStartPosition();
    void ForceStartPosition(const UMath::Vector3 &pos, const UMath::Vector3 &dir);
    void StartRace();
    void StartLap(int lapIndexOneBased);
    void StartCheckpoint(int checkIndex);
    void FinishRace();
    bool IsFinishedRacing() const {
        return mFinishedRacing;
    }
#ifndef EA_BUILD_A124
    bool AreStatsReady() const;
#endif
    void DetachCamera() {
        mCameraDetached = true;
    };
    void IncPerfectShifts() {
        mNumPerfectShifts++;
    };
    void NotifyTrafficCollision() {
        mNumTrafficCarsHit++;
    };
    void NotifyTollboothCrossed(float RaceClock) {
        mTollboothsCrossed++;
    };
    void NotifySpeedTrapTriggered(float mps) {
        mSpeedTrapsCrossed++;
    };
    bool IsAheadOf(const struct GRacerInfo &rhs) {};
    bool IsBehind(const struct GRacerInfo &rhs);
    IVehicle *CreateVehicle(unsigned int default_key);
    void FinalizeRaceStats();
    void ClearRaceStats() {};

  private:
    // GRacerInfo();
    void Kill();
    void ClearAll() {};
    void ChooseRandomName();
    bool ChooseBossName();
    bool ChooseRacerName();

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

DECLARE_CONTAINER_TYPE(ID_GRaceStatusTriggerList);

// total size: 0x46AC
class GRaceStatus : public UTL::COM::Object, public IVehicleCache {
  public:
    enum PlayMode {
        kPlayMode_Roaming = 0,
        kPlayMode_Racing = 1,
    };

    enum eAdaptiveGainReason {
        kAdaptiveGain_FromEngineBlown = 0,
        kAdaptiveGain_FromVehicleDestroyed = 1,
        kAdaptiveGain_FromFinalizeRace = 2,
        kAdaptiveGain_FromRacerKOed = 3,
        kAdaptiveGain_FromRacerFinished = 4,
        kAdaptiveGain_FromRestart = 5,
    };

    typedef UTL::Std::vector<struct GTrigger *, _type_ID_GRaceStatusTriggerList> TriggerList;

    static void Init();

    static void Shutdown();

    static GRaceStatus &Get() {
        return *fObj;
    }

    static bool Exists() {
        return fObj != nullptr;
    }

    void Update(float dT);

    void CalculateRankings();

    void SortCheckPointRankings();

    PlayMode GetPlayMode() const {
        return mPlayMode;
    }

    GRace::Context GetRaceContext() const {
        return mRaceContext;
    }

    void SetRaceContext(GRace::Context context);

    void SetIsLoading(bool loading) {
        mIsLoading = loading;
    };

    bool GetIsLoading() const {
        return mIsLoading;
    };

    GRace::Type GetRaceType() const {
        return mRaceParms ? mRaceParms->GetRaceType() : GRace::kRaceType_None;
    }

    GRaceParameters *GetRaceParameters() const {
        return mRaceParms;
    }

    GRaceBin *GetCurrentBin() const {
        return mRaceBin;
    }

    void RefreshBinWhileInGame();

    GRacerInfo &GetRacerInfo(int index);

    GRacerInfo *GetRacerInfo(ISimable *isim);

    int GetRacerCount() const;

    GRacerInfo *GetWinningPlayerInfo();

    void StartMasterTimer();

    void StopMasterTimer();

    float GetRaceTimeElapsed() const;

    bool GetIsTimeLimited() const {
        return mRaceParms && mRaceParms->GetTimeLimit() > 0.0f;
    }

    float GetRaceTimeRemaining() const;

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

    bool GetRaceRouteError() {}

    float GetRaceLength() const {
        return fRaceLength;
    }

    float GetFirstLapLength() {}

    float GetSegmentLength(int n) {
        return mSegmentLengths[n];
    }

    float GetSubsequentLapLength() {
        return fSubsequentLapLength;
    }

    float GetLapLength(int lap) {}

    float GetSegmentLength(int segment, int lap);

    float GetAveragePercentComplete() {
        return fAveragePercentComplete;
    }

    bool IsLoading();

    bool IsAudioLoading();

    bool IsModelsLoading();

    bool GetIsScriptWaitingForLoading() const {
        return mScriptWaitingForLoad;
    }

    static bool IsDragRace() {
        return Exists() && Get().GetRaceType() == GRace::kRaceType_Drag;
    }

    static bool IsP2PRace() {
        return Exists() && Get().GetRaceType() == GRace::kRaceType_P2P;
    }

    static bool IsSprintRace() {
        return IsP2PRace();
    }

    static bool IsCircuitRace() {
        return Exists() && Get().GetRaceType() == GRace::kRaceType_Circuit;
    }

    static bool IsKnockoutRace() {
        return Exists() && Get().GetRaceType() == GRace::kRaceType_Knockout;
    }

    static bool IsTollboothRace() {
        return Exists() && Get().GetRaceType() == GRace::kRaceType_Tollbooth;
    }

    static bool IsChallengeRace() {
        return Exists() && Get().GetRaceType() == GRace::kRaceType_Challenge;
    }

    static bool IsSpeedTrapRace() {
        return Exists() && Get().GetRaceType() == GRace::kRaceType_SpeedTrap;
    }

    static bool IsFinalEpicPursuit() {
        return GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() && GRaceStatus::Get().GetRaceParameters()->GetIsEpicPursuitRace();
    }

    float GetBinBaseHeat() const {
        return mRaceBin->GetBaseOpenWorldHeat();
    }

    float GetBinMaxHeat() const {
        return mRaceBin->GetMaxOpenWorldHeat();
    }

    float GetBinHeatScale() const {
        return mRaceBin->GetScaleOpenWorldHeat();
    }

    void SkipToEndOfRaceForRacer(ISimable *thisPlayer, int index, float time);

    void SkipToEndOfRaceInPlace(int place, float finishTime);

    void ClearRacers();

    GRacerInfo &AddSimablePlayer(ISimable *isim);

    void AddRacer(GRuntimeInstance *racer);

    void NotifyScriptWhenLoaded();

    void LockVehicleCache(bool lock) {
        mVehicleCacheLocked = lock;
    }

    int GetTrafficDensity() const {
        return mTrafficDensity;
    }

    void SetTrafficDensity(int density) {
        mTrafficDensity = density;
    }

    Attrib::Key GetTrafficPattern() const {
        return mTrafficPattern;
    }

    void SetTrafficPattern(Attrib::Key key) {
        mTrafficPattern = key;
    }

    void ClearTrafficPattern() {}

    void SetRoaming();

    void SetRacing();

    void SetRaceActivity(GActivity *activity);

    void EnterSuddenDeath() {
        mSuddenDeathMode = true;
    }

    bool GetIsSuddenDeath() const {
        return mSuddenDeathMode;
    }

    void EnableBarriers();

    void DisableBarriers();

    void EnableBinBarriers();

    void DisableBinBarriers();

    void ClearCheckpoints();

    void AddCheckpoint(GRuntimeInstance *trigger);

    void SetNextCheckpointPos(GRuntimeInstance *trigger);

    void DetermineRaceLength();

    void MakeCatchUpData();

    bool ComputeCatchUpSkill(GRacerInfo *racer_info, PidError *pid, float *output, float *skill, bool off_world);

#if EA_BUILD_A124
    void UpdateAdaptiveBonus();
#endif

    void SyncronizeAdaptiveBonus();

    float GetAdaptiveDifficutly() const;

    void UpdateAdaptiveDifficulty(eAdaptiveGainReason reason, ISimable *who);

    void AddAvailableEventToMap(GRuntimeInstance *trigger, GRuntimeInstance *activity);

    void AddSpeedTrapToMap(GRuntimeInstance *trigger);

    void AwardBonusTime(float seconds);

    void SetTaskTime(float seconds) {
        mTaskTime = seconds;
    }

    float GetTaskTime() const {
        return mTaskTime;
    }

    void SetActivelyRacing(bool racing) {
        mActivelyRacing = racing;
    }

    bool GetActivelyRacing() const {
        return mActivelyRacing;
    }

    int GetCheckpointCount();

    GTrigger *GetCheckpoint(int index);

    GTrigger *GetNextCheckpoint();

    int GetNumRaceSpeedTraps() {
        return nSpeedTraps;
    }

    float GetRaceSpeedTrapSpeed(int trapIndex, int racerIndex);

    int GetRaceSpeedTrapPosition(int trapIndex, int racerIndex);

    float GetBestSpeedTrapSpeed(int racerIndex);

    float GetWorstSpeedTrapSpeed(int racerIndex);

    GTrigger *GetRaceSpeedTrap(int n) {
        return aSpeedTraps[n];
    }

    int GetNumRaceTollbooths() {
        return mNumTollbooths;
    }

    float GetRaceTollboothTime(int boothIndex, int racerIndex);

    void RaceAbandoned();

    void EndStopAll();

    void FinalizeRaceStats();

    // Overrides: IVehicleCache
    enum eVehicleCacheResult OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const override;

    // Overrides: IVehicleCache
    void OnRemovedVehicleCache(IVehicle *ivehicle) override;

    bool CanUnspawnRoamer(const IVehicle *roamer) const;

  private:
    GRaceStatus();
    ~GRaceStatus() override;
    void EnterBin(unsigned int binNumber);
    float DetermineRaceSegmentLength(const UMath::Vector4 *positions, const UMath::Vector4 *directions, int start, int end);
    static void NotifyEmitterGroupDelete(void *obj, EmitterGroup *group);
    void ParseCatchUpData(const char *skill, const char *spread);
    void MakeDefaultCatchUpData();

    static GRaceStatus *fObj;

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
    float mBonusTime;                 // offset 0x1AC4, size 0x4
    float mTaskTime;                  // offset 0x1AC8, size 0x4
    bool mSuddenDeathMode;            // offset 0x1ACC, size 0x1
    bool mTimeExpiredMsgSent;         // offset 0x1AD0, size 0x1
    bool mActivelyRacing;             // offset 0x1AD4, size 0x1
    int mLastSecondTickSent;          // offset 0x1AD8, size 0x4
    WorldModel *mCheckpointModel;     // offset 0x1ADC, size 0x4
    EmitterGroup *mCheckpointEmitter; // offset 0x1AE0, size 0x4
    bool mQueueBinChange;             // offset 0x1AE4, size 0x1
#ifndef EA_BUILD_A124
    unsigned int mWarpWhenInFreeRoam; // offset 0x1AE8, size 0x4
#endif
    int mNumTollbooths;         // offset 0x1AEC, size 0x4
    bool mScriptWaitingForLoad; // offset 0x1AF0, size 0x1
#ifndef EA_BUILD_A124
    bool mRefreshBinAfterRace; // offset 0x1AF4, size 0x1
#endif
    TriggerList mCheckpoints;      // offset 0x1AF8, size 0x10
    GTrigger *mNextCheckpoint;     // offset 0x1B08, size 0x4
    float mLapTimes[10][16];       // offset 0x1B0C, size 0x280
    float mCheckTimes[10][16][16]; // offset 0x1D8C, size 0x2800
    float mSegmentLengths[18];     // offset 0x458C, size 0x48
    float fRaceLength;             // offset 0x45D4, size 0x4
    float fFirstLapLength;         // offset 0x45D8, size 0x4
    float fSubsequentLapLength;    // offset 0x45DC, size 0x4
#ifndef EA_BUILD_A124
    bool mCaluclatedAdaptiveGain; // offset 0x45E0, size 0x1
#endif
    float fCatchUpIntegral;        // offset 0x45E4, size 0x4
    float fCatchUpDerivative;      // offset 0x45E8, size 0x4
    float fCatchUpAdaptiveBonus;   // offset 0x45EC, size 0x4
    float fAveragePercentComplete; // offset 0x45F0, size 0x4
    int nCatchUpSkillEntries;      // offset 0x45F4, size 0x4
    float aCatchUpSkillData[11];   // offset 0x45F8, size 0x2C
    int nCatchUpSpreadEntries;     // offset 0x4624, size 0x4
    float aCatchUpSpreadData[11];  // offset 0x4628, size 0x2C
    int nSpeedTraps;               // offset 0x4654, size 0x4
    GTrigger *aSpeedTraps[16];     // offset 0x4658, size 0x40
    bool mVehicleCacheLocked;      // offset 0x4698, size 0x1
    bool bRaceRouteError;          // offset 0x469C, size 0x1
    int mTrafficDensity;           // offset 0x46A0, size 0x4
    Attrib::Key mTrafficPattern;   // offset 0x46A4, size 0x4
    bool mHasBeenWon;              // offset 0x46A8, size 0x1
};

// total size: 0x28
class GRaceCustom : public GRaceParameters {
  private:
    // void *operator new(unsigned int size, void *ptr) {}

    // void operator delete(void *mem, void *ptr) {}

    // void *operator new(unsigned int size) {}

    // void operator delete(void *mem, unsigned int size) {}

    // void *operator new(unsigned int size, const char *name) {}

    // void operator delete(void *mem, const char *name) {}

    // void operator delete(void *mem, unsigned int size, const char *name) {}

  public:
    // Overrides: GRaceParameters
    void GetCheckpointPosition(unsigned int index, UMath::Vector3 &pos) const override;

    // Overrides: GRaceParameters
    void GetCheckpointDirection(unsigned int index, UMath::Vector3 &dir) const override;

    void SetNumLaps(int numLaps);

    void SetBustedLives(int bustedLives);

    void SetPositionalKnockout(bool enabled, int knockoutsPerLap);

    void SetTimedKnockout(bool enabled, float secondsBehindLeader);

    void SetCopsEnabled(bool copsEnabled);

#if EA_BUILD_A124
    void SetCopDensity(GRace::CopDensity copDensity);
#else
    void SetHeatLevel(int level) {
        mHeatLevel = level;
    }

    void SetForceHeatLevel(int level);

    void SetAttribute(Attrib::Key key, const int &value, Attrib::Key index);

    void SetAttribute(Attrib::Key key, const float &value, unsigned int index);

    void SetAttribute(unsigned int key, const bool &value, unsigned int index);

#endif

    void SetTrafficDensity(int density);

    void SetDifficulty(GRace::Difficulty difficulty);

    void SetReversed(bool isReverseDir);

    void SetCatchUp(bool catchUpEnabled);

    void SetNumOpponents(int numOpponents);

    void CreateRaceActivity();

    GActivity *GetRaceActivity() const;

  private:
    GRaceCustom(const struct GRaceParameters &other);

    // Overrides: GRaceParameters
    ~GRaceCustom() override;

    void SetFreedByOwner() {
        mFreedByOwner = true;
    }

    bool GetFreedByOwner() {
        return mFreedByOwner;
    }

    GActivity *mRaceActivity;   // offset 0x14, size 0x4
    unsigned int mNumOpponents; // offset 0x18, size 0x4
    bool mReversed;             // offset 0x1C, size 0x1
    bool mFreedByOwner;         // offset 0x20, size 0x1
    int mHeatLevel;             // offset 0x24, size 0x4
};

#endif
