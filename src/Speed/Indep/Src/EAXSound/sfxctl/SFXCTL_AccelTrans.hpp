#ifndef EAXSOUND_SFXCTL_SFXCTL_ACCELTRANS_H
#define EAXSOUND_SFXCTL_SFXCTL_ACCELTRANS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct SFXCTL_Engine;
struct SFXCTL_Shifting;
namespace Attrib {
namespace Gen {
struct acceltrans;
}
}

enum FX_ACCEL_STATE {
    FX_ACCEL_STATE_NONE = 0,
    FX_ACCEL_STATE_ATTACK = 1,
    FX_ACCEL_STATE_IDLE_REVING = 2,
    FX_ACCEL_STATE_IDLE_ENGAGING = 3,
    FX_ACCEL_STATE_INTERRUPT = 4,
};

struct SFXCTL_AccelTrans : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXCTL_AccelTrans();
    /* 0x28 */ SFXCTL_Engine *m_pEngineCtl;
    /* 0x2c */ SFXCTL_Shifting *m_pShiftCtl;
    /* 0x30 */ cInterpLine m_InterpEngRPM;
    /* 0x4c */ cInterpLine m_InterpEngVol;
    /* 0x68 */ cInterpLine m_InterpEngTorque;
    /* 0x84 */ int eAccelTransFxState;
    /* 0x88 */ float t_LastAccelTrans;
    /* 0x8c */ bool IsAccelerating;
    /* 0x90 */ bool OldIsAccelerating;
    /* 0x94 */ Attrib::Gen::acceltrans *m_pAccelTransDataSet;
    /* 0x98 */ bool PlayEngOffSweet;

    ~SFXCTL_AccelTrans() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    int GetController(int Index) override;
    void Destroy() override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void UpdateParams(float t) override;
    void AttachController(SFXCTL *) override;
    void UpdateRPM(float t);
    void UpdateTRQ(float t);
    void UpdateState(float t);
    void BeginAccelTrans();
    void BeginAccelTrans_Idle();
    bool ShouldBeginAccelTrans_Idle();
    bool ShouldBeginAccelTrans();
    bool ShouldPlayEngOffSweet();

    bool IsActive() {
        return eAccelTransFxState != FX_ACCEL_STATE_NONE;
    }
};

#endif
