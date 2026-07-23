//
#ifndef CARSFX_ENGINE_H
#define CARSFX_ENGINE_H

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/Ginsu/ginsu.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_HybridMotor.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/ENGINES_AEMS2.h"

#define NUM_NIS_REV_SEQUENCES 2 // Decl: 49
#define SIZE_REV_DATA_SETS 31   // Decl: 50
#define REV_TIME 0              // Decl: 51
#define REV_RPM 1               // Decl: 52
#define REV_TRQ 2               // Decl: 53
#define NUM_FRAMES_DELAY 2      // Decl: 54

// total size: 0x40
// Decl: 59
class CARSFX_EngineBase : public CARSFX {
  public:
    CARSFX_EngineBase();
    ~CARSFX_EngineBase() override;

    // Overrides: SndBase
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void Detach() override;

    virtual void InitializeEngine();
    virtual void SetEngineParams();
    bool IsEngineFinishedLoading();

    Csis::CAR *m_pcsisCarCtrl; // offset 0x28, size 0x4, Decl: 78

    int SPU_or_EE;               // offset 0x2C, size 0x4, Decl: 85
    Csis::CAR_TRANNY *m_pTranny; // offset 0x30, size 0x4, Decl: 86

    SFXCTL_3DCarPos *m_p3DCarPosCtl; // offset 0x34, size 0x4, Decl: 88
    SFXCTL_Engine *m_pEngineCtl;     // offset 0x38, size 0x4, Decl: 89

    float PitchMultipli; // offset 0x3C, size 0x4, Decl: 93

    virtual void Debug() {} // Decl: 96
};

// total size: 0x44
// Decl: 105
class CARSFX_AEMSEngine : public CARSFX_EngineBase {
  public:
    DECLARE_CREATABLE();

    CARSFX_AEMSEngine();
    ~CARSFX_AEMSEngine() override;

    // Overrides: SndBase
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void SetupLoadData() override;
    void UpdateParams(float t) override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;

    // Overrides: CARSFX_EngineBase
    void SetEngineParams() override;

    // Overrides: SndBase
    void InitSFX() override;

    int m_iAIPitchOffset; // offset 0x40, size 0x4, Decl: 123
};

// total size: 0x90
// Decl: 132
class CARSFX_GinsuEngine : public CARSFX_EngineBase {
  public:
    CARSFX_GinsuEngine();
    ~CARSFX_GinsuEngine() override;

    // Overrides: SndBase
    void SetupSFX(CSTATE_Base *_StateBase) override;

    // Overrides: SndBase
    void InitSFX() override;

    // Overrides: CARSFX_EngineBase
    void InitializeEngine() override;
    void SetEngineParams() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;

    void StartupGinsu();

    SFXCTL_HybridMotor *m_pHybridEngCtl; // offset 0x40, size 0x4, Decl: 149

    SFXCTL_Tunnel *m_pTunnelCtl;     // offset 0x44, size 0x4, Decl: 151
    SFXCTL_Shifting *m_pShiftingCtl; // offset 0x48, size 0x4, Decl: 152

    bool GinsuInitialized; // offset 0x4C, size 0x1, Decl: 155
    int InitCnt;           // offset 0x50, size 0x4, Decl: 156

    // Decl: 159
    struct stGinsuData {
        stGinsuData()
            : mSynthData(nullptr),  //
              mSynth(nullptr),      //
              mSynthBlock(nullptr), //
              mMaxFrequency(0.0f),  //
              mMinFrequency(0.0f),  //
              mSNDhandle(-1) {}     // Decl: 160

        GinsuSynthData *mSynthData; // offset 0x0, size 0x4, Decl: 170
        GinsuSynthesis *mSynth;     // offset 0x4, size 0x4, Decl: 171
        void *mSynthBlock;          // offset 0x8, size 0x4, Decl: 172
        float mMaxFrequency;        // offset 0xC, size 0x4, Decl: 173
        float mMinFrequency;        // offset 0x10, size 0x4, Decl: 174
        int mSNDhandle;             // offset 0x14, size 0x4, Decl: 175
    } m_GinsuData[2];               // offset 0x54, size 0x30, Decl: 176

    virtual void SetGinsuParams() = 0; // Decl: 178

    float m_GinsuRPM;    // offset 0x84, size 0x4, Decl: 181
    int m_GinsuAccelVol; // offset 0x88, size 0x4
    int m_GinsuDecelVol; // offset 0x8C, size 0x4

    // Overrides: SndBase
    void Detach() override;
};

// total size: 0x90
// Decl: 196
class CARSFX_DualGinsuEng : public CARSFX_GinsuEngine {
  public:
    DECLARE_CREATABLE();

    CARSFX_DualGinsuEng();
    ~CARSFX_DualGinsuEng() override;

    // Overrides: CARSFX_GinsuEngine
    void SetGinsuParams() override;

    // Overrides: SndBase
    void SetupLoadData() override;
};

// total size: 0x90
// Decl: 208
class CARSFX_SingleGinsuEng : public CARSFX_GinsuEngine {
  public:
    DECLARE_CREATABLE();

    CARSFX_SingleGinsuEng();
    ~CARSFX_SingleGinsuEng() override;

    // Overrides: CARSFX_GinsuEngine
    void SetGinsuParams() override;

    // Overrides: SndBase
    void SetupLoadData() override;
};

#endif
