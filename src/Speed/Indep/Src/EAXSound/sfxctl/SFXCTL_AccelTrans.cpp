#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"

static const float AccelAttack_PEAK_VOL = 0.8f;            // size: 0x4, Decl: 17
static const int AccelAttack_PEAK_VOL_T = 200;             // size: 0x4, Decl: 18
static const float AccelAttack_PEAK_RPM = 1000.0f;         // size: 0x4, Decl: 19
static const int AccelAttack_PEAK_RPM_T = 500;             // size: 0x4, Decl: 20
static const float AccelAttackThrottleSensitivity = 30.0f; // size: 0x4, Decl: 21
static const int DisableAccelTransition = 0;               // size: 0x4, Decl: 22

DEFINE_CREATABLE(0x20030, SFXCTL_AccelTrans, SFXCTL);

SFXCTL_AccelTrans::SFXCTL_AccelTrans()
    : m_pEngineCtl(nullptr), //
      m_pShiftCtl(nullptr),  //
      m_pAccelTransDataSet(nullptr) {}

SFXCTL_AccelTrans::~SFXCTL_AccelTrans() {}

void SFXCTL_AccelTrans::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_UGL = this->m_pEAXCar->GetEngineUpgradeLevel();
    this->m_pAccelTransDataSet = &this->m_pEAXCar->GetAccelInfo();
}

void SFXCTL_AccelTrans::InitSFX() {
    SFXCTL::InitSFX();
    this->eAccelTransFxState = FX_ACCEL_STATE_NONE;
    this->t_LastAccelTrans = 0.0f;
    this->IsAccelerating = false;
    this->OldIsAccelerating = false;
    this->PlayEngOffSweet = false;
}

int SFXCTL_AccelTrans::GetController(int Index) {
    switch (Index) {
        case 0:
            return 4;
        case 1:
            return 2;
        case 2:
        default:
            return -1;
    }
}

void SFXCTL_AccelTrans::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
        case 4:
            this->m_pEngineCtl = static_cast<SFXCTL_Engine *>(psfxctl);
            break;
        case 2:
            this->m_pShiftCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
            break;
    }
}

void SFXCTL_AccelTrans::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);
    if (this->m_pStateBase->m_eStateType == eMM_AIRACECAR) {
        return;
    }

    this->PlayEngOffSweet = false;
    this->OldIsAccelerating = this->IsAccelerating;
    this->IsAccelerating = this->m_pEAXCar->IsAccelerating();
    this->PlayEngOffSweet = false;

    if (this->IsAccelerating && !this->OldIsAccelerating) {
        if (this->ShouldBeginAccelTrans_Idle()) {
            this->BeginAccelTrans_Idle();
        } else if (this->ShouldBeginAccelTrans()) {
            this->BeginAccelTrans();
        }
    }

    if (!this->IsAccelerating && this->OldIsAccelerating && this->ShouldPlayEngOffSweet()) {
        this->m_pShiftCtl->m_bNeed_DeccelSnd = true;
    }

    this->UpdateState(t);
}

void SFXCTL_AccelTrans::UpdateRPM(float t) {
    switch (this->eAccelTransFxState) {
        case FX_ACCEL_STATE_NONE:
            break;

        case FX_ACCEL_STATE_IDLE_REVING:
            this->m_InterpEngRPM.Update(t);
            break;

        case FX_ACCEL_STATE_ATTACK:
        case FX_ACCEL_STATE_IDLE_ENGAGING:
            this->m_InterpEngRPM.Update(t, this->GetPhysRPM());
            break;

        case FX_ACCEL_STATE_INTERRUPT:
            this->m_InterpEngRPM.Update(t, this->GetPhysRPM());
            break;

        default:
            break;
    }
}

// TODO switch
void SFXCTL_AccelTrans::UpdateTRQ(float t) {
    if (this->eAccelTransFxState != FX_ACCEL_STATE_NONE) {
        if (this->eAccelTransFxState >= FX_ACCEL_STATE_NONE) {
            if (this->eAccelTransFxState <= FX_ACCEL_STATE_INTERRUPT) {
                this->m_InterpEngTorque.Update(t, this->GetPhysTRQ());

                if (this->m_InterpEngTorque.IsFinished()) {
                    this->m_InterpEngTorque.Initialize(this->GetPhysTRQ(), this->GetPhysTRQ(), 0, LINEAR);
                }
            }
        }
    }
}

