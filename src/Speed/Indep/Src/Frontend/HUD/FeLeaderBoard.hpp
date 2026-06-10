#ifndef FELEADERBOARD_H
#define FELEADERBOARD_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"

// total size: 0x80
struct LeaderBoardRacerData {
    char mRacerName[16];          // offset 0x0, size 0x10
    float mPercentComplete;       // offset 0x10, size 0x4
    float mRaceTimeOfSegment[20]; // offset 0x14, size 0x50
    float mRaceTimeOfLastLap;     // offset 0x64, size 0x4
    int mRacerNum;                // offset 0x68, size 0x4
    float mTotalPoints;           // offset 0x6C, size 0x4
    int mNumLapsCompleted;        // offset 0x70, size 0x4
    bool mHasHeadset;             // offset 0x74, size 0x1
    bool mIsBusted;               // offset 0x78, size 0x1
    bool mIsKoed;                 // offset 0x7C, size 0x1
};

// File: speed/indep/src/frontend/hud/FeLeaderBoard.hpp
// total size: 0x290
// Decl: speed/indep/src/frontend/hud/FeLeaderBoard.hpp:8
class LeaderBoard : public HudElement, public ILeaderBoard {
  public:
    LeaderBoard(struct Object *pOutter, const char *pkg_name, int player_number);
    ~LeaderBoard() override {} // Decl: speed/indep/src/frontend/hud/FeLeaderBoard.hpp:11

  private:
    int mNumRacers;                          // offset 0x30, size 0x4
    int mNumLaps;                            // offset 0x34, size 0x4
    int mPlayerIndex;                        // offset 0x38, size 0x4
    bool mSplitTimeQueued;                   // offset 0x3C, size 0x1
    LeaderBoardRacerData mTopRacers[4];      // offset 0x40, size 0x200
    int mNumFramesBeforeTogglingPlayerTimes; // offset 0x240, size 0x4
    bool mShowingRacerTimes;                 // offset 0x244, size 0x1, Decl: speed/indep/src/frontend/hud/FeLeaderBoard.hpp:22

    FEGroup *mDataLeaderboardGroup;     // offset 0x248, size 0x4, Decl: speed/indep/src/frontend/hud/FeLeaderBoard.hpp:26
    FEString *mDataRacerText[4];        // offset 0x24C, size 0x10
    FEString *mDataRacerNum[4];         // offset 0x25C, size 0x10, Decl: speed/indep/src/frontend/hud/FeLeaderBoard.hpp:30
    FEImage *mDataRacerIcon[4];         // offset 0x26C, size 0x10
    FEImage *mDataRacerTextBackings[4]; // offset 0x27C, size 0x10

  public:
    static const int mTopRacerCount; // size: 0x4, address: 0xFFFFFFFF

    void Update(IPlayer *player) override;

    void SetNumRacers(int numRacers) override {
        mNumRacers = numRacers;
    }

    void SetNumLaps(int numLaps) override {
        mNumLaps = numLaps;
    }

    void SetPlayerIndex(int index) override {
        mPlayerIndex = index;
    }

    void SetRacerName(int pos, const char *name) override;

    void SetRacerNum(int pos, int num) override;

    void SetRacerTotalPoints(int pos, float points) override;

    void SetRacerNumLapsCompleted(int pos, int numLaps, float time, IPlayer *player) override;

    void SetRacerPercentComplete(int pos, float percent, float time, IPlayer *player) override;

    void SetRacerHasHeadset(int pos, bool racerHasHeadset) override;

    void SetRacerIsBusted(int pos, bool busted) override {
        mTopRacers[pos].mIsBusted = busted;
    }

    void SetRacerIsKoed(int pos, bool koed) override {
        mTopRacers[pos].mIsKoed = koed;
    }

  private:
    bool ShowSplitTime(IPlayer *player);

    bool ShowLapTime(IPlayer *player) const;
};

#endif
