#ifndef EAXSOUND_CARSFX_CARSFX_H
#define EAXSOUND_CARSFX_CARSFX_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"

struct CARSFX : SFX_Base {
  public:
    CARSFX();
    virtual ~CARSFX();
    float GetPhysTRQ() { return m_pEAXCar->GetPhysTRQ(); }
    float GetPhysRPM() { return m_pEAXCar->GetPhysRPM(); }

  protected:
    // total size: 0x28
    eAemsUpgradeLevel m_UGL; // offset 0x24, size 0x4
};

#endif
