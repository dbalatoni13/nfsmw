#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Collision.hpp"

SndBase::TypeInfo *SFXCTL_3DColPos::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_3DColPos::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase::TypeInfo *SFXCTL_3DScrapePos::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_3DScrapePos::GetTypeName() const { return s_TypeInfo.typeName; }
