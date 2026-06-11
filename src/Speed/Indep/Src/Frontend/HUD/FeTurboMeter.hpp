#ifndef FETURBOMETER_H
#define FETURBOMETER_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"

class TurboMeter : public HudElement, public ITurbometer {
  public:
    TurboMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetInductionPsi(float psi) override;

  private:
    float CalcNeedleAngle(float psi, float min_angle, float max_angle);

    bool mUpdated;             // offset 0x30, size 0x1
    float mInductionPsi;       // offset 0x34, size 0x4
    FEGroup *pTurboGroup;      // offset 0x38, size 0x4
    FEObject *pTurboDialLines; // offset 0x3C, size 0x4
    FEObject *pTurboNeedle;    // offset 0x40, size 0x4
};

#endif
