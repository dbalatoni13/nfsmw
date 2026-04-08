#ifndef EAXSOUND_SFXCTL_SFXCTL_PHYSICS_H
#define EAXSOUND_SFXCTL_SFXCTL_PHYSICS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAIUtils.hpp"

struct EngRevDataPoint;
struct MAIEngineRev;

typedef Sound::Gear Gear;
enum NIS_ENGINE_REVING_STATE {
    NIS_DISABLED = 0,
    NIS_OFF = 1,
    NIS_PATTERN_ON = 2,
    NIS_MERGE_WITH_PHYSICS = 3
};

namespace Hermes {
struct _h_HHANDLER__;
} // namespace Hermes
typedef Hermes::_h_HHANDLER__ *HHANDLER;

struct bMatrix4;

struct SFXCTL_Physics : public SFXCTL {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    bool m_bBlownEngineStreamQueued;             // offset 0x28, size 0x1
    float PhysicsRPM;                            // offset 0x2C, size 0x4
    float RedLineRPM;                            // offset 0x30, size 0x4
    float fMinPhysRPM;                           // offset 0x34, size 0x4
    float fMaxPhysRPM;                           // offset 0x38, size 0x4
    float fRedLinePhysRPM;                       // offset 0x3C, size 0x4
    float PhysicsTRQ;                            // offset 0x40, size 0x4
    float fMaxPhysTRQ;                           // offset 0x44, size 0x4
    float m_fThrottle;                           // offset 0x48, size 0x4
    float m_OldThrottle;                         // offset 0x4C, size 0x4
    float m_OldDesiredSpeed;                     // offset 0x50, size 0x4
    Average m_fDeltaDesiredSpeed;                // offset 0x54, size 0x28
    float m_tHoldDecel;                          // offset 0x7C, size 0x4
    bool IsAccelerating;                         // offset 0x80, size 0x1
    float t_Last_Deccel;                         // offset 0x84, size 0x4
    float t_Last_Accel;                          // offset 0x88, size 0x4
    Gear m_CurGear;                              // offset 0x8C, size 0x4
    Gear m_LastGear;                             // offset 0x90, size 0x4
    bMatrix4 *mRPMCurve;                         // offset 0x94, size 0x4
    HHANDLER mMsgRevEngine;                      // offset 0x98, size 0x4
    HHANDLER mMsgRevOff;                         // offset 0x9C, size 0x4
    bool PattternPlay;                           // offset 0xA0, size 0x1
    int PatternNumber;                           // offset 0xA4, size 0x4
    int CarID;                                   // offset 0xA8, size 0x4
    bool bPlayerEngEnable;                       // offset 0xAC, size 0x1
    int RevFramesRemaining;                      // offset 0xB0, size 0x4
    bool NISRevingEnabled;                       // offset 0xB4, size 0x1
    NIS_ENGINE_REVING_STATE eCurNisRevingState;  // offset 0xB8, size 0x4
    float TimeIntoRev;                           // offset 0xBC, size 0x4
    int NumDataPoints;                           // offset 0xC0, size 0x4
    EngRevDataPoint *pRevData;                   // offset 0xC4, size 0x4
    float NISRPM;                                // offset 0xC8, size 0x4
    float NISTRQ;                                // offset 0xCC, size 0x4

    SFXCTL_Physics();
    ~SFXCTL_Physics() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void UpdateParams(float t) override;
    void UpdateMixerOutputs() override;

    void MsgRevEngine(const MAIEngineRev &message);
    void MsgRevOff(const MAIEngineRev &message);
    void UpdateNIS(float t, float dt);

    float GetPhysRPM() { return PhysicsRPM; }
    virtual float GetPhysTRQ() { return PhysicsTRQ; }
};

struct SndAIStateManager;
struct SFXCTL_Shifting;

struct SFXCTL_AIPhysics : public SFXCTL_Physics {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXCTL_AIPhysics();
    SFXCTL_Shifting *m_pShiftCtl;                // offset 0xD0, size 0x4
    SndAIStateManager AIStateManager;            // offset 0xD4, size 0x194
    bool IsDrifting;                             // offset 0x268, size 0x1
    bool IsCornering;                            // offset 0x26C, size 0x1
    float SteadyVelocityFactor;                  // offset 0x270, size 0x4
    float TargetRPMOffset;                       // offset 0x274, size 0x4
    unsigned short m_AngleDeltaRPM_LFO;          // offset 0x278, size 0x2
    float m_DeltaRPM_LFO_Offset;                // offset 0x27C, size 0x4
    int UpShiftSameGearCount;                    // offset 0x280, size 0x4
    int DownShiftSameGearCount;                  // offset 0x284, size 0x4
    float Zero60Time;                            // offset 0x288, size 0x4
    float m_fDeltaRPM;                           // offset 0x28C, size 0x4

    ~SFXCTL_AIPhysics() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    int GetController(int Index) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void UpdateParams(float t) override;
    void AttachController(SFXCTL *) override;
    float GenDeltaRPM();
    void UpdateRPM(float t);
    void UpdateTorque(float t);
    void UpdateAccel(float t);
    Gear SuggestGear();
    void UpdateGear();
    void Destroy() override;
    void UpdateMixerOutputs() override {}
    SndAIStateManager *GetAIStateManager() { return &AIStateManager; }
};

struct SFXCTL_TruckPhysics : public SFXCTL_AIPhysics {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    static SndBase *CreateObject(unsigned int allocator);
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
};

#endif
