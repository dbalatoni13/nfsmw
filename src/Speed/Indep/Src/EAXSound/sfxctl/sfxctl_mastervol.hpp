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
    SFXCTL_MasterVol();
    ~SFXCTL_MasterVol() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void InitSFX() override;
    void UpdateParams(float t) override;
};

struct SFXCTL_GameState : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    SFXCTL_GameState() {}
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }
    static SndBase *CreateObject(unsigned int allocator);
    void UpdateMixerOutputs() override;
};

#endif
