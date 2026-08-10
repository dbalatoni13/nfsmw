#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_WindNoise.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

static const float gfCarWindRadius = 65.0f;            // size: 0x4, Decl: 99
static const float gfWindNoiseStartVelocity = 2.0f;    // size: 0x4, Decl: 100
static const float gfWindNoiseMaxVelocity = 40.0f;     // size: 0x4, Decl: 101
static const int gnWindNoiseMinSepAngle = 1280;        // size: 0x4, Decl: 102
static const int gnWindNoiseMaxSepAngle = 13568;       // size: 0x4, Decl: 103
static const float gfMaxWindNoiseVelocityMag = 70.0f;  // size: 0x4, Decl: 104
static const int gnWindSphereMixCtl = 0;               // size: 0x4, Decl: 105
static const int gnWindSphereExitMixCtl = 0;           // size: 0x4, Decl: 106
static const float gfMinWindSphereTravelTime = 240.0f; // size: 0x4, Decl: 107
static const float gfWindSphereTimeScale = 1.0f;       // size: 0x4, Decl: 108
static const float gfWindMinDist = 13.98f;             // size: 0x4, Decl: 109
static const float gfWindMaxDist = 100.0f;             // size: 0x4, Decl: 110
static const float gfMinWindRadiusScale = 1.01f;       // size: 0x4, Decl: 111
static const float gfWindIntersectionVelocity = 18.8f; // size: 0x4, Decl: 112

DEFINE_CREATABLE(0x200D0, SFXCTL_3DLeftWindPos, SFXCTL_3DObjPos);

DEFINE_CREATABLE(0x200E0, SFXCTL_3DRightWindPos, SFXCTL_3DObjPos);

DEFINE_CREATABLE(0x20090, CARSFX_WindNoise, SndBase);

CARSFX_WindNoise::CARSFX_WindNoise() : CARSFX() {
    this->IsInitialized = false;
    this->m_pCsisWind = nullptr;
    this->m_pEAXCar = nullptr;
    this->m_v3CarBaseLeftPos = bVector3(0.0f, 0.0f, 0.0f);
    this->m_v3CarBaseRightPos = bVector3(0.0f, 0.0f, 0.0f);
}

CARSFX_WindNoise::~CARSFX_WindNoise() {
    this->Destroy();
}

void CARSFX_WindNoise::Destroy() {
    if (this->m_pCsisWind != nullptr) {
        delete this->m_pCsisWind;
        this->m_pCsisWind = nullptr;
    }
}

int CARSFX_WindNoise::GetController(int Index) {
    if (Index == 0) {
        return 0xE;
    }
    if (Index == 1) {
        return 0xD;
    }
    return -1;
}

void CARSFX_WindNoise::AttachController(SFXCTL *psfxctl) {
    unsigned int objIndex = psfxctl->GetObjectIndex();
    switch (objIndex) {
        case 0xE:
            this->m_p3DRightWindPos = static_cast<SFXCTL_3DRightWindPos *>(psfxctl);
            break;
        case 0xD:
            this->m_p3DLeftWindPos = static_cast<SFXCTL_3DLeftWindPos *>(psfxctl);
            break;
        default:
            break;
    }
}

int CARSFX_WindNoise::UpdateMasterVolume() {
    return 0;
}

void CARSFX_WindNoise::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);

    int i;
    for (i = 0; i < 2; i++) {
        this->m_stWindParams[i].nCurrentChannel = i & 1;
    }
}

void CARSFX_WindNoise::InitSFX() {
    if (IsSoundEnabled != 0) {
        SndBase::InitSFX();
        this->IsInitialized = true;
        this->m_nLastTickUpdate = bGetTicker();
        this->m_bIsInSpacialTranslation = false;
        g_pEAXSound->SetCsisName(this);
        this->m_pCsisWind = new Csis::FX_WIND(0x1000, 0, 0, 0, 0, 0, 0, 0, 0, 25000, 0, 0x7FFF, 0, 0xF);
        this->m_p3DLeftWindPos->AssignPositionVector(&this->m_v3CarBaseLeftPos);
        this->m_p3DLeftWindPos->AssignDirectionVector(nullptr);
        this->m_p3DRightWindPos->AssignPositionVector(&this->m_v3CarBaseRightPos);
        this->m_p3DRightWindPos->AssignDirectionVector(nullptr);
        this->Enable();
    }
}