void SFXCTL_AccelTrans::UpdateState(float t) {
    this->m_InterpEngVol.Update(t);
    this->UpdateRPM(t);
    this->UpdateTRQ(t);
    if (this->eAccelTransFxState != FX_ACCEL_STATE_NONE) {
        if (!this->m_pEAXCar->IsAccelerating() && this->eAccelTransFxState != FX_ACCEL_STATE_INTERRUPT) {
            this->m_InterpEngRPM.Initialize(this->m_pEngineCtl->GetEngRPM(), this->GetPhysRPM(),
                                            this->m_pAccelTransDataSet->AccelFromIdle_INTERUPT_T(), EQ_PWR_SQ);
            this->eAccelTransFxState = FX_ACCEL_STATE_INTERRUPT;
            this->m_InterpEngVol.Initialize(this->m_InterpEngVol.GetValue(), 0.0f, this->m_pAccelTransDataSet->AccelFromIdle_INTERUPT_T(), LINEAR);
        }
        if (!this->m_pEAXCar->IsAccelerating() && this->eAccelTransFxState != FX_ACCEL_STATE_INTERRUPT) {
            this->eAccelTransFxState = FX_ACCEL_STATE_INTERRUPT;
            this->m_InterpEngRPM.Initialize(this->m_pEngineCtl->GetEngRPM(), this->GetPhysRPM(), 100, LINEAR);
            this->m_InterpEngVol.Initialize(this->m_InterpEngVol.GetValue(), 0.0f, 100, LINEAR);
        }
        switch (this->eAccelTransFxState) {
            case FX_ACCEL_STATE_NONE:
                break;

            case FX_ACCEL_STATE_IDLE_REVING:
                if (this->m_InterpEngRPM.IsFinished()) {
                    this->eAccelTransFxState = FX_ACCEL_STATE_IDLE_ENGAGING;
                    this->m_InterpEngRPM.Initialize(this->m_InterpEngRPM.GetValue(), this->GetPhysRPM(),
                                                    this->m_pAccelTransDataSet->AccelFromIdle_RESUME_T(), LINEAR);
                    this->m_InterpEngVol.Initialize(this->m_InterpEngVol.GetValue(), 0.0f, this->m_pAccelTransDataSet->AccelFromIdle_RESUME_T(),
                                                    LINEAR);
                }
                break;

            case FX_ACCEL_STATE_INTERRUPT:
                if (this->m_InterpEngRPM.IsFinished()) {
                    this->eAccelTransFxState = FX_ACCEL_STATE_NONE;
                }
                break;

            case FX_ACCEL_STATE_ATTACK:
            case FX_ACCEL_STATE_IDLE_ENGAGING:
                if (this->m_InterpEngRPM.IsFinished()) {
                    this->eAccelTransFxState = FX_ACCEL_STATE_NONE;
                }
                break;

            default:
                break;
        }
    }
}

void SFXCTL_AccelTrans::BeginAccelTrans() {
    this->m_InterpEngRPM.Initialize(this->GetPhysRPM() + AccelAttack_PEAK_RPM, this->GetPhysRPM(), AccelAttack_PEAK_RPM_T, EQ_PWR_SQ);
    this->eAccelTransFxState = FX_ACCEL_STATE_ATTACK;
    this->m_InterpEngVol.Initialize(AccelAttack_PEAK_VOL, 0.0f, AccelAttack_PEAK_VOL_T, LINEAR);
    this->m_pShiftCtl->m_bNeed_AccelSnd = true;
    this->m_InterpEngTorque.Initialize(100.0f, 100.0f, 10, LINEAR);
    this->t_LastAccelTrans = SndBase::m_fRunningTime;
}

void SFXCTL_AccelTrans::BeginAccelTrans_Idle() {
    this->eAccelTransFxState = FX_ACCEL_STATE_IDLE_REVING;
    this->t_LastAccelTrans = SndBase::m_fRunningTime;
    this->m_InterpEngRPM.Initialize(this->GetPhysRPM(), this->GetPhysRPM() + this->m_pAccelTransDataSet->AccelFromIdle_PEAK_RPM(),
                                    this->m_pAccelTransDataSet->AccelFromIdle_PEAK_T(), LINEAR);
    this->m_InterpEngVol.Initialize(0.0f, this->m_pAccelTransDataSet->AccelFromIdle_PEAK_VOL(), this->m_pAccelTransDataSet->AccelFromIdle_PEAK_T(),
                                    LINEAR);
    this->m_InterpEngTorque.Initialize(this->GetPhysTRQ(), this->GetPhysTRQ(), 10, LINEAR);
}

bool SFXCTL_AccelTrans::ShouldBeginAccelTrans_Idle() {
    if (this->m_pEAXCar->GetVelocityMagnitudeMPH() > 15.0f) {
        return false;
    }
    if (AccelAttackThrottleSensitivity > this->m_pEAXCar->GetThrottle() - this->m_pEngineCtl->m_pPhysicsCtl->m_OldThrottle) {
        return false;
    }
    if (this->m_pEAXCar->GetPhysicsCTL()->NISRevingEnabled) {
        return false;
    }
    if (this->eAccelTransFxState != FX_ACCEL_STATE_NONE) {
        return false;
    }
    if (this->m_pShiftCtl != nullptr && this->m_pShiftCtl->IsActive()) {
        return false;
    }
    if (this->m_pEAXCar->GetCurGear() != FIRST_GEAR) {
        return false;
    }
    if (this->GetPhysRPM() > 1500.0f) {
        return false;
    }
    if (INIS::Get() != nullptr && INIS::Get()->IsPlaying()) {
        return false;
    }

    return true;
}

bool SFXCTL_AccelTrans::ShouldBeginAccelTrans() {
    if (AccelAttackThrottleSensitivity > this->m_pEAXCar->GetThrottle() - this->m_pEngineCtl->m_pPhysicsCtl->m_OldThrottle) {
        return false;
    }
    if (this->eAccelTransFxState != FX_ACCEL_STATE_NONE) {
        return false;
    }
    if ((this->m_pShiftCtl != nullptr) && this->m_pShiftCtl->IsActive()) {
        return false;
    }
    if (this->GetPhysRPM() < 3000.0f) {
        return false;
    }
    if (SndBase::m_fRunningTime - this->t_LastAccelTrans < 2.0f) {
        return false;
    }
    if (!this->GetPhysCar()->IsAICar() && this->m_pEAXCar->GetCurGear() < SECOND_GEAR) {
        return false;
    }

    return true;
}

bool SFXCTL_AccelTrans::ShouldPlayEngOffSweet() {
    if (this->eAccelTransFxState != FX_ACCEL_STATE_NONE) {
        return false;
    }
    if ((this->m_pShiftCtl != nullptr) && this->m_pShiftCtl->IsActive()) {
        return false;
    }
    if (this->GetPhysRPM() < 6000.0f) {
        return false;
    }
    if (this->m_pEAXCar->GetCurGear() < SECOND_GEAR) {
        return false;
    }

    return true;
}

void SFXCTL_AccelTrans::Destroy() {}
