#ifndef EAXSOUND_SFXCTL_SFXCTL_TUNNEL_H
#define EAXSOUND_SFXCTL_SFXCTL_TUNNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"

struct SFXCTL_Tunnel : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    /* 0x28 */ char _pad_tun0[0x7c - 0x28];
    /* 0x7c */ cInterpLine ReflRamp;

    ~SFXCTL_Tunnel() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void Destroy() override;

    void EndTunnelVerb();
};

#endif
