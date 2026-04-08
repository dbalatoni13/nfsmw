#ifndef EAXSOUND_CARSFX_CARSFX_TURBO_H
#define EAXSOUND_CARSFX_CARSFX_TURBO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/TURBO.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/turbosfx.h"
#include "Speed/Indep/Src/Misc/Table.hpp"

enum eSFX_TURBO {
    SFXTURBO_NONE = 0,
    SFXTURBO_SPOOLING = 1,
    SFXTURBO_BLOWOFF = 2,
};

// total size: 0x148
class CARSFX_Turbo : public CARSFX {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    CARSFX_Turbo();
    ~CARSFX_Turbo() override;

    static SndBase *CreateObject(unsigned int allocator);

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Detach() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void SetupLoadData() override;

    int PlayBlowoff(int id, int volume, int pSI, int azimuth, int rotation);
    bool IsBlowOffDone();
    void UpdateBlowOff(float t);
    void StopBlowOff();
    int PlaySpl(int id, int volume, int pSI, int azimuth, int rotation);
    void ResetSpool();
    int UpdateSpool(float t);

  private:
    bool bStopBlowoff;                          // offset 0x28, size 0x1
    SFXCTL_Shifting *m_pShiftingCtl;            // offset 0x2C, size 0x4
    SFXCTL_Engine *m_pEngineCtl;                // offset 0x30, size 0x4
    FX_TURBO_01 *m_pTurboSplControl;            // offset 0x34, size 0x4
    FX_TURBO_01 *m_pTurboBlowoffControl;        // offset 0x38, size 0x4
    cInterpLine m_BlowoffRampDown;              // offset 0x3C, size 0x1C
    Attrib::Gen::turbosfx *m_pTurboData;        // offset 0x58, size 0x4
    int vol_Spool;                              // offset 0x5C, size 0x4
    int BlowoffVol;                             // offset 0x60, size 0x4
    int BlowoffID;                              // offset 0x64, size 0x4
    float tLastBlowoffTime;                     // offset 0x68, size 0x4
    float SpoolCharge;                          // offset 0x6C, size 0x4
    float SpoolPercent;                         // offset 0x70, size 0x4
    float tReachedPeak;                         // offset 0x74, size 0x4
    bool bReachedPeak;                          // offset 0x78, size 0x1
    cPathLine m_SpoolDuck;                      // offset 0x7C, size 0x8C
    eSFX_TURBO eTurboState;                     // offset 0x108, size 0x4
    float m_fTurbo;                             // offset 0x10C, size 0x4
    Average m_fDeltaTurbo;                      // offset 0x110, size 0x28
    float t_LastBlowOff;                        // offset 0x138, size 0x4
    float m_MaxTurbo;                           // offset 0x13C, size 0x4
    bool m_HasTurbo;                            // offset 0x140, size 0x1
    SFXCTL_3DCarPos *m_p3DCarPosCtl;            // offset 0x144, size 0x4
};

#endif
