#ifndef EAXSOUND_CARSFX_CARSFX_H
#define EAXSOUND_CARSFX_CARSFX_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"

struct CARSFX : SFX_Base {
  public:
    CARSFX();
    virtual ~CARSFX();
    float GetPhysTRQ();
    float GetPhysRPM();

  protected:
    // total size: 0x28
    eAemsUpgradeLevel m_UGL; // offset 0x24, size 0x4
};

#endif
