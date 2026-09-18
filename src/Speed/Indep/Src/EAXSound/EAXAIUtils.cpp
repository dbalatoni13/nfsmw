#include "Speed/Indep/Src/EAXSound/EAXAIUtils.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <types.h>

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

SndAITrigger::SndAITrigger()
    : AvgMonitor() //
{
    this->m_fThreshold = 0.0f;
    this->t_fSustain = 0.0f;
    this->m_fAutoTrigger = 0.0f;
    this->t_TriggerLength = 0.0f;
    this->CurSustain = -1.0f;
    this->CurTriggerLength = -1.0f;
    this->CurValue = -1.0f;
    this->fSign = 1.0f;
    this->bTrigger = false;
}

SndAITrigger::~SndAITrigger() {}

void SndAITrigger::Initialize(int AvgSize) {
    this->AvgMonitor.Init(AvgSize);
    this->AvgMonitor.Reset(0.0f);
    this->CurSustain = this->t_fSustain;
}

void SndAITrigger::BeginTrigger() {
    this->bTrigger = true;
    this->CurTriggerLength = this->t_TriggerLength;
}

void SndAITrigger::EndTrigger() {
    this->bTrigger = false;
    this->CurSustain = this->t_fSustain;
}

void SndAITrigger::Update(float UpdateVal, float t) {
    this->AvgMonitor.Record(UpdateVal);
    this->AvgMonitor.Recalculate();

    this->CurValue = this->AvgMonitor.GetValue();

    if (!this->bTrigger) {
        if (this->fSign * this->CurValue > this->fSign * this->m_fAutoTrigger) {
            this->BeginTrigger();
            return;
        }
        if (this->fSign * this->CurValue > this->fSign * this->m_fThreshold) {
            this->CurSustain -= t;
            if (this->CurSustain < 0.0f) {
                this->BeginTrigger();
            }
            return;
        }
        this->CurSustain = this->t_fSustain;
        return;
    }

    if (this->fSign * this->CurValue > this->fSign * this->m_fThreshold) {
        this->CurTriggerLength = this->t_TriggerLength;
    } else {
        this->CurTriggerLength -= t;
        if (this->CurTriggerLength < 0.0f) {
            this->EndTrigger();
        }
    }
}

SndAIStateManager::SndAIStateManager()
    : SteeringMonitorLeft() //
    , SteeringMonitorRight() //
    , AccelMonitor() //
    , DeccelMonitor() //
    , ThrottleMonitor() {
    this->bTransition = false;
    this->CurState = SND_AI_STATE_UNKNOWN;
    this->PrevState = SND_AI_STATE_UNKNOWN;
    this->m_pPhysicsCTL = nullptr;
    this->m_tLastSwitch = 0.0f;
}

SndAIStateManager::~SndAIStateManager() {}

