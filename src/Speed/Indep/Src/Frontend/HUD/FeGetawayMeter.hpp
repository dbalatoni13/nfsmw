#ifndef FEGETAWAYMETER_H
#define FEGETAWAYMETER_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

// total size: 0x40
class GetAwayMeter : public HudElement, public IGetAwayMeter {
  public:
    GetAwayMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetGetAwayDistance(float distance) override {
        mGetawayDistance = distance;
    };

  private:
    float mGetawayDistance;         // offset 0x30
    FEObject *mpDataDistanceBar;    // offset 0x34
    FEString *mpDataDistanceString; // offset 0x38
};

#endif
