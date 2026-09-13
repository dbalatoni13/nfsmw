#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Frontend/HUD/FeSpeedBreakerMeter.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
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
    if (mpSpeedBreakerBar != nullptr) {
        mSpeedBreakerBarOriginalWidth = mpSpeedBreakerBar->GetObjData()->Size.x;
    }
}

void SpeedBreakerMeter::Update(IPlayer *player) {
    if (!mPursuitLevelChanged) {
        return;
    }

    mPursuitLevelChanged = false;

    if ((mpSpeedBreakerMeterBar != nullptr) && (mpSpeedBreakerBar != nullptr)) {
        float min_angle = 175.0f;
        const float max_angle = 0;
        const float min_pursuit = 0;
        const float max_pursuit = 0;
        const float frac = 0;

        if (bStrICmp(GetPackageName(), "HUD_Drag.fng") == 0) {
            min_angle = -48.0f;
        }

        const float angle = mPursuitLevel;
        FEngSetMultiImageRot(mpSpeedBreakerMeterBar, (angle * -min_angle) + min_angle);

        if (FEngIsScriptSet(mpSpeedBreakerGroup, 0x5b0d9106)) {
            const float originalLeftX = FEngGetTopLeftX(mpSpeedBreakerBar);
            FEngSetSizeX(mpSpeedBreakerBar, mSpeedBreakerBarOriginalWidth * angle);
            FEngSetTopLeftX(mpSpeedBreakerBar, originalLeftX);

            if (angle <= 0.3f) {
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
    if (mpSpeedBreakerMeterIcon != nullptr) {
        if (mPursuitLevel > 0.0f) {
            FEngSetScript(mpSpeedBreakerMeterIcon, 0x77031c70, true);
        } else {
            FEngSetScript(mpSpeedBreakerMeterIcon, FEHASH_INIT, true);
        }
    }
}

void SpeedBreakerMeter::SetPursuitLevel(float pursuitLevel) {
    if (mPursuitLevel == pursuitLevel) {
        return;
    }
    mPursuitLevel = pursuitLevel;
    mPursuitLevelChanged = true;
}
