#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"

inline const unsigned int &Attrib::Gen::acceltrans::AccelFromIdle_INTERUPT_T() const {
    const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(GetAttributePointer(0x49fb8ce5, 0));
    if (!resultptr) {
        resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
    }
    return *resultptr;
}

inline const unsigned int &Attrib::Gen::acceltrans::AccelFromIdle_RESUME_T() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->AccelFromIdle_RESUME_T;
}

SFXCTL_AccelTrans::SFXCTL_AccelTrans()
    : m_pEngineCtl(nullptr) //
    , m_pShiftCtl(nullptr) //
    , m_pAccelTransDataSet(nullptr) {}

SndBase *SFXCTL_AccelTrans::CreateObject(unsigned int allocator) {
    if (allocator != 0) {
        return new (static_cast< SFXCTL_AccelTrans * >(
            gAudioMemoryManager.AllocateMemory(sizeof(SFXCTL_AccelTrans), SFXCTL_AccelTrans::s_TypeInfo.typeName, true)))
            SFXCTL_AccelTrans();
    } else {
        return new (static_cast< SFXCTL_AccelTrans * >(
            gAudioMemoryManager.AllocateMemory(sizeof(SFXCTL_AccelTrans), SFXCTL_AccelTrans::s_TypeInfo.typeName, false)))
            SFXCTL_AccelTrans();
    }
}

SFXCTL_AccelTrans::~SFXCTL_AccelTrans() {}

SndBase::TypeInfo *SFXCTL_AccelTrans::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_AccelTrans::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_AccelTrans::Destroy() {}

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

void SFXCTL_AccelTrans::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_UGL = static_cast<eAemsUpgradeLevel>(m_pEAXCar->GetEngineUpgradeLevel());
    m_pAccelTransDataSet = &m_pEAXCar->GetAccelInfo();
}

void SFXCTL_AccelTrans::InitSFX() {
    SFXCTL::InitSFX();
    eAccelTransFxState = 0;
    t_LastAccelTrans = 0.0f;
    IsAccelerating = false;
    OldIsAccelerating = false;
    PlayEngOffSweet = false;
}

void SFXCTL_AccelTrans::AttachController(SFXCTL *ctrl) {
    int id = (ctrl->GetTypeInfo()->ObjectID >> 4) & 0xFFF;
    if (id != 2) {
        if (id == 4) {
            m_pEngineCtl = static_cast<SFXCTL_Engine *>(ctrl);
        }
    } else {
        m_pShiftCtl = static_cast<SFXCTL_Shifting *>(ctrl);
    }
}

void SFXCTL_AccelTrans::UpdateParams(float t) {
    bool bVar1;
    int iVar2;
    int iVar3;

    SFXCTL::UpdateParams(t);
    if (m_pStateBase->m_eStateType == eMM_AIRACECAR) {
        return;
    }

    iVar2 = *reinterpret_cast< int * >(&IsAccelerating);
    *reinterpret_cast< int * >(&PlayEngOffSweet) = 0;
    *reinterpret_cast< int * >(&OldIsAccelerating) = iVar2;

    iVar3 = *reinterpret_cast< int * >(&m_pEAXCar->bIsAccelerating);
    *reinterpret_cast< int * >(&IsAccelerating) = iVar3;

    if (iVar3 != 0) {
        if (iVar2 == 0) {
            bVar1 = ShouldBeginAccelTrans_Idle();
            if (bVar1) {
                BeginAccelTrans_Idle();
            } else {
                bVar1 = ShouldBeginAccelTrans();
                if (bVar1) {
                    BeginAccelTrans();
                }
            }
        }

        if (*reinterpret_cast< int * >(&IsAccelerating) != 0) {
            goto end;
        }
    }

    if (*reinterpret_cast< int * >(&OldIsAccelerating) != 0 &&
        (bVar1 = ShouldPlayEngOffSweet(), bVar1)) {
        *reinterpret_cast< int * >(static_cast< void * >(&m_pShiftCtl->m_bNeed_DeccelSnd)) = 1;
    }

end:
    UpdateState(t);
}

