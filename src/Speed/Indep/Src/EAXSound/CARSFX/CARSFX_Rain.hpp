#ifndef EAXSOUND_CARSFX_CARSFX_RAIN_H
#define EAXSOUND_CARSFX_CARSFX_RAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/MAIN_AEMS.h"

struct CARSFX_Rain : public CARSFX {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    CARSFX_Rain();
    ~CARSFX_Rain() override;

    static SndBase *CreateObject(unsigned int allocator);

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    virtual void QueueWeatherStream();
    void UpdateParams(float t) override;
    void UpdateMixerOutputs() override;
    void ProcessUpdate() override;

    void Play();
    void Stop();
    static void PlayWeatherStream();

  private:
    float m_fWeatherIntensity;          // offset 0x28, size 0x4
    float m_fPrevWeatherIntensity;      // offset 0x2C, size 0x4
    float m_fThunderTime;               // offset 0x30, size 0x4
    float m_fThunderDeltaTime;          // offset 0x34, size 0x4
    bool m_bWeatherStreamQueued;        // offset 0x38, size 0x1
    int m_BlockStrmTest;                // offset 0x3C, size 0x4
    float m_fTimeLeftToFadeOut;         // offset 0x40, size 0x4
    bool bFadingOut;                    // offset 0x44, size 0x1
    FX_Weather *m_pCsisRain;            // offset 0x48, size 0x4
    SFXCTL_Tunnel *m_pTunnelCtl;        // offset 0x4C, size 0x4
};

#endif
