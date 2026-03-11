#include "Speed/Indep/Src/EAXSound/sfxctl/sfxctl_mastervol.hpp"

SndBase::TypeInfo *SFXCTL_MasterVol::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_MasterVol::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_MasterVol::InitSFX() {}

SndBase::TypeInfo *SFXCTL_GameState::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_GameState::GetTypeName() const { return s_TypeInfo.typeName; }
