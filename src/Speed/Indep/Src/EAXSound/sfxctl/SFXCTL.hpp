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
    eAemsUpgradeLevel m_UGL; // offset 0x24, size 0x4

    SFXCTL();
    ~SFXCTL() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    void InitSFX() override;
    void UpdateParams(float t) override;

    float GetPhysTRQ();
    float GetPhysRPM();

    static TypeInfo *GetStaticTypeInfo(void) { return &s_TypeInfo; }
};

#endif
