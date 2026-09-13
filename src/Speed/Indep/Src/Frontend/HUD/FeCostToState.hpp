#ifndef FECOSTTOSTATE_H
#define FECOSTTOSTATE_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

// total size: 0x48
class CostToState : public HudElement, public ICostToState {
  public:
    CostToState(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetCostToState(int cost) override;
    void SetInPursuit(bool inPursuit) override {
        mInPursuit = inPursuit;
    };

  private:
    bool mCostToStateOn;        // offset 0x30
    int mCostToState;           // offset 0x34
    bool mInPursuit;            // offset 0x38
    int mNumFramesLeftToShow;   // offset 0x3C
    FEString *mDataCostToState; // offset 0x40
    FEString *mDataTitle;       // offset 0x44
};

#endif
