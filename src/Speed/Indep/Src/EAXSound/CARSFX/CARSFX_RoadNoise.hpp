//
//
//
#ifndef CARSFX_ROADNOISE_H
#define CARSFX_ROADNOISE_H

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Skids.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/ENVIRO_AEMS.h"

// total size: 0x80
// Decl: 16
class CARSFX_RoadNoise : public CARSFX {
  public:
    DECLARE_CREATABLE();
    CARSFX_RoadNoise();
    ~CARSFX_RoadNoise() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void Detach() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    void GenerateRoadNoise();
    eVOL_ROADNOISE MapLoopToVolume(FXROADNOISE_LOOP ID);
    void Play(FXROADNOISE_LOOP ID, int side);
    void PlayTransition(FXROADNOISE_TRANSITION ID, int side);

    SFXCTL_Wheel *m_pWheelCtl;                // offset 0x28, size 0x4, Decl: 41
    SFXCTL_3DLeftWheelPos *m_pLeftWheelPos;   // offset 0x2C, size 0x4, Decl: 42
    SFXCTL_3DRightWheelPos *m_pRightWheelPos; // offset 0x30, size 0x4, Decl: 43

    DriverTypeEnum m_eDriverType; // offset 0x34, size 0x4, Decl: 46

    Csis::FX_ROADNOISE *m_pWetRoad[2];          // offset 0x38, size 0x8, Decl: 49
    Csis::FX_ROADNOISE *m_pRoadNoiseControl[2]; // offset 0x40, size 0x8, Decl: 50
    cStitchLoop *m_pStitchLoopControl[2];       // offset 0x48, size 0x8, Decl: 51

    Csis::FX_ROADNOISE_TRANS *m_pTransition[2]; // offset 0x50, size 0x8, Decl: 54

    cStichWrapper *m_pStitchTransition[2]; // offset 0x58, size 0x8, Decl: 56

    int TransitionVol[2]; // offset 0x60, size 0x8, Decl: 61

    FXROADNOISE_LOOP LoopID[2]; // offset 0x68, size 0x8, Decl: 63

    int m_nRTRoadNoiseVol; // offset 0x70, size 0x4, Decl: 65
    int m_nLTRoadNoiseVol; // offset 0x74, size 0x4, Decl: 66

    int m_nRTRoadNoisePitch; // offset 0x78, size 0x4, Decl: 68
    int m_nLTRoadNoisePitch; // offset 0x7C, size 0x4, Decl: 69
};

#endif
