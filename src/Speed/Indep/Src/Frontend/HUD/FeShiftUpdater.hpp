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
    void SetGear(GearID gear, ShiftStatus status, ShiftPotential potential, bool hasGoodEnoughTraction) override {
        if (gear != mGear) {
            int dir = -1;
            if (gear > mGear) {
                dir = 1;
            }
            mGear = gear;
            mGearChanged = dir;
            mShiftPotential = SHIFT_POTENTIAL_NONE;
            if (hasGoodEnoughTraction) {
                mLastShiftStatus = status;
            } else {
                mLastShiftStatus = SHIFT_STATUS_NORMAL;
            }
            return;
        }
        if (hasGoodEnoughTraction) {
            mShiftPotential = potential;
        } else {
            mShiftPotential = SHIFT_POTENTIAL_NONE;
        }
    };
    void SetEngineBlown(bool blown) override {
        mIsEngineBlown = blown;
    };
    void SetEngineTemp(float temp) override {
        mEngineTemp = temp;
    };

  private:
    FEImage *pShiftIndicator;
    FEGroup *pShiftIndicatorOverheatGroup;
    FEGroup *pShiftMsgGroup;
    FEString *pShiftMsg;
    FEString *pShiftMsgShadow;
    GearID mGear;
    ShiftPotential mShiftPotential;
    int mGearChanged;
    ShiftStatus mLastShiftStatus;
    bool mIsEngineBlown;
    float mEngineTemp;
};

#endif
