#include "Speed/Indep/Src/Frontend/HUD/FeTachometer.hpp"

#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

static float CalcAngleForRPM(float rpm, float redline) {
    float factor = rpm / FEngHud::ChooseMaxRpmTextureNumber(redline);
    if (factor < 0.0f) {
        factor = 0.0f;
    }
    if (factor > 1.0f) {
        factor = 1.0f;
    }
    float min_angle = 66.0f;
    float max_angle = 360.0f;
    float fRange = 294.0f - min_angle;
    float angle = factor * fRange + min_angle;
    if (angle > max_angle) {
        angle = angle - max_angle;
    }
    if (angle < 0.0f) {
        angle = max_angle - angle;
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
    RegisterGroup(0x045E9562);
    PerfectShiftDetectedTimer.ResetLow();
    MissedShiftTimer.ResetLow();
    mOriginalNeedleWidth = TachNeedle->GetObjData()->Size.x;
}

void Tachometer::Update(IPlayer *player) {
    if (Sim::GetUserMode() == 1) {
        float originalLeftX = FEngGetTopLeftX(TachNeedle);
        float normalizedRev = mRpm / mMaxRpm;
        FEngSetSizeX(TachNeedle, normalizedRev * mOriginalNeedleWidth);
        FEngSetTopLeftX(TachNeedle, originalLeftX);
        if (mRpm >= mRedline) {
            FEngSetScript(TachNeedle, FEHASH_REDLINE, true);
        } else {
            FEngSetScript(TachNeedle, FEHASH_INIT, true);
        }
    } else {
        FEngSetRotationZ(TachNeedle, CalcAngleForRPM(mRpm, mMaxRpm));
    }

    if (pGearString) {
        FEPrintf(pGearString, "%c", GetLetterForGear(mGear));

        if (Sim::GetUserMode() != 1) {
            const FEColor colourGearNormal(0xFF000000);
            const FEColor colourGearChanging(0x88000000);

            if (mIsShifting) {
                FEngSetColor(pGearString, static_cast<u32>(colourGearChanging));
            } else {
                FEngSetColor(pGearString, static_cast<u32>(colourGearNormal));
            }
        }
    }

    if (mShiftPotential > 1) {
        if (!FEngIsScriptSet(pShiftIndicator, FEHASH_GREEN)) {
            FEngSetScript(pShiftIndicator, FEHASH_GREEN, true);
        }
    } else {
        if (!FEngIsScriptSet(pShiftIndicator, FEHASH_INIT)) {
            FEngSetScript(pShiftIndicator, FEHASH_INIT, true);
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
