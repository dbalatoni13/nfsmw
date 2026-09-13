#include "Speed/Indep/Src/Frontend/HUD/FeRadarDetector.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"

float TWK_RadarDetectorMinThreshold;

float RadarDetector::mStaticRange;

RadarDetector::RadarDetector(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x200000), //
      IRadarDetector(pOutter),        //
      mRange(0.0f),                   //
      mDirection(0.0f),               //
      mTargetType(RADAR_TARGET_NONE), //
      mCurrLedAmountShowing(0.3f),    //
      mInPursuit(false),              //
      mIsCoolingDown(false),          //
      mTimeCycleStarted() {
    mpDataRadarDetectorGroup = RegisterGroup(0x062743f5);
    mpDataRadarDetectorLightsLeft = FEngFindObject(pkg_name, 0x69aa01e7);
    mpDataRadarDetectorLightsRight = FEngFindObject(pkg_name, 0x9f59065a);
    mpDataRadarDetectorArrow = FEngFindObject(pkg_name, FEHashUpper("Radar_DirectionArrow"));
    mpDataRadarIcon = FEngFindObject(pkg_name, FEHashUpper("Radar_Icon"));
    mpDataRadarDetectorBacking = RegisterObject(0x839e7d77);
    mpDataRadarDetectorBackingWithMirror = RegisterObject(0x9ee06631);
}

