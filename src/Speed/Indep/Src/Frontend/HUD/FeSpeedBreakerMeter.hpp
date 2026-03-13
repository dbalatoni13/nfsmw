#ifndef FRONTEND_HUD_FESPEEDBREAKERMETER_H
#define FRONTEND_HUD_FESPEEDBREAKERMETER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

struct FEGroup;
struct FEMultiImage;

class ISpeedBreakerMeter : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ISpeedBreakerMeter(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~ISpeedBreakerMeter() {}

  public:
    virtual void SetPursuitLevel(float level);
};

// total size: 0x50
class SpeedBreakerMeter : public HudElement, public ISpeedBreakerMeter {
  public:
    SpeedBreakerMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetPursuitLevel(float level) override;

  private:
    float mPursuitLevel;                       // offset 0x30
    bool mPursuitLevelChanged;                 // offset 0x34
    FEObject *mpSpeedBreakerMeterIcon;         // offset 0x38
    FEMultiImage *mpSpeedBreakerMeterBar;      // offset 0x3C
    FEGroup *mpSpeedBreakerGroup;              // offset 0x40
    FEObject *mpSpeedBreakerBar;               // offset 0x44
    float mSpeedBreakerBarOriginalWidth;       // offset 0x48
};

#endif
