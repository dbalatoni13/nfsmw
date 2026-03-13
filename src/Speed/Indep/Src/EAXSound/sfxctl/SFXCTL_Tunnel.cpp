#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"

SFXCTL_Tunnel::SFXCTL_Tunnel() {}

SFXCTL_Tunnel::~SFXCTL_Tunnel() {}

SndBase *SFXCTL_Tunnel::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_Tunnel();
}

SndBase::TypeInfo *SFXCTL_Tunnel::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Tunnel::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Tunnel::InitSFX() {
    SFXCTL::InitSFX();
}

void SFXCTL_Tunnel::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);
    UpdateIsInTunnel(t);
    UpdateDriveBySnds(t);
    UpdateCityVerb(t);
    UpdateReflectionParams(t);
}

void SFXCTL_Tunnel::UpdateMixerOutputs() {
    SetDMIX_Input(0, 0);
    SetDMIX_Input(1, 0);
    SetDMIX_Input(2, 0);
    SetDMIX_Input(3, 0);
    SetDMIX_Input(4, 0);
    SetDMIX_Input(5, 0);
    SetDMIX_Input(6, 0);
}

int SFXCTL_Tunnel::GetController(int Index) { return -1; }

void SFXCTL_Tunnel::AttachController(SFXCTL *psfxctl) {}

void SFXCTL_Tunnel::Destroy() {}

void SFXCTL_Tunnel::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

TrackPathZone *SFXCTL_Tunnel::GetTunnelType(bVector3 &pos, eTrackPathZoneType zonetype) {
    TrackPathZone *zone = nullptr;
    while (true) {
        zone = TheTrackPathManager.FindZone(reinterpret_cast< bVector2 * >(&pos), zonetype, zone);
        if (zone == nullptr) {
            return nullptr;
        }
        if (zone->Elevation == 0.0f) {
            break;
        }
        return zone;
    }
    return zone;
}

void SFXCTL_Tunnel::UpdateIsInTunnel(float t) {
    (void)t;
}

void SFXCTL_Tunnel::SetCurrentReverbType(eREVERBFX type, int reverboffset) {
    (void)type;
    (void)reverboffset;
}

void SFXCTL_Tunnel::UpdateCityVerb(float t) {
    (void)t;
}

void SFXCTL_Tunnel::AdjustReverbOffset(int reverboffset) {
    (void)reverboffset;
}

void SFXCTL_Tunnel::UpdateReflectionParams(float t) {
    ReflRamp.Update(t);
}

void SFXCTL_Tunnel::UpdateOcclusion(float t) {
    (void)t;
}

void SFXCTL_Tunnel::EndTunnelVerb() {}

void SFXCTL_Tunnel::UpdateDriveBySnds(float t) {
    float target = (m_pEAXCar != nullptr) ? 1.0f : 0.0f;
    ReflRamp.Update(t, target);
    SetDMIX_Input(DMX_VOL, static_cast<int>(ReflRamp.GetValue() * 32767.0f));
    SetDMIX_Input(DMX_PITCH, static_cast<int>(ReflRamp.GetValue() * 4096.0f));
}
