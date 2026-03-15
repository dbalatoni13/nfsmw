#ifndef FRONTEND_HUD_FELEADERBOARD_H
#define FRONTEND_HUD_FELEADERBOARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEImage.h"

struct LeaderBoardRacerData {
    char mRacerName[16];
    float mPercentComplete;
    float mRaceTimeOfSegment[20];
    float mRaceTimeOfLastLap;
    int mRacerNum;
    float mTotalPoints;
    int mNumLapsCompleted;
    bool mHasHeadset;
    bool mIsBusted;
    bool mIsKoed;
};

class LeaderBoard : public HudElement, public ILeaderBoard {
  public:
    LeaderBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetNumRacers(int numRacers) override;
    void SetNumLaps(int numLaps) override;
    void SetPlayerIndex(int index) override;
    void SetRacerIsBusted(int pos, bool busted) override;
    void SetRacerIsKoed(int pos, bool koed) override;
    void SetRacerName(int pos, const char *name) override;
    void SetRacerNum(int pos, int num) override;
    void SetRacerTotalPoints(int pos, float points) override;
    void SetRacerNumLapsCompleted(int pos, int numLaps, float time, IPlayer *player) override;
    void SetRacerPercentComplete(int pos, float percent, float time, IPlayer *player) override;
    void SetRacerHasHeadset(int pos, bool racerHasHeadset) override;
    bool ShowLapTime(IPlayer *player) const;
    bool ShowSplitTime(IPlayer *player);

  private:
    int mNumRacers;                            // offset 0x30
    int mNumLaps;                              // offset 0x34
    int mPlayerIndex;                          // offset 0x38
    bool mSplitTimeQueued;                     // offset 0x3C
    LeaderBoardRacerData mTopRacers[4];        // offset 0x40
    int mNumFramesBeforeTogglingPlayerTimes;   // offset 0x240
    int mShowingRacerTimes;                    // offset 0x244
    FEGroup *mDataLeaderboardGroup;            // offset 0x248
    FEString *mDataRacerText[4];               // offset 0x24C
    FEString *mDataRacerNum[4];                // offset 0x25C
    FEImage *mDataRacerIcon[4];                // offset 0x26C
    FEImage *mDataRacerTextBackings[4];        // offset 0x27C
};

#endif
