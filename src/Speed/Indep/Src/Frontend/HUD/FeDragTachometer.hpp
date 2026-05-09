#ifndef FRONTEND_HUD_FEDRAGTACHOMETER_H
#define FRONTEND_HUD_FEDRAGTACHOMETER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Frontend/HUD/FeTachometer.hpp"

class DragTachometer : public HudElement, public ITachometer, public ITachometerDrag {
  public:
    DragTachometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetRpm(float rpm) override {
        mRpm = rpm;
    }
    void SetRevLimiter(float redline, float maxRpm) override {
        mRedline = redline;
        mMaxRpm = maxRpm;
    }
    void SetGear(GearID gear, ShiftPotential potential, bool hasGoodEnoughTraction) {
        if (gear != mGear) {
            mGear = gear;
        }
    }
    void SetInPerfectLaunchRange(bool inRange) override {
        mInPerfectLaunchRange = inRange;
    }
    void SetShifting(bool shifting) override {
        mGearShifting = shifting;
    }

  private:
    float CalcAngleForRPMDrag(float rpm, float redline);

    FEImage *TachNeedle;
    FEImage *Needle;
    FEMultiImage *pRedline;
    FEImage *pTachLines;
    FEString *pGearString;
    float mRpm;
    float mRedline;
    float mMaxRpm;
    GearID mGear;
    bool mGearShifting;
    bool mInPerfectLaunchRange;
#ifdef EA_BUILD_A124
    int mOriginalNeedleColour;
#else
    bool mNeedleColourSetToPerfectLaunch;
    float mOriginalNeedleWidth;
    float mOriginalNeedleLeftX;
#endif
};

#endif
