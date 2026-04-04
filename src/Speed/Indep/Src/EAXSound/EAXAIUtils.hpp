#ifndef EAXSOUND_EAXAIUTILS_H
#define EAXSOUND_EAXAIUTILS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"

enum SND_AI_STATE {
    SND_AI_STATE_UNKNOWN = 0,
    SND_AI_STATE_PRERACE = 1,
    SND_AI_STATE_IDLE = 2,
    SND_AI_STATE_ACCEL = 3,
    SND_AI_STATE_DECEL = 4,
    SND_AI_STATE_CORNER_LEFT = 5,
    SND_AI_STATE_CORNER_RIGHT = 6,
    MAX_NUM_SND_AI_STATE = 7,
};

struct SFXCTL_Physics;

struct SndAITrigger {
    float m_fThreshold;       // offset 0x0, size 0x4
    float m_fAutoTrigger;     // offset 0x4, size 0x4
    float t_fSustain;         // offset 0x8, size 0x4
    float t_TriggerLength;    // offset 0xC, size 0x4
    float fSign;              // offset 0x10, size 0x4
    Average AvgMonitor;       // offset 0x14, size 0x28
    bool bTrigger;            // offset 0x3C, size 0x1
    float CurSustain;         // offset 0x40, size 0x4
    float CurTriggerLength;   // offset 0x44, size 0x4
    float CurValue;           // offset 0x48, size 0x4

    void BeginTrigger();
    void EndTrigger();
    SndAITrigger();
    ~SndAITrigger();
    void Initialize(int AvgSize);
    void Update(float UpdateVal, float t);
    bool IsTriggering() {
        return bTrigger;
    }
};

struct SndAIStateManager : public AudioMemBase {
    SndAITrigger SteeringMonitorLeft;   // offset 0x4, size 0x4C
    SndAITrigger SteeringMonitorRight;  // offset 0x50, size 0x4C
    SndAITrigger AccelMonitor;          // offset 0x9C, size 0x4C
    SndAITrigger DeccelMonitor;         // offset 0xE8, size 0x4C
    SndAITrigger ThrottleMonitor;       // offset 0x134, size 0x4C
    SFXCTL_Physics *m_pPhysicsCTL;      // offset 0x180, size 0x4
    SND_AI_STATE CurState;              // offset 0x184, size 0x4
    SND_AI_STATE PrevState;             // offset 0x188, size 0x4
    bool bTransition;                   // offset 0x18C, size 0x1
    float m_tLastSwitch;                // offset 0x190, size 0x4

    virtual ~SndAIStateManager();
    SndAIStateManager();

    void SwitchState(SND_AI_STATE NewState);
    void Initialize(SFXCTL_Physics *_m_pPhysicsCTL);
    void Update(float t);
    void UpdateState(float t);
    void GeneratePotentialStates(bool *ArrayList);
    SND_AI_STATE GetState() {
        return CurState;
    }
};

#endif
