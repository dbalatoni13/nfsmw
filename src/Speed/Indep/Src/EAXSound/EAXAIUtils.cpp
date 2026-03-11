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

void SndAIStateManager::SwitchState(SND_AI_STATE NewState) {
    bTransition = true;
    m_tLastSwitch = SndBase::m_fRunningTime;
    PrevState = CurState;
    CurState = NewState;
}