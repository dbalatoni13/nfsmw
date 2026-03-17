#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"

namespace {
struct EAXCarAccelTransView {
    char _pad[0x100];
    char mAccelTransDataSet;
};
} // namespace

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
    m_pAccelTransDataSet =
        static_cast<acceltrans *>(static_cast<void *>(&static_cast<EAXCarAccelTransView *>(static_cast<void *>(m_pEAXCar))
                                                           ->mAccelTransDataSet));
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
    UpdateRPM(t);
    UpdateTRQ(t);
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
    (void)t;
    OldIsAccelerating = IsAccelerating;
    IsAccelerating = (m_pEngineCtl != nullptr) ? (m_pEngineCtl->GetSmoothedEngTorque() > 0.0f) : false;
    if (ShouldBeginAccelTrans()) {
        BeginAccelTrans();
    } else if (ShouldBeginAccelTrans_Idle()) {
        BeginAccelTrans_Idle();
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
    return !IsAccelerating && OldIsAccelerating;
}

bool SFXCTL_AccelTrans::ShouldBeginAccelTrans() {
    return IsAccelerating && !OldIsAccelerating;
}

bool SFXCTL_AccelTrans::ShouldPlayEngOffSweet() {
    return PlayEngOffSweet;
}
