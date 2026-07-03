#ifndef FETACHOMETER_H
#define FETACHOMETER_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

// File: speed/indep/src/frontend/hud/FeTachometer.hpp
// total size: 0x70
// Decl: speed/indep/src/frontend/hud/FeTachometer.hpp:13
class Tachometer : public HudElement, public ITachometer {
  public:
    Tachometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~Tachometer() override {} // Decl: speed/indep/src/frontend/hud/FeTachometer.hpp:16

    void Update(IPlayer *player) override;
    void SetRpm(float rpm) override {
        mRpm = rpm;
    };
    void SetRevLimiter(float redline, float maxRpm) override {
        mRedline = redline;
        mMaxRpm = maxRpm;
    }
    void SetShifting(bool shifting) override {
        mIsShifting = shifting;
    };
    void SetInPerfectLaunchRange(bool inRange) override {
        mInPerfectLaunchRange = inRange;
    };
    void SetGear(GearID gear, ShiftPotential potential, bool hasGoodEnoughTraction) override {
        if (gear != mGear) {
            mGear = gear;
            mShiftPotential = static_cast<ShiftPotential>(0);
            return;
        }
        if (hasGoodEnoughTraction) {
            mShiftPotential = potential;
            return;
        }
        mShiftPotential = static_cast<ShiftPotential>(0);
    };

    static char GetLetterForGear(GearID gear);

  private:
    Timer PerfectShiftDetectedTimer;      // offset 0x30, size 0x4
    Timer MissedShiftTimer;               // offset 0x34, size 0x4
    FEObject *TachNeedle;                 // offset 0x38, size 0x4
    FEObject *pRPM_bar;                   // offset 0x3C, size 0x4
    FEString *pGearString;                // offset 0x40, size 0x4
    FEObject *pShiftIndicator;            // offset 0x44, size 0x4
    FEObject *pRedline;                   // offset 0x48, size 0x4
    float mRpm;                           // offset 0x4C, size 0x4
    float mRedline;                       // offset 0x50, size 0x4
    float mMaxRpm;                        // offset 0x54, size 0x4
    GearID mGear;                         // offset 0x58, size 0x4
    bool mIsShifting;                     // offset 0x5C, size 0x1
    bool mInPerfectLaunchRange;           // offset 0x60, size 0x1
    ShiftPotential mShiftPotential;       // offset 0x64, size 0x4
    bool mNeedleColourSetToPerfectLaunch; // offset 0x68, size 0x1
    float mOriginalNeedleWidth;           // offset 0x6C, size 0x4
};

#endif
