#ifndef FRONTEND_HUD_FEONLINEHUDSUPPORT_H
#define FRONTEND_HUD_FEONLINEHUDSUPPORT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

class OnlineHUDSupport : public HudElement {
  public:
    OnlineHUDSupport(const char *pkg_name);
    void Update(IPlayer *player) override;
    void DisplayGenericMessage();

  private:
    char *pPackageName;
    IGenericMessage *mIGenericMessage;
};

#endif
