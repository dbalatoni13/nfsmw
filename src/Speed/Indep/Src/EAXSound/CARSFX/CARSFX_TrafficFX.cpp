#include "./CARSFX_TrafficFX.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/World/RaceParameters.hpp"

extern bool IsPlayerGoingFastEnough(float, int);

namespace {
int HonkingCarCnt = 0;
}

namespace Csis {
InterfaceId ENV_STATICId = {"ENV_STATIC", 0x1981, 0x0002};
ClassHandle gENV_STATICHandle;
InterfaceId FX_TRAFFICId = {"FX_TRAFFIC", 0x1981, 0x5BE3};
ClassHandle gFX_TRAFFICHandle;
} // namespace Csis

float g_LastTrafficHonkTime = 0.0f;

CARSFX_TrafficEngine::TypeInfo CARSFX_TrafficEngine::s_TypeInfo = {
    0x00000500,
    "CARSFX_TrafficEngine",
    &SndBase::s_TypeInfo,
    CARSFX_TrafficEngine::CreateObject,
};

CARSFX_TrafficEngine::TypeInfo *CARSFX_TrafficEngine::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *CARSFX_TrafficEngine::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *CARSFX_TrafficEngine::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) CARSFX_TrafficEngine();
    }
    return new (s_TypeInfo.typeName, true) CARSFX_TrafficEngine();
}

CARSFX_TrafficEngine::CARSFX_TrafficEngine()
    : CARSFX() {
    refCnt = 0;
    m_pcsisTranfficEng = nullptr;
    m_p3DCarPosCtl = nullptr;
}

CARSFX_TrafficEngine::~CARSFX_TrafficEngine() {
    Destroy();
}

int CARSFX_TrafficEngine::GetController(int Index) {
    if (Index == 0) {
        return 0;
    }
    return -1;
}

void CARSFX_TrafficEngine::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 0) {
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
    }
}

void CARSFX_TrafficEngine::InitSFX() {
    SndBase::InitSFX();
    m_pcsisTranfficEng = nullptr;
    EngTypeID = GetPhysCar()->GetAttributes()->TrafficEngType();
    InitEngine();
}

void CARSFX_TrafficEngine::InitEngine() {
    if (!m_pcsisTranfficEng) {
        g_pEAXSound->SetCsisName(this);
        m_pcsisTranfficEng = new FX_TRAFFIC(EngTypeID, 0, 0, 0, 0, 25000, 0, 0x7FFF, 0);
        refCnt = m_pcsisTranfficEng->GetRefCount();
        m_p3DCarPosCtl->AssignPositionVector(GetPhysCar()->GetPosition());
        m_p3DCarPosCtl->AssignDirectionVector(GetPhysCar()->GetForwardVector());
        m_p3DCarPosCtl->AssignVelocityVector(GetPhysCar()->GetVelocity());
    }
}

void CARSFX_TrafficEngine::Detach() {
    if (m_pcsisTranfficEng) {
        delete m_pcsisTranfficEng;
    }
    m_pcsisTranfficEng = nullptr;
    m_p3DCarPosCtl->AssignPositionVector(nullptr);
    m_p3DCarPosCtl->AssignDirectionVector(nullptr);
    m_p3DCarPosCtl->AssignVelocityVector(nullptr);
}

void CARSFX_TrafficEngine::Destroy() {
    SndBase::Destroy();
    if (m_pcsisTranfficEng) {
        delete m_pcsisTranfficEng;
    }
    refCnt = 0;
    m_pcsisTranfficEng = nullptr;
}

void CARSFX_TrafficEngine::ProcessUpdate() {
    int VelocityFactor;
    int PitchFactor;
    float fDMIXDoppler;

    SndBase::ProcessUpdate();
    if (m_pStateBase->IsAttached()) {
        if (!GetPhysCar() || GetPhysCar()->IsSimUpdating()) {
            if (m_pcsisTranfficEng) {
                fDMIXDoppler = bClamp(GetPhysCar()->GetVelocityMagnitudeMPH() * 7.3142858f, 0.0f, 1024.0f);
                VelocityFactor = static_cast<int>(fDMIXDoppler);
                PitchFactor = GetDMixOutput(4, DMX_PITCH) - 0x1000;
                m_pcsisTranfficEng->SetVolume(GetDMixOutput(1, DMX_VOL));
                m_pcsisTranfficEng->SetAzimuth(GetDMixOutput(0, DMX_AZIM));
                m_pcsisTranfficEng->SetRange(VelocityFactor);
                m_pcsisTranfficEng->SetPitch_OFFSET(PitchFactor);
                m_pcsisTranfficEng->CommitMemberData();
                refCnt = m_pcsisTranfficEng->GetRefCount();
            }
        } else if (m_pcsisTranfficEng) {
            m_pcsisTranfficEng->SetVolume(0);
            m_pcsisTranfficEng->SetAzimuth(0);
            m_pcsisTranfficEng->CommitMemberData();
        }
    }
}

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

