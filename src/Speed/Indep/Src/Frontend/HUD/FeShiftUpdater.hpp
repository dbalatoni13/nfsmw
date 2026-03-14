#ifndef FRONTEND_HUD_FESHIFTUPDATER_H
#define FRONTEND_HUD_FESHIFTUPDATER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"

class ShiftUpdater : public HudElement, public IShiftUpdater {
  public:
    ShiftUpdater(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetGear(GearID gear, ShiftStatus status, ShiftPotential potential, bool hasGoodEnoughTraction) override;
    void SetEngineBlown(bool blown) override;
    void SetEngineTemp(float temp) override;

  private:
    FEImage * pShiftIndicator;
    FEGroup * pShiftIndicatorOverheatGroup;
    FEGroup * pShiftMsgGroup;
    FEString * pShiftMsg;
    FEString * pShiftMsgShadow;
    GearID mGear;
    ShiftPotential mShiftPotential;
    int mGearChanged;
    ShiftStatus mLastShiftStatus;
    bool mIsEngineBlown;
    float mEngineTemp;
};

#endif
