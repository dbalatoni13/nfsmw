#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"

SFXCTL_Engine::~SFXCTL_Engine() {}

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
