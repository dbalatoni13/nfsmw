#ifndef EAXSOUND_CARSFX_CARSFX_WINDNOISE_H
#define EAXSOUND_CARSFX_CARSFX_WINDNOISE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct FX_WIND;
struct FX_WIND_Weather;

// total size: 0x8
struct stEAX_WindParams {
    int nCurrentChannel;     // offset 0x0, size 0x4
    int nCrossFadeWeight;    // offset 0x4, size 0x4
};

// total size: 0x64
struct SFXCTL_3DLeftWindPos : public SFXCTL_3DObjPos {
  protected:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    SFXCTL_3DLeftWindPos() {}
    ~SFXCTL_3DLeftWindPos() override {}

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;

    static SndBase *CreateObject(unsigned int allocator);
};

// total size: 0x64
struct SFXCTL_3DRightWindPos : public SFXCTL_3DObjPos {
  protected:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    SFXCTL_3DRightWindPos() {}
    ~SFXCTL_3DRightWindPos() override {}

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;

    static SndBase *CreateObject(unsigned int allocator);
};

// total size: 0x124
struct CARSFX_WindNoise : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    bool IsInitialized;                        // offset 0x28, size 0x1
    bool m_bIsInSpacialTranslation;           // offset 0x2C, size 0x1
    bVector3 m_v3FinalLeftPos;                // offset 0x30, size 0x10
    bVector3 m_v3FinalRightPos;               // offset 0x40, size 0x10
    bVector3 m_v3StartLeftPos;                // offset 0x50, size 0x10
    bVector3 m_v3StartRightPos;               // offset 0x60, size 0x10
    bVector3 m_v3CurLeftPos;                  // offset 0x70, size 0x10
    bVector3 m_v3CurRightPos;                 // offset 0x80, size 0x10
    bVector3 m_v3CarBaseLeftPos;              // offset 0x90, size 0x10
    bVector3 m_v3CarBaseRightPos;             // offset 0xA0, size 0x10
    bVector3 m_v3LeftDeltaTrans;              // offset 0xB0, size 0x10
    bVector3 m_v3RightDeltaTrans;             // offset 0xC0, size 0x10
    float m_fLeftDistTravel;                  // offset 0xD0, size 0x4
    float m_fRightDistTravel;                 // offset 0xD4, size 0x4
    float m_fmsTimeToTravelLeftDist;          // offset 0xD8, size 0x4
    float m_fmsTimeToTravelRightDist;         // offset 0xDC, size 0x4
    bool m_bReturnFromLeft;                   // offset 0xE0, size 0x1
    bool m_bReturnFromRight;                  // offset 0xE4, size 0x1
    float m_fTimeEffectStartLeft;             // offset 0xE8, size 0x4
    float m_fTimeEffectStartRight;            // offset 0xEC, size 0x4
    float m_fmsTickDiff;                      // offset 0xF0, size 0x4
    unsigned int m_nLastTickUpdate;           // offset 0xF4, size 0x4
    bAngle m_angleTravelLeft;                 // offset 0xF8, size 0x2
    bAngle m_angleTravelRight;                // offset 0xFA, size 0x2
    float m_fStartVelocityMag;                // offset 0xFC, size 0x4
    float m_fcurwindradius;                   // offset 0x100, size 0x4
    int m_nVelocityWeightedVolume;            // offset 0x104, size 0x4
    FX_WIND *m_pCsisWind;                     // offset 0x108, size 0x4
    stEAX_WindParams m_stWindParams[2];       // offset 0x10C, size 0x10
    SFXCTL_3DLeftWindPos *m_p3DLeftWindPos;   // offset 0x11C, size 0x4
    SFXCTL_3DRightWindPos *m_p3DRightWindPos; // offset 0x120, size 0x4

    CARSFX_WindNoise();
    ~CARSFX_WindNoise() override;

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;

    static SndBase *CreateObject(unsigned int allocator);

    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    virtual int UpdateMasterVolume();

    void UpdateCSISParams();
};

// total size: 0x38
struct CARSFX_WindWeather : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    float WeatherIntensity;       // offset 0x28, size 0x4
    float WeatherSpeedScale;      // offset 0x2C, size 0x4
    FX_WIND_Weather *m_pcsisWind; // offset 0x30, size 0x4
    int refCnt;                   // offset 0x34, size 0x4

    CARSFX_WindWeather();
    ~CARSFX_WindWeather() override;

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;

    static SndBase *CreateObject(unsigned int allocator);

    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    virtual void Play();
    void ProcessUpdate() override;
};
#endif
