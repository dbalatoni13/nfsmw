#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"

SndBase::TypeInfo *SFXCTL_AccelTrans::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_AccelTrans::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_AccelTrans::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_AccelTrans::GetStaticTypeInfo()->typeName, false) SFXCTL_AccelTrans();
    }
    return new (SFXCTL_AccelTrans::GetStaticTypeInfo()->typeName, true) SFXCTL_AccelTrans();
}

SFXCTL_AccelTrans::SFXCTL_AccelTrans()
    : m_pEngineCtl(nullptr) //
    , m_pShiftCtl(nullptr) //
    , m_pAccelTransDataSet(nullptr) {}

SFXCTL_AccelTrans::~SFXCTL_AccelTrans() {}

void SFXCTL_AccelTrans::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_UGL = static_cast<eAemsUpgradeLevel>(this->m_pEAXCar->GetEngineUpgradeLevel());
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
    if (Index == 1) {
        goto ReturnTwo;
    }
    if (Index > 1) {
        goto ReturnNegOne;
    }
    if (Index != 0) {
        goto ReturnNegOne;
    }
    return 4;

ReturnTwo:
    return 2;

ReturnNegOne:
    return -1;
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
    bool bVar1;
    bool wasAccelerating;

    SFXCTL::UpdateParams(t);
    if (this->m_pStateBase->m_eStateType == eMM_AIRACECAR) {
        return;
    }

    wasAccelerating = this->IsAccelerating;
    this->PlayEngOffSweet = false;
    this->OldIsAccelerating = wasAccelerating;
    this->IsAccelerating = this->m_pEAXCar->bIsAccelerating;

    if (this->IsAccelerating) {
        if (!wasAccelerating) {
            bVar1 = this->ShouldBeginAccelTrans_Idle();
            if (bVar1) {
                this->BeginAccelTrans_Idle();
            } else {
                bVar1 = this->ShouldBeginAccelTrans();
                if (bVar1) {
                    this->BeginAccelTrans();
                }
            }
        }

        if (this->IsAccelerating) {
            goto end;
        }
    }

    if (this->OldIsAccelerating &&
        (bVar1 = this->ShouldPlayEngOffSweet(), bVar1)) {
        this->m_pShiftCtl->m_bNeed_DeccelSnd = true;
    }

end:
    this->UpdateState(t);
}

void SFXCTL_AccelTrans::UpdateRPM(float t) {
    switch (this->eAccelTransFxState) {
    case FX_ACCEL_STATE_IDLE_REVING:
        this->m_InterpEngRPM.Update(t);
        return;

    case FX_ACCEL_STATE_ATTACK:
    case FX_ACCEL_STATE_IDLE_ENGAGING:
        this->m_InterpEngRPM.Update(t, this->GetPhysRPM());
        return;

    case FX_ACCEL_STATE_INTERRUPT:
        this->m_InterpEngRPM.Update(t, this->GetPhysRPM());
        return;

    default:
        return;
    }
}

void SFXCTL_AccelTrans::UpdateTRQ(float t) {
    if (this->eAccelTransFxState > FX_ACCEL_STATE_NONE && this->eAccelTransFxState < 5) {
        this->m_InterpEngTorque.Update(t, this->GetPhysTRQ());
        if (this->m_InterpEngTorque.IsFinished()) {
            this->m_InterpEngTorque.Initialize(this->GetPhysTRQ(), this->GetPhysTRQ(), 0, LINEAR);
        }
    }
}

