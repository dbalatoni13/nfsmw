#ifndef EAXSOUND_SFXCTL_SFXCTL_MASTERVOL_H
#define EAXSOUND_SFXCTL_SFXCTL_MASTERVOL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct SFXCTL_MasterVol : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    void InitSFX() override;
};

struct SFXCTL_GameState : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
};

#endif
