#ifndef FERACEINFORMATION_H
#define FERACEINFORMATION_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"

// File: speed/indep/src/frontend/hud/FeRaceInformation.hpp
// total size: 0x78
// Decl: speed/indep/src/frontend/hud/FeRaceInformation.hpp:8
class RaceInformation : public HudElement, public IRaceInformation {
  public:
    RaceInformation(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~RaceInformation() override {} // Decl: speed/indep/src/frontend/hud/FeRaceInformation.hpp:11

    void Update(IPlayer *player) override;
    void SetNumRacers(int numRacers) override {
        mNumRacers = numRacers;
    };
    void SetNumLaps(int numLaps) override {
        mNumLaps = numLaps;
    };
    void SetPlayerPosition(int position) override {
        mPlayerPosition = position;
    };
    void SetPlayerLapNumber(int lap) override {
        if (lap > mNumLaps) {
            lap = mNumLaps;
        }
        mPlayerLapNumber = lap;
    };
    void SetPlayerLapTime(float lapTime) override {
        mPlayerLapTime = lapTime;
    };
    void SetSuddenDeathMode(bool suddenDeath) override {
        mSuddenDeath = suddenDeath;
    };
    void SetPlayerPercentComplete(float percent) override {
        mPlayerPercentComplete = percent;
    };
    void SetPlayerTollboothsCrossed(int num) override {
        mPlayerTollboothNumber = num;
    };
    void SetNumTollbooths(int num) override {
        mNumTollbooths = num;
    };

  private:
    int mNumRacers;                  // offset 0x30, size 0x4
    int mNumLaps;                    // offset 0x34, size 0x4
    int mPlayerPosition;             // offset 0x38, size 0x4
    int mPlayerLapNumber;            // offset 0x3C, size 0x4
    float mPlayerLapTime;            // offset 0x40, size 0x4
    bool mSuddenDeath;               // offset 0x44, size 0x1
    float mPlayerPercentComplete;    // offset 0x48, size 0x4
    int mPlayerTollboothNumber;      // offset 0x4C, size 0x4
    int mNumTollbooths;              // offset 0x50, size 0x4
    FEString *mDataCurrentLapTime;   // offset 0x54, size 0x4
    FEString *mDataCompleteText;     // offset 0x58, size 0x4
    FEGroup *mDataPositionGroup;     // offset 0x5C, size 0x4
    FEImage *mDataIconTollbooth;     // offset 0x60, size 0x4
    FEObject *mpDataTollboothNumTop; // offset 0x64, size 0x4
    FEObject *mpDataTollboothNumBot; // offset 0x68, size 0x4
    FEObject *mpDataRacePosNum;      // offset 0x6C, size 0x4
    FEObject *mpDataRacerCount;      // offset 0x70, size 0x4
    FEObject *mpDataPercentComplete; // offset 0x74, size 0x4
};

#endif
