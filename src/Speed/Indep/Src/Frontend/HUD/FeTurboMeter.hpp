#ifndef FRONTEND_HUD_FETURBOMETER_H
#define FRONTEND_HUD_FETURBOMETER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"

class TurboMeter : public HudElement, public ITurbometer {
  public:
    TurboMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetInductionPsi(float psi) override;
    float CalcNeedleAngle(float psi, float min_angle, float max_angle);

  private:
    bool mUpdated;
    float mInductionPsi;
    FEGroup * pTurboGroup;
    FEObject * pTurboDialLines;
    FEObject * pTurboNeedle;
};

#endif
