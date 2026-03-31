#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

SndBase::TypeInfo *SFXCTL::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL::GetTypeName() const { return s_TypeInfo.typeName; }

SFXCTL::SFXCTL() : m_UGL(AEMS_LEVEL0) {}

SFXCTL::~SFXCTL() {}

void SFXCTL::InitSFX() {}

void SFXCTL::UpdateParams(float t) {
    SndBase::UpdateParams(t);
}
