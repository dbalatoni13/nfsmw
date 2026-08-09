//
//
//
#ifndef CARSFX_TURBO_H
#define CARSFX_TURBO_H // Decl: 5

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/TURBO.h"

#define NEEDLE_DROP_BEFORE_BLOWOFF 0.2f // Decl: 12

// total size: 0x148
// Decl: 20
class CARSFX_Turbo : public CARSFX {
  public:
    DECLARE_CREATABLE();
    CARSFX_Turbo();
    ~CARSFX_Turbo() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void SetupLoadData() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    // float GetTurbo() {} // Decl: 42

    int PlaySpl(int _ID, int Vol, int PSI, int Azimuth, int rotation);
    int UpdateSpool(float t);
    void ResetSpool();
    int PlayBlowoff(int _ID, int Vol, int PSI, int Azimuth, int rotation);
    void UpdateBlowOff(float t);
    void StopBlowOff();
    bool IsBlowOffDone();

    // Overrides: SndBase
    void Detach() override;

    bool bStopBlowoff;                         // offset 0x28, size 0x1, Decl: 52
    SFXCTL_Shifting *m_pShiftingCtl;           // offset 0x2C, size 0x4, Decl: 55
    SFXCTL_Engine *m_pEngineCtl;               // offset 0x30, size 0x4, Decl: 56
    Csis::FX_TURBO_01 *m_pTurboSplControl;     // offset 0x34, size 0x4, Decl: 58
    Csis::FX_TURBO_01 *m_pTurboBlowoffControl; // offset 0x38, size 0x4, Decl: 59
    cInterpLine m_BlowoffRampDown;             // offset 0x3C, size 0x1C, Decl: 60
    Attrib::Gen::turbosfx *m_pTurboData;       // offset 0x58, size 0x4, Decl: 61
    int vol_Spool;                             // offset 0x5C, size 0x4, Decl: 63
    int BlowoffVol;                            // offset 0x60, size 0x4, Decl: 64
    int BlowoffID;                             // offset 0x64, size 0x4, Decl: 65
    float tLastBlowoffTime;                    // offset 0x68, size 0x4, Decl: 66
    float SpoolCharge;                         // offset 0x6C, size 0x4, Decl: 68
    float SpoolPercent;                        // offset 0x70, size 0x4, Decl: 69
    float tReachedPeak;                        // offset 0x74, size 0x4, Decl: 70
    bool bReachedPeak;                         // offset 0x78, size 0x1, Decl: 71
    cPathLine m_SpoolDuck;                     // offset 0x7C, size 0x8C, Decl: 72
    eSFX_TURBO eTurboState;                    // offset 0x108, size 0x4, Decl: 75
    float m_fTurbo;                            // offset 0x10C, size 0x4, Decl: 78
    Average m_fDeltaTurbo;                     // offset 0x110, size 0x28, Decl: 79
    float t_LastBlowOff;                       // offset 0x138, size 0x4, Decl: 81
    float m_MaxTurbo;                          // offset 0x13C, size 0x4, Decl: 82
    bool m_HasTurbo;                           // offset 0x140, size 0x1, Decl: 83
    SFXCTL_3DCarPos *m_p3DCarPosCtl;           // offset 0x144, size 0x4, Decl: 85
};

#endif
