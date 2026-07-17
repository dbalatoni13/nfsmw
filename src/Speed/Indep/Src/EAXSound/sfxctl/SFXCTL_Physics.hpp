//
#ifndef SFXCTL_PHYSICS_H
#define SFXCTL_PHYSICS_H

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAIUtils.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Generated/Messages/MAIEngineRev.h"
#include "Speed/Indep/Src/Physics/CarBasics.hpp"

// Decl: 10
enum NIS_ENGINE_REVING_STATE {
    NIS_DISABLED = 0,
    NIS_OFF = 1,
    NIS_PATTERN_ON = 2,
    NIS_MERGE_WITH_PHYSICS = 3,
};

// total size: 0xD0
// Decl: 22
class SFXCTL_Physics : public SFXCTL {
    DECLARE_CREATABLE();

  public:
    SFXCTL_Physics();

    // Overrides: AudioMemBase
    ~SFXCTL_Physics() override;

    // Overrides: SndBase
    void UpdateMixerOutputs() override;
    void UpdateParams(float t) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;

    float GetPhysRPM() {} // Decl: 37

    virtual float GetPhysTRQ() {} // Decl: 45

    bool IsCarAccelerating() {} // Decl: 58

    Gear GetGear() {} // Decl: 67

    void MsgRevEngine(const MAIEngineRev &message);

    void MsgRevOff(const MAIEngineRev &message);

    void UpdateNIS(float TotalTime, float deltaTime);

    bool m_bBlownEngineStreamQueued; // offset 0x28, size 0x1, Decl: 35

    float PhysicsRPM;      // offset 0x2C, size 0x4, Decl: 38
    float RedLineRPM;      // offset 0x30, size 0x4, Decl: 39
    float fMinPhysRPM;     // offset 0x34, size 0x4, Decl: 41
    float fMaxPhysRPM;     // offset 0x38, size 0x4, Decl: 42
    float fRedLinePhysRPM; // offset 0x3C, size 0x4, Decl: 43
    float PhysicsTRQ;      // offset 0x40, size 0x4, Decl: 46
    float fMaxPhysTRQ;     // offset 0x44, size 0x4, Decl: 47

    float m_fThrottle;   // offset 0x48, size 0x4, Decl: 50
    float m_OldThrottle; // offset 0x4C, size 0x4, Decl: 51

    float m_OldDesiredSpeed;      // offset 0x50, size 0x4, Decl: 53
    Average m_fDeltaDesiredSpeed; // offset 0x54, size 0x28, Decl: 54
    float m_tHoldDecel;           // offset 0x7C, size 0x4, Decl: 55

    bool IsAccelerating; // offset 0x80, size 0x1, Decl: 59
    float t_Last_Deccel; // offset 0x84, size 0x4, Decl: 60
    float t_Last_Accel;  // offset 0x88, size 0x4, Decl: 61

    Gear m_CurGear;  // offset 0x8C, size 0x4, Decl: 64
    Gear m_LastGear; // offset 0x90, size 0x4, Decl: 65

    bMatrix4 *mRPMCurve; // offset 0x94, size 0x4, Decl: 69

    Hermes::HHANDLER mMsgRevEngine; // offset 0x98, size 0x4, Decl: 77

    Hermes::HHANDLER mMsgRevOff; // offset 0x9C, size 0x4, Decl: 80

    bool PattternPlay;     // offset 0xA0, size 0x1, Decl: 86
    int PatternNumber;     // offset 0xA4, size 0x4, Decl: 87
    int CarID;             // offset 0xA8, size 0x4, Decl: 88
    bool bPlayerEngEnable; // offset 0xAC, size 0x1, Decl: 89

    int RevFramesRemaining; // offset 0xB0, size 0x4, Decl: 92

    bool NISRevingEnabled;                      // offset 0xB4, size 0x1, Decl: 94
    NIS_ENGINE_REVING_STATE eCurNisRevingState; // offset 0xB8, size 0x4, Decl: 95

    float TimeIntoRev; // offset 0xBC, size 0x4, Decl: 98
    int NumDataPoints; // offset 0xC0, size 0x4, Decl: 99

    EngRevDataPoint *pRevData; // offset 0xC4, size 0x4, Decl: 101
    float NISRPM;              // offset 0xC8, size 0x4, Decl: 103
    float NISTRQ;              // offset 0xCC, size 0x4, Decl: 104
};

// total size: 0x290
// Decl: 111
class SFXCTL_AIPhysics : public SFXCTL_Physics {
    DECLARE_CREATABLE();

  public:
    SFXCTL_AIPhysics();

    // Overrides: AudioMemBase
    ~SFXCTL_AIPhysics() override;

    // Overrides: SndBase
    void UpdateParams(float t) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void UpdateMixerOutputs() override {} // Decl: 126
    void UpdateAccel(float t);
    SFXCTL_Shifting *m_pShiftCtl; // offset 0xD0, size 0x4, Decl: 128

    SndAIStateManager AIStateManager;         // offset 0xD4, size 0x194, Decl: 134
    SndAIStateManager *GetAIStateManager() {} // Decl: 135

    bool IsDrifting;  // offset 0x268, size 0x1, Decl: 137
    bool IsCornering; // offset 0x26C, size 0x1, Decl: 138

    float SteadyVelocityFactor; // offset 0x270, size 0x4, Decl: 140

    float TargetRPMOffset;       // offset 0x274, size 0x4, Decl: 142
    bAngle m_AngleDeltaRPM_LFO;  // offset 0x278, size 0x2, Decl: 143
    float m_DeltaRPM_LFO_Offset; // offset 0x27C, size 0x4, Decl: 144

    void UpdateGear();
    Gear SuggestGear();
    int UpShiftSameGearCount;   // offset 0x280, size 0x4, Decl: 147
    int DownShiftSameGearCount; // offset 0x284, size 0x4, Decl: 148

    float GenDeltaRPM();
    void UpdateRPM(float t);
    void UpdateTorque(float t);

    float Zero60Time;  // offset 0x288, size 0x4, Decl: 153
    float m_fDeltaRPM; // offset 0x28C, size 0x4, Decl: 154
};

// total size: 0x290
// Decl: 170
class SFXCTL_TruckPhysics : public SFXCTL_AIPhysics {
    DECLARE_CREATABLE();

  public:
    SFXCTL_TruckPhysics() {} // Decl: 170
};

#endif
