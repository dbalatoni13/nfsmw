#ifndef FRONTEND_HUD_FETACHOMETER_H
#define FRONTEND_HUD_FETACHOMETER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

struct FEMultiImage;

class ITachometer : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ITachometer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual void SetRpm(float rpm);
    virtual void SetRevLimiter(float redline, float maxRpm);
    virtual void SetGear(GearID gear, ShiftPotential potential, bool hasGoodEnoughTraction);
    virtual void SetShifting(bool shifting);
    virtual void SetInPerfectLaunchRange(bool inRange);

  protected:
    virtual ~ITachometer();
};

// total size: 0x70
class Tachometer : public HudElement, public ITachometer {
  public:
    Tachometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetRpm(float rpm) override;
    void SetRevLimiter(float redline, float maxRpm) override {
        mRedline = redline;
        mMaxRpm = maxRpm;
    }
    void SetShifting(bool shifting) override;
    void SetInPerfectLaunchRange(bool inRange) override;
    void SetGear(GearID gear, ShiftPotential potential, bool hasGoodEnoughTraction) override;

    static char GetLetterForGear(GearID gear);

  private:
    Timer PerfectShiftDetectedTimer;           // offset 0x30
    Timer MissedShiftTimer;                    // offset 0x34
    FEObject *TachNeedle;                      // offset 0x38
    FEObject *pRPM_bar;                        // offset 0x3C
    FEString *pGearString;                     // offset 0x40
    FEObject *pShiftIndicator;                 // offset 0x44
    FEObject *pRedline;                        // offset 0x48
    float mRpm;                                // offset 0x4C
    float mRedline;                            // offset 0x50
    float mMaxRpm;                             // offset 0x54
    GearID mGear;                              // offset 0x58
    bool mIsShifting;                           // offset 0x5C
    bool mInPerfectLaunchRange;                 // offset 0x60
    ShiftPotential mShiftPotential;             // offset 0x64
    bool mNeedleColourSetToPerfectLaunch;       // offset 0x68
    float mOriginalNeedleWidth;                 // offset 0x6C
};

#endif
