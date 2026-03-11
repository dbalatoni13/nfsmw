#ifndef EAXSOUND_SFXCTL_SFXCTL_HELICOPTER_H
#define EAXSOUND_SFXCTL_SFXCTL_HELICOPTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

struct SFXCTL_3DHeliPos : public SFXCTL_3DObjPos {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
};

struct EAX_HeliState;

struct SFXCTL_Helicopter : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    /* 0x28 */ EAX_HeliState *m_pHeliState;
    /* 0x2c */ SFXCTL_3DHeliPos *m_p3DHeliPosCtl;

    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    int GetController(int Index) override;
    void Detach() override;
};

#endif
