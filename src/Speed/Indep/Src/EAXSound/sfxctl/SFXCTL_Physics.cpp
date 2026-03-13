#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/Misc/Hermes.h"

SFXCTL_Physics::SFXCTL_Physics()
    : m_bBlownEngineStreamQueued(false) //
    , PhysicsRPM(0.0f) //
    , RedLineRPM(0.0f) //
    , fMinPhysRPM(0.0f) //
    , fMaxPhysRPM(1.0f) //
    , fRedLinePhysRPM(0.0f) //
    , PhysicsTRQ(0.0f) //
    , fMaxPhysTRQ(0.0f) //
    , m_fThrottle(0.0f) //
    , m_OldThrottle(0.0f) //
    , m_OldDesiredSpeed(0.0f) //
    , m_tHoldDecel(0.0f) //
    , IsAccelerating(false) //
    , t_Last_Deccel(0.0f) //
    , t_Last_Accel(0.0f) //
    , m_CurGear(static_cast< Gear >(0)) //
    , m_LastGear(static_cast< Gear >(0)) //
    , mRPMCurve(nullptr) //
    , mMsgRevEngine(nullptr) //
    , mMsgRevOff(nullptr) //
    , PattternPlay(false) //
    , PatternNumber(0) //
    , CarID(-1) //
    , bPlayerEngEnable(false) //
    , RevFramesRemaining(0) //
    , NISRevingEnabled(false) //
    , eCurNisRevingState(NIS_OFF) //
    , TimeIntoRev(0.0f) //
    , NumDataPoints(0) //
    , pRevData(nullptr) //
    , NISRPM(0.0f) //
    , NISTRQ(0.0f) {}

SndBase *SFXCTL_Physics::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_Physics();
}

SFXCTL_Physics::~SFXCTL_Physics() {
    if (mMsgRevEngine) {
        Hermes::Handler::Destroy(mMsgRevEngine);
    }
    if (mMsgRevOff) {
        Hermes::Handler::Destroy(mMsgRevOff);
    }
}

SFXCTL_AIPhysics::~SFXCTL_AIPhysics() {}

SFXCTL_AIPhysics::SFXCTL_AIPhysics()
    : m_pShiftCtl(nullptr) //
    , Zero60Time(0.0f) //
    , m_fDeltaRPM(0.0f) {}

SndBase *SFXCTL_AIPhysics::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_AIPhysics();
}

SndBase::TypeInfo *SFXCTL_Physics::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Physics::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Physics::InitSFX() {
    SFXCTL::InitSFX();
    m_tHoldDecel = 0.0f;
    PhysicsTRQ = 0.0f;
    PhysicsRPM = 0.0f;
    NISRPM = 0.0f;
    NISTRQ = 0.0f;
    IsAccelerating = false;
    SetDMIX_Input(11, 0);
}

void SFXCTL_Physics::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);
    m_LastGear = m_CurGear;
    m_OldThrottle = m_fThrottle;
    m_fThrottle = smooth(m_fThrottle, 1.0f, 10.0f);
    PhysicsTRQ = smooth(PhysicsTRQ, m_fThrottle, 10.0f);
    PhysicsRPM = smooth(PhysicsRPM, m_fThrottle, 10.0f);
    UpdateNIS(t, SndBase::m_fDeltaTime);
}

void SFXCTL_Physics::UpdateMixerOutputs() {
    SetDMIX_Input(DMX_VOL, static_cast< int >(PhysicsRPM * 32767.0f));
    SetDMIX_Input(DMX_PITCH, static_cast< int >(PhysicsTRQ * 32767.0f));
    SetDMIX_Input(DMX_FREQ, static_cast< int >(PhysicsRPM * 32767.0f));
    SetDMIX_Input(DMX_AZIM, 0);
}

void SFXCTL_Physics::MsgRevEngine(const MAIEngineRev &message) {
    (void)message;
    eCurNisRevingState = NIS_OFF;
    if (m_pStateBase->m_eStateType == eMM_AIRACECAR) {
        PattternPlay = true;
        PatternNumber = 5;
    }
}

void SFXCTL_Physics::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_pEAXCar->m_pPhysicsCTL = this;
}

