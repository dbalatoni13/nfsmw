#ifndef CARSFX_SKIDS_H
#define CARSFX_SKIDS_H

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/MAIN_AEMS.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"

// total size: 0x64
class SFXCTL_3DLeftWheelPos : public SFXCTL_3DObjPos {
  public:
    DECLARE_CREATABLE();

    SFXCTL_3DLeftWheelPos() {}

    ~SFXCTL_3DLeftWheelPos() override {}
};

// total size: 0x64
class SFXCTL_3DRightWheelPos : public SFXCTL_3DObjPos {
  public:
    DECLARE_CREATABLE();

    SFXCTL_3DRightWheelPos() {}

    ~SFXCTL_3DRightWheelPos() override {}
};

// total size: 0x48
class CARSFX_Skids : public CARSFX {
  public:
    DECLARE_CREATABLE();

    CARSFX_Skids();
    ~CARSFX_Skids() override;

    // Overrides: SndBase
    void SetupLoadData() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void Detach() override;
    void UpdateMixerOutputs() override;
    void ProcessUpdate() override;

    Csis::FX_SKID *m_pSkidControl;            // offset 0x28, size 0x4
    int SkidType;                             // offset 0x2C, size 0x4
    int m_SkidAzimuth[2];                     // offset 0x30, size 0x8
    SFXCTL_Wheel *m_pWheelCtl;                // offset 0x38, size 0x4
    SFXCTL_3DLeftWheelPos *m_pLeftWheelPos;   // offset 0x3C, size 0x4
    SFXCTL_3DRightWheelPos *m_pRightWheelPos; // offset 0x40, size 0x4
    bool mDataLoaded;                         // offset 0x44, size 0x1
};

// total size: 0x48
class CARSFX_TrafficSkids : public CARSFX_Skids {
  public:
    DECLARE_CREATABLE();

    CARSFX_TrafficSkids();
    ~CARSFX_TrafficSkids() override;

    // Overrides: SndBase
    void Detach() override;
    int GetController(int Index) override;
};

#endif
