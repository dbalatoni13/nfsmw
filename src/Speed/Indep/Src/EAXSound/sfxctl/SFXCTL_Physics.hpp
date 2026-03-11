#ifndef EAXSOUND_SFXCTL_SFXCTL_PHYSICS_H
#define EAXSOUND_SFXCTL_SFXCTL_PHYSICS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct EngRevDataPoint;
struct Average;
struct MAIEngineRev;

enum Gear {};
enum NIS_ENGINE_REVING_STATE {
    NIS_DISABLED = 0,
    NIS_OFF = 1,
    NIS_PATTERN_ON = 2,
    NIS_MERGE_WITH_PHYSICS = 3
};

typedef int HHANDLER;
struct bMatrix4;

struct SFXCTL_Physics : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

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
    /* 0x54 */ char m_fDeltaDesiredSpeed_pad[0x28]; // Average m_fDeltaDesiredSpeed
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

    ~SFXCTL_Physics() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    void SetupSFX(CSTATE_Base *_StateBase) override;

    void MsgRevOff(const MAIEngineRev &message);

    float GetPhysTRQ() { return PhysicsTRQ; }
};

struct SndAIStateManager;
struct SFXCTL_Shifting;

struct SFXCTL_AIPhysics : public SFXCTL_Physics {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    /* 0xd0 */ SFXCTL_Shifting *m_pShiftCtl;

    ~SFXCTL_AIPhysics() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    int GetController(int Index) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void Destroy() override;
    void UpdateMixerOutputs() override;
};

struct SFXCTL_TruckPhysics : public SFXCTL_AIPhysics {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
};

#endif