void SFXCTL_AccelTrans::UpdateRPM(float t) {
    switch (eAccelTransFxState) {
    case FX_ACCEL_STATE_IDLE_REVING:
        m_InterpEngRPM.Update(t);
        return;

    case FX_ACCEL_STATE_ATTACK:
    case FX_ACCEL_STATE_IDLE_ENGAGING:
        m_InterpEngRPM.Update(t, GetPhysRPM());
        return;

    case FX_ACCEL_STATE_INTERRUPT:
        m_InterpEngRPM.Update(t, GetPhysRPM());
        return;

    default:
        return;
    }
}

void SFXCTL_AccelTrans::UpdateTRQ(float t) {
    if (eAccelTransFxState > FX_ACCEL_STATE_NONE && eAccelTransFxState < 5) {
        m_InterpEngTorque.Update(t, GetPhysTRQ());
        if (m_InterpEngTorque.IsFinished()) {
            m_InterpEngTorque.Initialize(GetPhysTRQ(), GetPhysTRQ(), 0, LINEAR);
        }
    }
}

void SFXCTL_AccelTrans::UpdateState(float t) {
    m_InterpEngVol.Update(t);
    UpdateRPM(t);
    UpdateTRQ(t);
    if (eAccelTransFxState != FX_ACCEL_STATE_NONE) {
        if (!m_pEAXCar->IsAccelerating() && eAccelTransFxState != FX_ACCEL_STATE_INTERRUPT) {
            m_InterpEngRPM.Initialize(
                m_pEngineCtl->GetEngRPM(), GetPhysRPM(), m_pAccelTransDataSet->AccelFromIdle_INTERUPT_T(), EQ_PWR_SQ
            );
            eAccelTransFxState = FX_ACCEL_STATE_INTERRUPT;
            m_InterpEngVol.Initialize(m_InterpEngVol.GetValue(), 0.0f, m_pAccelTransDataSet->AccelFromIdle_INTERUPT_T(), LINEAR);
        }
        if (!m_pEAXCar->IsAccelerating() && eAccelTransFxState != FX_ACCEL_STATE_INTERRUPT) {
            eAccelTransFxState = FX_ACCEL_STATE_INTERRUPT;
            m_InterpEngRPM.Initialize(m_pEngineCtl->GetEngRPM(), GetPhysRPM(), 100, LINEAR);
            m_InterpEngVol.Initialize(m_InterpEngVol.GetValue(), 0.0f, 100, LINEAR);
        }
        switch (eAccelTransFxState) {
        case FX_ACCEL_STATE_NONE:
            break;

        case FX_ACCEL_STATE_IDLE_REVING:
            if (m_InterpEngRPM.IsFinished()) {
                eAccelTransFxState = FX_ACCEL_STATE_IDLE_ENGAGING;
                m_InterpEngRPM.Initialize(m_InterpEngRPM.GetValue(), GetPhysRPM(), m_pAccelTransDataSet->AccelFromIdle_RESUME_T(), LINEAR);
                m_InterpEngVol.Initialize(m_InterpEngVol.GetValue(), 0.0f, m_pAccelTransDataSet->AccelFromIdle_RESUME_T(), LINEAR);
            }
            break;

        case FX_ACCEL_STATE_IDLE_ENGAGING:
            if (m_InterpEngRPM.IsFinished()) {
                eAccelTransFxState = FX_ACCEL_STATE_NONE;
            }
            break;

        case FX_ACCEL_STATE_INTERRUPT:
            if (m_InterpEngRPM.IsFinished()) {
                eAccelTransFxState = FX_ACCEL_STATE_NONE;
            }
            break;

        case FX_ACCEL_STATE_ATTACK:
            if (m_InterpEngRPM.IsFinished()) {
                eAccelTransFxState = FX_ACCEL_STATE_NONE;
            }
            break;

        default:
            break;
        }
    }
}

