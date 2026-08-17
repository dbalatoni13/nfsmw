#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_TrafficFX.hpp"
#include "SFXObj_Woosh.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/STITCH_WSH.h"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"

static const float TRAFFIC_MIN_DISTANCE_FOR_HORN = 20.0f; // size: 0x4, Decl: 18

static const float TRAFFIC_MIN_PLAYERSPEED_FOR_FX = 10.0f; // size: 0x4, Decl: 20
static const float TRAFFIC_MAX_PLAYERSPEED_FOR_FX = 60.0f; // size: 0x4, Decl: 21

static const int TRAFFIC_MIN_PITCH_FOR_WOOSH = 3400; // size: 0x4, Decl: 23
static const int TRAFFIC_MAX_PITCH_FOR_WOOSH = 4500; // size: 0x4, Decl: 24

float g_LastTrafficHonkTime = 0.0f;                          // size: 0x4, address: 0x80418174, Decl: 26
static const float TRAFFIC_MAX_TIME_FOR_RANDOM_HORNS = 5.0f; // size: 0x4, Decl: 27

static const float MAX_TRAFFIC_HORN_DURATION = 4.0f; // size: 0x4, Decl: 30
static const float MIN_TRAFFIC_HORN_DURATION = 1.2f; // size: 0x4, Decl: 31

#define TRAFFIC_MIN_TIME_BETWEENHORNS 3.0f // Decl: 36

static const float PercentageChangeOfCarHonking = 0.3f; // size: 0x4, Decl: 38

static const int MEM_LEAK_DISABLE_TRAFFIC = 0; // size: 0x4, Decl: 41

bool IsPlayerGoingFastEnough(float Speed, int nplayer) {
    if (nplayer >= SndCamera::NumPlayers) {
        return false;
    }
    if (SndCamera::GetPlayerCar(nplayer) == nullptr) {
        return false;
    }
    return SndCamera::GetPlayerCar(nplayer)->GetPhysCar()->GetForwardSpeed() > Speed;
}

DEFINE_CREATABLE(0x50000, CARSFX_TrafficEngine, SndBase);

CARSFX_TrafficEngine::CARSFX_TrafficEngine() : CARSFX() {
    this->m_pcsisTranfficEng = nullptr;
    this->m_p3DCarPosCtl = nullptr;
    this->refCnt = 0;
}

CARSFX_TrafficEngine::~CARSFX_TrafficEngine() {
    this->Destroy();
}

int CARSFX_TrafficEngine::GetController(int Index) {
    if (Index == 0) {
        return 0;
    }
    return -1;
}

void CARSFX_TrafficEngine::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 0) {
        this->m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
    }
}

void CARSFX_TrafficEngine::InitSFX() {
    SndBase::InitSFX();
    this->m_pcsisTranfficEng = nullptr;
    this->EngTypeID = this->GetPhysCar()->GetAttributes()->TrafficEngType();
    this->InitEngine();
}

void CARSFX_TrafficEngine::InitEngine() {
    if (this->m_pcsisTranfficEng == nullptr) {
        g_pEAXSound->SetCsisName(this);
        this->m_pcsisTranfficEng = new Csis::FX_TRAFFIC(this->EngTypeID, 0, 0, 0, 0, 25000, 0, 0x7FFF, 0);
        this->refCnt = this->m_pcsisTranfficEng->GetRefCount();
        this->m_p3DCarPosCtl->AssignPositionVector(GetPhysCar()->GetPosition());
        this->m_p3DCarPosCtl->AssignDirectionVector(GetPhysCar()->GetForwardVector());
        this->m_p3DCarPosCtl->AssignVelocityVector(GetPhysCar()->GetVelocity());
    }
}

