#include "Speed/Indep/Src/EAXSound/EAXAIUtils.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <types.h>

extern "C" void Average_Record(Average *avg, float value) asm("Record__7Averagef");
extern "C" void AverageBase_Recalculate(AverageBase *avg) asm("Recalculate__11AverageBase");

static const float MIN_StateSustainTime[MAX_NUM_SND_AI_STATE] = {
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    0.5f,
    0.5f,
    0.5f,
};

static const float TWK_SND_SteeringMonitor[6] = {
    15.0f,
    1.0f,
    3.0f,
    0.5f,
    0.5f,
    1.0f,
};

static const float TWK_SND_AccelMonitor[6] = {
    3.0f,
    0.0f,
    1.0f,
    0.4f,
    0.5f,
    1.0f,
};

static const float TWK_SND_DeccelMonitor[6] = {
    3.0f,
    -0.1f,
    -1.0f,
    0.1f,
    0.5f,
    -1.0f,
};

static const float TWK_SND_ThrottleMonitor[6] = {
    2.0f,
    80.0f,
    30.0f,
    0.2f,
    0.5f,
    -1.0f,
};

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

    CurValue = static_cast<const Average &>(AvgMonitor).GetValue();

    if (!bTrigger) {
        if (fSign * CurValue > fSign * m_fAutoTrigger) {
            BeginTrigger();
            return;
        }
        if (fSign * CurValue > fSign * m_fThreshold) {
            CurSustain -= t;
            if (CurSustain < 0.0f) {
                BeginTrigger();
            }
            return;
        }
        CurSustain = t_fSustain;
        return;
    }

    if (fSign * CurValue > fSign * m_fThreshold) {
        CurTriggerLength = t_TriggerLength;
    } else {
        CurTriggerLength -= t;
        if (CurTriggerLength < 0.0f) {
            EndTrigger();
        }
    }
}

