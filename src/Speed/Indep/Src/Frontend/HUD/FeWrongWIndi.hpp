#ifndef FEWRONGWINDI_H
#define FEWRONGWINDI_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// File: speed/indep/src/frontend/hud/FeWrongWIndi.hpp
// total size: 0x40
// Decl: speed/indep/src/frontend/hud/FeWrongWIndi.hpp:16
class WrongWIndi : public HudElement, public IWrongWay {
  public:
    WrongWIndi(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~WrongWIndi() override {} // Decl: speed/indep/src/frontend/hud/FeWrongWIndi.hpp:19

    void Update(IPlayer *player) override;
    void SetWrongWay(bool isWrongWay) override;

  private:
    FEImage *mpWrongWayImage;    // offset 0x30, size 0x4
    bool mIsWrongWay;            // offset 0x34, size 0x1, Decl: speed/indep/src/frontend/hud/FeWrongWIndi.hpp:27
    Timer mTimeBeforeDisplaying; // offset 0x38, size 0x4, Decl: speed/indep/src/frontend/hud/FeWrongWIndi.hpp:28
    Timer mTimeBeforeClosing;    // offset 0x3C, size 0x4, Decl: speed/indep/src/frontend/hud/FeWrongWIndi.hpp:29
};

#endif
