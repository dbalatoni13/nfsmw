#ifndef FERADARDETECTOR_H
#define FERADARDETECTOR_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// File: speed/indep/src/frontend/hud/FeRadarDetector.hpp
// total size: 0x68
// Decl: speed/indep/src/frontend/hud/FeRadarDetector.hpp:8
class RadarDetector : public HudElement, public IRadarDetector {
  public:
    RadarDetector(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~RadarDetector() override {} // Decl: speed/indep/src/frontend/hud/FeRadarDetector.hpp:11

    void Update(IPlayer *player) override;

    void SetTarget(RadarTarget targetType, float range, Radians direction) override {
        mTargetType = targetType;
        mDirection = direction;
        mRange = range;
        mStaticRange = range;
    };
    void SetInPursuit(bool inPursuit) override {
        mInPursuit = inPursuit;
    };
    void SetIsCoolingDown(bool coolingDown) override {
        mIsCoolingDown = coolingDown;
    };

    static float mStaticRange; // size: 0x4, address: 0x8041BC64

  private:
    FEGroup *mpDataRadarDetectorGroup;              // offset 0x30, size 0x4
    FEObject *mpDataRadarDetectorLightsLeft;        // offset 0x34, size 0x4
    FEObject *mpDataRadarDetectorLightsRight;       // offset 0x38, size 0x4
    FEObject *mpDataRadarDetectorArrow;             // offset 0x3C, size 0x4
    FEObject *mpDataRadarIcon;                      // offset 0x40, size 0x4
    FEObject *mpDataRadarDetectorBacking;           // offset 0x44, size 0x4
    FEObject *mpDataRadarDetectorBackingWithMirror; // offset 0x48, size 0x4
    float mRange;                                   // offset 0x4C, size 0x4
    Radians mDirection;                             // offset 0x50, size 0x4
    RadarTarget mTargetType;                        // offset 0x54, size 0x4
    float mCurrLedAmountShowing;                    // offset 0x58, size 0x4
    bool mInPursuit;                                // offset 0x5C, size 0x1
    bool mIsCoolingDown;                            // offset 0x60, size 0x1
    Timer mTimeCycleStarted;                        // offset 0x64, size 0x4
};

#endif
