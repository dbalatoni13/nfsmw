#ifndef FETIMEEXTENSION_H
#define FETIMEEXTENSION_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// File: speed/indep/src/frontend/hud/FeTimeExtension.hpp
// total size: 0x48
// Decl: speed/indep/src/frontend/hud/FeTimeExtension.hpp:11
class TimeExtension : public HudElement, public ITimeExtension {
  public:
    TimeExtension(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~TimeExtension() override {} // Decl: speed/indep/src/frontend/hud/FeTimeExtension.hpp:14

    void Update(IPlayer *player) override;
    void SetPlayerLapTime(float lapTime) override {
        mPlayerLapTime = lapTime;
    }
    void RequestTimeExtensionMessage(IPlayer *iplayer, float timeToShow) override;

  private:
    bool mShowingCountdown;    // offset 0x30, size 0x1, Decl: speed/indep/src/frontend/hud/FeTimeExtension.hpp:21
    float mPlayerLapTime;      // offset 0x34, size 0x4
    float mTimeToShow;         // offset 0x38, size 0x4
    int mScriptHash;           // offset 0x3C, size 0x4
    Timer mTimerTimeExtension; // offset 0x40, size 0x4
    Timer mTimerNextTollbooth; // offset 0x44, size 0x4
};

#endif
