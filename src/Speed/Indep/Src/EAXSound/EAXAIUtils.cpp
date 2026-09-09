#include "Speed/Indep/Src/EAXSound/EAXAIUtils.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/SimStates/EAX_HeliState.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"

#define AVGSIZE 0         // Decl: 5
#define THRESHOLD 1       // Decl: 6
#define FORCETRIGGER 2    // Decl: 7
#define SUSTAIN 3         // Decl: 8
#define TRIGGERLENGTH 4   // Decl: 9
#define POSITIVETRIGGER 5 // Decl: 10

static const int PRINT_AI_STATE_INFO = 0; // Decl: 12
int SPEW_AI_STATE_INFO = 0;               // Decl: 13

// Decl: 16
static const float TWK_SND_SteeringMonitor[6] = {
    15.0f, // AVGSIZE
    1.0f,  // THRESHOLD
    3.0f,  // FORCETRIGGER
    0.5f,  // SUSTAIN
    0.5f,  // TRIGGERLENGTH
    1.0f,  // POSITIVETRIGGER
};

// Decl: 27
static const float TWK_SND_AccelMonitor[6] = {
    3.0f, // AVGSIZE
    0.0f, // THRESHOLD
    1.0f, // FORCETRIGGER
    0.4f, // SUSTAIN
    0.5f, // TRIGGERLENGTH
    1.0f, // POSITIVETRIGGER
};

// Decl: 38
static const float TWK_SND_DeccelMonitor[6] = {
    3.0f,  // AVGSIZE
    -0.1f, // THRESHOLD
    -1.0f, // FORCETRIGGER
    0.1f,  // SUSTAIN
    0.5f,  // TRIGGERLENGTH
    -1.0f, // POSITIVETRIGGER
};

// Decl: 49
static const float TWK_SND_ThrottleMonitor[6] = {
    2.0f,  // AVGSIZE
    80.0f, // THRESHOLD
    30.0f, // FORCETRIGGER
    0.2f,  // SUSTAIN
    0.5f,  // TRIGGERLENGTH
    -1.0f, // POSITIVETRIGGER
};

// Decl: 60
static const float MIN_StateSustainTime[7] = {
    1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f,
};

#define GetAIStateName(Name) ; // Decl: 91

SndAITrigger::SndAITrigger() {
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

SndAIStateManager::SndAIStateManager() {
    this->bTransition = false;
    this->CurState = SND_AI_STATE_UNKNOWN;
    this->PrevState = SND_AI_STATE_UNKNOWN;
    this->m_pPhysicsCTL = nullptr;
    this->m_tLastSwitch = 0.0f;
}

SndAIStateManager::~SndAIStateManager() {}

void SndAIStateManager::Initialize(SFXCTL_Physics *_m_pPhysicsCTL) {
    this->m_pPhysicsCTL = _m_pPhysicsCTL;

    this->SteeringMonitorLeft.m_fThreshold = TWK_SND_SteeringMonitor[THRESHOLD];
    this->SteeringMonitorLeft.m_fAutoTrigger = TWK_SND_SteeringMonitor[FORCETRIGGER];
    this->SteeringMonitorLeft.t_fSustain = TWK_SND_SteeringMonitor[SUSTAIN];
    this->SteeringMonitorLeft.t_TriggerLength = TWK_SND_SteeringMonitor[TRIGGERLENGTH];
    this->SteeringMonitorLeft.fSign = TWK_SND_SteeringMonitor[POSITIVETRIGGER];
    this->SteeringMonitorLeft.Initialize(static_cast<int>(TWK_SND_SteeringMonitor[AVGSIZE]));

    this->SteeringMonitorRight.m_fThreshold = -TWK_SND_SteeringMonitor[THRESHOLD];
    this->SteeringMonitorRight.m_fAutoTrigger = -TWK_SND_SteeringMonitor[FORCETRIGGER];
    this->SteeringMonitorRight.t_fSustain = TWK_SND_SteeringMonitor[SUSTAIN];
    this->SteeringMonitorRight.t_TriggerLength = TWK_SND_SteeringMonitor[TRIGGERLENGTH];
    this->SteeringMonitorRight.fSign = -TWK_SND_SteeringMonitor[POSITIVETRIGGER];
    this->SteeringMonitorRight.Initialize(static_cast<int>(TWK_SND_SteeringMonitor[AVGSIZE]));

    this->AccelMonitor.m_fThreshold = TWK_SND_AccelMonitor[THRESHOLD];
    this->AccelMonitor.m_fAutoTrigger = TWK_SND_AccelMonitor[FORCETRIGGER];
    this->AccelMonitor.t_fSustain = TWK_SND_AccelMonitor[SUSTAIN];
    this->AccelMonitor.t_TriggerLength = TWK_SND_AccelMonitor[TRIGGERLENGTH];
    this->AccelMonitor.fSign = TWK_SND_AccelMonitor[POSITIVETRIGGER];
    this->AccelMonitor.Initialize(static_cast<int>(TWK_SND_AccelMonitor[AVGSIZE]));

    this->DeccelMonitor.m_fThreshold = TWK_SND_DeccelMonitor[THRESHOLD];
    this->DeccelMonitor.m_fAutoTrigger = TWK_SND_DeccelMonitor[FORCETRIGGER];
    this->DeccelMonitor.t_fSustain = TWK_SND_DeccelMonitor[SUSTAIN];
    this->DeccelMonitor.t_TriggerLength = TWK_SND_DeccelMonitor[TRIGGERLENGTH];
    this->DeccelMonitor.fSign = TWK_SND_DeccelMonitor[POSITIVETRIGGER];
    this->DeccelMonitor.Initialize(static_cast<int>(TWK_SND_DeccelMonitor[AVGSIZE]));

    this->ThrottleMonitor.m_fThreshold = TWK_SND_ThrottleMonitor[THRESHOLD];
    this->ThrottleMonitor.m_fAutoTrigger = TWK_SND_ThrottleMonitor[FORCETRIGGER];
    this->ThrottleMonitor.t_fSustain = TWK_SND_ThrottleMonitor[SUSTAIN];
    this->ThrottleMonitor.t_TriggerLength = TWK_SND_ThrottleMonitor[TRIGGERLENGTH];
    this->ThrottleMonitor.fSign = TWK_SND_ThrottleMonitor[POSITIVETRIGGER];
    this->ThrottleMonitor.Initialize(static_cast<int>(TWK_SND_ThrottleMonitor[AVGSIZE]));
}

void SndAIStateManager::Update(float t) {
    float SteeringVal = static_cast<float>(this->m_pPhysicsCTL->GetPhysCar()->GetSteering()) * 0.005493248f;
    if (180.0f < SteeringVal) {
        SteeringVal -= 360.0f;
    }

    this->SteeringMonitorLeft.Update(SteeringVal, t);
    this->SteeringMonitorRight.Update(SteeringVal, t);

    this->AccelMonitor.Update(this->m_pPhysicsCTL->GetPhysCar()->GetVelocityMagnitude() - bLength(this->m_pPhysicsCTL->GetPhysCar()->GetOldVel()), t);

    this->DeccelMonitor.Update(this->m_pPhysicsCTL->GetPhysCar()->GetVelocityMagnitude() - bLength(this->m_pPhysicsCTL->GetPhysCar()->GetOldVel()),
                               t);

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
