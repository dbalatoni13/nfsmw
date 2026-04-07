#include "./CARSFX_TrafficFX.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

namespace {
int HonkingCarCnt = 0;
}

namespace Csis {
InterfaceId ENV_STATICId = {"ENV_STATIC", 0x1981, 0x0002};
ClassHandle gENV_STATICHandle;
} // namespace Csis

float g_LastTrafficHonkTime = 0.0f;

CARSFX_TrafficHorn::TypeInfo CARSFX_TrafficHorn::s_TypeInfo = {
    0x00050020,
    "CARSFX_TrafficHorn",
    &SndBase::s_TypeInfo,
    CARSFX_TrafficHorn::CreateObject,
};

CARSFX_TrafficHorn::TypeInfo *CARSFX_TrafficHorn::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *CARSFX_TrafficHorn::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *CARSFX_TrafficHorn::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) CARSFX_TrafficHorn();
    }
    return new (s_TypeInfo.typeName, true) CARSFX_TrafficHorn();
}

CARSFX_TrafficHorn::CARSFX_TrafficHorn()
    : CARSFX() {
    *reinterpret_cast<int *>(&ShouldHonk) = 1;
    tSinceLastAttemptedToHonk = 0.0f;
    m_HornSound = nullptr;
    *reinterpret_cast<int *>(&AIPlayingHonk) = 0;
    *reinterpret_cast<int *>(&SND_PlayingHonk) = 0;
    *reinterpret_cast<int *>(&IsEndingHonk) = 0;
    HonkingCarCnt = (HonkingCarCnt + 1) % 2;
    HonkFadeOut.Initialize(1.0f, 1.0f, 1, LINEAR);
    tSinceLastHorn = 0.0f;
    g_LastTrafficHonkTime = 0.0f;
    pClosestCar = nullptr;
}

CARSFX_TrafficHorn::~CARSFX_TrafficHorn() {
    Destroy();
}

void CARSFX_TrafficHorn::Destroy() {
    CSIS_EndHonk();
}

void CARSFX_TrafficHorn::Detach() {
    CSIS_EndHonk();
}

void CARSFX_TrafficHorn::ProcessUpdate() {
    CSIS_UpdateHOnk();
}

int CARSFX_TrafficHorn::GetController(int Index) {
    int iVar1;

    iVar1 = -1;
    if (Index == 0) {
        iVar1 = 0;
    }
    return iVar1;
}

void CARSFX_TrafficHorn::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 0) {
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
    }
}

void CARSFX_TrafficHorn::StartHonkHorn() {}

void CARSFX_TrafficHorn::StopHonkHorn() {
    if (*reinterpret_cast<int *>(&AIPlayingHonk) != 0 && *reinterpret_cast<int *>(&IsEndingHonk) == 0) {
        EndCarHonk();
    }
}

void CARSFX_TrafficHorn::EndCarHonk() {
    *reinterpret_cast<int *>(&IsEndingHonk) = 1;
    HonkFadeOut.Initialize(1.0f, 0.0f, 0x96, LINEAR);
}

void CARSFX_TrafficHorn::UpdateMixerOutputs() {
    if (m_HornSound) {
        SetDMIX_Input(2, 0x7FFF);
    } else {
        SetDMIX_Input(2, 0);
    }
}

void CARSFX_TrafficHorn::CSIS_EndHonk() {
    if (m_HornSound) {
        delete m_HornSound;
    }
    *reinterpret_cast<int *>(&IsEndingHonk) = 0;
    m_HornSound = nullptr;
    tSinceLastHorn = m_pStateBase->GetCurTime();
    *reinterpret_cast<int *>(&AIPlayingHonk) = 0;
    *reinterpret_cast<int *>(&SND_PlayingHonk) = 0;
}

bool CARSFX_TrafficHorn::IsHonking() {
    return m_HornSound != nullptr;
}
