#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Turbo.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

int gnMemLeakTurboBLOWOFFCountTest = 0; // size: 0x4, address: 0x80417F88, Decl: 24
int gnMemLeakTurboSPOOLCountTest = 0;   // size: 0x4, address: 0x80417F8C, Decl: 25

static const float TURBO_DUCK_POWER_DROP = 6.0f; // size: 0x4, Decl: 29
static const int T_TURBO_DUCK_POWER_DROP = 2000; // size: 0x4, Decl: 30
static const int T_TURBO_SUSTAIN = 1000;         // size: 0x4, Decl: 31

static const int TURBO_PEAK_LIMIT_MIN_RPM = 1000;     // size: 0x4, Decl: 34
static const int TURBO_PEAK_LIMIT_MAX_RPM = 1100;     // size: 0x4, Decl: 35
static const float TURBO_PEAK_LIMIT_MIN_SCALE = 0.4f; // size: 0x4, Decl: 36

float MIN_TORQUE_FOR_BLOWOFF = 20.0f; // size: 0x4, address: 0x80417F90, Decl: 38

static const float gfxturbomaxtorque = 0.25f; // size: 0x4, Decl: 45

DEFINE_CREATABLE(0x20040, CARSFX_Turbo, SndBase);

CARSFX_Turbo::CARSFX_Turbo() : CARSFX(), m_BlowoffRampDown(), m_SpoolDuck(), m_fDeltaTurbo(3) {
    this->m_pTurboBlowoffControl = nullptr;
    this->m_pTurboSplControl = nullptr;
    this->bStopBlowoff = false;
    this->bReachedPeak = false;
    this->tLastBlowoffTime = 0.0f;
    this->m_pShiftingCtl = nullptr;
    this->m_pEngineCtl = nullptr;
    this->eTurboState = SFXTURBO_NONE;
    this->m_pTurboData = nullptr;
}

CARSFX_Turbo::~CARSFX_Turbo() {
#ifdef EA_BUILD_A124
    if (this->m_pTurboBlowoffControl != nullptr) {
        delete this->m_pTurboBlowoffControl;
        this->m_pTurboBlowoffControl = nullptr;
        gnMemLeakTurboBLOWOFFCountTest--;
    }

    if (this->m_pTurboSplControl != nullptr) {
        delete this->m_pTurboSplControl;
        this->m_pTurboSplControl = nullptr;
        gnMemLeakTurboSPOOLCountTest--;
    }
#else
    this->Destroy();
#endif
}

int CARSFX_Turbo::GetController(int Index) {
    switch (Index) {
        case 0:
            return 2;
        case 1:
            return 7;
        case 2:
            return 4;
        default:
            return -1;
    }
}

void CARSFX_Turbo::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
        case 2:
            this->m_pShiftingCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
            break;
        case 4:
            this->m_pEngineCtl = static_cast<SFXCTL_Engine *>(psfxctl);
            break;
        case 7:
            this->m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
            break;
    }
}

void CARSFX_Turbo::SetupSFX(CSTATE_Base *_StateBase) {
    this->SndBase::SetupSFX(_StateBase);
    this->eTurboState = SFXTURBO_NONE;
    this->m_pTurboData = &this->m_pEAXCar->GetTurboInfo();
}

void CARSFX_Turbo::InitSFX() {
    this->SndBase::InitSFX();
    this->m_MaxTurbo = this->GetPhysCar()->GetMaxTurboTorque() / gfxturbomaxtorque;
    this->m_fTurbo = 0.0f;

    int TurboManufacter;

    if (this->m_pTurboData->GetCollection() == Attrib::key_default) {
        this->Disable();
    } else {
        this->Enable();
        this->m_fDeltaTurbo.Flush(0.0f);
        this->eTurboState = SFXTURBO_NONE;

        bClamp(0, 0, 5);

#ifndef EA_BUILD_A124
        if (this->m_pTurboSplControl == nullptr) {
            this->PlaySpl(0, 0, 0, 0, 0);
        }
#endif

#ifdef EA_BUILD_A124
        this->m_pTurboBlowoffControl = nullptr;
        this->m_pTurboSplControl = nullptr;
#endif
        this->bStopBlowoff = false;
        this->bReachedPeak = false;
        this->tLastBlowoffTime = 0.0f;
        this->m_fTurbo = 0.0f;
    }
}

void CARSFX_Turbo::Detach() {
    this->Destroy();
    this->Disable();
}

void CARSFX_Turbo::Destroy() {
#ifdef EA_BUILD_A124
    if (this->m_pTurboSplControl != nullptr) {
        delete this->m_pTurboSplControl;
        this->m_pTurboSplControl = nullptr;
    }

    if (this->m_pTurboBlowoffControl != nullptr) {
        delete this->m_pTurboBlowoffControl;
        this->m_pTurboBlowoffControl = nullptr;
    }
#else
    if (this->m_pTurboBlowoffControl != nullptr) {
        delete this->m_pTurboBlowoffControl;
        this->m_pTurboBlowoffControl = nullptr;
        gnMemLeakTurboBLOWOFFCountTest--;
    }

    if (this->m_pTurboSplControl != nullptr) {
        delete this->m_pTurboSplControl;
        this->m_pTurboSplControl = nullptr;
        gnMemLeakTurboSPOOLCountTest--;
    }
#endif
}