void RadarDetector::Update(IPlayer *player) {
    if (eGetCurrentViewMode() == EVIEWMODE_ONE_RVM && FEDatabase->GetGameplaySettings()->RearviewOn) {
        if (!FEngIsScriptSet(mpDataRadarDetectorBacking, FEHASH_HIDE)) {
            FEngSetScript(mpDataRadarDetectorBacking, FEHASH_HIDE, true);
        }

        if (!mInPursuit || mIsCoolingDown) {
            if (!FEngIsScriptSet(mpDataRadarDetectorGroup, FEHASH_INIT)) {
                FEngSetScript(mpDataRadarDetectorGroup, FEHASH_INIT, true);
            }
        } else {
            if (!FEngIsScriptSet(mpDataRadarDetectorGroup, FEHASH_HIDE)) {
                FEngSetScript(mpDataRadarDetectorGroup, FEHASH_HIDE, true);
            }
        }

        if (!FEngIsScriptSet(mpDataRadarDetectorBackingWithMirror, FEHASH_APPEAR)) {
            FEngSetScript(mpDataRadarDetectorBackingWithMirror, FEHASH_APPEAR, true);
        }
    } else {
        if (!FEngIsScriptSet(mpDataRadarDetectorBackingWithMirror, FEHASH_HIDE)) {
            FEngSetScript(mpDataRadarDetectorBackingWithMirror, FEHASH_HIDE, true);
        }

        if (!mInPursuit || mIsCoolingDown) {
            if (!FEngIsScriptSet(mpDataRadarDetectorBacking, FEHASH_APPEAR)) {
                FEngSetScript(mpDataRadarDetectorBacking, FEHASH_APPEAR, true);
            }
            if (!FEngIsScriptSet(mpDataRadarDetectorGroup, FEHASH_APPEAR)) {
                FEngSetScript(mpDataRadarDetectorGroup, FEHASH_APPEAR, true);
            }
        } else {
            if (FEngIsScriptSet(mpDataRadarDetectorBacking, FEHASH_INIT)) {
                FEngSetScript(mpDataRadarDetectorBacking, FEHASH_HIDE, true);
            } else if (!FEngIsScriptSet(mpDataRadarDetectorBacking, 0x033113ac)) {
                if (!FEngIsScriptSet(mpDataRadarDetectorBacking, FEHASH_HIDE)) {
                    FEngSetScript(mpDataRadarDetectorBacking, 0x033113ac, true);
                }
            }

            if (FEngIsScriptSet(mpDataRadarDetectorGroup, FEHASH_INIT)) {
                FEngSetScript(mpDataRadarDetectorGroup, FEHASH_HIDE, true);
            } else if (!FEngIsScriptSet(mpDataRadarDetectorGroup, 0x033113ac)) {
                if (!FEngIsScriptSet(mpDataRadarDetectorBacking, FEHASH_HIDE)) {
                    FEngSetScript(mpDataRadarDetectorGroup, 0x033113ac, true);
                }
            }
        }
    }

    if (mRange > 0.0f && !(mInPursuit && !mIsCoolingDown)) {
        float range;
        const float max_range = TWK_RadarDetectorMinThreshold;
        if (mRange > TWK_RadarDetectorMinThreshold) {
            range = mRange;
        } else {
            range = TWK_RadarDetectorMinThreshold;
        }

        if (!mTimeCycleStarted.IsSet()) {
            mTimeCycleStarted = WorldTimer;
        }

        mCurrLedAmountShowing += 0.1f;
        if (mCurrLedAmountShowing > 1.0f) {
            mCurrLedAmountShowing = 1.0f;
        }

        if ((WorldTimer - mTimeCycleStarted).GetSeconds() > range * 1.5f) {
            mTimeCycleStarted = WorldTimer;
            mCurrLedAmountShowing = 0.3f;
            MMiscSound(0).Send(UCrc32("Snd"));
        }

        FEVector2 ledUVs(mCurrLedAmountShowing, 1.0f);
        FEngSetMultiImageBottomRightUVs(static_cast<FEMultiImage *>(mpDataRadarDetectorLightsLeft), ledUVs, 0);
        FEngSetMultiImageBottomRightUVs(static_cast<FEMultiImage *>(mpDataRadarDetectorLightsRight), ledUVs, 0);

        FEngSetRotationZ(mpDataRadarDetectorArrow, RAD2DEG(mDirection));

        if (mTargetType == RADAR_TARGET_CAMERA) {
            if (!FEngIsScriptSet(mpDataRadarDetectorArrow, 0xfa44e387)) {
                FEngSetScript(mpDataRadarDetectorArrow, 0xfa44e387, true);
            }
            if (!FEngIsScriptSet(mpDataRadarIcon, 0xfa44e387)) {
                FEngSetScript(mpDataRadarIcon, 0xfa44e387, true);
            }
            if (!FEngIsScriptSet(mpDataRadarDetectorLightsLeft, 0xfa44e387)) {
                FEngSetScript(mpDataRadarDetectorLightsLeft, 0xfa44e387, true);
            }
            if (FEngIsScriptSet(mpDataRadarDetectorLightsRight, 0xfa44e387)) {
                return;
            }
            FEngSetScript(mpDataRadarDetectorLightsRight, 0xfa44e387, true);
        } else {
            if (!FEngIsScriptSet(mpDataRadarDetectorArrow, FEHASH_INIT)) {
                FEngSetScript(mpDataRadarDetectorArrow, FEHASH_INIT, true);
            }
            if (!FEngIsScriptSet(mpDataRadarIcon, FEHASH_INIT)) {
                FEngSetScript(mpDataRadarIcon, FEHASH_INIT, true);
            }
            if (!FEngIsScriptSet(mpDataRadarDetectorLightsLeft, FEHASH_INIT)) {
                FEngSetScript(mpDataRadarDetectorLightsLeft, FEHASH_INIT, true);
            }
            if (FEngIsScriptSet(mpDataRadarDetectorLightsRight, FEHASH_INIT)) {
                return;
            }
            FEngSetScript(mpDataRadarDetectorLightsRight, FEHASH_INIT, true);
        }
        return;
    } else {
        if (mTimeCycleStarted.IsSet()) {
            mTimeCycleStarted.UnSet();
        }
        FEVector2 ledUVs(0.0f, 1.0f);
        FEngSetMultiImageBottomRightUVs(static_cast<FEMultiImage *>(mpDataRadarDetectorLightsLeft), ledUVs, 0);
        FEngSetMultiImageBottomRightUVs(static_cast<FEMultiImage *>(mpDataRadarDetectorLightsRight), ledUVs, 0);
    }

    if (!FEngIsScriptSet(mpDataRadarDetectorArrow, FEHASH_HIDE)) {
        FEngSetScript(mpDataRadarDetectorArrow, FEHASH_HIDE, true);
    }
    if (!FEngIsScriptSet(mpDataRadarIcon, FEHASH_INIT)) {
        FEngSetScript(mpDataRadarIcon, FEHASH_INIT, true);
    }
}
