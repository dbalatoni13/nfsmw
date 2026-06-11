#ifndef FESPEEDBREAKERMETER_H
#define FESPEEDBREAKERMETER_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

// File: speed/indep/src/frontend/hud/FeSpeedBreakerMeter.hpp
// total size: 0x50
// Decl: speed/indep/src/frontend/hud/FeSpeedBreakerMeter.hpp:8
class SpeedBreakerMeter : public HudElement, public ISpeedBreakerMeter {
  public:
    SpeedBreakerMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~SpeedBreakerMeter() override {} // Decl: speed/indep/src/frontend/hud/FeSpeedBreakerMeter.hpp:11

    void Update(IPlayer *player) override;
    void SetPursuitLevel(float pursuitLevel) override;

  private:
    float mPursuitLevel;                  // offset 0x30, size 0x4
    bool mPursuitLevelChanged;            // offset 0x34, size 0x1
    FEObject *mpSpeedBreakerMeterIcon;    // offset 0x38, size 0x4
    FEMultiImage *mpSpeedBreakerMeterBar; // offset 0x3C, size 0x4
    FEGroup *mpSpeedBreakerGroup;         // offset 0x40, size 0x4
    FEObject *mpSpeedBreakerBar;          // offset 0x44, size 0x4
    float mSpeedBreakerBarOriginalWidth;  // offset 0x48, size 0x4
};

#endif
