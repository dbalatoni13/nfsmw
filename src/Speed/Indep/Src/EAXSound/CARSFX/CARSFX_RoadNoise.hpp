#ifndef EAXSOUND_CARSFX_CARSFX_ROADNOISE_H
#define EAXSOUND_CARSFX_CARSFX_ROADNOISE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/simsurface.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"

struct SFXCTL_Wheel;
struct SFXCTL_3DLeftWheelPos;
struct SFXCTL_3DRightWheelPos;
struct FX_ROADNOISE;
struct FX_ROADNOISE_TRANS;
struct cStitchLoop;
struct cStichWrapper;

enum eVOL_ROADNOISE {
    eAZI_ROADNOISE_LEFT_AZ = 0,
    eAZI_ROADNOISE_RIGHT_AZ = 1,
    eVOL_ROADNOISE_TRANSITION = 2,
    eVOL_ROADNOISE_WET_ROAD = 3,
    eVOL_ROADNOISE_STITCH_TRANS = 4,
    eVOL_ROADNOISE_GRAVEL = 5,
    eVOL_ROADNOISE_SIDEWALK = 6,
    eVOL_ROADNOISE_COBBLESTONE = 7,
    eVOL_ROADNOISE_DEEPWATER = 8,
    eVOL_ROADNOISE_WETROAD = 9,
    eVOL_ROADNOISE_ASHPHALT = 10,
    eVOL_ROADNOISE_METAL = 11,
    eVOL_ROADNOISE_STITCH_LOOP = 12,
    eVOL_ROADNOISE_SPIKE = 13,
    eVOL_ROADNOISE_BLOWN = 14,
    eVRB_ROADNOISE_VERB = 15,
};

class CARSFX_RoadNoise : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    CARSFX_RoadNoise();
    ~CARSFX_RoadNoise() override;

    static SndBase *CreateObject(unsigned int allocator);
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void Destroy() override;
    void Detach() override;

    eVOL_ROADNOISE MapLoopToVolume(FXROADNOISE_LOOP ID);
    void PlayTransition(FXROADNOISE_TRANSITION TransitionID, int n);
    void GenerateRoadNoise();
    void Play(FXROADNOISE_LOOP ID, int n);

    SFXCTL_Wheel *m_pWheelCtl;                 // offset 0x28, size 0x4
    SFXCTL_3DLeftWheelPos *m_pLeftWheelPos;   // offset 0x2C, size 0x4
    SFXCTL_3DRightWheelPos *m_pRightWheelPos; // offset 0x30, size 0x4
    DriverTypeEnum m_eDriverType;             // offset 0x34, size 0x4
    FX_ROADNOISE *m_pWetRoad[2];              // offset 0x38, size 0x8
    FX_ROADNOISE *m_pRoadNoiseControl[2];     // offset 0x40, size 0x8
    cStitchLoop *m_pStitchLoopControl[2];     // offset 0x48, size 0x8
    FX_ROADNOISE_TRANS *m_pTransition[2];     // offset 0x50, size 0x8
    cStichWrapper *m_pStitchTransition[2];    // offset 0x58, size 0x8
    int TransitionVol[2];                     // offset 0x60, size 0x8
    FXROADNOISE_LOOP LoopID[2];               // offset 0x68, size 0x8
    int m_nRTRoadNoiseVol;                    // offset 0x70, size 0x4
    int m_nLTRoadNoiseVol;                    // offset 0x74, size 0x4
    int m_nRTRoadNoisePitch;                  // offset 0x78, size 0x4
    int m_nLTRoadNoisePitch;                  // offset 0x7C, size 0x4
};

#endif
