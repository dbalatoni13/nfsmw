#ifndef FRONTEND_HUD_FEGETAWAYMETER_H
#define FRONTEND_HUD_FEGETAWAYMETER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

class IGetAwayMeter : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IGetAwayMeter(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual void SetGetAwayDistance(float distance);

  protected:
    virtual ~IGetAwayMeter() {}
};

// total size: 0x40
class GetAwayMeter : public HudElement, public IGetAwayMeter {
  public:
    GetAwayMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetGetAwayDistance(float distance) override;

  private:
    float mGetawayDistance;               // offset 0x30
    FEObject *mpDataDistanceBar;          // offset 0x34
    FEString *mpDataDistanceString;       // offset 0x38
};

#endif
