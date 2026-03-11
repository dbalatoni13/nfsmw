#ifndef EAXSOUND_SFXCTL_SFXCTL_HYBRIDMOTOR_H
#define EAXSOUND_SFXCTL_SFXCTL_HYBRIDMOTOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct SFXCTL_Engine;
struct SFXCTL_Shifting;
struct SFXCTL_AccelTrans;

struct SFXCTL_HybridMotor : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    /* 0x28 */ SFXCTL_Engine *m_pEngineCtl;
    /* 0x2c */ SFXCTL_Shifting *m_pShiftingCtl;
    /* 0x30 */ SFXCTL_AccelTrans *m_pAccelTranCtl;
    /* 0x34 */ float tSteadyDuration;
    /* 0x38 */ int m_EngVolAEMS;
    /* 0x3c */ int m_EngVolAccelGinsu;
    /* 0x40 */ int m_EngVolDecelGinsu;
    /* 0x44 */ char _pad_hm0[0xd4 - 0x44]; // rest of fields

    ~SFXCTL_HybridMotor() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    int GetController(int Index) override;
    void InitSFX() override;
};

#endif
