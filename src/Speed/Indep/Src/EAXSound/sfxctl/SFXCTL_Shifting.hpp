//
#ifndef SFXCTL_SHIFT_H
#define SFXCTL_SHIFT_H

#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/shiftpattern.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/CarBasics.hpp"

// Decl: 9
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

// Decl: 23
enum FX_POST_SHIFT_LFO {
    SHIFT_LFO_NONE = 0,
    SHIFT_LFO_ON = 1,
};

class SFXCTL_Engine;

// total size: 0x17C
// Decl: 31
class SFXCTL_Shifting : public SFXCTL {
    DECLARE_CREATABLE();

  public:
    SFXCTL_Shifting();

    // Overrides: AudioMemBase
    ~SFXCTL_Shifting() override;

    // Overrides: SndBase
    void UpdateParams(float t) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;

    SFXCTL_Engine *m_pEngineCtl; // offset 0x28, size 0x4, Decl: 47

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void UpdateMixerOutputs() override;

    bool IsActive() {} // Decl: 51

    float GetShiftingRPM();
    float GetShiftingTRQ();
    float GetShiftingVOL();
    void BeginUpShift();
    void BeginDownShift();
    void UpdateGearShiftState(float t);
    void CleanUpShiftFX();
    bool IsUpshifting() {} // Decl: 62

    bool IsDownShifting() {} // Decl: 67

    Gear GetCurGear();
    Gear GetLastGear();
    void SetupGraph(int rpm);
    void UpdateRPM(float t);
    void UpdateTorque(float t);
    void PostShiftFX_Init();
    void PostShiftFX_End();
    void PostShiftFX_Update(float t);

    bool m_bNeed_ShiftGearSnd; // offset 0x2C, size 0x1, Decl: 75
    bool m_bNeed_DisengageSnd; // offset 0x30, size 0x1, Decl: 76
    bool m_bNeed_EngageSnd;    // offset 0x34, size 0x1, Decl: 77
    bool m_bNeed_AccelSnd;     // offset 0x38, size 0x1, Decl: 78
    bool m_bNeed_DeccelSnd;    // offset 0x3C, size 0x1, Decl: 79
    bool m_bShouldBeWhining;   // offset 0x40, size 0x1, Decl: 80
    bool m_bBrakePedalMashed;  // offset 0x44, size 0x1, Decl: 81

    Attrib::Gen::shiftpattern *m_pShiftingPatternData; // offset 0x48, size 0x4, Decl: 86

    SHIFT_STAGE eShiftState;        // offset 0x4C, size 0x4, Decl: 88
    SHIFT_STAGE eShiftStageChanged; // offset 0x50, size 0x4, Decl: 89

    int m_VOL_LFO_AMP; // offset 0x54, size 0x4, Decl: 92
    int m_VOL_LFO_FRQ; // offset 0x58, size 0x4, Decl: 93

    int m_TRQ_LFO_AMP; // offset 0x5C, size 0x4, Decl: 95
    int m_TRQ_LFO_FRQ; // offset 0x60, size 0x4, Decl: 96

    int m_RPM_LFO_AMP; // offset 0x64, size 0x4, Decl: 98
    int m_RPM_LFO_FRQ; // offset 0x68, size 0x4, Decl: 99

    bool m_bPendingNeedShiftSound;   // offset 0x6C, size 0x1, Decl: 101
    AEMS_SHIFTING_SAMPLES ShiftType; // offset 0x70, size 0x4, Decl: 102
    float tShiftDelay;               // offset 0x74, size 0x4, Decl: 103
    float t_Last_Shift;              // offset 0x78, size 0x4, Decl: 104

    float RPM_AtShift; // offset 0x7C, size 0x4, Decl: 109

    cInterpLine m_InterpShiftTorque; // offset 0x80, size 0x1C, Decl: 111
    cInterpLine m_InterpShiftRPM;    // offset 0x9C, size 0x1C, Decl: 112
    cInterpLine m_InterpShiftVol;    // offset 0xB8, size 0x1C, Decl: 113

    Graph m_RPMGraph;               // offset 0xD4, size 0x8, Decl: 116
    struct bVector2 m_RPMPoints[7]; // offset 0xDC, size 0x38, Decl: 117
    float t_CurStage;               // offset 0x114, size 0x4, Decl: 118
    uint8 m_CurStage;               // offset 0x118, size 0x1, Decl: 119
    uint16 RPMOffset;               // offset 0x11A, size 0x2, Decl: 120
    Timer m_timeBrakeLastMashed;    // offset 0x11C, size 0x4, Decl: 121

    int m_nPostShiftFXLevel;           // offset 0x120, size 0x4, Decl: 130
    cInterpLine m_Shift_RPM_AMP_DECAY; // offset 0x124, size 0x1C, Decl: 131
    cInterpLine m_Shift_VOL_AMP_DECAY; // offset 0x140, size 0x1C, Decl: 132
    FX_POST_SHIFT_LFO eShift_LFO;      // offset 0x15C, size 0x4, Decl: 133

    cInterpLine m_VisualRPM; // offset 0x160, size 0x1C, Decl: 135
};

// total size: 0x17C
// Decl: 143
class SFXCTL_AIShifting : public SFXCTL_Shifting {
    DECLARE_CREATABLE();

  public:
    SFXCTL_AIShifting() {} // Decl: 143

    // Overrides: AudioMemBase
    ~SFXCTL_AIShifting() override {} // Decl: 143

    // Overrides: SndBase
    void UpdateMixerOutputs() override {} // Decl: 147
};

#endif