void CARSFX_WindNoise::ProcessUpdate() {
    if (this->IsEnabled() && (this->m_pCsisWind != nullptr)) {
        this->UpdateCSISParams();
    }
}

float gfTestRatio;  // size: 0x4, address: 0xFFFFFFFF, Decl: 258
float gfTestCurVel; // size: 0x4, address: 0xFFFFFFFF, Decl: 259

void CARSFX_WindNoise::UpdateParams(float t) {
    if (!this->IsEnabled()) {
        return;
    }

    this->m_fmsTickDiff = bGetTickerDifference(this->m_nLastTickUpdate, bGetTicker());
    this->m_nLastTickUpdate = bGetTicker();

    Camera *pcam = eGetView(1, false)->GetCamera();
    if (pcam == nullptr) {
        return;
    }

    EAX_CarState *pcar = this->GetPhysCar();
    float fcurvel = pcar->GetVelocityMagnitude();

    if (fcurvel < gfWindNoiseStartVelocity) {
        fcurvel = gfWindNoiseStartVelocity;
    } else if (fcurvel > gfWindNoiseMaxVelocity) {
        fcurvel = gfWindNoiseMaxVelocity - 0.05f;
    }

    const bVector3 *pv3CarDir = pcar->GetForwardVector();
    const bVector3 *pv3CarPos = pcar->GetPosition();

    float fratio = fcurvel / gfWindNoiseMaxVelocity;
    this->m_stWindParams[0].nCrossFadeWeight = static_cast<int>(fratio * 1040.0f);

    int nAngleSep = static_cast<int>(fratio * 12288.0f) + gnWindNoiseMinSepAngle;
    this->m_nVelocityWeightedVolume = static_cast<int>(fratio * 32767.0f);
    this->m_fcurwindradius = (1.0f - fratio) * gfCarWindRadius;

    if (this->m_fcurwindradius < this->m_pEAXCar->m_fSphereRadius) {
        this->m_fcurwindradius = this->m_pEAXCar->m_fSphereRadius * gfMinWindRadiusScale;
    }

    bVector3 v3NewPosLeft;
    bVector3 v3NewPosRight;

    v3NewPosLeft.x =
        this->m_fcurwindradius * (bCos(static_cast<bAngle>(nAngleSep)) * pv3CarDir->x - bSin(static_cast<bAngle>(nAngleSep)) * pv3CarDir->y);
    v3NewPosLeft.y =
        this->m_fcurwindradius * (bSin(static_cast<bAngle>(nAngleSep)) * pv3CarDir->x + bCos(static_cast<bAngle>(nAngleSep)) * pv3CarDir->y);
    v3NewPosLeft.z = pv3CarDir->z;

    v3NewPosRight.x =
        this->m_fcurwindradius * (bCos(static_cast<bAngle>(~nAngleSep)) * pv3CarDir->x - bSin(static_cast<bAngle>(~nAngleSep)) * pv3CarDir->y);
    v3NewPosRight.y =
        this->m_fcurwindradius * (bSin(static_cast<bAngle>(~nAngleSep)) * pv3CarDir->x + bCos(static_cast<bAngle>(~nAngleSep)) * pv3CarDir->y);
    v3NewPosRight.z = pv3CarDir->z;

    int i = 0;
    while (i < 2) {
        if (this->m_stWindParams[i].nCurrentChannel == 0) {
            this->m_v3CarBaseLeftPos.x = v3NewPosLeft.x + pv3CarPos->x;
            this->m_v3CarBaseLeftPos.y = v3NewPosLeft.y + pv3CarPos->y;
            this->m_v3CarBaseLeftPos.z = pv3CarPos->z;
        } else {
            this->m_v3CarBaseRightPos.x = pv3CarPos->x + v3NewPosRight.x;
            this->m_v3CarBaseRightPos.y = pv3CarPos->y + v3NewPosRight.y;
            this->m_v3CarBaseRightPos.z = pv3CarPos->z;
        }

        i++;
    }
}

