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

  public:
    SFXCTL_Tunnel();
    /* 0x28 */ char _pad_tun0[0x7c - 0x28];
    /* 0x7c */ cInterpLine ReflRamp;

    ~SFXCTL_Tunnel() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
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
