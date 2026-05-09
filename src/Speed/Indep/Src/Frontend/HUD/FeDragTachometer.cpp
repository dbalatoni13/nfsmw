#include "Speed/Indep/Src/Frontend/HUD/FeDragTachometer.hpp"
#include "Speed/Indep/Src/FEng/feimage.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeTachometer.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"

DragTachometer::DragTachometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x2), ITachometer(pOutter), ITachometerDrag(pOutter) {
    mMaxRpm = 0.0f;
    mGear = G_NEUTRAL;
    mNeedleColourSetToPerfectLaunch = false;
    mRpm = 0.0f;
    mRedline = 0.0f;
    mGearShifting = false;
    RegisterImage(FEHashUpper("RPM_fill"));
    RegisterImage(FEHashUpper("Drag_Turbo_Backing"));
    RegisterImage(FEHashUpper("Drag_Turbo_Lines"));
    RegisterImage(FEHashUpper("3rdPersonSpeedUnits"));
    RegisterImage(FEHashUpper("SPEED_BACKING"));
    RegisterGroup(FEHashUpper("RPM"));
    pTachLines = RegisterImage(FEHashUpper("TAC_Lines_7500"));
    pRedline = RegisterMultiImage(FEHashUpper("RPM_Redline"));
    TachNeedle = RegisterImage(FEHashUpper("3rdPersonNeedle"));
    pGearString = RegisterString(FEHashUpper("3rdPersonGear"));
    mOriginalNeedleWidth = TachNeedle->GetObjData()->Size.x;
    mOriginalNeedleLeftX = FEngGetTopLeftX(TachNeedle);
}

void DragTachometer::Update(IPlayer *player) {
    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
        float normalizedRev = mRpm / mMaxRpm;
        float w, h;
        FEngGetSize(TachNeedle, w, h);
        FEngSetSize(TachNeedle, normalizedRev * mOriginalNeedleWidth, h);
        float x, y;
        FEngGetTopLeft(TachNeedle, x, y);
        FEngSetTopLeft(TachNeedle, mOriginalNeedleLeftX, y);
        if (mRpm < mRedline) {
            FEngSetScript(TachNeedle, 0x1744B3, true);
        } else {
            FEngSetScript(TachNeedle, 0x61D30442, true);
        }
    } else {
        FEngSetRotationZ(TachNeedle, CalcAngleForRPMDrag(mRpm, mMaxRpm));
    }

    if (pGearString) {
        const FEColor colourGearNormal(0xFFFFFFFF);
        const FEColor colourGearChanging(0x88FFFFFF);
        FEPrintf(pGearString, "%c", Tachometer::GetLetterForGear(mGear));
        if (!mGearShifting) {
            FEngSetColor(pGearString, static_cast<unsigned int>(colourGearNormal));
        } else {
            FEngSetColor(pGearString, static_cast<unsigned int>(colourGearChanging));
        }
    }

    if (!mInPerfectLaunchRange) {
        if (mNeedleColourSetToPerfectLaunch) {
            mNeedleColourSetToPerfectLaunch = false;
            FEColor originalNeedleColour = FEngGetObjectColor(TachNeedle);
            unsigned int oppositeOriginal = static_cast<unsigned int>(originalNeedleColour);
            FEngSetColor(TachNeedle, ~oppositeOriginal | 0xFF000000);
        }
    } else if (!mNeedleColourSetToPerfectLaunch) {
        mNeedleColourSetToPerfectLaunch = true;
        FEColor originalNeedleColour = FEngGetObjectColor(TachNeedle);
        unsigned int oppositeOriginal = static_cast<unsigned int>(originalNeedleColour);
        FEngSetColor(TachNeedle, ~oppositeOriginal | 0xFF000000);
    }
}

float DragTachometer::CalcAngleForRPMDrag(float rpm, float redline) {
    float factor = rpm / FEngHud::ChooseMaxRpmTextureNumber(redline);
    if (factor < 0.0f) {
        factor = 0.0f;
    }
    if (factor > 1.0f) {
        factor = 1.0f;
    }
    float min_angle = -43.0f;
    float max_angle = 44.5f;
    float fRange = max_angle - min_angle;
    return factor * fRange + min_angle;
}