void CARSFX_Turbo::UpdateParams(float t) {
    if (this->IsEnabled()) {
        if (this->m_pEngineCtl->GetEngTorque() > MIN_TORQUE_FOR_BLOWOFF) {
            this->m_fTurbo = this->m_pEngineCtl->GetEngTorque() * 0.01f;
        } else {
            this->m_fTurbo = 0.0f;
        }

        this->UpdateBlowOff(t);
        this->UpdateSpool(t);

        switch (this->eTurboState) {
            case SFXTURBO_NONE:
                if (this->m_pEngineCtl->GetEngTorque() > MIN_TORQUE_FOR_BLOWOFF) {
                    this->eTurboState = SFXTURBO_SPOOLING;
                }
                break;
            case SFXTURBO_SPOOLING:
                if (this->m_pEngineCtl->GetEngTorque() < MIN_TORQUE_FOR_BLOWOFF) {
                    this->eTurboState = SFXTURBO_BLOWOFF;
                    this->PlayBlowoff(0, 0, 0, 0, this->m_pEAXCar->GetRotation());
                    this->ResetSpool();
                    this->UpdateSpool(t);
                }
                break;
            case SFXTURBO_BLOWOFF:
                if (this->IsBlowOffDone()) {
                    this->eTurboState = SFXTURBO_NONE;
                }
                break;
        }
    }
}

// UNSOLVED
void CARSFX_Turbo::ProcessUpdate() {
    if (this->m_pTurboBlowoffControl != nullptr) {
        int nDMixOut;
        int Az;
        if (this->BlowoffID == 1) {
            nDMixOut = this->GetDMixOutput(2, DMX_VOL);
            Az = this->GetDMixOutput(0, DMX_AZIM);
        } else {
            nDMixOut = this->GetDMixOutput(3, DMX_VOL);
            Az = this->GetDMixOutput(0, DMX_AZIM);
        }

        int TmpBlowoffVol = static_cast<int>(static_cast<float>((this->BlowoffVol * nDMixOut) >> 15) * this->m_BlowoffRampDown.GetValue());
        this->m_pTurboBlowoffControl->SetAzimuth(Az);
        this->m_pTurboBlowoffControl->SetVolume(TmpBlowoffVol);
        this->m_pTurboBlowoffControl->CommitMemberData();
    }

    if (this->m_pTurboSplControl != nullptr) {
        int Az = this->GetDMixOutput(0, DMX_AZIM);
        int nDMixOut = this->GetDMixOutput(1, DMX_VOL);
        int Tmpvol_Spool = this->vol_Spool * nDMixOut >> 15;

        this->m_pTurboSplControl->SetVolume(Tmpvol_Spool);
        this->m_pTurboSplControl->SetPSI(static_cast<int>(this->SpoolPercent * 1024.0f));
        this->m_pTurboSplControl->SetAzimuth(Az);
        this->m_pTurboSplControl->SetRotation(static_cast<int>(this->m_pEAXCar->GetPhysTRQ() * 10.24f));
        this->m_pTurboSplControl->SetRPM(static_cast<int>(this->GetPhysRPM()));
        this->m_pTurboSplControl->CommitMemberData();
    }
}

int CARSFX_Turbo::PlayBlowoff(int _ID, int Vol, int PSI, int Azimuth, int rotation) {
    if (!this->IsEnabled() || IsSoundEnabled != 1 || this->m_pTurboBlowoffControl != nullptr) {
        return 0;
    }

    this->BlowoffID = 1;

    if (this->SpoolPercent > 0.75f) {
        this->BlowoffID = g_pEAXSound->Random(2) + 2;
    }

    if (this->BlowoffID == 1) {
        this->BlowoffVol = static_cast<int>(this->m_pTurboData->Vol_Blowoff1() * this->SpoolPercent);
    } else {
        this->BlowoffVol = static_cast<int>(this->m_pTurboData->Vol_Blowoff2() * this->SpoolPercent);
    }

    g_pEAXSound->SetCsisName("SND: Turbo");
    this->m_pTurboBlowoffControl = new Csis::FX_TURBO_01(this->BlowoffID, 0, static_cast<int>(this->SpoolPercent * 1024.0f), 0, rotation,
                                                         static_cast<int>(this->GetPhysRPM()));
#ifndef EA_BUILD_A124
    gnMemLeakTurboBLOWOFFCountTest++;
#endif

    this->m_refCount = static_cast<unsigned short>(this->m_pTurboBlowoffControl != nullptr ? this->m_pTurboBlowoffControl->GetRefCount() : 0);
    this->tLastBlowoffTime = this->m_pEAXCar->GetCurTime();
    this->m_BlowoffRampDown.Initialize(1.0f, 1.0f, 1, LINEAR);

    return 0;
}

