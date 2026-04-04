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
    SFXCTL_Engine *m_pEngineCtl;                 // offset 0x28, size 0x4
    bool m_bNeed_ShiftGearSnd;                   // offset 0x2C, size 0x1
    bool m_bNeed_DisengageSnd;                   // offset 0x30, size 0x1
    bool m_bNeed_EngageSnd;                      // offset 0x34, size 0x1
    bool m_bNeed_AccelSnd;                       // offset 0x38, size 0x1
    bool m_bNeed_DeccelSnd;                      // offset 0x3C, size 0x1
    bool m_bShouldBeWhining;                     // offset 0x40, size 0x1
    bool m_bBrakePedalMashed;                    // offset 0x44, size 0x1
    Attrib::Gen::shiftpattern *m_pShiftingPatternData; // offset 0x48, size 0x4
    SHIFT_STAGE eShiftState;                     // offset 0x4C, size 0x4
    SHIFT_STAGE eShiftStageChanged;              // offset 0x50, size 0x4
    int m_VOL_LFO_AMP;                          // offset 0x54, size 0x4
    int m_VOL_LFO_FRQ;                          // offset 0x58, size 0x4
    int m_TRQ_LFO_AMP;                          // offset 0x5C, size 0x4
    int m_TRQ_LFO_FRQ;                          // offset 0x60, size 0x4
    int m_RPM_LFO_AMP;                          // offset 0x64, size 0x4
    int m_RPM_LFO_FRQ;                          // offset 0x68, size 0x4
    bool m_bPendingNeedShiftSound;               // offset 0x6C, size 0x1
    AEMS_SHIFTING_SAMPLES ShiftType;             // offset 0x70, size 0x4
    float tShiftDelay;                           // offset 0x74, size 0x4
    float t_Last_Shift;                          // offset 0x78, size 0x4
    float RPM_AtShift;                           // offset 0x7C, size 0x4
    cInterpLine m_InterpShiftTorque;             // offset 0x80, size 0x1C
    cInterpLine m_InterpShiftRPM;                // offset 0x9C, size 0x1C
    cInterpLine m_InterpShiftVol;                // offset 0xB8, size 0x1C
    Graph m_RPMGraph;                            // offset 0xD4, size 0x8
    bVector2 m_RPMPoints[7];                     // offset 0xDC, size 0x38
    float t_CurStage;                            // offset 0x114, size 0x4
    unsigned char m_CurStage;                    // offset 0x118, size 0x1
    unsigned short RPMOffset;                    // offset 0x11A, size 0x2
    Timer m_timeBrakeLastMashed;                 // offset 0x11C, size 0x4
    int m_nPostShiftFXLevel;                     // offset 0x120, size 0x4
    cInterpLine m_Shift_RPM_AMP_DECAY;           // offset 0x124, size 0x1C
    cInterpLine m_Shift_VOL_AMP_DECAY;           // offset 0x140, size 0x1C
    FX_POST_SHIFT_LFO eShift_LFO;               // offset 0x15C, size 0x4
    cInterpLine m_VisualRPM;                     // offset 0x160, size 0x1C

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
