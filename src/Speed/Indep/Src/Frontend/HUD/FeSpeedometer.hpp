#ifndef FRONTEND_HUD_FESPEEDOMETER_H
#define FRONTEND_HUD_FESPEEDOMETER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

class ISpeedometer : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ISpeedometer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual void SetSpeed(float speed);

  protected:
    virtual ~ISpeedometer() {}
};

// total size: 0x48
class Speedometer : public HudElement, public ISpeedometer {
  public:
    Speedometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetSpeed(float speed) override;

  private:
    FEString *mpSpeedDigit1;                   // offset 0x30
    FEString *mpSpeedDigit2;                   // offset 0x34
    FEString *mpSpeedDigit3;                   // offset 0x38
    FEString *SpeedUnits;                      // offset 0x3C
    float mSpeed;                              // offset 0x40
};

#endif
