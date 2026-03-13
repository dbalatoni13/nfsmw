#ifndef FRONTEND_HUD_FERACEOVERMESSAGE_H
#define FRONTEND_HUD_FERACEOVERMESSAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

class RaceOverMessage : public HudElement, public IRaceOverMessage {
  public:
    RaceOverMessage(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void RequestRaceOverMessage(IPlayer *player) override;
    void DismissRaceOverMessage() override;
    int ShouldShowRaceOverMessage() override;

  private:
    int bShowMessage;
    int bShowTotalledMessage;
};

#endif