void SFXCTL_AccelTrans::BeginAccelTrans() {
    m_InterpEngRPM.Initialize(GetPhysRPM() + 1000.0f, GetPhysRPM(), 500, EQ_PWR_SQ);
    eAccelTransFxState = FX_ACCEL_STATE_ATTACK;
    m_InterpEngVol.Initialize(0.8f, 0.0f, 200, LINEAR);
    *static_cast< int * >(static_cast< void * >(&m_pShiftCtl->m_bNeed_AccelSnd)) = 1;
    m_InterpEngTorque.Initialize(100.0f, 100.0f, 10, LINEAR);
    t_LastAccelTrans = SndBase::m_fRunningTime;
}

void SFXCTL_AccelTrans::BeginAccelTrans_Idle() {
    eAccelTransFxState = FX_ACCEL_STATE_IDLE_REVING;
    t_LastAccelTrans = SndBase::m_fRunningTime;
    m_InterpEngRPM.Initialize(GetPhysRPM(), GetPhysRPM() + m_pAccelTransDataSet->AccelFromIdle_PEAK_RPM(),
                              m_pAccelTransDataSet->AccelFromIdle_PEAK_T(), LINEAR);
    m_InterpEngVol.Initialize(0.0f, m_pAccelTransDataSet->AccelFromIdle_PEAK_VOL(),
                              m_pAccelTransDataSet->AccelFromIdle_PEAK_T(), LINEAR);
    m_InterpEngTorque.Initialize(GetPhysTRQ(), GetPhysTRQ(), 10, LINEAR);
}

bool SFXCTL_AccelTrans::ShouldBeginAccelTrans_Idle() {
    if (m_pEAXCar->GetVelocityMagnitudeMPH() <= 15.0f &&
        30.0f <= m_pEAXCar->GetThrottle() - m_pEngineCtl->m_pPhysicsCtl->m_OldThrottle &&
        !m_pEAXCar->GetPhysicsCTL()->NISRevingEnabled) {
        if (eAccelTransFxState != FX_ACCEL_STATE_NONE) {
            return false;
        }

        if ((!m_pShiftCtl || !m_pShiftCtl->IsActive()) &&
            m_pEAXCar->GetCurGear() == Sound::FIRST_GEAR && m_pEAXCar->GetPhysRPM() <= 1500.0f) {
            if (INIS::Get() != nullptr && INIS::Get()->IsPlaying()) {
                return false;
            }

            return true;
        }
    }

    return false;
}

bool SFXCTL_AccelTrans::ShouldBeginAccelTrans() {
    if (!(30.0f <= m_pEAXCar->GetThrottle() - m_pEngineCtl->m_pPhysicsCtl->m_OldThrottle)) {
        return false;
    }
    if (eAccelTransFxState != FX_ACCEL_STATE_NONE) {
        return false;
    }
    if (m_pShiftCtl && m_pShiftCtl->IsActive()) {
        return false;
    }
    if (GetPhysRPM() < 3000.0f || SndBase::m_fRunningTime - t_LastAccelTrans < 2.0f) {
        return false;
    }
    if (!GetPhysCar()->IsAICar() && m_pEAXCar->GetCurGear() < Sound::SECOND_GEAR) {
        return false;
    }
    return true;
}

bool SFXCTL_AccelTrans::ShouldPlayEngOffSweet() {
    if (eAccelTransFxState != FX_ACCEL_STATE_NONE) {
        return false;
    }
    if (m_pShiftCtl && m_pShiftCtl->IsActive()) {
        return false;
    }
    if (GetPhysRPM() < 6000.0f) {
        return false;
    }
    return !(m_pEAXCar->GetCurGear() < Sound::SECOND_GEAR);
}
