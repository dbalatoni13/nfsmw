#ifndef FRONTEND_HUD_FEMILESTONEBOARD_H
#define FRONTEND_HUD_FEMILESTONEBOARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

struct MilestoneBoard_Milestone {
    unsigned int mMilestoneIconHash;
    unsigned int mType;
    float mGoal;
    float mCurrVal;
    int mHeaderHash;
    bool mComplete;
};

class MilestoneBoard : public HudElement, public IMilestoneBoard {
  public:
    MilestoneBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;

  private:
    bool mInPursuit;
    bool mChallengeSeries;
    int mPlayerBinNumber;
    int mNumMilestones;
    int mMilestoneSetVisible;
    Timer mScrollTimer;
    MilestoneBoard_Milestone mMilestones[4];
    FEObject * mpDataMilestoneInfoGroup;
    FEObject * mpDataMilestoneIconGroup;
    FEString * mpDataMilestonesTotal;
    FEObject * mpDataIcons[4];
    FEObject * mpDataIconBackings[4];
    FEObject * mpDataDetailsBacking;
    FEObject * mpDataDetailsGroup;
    FEString * mpDataMilestoneGoal;
};

#endif
