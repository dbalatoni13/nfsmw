#include "Speed/Indep/Src/Frontend/HUD/FeTachometer.hpp"

#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"

static float CalcAngleForRPM(float rpm, float redline) {
    float factor = rpm / FEngHud::ChooseMaxRpmTextureNumber(redline);
    if (factor < 0.0f) {
        factor = 0.0f;
    }
    if (factor > 1.0f) {
        factor = 1.0f;
    }
    float min_angle = 66.0f;
    float max_angle = 294.0f;
    float fRange = max_angle - min_angle;
    float angle = factor * fRange + min_angle;
    if (angle > 360.0f) {
        angle = angle - 360.0f;
    }
    if (angle < 0.0f) {
        angle = 360.0f - angle;
    }
    return angle;
}

Tachometer::Tachometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 2),               //
      ITachometer(pOutter),                  //
      mRpm(0.0f),                            //
      mRedline(0.0f),                        //
      mMaxRpm(0.0f),                         //
      mGear(G_NEUTRAL),                      //
      mIsShifting(false),                    //
      mShiftPotential(SHIFT_POTENTIAL_NONE), //
      mNeedleColourSetToPerfectLaunch(false) //
{
    RegisterGroup(FEHashUpper("GaugeCluster"));
    TachNeedle = FEngFindObject(pkg_name, FEHashUpper("3rdPersonNeedle"));
    pRedline = FEngFindObject(pkg_name, FEHashUpper("RPM_REDLINE"));
    pShiftIndicator = FEngFindObject(pkg_name, FEHashUpper("Shift_light"));
    pRPM_bar = FEngFindObject(pkg_name, FEHashUpper("TAC_Lines_7500"));
    pGearString = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper("3rdPersonGear")));

    const u32 FEObj_NOSPersuitMeterGroup = 0x045E9562;
    RegisterGroup(FEObj_NOSPersuitMeterGroup);
    PerfectShiftDetectedTimer.ResetLow();
    MissedShiftTimer.ResetLow();
    mOriginalNeedleWidth = TachNeedle->GetObjData()->Size.x;
}

void Tachometer::Update(IPlayer *player) {
    if (Sim::GetUserMode() == 1) {
        float originalLeftX = FEngGetTopLeftX(TachNeedle);
        float normalizedRev = mRpm / mMaxRpm;
        normalizedRev = normalizedRev * mOriginalNeedleWidth;
        FEngSetSizeX(TachNeedle, normalizedRev);
        FEngSetTopLeftX(TachNeedle, originalLeftX);

        if (mRpm >= mRedline) {
            FEngSetScript(TachNeedle, 0x61D30442, true);
        } else {
            FEngSetScript(TachNeedle, 0x001744B3, true);
        }
    } else {
        FEngSetRotationZ(TachNeedle, CalcAngleForRPM(mRpm, mMaxRpm));
    }

    if (pGearString != nullptr) {
        FEPrintf(pGearString, "%c", GetLetterForGear(mGear));

        if (Sim::GetUserMode() != 1) {
            const FEColor colourGearNormal(0xFF000000);
            const FEColor colourGearChanging(0x88000000);

            if (mIsShifting) {
                FEngSetColor(pGearString, colourGearChanging);
            } else {
                FEngSetColor(pGearString, colourGearNormal);
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
            int originalNeedleColour = FEngGetColor(TachNeedle);
            int oppositeOriginal = ~originalNeedleColour | 0xFF000000;
            FEngSetColor(TachNeedle, oppositeOriginal);
        }
    } else {
        if (mNeedleColourSetToPerfectLaunch) {
            mNeedleColourSetToPerfectLaunch = false;
            int originalNeedleColour = FEngGetColor(TachNeedle);
            int oppositeOriginal = ~originalNeedleColour | 0xFF000000;
            FEngSetColor(TachNeedle, oppositeOriginal);
        }
    }
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
