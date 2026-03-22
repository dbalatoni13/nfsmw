#ifndef FRONTEND_HUD_FENITROUSGAUGE_H
#define FRONTEND_HUD_FENITROUSGAUGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

struct FEMultiImage;

class INos : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    INos(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~INos() {}

  public:
    virtual void SetNos(float nos);
};

// total size: 0x40
class NitrousGauge : public HudElement, public INos {
  public:
    NitrousGauge(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetNos(float nos) override;

  private:
    float mNos;                            // offset 0x30
    FEObject *mpDataNosMeterIcon;          // offset 0x34
    FEMultiImage *mpNosMeterBar;           // offset 0x38
};

#endif
