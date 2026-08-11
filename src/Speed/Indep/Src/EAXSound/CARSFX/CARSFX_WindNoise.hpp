//
#ifndef CARSFX_WINDNOISE_H
#define CARSFX_WINDNOISE_H

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/ENVIRO_AEMS.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

// total size: 0x64
// Decl: 10
class SFXCTL_3DRightWindPos : public SFXCTL_3DObjPos {
  public:
    DECLARE_CREATABLE();
    SFXCTL_3DRightWindPos() {}           // Decl: 13
    ~SFXCTL_3DRightWindPos() override {} // Decl: 14
};

// total size: 0x64
// Decl: 18
class SFXCTL_3DLeftWindPos : public SFXCTL_3DObjPos {
  public:
    DECLARE_CREATABLE();
    SFXCTL_3DLeftWindPos() {}           // Decl: 21
    ~SFXCTL_3DLeftWindPos() override {} // Decl: 22
};

// total size: 0x8
// Decl: 26
struct stEAX_WindParams {
    int nCurrentChannel;  // offset 0x0, size 0x4, Decl: 27
    int nCrossFadeWeight; // offset 0x4, size 0x4, Decl: 28
};

// total size: 0x130
// Decl: 33
class CARSFX_WindNoise : public CARSFX {
  public:
    DECLARE_CREATABLE();
    CARSFX_WindNoise();
    ~CARSFX_WindNoise() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    void SetupSoundSphere(bVector2 *pv2trpdir, float fvelnorm);

    void UpdateCSISParams();

    virtual int UpdateMasterVolume();

    bool IsInitialized;             // offset 0x28, size 0x1, Decl: 56
    bool m_bIsInSpacialTranslation; // offset 0x2C, size 0x1, Decl: 57

    ALIGNVEC bVector3 m_v3FinalLeftPos;  // offset 0x30, size 0x10, Decl: 59
    ALIGNVEC bVector3 m_v3FinalRightPos; // offset 0x40, size 0x10, Decl: 60

    ALIGNVEC bVector3 m_v3StartLeftPos;  // offset 0x50, size 0x10, Decl: 62
    ALIGNVEC bVector3 m_v3StartRightPos; // offset 0x60, size 0x10, Decl: 63

    ALIGNVEC bVector3 m_v3CurLeftPos;  // offset 0x70, size 0x10, Decl: 65
    ALIGNVEC bVector3 m_v3CurRightPos; // offset 0x80, size 0x10, Decl: 66

    ALIGNVEC bVector3 m_v3CarBaseLeftPos;  // offset 0x90, size 0x10, Decl: 68
    ALIGNVEC bVector3 m_v3CarBaseRightPos; // offset 0xA0, size 0x10, Decl: 69

    ALIGNVEC bVector3 m_v3LeftDeltaTrans;  // offset 0xB0, size 0x10, Decl: 71
    ALIGNVEC bVector3 m_v3RightDeltaTrans; // offset 0xC0, size 0x10, Decl: 72

    float m_fLeftDistTravel;  // offset 0xD0, size 0x4, Decl: 74
    float m_fRightDistTravel; // offset 0xD4, size 0x4, Decl: 75

    float m_fmsTimeToTravelLeftDist;  // offset 0xD8, size 0x4, Decl: 77
    float m_fmsTimeToTravelRightDist; // offset 0xDC, size 0x4, Decl: 78

    bool m_bReturnFromLeft;  // offset 0xE0, size 0x1, Decl: 80
    bool m_bReturnFromRight; // offset 0xE4, size 0x1, Decl: 81

    float m_fTimeEffectStartLeft;  // offset 0xE8, size 0x4, Decl: 83
    float m_fTimeEffectStartRight; // offset 0xEC, size 0x4, Decl: 84
    float m_fmsTickDiff;           // offset 0xF0, size 0x4, Decl: 85
    uint32 m_nLastTickUpdate;      // offset 0xF4, size 0x4, Decl: 86
    bAngle m_angleTravelLeft;      // offset 0xF8, size 0x2, Decl: 87
    bAngle m_angleTravelRight;     // offset 0xFA, size 0x2, Decl: 88
    float m_fStartVelocityMag;     // offset 0xFC, size 0x4, Decl: 90
    float m_fcurwindradius;        // offset 0x100, size 0x4, Decl: 91

    int m_nVelocityWeightedVolume; // offset 0x104, size 0x4, Decl: 93

    Csis::FX_WIND *m_pCsisWind;                // offset 0x108, size 0x4, Decl: 95
    struct stEAX_WindParams m_stWindParams[2]; // offset 0x10C, size 0x10, Decl: 96

    SFXCTL_3DLeftWindPos *m_p3DLeftWindPos;   // offset 0x11C, size 0x4, Decl: 98
    SFXCTL_3DRightWindPos *m_p3DRightWindPos; // offset 0x120, size 0x4, Decl: 99
};

// total size: 0x38
// Decl: 108
class CARSFX_WindWeather : public CARSFX {
  public:
    DECLARE_CREATABLE();
    CARSFX_WindWeather();
    ~CARSFX_WindWeather() override;

    // Overrides: SndBase
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;

    virtual void Play();

    // Overrides: SndBase
    void ProcessUpdate() override;

    float WeatherIntensity;  // offset 0x28, size 0x4, Decl: 125
    float WeatherSpeedScale; // offset 0x2C, size 0x4, Decl: 126

    Csis::FX_WIND_Weather *m_pcsisWind; // offset 0x30, size 0x4, Decl: 130

    int refCnt; // offset 0x34, size 0x4, Decl: 132
};

#endif
