#ifndef EAXSOUND_SFXCTL_SFXCTL_H
#define EAXSOUND_SFXCTL_SFXCTL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/SndBase.hpp"

struct SFXCTL : public SndBase {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    /* 0x24 */ eAemsUpgradeLevel m_UGL;

    SFXCTL();
    virtual ~SFXCTL();
    virtual TypeInfo *GetTypeInfo();
    virtual char *GetTypeName();
    virtual void InitSFX();
    virtual void UpdateParams();

    float GetPhysTRQ();
    float GetPhysRPM();

    static TypeInfo *GetStaticTypeInfo(void);
};

#endif
