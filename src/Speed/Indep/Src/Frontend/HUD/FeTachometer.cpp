#include "Speed/Indep/Src/Frontend/HUD/FeTachometer.hpp"

#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
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
int bStrICmp(const char *s1, const char *s2);

extern const char lbl_803E4D20[];
extern const char lbl_803E4EB8[];
extern const char lbl_803E4ED0[];
extern const char lbl_803E4EDC[];
extern const char lbl_803E4EEC[];
extern const char lbl_803E4EF8[];
extern const char lbl_803E4F04[];
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
    , PerfectShiftDetectedTimer(0) //
    , MissedShiftTimer(0) //
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
    TachNeedle = FEngFindObject(pkg_name, FEHashUpper(lbl_803E4ED0));
    pRPM_bar = FEngFindObject(pkg_name, FEHashUpper(lbl_803E4EDC));
    pGearString = static_cast< FEString * >(FEngFindObject(pkg_name, FEHashUpper(lbl_803E4EEC)));
    pShiftIndicator = FEngFindObject(pkg_name, FEHashUpper(lbl_803E4EF8));
    pRedline = FEngFindObject(pkg_name, FEHashUpper(lbl_803E4F04));
    if (TachNeedle != nullptr) {
        float x, y;
        FEngGetSize(TachNeedle, x, y);
        mOriginalNeedleWidth = x;
    }
}

void Tachometer::Update(IPlayer *player) {}

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
    switch (gear) {
    case G_REVERSE:
        return 'R';
    case G_FIRST:
        return '1';
    case G_SECOND:
        return '2';
    case G_THIRD:
        return '3';
    case G_FOURTH:
        return '4';
    case G_FIFTH:
        return '5';
    case G_SIXTH:
        return '6';
    case G_SEVENTH:
        return '7';
    case G_EIGHTH:
        return '8';
    default:
        return 'N';
    }
}
