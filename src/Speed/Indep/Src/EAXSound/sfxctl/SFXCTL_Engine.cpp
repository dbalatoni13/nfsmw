#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"

SndBase::TypeInfo *SFXCTL_Engine::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Engine::GetTypeName() const { return s_TypeInfo.typeName; }
