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
    void SetInPursuit(bool inPursuit) override;
    void SetChallengeSeries(bool challenge) override;
    void SetNumberOfMilestones(int num) override;
    void SetMilestoneIconHash(int milestoneNum, int hash) override {
        mMilestones[milestoneNum].mMilestoneIconHash = hash;
    }
    void SetMilestoneType(int milestoneNum, unsigned int type) override {
        mMilestones[milestoneNum].mType = type;
    }
    void SetMilestoneGoal(int milestoneNum, float goal) override {
        mMilestones[milestoneNum].mGoal = goal;
    }
    void SetMilestoneHeaderHash(int milestoneNum, int hash) override {
        mMilestones[milestoneNum].mHeaderHash = hash;
    }
    void SetMilestoneComplete(int milestoneNum, bool complete) override;
    void SetMilestoneCurrValue(int milestoneNum, float currVal) override;
    bool GetIsMilestoneComplete(int index) const;
    int GetNumIncompleteMilestones() const;
    int GetNumCompleteMilestones() const;
    int GetNextVisibleMilestone() const;
    int GetFirstIncompleteMilestone() const;

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
