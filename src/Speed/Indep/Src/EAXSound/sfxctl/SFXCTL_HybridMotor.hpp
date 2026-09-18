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
    float Aems;       // offset 0x0, size 0x4
    float AccelGinsu; // offset 0x4, size 0x4
    float DecelGinsu; // offset 0x8, size 0x4
    int Cutoff;       // offset 0xC, size 0x4

    EngineMix()
        : Aems(0.0f)       //
        , AccelGinsu(0.0f) //
        , DecelGinsu(0.0f) //
        , Cutoff(0) {}
};

struct SFXCTL_HybridMotor : public SFXCTL {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXCTL_HybridMotor();
    SFXCTL_Engine *m_pEngineCtl;            // offset 0x28, size 0x4
    SFXCTL_Shifting *m_pShiftingCtl;        // offset 0x2C, size 0x4
    SFXCTL_AccelTrans *m_pAccelTranCtl;     // offset 0x30, size 0x4
    float tSteadyDuration;                  // offset 0x34, size 0x4
    int m_EngVolAEMS;                       // offset 0x38, size 0x4
    int m_EngVolAccelGinsu;                 // offset 0x3C, size 0x4
    int m_EngVolDecelGinsu;                 // offset 0x40, size 0x4
    Graph DecelCrossfadeMix;                // offset 0x44, size 0x8
    bVector2 CrossFadesPoints[5];           // offset 0x4C, size 0x28
    int m_EngVolRedLine;                    // offset 0x74, size 0x4
    int mPrevDeltaRPM;                      // offset 0x78, size 0x4
    float m_GinsuLPFVal;                    // offset 0x7C, size 0x4
    bool m_bAEMSLPF;                        // offset 0x80, size 0x1
    unsigned short SteadyFrameCnt;          // offset 0x84, size 0x2
    Average m_AvgDeltaRPM;                  // offset 0x88, size 0x28
    float m_CurPhyDeltaRPMVal;             // offset 0xB0, size 0x4
    float m_CurAudDeltaRPMVal;             // offset 0xB4, size 0x4
    float PrevRPM;                          // offset 0xB8, size 0x4
    float m_GinsuScaledRPM;                // offset 0xBC, size 0x4
    float mPrevPhyDeltaRPM;               // offset 0xC0, size 0x4
    EngineMix m_EngineMix;                  // offset 0xC4, size 0x10
    float PercentOfAccelThreshold;          // offset 0xD4, size 0x4
    float PercentOfDecelThreshold;          // offset 0xD8, size 0x4

    ~SFXCTL_HybridMotor() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
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