void SndAIStateManager::Initialize(SFXCTL_Physics *_m_pPhysicsCTL) {
    this->m_pPhysicsCTL = _m_pPhysicsCTL;

    this->SteeringMonitorLeft.m_fThreshold = TWK_SND_SteeringMonitor[1];
    this->SteeringMonitorLeft.m_fAutoTrigger = TWK_SND_SteeringMonitor[2];
    this->SteeringMonitorLeft.t_fSustain = TWK_SND_SteeringMonitor[3];
    this->SteeringMonitorLeft.t_TriggerLength = TWK_SND_SteeringMonitor[4];
    this->SteeringMonitorLeft.fSign = TWK_SND_SteeringMonitor[5];
    this->SteeringMonitorLeft.Initialize(static_cast<int>(TWK_SND_SteeringMonitor[0]));

    this->SteeringMonitorRight.m_fThreshold = -TWK_SND_SteeringMonitor[1];
    this->SteeringMonitorRight.m_fAutoTrigger = -TWK_SND_SteeringMonitor[2];
    this->SteeringMonitorRight.t_fSustain = TWK_SND_SteeringMonitor[3];
    this->SteeringMonitorRight.t_TriggerLength = TWK_SND_SteeringMonitor[4];
    this->SteeringMonitorRight.fSign = -TWK_SND_SteeringMonitor[5];
    this->SteeringMonitorRight.Initialize(static_cast<int>(TWK_SND_SteeringMonitor[0]));

    this->AccelMonitor.m_fThreshold = TWK_SND_AccelMonitor[1];
    this->AccelMonitor.m_fAutoTrigger = TWK_SND_AccelMonitor[2];
    this->AccelMonitor.t_fSustain = TWK_SND_AccelMonitor[3];
    this->AccelMonitor.t_TriggerLength = TWK_SND_AccelMonitor[4];
    this->AccelMonitor.fSign = TWK_SND_AccelMonitor[5];
    this->AccelMonitor.Initialize(static_cast<int>(TWK_SND_AccelMonitor[0]));

    this->DeccelMonitor.m_fThreshold = TWK_SND_DeccelMonitor[1];
    this->DeccelMonitor.m_fAutoTrigger = TWK_SND_DeccelMonitor[2];
    this->DeccelMonitor.t_fSustain = TWK_SND_DeccelMonitor[3];
    this->DeccelMonitor.t_TriggerLength = TWK_SND_DeccelMonitor[4];
    this->DeccelMonitor.fSign = TWK_SND_DeccelMonitor[5];
    this->DeccelMonitor.Initialize(static_cast<int>(TWK_SND_DeccelMonitor[0]));

    this->ThrottleMonitor.m_fThreshold = TWK_SND_ThrottleMonitor[1];
    this->ThrottleMonitor.m_fAutoTrigger = TWK_SND_ThrottleMonitor[2];
    this->ThrottleMonitor.t_fSustain = TWK_SND_ThrottleMonitor[3];
    this->ThrottleMonitor.t_TriggerLength = TWK_SND_ThrottleMonitor[4];
    this->ThrottleMonitor.fSign = TWK_SND_ThrottleMonitor[5];
    this->ThrottleMonitor.Initialize(static_cast<int>(TWK_SND_ThrottleMonitor[0]));
}

void SndAIStateManager::Update(float t) {
    float steering = static_cast<float>(this->m_pPhysicsCTL->GetPhysCar()->GetSteering()) * 0.005493248f;
    if (180.0f < steering) {
        steering -= 360.0f;
    }

    this->SteeringMonitorLeft.Update(steering, t);
    this->SteeringMonitorRight.Update(steering, t);

    this->AccelMonitor.Update(this->m_pPhysicsCTL->GetPhysCar()->GetVelocityMagnitude()
                                 - bLength(this->m_pPhysicsCTL->GetPhysCar()->GetOldVel()), t);

    this->DeccelMonitor.Update(this->m_pPhysicsCTL->GetPhysCar()->GetVelocityMagnitude()
                                  - bLength(this->m_pPhysicsCTL->GetPhysCar()->GetOldVel()), t);

    this->ThrottleMonitor.Update(this->m_pPhysicsCTL->m_fThrottle, t);

    this->UpdateState(t);
}

// Decl: 296
#define CHANGESTATE(NewState)                                                                                                                        \
    {                                                                                                                                                \
        SwitchState(NewState);                                                                                                                       \
        break;                                                                                                                                       \
    }

// Decl: 297
#define CHECK_TIME_REMAINING()                                                                                                                       \
    {                                                                                                                                                \
        if (m_tLastSwitch > m_pPhysicsCTL->m_fRunningTime - MIN_StateSustainTime[CurState])                                                          \
            break;                                                                                                                                   \
    }