bVector3 v3NULL(0.0f, 0.0f, 0.0f); // size: 0x10, address: 0x8045E538, Decl: 130
void CARSFX_TrafficEngine::Detach() {
    delete this->m_pcsisTranfficEng;
    this->m_pcsisTranfficEng = nullptr;

    this->m_p3DCarPosCtl->AssignPositionVector(nullptr);
    this->m_p3DCarPosCtl->AssignDirectionVector(nullptr);
    this->m_p3DCarPosCtl->AssignVelocityVector(nullptr);
}

void CARSFX_TrafficEngine::Destroy() {
    SndBase::Destroy();
    delete this->m_pcsisTranfficEng;
    this->m_pcsisTranfficEng = nullptr;
    refCnt = 0;
}

static const float TrafficVelocityScale = 140.0f; // size: 0x4, Decl: 152

bool bPRINTDOPPLERINMIXER; // size: 0x1, address: 0xFFFFFFFF, Decl: 155
void CARSFX_TrafficEngine::ProcessUpdate() {
    SndBase::ProcessUpdate();
    if (!this->m_pStateBase->IsAttached()) {
        return;
    }

    if ((this->GetPhysCar() != nullptr) && !this->GetPhysCar()->IsSimUpdating()) {
        if (this->m_pcsisTranfficEng != nullptr) {
            this->m_pcsisTranfficEng->SetVolume(0);
            this->m_pcsisTranfficEng->SetAzimuth(0);
            this->m_pcsisTranfficEng->CommitMemberData();
        }
        return;
    }
    if (this->m_pcsisTranfficEng == nullptr) {
        return;
    };

    int VelocityFactor = static_cast<int>(bClamp(this->GetPhysCar()->GetVelocityMagnitudeMPH() * 7.3142858f, 0.0f, 1024.0f));
    this->GetInstanceID();
    int PitchFactor = this->GetDMixOutput(4, DMX_PITCH) - 0x1000;
    float fDMIXDoppler = this->GetDMixOutput(4, DMX_PITCH);
    this->m_pcsisTranfficEng->SetVolume(this->GetDMixOutput(1, DMX_VOL));
    this->m_pcsisTranfficEng->SetAzimuth(this->GetDMixOutput(0, DMX_AZIM));
    this->m_pcsisTranfficEng->SetRange(VelocityFactor);
    this->m_pcsisTranfficEng->SetPitch_OFFSET(PitchFactor);
    this->m_pcsisTranfficEng->CommitMemberData();
    this->refCnt = this->m_pcsisTranfficEng->GetRefCount();
}

DEFINE_CREATABLE(0x50000, SFXCTL_3DTrafficPos, SFXCTL_3DObjPos);

DEFINE_CREATABLE(0x50020, CARSFX_TrafficHorn, SndBase);

DEFINE_CREATABLE(0xc0140, CARSFX_TruckHorn, CARSFX_TrafficHorn);

CARSFX_TrafficHorn::CARSFX_TrafficHorn() : CARSFX() {
    static int HonkingCarCnt = 0;
    HonkingCarCnt = (HonkingCarCnt + 1) % 2;

    this->ShouldHonk = true;
    this->tSinceLastAttemptedToHonk = 0.0f;
    this->m_HornSound = nullptr;
    this->AIPlayingHonk = false;
    this->SND_PlayingHonk = false;
    this->IsEndingHonk = false;

    this->HonkFadeOut.Initialize(1.0f, 1.0f, 1, LINEAR);
    this->tSinceLastHorn = 0.0f;
    g_LastTrafficHonkTime = 0.0f;
    this->pClosestCar = nullptr;
}

CARSFX_TrafficHorn::~CARSFX_TrafficHorn() {}

void CARSFX_TrafficHorn::Destroy() {
    this->CSIS_EndHonk();
}

void CARSFX_TrafficHorn::Detach() {
    this->CSIS_EndHonk();
}

