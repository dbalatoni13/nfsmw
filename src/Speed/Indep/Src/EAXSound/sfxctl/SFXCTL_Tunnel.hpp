#ifndef EAXSOUND_SFXCTL_SFXCTL_TUNNEL_H
#define EAXSOUND_SFXCTL_SFXCTL_TUNNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"

enum eREVERBFX {
    RVRB_CARSHOW_SML = 0,
    RVRB_CARSHOW_BASS = 1,
    RVRB_CARSHOW_BASS_2 = 2,
    RVRB_GARAGE = 3,
    RVRB_GARAGE_SML = 4,
    RVRB_SIMPLE_TUNNEL = 5,
    RVRB_SIMPLE_TUNNEL_SML = 6,
    RVRB_HILLS_CLOSE = 7,
    RVRB_HILLS = 8,
    RVRB_CITYOPEN = 9,
    RVRB_CITYDENSE = 10,
    RVRB_ALLEY = 11,
    RVRB_MAX_TYPES = 12,
};

struct stREVERB_PARAMS {
    int GinsuWet;  // offset 0x0, size 0x4
    int GinsuDry;  // offset 0x4, size 0x4
    int AemsWet;   // offset 0x8, size 0x4
    int AemsDry;   // offset 0xC, size 0x4
    int FadeOut;   // offset 0x10, size 0x4
    int FadeIn;    // offset 0x14, size 0x4
};

extern stREVERB_PARAMS g_REVERBFXMODULES[];

struct SFXCTL_Tunnel : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }
    static eTrackPathZoneType m_PlayerZoneType;

  public:
    SFXCTL_Tunnel();
    bool m_bIsInTunnel;                         // offset 0x28, size 0x1
    bool m_bWasInTunnel;                        // offset 0x2C, size 0x1
    float m_LastOcclusionTest;                  // offset 0x30, size 0x4
    bool IsOccluded;                            // offset 0x34, size 0x1
    eTrackPathZoneType FutureZoneType;          // offset 0x38, size 0x4
    eTrackPathZoneType CurZoneType;             // offset 0x3C, size 0x4
    bool bPlayDriveBy;                          // offset 0x40, size 0x1
    bVector3 vDriveByLoc;                       // offset 0x44, size 0x10
    float m_fIntensity;                         // offset 0x54, size 0x4
    float tTimeToWaitBeforeAnotherDriveBy;      // offset 0x58, size 0x4
    TrackPathZone *pLastZoneWePlayedWooshFor;   // offset 0x5C, size 0x4
    bool bPlayTunnelExit;                       // offset 0x60, size 0x1
    float m_fExitIntensity;                     // offset 0x64, size 0x4
    float tTimeToWaitBeforeAnotherExitDriveBy;  // offset 0x68, size 0x4
    TrackPathZone *pLastZoneWePlayedExitWooshFor; // offset 0x6C, size 0x4
    bool bIsTunnelRamping;                      // offset 0x70, size 0x1
    int m_PrevReverbZone;                       // offset 0x74, size 0x4
    int m_CurReverbZone;                        // offset 0x78, size 0x4
    cInterpLine ReflRamp;                       // offset 0x7C, size 0x1C
    bool bFadingOut;                            // offset 0x98, size 0x1
    bool bFadingIn;                             // offset 0x9C, size 0x1
    bool bIsReadyForSwitch;                     // offset 0xA0, size 0x1
    bool m_IsLeadCar;                           // offset 0xA4, size 0x1
    eREVERBFX m_ReverbType;                     // offset 0xA8, size 0x4
    float m_ReverbOffset;                       // offset 0xAC, size 0x4
    eREVERBFX m_TargetType;                     // offset 0xB0, size 0x4
    float m_CurWetGinsu;                        // offset 0xB4, size 0x4
    float m_CurDryGinsu;                        // offset 0xB8, size 0x4
    float m_CurWetAems;                         // offset 0xBC, size 0x4
    float m_CurDryAems;                         // offset 0xC0, size 0x4
    float m_CurWetGinsuTarget;                  // offset 0xC4, size 0x4
    float m_CurWetAemsTarget;                   // offset 0xC8, size 0x4
    float m_CurDryGinsuTarget;                  // offset 0xCC, size 0x4
    float m_CurDryAemsTarget;                   // offset 0xD0, size 0x4
    int m_GinsuWetVol;                          // offset 0xD4, size 0x4
    int m_GinsuDryVol;                          // offset 0xD8, size 0x4
    int m_AEMSWetVol;                           // offset 0xDC, size 0x4
    int m_AEMSDryVol;                           // offset 0xE0, size 0x4
    bool bToggleOffset;                         // offset 0xE4, size 0x1

    ~SFXCTL_Tunnel() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void InitSFX() override;
    void UpdateParams(float t) override;
    void UpdateMixerOutputs() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void Destroy() override;

    TrackPathZone *GetTunnelType(bVector3 &pos, eTrackPathZoneType zonetype);
    void UpdateIsInTunnel(float t);
    void EndTunnelVerb();
    void UpdateDriveBySnds(float t);
    void UpdateOcclusion(float t);
    void SetCurrentReverbType(eREVERBFX type, int reverboffset);
    void UpdateCityVerb(float t);
    void AdjustReverbOffset(int reverboffset);
    void UpdateReflectionParams(float t);
};

#endif
