#ifndef EAXSOUND_SFXCTL_SFXCTL_ACCELTRANS_H
#define EAXSOUND_SFXCTL_SFXCTL_ACCELTRANS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct SFXCTL_Engine;
struct SFXCTL_Shifting;
struct acceltrans;

struct SFXCTL_AccelTrans : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

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
    /* 0x94 */ acceltrans *m_pAccelTransDataSet;
    /* 0x98 */ bool PlayEngOffSweet;

    ~SFXCTL_AccelTrans() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
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
};

#endif
