#include "Speed/Indep/Src/Frontend/HUD/FeHeatMeter.hpp"

#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEString.h"

void FEngSetMultiImageRot(FEMultiImage *image, float angle_degrees);
void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
void FEngSetVisible(FEObject *obj);
void FEngSetInvisible(FEObject *obj);
int FEPrintf(const char *pkg_name, int obj_hash, const char *fmt, ...);
FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);

extern const float lbl_803E4888;
extern const char lbl_803E488C[];
extern const float lbl_803E4890;
extern const float lbl_803E4898;
extern const float lbl_803E48A0;
extern const float lbl_803E48A4;
extern const float lbl_803E48A8;
extern const float lbl_803E48AC;
extern const float lbl_803E48B0;

HeatMeter::HeatMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x40004000ULL) //
    , IHeatMeter(pOutter) //
    , mHeatChanged(true) //
    , mPursuitHeat(lbl_803E4888) //
    , mVehicleHeat(lbl_803E4888) //
{
    RegisterGroup(0xC46A80A9);
    mpDataHeatMultiplier = FEngFindObject(GetPackageName(), 0x7F91DA62);
    mpDataHeatMeterIcon = FEngFindObject(GetPackageName(), 0x6F85ED55);
    mpHeatMeterBar = RegisterMultiImage(0x862824C9);
    mpHeatMeterBar2 = RegisterMultiImage(0x4B2CBE1B);
}

void HeatMeter::Update(IPlayer *player) {
    mHeatChanged = false;
    float heat = mVehicleHeat;
    if (lbl_803E4890 < mPursuitHeat) {
        heat = mPursuitHeat;
    }

    float half = heat - static_cast< float >(static_cast< int >(heat));
    float clampedHalf = half;
    if (lbl_803E48A0 < half) {
        clampedHalf = lbl_803E48A0;
    }

    FEngSetMultiImageRot(mpHeatMeterBar, (clampedHalf + clampedHalf) * lbl_803E48A8 + lbl_803E48A4);

    float remainder = lbl_803E4890;
    if (lbl_803E48A0 < half) {
        remainder = half - lbl_803E48A0;
    }

    FEngSetMultiImageRot(mpHeatMeterBar2, (remainder + remainder) * lbl_803E48A8 + lbl_803E48A4);

    if (heat < lbl_803E48AC) {
        FEngSetInvisible(mpDataHeatMultiplier);
    } else {
        if (lbl_803E48A0 <= half) {
            if (!FEngIsScriptSet(mpDataHeatMultiplier, 0x1744B3)) {
                FEngSetScript(mpDataHeatMultiplier, 0x1744B3, true);
            }
        } else {
            if (!FEngIsScriptSet(mpDataHeatMultiplier, 0x41E1FEDC)) {
                FEngSetScript(mpDataHeatMultiplier, 0x41E1FEDC, true);
            }
        }
        FEPrintf(pPackageName, 0x7F91DA62, lbl_803E488C, static_cast< int >(heat));
        FEngSetVisible(mpDataHeatMultiplier);
    }

    if (heat <= lbl_803E4890) {
        if (!FEngIsScriptSet(mpDataHeatMeterIcon, 0x1744B3)) {
            FEngSetScript(mpDataHeatMeterIcon, 0x1744B3, true);
        }
    } else {
        if (half <= lbl_803E48B0) {
            if (!FEngIsScriptSet(mpDataHeatMeterIcon, 0x77031C70)) {
                FEngSetScript(mpDataHeatMeterIcon, 0x77031C70, true);
            }
        } else {
            if (!FEngIsScriptSet(mpDataHeatMeterIcon, 0xDA600155)) {
                FEngSetScript(mpDataHeatMeterIcon, 0xDA600155, true);
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
