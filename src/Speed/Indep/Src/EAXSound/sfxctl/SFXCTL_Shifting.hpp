#ifndef EAXSOUND_SFXCTL_SFXCTL_SHIFTING_H
#define EAXSOUND_SFXCTL_SFXCTL_SHIFTING_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

enum SHIFT_STAGE {
    SHFT_NONE = 0,
    SHFT_UP_DISENGAGE = 1,
    SHFT_UP_ENGAGING = 2,
    SHFT_UP_LFO = 3,
    SHFT_DOWN_DISENGAGE = 4,
    SHFT_DOWN_ENGAGING_RISE = 5,
    SHFT_DOWN_ENGAGING_FALL = 6,
    SHFT_DOWN_ENGAGING_REATTACH = 7,
};
enum AEMS_SHIFTING_SAMPLES {};
enum FX_POST_SHIFT_LFO {
    SHIFT_LFO_NONE = 0,
    SHIFT_LFO_ON = 1,
};

namespace Attrib {
namespace Gen {
struct shiftpattern;
}
}

struct SFXCTL_Engine;

struct SFXCTL_Shifting : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    /* 0x28 */ SFXCTL_Engine *m_pEngineCtl;
    /* 0x2c */ bool m_bNeed_ShiftGearSnd;
    /* 0x30 */ bool m_bNeed_DisengageSnd;
    /* 0x34 */ bool m_bNeed_EngageSnd;
    /* 0x38 */ bool m_bNeed_AccelSnd;
    /* 0x3c */ bool m_bNeed_DeccelSnd;
    /* 0x40 */ bool m_bShouldBeWhining;
    /* 0x44 */ bool m_bBrakePedalMashed;
    /* 0x48 */ Attrib::Gen::shiftpattern *m_pShiftingPatternData;
    /* 0x4c */ SHIFT_STAGE eShiftState;
    /* 0x50 */ SHIFT_STAGE eShiftStageChanged;
    /* 0x54 */ int m_VOL_LFO_AMP;
    /* 0x58 */ int m_VOL_LFO_FRQ;
    /* 0x5c */ int m_TRQ_LFO_AMP;
    /* 0x60 */ int m_TRQ_LFO_FRQ;
    /* 0x64 */ int m_RPM_LFO_AMP;
    /* 0x68 */ int m_RPM_LFO_FRQ;
    /* 0x6c */ bool m_bPendingNeedShiftSound;
    /* 0x70 */ AEMS_SHIFTING_SAMPLES ShiftType;
    /* 0x74 */ float tShiftDelay;
    /* 0x78 */ float t_Last_Shift;
    /* 0x7c */ float RPM_AtShift;
    /* 0x80 */ cInterpLine m_InterpShiftTorque;
    /* 0x9c */ cInterpLine m_InterpShiftRPM;
    /* 0xb8 */ cInterpLine m_InterpShiftVol;
    /* 0xd4 */ Graph m_RPMGraph;
    /* 0xdc */ bVector2 m_RPMPoints[7];
    /* 0x114 */ float t_CurStage;
    /* 0x118 */ unsigned char m_CurStage;
    /* 0x11a */ unsigned short RPMOffset;
    /* 0x11c */ Timer m_timeBrakeLastMashed;
    /* 0x120 */ int m_nPostShiftFXLevel;
    /* 0x124 */ cInterpLine m_Shift_RPM_AMP_DECAY;
    /* 0x140 */ cInterpLine m_Shift_VOL_AMP_DECAY;
    /* 0x15c */ FX_POST_SHIFT_LFO eShift_LFO;
    /* 0x160 */ cInterpLine m_VisualRPM;

    SFXCTL_Shifting();
    ~SFXCTL_Shifting() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void InitSFX() override;
    void UpdateParams(float t) override;
    int GetController(int Index) override;

    float GetShiftingRPM();
    float GetShiftingTRQ();
    float GetShiftingVOL();
    Gear GetCurGear();
    Gear GetLastGear();
    void PostShiftFX_End();
    void CleanUpShiftFX();
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void AttachController(SFXCTL *) override;
    void UpdateMixerOutputs() override;
    void UpdateGearShiftState(float t);
    void UpdateTorque(float t);
    void UpdateRPM(float t);
    void BeginUpShift();
    void BeginDownShift();
    void PostShiftFX_Update(float t);
    void PostShiftFX_Init();

    bool IsActive() {
        return eShiftState != SHFT_NONE;
    }

    bool IsDownShifting() {
        return static_cast<unsigned int>(eShiftState - SHFT_DOWN_DISENGAGE) < 4u;
    }
};

#endif
