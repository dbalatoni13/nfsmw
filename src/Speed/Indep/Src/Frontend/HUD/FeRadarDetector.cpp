#include "Speed/Indep/Src/Frontend/HUD/FeRadarDetector.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"

extern bool FEngIsScriptSet(FEObject *, unsigned int);
extern void FEngSetScript(FEObject *, unsigned int, bool);
extern void FEngSetMultiImageBottomRightUVs(FEMultiImage *, FEVector2 &, int);
extern void FEngSetRotationZ(FEObject *, float);
extern float TWK_RadarDetectorMinThreshold;

float RadarDetector::mStaticRange;

RadarDetector::RadarDetector(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x200000) //
    , IRadarDetector(pOutter) //
    , mRange(0.0f) //
    , mDirection(0.0f) //
    , mTargetType(RADAR_TARGET_NONE) //
    , mCurrLedAmountShowing(0.3f) //
    , mInPursuit(false) //
    , mIsCoolingDown(false) //
    , mTimeCycleStarted(0)
{
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
        if (!FEngIsScriptSet(mpDataRadarDetectorBacking, 0x16a259)) {
            FEngSetScript(mpDataRadarDetectorBacking, 0x16a259, true);
        }

        if (!mInPursuit || mIsCoolingDown) {
            if (!FEngIsScriptSet(mpDataRadarDetectorGroup, 0x1744b3)) {
                FEngSetScript(mpDataRadarDetectorGroup, 0x1744b3, true);
            }
        } else {
            if (!FEngIsScriptSet(mpDataRadarDetectorGroup, 0x16a259)) {
                FEngSetScript(mpDataRadarDetectorGroup, 0x16a259, true);
            }
        }

        if (!FEngIsScriptSet(mpDataRadarDetectorBackingWithMirror, 0x5079c8f8)) {
            FEngSetScript(mpDataRadarDetectorBackingWithMirror, 0x5079c8f8, true);
        }
    } else {
        if (!FEngIsScriptSet(mpDataRadarDetectorBackingWithMirror, 0x16a259)) {
            FEngSetScript(mpDataRadarDetectorBackingWithMirror, 0x16a259, true);
        }

        if (!mInPursuit || mIsCoolingDown) {
            if (!FEngIsScriptSet(mpDataRadarDetectorBacking, 0x5079c8f8)) {
                FEngSetScript(mpDataRadarDetectorBacking, 0x5079c8f8, true);
            }
            if (!FEngIsScriptSet(mpDataRadarDetectorGroup, 0x5079c8f8)) {
                FEngSetScript(mpDataRadarDetectorGroup, 0x5079c8f8, true);
            }
        } else {
            if (FEngIsScriptSet(mpDataRadarDetectorBacking, 0x1744b3)) {
                FEngSetScript(mpDataRadarDetectorBacking, 0x16a259, true);
            } else if (!FEngIsScriptSet(mpDataRadarDetectorBacking, 0x033113ac)) {
                if (!FEngIsScriptSet(mpDataRadarDetectorBacking, 0x16a259)) {
                    FEngSetScript(mpDataRadarDetectorBacking, 0x033113ac, true);
                }
            }

            if (FEngIsScriptSet(mpDataRadarDetectorGroup, 0x1744b3)) {
                FEngSetScript(mpDataRadarDetectorGroup, 0x16a259, true);
            } else if (!FEngIsScriptSet(mpDataRadarDetectorGroup, 0x033113ac)) {
                if (!FEngIsScriptSet(mpDataRadarDetectorBacking, 0x16a259)) {
                    FEngSetScript(mpDataRadarDetectorGroup, 0x033113ac, true);
                }
            }
        }
    }

    if (mRange > 0.0f) {
        if (mInPursuit && !mIsCoolingDown) {
            goto low_range;
        }

        const float max_range = TWK_RadarDetectorMinThreshold;
        float range;
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
            MMiscSound sound(0);
            sound.Send(UCrc32("Snd"));
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
            if (!FEngIsScriptSet(mpDataRadarDetectorArrow, 0x1744b3)) {
                FEngSetScript(mpDataRadarDetectorArrow, 0x1744b3, true);
            }
            if (!FEngIsScriptSet(mpDataRadarIcon, 0x1744b3)) {
                FEngSetScript(mpDataRadarIcon, 0x1744b3, true);
            }
            if (!FEngIsScriptSet(mpDataRadarDetectorLightsLeft, 0x1744b3)) {
                FEngSetScript(mpDataRadarDetectorLightsLeft, 0x1744b3, true);
            }
            if (FEngIsScriptSet(mpDataRadarDetectorLightsRight, 0x1744b3)) {
                return;
            }
            FEngSetScript(mpDataRadarDetectorLightsRight, 0x1744b3, true);
        }
        return;
    }

low_range:
    {
        if (mTimeCycleStarted.IsSet()) {
            mTimeCycleStarted.UnSet();
        }
        FEVector2 ledUVs(0.0f, 1.0f);
        FEngSetMultiImageBottomRightUVs(static_cast<FEMultiImage *>(mpDataRadarDetectorLightsLeft), ledUVs, 0);
        FEngSetMultiImageBottomRightUVs(static_cast<FEMultiImage *>(mpDataRadarDetectorLightsRight), ledUVs, 0);
    }

    if (!FEngIsScriptSet(mpDataRadarDetectorArrow, 0x16a259)) {
        FEngSetScript(mpDataRadarDetectorArrow, 0x16a259, true);
    }
    if (!FEngIsScriptSet(mpDataRadarIcon, 0x1744b3)) {
        FEngSetScript(mpDataRadarIcon, 0x1744b3, true);
    }
}

void RadarDetector::SetInPursuit(bool inPursuit) {
    mInPursuit = inPursuit;
}

void RadarDetector::SetIsCoolingDown(bool coolingDown) {
    mIsCoolingDown = coolingDown;
}

void RadarDetector::SetTarget(RadarTarget targetType, float range, float direction) {
    mTargetType = targetType;
    mDirection = direction;
    mRange = range;
    mStaticRange = range;
}
