#ifndef FEHEATMETER_H
#define FEHEATMETER_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

// File: speed/indep/src/frontend/hud/FeHeatMeter.hpp
// total size: 0x50
// Decl: speed/indep/src/frontend/hud/FeHeatMeter.hpp:9
struct HeatMeter : public HudElement, public IHeatMeter {
    HeatMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~HeatMeter() override {} // Decl: speed/indep/src/frontend/hud/FeHeatMeter.hpp:12

    void Update(IPlayer *player) override;

    void SetVehicleHeat(float heat) override;

    void SetPursuitHeat(float heat) override;

  private:
    bool mHeatChanged;                    // offset 0x30, size 0x1
    float mPursuitHeat;                   // offset 0x34, size 0x4
    float mVehicleHeat;                   // offset 0x38, size 0x4
    FEObject *mpDataHeatMultiplier;       // offset 0x3C, size 0x4, Decl: speed/indep/src/frontend/hud/FeHeatMeter.hpp:22
    FEObject *mpDataHeatMeterIcon;        // offset 0x40, size 0x4, Decl: speed/indep/src/frontend/hud/FeHeatMeter.hpp:23
    FEObject *mpDataHeatMeterIconWarning; // offset 0x44, size 0x4
    FEMultiImage *mpHeatMeterBar;         // offset 0x48, size 0x4, Decl: speed/indep/src/frontend/hud/FeHeatMeter.hpp:21
    FEMultiImage *mpHeatMeterBar2;        // offset 0x4C, size 0x4
};

#endif