void SndAIStateManager::UpdateState(float t) {
    bool PossibleState[MAX_NUM_SND_AI_STATE];

    this->bTransition = false;
    this->GeneratePotentialStates(PossibleState);

    switch (this->CurState) {
        case SND_AI_STATE_UNKNOWN:
            if (!PossibleState[SND_AI_STATE_PRERACE]) {
                CHANGESTATE(SND_AI_STATE_ACCEL);
            }

            CHANGESTATE(SND_AI_STATE_PRERACE);

        case SND_AI_STATE_PRERACE:
            if (!PossibleState[SND_AI_STATE_PRERACE]) {
                CHANGESTATE(SND_AI_STATE_ACCEL);
            }
            break;

        case SND_AI_STATE_IDLE:
            if (PossibleState[SND_AI_STATE_ACCEL]) {
                CHANGESTATE(SND_AI_STATE_ACCEL);
            }
            break;

        case SND_AI_STATE_ACCEL:
            CHECK_TIME_REMAINING();

            if (PossibleState[SND_AI_STATE_CORNER_LEFT]) {
                CHANGESTATE(SND_AI_STATE_CORNER_LEFT);
            }

            if (PossibleState[SND_AI_STATE_CORNER_RIGHT]) {
                CHANGESTATE(SND_AI_STATE_CORNER_RIGHT);
            }

            if (PossibleState[SND_AI_STATE_DECEL]) {
                CHANGESTATE(SND_AI_STATE_DECEL);
            }

            if (PossibleState[SND_AI_STATE_IDLE]) {
                CHANGESTATE(SND_AI_STATE_IDLE);
            }
            break;

        case SND_AI_STATE_DECEL:
            CHECK_TIME_REMAINING();

            if (PossibleState[SND_AI_STATE_CORNER_LEFT]) {
                CHANGESTATE(SND_AI_STATE_CORNER_LEFT);
            }

            if (PossibleState[SND_AI_STATE_CORNER_RIGHT]) {
                CHANGESTATE(SND_AI_STATE_CORNER_RIGHT);
            }

            if (PossibleState[SND_AI_STATE_ACCEL] && !PossibleState[SND_AI_STATE_DECEL]) {
                CHANGESTATE(SND_AI_STATE_ACCEL);
            }
            break;

        case SND_AI_STATE_CORNER_LEFT:
        case SND_AI_STATE_CORNER_RIGHT:
            CHECK_TIME_REMAINING();

            if (PossibleState[SND_AI_STATE_ACCEL]) {
                if (!PossibleState[SND_AI_STATE_DECEL]) {
                    CHANGESTATE(SND_AI_STATE_ACCEL);
                }
            } else if (!PossibleState[SND_AI_STATE_DECEL]) {
                break;
            }

            CHANGESTATE(SND_AI_STATE_DECEL);

        default:
            break;
    }
}

void SndAIStateManager::SwitchState(SND_AI_STATE NewState) {
    this->m_tLastSwitch = SndBase::m_fRunningTime;
    this->PrevState = this->CurState;
    this->CurState = NewState;
    this->bTransition = true;
}

void SndAIStateManager::GeneratePotentialStates(bool *ArrayList) {
    ArrayList[SND_AI_STATE_PRERACE] = false;
    if (this->m_pPhysicsCTL->GetPhysCar()->GetVelocityMagnitudeMPH() < 5.0f) {
        ArrayList[SND_AI_STATE_IDLE] = true;
    } else {
        ArrayList[SND_AI_STATE_IDLE] = false;
    }

    if (this->AccelMonitor.IsTriggering()) {
        ArrayList[SND_AI_STATE_ACCEL] = true;
    } else {
        ArrayList[SND_AI_STATE_ACCEL] = false;
    }

    if (this->DeccelMonitor.IsTriggering()) {
        ArrayList[SND_AI_STATE_DECEL] = true;
    } else {
        ArrayList[SND_AI_STATE_DECEL] = false;
    }

    if (this->DeccelMonitor.IsTriggering() && this->SteeringMonitorLeft.IsTriggering()) {
        ArrayList[SND_AI_STATE_CORNER_LEFT] = true;
    } else {
        ArrayList[SND_AI_STATE_CORNER_LEFT] = false;
    }

    if (this->DeccelMonitor.IsTriggering()) {
        if (this->SteeringMonitorRight.IsTriggering()) {
            ArrayList[SND_AI_STATE_CORNER_RIGHT] = true;
            return;
        }
    }

    ArrayList[SND_AI_STATE_CORNER_RIGHT] = false;
}
