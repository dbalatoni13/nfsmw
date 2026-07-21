//
//
//
#ifndef CARSFX_RAIN_H
#define CARSFX_RAIN_H

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/MAIN_AEMS.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"

// total size: 0x50
// Decl: 13
class CARSFX_Rain : public CARSFX {
  public:
    DECLARE_CREATABLE();

    CARSFX_Rain();
    ~CARSFX_Rain() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void UpdateMixerOutputs() override;

    virtual void QueueWeatherStream();
    static void PlayWeatherStream();
    void Play();
    void Stop();

    // Overrides: SndBase
    void Destroy() override;

    float m_fWeatherIntensity;     // offset 0x28, size 0x4, Decl: 39
    float m_fPrevWeatherIntensity; // offset 0x2C, size 0x4, Decl: 40
    float m_fThunderTime;          // offset 0x30, size 0x4, Decl: 41
    float m_fThunderDeltaTime;     // offset 0x34, size 0x4, Decl: 42
    bool m_bWeatherStreamQueued;   // offset 0x38, size 0x1, Decl: 43
    int m_BlockStrmTest;           // offset 0x3C, size 0x4, Decl: 44

    float m_fTimeLeftToFadeOut;    // offset 0x40, size 0x4, Decl: 51
    bool bFadingOut;               // offset 0x44, size 0x1, Decl: 52
    Csis::FX_Weather *m_pCsisRain; // offset 0x48, size 0x4, Decl: 53

    SFXCTL_Tunnel *m_pTunnelCtl; // offset 0x4C, size 0x4, Decl: 55
};

#endif