SndBase::TypeInfo *SFXCTL_AIPhysics::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_AIPhysics::GetTypeName() const { return s_TypeInfo.typeName; }

int SFXCTL_AIPhysics::GetController(int Index) {
    if (Index == 0) {
        return 2;
    }
    return -1;
}

void SFXCTL_AIPhysics::Destroy() {}

void SFXCTL_AIPhysics::UpdateMixerOutputs() {}

void SFXCTL_AIPhysics::SetupSFX(CSTATE_Base *_StateBase) {
    SFXCTL_Physics::SetupSFX(_StateBase);
}

void SFXCTL_AIPhysics::InitSFX() {
    SFXCTL_Physics::InitSFX();
    AIStateManager.Initialize(static_cast<SFXCTL_Physics *>(this));
    Zero60Time = 4.5f;
}

void SFXCTL_AIPhysics::AttachController(SFXCTL *psfxctl) {
    TypeInfo *pInfo = psfxctl->GetTypeInfo();
    if (((pInfo->ObjectID >> 4) & 0xFFF) == 2) {
        m_pShiftCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
    }
}

void SFXCTL_AIPhysics::UpdateTorque(float t) {
    float target;
    if (IsAccelerating) {
        target = 100.0f;
    } else {
        target = 0.0f;
    }
    PhysicsTRQ = smooth(PhysicsTRQ, target, 50.0f);
}

void SFXCTL_AIPhysics::UpdateParams(float t) {
    SFXCTL_Physics::UpdateParams(t);
    GenDeltaRPM();
    UpdateRPM(t);
    UpdateTorque(t);
    UpdateAccel(t);
    UpdateGear();
}

void SFXCTL_AIPhysics::GenDeltaRPM() {
    float prev = m_fDeltaRPM;
    m_fDeltaRPM = PhysicsRPM - prev;
}

void SFXCTL_AIPhysics::UpdateRPM(float t) {
    (void)t;
    PhysicsRPM = smooth(PhysicsRPM, m_fThrottle, 10.0f);
}

void SFXCTL_AIPhysics::UpdateAccel(float t) {
    (void)t;
    IsAccelerating = (m_fThrottle > 0.2f);
}

int SFXCTL_AIPhysics::SuggestGear() {
    return static_cast< int >(m_CurGear);
}

void SFXCTL_AIPhysics::UpdateGear() {
    m_LastGear = m_CurGear;
    m_CurGear = static_cast< Gear >(SuggestGear());
}

SndBase::TypeInfo *SFXCTL_TruckPhysics::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_TruckPhysics::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_TruckPhysics::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_TruckPhysics();
}

void SFXCTL_Physics::UpdateNIS(float t, float dt) {
    if (!NISRevingEnabled || !bPlayerEngEnable) {
        eCurNisRevingState = NIS_DISABLED;
        TimeIntoRev = 0.0f;
        NISRPM = 0.0f;
        NISTRQ = 0.0f;
        return;
    }

    switch (eCurNisRevingState) {
    case NIS_OFF:
        TimeIntoRev = 0.0f;
        NISRPM = PhysicsRPM;
        NISTRQ = PhysicsTRQ;
        if (PattternPlay) {
            eCurNisRevingState = NIS_PATTERN_ON;
        }
        break;
    case NIS_PATTERN_ON:
        TimeIntoRev += t;
        NISRPM += dt * 1000.0f;
        NISTRQ += dt * 100.0f;
        if (TimeIntoRev > 1.0f) {
            eCurNisRevingState = NIS_MERGE_WITH_PHYSICS;
        }
        break;
    case NIS_MERGE_WITH_PHYSICS:
        TimeIntoRev += t;
        NISRPM += (PhysicsRPM - NISRPM) * (t * 2.0f);
        NISTRQ += (PhysicsTRQ - NISTRQ) * (t * 2.0f);
        if (TimeIntoRev > 2.0f) {
            eCurNisRevingState = NIS_OFF;
            TimeIntoRev = 0.0f;
        }
        break;
    default:
        eCurNisRevingState = NIS_OFF;
        break;
    }
}
