#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"

SFXCTL_Wheel::~SFXCTL_Wheel() {}

SndBase::TypeInfo *SFXCTL_Wheel::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Wheel::GetTypeName() const { return s_TypeInfo.typeName; }
