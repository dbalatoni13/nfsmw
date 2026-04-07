#ifndef EAXSOUND_CARSFX_CARSFX_SIREN_H
#define EAXSOUND_CARSFX_CARSFX_SIREN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/COP_SIREN_AEMS.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// total size: 0x40
class CARSFX_Siren : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;

  private:
    SIREN *mSiren;               // offset 0x28, size 0x4
    float tSirenState;           // offset 0x2C, size 0x4
    bool bIgnoreHealth;          // offset 0x30, size 0x1
    Timer mT_death;              // offset 0x34, size 0x4
    SirenState m_SirenState;     // offset 0x38, size 0x4
    SirenState m_PrevSirenState; // offset 0x3C, size 0x4

  public:
    CARSFX_Siren();
    ~CARSFX_Siren() override;

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;

    static SndBase *CreateObject(unsigned int allocator);

    void Detach() override;
    void Destroy() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    SirenState UpdateSirenState(float t);
};

#endif
