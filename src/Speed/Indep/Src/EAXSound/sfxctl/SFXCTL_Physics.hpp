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
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    /* 0x28 */ bool m_bBlownEngineStreamQueued;
    /* 0x2c */ float PhysicsRPM;
    /* 0x30 */ float RedLineRPM;
    /* 0x34 */ float fMinPhysRPM;
    /* 0x38 */ float fMaxPhysRPM;
    /* 0x3c */ float fRedLinePhysRPM;
    /* 0x40 */ float PhysicsTRQ;
    /* 0x44 */ float fMaxPhysTRQ;
    /* 0x48 */ float m_fThrottle;
    /* 0x4c */ float m_OldThrottle;
    /* 0x50 */ float m_OldDesiredSpeed;
    /* 0x54 */ Average m_fDeltaDesiredSpeed;
    /* 0x7c */ float m_tHoldDecel;
    /* 0x80 */ bool IsAccelerating;
    /* 0x84 */ float t_Last_Deccel;
    /* 0x88 */ float t_Last_Accel;
    /* 0x8c */ Gear m_CurGear;
    /* 0x90 */ Gear m_LastGear;
    /* 0x94 */ bMatrix4 *mRPMCurve;
    /* 0x98 */ HHANDLER mMsgRevEngine;
    /* 0x9c */ HHANDLER mMsgRevOff;
    /* 0xa0 */ bool PattternPlay;
    /* 0xa4 */ int PatternNumber;
    /* 0xa8 */ int CarID;
    /* 0xac */ bool bPlayerEngEnable;
    /* 0xb0 */ int RevFramesRemaining;
    /* 0xb4 */ bool NISRevingEnabled;
    /* 0xb8 */ NIS_ENGINE_REVING_STATE eCurNisRevingState;
    /* 0xbc */ float TimeIntoRev;
    /* 0xc0 */ int NumDataPoints;
    /* 0xc4 */ EngRevDataPoint *pRevData;
    /* 0xc8 */ float NISRPM;
    /* 0xcc */ float NISTRQ;

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
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXCTL_AIPhysics();
    /* 0xd0 */ SFXCTL_Shifting *m_pShiftCtl;
    /* 0xd4 */ SndAIStateManager AIStateManager;
    /* 0x268 */ bool IsDrifting;
    /* 0x26c */ bool IsCornering;
    /* 0x270 */ float SteadyVelocityFactor;
    /* 0x274 */ float TargetRPMOffset;
    /* 0x278 */ bAngle m_AngleDeltaRPM_LFO;
    /* 0x27c */ float m_DeltaRPM_LFO_Offset;
    /* 0x280 */ int UpShiftSameGearCount;
    /* 0x284 */ int DownShiftSameGearCount;
    /* 0x288 */ float Zero60Time;
    /* 0x28c */ float m_fDeltaRPM;

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
    void UpdateMixerOutputs() override;
    SndAIStateManager *GetAIStateManager() { return &AIStateManager; }
};

struct SFXCTL_TruckPhysics : public SFXCTL_AIPhysics {
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    static SndBase *CreateObject(unsigned int allocator);
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
};

#endif
