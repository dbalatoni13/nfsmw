#ifndef FEAUTOSAVEICON_H
#define FEAUTOSAVEICON_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

class AutoSaveIcon : public HudElement, public IAutoSaveIcon {
  public:
    AutoSaveIcon(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;

  private:
    FEObject *mpIcon;
    bool mShowingIcon;
    bool mIconRequested;
};

#endif
