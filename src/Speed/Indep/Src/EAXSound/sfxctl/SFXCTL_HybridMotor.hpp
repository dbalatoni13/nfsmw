#ifndef EAXSOUND_SFXCTL_SFXCTL_HYBRIDMOTOR_H
#define EAXSOUND_SFXCTL_SFXCTL_HYBRIDMOTOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"

struct SFXCTL_Engine;
struct SFXCTL_Shifting;
struct SFXCTL_AccelTrans;

struct SFXCTL_HybridMotor : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    SFXCTL_HybridMotor();
    /* 0x28 */ SFXCTL_Engine *m_pEngineCtl;
    /* 0x2c */ SFXCTL_Shifting *m_pShiftingCtl;
    /* 0x30 */ SFXCTL_AccelTrans *m_pAccelTranCtl;
    /* 0x34 */ float tSteadyDuration;
    /* 0x38 */ int m_EngVolAEMS;
    /* 0x3c */ int m_EngVolAccelGinsu;
    /* 0x40 */ int m_EngVolDecelGinsu;
    /* 0x44 */ char _pad_hm0[0x88 - 0x44]; // Graph, CrossFades, etc
    /* 0x88 */ Average m_AvgDeltaRPM;
    /* 0xb0 */ char _pad_hm1[0xd4 - 0xb0]; // remaining fields

    ~SFXCTL_HybridMotor() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    int GetController(int Index) override;
    static SndBase *CreateObject(unsigned int);
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void AttachController(SFXCTL *) override;
    void UpdateDeltaRPM();
    void UpdateParams(float t) override;
    void UpdateDualMixEng(float t);
    void UpdateSingleMixEng(float t);
    void UpdateVolumeRedlining();
    void UpdateMixerOutputs() override;
};

#endif
