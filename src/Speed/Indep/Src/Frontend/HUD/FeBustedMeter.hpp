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
    void SetInPursuit(bool inPursuit) override {
        mInPursuit = inPursuit;
    };
    void SetIsHiding(bool isHiding) override {
        mIsHiding = isHiding;
    };
    void SetTimeUntilBusted(float time) override {
        mTimeUntilBusted = time;
    };
    void SetIsBusted(bool isBusted) override {
        mIsBusted = isBusted;
        if (!isBusted) {
            if (mBustedFlasherShown) {
                mBustedFlasherShown = false;
            }
        }
    };

  private:
    bool mInPursuit;
    float mTimeUntilBusted;
    bool mIsHiding;
    FEObject *mpDataBustedCountdownGroup;
    FEObject *mpDataBustedCountdownBar;
    float mBustedCountdownBarOriginalWidth;
    bool mIsBusted;
    bool mBustedFlasherShown;
};

#endif
