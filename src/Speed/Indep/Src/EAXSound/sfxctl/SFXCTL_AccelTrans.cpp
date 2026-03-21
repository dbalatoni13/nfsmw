#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"

inline float SFXCTL::GetPhysRPM() { return m_pEAXCar->GetPhysRPM(); }

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
    , eAccelTransFxState(0) //
    , t_LastAccelTrans(0.0f) //
    , IsAccelerating(false) //
    , OldIsAccelerating(false) //
    , m_pAccelTransDataSet(nullptr) //
    , PlayEngOffSweet(false) {}

SndBase *SFXCTL_AccelTrans::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_AccelTrans();
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
    SFXCTL::UpdateParams(t);
    UpdateState(t);
}

void SFXCTL_AccelTrans::UpdateRPM(float t) {
    (void)t;
    float rpm = (m_pEngineCtl != nullptr) ? m_pEngineCtl->GetSmoothedEngRPM() : 0.0f;
    m_InterpEngRPM.Update(SndBase::m_fDeltaTime, rpm);
}

void SFXCTL_AccelTrans::UpdateTRQ(float t) {
    (void)t;
    float trq = (m_pEngineCtl != nullptr) ? m_pEngineCtl->GetSmoothedEngTorque() : 0.0f;
    m_InterpEngTorque.Update(SndBase::m_fDeltaTime, trq);
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
    eAccelTransFxState = 1;
    t_LastAccelTrans = 0.0f;
}

void SFXCTL_AccelTrans::BeginAccelTrans_Idle() {
    eAccelTransFxState = 2;
    t_LastAccelTrans = 0.0f;
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
    return IsAccelerating && !OldIsAccelerating;
}

bool SFXCTL_AccelTrans::ShouldPlayEngOffSweet() {
    return PlayEngOffSweet;
}
