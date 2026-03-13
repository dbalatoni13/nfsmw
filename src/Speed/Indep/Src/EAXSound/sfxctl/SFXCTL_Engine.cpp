#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyVehicleDestroyed.h"

SFXCTL_Engine::SFXCTL_Engine()
    : m_pShiftCtl(nullptr) //
    , m_pAccelTransitionCtl(nullptr) //
    , m_pPhysicsCtl(nullptr) //
    , m_p3DCarPosCtl(nullptr) //
    , tMergeWithPhysicsOffStart(0.0f) //
    , bPreRace(0) //
    , m_iEngineVol(0) //
    , bIsRedlining(false) //
    , m_fEng_RPM(0.0f) //
    , m_fPrevRPM(0.0f) //
    , m_fSmoothedEng_RPM(0.0f) //
    , m_fEng_Trq(0.0f) //
    , m_fSmoothedEng_Trq(0.0f) //
    , m_Rotation(0) //
    , m_bIsEngineBlown(false) //
    , m_DistanceFltr(0) //
    , bClutchStateOn(false) //
    , mmsgMVehicleDestroyed(nullptr) //
    , mmsgMVehicleDestroyed2(nullptr) {}

SndBase *SFXCTL_Engine::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_Engine();
}

SFXCTL_Engine::~SFXCTL_Engine() {
    if (mmsgMVehicleDestroyed) {
        Hermes::Handler::Destroy(mmsgMVehicleDestroyed);
    }
    if (mmsgMVehicleDestroyed2) {
        Hermes::Handler::Destroy(mmsgMVehicleDestroyed2);
    }
}

SndBase::TypeInfo *SFXCTL_Engine::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Engine::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Engine::InitSFX() {
    SFXCTL::InitSFX();
    m_iEngineVol = 0;
    bIsRedlining = false;
    m_fEng_RPM = 0.0f;
    m_fPrevRPM = 0.0f;
    m_fSmoothedEng_RPM = 0.0f;
    m_fEng_Trq = 0.0f;
    m_fSmoothedEng_Trq = 0.0f;
}

void SFXCTL_Engine::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);
    UpdateRPM(t);
    UpdateTorque(t);
    UpdateCompression(t);
    UpdateRedlining(t);
    UpdateVolume(t);
    UpdateFilterFX();
    UpdateEngineLFO_FX(t);
    UpdateClutchState();
}

void SFXCTL_Engine::MessageVehicleDestroyed(const MNotifyVehicleDestroyed &message) {
    (void)message;
    m_bIsEngineBlown = true;
}

float SFXCTL_Engine::GetEngRPM() { return m_fEng_RPM; }

float SFXCTL_Engine::GetSmoothedEngRPM() { return m_fSmoothedEng_RPM; }

float SFXCTL_Engine::GetEngTorque() { return m_fEng_Trq; }

float SFXCTL_Engine::GetSmoothedEngTorque() { return m_fSmoothedEng_Trq; }

void SFXCTL_Engine::MsgCountdownDone(const MCountdownDone &message) {
    tMergeWithPhysicsOffStart = 0.7f;
    bPreRace = 0;
}

void SFXCTL_Engine::UpdateClutchState() {
    bClutchStateOn = ShouldTurnOnClutch();
}

bool SFXCTL_Engine::ShouldTurnOnClutch() {
    return m_pPhysicsCtl != nullptr && m_pPhysicsCtl->m_CurGear <= 0;
}

void SFXCTL_Engine::UpdateFilterFX() {
    m_DistanceFltr = (m_p3DCarPosCtl != nullptr) ? 1 : 0;
}

void SFXCTL_Engine::UpdateCompression(float t) {
    (void)t;
}

void SFXCTL_Engine::UpdateRedlining(float t) {
    (void)t;
    bIsRedlining = (m_fEng_RPM > 0.95f);
}

void SFXCTL_Engine::UpdateVolume(float t) {
    (void)t;
    m_iEngineVol = static_cast< int >(m_fSmoothedEng_RPM * 32767.0f);
    if (m_iEngineVol < 0) {
        m_iEngineVol = 0;
    } else if (m_iEngineVol > 32767) {
        m_iEngineVol = 32767;
    }
    SetDMIX_Input(DMX_VOL, m_iEngineVol);
}

void SFXCTL_Engine::UpdateRPM(float t) {
    (void)t;
    if (m_pPhysicsCtl != nullptr) {
        m_fEng_RPM = m_pPhysicsCtl->PhysicsRPM;
    }
    m_fSmoothedEng_RPM = m_fEng_RPM;
}

void SFXCTL_Engine::UpdateTorque(float t) {
    (void)t;
    if (m_pPhysicsCtl != nullptr) {
        m_fEng_Trq = m_pPhysicsCtl->PhysicsTRQ;
    }
    m_fSmoothedEng_Trq = m_fEng_Trq;
}

void SFXCTL_Engine::UpdateEngineLFO_FX(float t) {
    (void)t;
}

void SFXCTL_Engine::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_UGL = static_cast<eAemsUpgradeLevel>(m_pEAXCar->GetEngineUpgradeLevel());
}

int SFXCTL_Engine::GetController(int Index) {
    switch (Index) {
    case 0:
        return 2;
    case 1:
        return 3;
    case 2:
        return 0;
    case 3:
        return 7;
    default:
        return -1;
    }
}

void SFXCTL_Engine::AttachController(SFXCTL *ctrl) {
    int id = (ctrl->GetTypeInfo()->ObjectID >> 4) & 0xFFF;
    switch (id) {
    case 2:
        m_pShiftCtl = static_cast<SFXCTL_Shifting *>(ctrl);
        break;
    case 3:
        m_pAccelTransitionCtl = static_cast<SFXCTL_AccelTrans *>(ctrl);
        break;
    case 0:
        m_pPhysicsCtl = static_cast<SFXCTL_Physics *>(ctrl);
        break;
    case 7:
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(ctrl);
        break;
    }
}
