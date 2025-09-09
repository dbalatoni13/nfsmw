#ifndef GAMEPLAY_GRACE_STATUS_H
#define GAMEPLAY_GRACE_STATUS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GRace.h"
#include "GTimer.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/Simables/isimable.h"

struct GRacerInfo {
    // total size: 0x1A8
    HSIMABLE mhSimable;                    // offset 0x0, size 0x4
    struct GCharacter *mGameCharacter;     // offset 0x4, size 0x4
    const char *mName;                     // offset 0x8, size 0x4
    int mIndex;                            // offset 0xC, size 0x4
    int mRanking;                          // offset 0x10, size 0x4
    int mAiRanking;                        // offset 0x14, size 0x4
    float mPctRaceComplete;                // offset 0x18, size 0x4
    bool mKnockedOut;                      // offset 0x1C, size 0x1
    bool mTotalled;                        // offset 0x20, size 0x1
    bool mEngineBlown;                     // offset 0x24, size 0x1
    bool mBusted;                          // offset 0x28, size 0x1
    bool mChallengeComplete;               // offset 0x2C, size 0x1
    bool mFinishedRacing;                  // offset 0x30, size 0x1
    bool mCameraDetached;                  // offset 0x34, size 0x1
    float mPctLapComplete;                 // offset 0x38, size 0x4
    int mLapsCompleted;                    // offset 0x3C, size 0x4
    int mCheckpointsHitThisLap;            // offset 0x40, size 0x4
    int mTollboothsCrossed;                // offset 0x44, size 0x4
    float mTimeRemainingToBooth[16];       // offset 0x48, size 0x40
    int mSpeedTrapsCrossed;                // offset 0x88, size 0x4
    float mSpeedTrapSpeed[16];             // offset 0x8C, size 0x40
    int mSpeedTrapPosition[16];            // offset 0xCC, size 0x40
    float mDistToNextCheckpoint;           // offset 0x10C, size 0x4
    float mDistanceDriven;                 // offset 0x110, size 0x4
    float mTopSpeed;                       // offset 0x114, size 0x4
    float mFinishingSpeed;                 // offset 0x118, size 0x4
    float mPoundsNOSUsed;                  // offset 0x11C, size 0x4
    float mTimeCrossedLastCheck;           // offset 0x120, size 0x4
    float mTotalUpdateTime;                // offset 0x124, size 0x4
    int mNumPerfectShifts;                 // offset 0x128, size 0x4
    int mNumTrafficCarsHit;                // offset 0x12C, size 0x4
    float mSpeedBreakerTime;               // offset 0x130, size 0x4
    float mPointTotal;                     // offset 0x134, size 0x4
    float mZeroToSixtyTime;                // offset 0x138, size 0x4
    float mQuarterMileTime;                // offset 0x13C, size 0x4
    float mSplitTimes[4];                  // offset 0x140, size 0x10
    int mSplitRankings[4];                 // offset 0x150, size 0x10
    GTimer mRaceTimer;                     // offset 0x160, size 0xC
    GTimer mLapTimer;                      // offset 0x16C, size 0xC
    GTimer mCheckTimer;                    // offset 0x178, size 0xC
    struct UMath::Vector3 mSavedPosition;  // offset 0x184, size 0xC
    float mSavedHeatLevel;                 // offset 0x190, size 0x4
    struct UMath::Vector3 mSavedDirection; // offset 0x194, size 0xC
    float mSavedSpeed;                     // offset 0x1A0, size 0x4
    bool mDNF;                             // offset 0x1A4, size 0x1
};

struct _type_ID_GRaceStatusTriggerList {
    const char *name() {
        return "GRaceStatusTriggerList";
    }
};

class GRaceStatus : public UTL::COM::Object, public IVehicleCache {
  public:
    enum PlayMode {
        kPlayMode_Roaming = 0,
        kPlayMode_Racing = 1,
    };

