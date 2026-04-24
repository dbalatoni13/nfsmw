#ifndef FRONTEND_HUD_FEBUSTEDMETER_H
#define FRONTEND_HUD_FEBUSTEDMETER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

class BustedMeter : public HudElement, public IBustedMeter {
  public:
    BustedMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetInPursuit(bool inPursuit) override;
    void SetIsHiding(bool isHiding) override;
    void SetTimeUntilBusted(float time) override;
    void SetIsBusted(bool isBusted) override;

  private:
    bool mInPursuit;
    float mTimeUntilBusted;
    bool mIsHiding;
    FEObject * mpDataBustedCountdownGroup;
    FEObject * mpDataBustedCountdownBar;
    float mBustedCountdownBarOriginalWidth;
    bool mIsBusted;
    bool mBustedFlasherShown;
};

#endif
