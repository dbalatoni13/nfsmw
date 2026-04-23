#ifndef FRONTEND_HUD_FEENGINETEMPGAUGE_H
#define FRONTEND_HUD_FEENGINETEMPGAUGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

struct FEMultiImage;

class IEngineTempGauge : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IEngineTempGauge(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~IEngineTempGauge() {}

  public:
    virtual void SetEngineTemp(float temp);
};

// total size: 0x40
class EngineTempGauge : public HudElement, public IEngineTempGauge {
  public:
    EngineTempGauge(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetEngineTemp(float temp) override;

  private:
    float mEngineTemp;                         // offset 0x30
    bool mEngineTempChanged;                   // offset 0x34
    FEObject *mpWarningLight;                  // offset 0x38
    FEMultiImage *mpEngineTempGaugeBar;        // offset 0x3C
};

#endif
