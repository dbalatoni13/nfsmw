#include "Speed/Indep/Src/Frontend/HUD/FeTachometerDrag.hpp"
#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeTachometer.hpp"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"

DragTachometer::DragTachometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x2), ITachometer(pOutter), ITachometerDrag(pOutter), mMaxRpm(0.0f), mGear(G_NEUTRAL),
#ifndef EA_BUILD_A124
      mNeedleColourSetToPerfectLaunch(false),
#endif
      mRpm(0.0f), mRedline(0.0f), mGearShifting(false) {
    RegisterImage("RPM_fill");
    RegisterImage("Drag_Turbo_Backing");
    RegisterImage("Drag_Turbo_Lines");
    RegisterImage("3rdPersonSpeedUnits");
    RegisterImage("SPEED_BACKING");
    RegisterGroup(FEHashUpper("RPM"));
    pTachLines = RegisterImage("TAC_Lines_7500");
    pRedline = RegisterMultiImage(FEHashUpper("RPM_Redline"));
    TachNeedle = RegisterImage("3rdPersonNeedle");
    pGearString = RegisterString("3rdPersonGear");
#ifndef EA_BUILD_A124
    mOriginalNeedleWidth = TachNeedle->GetObjData()->Size.x;
    mOriginalNeedleLeftX = FEngGetTopLeftX(TachNeedle);
#endif
}

void DragTachometer::Update(IPlayer *player) {
#ifndef EA_BUILD_A124
    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
        float normalizedRev = mRpm / mMaxRpm;
        FEngSetSizeX(TachNeedle, normalizedRev * mOriginalNeedleWidth);
        FEngSetTopLeftX(TachNeedle, mOriginalNeedleLeftX);
        if (mRpm >= mRedline) {
            FEngSetScript(TachNeedle, 0x61D30442, true);
        } else {
            FEngSetScript(TachNeedle, FEHASH_INIT, true);
        }
    } else
#endif
    {
        FEngSetRotationZ(TachNeedle, CalcAngleForRPMDrag(mRpm, mMaxRpm));
    }

    if (pGearString != nullptr) {
        const FEColor colourGearNormal(0xFFFFFFFF);
        const FEColor colourGearChanging(0x88FFFFFF);
        FEPrintf(pGearString, "%c", Tachometer::GetLetterForGear(mGear));
        if (mGearShifting) {
            FEngSetColor(pGearString, colourGearChanging);
        } else {
            FEngSetColor(pGearString, colourGearNormal);
        }
    }
#ifndef EA_BUILD_A124
    if (mInPerfectLaunchRange) {
        if (!mNeedleColourSetToPerfectLaunch) {
            mNeedleColourSetToPerfectLaunch = true;
            int originalNeedleColour = FEngGetColor(TachNeedle);
            int oppositeOriginal = ~originalNeedleColour | 0xFF000000;
            FEngSetColor(TachNeedle, oppositeOriginal);
        }
    } else if (mNeedleColourSetToPerfectLaunch) {
        mNeedleColourSetToPerfectLaunch = false;
        int originalNeedleColour = FEngGetColor(TachNeedle);
        int oppositeOriginal = ~originalNeedleColour | 0xFF000000;
        FEngSetColor(TachNeedle, oppositeOriginal);
    }
#endif
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
    float angle = factor * fRange + min_angle;
    return angle;
}
