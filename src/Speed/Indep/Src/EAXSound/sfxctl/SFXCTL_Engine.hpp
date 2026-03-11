#ifndef EAXSOUND_SFXCTL_SFXCTL_ENGINE_H
#define EAXSOUND_SFXCTL_SFXCTL_ENGINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct SFXCTL_Shifting;
struct SFXCTL_AccelTrans;
struct SFXCTL_3DCarPos;

struct SFXCTL_Engine : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    /* 0x28 */ SFXCTL_Shifting *m_pShiftCtl;
    /* 0x2c */ SFXCTL_AccelTrans *m_pAccelTransitionCtl;
    /* 0x30 */ struct SFXCTL_Physics *m_pPhysicsCtl;
    /* 0x34 */ SFXCTL_3DCarPos *m_p3DCarPosCtl;

    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
};

#endif
