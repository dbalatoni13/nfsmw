#include "Speed/Indep/Src/Frontend/HUD/FeHeatMeter.hpp"

#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

void FEngSetMultiImageRot(FEMultiImage *image, float angle_degrees);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
int FEPrintf(const char *pkg_name, int obj_hash, const char *fmt, ...);

HeatMeter::HeatMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x40004000ULL), //
      IHeatMeter(pOutter),                 //
      mHeatChanged(true),                  //
      mPursuitHeat(0.0f),          //
      mVehicleHeat(0.0f)           //
{
    RegisterGroup(0xC46A80A9);
    mpDataHeatMultiplier = FEngFindObject(GetPackageName(), 0x7F91DA62);
    mpDataHeatMeterIcon = FEngFindObject(GetPackageName(), 0x6F85ED55);
    mpHeatMeterBar = RegisterMultiImage(0x862824C9);
    mpHeatMeterBar2 = RegisterMultiImage(0x4B2CBE1B);
}

void HeatMeter::Update(IPlayer *player) {
    mHeatChanged = false;
    float heatToUse = mVehicleHeat;
    if (mPursuitHeat > 0.0f) {
        heatToUse = mPursuitHeat;
    }

    const int heatIntegerPart = static_cast<int>(heatToUse);
    const float heatDecimalPart = heatToUse - static_cast<float>(heatIntegerPart);

    {
        float heatDecimalPartToUse = heatDecimalPart;
        if (heatDecimalPart > 0.5f) {
            heatDecimalPartToUse = 0.5f;
        }
        const float min_angle = 175.0f;
        const float max_angle = -175.0f;
        FEngSetMultiImageRot(mpHeatMeterBar, (heatDecimalPartToUse + heatDecimalPartToUse) * -175.0f + 175.0f);
    }

    {
        float heatDecimalPartToUse;
        if (heatDecimalPart > 0.5f) {
            heatDecimalPartToUse = heatDecimalPart - 0.5f;
        } else {
            heatDecimalPartToUse = 0.0f;
        }
        const float min_angle = 175.0f;
        const float max_angle = -175.0f;
        FEngSetMultiImageRot(mpHeatMeterBar2, (heatDecimalPartToUse + heatDecimalPartToUse) * -175.0f + 175.0f);
    }

    if (heatToUse >= 1.0f) {
        if (heatDecimalPart < 0.5f) {
            if (!FEngIsScriptSet(mpDataHeatMultiplier, FEHASH_LEVEL_CHANGE)) {
                FEngSetScript(mpDataHeatMultiplier, FEHASH_LEVEL_CHANGE, true);
            }
        } else {
            if (!FEngIsScriptSet(mpDataHeatMultiplier, 0x1744B3)) {
                FEngSetScript(mpDataHeatMultiplier, 0x1744B3, true);
            }
        }
        FEPrintf(GetPackageName(), 0x7F91DA62, "x%d", heatIntegerPart);
        FEngSetVisible(mpDataHeatMultiplier);
    } else {
        FEngSetInvisible(mpDataHeatMultiplier);
    }

    if (heatToUse > 0.0f) {
        if (heatDecimalPart > 0.75f) {
            if (!FEngIsScriptSet(mpDataHeatMeterIcon, FEHASH_WARNING)) {
                FEngSetScript(mpDataHeatMeterIcon, FEHASH_WARNING, true);
            }
        } else {
            if (!FEngIsScriptSet(mpDataHeatMeterIcon, FEHASH_ACTIVATE)) {
                FEngSetScript(mpDataHeatMeterIcon, FEHASH_ACTIVATE, true);
            }
        }
    } else {
        if (!FEngIsScriptSet(mpDataHeatMeterIcon, 0x1744B3)) {
            FEngSetScript(mpDataHeatMeterIcon, 0x1744B3, true);
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
