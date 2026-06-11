#ifndef FEENGINETEMPGAUGE_H
#define FEENGINETEMPGAUGE_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

// total size: 0x40
class EngineTempGauge : public HudElement, public IEngineTempGauge {
  public:
    EngineTempGauge(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetEngineTemp(float temp) override;

  private:
    float mEngineTemp;                  // offset 0x30
    bool mEngineTempChanged;            // offset 0x34
    FEObject *mpWarningLight;           // offset 0x38
    FEMultiImage *mpEngineTempGaugeBar; // offset 0x3C
};

#endif
