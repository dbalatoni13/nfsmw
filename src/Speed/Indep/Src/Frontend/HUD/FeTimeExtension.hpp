#ifndef FRONTEND_HUD_FETIMEEXTENSION_H
#define FRONTEND_HUD_FETIMEEXTENSION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

class TimeExtension : public HudElement, public ITimeExtension {
  public:
    TimeExtension(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetPlayerLapTime(float lapTime) override;
    void RequestTimeExtensionMessage(IPlayer *iplayer, float timeToShow) override;

  private:
    bool mShowingCountdown;
    float mPlayerLapTime;
    float mTimeToShow;
    int mScriptHash;
    Timer mTimerTimeExtension;
    Timer mTimerNextTollbooth;
};

#endif
