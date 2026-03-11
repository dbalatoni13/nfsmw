#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"

SndBase::TypeInfo *SFXCTL_Wheel::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Wheel::GetTypeName() const { return s_TypeInfo.typeName; }
