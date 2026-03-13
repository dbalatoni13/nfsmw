#ifndef FRONTEND_HUD_FEDRAGTACHOMETER_H
#define FRONTEND_HUD_FEDRAGTACHOMETER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

struct FEMultiImage;

class DragTachometer : public HudElement, public ITachometer, public ITachometerDrag {
  public:
    DragTachometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetRpm(float rpm) override;
    void SetRevLimiter(float redline, float maxRpm) override {
        mRedline = redline;
        mMaxRpm = maxRpm;
    }
    void SetInPerfectLaunchRange(bool inRange) override;
    void SetShifting(bool shifting) override;
    void SetGear(GearID gear, ShiftPotential potential, bool hasGoodEnoughTraction);
    float CalcAngleForRPMDrag(float rpm, float redline);

  private:
    FEImage * TachNeedle;
    FEImage * Needle;
    FEMultiImage * pRedline;
    FEImage * pTachLines;
    FEString * pGearString;
    float mRpm;
    float mRedline;
    float mMaxRpm;
    GearID mGear;
    bool mGearShifting;
    bool mInPerfectLaunchRange;
    bool mNeedleColourSetToPerfectLaunch;
    float mOriginalNeedleWidth;
    float mOriginalNeedleLeftX;
};

#endif
