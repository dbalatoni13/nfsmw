#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"

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
