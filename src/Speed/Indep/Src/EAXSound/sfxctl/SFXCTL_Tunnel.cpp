#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"

SFXCTL_Tunnel::~SFXCTL_Tunnel() {}

SndBase::TypeInfo *SFXCTL_Tunnel::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Tunnel::GetTypeName() const { return s_TypeInfo.typeName; }

int SFXCTL_Tunnel::GetController(int Index) { return -1; }

void SFXCTL_Tunnel::AttachController(SFXCTL *psfxctl) {}

void SFXCTL_Tunnel::Destroy() {}

void SFXCTL_Tunnel::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void SFXCTL_Tunnel::UpdateDriveBySnds(float t) {
    float target = (m_pEAXCar != nullptr) ? 1.0f : 0.0f;
    ReflRamp.Update(t, target);
    SetDMIX_Input(DMX_VOL, static_cast<int>(ReflRamp.GetValue() * 32767.0f));
    SetDMIX_Input(DMX_PITCH, static_cast<int>(ReflRamp.GetValue() * 4096.0f));
}