void CARSFX_TrafficHorn::UpdateParams(float t) {
    if (!m_pStateBase->IsAttached()) {
        return;
    }

    HonkFadeOut.Update(t);

    if (!m_HornSound) {
        if (GetPhysCar()->IsSimUpdating() &&
            tSinceLastHorn + 3.0f < m_pStateBase->GetCurTime() &&
            tSinceLastAttemptedToHonk + 3.0f < m_pStateBase->GetCurTime() &&
            *reinterpret_cast<int *>(&ShouldHonk) != 0 && SndCamera::NumPlayers > 0) {
            int n;

            for (n = 0; n < SndCamera::NumPlayers; n++) {
                if (IsPlayerGoingFastEnough(10.0f, n) && IsPlayerCarInRange(n)) {
                    if (g_pEAXSound->Random(1.0f) < 0.3f ||
                        g_LastTrafficHonkTime + 5.0f < m_pStateBase->GetCurTime()) {
                        int ID;

                        ID = GetPhysCar()->GetAttributes()->HornType();
                        *reinterpret_cast<int *>(&SND_PlayingHonk) = 1;
                        HornDuration = g_pEAXSound->Random(2.8f) + 1.2f;
                        CSIS_BeginHonk(ID);
                    } else {
                        tSinceLastAttemptedToHonk = m_pStateBase->GetCurTime();
                    }
                }
            }
        }
    } else if ((HornDuration + tHornBegin <= m_pStateBase->GetCurTime() ||
                *reinterpret_cast<int *>(&SND_PlayingHonk) == 0) &&
               *reinterpret_cast<int *>(&AIPlayingHonk) == 0) {
        if (*reinterpret_cast<int *>(&IsEndingHonk) == 0) {
            EndCarHonk();
        }
    }

    if (*reinterpret_cast<int *>(&IsEndingHonk) != 0 && HonkFadeOut.IsFinished()) {
        CSIS_EndHonk();
    }
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

void CARSFX_TrafficHorn::CSIS_BeginHonk(int ID) {
    if (m_HornSound) {
        delete m_HornSound;
    }

    m_HornSound = nullptr;
    g_pEAXSound->SetCsisName("AUD: Traffic Horn");
    m_HornSound = new ENV_STATIC(ID, 0, 0, 0, ENVSTATICTYPETYPE_ENV_COMMON, 25000, 0, 0x7FFF, 0);

    g_LastTrafficHonkTime = m_pStateBase->GetCurTime();
    tHornBegin = g_LastTrafficHonkTime;
    HonkFadeOut.Initialize(1.0f, 1.0f, 1, LINEAR);
    *reinterpret_cast<int *>(&IsEndingHonk) = 0;
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

void CARSFX_TrafficHorn::CSIS_UpdateHOnk() {
    int TempVol;

    if (m_HornSound) {
        m_HornSound->SetPITCH(GetDMixOutput(4, DMX_PITCH));
        TempVol = static_cast<int>(static_cast<float>(GetDMixOutput(2, DMX_VOL)) * HonkFadeOut.CurValue);
        m_HornSound->SetVOLUME(TempVol);
        m_HornSound->SetAZIMUTH(GetDMixOutput(0, DMX_AZIM));
        m_HornSound->CommitMemberData();
    }
}

bool CARSFX_TrafficHorn::IsHonking() {
    return m_HornSound != nullptr;
}

bool CARSFX_TrafficHorn::IsPlayerCarInRange(int nplayer) {
    EAX_CarState *pPlayerCar;
    bVector3 m_pPlayerPosition;
    bVector3 m_pObjectPosition;
    bVector3 vPlayerDirection;
    float m_fObjectToPlayerDistance;
    bVector3 ObjFwdDirection;
    float DotProd;

    pPlayerCar = SndCamera::GetPlayerCar(nplayer)->GetPhysCar();
    m_pPlayerPosition = *pPlayerCar->GetPosition();
    m_pObjectPosition = *GetPhysCar()->GetPosition();
    bSub(&vPlayerDirection, &m_pPlayerPosition, &m_pObjectPosition);
    m_fObjectToPlayerDistance = bLength(&vPlayerDirection);

    if (m_fObjectToPlayerDistance > 20.0f) {
        return false;
    }

    if (m_fObjectToPlayerDistance < 3.0f) {
        return true;
    }

    vPlayerDirection.z = 0.0f;
    vPlayerDirection = bNormalize(vPlayerDirection);
    ObjFwdDirection = *GetPhysCar()->GetForwardVector();
    ObjFwdDirection.z = 0.0f;
    ObjFwdDirection = bNormalize(ObjFwdDirection);
    DotProd = bDot(vPlayerDirection, ObjFwdDirection);

    if (TheRaceParameters.IsDragRace()) {
        return bCos(115.0f) <= DotProd;
    }

    return bCos(40.0f) < DotProd || (DotProd < -bCos(40.0f) && m_fObjectToPlayerDistance < 10.0f);
}
