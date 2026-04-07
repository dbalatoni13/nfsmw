#include "./CARSFX_TrafficFX.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"

extern bool IsPlayerGoingFastEnough(float, int);

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
