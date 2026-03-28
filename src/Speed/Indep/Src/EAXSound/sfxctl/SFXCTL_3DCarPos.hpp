#ifndef EAXSOUND_SFXCTL_SFXCTL_3DCARPOS_H
#define EAXSOUND_SFXCTL_SFXCTL_3DCARPOS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

struct SFXCTL_3DCarPos : public SFXCTL_3DObjPos {
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXCTL_3DCarPos();
    ~SFXCTL_3DCarPos() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void InitSFX() override;
};

#endif
