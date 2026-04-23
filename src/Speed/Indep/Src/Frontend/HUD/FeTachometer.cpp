#include "Speed/Indep/Src/Frontend/HUD/FeTachometer.hpp"

#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

void FEngSetMultiImageRot(FEMultiImage *image, float angle_degrees);
void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
unsigned long FEHashUpper(const char *name);
int FEPrintf(FEString *text, const char *fmt, ...);
void FEngSetVisible(FEObject *obj);
void FEngSetInvisible(FEObject *obj);
void FEngSetRotationZ(FEObject *obj, float angle);
void FEngGetTopLeft(FEObject *obj, float &x, float &y);
void FEngGetSize(FEObject *obj, float &w, float &h);
void FEngSetSize(FEObject *obj, float w, float h);
void FEngSetTopLeft(FEObject *obj, float x, float y);
void FEngSetColor(FEObject *obj, unsigned int color);
FEColor FEngGetObjectColor(FEObject *obj);
int bStrICmp(const char *s1, const char *s2);

extern const char lbl_803E4D20[];
extern const char lbl_803E4EB8[];
extern const char lbl_803E4EC8[];
extern const char lbl_803E4ED8[];
extern const char lbl_803E4EE4[];
extern const char lbl_803E4EF0[];
extern const char lbl_803E4F00[];
extern const float lbl_803E4F10;
extern const char lbl_803E4F14[];

float ChooseMaxRpmTextureNumber(float redline);

extern const float lbl_803E4EA4;
extern const float lbl_803E4EA8;
extern const float lbl_803E4EAC;
extern const float lbl_803E4EB0;
extern const float lbl_803E4EB4;

static float CalcAngleForRPM(float rpm, float redline) {
    float factor = rpm / ChooseMaxRpmTextureNumber(redline);
    if (factor < lbl_803E4EA4) {
        factor = lbl_803E4EA4;
    }
    if (factor > lbl_803E4EA8) {
        factor = lbl_803E4EA8;
    }
    float min_angle = lbl_803E4EAC;
    float fRange = lbl_803E4EB0 - lbl_803E4EAC;
    float max_angle = lbl_803E4EB4;
    float angle = factor * fRange + min_angle;
    if (angle > max_angle) {
        angle = angle - max_angle;
    }
    if (angle < lbl_803E4EA4) {
        angle = max_angle - angle;
    }
    return angle;
}

Tachometer::Tachometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 2) //
    , ITachometer(pOutter) //
    , mRpm(lbl_803E4F10) //
    , mRedline(lbl_803E4F10) //
    , mMaxRpm(lbl_803E4F10) //
    , mGear(G_NEUTRAL) //
    , mIsShifting(false) //
    , mInPerfectLaunchRange(false) //
    , mShiftPotential(SHIFT_POTENTIAL_NONE) //
    , mNeedleColourSetToPerfectLaunch(false) //
{
    RegisterGroup(FEHashUpper(lbl_803E4EB8));
    TachNeedle = FEngFindObject(pkg_name, FEHashUpper(lbl_803E4EC8));
    pRedline = FEngFindObject(pkg_name, FEHashUpper(lbl_803E4ED8));
    pShiftIndicator = FEngFindObject(pkg_name, FEHashUpper(lbl_803E4EE4));
    pRPM_bar = FEngFindObject(pkg_name, FEHashUpper(lbl_803E4EF0));
    pGearString = static_cast< FEString * >(FEngFindObject(pkg_name, FEHashUpper(lbl_803E4F00)));
    RegisterGroup(0x045E9562);
    PerfectShiftDetectedTimer.ResetLow();
    MissedShiftTimer.ResetLow();
    mOriginalNeedleWidth = TachNeedle->GetObjData()->Size.x;
}

ITachometer::~ITachometer() {}

void Tachometer::Update(IPlayer *player) {
    if (Sim::GetUserMode() == 1) {
        float topX, topY;
        FEngGetTopLeft(TachNeedle, topX, topY);
        float needleWidth = mRpm / mMaxRpm;
        float sizeW, sizeH;
        needleWidth = needleWidth * mOriginalNeedleWidth;
        FEngGetSize(TachNeedle, sizeW, sizeH);
        FEngSetSize(TachNeedle, needleWidth, sizeH);
        float topX2, topY2;
        FEngGetTopLeft(TachNeedle, topX2, topY2);
        FEngSetTopLeft(TachNeedle, topX, topY2);
        if (mRpm >= mRedline) {
            FEngSetScript(TachNeedle, 0x61D30442, true);
        } else {
            FEngSetScript(TachNeedle, 0x001744B3, true);
        }
    } else {
        FEngSetRotationZ(TachNeedle, CalcAngleForRPM(mRpm, mMaxRpm));
    }

    if (pGearString) {
        FEPrintf(pGearString, lbl_803E4F14, GetLetterForGear(mGear));

        if (Sim::GetUserMode() != 1) {
            FEColor normalColor(0xFF000000);
            FEColor redColor(0x88000000);

            if (mIsShifting) {
                FEngSetColor(pGearString, static_cast<unsigned long>(redColor));
            } else {
                FEngSetColor(pGearString, static_cast<unsigned long>(normalColor));
            }
        }
    }

    if (mShiftPotential > 1) {
        if (!FEngIsScriptSet(pShiftIndicator, 0x02DDC8F0)) {
            FEngSetScript(pShiftIndicator, 0x02DDC8F0, true);
        }
    } else {
        if (!FEngIsScriptSet(pShiftIndicator, 0x001744B3)) {
            FEngSetScript(pShiftIndicator, 0x001744B3, true);
        }
    }

    if (mInPerfectLaunchRange) {
        if (!mNeedleColourSetToPerfectLaunch) {
            mNeedleColourSetToPerfectLaunch = true;
            FEColor col = FEngGetObjectColor(TachNeedle);
            FEngSetColor(TachNeedle, (~static_cast<unsigned long>(col)) | 0xFF000000);
        }
    } else {
        if (mNeedleColourSetToPerfectLaunch) {
            mNeedleColourSetToPerfectLaunch = false;
            FEColor col = FEngGetObjectColor(TachNeedle);
            FEngSetColor(TachNeedle, (~static_cast<unsigned long>(col)) | 0xFF000000);
        }
    }
}

void Tachometer::SetRpm(float rpm) {
    mRpm = rpm;
}

void Tachometer::SetShifting(bool shifting) {
    mIsShifting = shifting;
}

void Tachometer::SetInPerfectLaunchRange(bool inRange) {
    mInPerfectLaunchRange = inRange;
}

char Tachometer::GetLetterForGear(GearID gear) {
    if (gear == G_FIRST) {
        return '1';
    }
    if (gear == G_SECOND) {
        return '2';
    }
    if (gear == G_THIRD) {
        return '3';
    }
    if (gear == G_FOURTH) {
        return '4';
    }
    if (gear == G_FIFTH) {
        return '5';
    }
    if (gear == G_SIXTH) {
        return '6';
    }
    if (gear == G_SEVENTH) {
        return '7';
    }
    if (gear == G_EIGHTH) {
        return '8';
    }
    if (gear == G_REVERSE) {
        return 'R';
    }
    return 'N';
}

void Tachometer::SetGear(GearID gear, ShiftPotential potential, bool hasGoodEnoughTraction) {
    if (gear != mGear) {
        mGear = gear;
        mShiftPotential = static_cast<ShiftPotential>(0);
        return;
    }
    if (hasGoodEnoughTraction) {
        mShiftPotential = potential;
        return;
    }
    mShiftPotential = static_cast<ShiftPotential>(0);
}
