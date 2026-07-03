#ifndef FEONLINEHUDSUPPORT_H
#define FEONLINEHUDSUPPORT_H

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
