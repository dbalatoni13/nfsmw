#ifndef EAXSOUND_CARSFX_SFXOBJ_HELICOPTER_H
#define EAXSOUND_CARSFX_SFXOBJ_HELICOPTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/sfxctl_helicopter.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/MAIN_AEMS.h"

class SFXObj_Helicopter : public CARSFX {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXObj_Helicopter();
    ~SFXObj_Helicopter() override;

    static SndBase *CreateObject(unsigned int allocator);
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Detach() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    FX_HelicopterStruct m_HeliAemsData; // offset 0x28, size 0x30
    FX_Helicopter *m_pCsisHeli;         // offset 0x58, size 0x4
    SFXCTL_Helicopter *m_pHeliCtl;      // offset 0x5C, size 0x4
};

#endif
