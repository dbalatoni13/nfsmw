#ifndef FRONTEND_HUD_FEWRONGWINDI_H
#define FRONTEND_HUD_FEWRONGWINDI_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

class IWrongWay : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IWrongWay(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~IWrongWay() {}

  public:
    virtual void SetWrongWay(bool isWrongWay);
};

// total size: 0x40
class WrongWIndi : public HudElement, public IWrongWay {
  public:
    WrongWIndi(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetWrongWay(bool isWrongWay) override;

  private:
    FEImage *mpWrongWayImage;                  // offset 0x30
    bool mIsWrongWay;                          // offset 0x34
    Timer mTimeBeforeDisplaying;               // offset 0x38
    Timer mTimeBeforeClosing;                  // offset 0x3C
};

#endif