bool CARSFX_Turbo::IsBlowOffDone() {
    if (this->m_pTurboBlowoffControl == nullptr) {
        return true;
    }

    return this->m_pTurboBlowoffControl->GetRefCount() == 1 || this->m_BlowoffRampDown.GetValue() == 0.0f;
}

void CARSFX_Turbo::UpdateBlowOff(float t) {
    if (this->m_pTurboBlowoffControl != nullptr) {
        this->m_BlowoffRampDown.Update(t);

        if (this->m_pEAXCar->GetThrottle() > MIN_TORQUE_FOR_BLOWOFF && this->m_BlowoffRampDown.GetValue() == 1.0f) {
#ifdef EA_BUILD_A124
            if (this->m_pShiftingCtl->eShiftState != SHFT_UP_LFO) {
                if (this->m_pShiftingCtl->eShiftState != SHFT_NONE) {
#else
            if (!this->m_pShiftingCtl->IsActive()) {
#endif
                this->m_BlowoffRampDown.Initialize(1.0f, 0.0f, 150, LINEAR);
#ifdef EA_BUILD_A124
                }
            }
#else
            }
#endif
        }

        if (this->IsBlowOffDone()) {
            this->StopBlowOff();
        }
    }
}

void CARSFX_Turbo::StopBlowOff() {
    if (this->m_pTurboBlowoffControl != nullptr) {
        delete this->m_pTurboBlowoffControl;
        this->m_pTurboBlowoffControl = nullptr;
#ifndef EA_BUILD_A124
        gnMemLeakTurboBLOWOFFCountTest--;
#endif
    }
}

// UNSOLVED, regswap
int CARSFX_Turbo::PlaySpl(int _ID, int Vol, int PSI, int Azimuth, int rotation) {
    if (IsSoundEnabled == 1) {
        Vol = (Vol * this->GetDMixOutput(1, DMX_VOL)) >> 15;

        g_pEAXSound->SetCsisName("SND:Turbo Spool");
#ifdef EA_BUILD_A124
        delete this->m_pTurboSplControl;
        this->m_pTurboSplControl = nullptr;
#endif
        this->m_pTurboSplControl =
            new Csis::FX_TURBO_01(_ID, Vol, PSI, this->GetDMixOutput(0, DMX_AZIM), rotation, static_cast<int>(this->GetPhysRPM()));
        gnMemLeakTurboSPOOLCountTest++;
    }

    return 0;
}

void CARSFX_Turbo::ResetSpool() {
    this->SpoolCharge = 0.0f;
}

// TODO move
extern int g_nArrayCosTable[513];

int CARSFX_Turbo::UpdateSpool(float t) {
#ifdef EA_BUILD_A124
    if (this->m_pTurboSplControl == nullptr) {
        this->PlaySpl(0, 0, 0, 0, 0);
    }
#endif

    if (!g_EAXIsPaused()) {
        if (this->m_fTurbo > 0.01f) {
            this->SpoolCharge += this->m_fTurbo;
        } else {
            this->SpoolCharge -= this->m_pTurboData->Leak_Rate();
        }
    }

    this->SpoolCharge = bClamp(this->SpoolCharge, 0.0f, this->m_pTurboData->ChargeTime());
    int RPM = static_cast<int>(this->GetPhysRPM()) - TURBO_PEAK_LIMIT_MIN_RPM;
    float SpoolChargeRPMScale = bClamp(static_cast<float>(RPM) * 0.01f, 0.0f, 1.0f) * 0.6f + 0.4f;
    float SpoolChargeScale = this->m_pTurboData->ChargeTime() * SpoolChargeRPMScale;
    this->SpoolCharge = bClamp(this->SpoolCharge, 0.0f, SpoolChargeScale);

    if (this->SpoolCharge == this->m_pTurboData->ChargeTime() * SpoolChargeRPMScale && !this->bReachedPeak) {
        this->bReachedPeak = true;
        this->m_SpoolDuck.ClearStages();
        this->m_SpoolDuck.AddStage(0.0f, 0.0f, T_TURBO_SUSTAIN, LINEAR);
        this->m_SpoolDuck.AddStage(0.0f, 307.2f, T_TURBO_DUCK_POWER_DROP, LINEAR);
    } else if (this->SpoolCharge != this->m_pTurboData->ChargeTime() && this->bReachedPeak) {
        this->bReachedPeak = false;
    }

    this->SpoolPercent = this->SpoolCharge / this->m_pTurboData->ChargeTime();
    this->vol_Spool = this->m_pTurboData->Vol_Spool();

    if (this->bReachedPeak) {
        this->m_SpoolDuck.Update(t);
        this->vol_Spool = this->vol_Spool * g_nArrayCosTable[this->m_SpoolDuck.iGetValue()] >> 15;
    }

    return 0;
}
