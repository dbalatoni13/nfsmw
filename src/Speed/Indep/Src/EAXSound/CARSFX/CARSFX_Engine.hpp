#ifndef EAXSOUND_CARSFX_CARSFX_ENGINE_H
#define EAXSOUND_CARSFX_CARSFX_ENGINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/Ginsu/ginsu.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_HybridMotor.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/ENGINES_AEMS2.h"

struct CARSFX_EngineBase : public CARSFX {
    CARSFX_EngineBase();
    ~CARSFX_EngineBase() override;
    void Detach() override;
    void UpdateParams(float t) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void ProcessUpdate() override;

    bool IsEngineFinishedLoading();
    virtual void InitializeEngine();
    virtual void SetEngineParams();
    virtual void Debug();

    CAR *m_pcsisCarCtrl;                   // offset 0x28, size 0x4
    int SPU_or_EE;                         // offset 0x2C, size 0x4
    CAR_TRANNY *m_pTranny;                 // offset 0x30, size 0x4
    SFXCTL_3DCarPos *m_p3DCarPosCtl;       // offset 0x34, size 0x4
    SFXCTL_Engine *m_pEngineCtl;           // offset 0x38, size 0x4
    float PitchMultipli;                   // offset 0x3C, size 0x4
};

struct CARSFX_GinsuEngine : public CARSFX_EngineBase {
    struct stGinsuData {
        GinsuSynthData *mSynthData;        // offset 0x0, size 0x4
        GinsuSynthesis *mSynth;            // offset 0x4, size 0x4
        void *mSynthBlock;                 // offset 0x8, size 0x4
        float mMaxFrequency;               // offset 0xC, size 0x4
        float mMinFrequency;               // offset 0x10, size 0x4
        int mSNDhandle;                    // offset 0x14, size 0x4
    };

    CARSFX_GinsuEngine();
    ~CARSFX_GinsuEngine() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Detach() override;

    void StartupGinsu();
    void InitializeEngine() override;
    void SetEngineParams() override;
    virtual void SetGinsuParams();

    SFXCTL_HybridMotor *m_pHybridEngCtl;   // offset 0x40, size 0x4
    SFXCTL_Tunnel *m_pTunnelCtl;           // offset 0x44, size 0x4
    SFXCTL_Shifting *m_pShiftingCtl;       // offset 0x48, size 0x4
    bool GinsuInitialized;                 // offset 0x4C, size 0x1
    int InitCnt;                           // offset 0x50, size 0x4
    stGinsuData m_GinsuData[2];            // offset 0x54, size 0x30
    float m_GinsuRPM;                      // offset 0x84, size 0x4
    int m_GinsuAccelVol;                   // offset 0x88, size 0x4
    int m_GinsuDecelVol;                   // offset 0x8C, size 0x4
};

struct CARSFX_AEMSEngine : public CARSFX_EngineBase {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    CARSFX_AEMSEngine();
    ~CARSFX_AEMSEngine() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void UpdateParams(float t) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetEngineParams() override;
    void SetupLoadData() override;

    int m_iAIPitchOffset;                  // offset 0x40, size 0x4
};

struct CARSFX_SingleGinsuEng : public CARSFX_GinsuEngine {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    CARSFX_SingleGinsuEng();
    ~CARSFX_SingleGinsuEng() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void SetGinsuParams() override;
    void SetupLoadData() override;
};

struct CARSFX_DualGinsuEng : public CARSFX_GinsuEngine {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    CARSFX_DualGinsuEng();
    ~CARSFX_DualGinsuEng() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void SetGinsuParams() override;
    void SetupLoadData() override;
};

#endif
