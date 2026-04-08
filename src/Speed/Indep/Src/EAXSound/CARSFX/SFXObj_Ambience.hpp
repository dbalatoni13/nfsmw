#ifndef EAXSOUND_CARSFX_SFXOBJ_AMBIENCE_H
#define EAXSOUND_CARSFX_SFXOBJ_AMBIENCE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"

struct SFXObj_Ambience : public CARSFX {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo();

  public:
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    SFXObj_Ambience();
    ~SFXObj_Ambience() override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    int GetController(int Index) override { return -1; }
    void AttachController(SFXCTL *psfxctl) override {}
    void ProcessUpdate() override {}
};

#endif
