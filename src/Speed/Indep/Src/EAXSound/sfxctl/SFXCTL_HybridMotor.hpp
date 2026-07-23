//
//
//
//
#ifndef SFXCTL_HYBRIDMOTOR_H
#define SFXCTL_HYBRIDMOTOR_H

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"

// total size: 0x10
// Decl: 10
struct EngineMix {
    EngineMix() {} // Decl: 11

    float Aems;       // offset 0x0, size 0x4, Decl: 19
    float AccelGinsu; // offset 0x4, size 0x4, Decl: 20
    float DecelGinsu; // offset 0x8, size 0x4, Decl: 21
    int Cutoff;       // offset 0xC, size 0x4, Decl: 22
};

class SFXCTL_AccelTrans;
class SFXCTL_Engine;
class SFXCTL_Shifting;

// total size: 0xDC
// Decl: 28
class SFXCTL_HybridMotor : public SFXCTL {
  public:
    DECLARE_CREATABLE();

    SFXCTL_HybridMotor();
    ~SFXCTL_HybridMotor() override;

    // Overrides: SndBase
    void UpdateParams(float t) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void UpdateMixerOutputs() override;

    void UpdateVolumeRedlining();
    void UpdateDeltaRPM();
    void UpdateSingleMixEng(float t);
    void UpdateDualMixEng(float t);

    SFXCTL_Engine *m_pEngineCtl;        // offset 0x28, size 0x4, Decl: 46
    SFXCTL_Shifting *m_pShiftingCtl;    // offset 0x2C, size 0x4, Decl: 47
    SFXCTL_AccelTrans *m_pAccelTranCtl; // offset 0x30, size 0x4, Decl: 48

    float tSteadyDuration; // offset 0x34, size 0x4, Decl: 50

    int m_EngVolAEMS;             // offset 0x38, size 0x4, Decl: 52
    int m_EngVolAccelGinsu;       // offset 0x3C, size 0x4, Decl: 53
    int m_EngVolDecelGinsu;       // offset 0x40, size 0x4, Decl: 54
    Graph DecelCrossfadeMix;      // offset 0x44, size 0x8, Decl: 56
    bVector2 CrossFadesPoints[5]; // offset 0x4C, size 0x28, Decl: 57

    int m_EngVolRedLine; // offset 0x74, size 0x4, Decl: 59

    int mPrevDeltaRPM; // offset 0x78, size 0x4, Decl: 61

    float m_GinsuLPFVal;   // offset 0x7C, size 0x4, Decl: 64
    bool m_bAEMSLPF;       // offset 0x80, size 0x1, Decl: 65
    uint16 SteadyFrameCnt; // offset 0x84, size 0x2, Decl: 68

    Average m_AvgDeltaRPM;     // offset 0x88, size 0x28, Decl: 74
    float m_CurPhyDeltaRPMVal; // offset 0xB0, size 0x4, Decl: 75
    float m_CurAudDeltaRPMVal; // offset 0xB4, size 0x4, Decl: 76
    float PrevRPM;             // offset 0xB8, size 0x4, Decl: 77

    float m_GinsuScaledRPM; // offset 0xBC, size 0x4, Decl: 79
    float mPrevPhyDeltaRPM; // offset 0xC0, size 0x4, Decl: 80

    EngineMix m_EngineMix; // offset 0xC4, size 0x10, Decl: 84

    float PercentOfAccelThreshold; // offset 0xD4, size 0x4, Decl: 89
    float PercentOfDecelThreshold; // offset 0xD8, size 0x4, Decl: 90

    bool IsActive() {} // Decl: 98
};

#endif