void CARSFX_TrafficHorn::UpdateParams(float t) {
    if (!m_pStateBase->IsAttached()) {
        return;
    }

    this->HonkFadeOut.Update(t);

    if (this->m_HornSound == nullptr) {
        if (this->GetPhysCar()->IsSimUpdating() && this->tSinceLastHorn + TRAFFIC_MIN_TIME_BETWEENHORNS < this->m_pStateBase->GetCurTime() &&
            this->tSinceLastAttemptedToHonk + TRAFFIC_MIN_TIME_BETWEENHORNS < this->m_pStateBase->GetCurTime() && this->ShouldHonk) {
            for (int n = 0; n < SndCamera::NumPlayers; n++) {
                if (IsPlayerGoingFastEnough(TRAFFIC_MIN_PLAYERSPEED_FOR_FX, n) && this->IsPlayerCarInRange(n)) {
                    if (g_pEAXSound->Random(1.0f) < PercentageChangeOfCarHonking ||
                        (g_LastTrafficHonkTime + TRAFFIC_MAX_TIME_FOR_RANDOM_HORNS < this->m_pStateBase->GetCurTime())) {
                        int ID = this->GetPhysCar()->GetAttributes()->HornType();
                        this->SND_PlayingHonk = true;
                        this->HornDuration = g_pEAXSound->Random(MAX_TRAFFIC_HORN_DURATION - MIN_TRAFFIC_HORN_DURATION) + MIN_TRAFFIC_HORN_DURATION;
                        this->CSIS_BeginHonk(ID);
                    } else {
                        this->tSinceLastAttemptedToHonk = this->m_pStateBase->GetCurTime();
                    }
                }
            }
        }
    } else if ((this->HornDuration + this->tHornBegin <= this->m_pStateBase->GetCurTime() || !this->SND_PlayingHonk) && !this->AIPlayingHonk) {
        if (!this->IsEndingHonk) {
            this->EndCarHonk();
        }
    }

    if (this->IsEndingHonk && this->HonkFadeOut.IsFinished()) {
        this->CSIS_EndHonk();
    }
}

void CARSFX_TrafficHorn::ProcessUpdate() {
    CSIS_UpdateHOnk();
}

int CARSFX_TrafficHorn::GetController(int Index) {
    return Index != 0 ? -1 : 0;
}

void CARSFX_TrafficHorn::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 0) {
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
    }
}

void CARSFX_TrafficHorn::StartHonkHorn() {
    int ID;

    if (false) {
        if (m_pStateBase->GetCurTime() < 0.0f) {
            ID = GetPhysCar()->GetAttributes()->HornType();
        }
    }
}

void CARSFX_TrafficHorn::StopHonkHorn() {
    if (this->AIPlayingHonk && !this->IsEndingHonk) {
        this->EndCarHonk();
    }
}

void CARSFX_TrafficHorn::EndCarHonk() {
    this->IsEndingHonk = true;
    this->HonkFadeOut.Initialize(1.0f, 0.0f, 150, LINEAR);
}

void CARSFX_TrafficHorn::UpdateMixerOutputs() {
    if (this->m_HornSound != nullptr) {
        this->SetDMIX_Input(2, 0x7FFF);
    } else {
        this->SetDMIX_Input(2, 0);
    }
}

// UNSOLVED, Csis::ENV_STATIC
void CARSFX_TrafficHorn::CSIS_BeginHonk(int ID) {
    delete this->m_HornSound;
    m_HornSound = nullptr;

    g_pEAXSound->SetCsisName("AUD: Traffic Horn");
    this->m_HornSound = new Csis::ENV_STATIC(ID, 0, 0, 0, Csis::ENVSTATICTYPETYPE_ENV_COMMON, 25000, 0, 0x7FFF, 0);

    g_LastTrafficHonkTime = this->m_pStateBase->GetCurTime();
    this->tHornBegin = this->m_pStateBase->GetCurTime();
    this->HonkFadeOut.Initialize(1.0f, 1.0f, 1, LINEAR);
    this->IsEndingHonk = false;
}

