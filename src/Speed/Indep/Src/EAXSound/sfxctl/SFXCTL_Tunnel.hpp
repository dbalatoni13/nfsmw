#ifndef EAXSOUND_SFXCTL_SFXCTL_TUNNEL_H
#define EAXSOUND_SFXCTL_SFXCTL_TUNNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct SFXCTL_Tunnel : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void Destroy() override;
};

#endif
