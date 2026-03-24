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

struct EngineMix {
    float Aems;       // offset 0x0
    float AccelGinsu; // offset 0x4
    float DecelGinsu; // offset 0x8
    int Cutoff;       // offset 0xC

    EngineMix()
        : Aems(0.0f)       //
        , AccelGinsu(0.0f) //
        , DecelGinsu(0.0f) //
        , Cutoff(0) {}
};

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
    /* 0x44 */ Graph DecelCrossfadeMix;
    /* 0x4c */ bVector2 CrossFadesPoints[5];
    /* 0x74 */ int m_EngVolRedLine;
    /* 0x78 */ int mPrevDeltaRPM;
    /* 0x7c */ float m_GinsuLPFVal;
    /* 0x80 */ bool m_bAEMSLPF;
    /* 0x84 */ unsigned short SteadyFrameCnt;
    /* 0x88 */ Average m_AvgDeltaRPM;
    /* 0xb0 */ float m_CurPhyDeltaRPMVal;
    /* 0xb4 */ float m_CurAudDeltaRPMVal;
    /* 0xb8 */ float PrevRPM;
    /* 0xbc */ float m_GinsuScaledRPM;
    /* 0xc0 */ float mPrevPhyDeltaRPM;
    /* 0xc4 */ EngineMix m_EngineMix;
    /* 0xd4 */ float PercentOfAccelThreshold;
    /* 0xd8 */ float PercentOfDecelThreshold;

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
