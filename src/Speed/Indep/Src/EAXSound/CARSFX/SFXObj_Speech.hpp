#ifndef EAXSOUND_CARSFX_SFXOBJ_SPEECH_H
#define EAXSOUND_CARSFX_SFXOBJ_SPEECH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct EAX_CarState;

class SFXCTL_3DVoiceActorPos : private SFXCTL_3DObjPos {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXCTL_3DVoiceActorPos() {}
    ~SFXCTL_3DVoiceActorPos() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
};

class SFXObj_Speech : public CARSFX {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXObj_Speech();
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    ~SFXObj_Speech() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override {}

    SFXCTL_3DObjPos *m_pActorPos; // offset 0x28, size 0x4
    bVector3 fPosition;           // offset 0x2C, size 0x10
    bool bFresh;                  // offset 0x3C, size 0x1
    EAX_CarState *m_pClosestCar;  // offset 0x40, size 0x4
};

#endif
