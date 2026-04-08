#ifndef EAXSOUND_SFXCTL_SFXCTL_COLLISION_H
#define EAXSOUND_SFXCTL_SFXCTL_COLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

// total size: 0x64
struct SFXCTL_3DColPos : public SFXCTL_3DObjPos {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    ~SFXCTL_3DColPos() override {}
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
};

// total size: 0x64
struct SFXCTL_3DScrapePos : public SFXCTL_3DObjPos {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    ~SFXCTL_3DScrapePos() override {}
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
};

#endif
