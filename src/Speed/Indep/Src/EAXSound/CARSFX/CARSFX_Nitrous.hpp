//
//
//
#ifndef CARSFX_NITROUS_H
#define CARSFX_NITROUS_H

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/MAIN_AEMS.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"

// total size: 0x60
// Decl: 10
class CARSFX_Nitrous : public CARSFX {
  public:
    DECLARE_CREATABLE();

    CARSFX_Nitrous();
    ~CARSFX_Nitrous() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void SetupLoadData() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    float GetNOSPitchBoost() {} // Decl: 27

    int Play(int type, int Vol, int Azimuth);
    void Stop();

    // Overrides: SndBase
    void Destroy() override;

    void PlayPurge();
    void StopPurge();

    static void NOSStreamCued();

    Csis::FX_NITROUS *m_NitrousControl; // offset 0x28, size 0x4, Decl: 33
    Csis::FX_PURGE *m_NitrousPurge;     // offset 0x2C, size 0x4, Decl: 34
    eSFX_NITROUS eNitrousState;         // offset 0x30, size 0x4, Decl: 35
    int m_NitrousPlayHandle;            // offset 0x34, size 0x4, Decl: 36
    int CurVol;                         // offset 0x38, size 0x4, Decl: 37

    SFXCTL_3DCarPos *m_p3DCarPosCtl; // offset 0x3C, size 0x4, Decl: 39
    SFXCTL_Shifting *m_pShiftCtl;    // offset 0x40, size 0x4, Decl: 40

    cInterpLine m_NitrousPitchBoost; // offset 0x44, size 0x1C, Decl: 44
};

#endif
