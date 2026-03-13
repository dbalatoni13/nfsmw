#include "Speed/Indep/Src/EAXSound/sfxctl/sfxctl_mastervol.hpp"

SFXCTL_MasterVol::SFXCTL_MasterVol() {}

SFXCTL_MasterVol::~SFXCTL_MasterVol() {}

SndBase *SFXCTL_MasterVol::CreateObject(unsigned int allocator) {
    SFXCTL_MasterVol *object = reinterpret_cast< SFXCTL_MasterVol * >(
      gAudioMemoryManager.AllocateMemory(sizeof(SFXCTL_MasterVol), SFXCTL_MasterVol::s_TypeInfo.typeName, allocator != 0));
    return new (object) SFXCTL_MasterVol();
}

SndBase::TypeInfo *SFXCTL_MasterVol::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_MasterVol::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_MasterVol::InitSFX() { SFXCTL::InitSFX(); }

void SFXCTL_MasterVol::UpdateParams(float t) { SFXCTL::UpdateParams(t); }

SFXCTL_GameState::SFXCTL_GameState() {}

SndBase *SFXCTL_GameState::CreateObject(unsigned int allocator) {
    SFXCTL_GameState *object = reinterpret_cast< SFXCTL_GameState * >(
      gAudioMemoryManager.AllocateMemory(sizeof(SFXCTL_GameState), SFXCTL_GameState::s_TypeInfo.typeName, allocator != 0));
    return new (object) SFXCTL_GameState();
}

void SFXCTL_GameState::UpdateMixerOutputs() {
    SetDMIX_Input(0, 0);
    SetDMIX_Input(1, 0);
    SetDMIX_Input(2, 0);
    SetDMIX_Input(3, 0);
    SetDMIX_Input(4, 0);
    SetDMIX_Input(5, 0);
    SetDMIX_Input(6, 0);
}

SndBase::TypeInfo *SFXCTL_GameState::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_GameState::GetTypeName() const { return s_TypeInfo.typeName; }
