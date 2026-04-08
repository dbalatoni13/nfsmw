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
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXCTL_AccelTrans();
    SFXCTL_Engine *m_pEngineCtl;               // offset 0x28, size 0x4
    SFXCTL_Shifting *m_pShiftCtl;              // offset 0x2C, size 0x4
    cInterpLine m_InterpEngRPM;                // offset 0x30, size 0x1C
    cInterpLine m_InterpEngVol;                // offset 0x4C, size 0x1C
    cInterpLine m_InterpEngTorque;             // offset 0x68, size 0x1C
    FX_ACCEL_STATE eAccelTransFxState;         // offset 0x84, size 0x4
    float t_LastAccelTrans;                    // offset 0x88, size 0x4
    bool IsAccelerating;                       // offset 0x8C, size 0x1
    bool OldIsAccelerating;                    // offset 0x90, size 0x1
    Attrib::Gen::acceltrans *m_pAccelTransDataSet; // offset 0x94, size 0x4
    bool PlayEngOffSweet;                      // offset 0x98, size 0x1

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
