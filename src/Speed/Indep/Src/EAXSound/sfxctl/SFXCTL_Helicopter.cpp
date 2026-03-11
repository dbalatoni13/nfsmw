#include "Speed/Indep/Src/EAXSound/sfxctl/sfxctl_helicopter.hpp"

SFXCTL_3DHeliPos::~SFXCTL_3DHeliPos() {}

SFXCTL_Helicopter::~SFXCTL_Helicopter() {}

SndBase::TypeInfo *SFXCTL_3DHeliPos::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_3DHeliPos::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase::TypeInfo *SFXCTL_Helicopter::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Helicopter::GetTypeName() const { return s_TypeInfo.typeName; }

int SFXCTL_Helicopter::GetController(int Index) {
    if (Index == 0) {
        return 1;
    }
    return -1;
}

void SFXCTL_Helicopter::Detach() { m_pHeliState = nullptr; }

void SFXCTL_Helicopter::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}
