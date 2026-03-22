#ifndef FRONTEND_HUD_FECOUNTDOWN_H
#define FRONTEND_HUD_FECOUNTDOWN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

class Countdown : public HudElement, public ICountdown {
  public:
    Countdown(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void BeginCountdown() override;
    bool IsActive() override;
    float GetSecondsBeforeRaceStart() override;

  private:
    FEGroup * pMessageGroup;
    FEString * pMessage;
    FEString * pMessageShadow;
    eRaceCountdownNumber mCountdown;
    Timer mSecondTimer;
};

#endif
