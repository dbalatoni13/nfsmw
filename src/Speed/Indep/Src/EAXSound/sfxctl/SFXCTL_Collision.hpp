#ifndef EAXSOUND_SFXCTL_SFXCTL_COLLISION_H
#define EAXSOUND_SFXCTL_SFXCTL_COLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

struct SFXCTL_3DColPos : public SFXCTL_3DObjPos {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
};

struct SFXCTL_3DScrapePos : public SFXCTL_3DObjPos {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
};

#endif
