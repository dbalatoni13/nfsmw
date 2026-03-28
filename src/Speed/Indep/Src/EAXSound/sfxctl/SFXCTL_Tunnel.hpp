#ifndef EAXSOUND_SFXCTL_SFXCTL_TUNNEL_H
#define EAXSOUND_SFXCTL_SFXCTL_TUNNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"

enum eREVERBFX {
    RVRB_CITYOPEN = 0,
    RVRB_SIMPLE_TUNNEL = 1,
};

struct SFXCTL_Tunnel : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }
    static eTrackPathZoneType m_PlayerZoneType;

  public:
    SFXCTL_Tunnel();
    /* 0x28 */ bool m_bIsInTunnel;
    /* 0x2c */ bool m_bWasInTunnel;
    /* 0x30 */ float m_LastOcclusionTest;
    /* 0x34 */ bool IsOccluded;
    /* 0x38 */ eTrackPathZoneType FutureZoneType;
    /* 0x3c */ eTrackPathZoneType CurZoneType;
    /* 0x40 */ bool bPlayDriveBy;
    /* 0x44 */ bVector3 vDriveByLoc;
    /* 0x54 */ float m_fIntensity;
    /* 0x58 */ float tTimeToWaitBeforeAnotherDriveBy;
    /* 0x5c */ TrackPathZone *pLastZoneWePlayedWooshFor;
    /* 0x60 */ bool bPlayTunnelExit;
    /* 0x64 */ float m_fExitIntensity;
    /* 0x68 */ float tTimeToWaitBeforeAnotherExitDriveBy;
    /* 0x6c */ TrackPathZone *pLastZoneWePlayedExitWooshFor;
    /* 0x70 */ bool bIsTunnelRamping;
    /* 0x74 */ int m_PrevReverbZone;
    /* 0x78 */ int m_CurReverbZone;
    /* 0x7c */ cInterpLine ReflRamp;
    /* 0x98 */ bool bFadingOut;
    /* 0x9c */ bool bFadingIn;
    /* 0xa0 */ bool bIsReadyForSwitch;
    /* 0xa4 */ bool m_IsLeadCar;
    /* 0xa8 */ eREVERBFX m_ReverbType;
    /* 0xac */ float m_ReverbOffset;
    /* 0xb0 */ eREVERBFX m_TargetType;
    /* 0xb4 */ float m_CurWetGinsu;
    /* 0xb8 */ float m_CurDryGinsu;
    /* 0xbc */ float m_CurWetAems;
    /* 0xc0 */ float m_CurDryAems;
    /* 0xc4 */ float m_CurWetGinsuTarget;
    /* 0xc8 */ float m_CurWetAemsTarget;
    /* 0xcc */ float m_CurDryGinsuTarget;
    /* 0xd0 */ float m_CurDryAemsTarget;
    /* 0xd4 */ int m_GinsuWetVol;
    /* 0xd8 */ int m_GinsuDryVol;
    /* 0xdc */ int m_AEMSWetVol;
    /* 0xe0 */ int m_AEMSDryVol;
    /* 0xe4 */ bool bToggleOffset;

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
