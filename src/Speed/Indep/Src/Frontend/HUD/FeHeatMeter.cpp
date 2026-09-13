#include "Speed/Indep/Src/Frontend/HUD/FeHeatMeter.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

HeatMeter::HeatMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x40004000), //
      IHeatMeter(pOutter),              //
      mHeatChanged(true),               //
      mPursuitHeat(0.0f),               //
      mVehicleHeat(0.0f)                //
{
    RegisterGroup(0xC46A80A9);
    mpDataHeatMultiplier = FEngFindObject(GetPackageName(), 0x7F91DA62);
    mpDataHeatMeterIcon = FEngFindObject(GetPackageName(), 0x6F85ED55);
    mpHeatMeterBar = RegisterMultiImage(0x862824C9);
    mpHeatMeterBar2 = RegisterMultiImage(0x4B2CBE1B);
}

// UNSOLVED
void HeatMeter::Update(IPlayer *player) {
    {
        mHeatChanged = false;
        float heatToUse = mVehicleHeat;
        if (mPursuitHeat > 0.0f) {
            heatToUse = mPursuitHeat;
        }

        const int heatIntegerPart = static_cast<int>(heatToUse);
        const float heatDecimalPart = heatToUse - static_cast<float>(heatIntegerPart);

        {
            float heatDecimalPartToUse = heatDecimalPart > 0.5f ? 0.5f : heatDecimalPart; // = heatDecimalPart;
            const float min_angle = -175.0f;
            const float max_angle = 175.0f;
            const float min_heat = 0;
            const float max_heat = 0;
            const float frac = 0;
            const float angle = 0;

            FEngSetMultiImageRot(mpHeatMeterBar, (heatDecimalPartToUse * 2) * -175.0f + 175.0f);
        }
        {
            float heatDecimalPartToUse = heatDecimalPart > 0.5f ? heatDecimalPart - 0.5f : heatDecimalPart;
            const float min_angle = -175.0f;
            const float max_angle = 175.0f;
            const float min_heat = 0;
            const float max_heat = 0;
            const float frac = 0;
            const float angle = 0;

            FEngSetMultiImageRot(mpHeatMeterBar2, (heatDecimalPartToUse * 2) * -175.0f + 175.0f);
        }

        if (heatToUse >= 1.0f) {
            if (heatDecimalPart < 0.5f) {
                if (!FEngIsScriptSet(mpDataHeatMultiplier, 0x41E1FEDC)) {
                    FEngSetScript(mpDataHeatMultiplier, 0x41E1FEDC, true);
                }
            } else {
                if (!FEngIsScriptSet(mpDataHeatMultiplier, FEHASH_INIT)) {
                    FEngSetScript(mpDataHeatMultiplier, FEHASH_INIT, true);
                }
            }
            FEPrintf(GetPackageName(), 0x7F91DA62, "x%d", heatIntegerPart);
            FEngSetVisible(mpDataHeatMultiplier);
        } else {
            FEngSetInvisible(mpDataHeatMultiplier);
        }

        if (heatToUse > 0.0f) {
            if (heatDecimalPart > 0.75f) {
                if (!FEngIsScriptSet(mpDataHeatMeterIcon, 0xDA600155)) {
                    FEngSetScript(mpDataHeatMeterIcon, 0xDA600155, true);
                }
            } else {
                if (!FEngIsScriptSet(mpDataHeatMeterIcon, 0x77031C70)) {
                    FEngSetScript(mpDataHeatMeterIcon, 0x77031C70, true);
                }
            }
        } else {
            if (!FEngIsScriptSet(mpDataHeatMeterIcon, FEHASH_INIT)) {
                FEngSetScript(mpDataHeatMeterIcon, FEHASH_INIT, true);
            }
        }
    }
}

void HeatMeter::SetVehicleHeat(float heat) {
    if (mVehicleHeat == heat) {
        return;
    }
    mVehicleHeat = heat;
    mHeatChanged = true;
}

void HeatMeter::SetPursuitHeat(float heat) {
    if (mPursuitHeat == heat) {
        return;
    }
    mPursuitHeat = heat;
    mHeatChanged = true;
}
