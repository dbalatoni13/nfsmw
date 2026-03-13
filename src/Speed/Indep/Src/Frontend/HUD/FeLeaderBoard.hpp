#ifndef FRONTEND_HUD_FELEADERBOARD_H
#define FRONTEND_HUD_FELEADERBOARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"

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

  private:
    LeaderBoardRacerData mTopRacers[4];
    int mNumRacers;
    int mNumLaps;
    int mPlayerIndex;
    FEString * mpDataNames[4];
    FEString * mpDataTimes[4];
    FEString * mpDataLaps[4];
    FEObject * mpDataIcons[4];
    FEObject * mpDataIconBackings[4];
};

#endif
