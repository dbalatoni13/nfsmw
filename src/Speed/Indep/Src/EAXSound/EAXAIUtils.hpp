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
    float m_fThreshold;       // 0x00
    float m_fAutoTrigger;     // 0x04
    float t_fSustain;         // 0x08
    float t_TriggerLength;    // 0x0C
    float fSign;              // 0x10
    Average AvgMonitor;       // 0x14
    bool bTrigger;            // 0x3C
    float CurSustain;         // 0x40
    float CurTriggerLength;   // 0x44
    float CurValue;           // 0x48

    void BeginTrigger();
    void EndTrigger();
    SndAITrigger();
    ~SndAITrigger();
    void Initialize(int AvgSize);
};

struct SndAIStateManager : public AudioMemBase {
    SndAITrigger SteeringMonitorLeft;   // 0x004
    SndAITrigger SteeringMonitorRight;  // 0x050
    SndAITrigger AccelMonitor;          // 0x09C
    SndAITrigger DeccelMonitor;         // 0x0E8
    SndAITrigger ThrottleMonitor;       // 0x134
    SFXCTL_Physics *m_pPhysicsCTL;      // 0x180
    SND_AI_STATE CurState;              // 0x184
    SND_AI_STATE PrevState;             // 0x188
    int bTransition;                    // 0x18C
    float m_tLastSwitch;                // 0x190

    virtual ~SndAIStateManager();

    SND_AI_STATE GetState() { return CurState; }
    void SwitchState(SND_AI_STATE NewState);
};

#endif
