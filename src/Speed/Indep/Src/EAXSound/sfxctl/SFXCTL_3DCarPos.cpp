#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"

SndBase::TypeInfo *SFXCTL_3DCarPos::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_3DCarPos::GetTypeName() const { return s_TypeInfo.typeName; }