  private:
    // total size: 0x46AC
    GRacerInfo mRacerInfo[16];                                                         // offset 0x1C, size 0x1A80
    int mRacerCount;                                                                   // offset 0x1A9C, size 0x4
    bool mIsLoading;                                                                   // offset 0x1AA0, size 0x1
    PlayMode mPlayMode;                                                                // offset 0x1AA4, size 0x4
    GRace::Context mRaceContext;                                                       // offset 0x1AA8, size 0x4
    struct GRaceParameters *mRaceParms;                                                // offset 0x1AAC, size 0x4
    struct GRaceBin *mRaceBin;                                                         // offset 0x1AB0, size 0x4
    GTimer mRaceMasterTimer;                                                           // offset 0x1AB4, size 0xC
    bool mPlayerPursuitInCooldown;                                                     // offset 0x1AC0, size 0x1
    float mBonusTime;                                                                  // offset 0x1AC4, size 0x4
    float mTaskTime;                                                                   // offset 0x1AC8, size 0x4
    bool mSuddenDeathMode;                                                             // offset 0x1ACC, size 0x1
    bool mTimeExpiredMsgSent;                                                          // offset 0x1AD0, size 0x1
    bool mActivelyRacing;                                                              // offset 0x1AD4, size 0x1
    int mLastSecondTickSent;                                                           // offset 0x1AD8, size 0x4
    struct WorldModel *mCheckpointModel;                                               // offset 0x1ADC, size 0x4
    EmitterGroup *mCheckpointEmitter;                                                  // offset 0x1AE0, size 0x4
    bool mQueueBinChange;                                                              // offset 0x1AE4, size 0x1
    unsigned int mWarpWhenInFreeRoam;                                                  // offset 0x1AE8, size 0x4
    int mNumTollbooths;                                                                // offset 0x1AEC, size 0x4
    bool mScriptWaitingForLoad;                                                        // offset 0x1AF0, size 0x1
    bool mRefreshBinAfterRace;                                                         // offset 0x1AF4, size 0x1
    UTL::Std::vector<struct GTrigger *, _type_ID_GRaceStatusTriggerList> mCheckpoints; // offset 0x1AF8, size 0x10
    struct GTrigger *mNextCheckpoint;                                                  // offset 0x1B08, size 0x4
    float mLapTimes[10][16];                                                           // offset 0x1B0C, size 0x280
    float mCheckTimes[10][16][16];                                                     // offset 0x1D8C, size 0x2800
    float mSegmentLengths[18];                                                         // offset 0x458C, size 0x48
    float fRaceLength;                                                                 // offset 0x45D4, size 0x4
    float fFirstLapLength;                                                             // offset 0x45D8, size 0x4
    float fSubsequentLapLength;                                                        // offset 0x45DC, size 0x4
    bool mCaluclatedAdaptiveGain;                                                      // offset 0x45E0, size 0x1
    float fCatchUpIntegral;                                                            // offset 0x45E4, size 0x4
    float fCatchUpDerivative;                                                          // offset 0x45E8, size 0x4
    float fCatchUpAdaptiveBonus;                                                       // offset 0x45EC, size 0x4
    float fAveragePercentComplete;                                                     // offset 0x45F0, size 0x4
    int nCatchUpSkillEntries;                                                          // offset 0x45F4, size 0x4
    float aCatchUpSkillData[11];                                                       // offset 0x45F8, size 0x2C
    int nCatchUpSpreadEntries;                                                         // offset 0x4624, size 0x4
    float aCatchUpSpreadData[11];                                                      // offset 0x4628, size 0x2C
    int nSpeedTraps;                                                                   // offset 0x4654, size 0x4
    struct GTrigger *aSpeedTraps[16];                                                  // offset 0x4658, size 0x40
    bool mVehicleCacheLocked;                                                          // offset 0x4698, size 0x1
    bool bRaceRouteError;                                                              // offset 0x469C, size 0x1
    int mTrafficDensity;                                                               // offset 0x46A0, size 0x4
    unsigned int mTrafficPattern;                                                      // offset 0x46A4, size 0x4
    bool mHasBeenWon;                                                                  // offset 0x46A8, size 0x1

    static struct GRaceStatus *fObj;

    GRaceStatus();

  public:
    static GRaceStatus &Get() {
        return *fObj;
    }

    static bool Exists() {
        return fObj != nullptr;
    }

    PlayMode GetPlayMode() {
        return mPlayMode;
    }

    unsigned int GetTrafficPattern() const {
        return mTrafficPattern;
    }
};

#endif