void CARSFX_TrafficHorn::CSIS_EndHonk() {
    delete this->m_HornSound;
    this->m_HornSound = nullptr;
    this->AIPlayingHonk = false;
    this->SND_PlayingHonk = false;
    this->tSinceLastHorn = this->m_pStateBase->GetCurTime();
    this->IsEndingHonk = false;
}

void CARSFX_TrafficHorn::CSIS_UpdateHOnk() {
    if (this->m_HornSound == nullptr) {
        return;
    }

    this->m_HornSound->SetPITCH(GetDMixOutput(4, DMX_PITCH));
    int TempVol = static_cast<int>(GetDMixOutput(2, DMX_VOL) * this->HonkFadeOut.GetValue());
    this->m_HornSound->SetVOLUME(TempVol);
    this->m_HornSound->SetAZIMUTH(GetDMixOutput(0, DMX_AZIM));
    this->m_HornSound->CommitMemberData();
}

bool CARSFX_TrafficHorn::IsHonking() {
    return this->m_HornSound != nullptr;
}

// UNSOLVED
bool CARSFX_TrafficHorn::IsPlayerCarInRange(int nplayer) {
    EAX_CarState *pPlayerCar = SndCamera::GetPlayerCar(nplayer)->GetPhysCar();

    bVector3 m_pPlayerPosition = *pPlayerCar->GetPosition();
    bVector3 m_pObjectPosition = *this->GetPhysCar()->GetPosition();

    bVector3 vPlayerDirection;
    bSub(&vPlayerDirection, &m_pPlayerPosition, &m_pObjectPosition);

    float m_fObjectToPlayerDistance = bLength(&vPlayerDirection);

    if (m_fObjectToPlayerDistance > TRAFFIC_MIN_DISTANCE_FOR_HORN) {
        return false;
    }

    if (m_fObjectToPlayerDistance < 3.0f) {
        return true;
    }

    vPlayerDirection.z = 0.0f;
    vPlayerDirection = bNormalize(vPlayerDirection);

    bVector3 ObjFwdDirection = *this->GetPhysCar()->GetForwardVector();

    ObjFwdDirection.z = 0.0f;
    ObjFwdDirection = bNormalize(ObjFwdDirection);

    float DotProd = bDot(vPlayerDirection, ObjFwdDirection);

    if (TheRaceParameters.IsDragRace()
            ? (DotProd >= bCos(bDegToRad(115.0f)))
            : DotProd > bCos(bDegToRad(40.0f)) || (DotProd < -bCos(bDegToRad(40.0f)) && m_fObjectToPlayerDistance < TRAFFIC_MIN_PLAYERSPEED_FOR_FX)) {
    } else {
        return false;
    }

    return true;
}

DEFINE_CREATABLE(0x50010, CARSFX_TrafficWoosh, SndBase);

CARSFX_TrafficWoosh::CARSFX_TrafficWoosh()
    : CARSFX(),                //
      m_DriveByWoosh(nullptr), //
      tSinceLastWoosh(-10.0f) {}

CARSFX_TrafficWoosh::~CARSFX_TrafficWoosh() {
    this->Destroy();
}

void CARSFX_TrafficWoosh::Destroy() {
    SndBase::Destroy();
    delete this->m_DriveByWoosh;
    this->m_DriveByWoosh = nullptr;
}

void CARSFX_TrafficWoosh::Detach() {
    delete this->m_DriveByWoosh;
    this->m_DriveByWoosh = nullptr;
}

