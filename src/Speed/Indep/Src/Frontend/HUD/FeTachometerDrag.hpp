#ifndef FETACHOMETERDRAG_H
#define FETACHOMETERDRAG_H

// total size: 0x70
#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
class DragTachometer : public HudElement, public ITachometer, public ITachometerDrag {
  public:
    DragTachometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~DragTachometer() override {}

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

    FEImage *TachNeedle;        // offset 0x38, size 0x4
    FEImage *Needle;            // offset 0x3C, size 0x4
    FEMultiImage *pRedline;     // offset 0x40, size 0x4
    FEImage *pTachLines;        // offset 0x44, size 0x4
    FEString *pGearString;      // offset 0x48, size 0x4
    float mRpm;                 // offset 0x4C, size 0x4
    float mRedline;             // offset 0x50, size 0x4
    float mMaxRpm;              // offset 0x54, size 0x4
    GearID mGear;               // offset 0x58, size 0x4
    bool mGearShifting;         // offset 0x5C, size 0x1
    bool mInPerfectLaunchRange; // offset 0x60, size 0x1
#ifdef EA_BUILD_A124
    int mOriginalNeedleColour;
#else
    bool mNeedleColourSetToPerfectLaunch; // offset 0x64, size 0x1
    float mOriginalNeedleWidth;           // offset 0x68, size 0x4
    float mOriginalNeedleLeftX;           // offset 0x6C, size 0x4
#endif
};
#endif
