#ifndef FESHIFTUPDATER_H
#define FESHIFTUPDATER_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"

class ShiftUpdater : public HudElement, public IShiftUpdater {
  public:
    ShiftUpdater(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~ShiftUpdater() override {}

    void Update(IPlayer *player) override;
    void SetGear(GearID gear, ShiftStatus status, ShiftPotential potential, bool hasGoodEnoughTraction) override {
        if (gear != mGear) {
            int dir = -1;
            if (gear > mGear) {
                dir = 1;
            }
            mGear = gear;
            mGearChanged = dir;
            mShiftPotential = SHIFT_POTENTIAL_NONE;
            if (hasGoodEnoughTraction) {
                mLastShiftStatus = status;
            } else {
                mLastShiftStatus = SHIFT_STATUS_NORMAL;
            }
            return;
        }
        if (hasGoodEnoughTraction) {
            mShiftPotential = potential;
        } else {
            mShiftPotential = SHIFT_POTENTIAL_NONE;
        }
    };
    void SetEngineBlown(bool blown) override {
        mIsEngineBlown = blown;
    };
    void SetEngineTemp(float temp) override {
        mEngineTemp = temp;
    };

  private:
    FEImage *pShiftIndicator;              // offset 0x30, size 0x4
    FEGroup *pShiftIndicatorOverheatGroup; // offset 0x34, size 0x4
    FEGroup *pShiftMsgGroup;               // offset 0x38, size 0x4
    FEString *pShiftMsg;                   // offset 0x3C, size 0x4
    FEString *pShiftMsgShadow;             // offset 0x40, size 0x4
    GearID mGear;                          // offset 0x44, size 0x4
    ShiftPotential mShiftPotential;        // offset 0x48, size 0x4
    int mGearChanged;                      // offset 0x4C, size 0x4
    ShiftStatus mLastShiftStatus;          // offset 0x50, size 0x4
    bool mIsEngineBlown;                   // offset 0x54, size 0x1
    float mEngineTemp;                     // offset 0x58, size 0x4
};

#endif