int debugPrintWindNoise; // size: 0x4, address: 0xFFFFFFFF, Decl: 565

void CARSFX_WindNoise::UpdateCSISParams() {
    int nLeftVolume = this->GetDMixOutput(2, DMX_VOL) * this->m_nVelocityWeightedVolume >> 15;
    int nRightVolume = this->GetDMixOutput(3, DMX_VOL) * this->m_nVelocityWeightedVolume >> 15;
    int nRumbleVolume = this->GetDMixOutput(4, DMX_VOL) * this->m_nVelocityWeightedVolume >> 15;
    int refcount = this->m_pCsisWind->GetRefCount();

    this->m_pCsisWind->SetVolume_left(nLeftVolume);
    this->m_pCsisWind->SetVolume_right(nRightVolume);
    this->m_pCsisWind->SetRumble_Volume(nRumbleVolume);
    this->m_pCsisWind->SetPitch(this->GetDMixOutput(5, DMX_PITCH));
    this->m_pCsisWind->SetAzimuth_left(this->GetDMixOutput(0, DMX_AZIM));
    this->m_pCsisWind->SetAzimuth_right(this->GetDMixOutput(1, DMX_AZIM));
    this->m_pCsisWind->SetIntensity(this->m_stWindParams[0].nCrossFadeWeight);
    this->m_pCsisWind->SetFX_Dry(0x7FFF);
    this->m_pCsisWind->CommitMemberData();
}

DEFINE_CREATABLE(0x200C0, CARSFX_WindWeather, SndBase);

CARSFX_WindWeather::CARSFX_WindWeather() : CARSFX() {
    this->refCnt = 0;
    this->m_pcsisWind = nullptr;
}

CARSFX_WindWeather::~CARSFX_WindWeather() {
    this->Destroy();
}

void CARSFX_WindWeather::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void CARSFX_WindWeather::InitSFX() {
    SndBase::InitSFX();
}

void CARSFX_WindWeather::Destroy() {
    delete this->m_pcsisWind;
    this->m_pcsisWind = nullptr;
}

static const int WindWeatherVol = 7000; // size: 0x4, Decl: 855

static const float MaxSpeedAtWhichWindPlays = 70.0f; // size: 0x4, Decl: 860
Slope WeatherWindVolSlope(1.0f, 0.25f, 0.0f, 70.0f); // size: 0x1C, address: 0x8045E3B4, Decl: 861

void CARSFX_WindWeather::UpdateParams(float t) {
    SndBase::UpdateParams(t);
    if (this->m_pcsisWind == nullptr) {
        this->Play();
    }

    this->WeatherSpeedScale = WeatherWindVolSlope.GetValue(this->m_pEAXCar->GetPhysCar()->GetVelocityMagnitudeMPH());
}

void CARSFX_WindWeather::Play() {
    g_pEAXSound->SetCsisName("SND: Csis WindWeather");
    this->m_pcsisWind = new Csis::FX_WIND_Weather(0, 0, 0, 0);
    this->refCnt = this->m_pcsisWind->GetRefCount();
}

void CARSFX_WindWeather::ProcessUpdate() {
    if (this->m_pcsisWind == nullptr) {
        return;
    }
    int TempVol = static_cast<int>(static_cast<float>(this->GetDMixOutput(0, DMX_VOL)) * this->WeatherSpeedScale);

    this->m_pcsisWind->SetVolume(TempVol);
    this->m_pcsisWind->SetWidth(4000);
    this->m_pcsisWind->SetIntensity(0);
    this->m_pcsisWind->CommitMemberData();
}
