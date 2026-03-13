#include "Speed/Indep/Src/EAXSound/EAXAIUtils.hpp"
#include <types.h>

extern "C" void Average_Record(Average *avg, float value) asm("Record__7Averagef");
extern "C" void AverageBase_Recalculate(AverageBase *avg) asm("Recalculate__11AverageBase");

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

void SndAITrigger::Update(float UpdateVal, float t) {
    Average_Record(&AvgMonitor, UpdateVal);
    AverageBase_Recalculate(&AvgMonitor);

    float avgValue = AvgMonitor.GetValue();
    CurValue = avgValue;

    if (bTrigger) {
        if (fSign * m_fThreshold < fSign * avgValue) {
            CurTriggerLength = t_TriggerLength;
            return;
        }

        CurTriggerLength -= t;
        if (CurTriggerLength < 0.0f) {
            EndTrigger();
        }
        return;
    }

    if (fSign * m_fAutoTrigger < fSign * avgValue) {
        BeginTrigger();
        return;
    }

    if (fSign * m_fThreshold < fSign * avgValue) {
        CurSustain -= t;
        if (CurSustain < 0.0f) {
            BeginTrigger();
        }
        return;
    }

    CurSustain = t_fSustain;
}

void SndAIStateManager::SwitchState(SND_AI_STATE NewState) {
    PrevState = CurState;
    bTransition = true;
    m_tLastSwitch = SndBase::m_fRunningTime;
    CurState = NewState;
}
