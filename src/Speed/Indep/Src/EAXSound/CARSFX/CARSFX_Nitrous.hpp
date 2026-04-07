#ifndef EAXSOUND_CARSFX_CARSFX_NITROUS_H
#define EAXSOUND_CARSFX_CARSFX_NITROUS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/MAIN_AEMS.h"

enum eSFX_NITROUS {
    SFX_NITROUS_NONE = 0,
    SFX_NITROUS_ON = 1,
};

class CARSFX_Nitrous : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    CARSFX_Nitrous();
    ~CARSFX_Nitrous() override;

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void SetupLoadData() override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    int Play(int type, int Vol, int Azimuth);
    void Stop();
    void PlayPurge();

    FX_NITROUS *m_NitrousControl;      // offset 0x28, size 0x4
    FX_PURGE *m_NitrousPurge;          // offset 0x2C, size 0x4
    eSFX_NITROUS eNitrousState;        // offset 0x30, size 0x4
    int m_NitrousPlayHandle;           // offset 0x34, size 0x4
    int CurVol;                        // offset 0x38, size 0x4
    SFXCTL_3DCarPos *m_p3DCarPosCtl;   // offset 0x3C, size 0x4
    SFXCTL_Shifting *m_pShiftCtl;      // offset 0x40, size 0x4
    cInterpLine m_NitrousPitchBoost;   // offset 0x44, size 0x1C
};

#endif
