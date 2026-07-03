#ifndef FERACEOVERMESSAGE_H
#define FERACEOVERMESSAGE_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

// File: speed/indep/src/frontend/hud/FeRaceOverMessage.hpp
// total size: 0x38
// Decl: speed/indep/src/frontend/hud/FeRaceOverMessage.hpp:8
class RaceOverMessage : public HudElement, public IRaceOverMessage {
  public:
    RaceOverMessage(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~RaceOverMessage() override {} // Decl: speed/indep/src/frontend/hud/FeRaceOverMessage.hpp:11

    void Update(IPlayer *player) override;

  private:
    void RequestRaceOverMessage(IPlayer *player) override;
    void DismissRaceOverMessage() override;
    bool ShouldShowRaceOverMessage() override {
        return bShowMessage;
    };

    bool bShowMessage;         // offset 0x30, size 0x1
    bool bShowTotalledMessage; // offset 0x34, size 0x1
};

#endif
