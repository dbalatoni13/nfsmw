#ifndef FRONTEND_HUD_FERACEINFORMATION_H
#define FRONTEND_HUD_FERACEINFORMATION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"

class RaceInformation : public HudElement, public IRaceInformation {
  public:
    RaceInformation(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetNumRacers(int numRacers) override {
        mNumRacers = numRacers;
    };
    void SetNumLaps(int numLaps) override {
        mNumLaps = numLaps;
    };
    void SetPlayerPosition(int position) override {
        mPlayerPosition = position;
    };
    void SetPlayerLapNumber(int lap) override {
        if (lap > mNumLaps) {
            lap = mNumLaps;
        }
        mPlayerLapNumber = lap;
    };
    void SetPlayerLapTime(float lapTime) override {
        mPlayerLapTime = lapTime;
    };
    void SetSuddenDeathMode(bool suddenDeath) override {
        mSuddenDeath = suddenDeath;
    };
    void SetPlayerPercentComplete(float percent) override {
        mPlayerPercentComplete = percent;
    };
    void SetPlayerTollboothsCrossed(int num) override {
        mPlayerTollboothNumber = num;
    };
    void SetNumTollbooths(int num) override {
        mNumTollbooths = num;
    };

  private:
    int mNumRacers;
    int mNumLaps;
    int mPlayerPosition;
    int mPlayerLapNumber;
    float mPlayerLapTime;
    bool mSuddenDeath;
    float mPlayerPercentComplete;
    int mPlayerTollboothNumber;
    int mNumTollbooths;
    FEString *mDataCurrentLapTime;
    FEString *mDataCompleteText;
    FEGroup *mDataPositionGroup;
    FEImage *mDataIconTollbooth;
    FEObject *mpDataTollboothNumTop;
    FEObject *mpDataTollboothNumBot;
    FEObject *mpDataRacePosNum;
    FEObject *mpDataRacerCount;
    FEObject *mpDataPercentComplete;
};

#endif
