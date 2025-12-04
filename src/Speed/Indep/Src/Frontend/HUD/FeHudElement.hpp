#ifndef FRONTEND_HUD_FEHUDELEMENT_H
#define FRONTEND_HUD_FEHUDELEMENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"

// total size: 0x28
class HudElement {
  public:
  private:
    bPList<FEObject> Objects;              // offset 0x0, size 0x8
    const char *pPackageName;              // offset 0x8, size 0x4
    unsigned long long Mask;               // offset 0x10, size 0x8
    unsigned long long CurrentHudFeatures; // offset 0x18, size 0x8
    bool mCurrentlySetVisible;             // offset 0x20, size 0x1
};

#endif
