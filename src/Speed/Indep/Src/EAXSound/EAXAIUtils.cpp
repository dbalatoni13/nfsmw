#include "Speed/Indep/Src/EAXSound/EAXAIUtils.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
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

SndAIStateManager::SndAIStateManager()
    : SteeringMonitorLeft() //
    , SteeringMonitorRight() //
    , AccelMonitor() //
    , DeccelMonitor() //
    , ThrottleMonitor() {
    m_pPhysicsCTL = nullptr;
    CurState = SND_AI_STATE_UNKNOWN;
    PrevState = SND_AI_STATE_UNKNOWN;
    bTransition = 0;
    m_tLastSwitch = 0.0f;
}

SndAIStateManager::~SndAIStateManager() {}

void SndAIStateManager::Initialize(SFXCTL_Physics *pPhys) {
    m_pPhysicsCTL = pPhys;

    SteeringMonitorLeft.Initialize(8);
    SteeringMonitorLeft.fSign = 1.0f;
    SteeringMonitorLeft.m_fThreshold = 0.25f;
    SteeringMonitorLeft.m_fAutoTrigger = 0.65f;
    SteeringMonitorLeft.t_fSustain = 0.2f;
    SteeringMonitorLeft.t_TriggerLength = 0.4f;

    SteeringMonitorRight.Initialize(8);
    SteeringMonitorRight.fSign = 1.0f;
    SteeringMonitorRight.m_fThreshold = 0.25f;
    SteeringMonitorRight.m_fAutoTrigger = 0.65f;
    SteeringMonitorRight.t_fSustain = 0.2f;
    SteeringMonitorRight.t_TriggerLength = 0.4f;

    AccelMonitor.Initialize(8);
    AccelMonitor.fSign = 1.0f;
    AccelMonitor.m_fThreshold = 0.2f;
    AccelMonitor.m_fAutoTrigger = 0.6f;
    AccelMonitor.t_fSustain = 0.1f;
    AccelMonitor.t_TriggerLength = 0.4f;

    DeccelMonitor.Initialize(8);
    DeccelMonitor.fSign = 1.0f;
    DeccelMonitor.m_fThreshold = 0.2f;
    DeccelMonitor.m_fAutoTrigger = 0.6f;
    DeccelMonitor.t_fSustain = 0.1f;
    DeccelMonitor.t_TriggerLength = 0.4f;

    ThrottleMonitor.Initialize(8);
    ThrottleMonitor.fSign = 1.0f;
    ThrottleMonitor.m_fThreshold = 0.2f;
    ThrottleMonitor.m_fAutoTrigger = 0.55f;
    ThrottleMonitor.t_fSustain = 0.15f;
    ThrottleMonitor.t_TriggerLength = 0.4f;

    CurState = SND_AI_STATE_IDLE;
    PrevState = SND_AI_STATE_UNKNOWN;
    bTransition = 0;
    m_tLastSwitch = 0.0f;
}

void SndAIStateManager::GeneratePotentialStates(bool *pPotentialStates) {
    for (int i = 0; i < MAX_NUM_SND_AI_STATE; i++) {
        pPotentialStates[i] = false;
    }

    pPotentialStates[SND_AI_STATE_IDLE] = true;
    pPotentialStates[SND_AI_STATE_ACCEL] = AccelMonitor.bTrigger || ThrottleMonitor.bTrigger;
    pPotentialStates[SND_AI_STATE_DECEL] = DeccelMonitor.bTrigger;
    pPotentialStates[SND_AI_STATE_CORNER_LEFT] = SteeringMonitorLeft.bTrigger;
    pPotentialStates[SND_AI_STATE_CORNER_RIGHT] = SteeringMonitorRight.bTrigger;

    if (m_pPhysicsCTL != nullptr && m_pPhysicsCTL->eCurNisRevingState != NIS_DISABLED) {
        pPotentialStates[SND_AI_STATE_PRERACE] = true;
    }
}

void SndAIStateManager::UpdateState(float t) {
    bool potentialStates[MAX_NUM_SND_AI_STATE];
    GeneratePotentialStates(potentialStates);

    SND_AI_STATE newState = SND_AI_STATE_IDLE;
    if (potentialStates[SND_AI_STATE_PRERACE]) {
        newState = SND_AI_STATE_PRERACE;
    } else if (potentialStates[SND_AI_STATE_CORNER_LEFT]) {
        newState = SND_AI_STATE_CORNER_LEFT;
    } else if (potentialStates[SND_AI_STATE_CORNER_RIGHT]) {
        newState = SND_AI_STATE_CORNER_RIGHT;
    } else if (potentialStates[SND_AI_STATE_ACCEL]) {
        newState = SND_AI_STATE_ACCEL;
    } else if (potentialStates[SND_AI_STATE_DECEL]) {
        newState = SND_AI_STATE_DECEL;
    }

    if (newState != CurState) {
        SwitchState(newState);
    } else if (bTransition && (SndBase::m_fRunningTime - m_tLastSwitch > t)) {
        bTransition = 0;
    }
}

void SndAIStateManager::Update(float t) {
    if (m_pPhysicsCTL == nullptr) {
        return;
    }

    EAX_CarState *car = *static_cast<EAX_CarState **>(
        static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pPhysicsCTL->GetStateBase())) + 0x34));

    float steering = static_cast< float >(car->mSteering) * 0.005493248f;
    if (180.0f < steering) {
        steering -= 360.0f;
    }

    SteeringMonitorLeft.Update(steering, t);
    SteeringMonitorRight.Update(steering, t);

    float vel0Length = bSqrt(car->mVel0.x * car->mVel0.x + car->mVel0.y * car->mVel0.y + car->mVel0.z * car->mVel0.z);
    float vel1Length = bSqrt(car->mVel1.x * car->mVel1.x + car->mVel1.y * car->mVel1.y + car->mVel1.z * car->mVel1.z);
    AccelMonitor.Update(vel0Length - vel1Length, t);

    vel0Length = bSqrt(car->mVel0.x * car->mVel0.x + car->mVel0.y * car->mVel0.y + car->mVel0.z * car->mVel0.z);
    vel1Length = bSqrt(car->mVel1.x * car->mVel1.x + car->mVel1.y * car->mVel1.y + car->mVel1.z * car->mVel1.z);
    DeccelMonitor.Update(vel0Length - vel1Length, t);

    ThrottleMonitor.Update(m_pPhysicsCTL->m_fThrottle, t);

    UpdateState(t);
}
