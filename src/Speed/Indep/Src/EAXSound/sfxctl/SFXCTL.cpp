#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

SFXCTL::SFXCTL() : m_UGL(AEMS_LEVEL0) {}

SndBase::TypeInfo *SFXCTL::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL::InitSFX() {}

void SFXCTL::UpdateParams(float t) {
    SndBase::UpdateParams(t);
}

SFXCTL::~SFXCTL() {}
