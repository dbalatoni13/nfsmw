#include "Speed/Indep/Src/Frontend/HUD/FeSpeedBreakerMeter.hpp"

#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

void FEngSetMultiImageRot(FEMultiImage *image, float angle_degrees);
void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
unsigned long FEHashUpper(const char *name);
int bStrICmp(const char *s1, const char *s2);
void FEngGetTopLeft(FEObject *object, float &x, float &y);
void FEngGetSize(FEObject *object, float &w, float &h);
void FEngSetSize(FEObject *object, float w, float h);

extern const char lbl_803E4D20[];
extern const char lbl_803E4D40[];
extern const char lbl_803E4D5C[];
extern const char lbl_803E4D7C[];
extern const float lbl_803E4D9C;
extern const float lbl_803E4DA0;
extern const float lbl_803E4DA4;

SpeedBreakerMeter::SpeedBreakerMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x40000) //
    , ISpeedBreakerMeter(pOutter) //
    , mPursuitLevelChanged(true) //
    , mSpeedBreakerBarOriginalWidth(lbl_803E4D9C) //
    , mPursuitLevel(lbl_803E4D9C) //
{
    RegisterGroup(FEHashUpper(lbl_803E4D40));
    mpSpeedBreakerMeterIcon = FEngFindObject(GetPackageName(), FEHashUpper(lbl_803E4D5C));
    mpSpeedBreakerMeterBar = RegisterMultiImage(FEHashUpper(lbl_803E4D7C));
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

    if (mpSpeedBreakerMeterBar != nullptr && mpSpeedBreakerBar != nullptr) {
        float maxAngle = lbl_803E4DA0;
        if (bStrICmp(pPackageName, lbl_803E4D20) == 0) {
            maxAngle = lbl_803E4DA4;
        }
        FEngSetMultiImageRot(mpSpeedBreakerMeterBar, mPursuitLevel * -maxAngle + maxAngle);

        float topLeftX, topLeftY;
        FEngGetTopLeft(mpSpeedBreakerBar, topLeftX, topLeftY);
        float sizeW, sizeH;
        FEngGetSize(mpSpeedBreakerBar, sizeW, sizeH);
        FEngSetSize(mpSpeedBreakerBar, mPursuitLevel * mSpeedBreakerBarOriginalWidth, sizeH);
    }

    if (mPursuitLevel <= lbl_803E4D9C) {
        if (!FEngIsScriptSet(mpSpeedBreakerMeterIcon, 0x1744B3)) {
            FEngSetScript(mpSpeedBreakerMeterIcon, 0x1744B3, true);
        }
    } else {
        if (!FEngIsScriptSet(mpSpeedBreakerMeterIcon, 0x61D30442)) {
            FEngSetScript(mpSpeedBreakerMeterIcon, 0x61D30442, true);
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
