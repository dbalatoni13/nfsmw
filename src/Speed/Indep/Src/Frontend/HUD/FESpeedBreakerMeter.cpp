#include "Speed/Indep/Src/Frontend/HUD/FeSpeedBreakerMeter.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

SpeedBreakerMeter::SpeedBreakerMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x40000),       //
      ISpeedBreakerMeter(pOutter),         //
      mPursuitLevelChanged(true),          //
      mSpeedBreakerBarOriginalWidth(0.0f), //
      mPursuitLevel(0.0f)                  //
{
    RegisterGroup(FEHashUpper("Speedbreaker_Meter_Group"));
    mpSpeedBreakerMeterIcon = FEngFindObject(GetPackageName(), FEHashUpper("SPEEDBREAKER_METER_ICON_GROUP"));
    mpSpeedBreakerMeterBar = RegisterMultiImage(FEHashUpper("Speedbreaker_Meter_Multi_Image"));
    mpSpeedBreakerGroup = RegisterGroup(0x82D60021);
    mpSpeedBreakerBar = FEngFindObject(GetPackageName(), 0x1FDAF669);
    if (mpSpeedBreakerBar) {
        mSpeedBreakerBarOriginalWidth = mpSpeedBreakerBar->GetObjData()->Size.x;
    }
}

void SpeedBreakerMeter::Update(IPlayer *player) {
    if (!mPursuitLevelChanged) {
        return;
    }

    mPursuitLevelChanged = false;

    if (mpSpeedBreakerMeterBar && mpSpeedBreakerBar) {
        float min_angle = 175.0f;
        if (bStrICmp(GetPackageName(), "HUD_Drag.fng") == 0) {
            min_angle = -48.0f;
        }
        FEngSetMultiImageRot(mpSpeedBreakerMeterBar, mPursuitLevel * -min_angle + min_angle);

        if (FEngIsScriptSet(reinterpret_cast<FEObject *>(mpSpeedBreakerGroup), 0x5b0d9106)) {
            const float originalLeftX = FEngGetTopLeftX(mpSpeedBreakerBar);
            FEngSetSizeX(mpSpeedBreakerBar, mSpeedBreakerBarOriginalWidth * mPursuitLevel);
            FEngSetTopLeftX(mpSpeedBreakerBar, originalLeftX);

            if (mPursuitLevel <= 0.3f) {
                if (!FEngIsScriptSet(this->mpSpeedBreakerBar, 0x26ded57)) {
                    FEngSetScript(this->mpSpeedBreakerBar, 0x26ded57, true);
                }
            } else {
                if (!FEngIsScriptSet(this->mpSpeedBreakerBar, 0x620e4851)) {
                    FEngSetScript(this->mpSpeedBreakerBar, 0x620e4851, true);
                }
            }
        }
    }
    if (mpSpeedBreakerMeterIcon) {
        if (mPursuitLevel > 0.0f) {
            FEngSetScript(mpSpeedBreakerMeterIcon, 0x61D30442, true);
        } else {
            FEngSetScript(mpSpeedBreakerMeterIcon, 0x1744B3, true);
        }
    }
}

void SpeedBreakerMeter::SetPursuitLevel(float level) {
    if (mPursuitLevel == level) {
        return;
    }
    mPursuitLevel = level;
    mPursuitLevelChanged = true;
}
