//
#ifndef EAXAIUTILS_HPP
#define EAXAIUTILS_HPP

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"

// Decl: 5
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

// total size: 0x4C
// Decl: 22
class SndAITrigger {
  public:
    SndAITrigger();  // Decl: 24
    ~SndAITrigger(); // Decl: 25

    void Initialize(int AvgSize); // Decl: 27

    float m_fThreshold;    // offset 0x0, size 0x4, Decl: 30
    float m_fAutoTrigger;  // offset 0x4, size 0x4, Decl: 31
    float t_fSustain;      // offset 0x8, size 0x4, Decl: 33
    float t_TriggerLength; // offset 0xC, size 0x4, Decl: 34
    float fSign;           // offset 0x10, size 0x4, Decl: 35
    Average AvgMonitor;    // offset 0x14, size 0x28, Decl: 44

    bool IsTriggering() {}                 // Decl: 38
    void Update(float UpdateVal, float t); // Decl: 39

    void BeginTrigger(); // Decl: 41
    void EndTrigger();   // Decl: 42

  private:
    bool bTrigger;          // offset 0x3C, size 0x1, Decl: 47
    float CurSustain;       // offset 0x40, size 0x4, Decl: 49
    float CurTriggerLength; // offset 0x44, size 0x4, Decl: 50
    float CurValue;         // offset 0x48, size 0x4, Decl: 51
};

class SFXCTL_Physics;

// total size: 0x194
// Decl: 58
struct SndAIStateManager : public AudioMemBase {
    SndAIStateManager();

    // Overrides: AudioMemBase
    ~SndAIStateManager() override;

    SND_AI_STATE GetState() {} // Decl: 64

    void UpdateState(float t);

    void Initialize(SFXCTL_Physics *_m_pPhysicsCTL);

    void Update(float t);

  private:
    void SwitchState(SND_AI_STATE NewState);

    void GeneratePotentialStates(bool *ArrayList);

    SndAITrigger SteeringMonitorLeft;  // offset 0x4, size 0x4C, Decl: 71
    SndAITrigger SteeringMonitorRight; // offset 0x50, size 0x4C, Decl: 72
    SndAITrigger AccelMonitor;         // offset 0x9C, size 0x4C, Decl: 73
    SndAITrigger DeccelMonitor;        // offset 0xE8, size 0x4C, Decl: 74
    SndAITrigger ThrottleMonitor;      // offset 0x134, size 0x4C, Decl: 75
    SFXCTL_Physics *m_pPhysicsCTL;     // offset 0x180, size 0x4, Decl: 83
    SND_AI_STATE CurState;             // offset 0x184, size 0x4, Decl: 86
    SND_AI_STATE PrevState;            // offset 0x188, size 0x4, Decl: 87
    bool bTransition;                  // offset 0x18C, size 0x1, Decl: 88
    float m_tLastSwitch;               // offset 0x190, size 0x4, Decl: 89
};

#endif
