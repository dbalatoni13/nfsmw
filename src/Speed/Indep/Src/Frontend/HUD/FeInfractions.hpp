#ifndef FRONTEND_HUD_FEINFRACTIONS_H
#define FRONTEND_HUD_FEINFRACTIONS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"

class Infractions : public HudElement, public IInfractions {
  public:
    Infractions(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void RequestInfraction(const char *infractionString) override;

  private:
    FEObject * mpDataGenericIcon;
    FEGroup * mpDataInfractionStrings[4];
    FEString * mpDataTotalInfractions;
};

#endif
