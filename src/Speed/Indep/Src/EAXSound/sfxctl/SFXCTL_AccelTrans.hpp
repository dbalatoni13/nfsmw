#ifndef SFXCTL_ACCEL_TRANS_HPP
#define SFXCTL_ACCEL_TRANS_HPP

#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/acceltrans.h"

// Decl: 10
enum FX_ACCEL_STATE {
    FX_ACCEL_STATE_NONE = 0,
    FX_ACCEL_STATE_ATTACK = 1,
    FX_ACCEL_STATE_IDLE_REVING = 2,
    FX_ACCEL_STATE_IDLE_ENGAGING = 3,
    FX_ACCEL_STATE_INTERRUPT = 4,
};

// total size: 0x9C
// Decl: 26
class SFXCTL_AccelTrans : public SFXCTL {
  public:
    DECLARE_CREATABLE();

    SFXCTL_AccelTrans();
    ~SFXCTL_AccelTrans() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void UpdateParams(float t) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;

    SFXCTL_Engine *m_pEngineCtl;  // offset 0x28, size 0x4, Decl: 38
    SFXCTL_Shifting *m_pShiftCtl; // offset 0x2C, size 0x4, Decl: 39

    // bool IsActive() {} // Decl: 45

    void UpdateRPM(float t);

    void UpdateTRQ(float t);

    void UpdateState(float t);

    void BeginAccelTrans();

    void BeginAccelTrans_Idle();

    bool ShouldBeginAccelTrans();

    bool ShouldBeginAccelTrans_Idle();

    bool ShouldPlayEngOffSweet();

    // Overrides: SndBase
    void Destroy() override;

    cInterpLine m_InterpEngRPM;    // offset 0x30, size 0x1C, Decl: 55
    cInterpLine m_InterpEngVol;    // offset 0x4C, size 0x1C, Decl: 56
    cInterpLine m_InterpEngTorque; // offset 0x68, size 0x1C, Decl: 57

    FX_ACCEL_STATE eAccelTransFxState; // offset 0x84, size 0x4, Decl: 60
    float t_LastAccelTrans;            // offset 0x88, size 0x4, Decl: 61
    bool IsAccelerating;               // offset 0x8C, size 0x1, Decl: 63
    bool OldIsAccelerating;            // offset 0x90, size 0x1, Decl: 64

    Attrib::Gen::acceltrans *m_pAccelTransDataSet; // offset 0x94, size 0x4, Decl: 76

    bool PlayEngOffSweet; // offset 0x98, size 0x1, Decl: 80
};

#endif