void SFXCTL_AccelTrans::UpdateState(float t) {
    this->m_InterpEngVol.Update(t);
    this->UpdateRPM(t);
    this->UpdateTRQ(t);
    if (this->eAccelTransFxState != FX_ACCEL_STATE_NONE) {
        if (!this->m_pEAXCar->IsAccelerating() && this->eAccelTransFxState != FX_ACCEL_STATE_INTERRUPT) {
            this->m_InterpEngRPM.Initialize(
                this->m_pEngineCtl->GetEngRPM(), this->GetPhysRPM(), this->m_pAccelTransDataSet->AccelFromIdle_INTERUPT_T(), EQ_PWR_SQ
            );
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
                this->m_InterpEngRPM.Initialize(this->m_InterpEngRPM.GetValue(), this->GetPhysRPM(), this->m_pAccelTransDataSet->AccelFromIdle_RESUME_T(), LINEAR);
                this->m_InterpEngVol.Initialize(this->m_InterpEngVol.GetValue(), 0.0f, this->m_pAccelTransDataSet->AccelFromIdle_RESUME_T(), LINEAR);
            }
            break;

        case FX_ACCEL_STATE_IDLE_ENGAGING:
            if (this->m_InterpEngRPM.IsFinished()) {
                this->eAccelTransFxState = FX_ACCEL_STATE_NONE;
            }
            break;

        case FX_ACCEL_STATE_INTERRUPT:
            if (this->m_InterpEngRPM.IsFinished()) {
                this->eAccelTransFxState = FX_ACCEL_STATE_NONE;
            }
            break;

        case FX_ACCEL_STATE_ATTACK:
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
    this->m_InterpEngRPM.Initialize(this->GetPhysRPM() + 1000.0f, this->GetPhysRPM(), 500, EQ_PWR_SQ);
    this->eAccelTransFxState = FX_ACCEL_STATE_ATTACK;
    this->m_InterpEngVol.Initialize(0.8f, 0.0f, 200, LINEAR);
    this->m_pShiftCtl->m_bNeed_AccelSnd = true;
    this->m_InterpEngTorque.Initialize(100.0f, 100.0f, 10, LINEAR);
    this->t_LastAccelTrans = SndBase::m_fRunningTime;
}

void SFXCTL_AccelTrans::BeginAccelTrans_Idle() {
    this->eAccelTransFxState = FX_ACCEL_STATE_IDLE_REVING;
    this->t_LastAccelTrans = SndBase::m_fRunningTime;
    this->m_InterpEngRPM.Initialize(this->GetPhysRPM(), this->GetPhysRPM() + this->m_pAccelTransDataSet->AccelFromIdle_PEAK_RPM(),
                              this->m_pAccelTransDataSet->AccelFromIdle_PEAK_T(), LINEAR);
    this->m_InterpEngVol.Initialize(0.0f, this->m_pAccelTransDataSet->AccelFromIdle_PEAK_VOL(),
                              this->m_pAccelTransDataSet->AccelFromIdle_PEAK_T(), LINEAR);
    this->m_InterpEngTorque.Initialize(this->GetPhysTRQ(), this->GetPhysTRQ(), 10, LINEAR);
}

bool SFXCTL_AccelTrans::ShouldBeginAccelTrans_Idle() {
    if (this->m_pEAXCar->GetVelocityMagnitudeMPH() <= 15.0f &&
        30.0f <= this->m_pEAXCar->GetThrottle() - this->m_pEngineCtl->m_pPhysicsCtl->m_OldThrottle &&
        !this->m_pEAXCar->GetPhysicsCTL()->NISRevingEnabled) {
        if (this->eAccelTransFxState != FX_ACCEL_STATE_NONE) {
            return false;
        }

        if ((!this->m_pShiftCtl || !this->m_pShiftCtl->IsActive()) &&
            this->m_pEAXCar->GetCurGear() == Sound::FIRST_GEAR && this->m_pEAXCar->GetPhysRPM() <= 1500.0f) {
            if (INIS::Get() && INIS::Get()->IsPlaying()) {
                return false;
            }

            return true;
        }
    }

    return false;
}

bool SFXCTL_AccelTrans::ShouldBeginAccelTrans() {
    if (!(30.0f <= this->m_pEAXCar->GetThrottle() - this->m_pEngineCtl->m_pPhysicsCtl->m_OldThrottle)) {
        return false;
    }
    if (this->eAccelTransFxState != FX_ACCEL_STATE_NONE) {
        return false;
    }
    if (this->m_pShiftCtl && this->m_pShiftCtl->IsActive()) {
        return false;
    }
    if (this->GetPhysRPM() < 3000.0f || SndBase::m_fRunningTime - this->t_LastAccelTrans < 2.0f) {
        return false;
    }
    if (!this->GetPhysCar()->IsAICar() && this->m_pEAXCar->GetCurGear() < Sound::SECOND_GEAR) {
        return false;
    }
    return true;
}

bool SFXCTL_AccelTrans::ShouldPlayEngOffSweet() {
    if (this->eAccelTransFxState != FX_ACCEL_STATE_NONE) {
        return false;
    }
    if (this->m_pShiftCtl && this->m_pShiftCtl->IsActive()) {
        return false;
    }
    if (this->GetPhysRPM() < 6000.0f) {
        return false;
    }
    return !(this->m_pEAXCar->GetCurGear() < Sound::SECOND_GEAR);
}

void SFXCTL_AccelTrans::Destroy() {}
