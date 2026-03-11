#include "Speed/Indep/Src/EAXSound/EAXAIUtils.hpp"
#include <types.h>

void SndAITrigger::BeginTrigger() {
    bTrigger = true;
    CurTriggerLength = t_TriggerLength;
}

void SndAITrigger::EndTrigger() {
    bTrigger = false;
    CurSustain = t_fSustain;
}

SndAITrigger::SndAITrigger()
    : AvgMonitor() //
{
    CurValue = -1.0f;
    fSign = 1.0f;
    m_fThreshold = 0.0f;
    t_fSustain = 0.0f;
    m_fAutoTrigger = 0.0f;
    t_TriggerLength = 0.0f;
    CurSustain = -1.0f;
    CurTriggerLength = -1.0f;
    bTrigger = false;
}

SndAITrigger::~SndAITrigger() {}

void SndAITrigger::Initialize(int AvgSize) {
    AvgMonitor.Init(AvgSize);
    AvgMonitor.Reset(0.0f);
    CurSustain = t_fSustain;
}

void SndAIStateManager::SwitchState(SND_AI_STATE NewState) {
    bTransition = true;
    m_tLastSwitch = SndBase::m_fRunningTime;
    PrevState = CurState;
    CurState = NewState;
}