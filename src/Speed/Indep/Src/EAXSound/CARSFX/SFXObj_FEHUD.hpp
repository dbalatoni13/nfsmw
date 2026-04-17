#ifndef EAXSOUND_CARSFX_SFXOBJ_FEHUD_H
#define EAXSOUND_CARSFX_SFXOBJ_FEHUD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"

class SFXObj_FEHUD : public CARSFX {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXObj_FEHUD();
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    ~SFXObj_FEHUD() override;
    int GetController(int Index) override { return -1; }
    void AttachController(SFXCTL *psfxctl) override {}
    void InitSFX() override;
    void Destroy() override;
    void UpdateMixerOutputs() override;
};

#endif
