//
#ifndef SFXCTL_TUNNEL_H
#define SFXCTL_TUNNEL_H

#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/World/TrackPath.hpp"

// total size: 0xE8
// Decl: 31
class SFXCTL_Tunnel : public SFXCTL {
    DECLARE_CREATABLE();

  public:
    SFXCTL_Tunnel();
    ~SFXCTL_Tunnel() override;

    // Overrides: SndBase
    void UpdateParams(float t) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void UpdateMixerOutputs() override;

    static eTrackPathZoneType m_PlayerZoneType; // size: 0x4, address: 0x80417A08, Decl: 49

    void SetCurrentReverbType(eREVERBFX ereverbtype, int reverboffset);
    void AdjustReverbOffset(int reverboffset);

    bool IsInTunnel() {}  // Decl: 60
    bool WasInTunnel() {} // Decl: 61
    void UpdateOcclusion(float t);
    void UpdateIsInTunnel(float t);
    bool m_bIsInTunnel;  // offset 0x28, size 0x1, Decl: 64
    bool m_bWasInTunnel; // offset 0x2C, size 0x1, Decl: 65

    float m_LastOcclusionTest; // offset 0x30, size 0x4, Decl: 70

    bool IsOccluded; // offset 0x34, size 0x1, Decl: 71

    TrackPathZone *GetTunnelType(bVector3 &pos, eTrackPathZoneType zonetype);

    eTrackPathZoneType FutureZoneType; // offset 0x38, size 0x4, Decl: 84
    eTrackPathZoneType CurZoneType;    // offset 0x3C, size 0x4, Decl: 85
    void UpdateDriveBySnds(float t);
    bool bPlayDriveBy;                        // offset 0x40, size 0x1, Decl: 87
    bVector3 vDriveByLoc;                     // offset 0x44, size 0x10, Decl: 88
    float m_fIntensity;                       // offset 0x54, size 0x4, Decl: 89
    float tTimeToWaitBeforeAnotherDriveBy;    // offset 0x58, size 0x4, Decl: 90
    TrackPathZone *pLastZoneWePlayedWooshFor; // offset 0x5C, size 0x4, Decl: 91
    void UpdateCityVerb(float t);
    bool bPlayTunnelExit;                         // offset 0x60, size 0x1, Decl: 93
    float m_fExitIntensity;                       // offset 0x64, size 0x4, Decl: 94
    float tTimeToWaitBeforeAnotherExitDriveBy;    // offset 0x68, size 0x4, Decl: 95
    TrackPathZone *pLastZoneWePlayedExitWooshFor; // offset 0x6C, size 0x4, Decl: 96

    bool bIsTunnelRamping; // offset 0x70, size 0x1, Decl: 105

    int m_PrevReverbZone; // offset 0x74, size 0x4, Decl: 108

    int m_CurReverbZone; // offset 0x78, size 0x4, Decl: 109

    cInterpLine ReflRamp; // offset 0x7C, size 0x1C, Decl: 111

    bool IsReadyForSwitch() {} // Decl: 114
    bool bFadingOut;           // offset 0x98, size 0x1, Decl: 115
    bool bFadingIn;            // offset 0x9C, size 0x1, Decl: 116
    bool bIsReadyForSwitch;    // offset 0xA0, size 0x1, Decl: 117

    bool m_IsLeadCar; // offset 0xA4, size 0x1, Decl: 119

    eREVERBFX m_ReverbType; // offset 0xA8, size 0x4, Decl: 123
    float m_ReverbOffset;   // offset 0xAC, size 0x4, Decl: 124
    eREVERBFX m_TargetType; // offset 0xB0, size 0x4, Decl: 125

    float m_CurWetGinsu; // offset 0xB4, size 0x4, Decl: 128
    float m_CurDryGinsu; // offset 0xB8, size 0x4, Decl: 129
    float m_CurWetAems;  // offset 0xBC, size 0x4, Decl: 130
    float m_CurDryAems;  // offset 0xC0, size 0x4, Decl: 131

    float m_CurWetGinsuTarget; // offset 0xC4, size 0x4, Decl: 134
    float m_CurWetAemsTarget;  // offset 0xC8, size 0x4, Decl: 135
    float m_CurDryGinsuTarget; // offset 0xCC, size 0x4, Decl: 136
    float m_CurDryAemsTarget;  // offset 0xD0, size 0x4, Decl: 137

    int m_GinsuWetVol; // offset 0xD4, size 0x4, Decl: 140
    int m_GinsuDryVol; // offset 0xD8, size 0x4, Decl: 141
    int m_AEMSWetVol;  // offset 0xDC, size 0x4, Decl: 142
    int m_AEMSDryVol;  // offset 0xE0, size 0x4, Decl: 143

    bool bToggleOffset; // offset 0xE4, size 0x1, Decl: 145
};

#endif
