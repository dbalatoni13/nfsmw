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
    void SetNumRacers(int numRacers) override;
    void SetNumLaps(int numLaps) override;
    void SetPlayerPosition(int position) override;
    void SetPlayerLapTime(float lapTime) override;
    void SetSuddenDeathMode(bool suddenDeath) override;
    void SetPlayerPercentComplete(float percent) override;
    void SetPlayerTollboothsCrossed(int num) override;
    void SetNumTollbooths(int num) override;
    void SetPlayerLapNumber(int lap) override;

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
    FEString * mDataCurrentLapTime;
    FEString * mDataCompleteText;
    FEGroup * mDataPositionGroup;
    FEImage * mDataIconTollbooth;
    FEObject * mpDataTollboothNumTop;
    FEObject * mpDataTollboothNumBot;
    FEObject * mpDataRacePosNum;
    FEObject * mpDataRacerCount;
    FEObject * mpDataPercentComplete;
};

#endif
