#ifndef EAXSOUND_CARSFX_SFXOBJ_REVERB_H
#define EAXSOUND_CARSFX_SFXOBJ_REVERB_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"

namespace Snd {
class GlobalFxProcessor;
}

struct SFXObj_Reverb : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    static Snd::GlobalFxProcessor *m_pFXEditModule[2];
    static char *m_pFXEditPatch[12];
    SFXCTL_Tunnel *m_pTunnelCtl; // offset 0x28, size 0x4

    void InitSFX() override;
};

#endif
