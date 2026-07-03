#ifndef FESPEEDOMETER_H
#define FESPEEDOMETER_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

// total size: 0x48
class Speedometer : public HudElement, public ISpeedometer {
  public:
    Speedometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~Speedometer() override {}
    void Update(IPlayer *player) override;
    void SetSpeed(float speed) override {
        mSpeed = speed;
    };

  private:
    FEString *mpSpeedDigit1; // offset 0x30, size 0x4
    FEString *mpSpeedDigit2; // offset 0x34, size 0x4
    FEString *mpSpeedDigit3; // offset 0x38, size 0x4
    FEString *SpeedUnits;    // offset 0x3C, size 0x4
    float mSpeed;            // offset 0x40, size 0x4
};

#endif