void CARSFX_TrafficWoosh::UpdateParams(float t) {
    if (this->m_pStateBase->IsAttached()) {
        if ((this->m_DriveByWoosh != nullptr) || !this->GetPhysCar()->IsSimUpdating()) {
            return;
        }
        if (!this->IsPlayerCarInRadius()) {
            return;
        }

        float SpeedDiff = this->GetPlayerSpeedRelativeToUs();
        bool FastEnough = (SpeedDiff <= TRAFFIC_MIN_PLAYERSPEED_FOR_FX);

        if (FastEnough || (this->tSinceLastWoosh + 3.0f > this->m_pStateBase->GetCurTime())) {
            return;
        }

        float fSpeedScale =
            bClamp((SpeedDiff - TRAFFIC_MIN_PLAYERSPEED_FOR_FX) / (TRAFFIC_MAX_PLAYERSPEED_FOR_FX - TRAFFIC_MIN_PLAYERSPEED_FOR_FX), 0.0f, 1.0f);
        float fVelInensity = bClamp(fSpeedScale, 0.0f, 0.99f);
        fVelInensity = bClamp(fVelInensity * 127.0f, 0.0f, 127.0f);

        int StitchID;
        int numblocks;
        int sizeperblock;
        STICH_WHOOSH_TYPE base;
        eDRIVE_BY_TYPE wooshtype = this->GetWooshSample();
        GetWooshBlockSizeParams(wooshtype, base, numblocks, sizeperblock);

        GEN_RND_OFFSET(StitchID, fVelInensity, base, numblocks, sizeperblock);
        SND_Stich &StichData = g_pEAXSound->GetStichPlayer()->GetStich(STICH_TYPE_WOOSH, StitchID);

        this->m_DriveByWoosh = new cStichWrapper(StichData);

        SND_Params TempParams;
        TempParams.Mag = 0;
        TempParams.ID = StitchID;
        TempParams.RVerb = 0;
        TempParams.Az = 0;
        TempParams.Pitch = 0;
        TempParams.Vol = 0;

        this->m_DriveByWoosh->Play(&TempParams);
        this->tSinceLastWoosh = this->m_pStateBase->GetCurTime();
        this->SetDMIX_Input(3, 0x7FFF);
    }
}

eDRIVE_BY_TYPE CARSFX_TrafficWoosh::GetWooshSample() {
    return this->GetPhysCar()->GetAttributes()->WooshType();
}

bool CARSFX_TrafficWoosh::IsPlayerCarInRadius() {
    return GetPlayerCarInRadius(*this->GetPhysCar()->GetPosition(), TRAFFIC_MIN_PLAYERSPEED_FOR_FX) != nullptr;
}

void CARSFX_TrafficWoosh::ProcessUpdate() {
    this->SetDMIX_Input(3, 0);
    if (this->m_DriveByWoosh != nullptr) {
        SND_Params TmpParams;
        TmpParams.ID = 0;
        TmpParams.Az = 0;
        TmpParams.Mag = 0;
        TmpParams.RVerb = 0;

        TmpParams.Az = this->GetDMixOutput(0, DMX_AZIM);
        TmpParams.Vol = this->GetDMixOutput(3, DMX_VOL);
        TmpParams.Pitch = this->GetDMixOutput(4, DMX_VOL);

        this->m_DriveByWoosh->Update(&TmpParams);

        bool FastEnough = this->GetPlayerSpeedRelativeToUs() > TRAFFIC_MIN_PLAYERSPEED_FOR_FX;
        if (!FastEnough) {
            delete this->m_DriveByWoosh;
            this->m_DriveByWoosh = nullptr;
        } else if (!this->m_DriveByWoosh->IsPlaying()) {
            delete this->m_DriveByWoosh;
            this->m_DriveByWoosh = nullptr;
        } else if (g_EAXIsPaused()) {
            delete this->m_DriveByWoosh;
            this->m_DriveByWoosh = nullptr;
        }
    }
}

float CARSFX_TrafficWoosh::GetPlayerSpeedRelativeToUs() {
    EAX_CarState *pPlayerCar = GetClosestPlayerCar(this->GetPhysCar()->GetPosition());

    if (pPlayerCar == nullptr) {
        return 0.0f;
    }

    bVector2 PlayerVel = *pPlayerCar->GetVelocity2D();
    bVector2 ObjVel = *this->GetPhysCar()->GetVelocity2D();
    bVector2 VelDif = bSub(PlayerVel, ObjVel);
    return bLength(VelDif);
}
