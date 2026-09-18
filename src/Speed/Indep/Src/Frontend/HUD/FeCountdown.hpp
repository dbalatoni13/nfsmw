#ifndef FECOUNTDOWN_H
#define FECOUNTDOWN_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// File: speed/indep/src/frontend/hud/FeCountdown.hpp
// Decl: speed/indep/src/frontend/hud/FeCountdown.hpp:13
typedef enum {
    RACE_COUNTDOWN_NUMBER_NONE = -1,
    RACE_COUNTDOWN_NUMBER_GO = 0,
    RACE_COUNTDOWN_NUMBER_1 = 1,
    RACE_COUNTDOWN_NUMBER_2 = 2,
    RACE_COUNTDOWN_NUMBER_3 = 3,
    RACE_COUNTDOWN_NUMBER_4 = 4
} eRaceCountdownNumber;

// total size: 0x48
// Decl: speed/indep/src/frontend/hud/FeCountdown.hpp:25
class Countdown : public HudElement, public ICountdown {
  public:
    Countdown(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~Countdown() override {} // Decl: speed/indep/src/frontend/hud/FeCountdown.hpp:28

    void Update(IPlayer *player) override;

    void BeginCountdown() override;

    bool IsActive() override;

    float GetSecondsBeforeRaceStart() override;

  private:
    FEGroup *pMessageGroup;          // offset 0x30, size 0x4, Decl: speed/indep/src/frontend/hud/FeCountdown.hpp:39
    FEString *pMessage;              // offset 0x34, size 0x4, Decl: speed/indep/src/frontend/hud/FeCountdown.hpp:40
    FEString *pMessageShadow;        // offset 0x38, size 0x4
    eRaceCountdownNumber mCountdown; // offset 0x3C, size 0x4, Decl: speed/indep/src/frontend/hud/FeCountdown.hpp:42
    Timer mSecondTimer;              // offset 0x40, size 0x4, Decl: speed/indep/src/frontend/hud/FeCountdown.hpp:45
};

#endif
