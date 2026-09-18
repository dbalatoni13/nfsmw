#ifndef FEREPUTATION_H
#define FEREPUTATION_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

// total size: 0x48
class Reputation : public HudElement, public IReputation {
  public:
    Reputation(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetReputationCareer(int rep) override;
    void SetReputationPursuit(int rep) override;

  private:
    int mReputationCareer;           // offset 0x30, size 0x4
    int mNumFramesLeftToShow;        // offset 0x34, size 0x4
    FEString *mDataReputationCareer; // offset 0x38, size 0x4
    FEString *mDataTitle;            // offset 0x3C, size 0x4
    FEObject *mDataReputationGrp;    // offset 0x40, size 0x4
};

#endif
