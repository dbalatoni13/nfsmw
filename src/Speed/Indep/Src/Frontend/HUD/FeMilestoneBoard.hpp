#ifndef FRONTEND_HUD_FEMILESTONEBOARD_H
#define FRONTEND_HUD_FEMILESTONEBOARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// total size: 0x18
class MilestoneBoard_Milestone {
  public:
    unsigned int mMilestoneIconHash;
    unsigned int mType;
    float mGoal;
    float mCurrVal;
    int mHeaderHash;
    bool mComplete;
};

// total size: 0xE0
class MilestoneBoard : public HudElement, public IMilestoneBoard {
  public:
    MilestoneBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetInPursuit(bool inPursuit) override {
        mInPursuit = inPursuit;
    }
    void SetChallengeSeries(bool challenge) override {
        mChallengeSeries = challenge;
    };
    void SetNumberOfMilestones(int num) override {
        mNumMilestones = num;
    };
    void SetMilestoneIconHash(int milestoneNum, int hash) override {
        mMilestones[milestoneNum].mMilestoneIconHash = hash;
    }
    void SetMilestoneType(int milestoneNum, unsigned int type) override {
        mMilestones[milestoneNum].mType = type;
    }
    void SetMilestoneGoal(int milestoneNum, float goal) override {
        mMilestones[milestoneNum].mGoal = goal;
    }
    void SetMilestoneComplete(int milestoneNum, bool complete) override;
    void SetMilestoneCurrValue(int milestoneNum, float currVal) override;
    void SetMilestoneHeaderHash(int milestoneNum, int hash) override {
        mMilestones[milestoneNum].mHeaderHash = hash;
    }

  private:
    int GetNumIncompleteMilestones() const;
    int GetNumCompleteMilestones() const;
    int GetNextVisibleMilestone() const;
    int GetFirstIncompleteMilestone() const;
    bool GetIsMilestoneComplete(int index) const;

    bool mInPursuit;                         // offset 0x30, size 0x1
    bool mChallengeSeries;                   // offset 0x34, size 0x1
    int mPlayerBinNumber;                    // offset 0x38, size 0x4
    int mNumMilestones;                      // offset 0x3C, size 0x4
    int mMilestoneSetVisible;                // offset 0x40, size 0x4
    Timer mScrollTimer;                      // offset 0x44, size 0x4
    MilestoneBoard_Milestone mMilestones[4]; // offset 0x48, size 0x60
    FEObject *mpDataMilestoneInfoGroup;      // offset 0xA8, size 0x4
    FEObject *mpDataMilestoneIconGroup;      // offset 0xAC, size 0x4
    FEString *mpDataMilestonesTotal;         // offset 0xB0, size 0x4
    FEObject *mpDataIcons[4];                // offset 0xB4, size 0x10
    FEObject *mpDataIconBackings[4];         // offset 0xC4, size 0x10
    FEObject *mpDataDetailsBacking;          // offset 0xD4, size 0x4
    FEObject *mpDataDetailsGroup;            // offset 0xD8, size 0x4
    FEString *mpDataMilestoneGoal;           // offset 0xDC, size 0x4
};

#endif
