//
#ifndef CARSFX_TRAFFICFX_HPP
#define CARSFX_TRAFFICFX_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/ENVIRO_AEMS.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"

// total size: 0x38
// Decl: 15
class CARSFX_TrafficEngine : CARSFX {
  public:
    DECLARE_CREATABLE();
    CARSFX_TrafficEngine();
    ~CARSFX_TrafficEngine() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void InitSFX() override;

    void InitEngine();

    // Overrides: SndBase
    void Detach() override;
    void Destroy() override;
    void ProcessUpdate() override;

    Csis::FX_TRAFFIC *m_pcsisTranfficEng; // offset 0x28, size 0x4, Decl: 41
    SFXCTL_3DCarPos *m_p3DCarPosCtl;      // offset 0x2C, size 0x4, Decl: 43
    int refCnt;                           // offset 0x30, size 0x4, Decl: 45
    uint8 EngTypeID;                      // offset 0x34, size 0x1, Decl: 46
};

// total size: 0x64
// Decl: 51
class SFXCTL_3DTrafficPos : SFXCTL_3DCarPos {
  public:
    DECLARE_CREATABLE();
    SFXCTL_3DTrafficPos() {} // Decl: 51
};

// total size: 0x74
// Decl: 58
class CARSFX_TrafficHorn : public CARSFX {
  public:
    DECLARE_CREATABLE();
    CARSFX_TrafficHorn();
    ~CARSFX_TrafficHorn() override;

    // Overrides: SndBase
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void UpdateMixerOutputs() override;
    void Detach() override;

    bool IsPlayerCarInRange(int nplayer);
    void CSIS_UpdateHOnk();
    void CSIS_EndHonk();
    void CSIS_BeginHonk(int ID);
    void EndCarHonk();

    virtual void StartHonkHorn();
    virtual void StopHonkHorn();
    virtual bool IsHonking();

    SFXCTL_3DCarPos *m_p3DCarPosCtl; // offset 0x28, size 0x4, Decl: 79

    Csis::ENV_STATIC *m_HornSound; // offset 0x2C, size 0x4, Decl: 81

    float HornDuration;   // offset 0x30, size 0x4, Decl: 84
    float tHornBegin;     // offset 0x34, size 0x4, Decl: 85
    float tSinceLastHorn; // offset 0x38, size 0x4, Decl: 86

    bool AIPlayingHonk;   // offset 0x3C, size 0x1, Decl: 88
    bool SND_PlayingHonk; // offset 0x40, size 0x1, Decl: 89

    bool ShouldHonk;                 // offset 0x44, size 0x1, Decl: 92
    float tSinceLastAttemptedToHonk; // offset 0x48, size 0x4, Decl: 93

    int refCnt; // offset 0x4C, size 0x4, Decl: 95

    cInterpLine HonkFadeOut; // offset 0x50, size 0x1C, Decl: 104
    bool IsEndingHonk;       // offset 0x6C, size 0x1, Decl: 105

    EAX_CarState *pClosestCar; // offset 0x70, size 0x4, Decl: 107
};

// total size: 0x74
// Decl: 117
class CARSFX_TruckHorn : public CARSFX_TrafficHorn {
  public:
    DECLARE_CREATABLE();
    CARSFX_TruckHorn() {} // Decl: 117

    // Overrides: SndBase
    int GetController(int Index) override {
        return -1;
    } // Decl: 120
};

// total size: 0x30
// Decl: 126
class CARSFX_TrafficWoosh : public CARSFX {
  public:
    DECLARE_CREATABLE();
    CARSFX_TrafficWoosh();
    ~CARSFX_TrafficWoosh() override;

    // Overrides: SndBase
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void Detach() override;

    float GetPlayerSpeedRelativeToUs();

    virtual bool IsPlayerCarInRadius();

    virtual eDRIVE_BY_TYPE GetWooshSample();

    cStichWrapper *m_DriveByWoosh; // offset 0x28, size 0x4, Decl: 148

    float tSinceLastWoosh; // offset 0x2C, size 0x4, Decl: 150
};

#endif
