#ifndef FEONLINEHUDSUPPORT_H
#define FEONLINEHUDSUPPORT_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

class OnlineHUDSupport : public HudElement {
  public:
    OnlineHUDSupport(const char *pkg_name);
    void Update(IPlayer *player) override;
    void DisplayGenericMessage(const char *message);

  private:
    const char *pPackageName;
    IGenericMessage *mIGenericMessage;
};

#endif
