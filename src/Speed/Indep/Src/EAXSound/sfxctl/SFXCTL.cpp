#include "./SFXCTL.hpp"

DEFINE_TYPEINFO(SFXCTL, SndBase)

SFXCTL::SFXCTL() {
    m_UGL = AEMS_LEVEL0;
}

SFXCTL::~SFXCTL() {}

void SFXCTL::InitSFX() {}

void SFXCTL::UpdateParams(float) {}
