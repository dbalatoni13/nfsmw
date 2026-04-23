#ifndef FRONTEND_HUD_FEHEATMETER_H
#define FRONTEND_HUD_FEHEATMETER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

struct FEMultiImage;

class IHeatMeter : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IHeatMeter(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~IHeatMeter() {}

  public:
    virtual void SetVehicleHeat(float heat);
    virtual void SetPursuitHeat(float heat);
};

// total size: 0x50
class HeatMeter : public HudElement, public IHeatMeter {
  public:
    HeatMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetVehicleHeat(float heat) override;
    void SetPursuitHeat(float heat) override;

  private:
    bool mHeatChanged;                        // offset 0x30
    float mPursuitHeat;                       // offset 0x34
    float mVehicleHeat;                       // offset 0x38
    FEObject *mpDataHeatMultiplier;           // offset 0x3C
    FEObject *mpDataHeatMeterIcon;            // offset 0x40
    FEObject *mpDataHeatMeterIconWarning;     // offset 0x44
    FEMultiImage *mpHeatMeterBar;             // offset 0x48
    FEMultiImage *mpHeatMeterBar2;            // offset 0x4C
};

#endif
