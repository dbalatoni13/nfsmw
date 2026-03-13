#ifndef FRONTEND_HUD_FERADARDETECTOR_H
#define FRONTEND_HUD_FERADARDETECTOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

class RadarDetector : public HudElement, public IRadarDetector {
  public:
    RadarDetector(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetInPursuit(bool inPursuit) override;
    void SetIsCoolingDown(bool coolingDown) override;
    void SetTarget(RadarTarget targetType, float range, float direction) override;

  private:
    FEGroup * mpDataRadarDetectorGroup;
    FEObject * mpDataRadarDetectorLightsLeft;
    FEObject * mpDataRadarDetectorLightsRight;
    FEObject * mpDataRadarDetectorArrow;
    FEObject * mpDataRadarIcon;
    FEObject * mpDataRadarDetectorBacking;
    FEObject * mpDataRadarDetectorBackingWithMirror;
    float mRange;
    float mDirection;
    RadarTarget mTargetType;
    float mCurrLedAmountShowing;
    bool mInPursuit;
    bool mIsCoolingDown;
    Timer mTimeCycleStarted;

    static float mStaticRange;
};

#endif
