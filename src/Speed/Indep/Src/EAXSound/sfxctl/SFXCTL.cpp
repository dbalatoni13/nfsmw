#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

TYPEINFO_IMPLEMENT(0xFFFFFFFF, SFXCTL, SndBase, nullptr);

SFXCTL::SFXCTL() : m_UGL(AEMS_LEVEL0) {}

SFXCTL::~SFXCTL() {}

void SFXCTL::InitSFX() {}

void SFXCTL::UpdateParams(float t) {
    SndBase::UpdateParams(t);
}