void SndAIStateManager::SwitchState(SND_AI_STATE NewState) {
    m_tLastSwitch = SndBase::m_fRunningTime;
    PrevState = CurState;
    CurState = NewState;
    *(int *)&bTransition = 1;
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

void SndAIStateManager::Initialize(SFXCTL_Physics *_m_pPhysicsCTL) {
    m_pPhysicsCTL = _m_pPhysicsCTL;

    SteeringMonitorLeft.m_fThreshold = TWK_SND_SteeringMonitor[1];
    SteeringMonitorLeft.m_fAutoTrigger = TWK_SND_SteeringMonitor[2];
    SteeringMonitorLeft.t_fSustain = TWK_SND_SteeringMonitor[3];
    SteeringMonitorLeft.t_TriggerLength = TWK_SND_SteeringMonitor[4];
    SteeringMonitorLeft.fSign = TWK_SND_SteeringMonitor[5];
    SteeringMonitorLeft.Initialize(static_cast< int >(TWK_SND_SteeringMonitor[0]));

    SteeringMonitorRight.m_fThreshold = -TWK_SND_SteeringMonitor[1];
    SteeringMonitorRight.m_fAutoTrigger = -TWK_SND_SteeringMonitor[2];
    SteeringMonitorRight.t_fSustain = TWK_SND_SteeringMonitor[3];
    SteeringMonitorRight.t_TriggerLength = TWK_SND_SteeringMonitor[4];
    SteeringMonitorRight.fSign = -TWK_SND_SteeringMonitor[5];
    SteeringMonitorRight.Initialize(static_cast< int >(TWK_SND_SteeringMonitor[0]));

    AccelMonitor.m_fThreshold = TWK_SND_AccelMonitor[1];
    AccelMonitor.m_fAutoTrigger = TWK_SND_AccelMonitor[2];
    AccelMonitor.t_fSustain = TWK_SND_AccelMonitor[3];
    AccelMonitor.t_TriggerLength = TWK_SND_AccelMonitor[4];
    AccelMonitor.fSign = TWK_SND_AccelMonitor[5];
    AccelMonitor.Initialize(static_cast< int >(TWK_SND_AccelMonitor[0]));

    DeccelMonitor.m_fThreshold = TWK_SND_DeccelMonitor[1];
    DeccelMonitor.m_fAutoTrigger = TWK_SND_DeccelMonitor[2];
    DeccelMonitor.t_fSustain = TWK_SND_DeccelMonitor[3];
    DeccelMonitor.t_TriggerLength = TWK_SND_DeccelMonitor[4];
    DeccelMonitor.fSign = TWK_SND_DeccelMonitor[5];
    DeccelMonitor.Initialize(static_cast< int >(TWK_SND_DeccelMonitor[0]));

    ThrottleMonitor.m_fThreshold = TWK_SND_ThrottleMonitor[1];
    ThrottleMonitor.m_fAutoTrigger = TWK_SND_ThrottleMonitor[2];
    ThrottleMonitor.t_fSustain = TWK_SND_ThrottleMonitor[3];
    ThrottleMonitor.t_TriggerLength = TWK_SND_ThrottleMonitor[4];
    ThrottleMonitor.fSign = TWK_SND_ThrottleMonitor[5];
    ThrottleMonitor.Initialize(static_cast< int >(TWK_SND_ThrottleMonitor[0]));
}

void SndAIStateManager::GeneratePotentialStates(bool *ArrayList) {
    ArrayList[SND_AI_STATE_PRERACE] = false;
    if (m_pPhysicsCTL->GetStateBase()->GetPhysCar()->GetVelocityMagnitudeMPH() < 5.0f) {
        ArrayList[SND_AI_STATE_IDLE] = true;
    } else {
        ArrayList[SND_AI_STATE_IDLE] = false;
    }

    if (AccelMonitor.IsTriggering()) {
        ArrayList[SND_AI_STATE_ACCEL] = true;
    } else {
        ArrayList[SND_AI_STATE_ACCEL] = false;
    }

    if (DeccelMonitor.IsTriggering()) {
        ArrayList[SND_AI_STATE_DECEL] = true;
    } else {
        ArrayList[SND_AI_STATE_DECEL] = false;
    }

    if (DeccelMonitor.IsTriggering()) {
        if (SteeringMonitorLeft.IsTriggering()) {
            ArrayList[SND_AI_STATE_CORNER_LEFT] = true;
        } else {
            ArrayList[SND_AI_STATE_CORNER_LEFT] = false;
        }
    } else {
        ArrayList[SND_AI_STATE_CORNER_LEFT] = false;
    }

    if (DeccelMonitor.IsTriggering()) {
        if (SteeringMonitorRight.IsTriggering()) {
            ArrayList[SND_AI_STATE_CORNER_RIGHT] = true;
            return;
        }
    }

    ArrayList[SND_AI_STATE_CORNER_RIGHT] = false;
}

void SndAIStateManager::UpdateState(float t) {
    bool PossibleState[MAX_NUM_SND_AI_STATE];

    static_cast<void>(t);
    bTransition = 0;
    GeneratePotentialStates(PossibleState);

    switch (CurState) {
    case SND_AI_STATE_UNKNOWN:
        if (!PossibleState[SND_AI_STATE_PRERACE]) {
            SwitchState(SND_AI_STATE_ACCEL);
            return;
        }

        SwitchState(SND_AI_STATE_PRERACE);
        return;
    case SND_AI_STATE_PRERACE:
        if (!PossibleState[SND_AI_STATE_PRERACE]) {
            SwitchState(SND_AI_STATE_ACCEL);
        }
        return;
    case SND_AI_STATE_IDLE:
        if (PossibleState[SND_AI_STATE_ACCEL]) {
            SwitchState(SND_AI_STATE_ACCEL);
        }
        return;
    case SND_AI_STATE_ACCEL:
        if (m_tLastSwitch > SndBase::m_fRunningTime - MIN_StateSustainTime[SND_AI_STATE_ACCEL]) {
            return;
        }

        if (PossibleState[SND_AI_STATE_CORNER_LEFT]) {
            SwitchState(SND_AI_STATE_CORNER_LEFT);
            return;
        }

        if (PossibleState[SND_AI_STATE_CORNER_RIGHT]) {
            SwitchState(SND_AI_STATE_CORNER_RIGHT);
            return;
        }

        if (PossibleState[SND_AI_STATE_DECEL]) {
            SwitchState(SND_AI_STATE_DECEL);
            return;
        }

        if (PossibleState[SND_AI_STATE_IDLE]) {
            SwitchState(SND_AI_STATE_IDLE);
        }
        return;
    case SND_AI_STATE_DECEL:
        if (m_tLastSwitch > SndBase::m_fRunningTime - MIN_StateSustainTime[SND_AI_STATE_DECEL]) {
            return;
        }

        if (PossibleState[SND_AI_STATE_CORNER_LEFT]) {
            SwitchState(SND_AI_STATE_CORNER_LEFT);
            return;
        }

        if (PossibleState[SND_AI_STATE_CORNER_RIGHT]) {
            SwitchState(SND_AI_STATE_CORNER_RIGHT);
            return;
        }

        if (PossibleState[SND_AI_STATE_ACCEL] && !PossibleState[SND_AI_STATE_DECEL]) {
            SwitchState(SND_AI_STATE_ACCEL);
        }
        return;
    case SND_AI_STATE_CORNER_LEFT:
    case SND_AI_STATE_CORNER_RIGHT:
        if (m_tLastSwitch > SndBase::m_fRunningTime - MIN_StateSustainTime[CurState]) {
            return;
        }

        if (PossibleState[SND_AI_STATE_ACCEL]) {
            if (!PossibleState[SND_AI_STATE_DECEL]) {
                SwitchState(SND_AI_STATE_ACCEL);
                return;
            }
        } else if (!PossibleState[SND_AI_STATE_DECEL]) {
            return;
        }

        SwitchState(SND_AI_STATE_DECEL);
        return;
    default:
        return;
    }
}

void SndAIStateManager::Update(float t) {
    if (m_pPhysicsCTL == nullptr) {
        return;
    }

    float steering = static_cast< float >(m_pPhysicsCTL->GetPhysCar()->GetSteering()) * 0.005493248f;
    if (180.0f < steering) {
        steering -= 360.0f;
    }

    SteeringMonitorLeft.Update(steering, t);
    SteeringMonitorRight.Update(steering, t);

    float vel0Length = m_pPhysicsCTL->GetPhysCar()->GetVelocityMagnitude();
    float vel1Length = bLength(m_pPhysicsCTL->GetPhysCar()->GetOldVel());
    AccelMonitor.Update(vel0Length - vel1Length, t);

    vel0Length = m_pPhysicsCTL->GetPhysCar()->GetVelocityMagnitude();
    vel1Length = bLength(m_pPhysicsCTL->GetPhysCar()->GetOldVel());
    DeccelMonitor.Update(vel0Length - vel1Length, t);

    ThrottleMonitor.Update(m_pPhysicsCTL->m_fThrottle, t);

    UpdateState(t);
}
