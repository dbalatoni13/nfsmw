#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

Slope ShiftingAttackVolSlope(26000.0f, 32767.0f, 4000.0f, 7000.0f); // size: 0x1C, address: 0x8045E314, Decl: 25

static const int SHIFT_ATTACK_ENGAGE_ID = 0;    // size: 0x4, Decl: 31
static const int SHIFT_ATTACK_DISENGAGE_ID = 0; // size: 0x4, Decl: 32

static const float VOL_SHIFT_MAX_RPM_FOR_SCALE = 8000.0f; // size: 0x4, Decl: 35
static const float VOL_SHIFT_MIN_RPM_FOR_SCALE = 1500.0f; // size: 0x4, Decl: 36
static const float VOL_SHIFT_MIN_SCALE = 0.1f;            // size: 0x4, Decl: 37

float SHIFTING_ATTACK_MIXER_VOL = 30.0f; // size: 0x4, address: 0xFFFFFFFF, Decl: 39

DEFINE_CREATABLE(0x20030, CARSFX_Shift, SndBase);

CARSFX_Shift::CARSFX_Shift() : CARSFX() {
    this->m_ShiftGear = nullptr;
    this->m_BrakePedal = nullptr;
    this->m_Disengage = nullptr;
    this->m_Engage = nullptr;
    this->m_AccelSnd = nullptr;
    this->m_DeccelSnd = nullptr;
    this->m_Whine = nullptr;
    this->m_pShiftCtl = nullptr;
    this->m_pShiftingPatternData = nullptr;
    this->m_pSweetnersData = nullptr;
}

CARSFX_Shift::~CARSFX_Shift() {
    this->Destroy();
}

void CARSFX_Shift::UpdateMixerOutputs() {
    if (this->m_pShiftCtl->eShiftState == SHFT_UP_DISENGAGE) {
        this->SetDMIX_Input(7, 0x7FFF);
    } else {
        this->SetDMIX_Input(7, 0);
    }
}

void CARSFX_Shift::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_UGL = this->m_pEAXCar->m_TransmissionUGL;
    this->m_pShiftingPatternData = &this->m_pEAXCar->GetShiftInfo();
    this->m_pSweetnersData = &this->m_pEAXCar->GetAttributes();
}

void CARSFX_Shift::InitSFX() {
    SndBase::InitSFX();
}

void CARSFX_Shift::Destroy() {
    if (this->m_DeccelSnd != nullptr) {
        delete this->m_DeccelSnd;
        this->m_DeccelSnd = nullptr;
    }

    if (this->m_AccelSnd != nullptr) {
        delete this->m_AccelSnd;
        this->m_AccelSnd = nullptr;
    }

    if (this->m_ShiftGear != nullptr) {
        delete this->m_ShiftGear;
        this->m_ShiftGear = nullptr;
    }

    if (this->m_BrakePedal != nullptr) {
        delete this->m_BrakePedal;
        this->m_BrakePedal = nullptr;
    }

    if (this->m_Disengage != nullptr) {
        delete this->m_Disengage;
        this->m_Disengage = nullptr;
    }

    if (this->m_Engage != nullptr) {
        delete this->m_Engage;
        this->m_Engage = nullptr;
    }

    if (this->m_Whine != nullptr) {
        delete this->m_Whine;
        this->m_Whine = nullptr;
    }
}

int CARSFX_Shift::GetController(int Index) {
    switch (Index) {
        case 0:
            return 2;
        case 1:
            return 3;
        case 2:
            return 7;
        default:
            return -1;
    }
}

void CARSFX_Shift::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
        case 2:
            this->m_pShiftCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
            break;
        case 3:
            this->m_pAccelTransCtl = static_cast<SFXCTL_AccelTrans *>(psfxctl);
            break;
        case 7:
            this->m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
            break;
    }
}

void CARSFX_Shift::UpdateParams(float t) {
    SndBase::UpdateParams(t);

    if (this->m_DeccelSnd != nullptr && this->m_DeccelSnd->GetRefCount() < 2) {
        delete this->m_DeccelSnd;
        this->m_DeccelSnd = nullptr;
    }

    if (this->m_AccelSnd != nullptr && this->m_AccelSnd->GetRefCount() < 2) {
        delete this->m_AccelSnd;
        this->m_AccelSnd = nullptr;
    }

    if (this->m_Disengage != nullptr && this->m_Disengage->GetRefCount() < 2) {
        delete this->m_Disengage;
        this->m_Disengage = nullptr;
    }

    if (this->m_Engage != nullptr && this->m_Engage->GetRefCount() < 2) {
        delete this->m_Engage;
        this->m_Engage = nullptr;
    }

    if (this->m_pShiftCtl->m_bNeed_ShiftGearSnd) {
        this->PlayShiftSnd();
    }

    if (this->m_pShiftCtl->m_bNeed_DisengageSnd) {
        this->PlayDisengageSnd();
    }

    if (this->m_pShiftCtl->m_bNeed_EngageSnd) {
        this->PlayEngageSnd();
    }

    if (this->m_pShiftCtl->m_bNeed_AccelSnd) {
        this->PlayAccelSnd();
    }

    if (this->m_pShiftCtl->m_bNeed_DeccelSnd) {
        this->PlayDecelSnd();
    }

    if (this->m_pShiftCtl->m_bShouldBeWhining) {
        this->PlayGearWhine();
    } else if (this->m_Whine != nullptr) {
        delete this->m_Whine;
        this->m_Whine = nullptr;
    }

    if (this->m_pShiftCtl->m_bBrakePedalMashed) {
        this->PlayBrakesMashed();
    } else if (this->m_BrakePedal != nullptr) {
        delete this->m_BrakePedal;
        this->m_BrakePedal = nullptr;
    }
}

void CARSFX_Shift::PlayGearWhine() {
    if (IsSoundEnabled == 0) {
        return;
    }

    if (this->m_Whine == nullptr) {
        g_pEAXSound->SetCsisName("Csis:Whine");
        this->m_Whine = new Csis::CAR_WHINE(0, 0, 0, 0, 0, 0, 25000, 0, 0x7FFF);

        if (this->m_Whine == nullptr) {
            return;
        } else {
            int refcount = this->m_Whine->GetRefCount();

            if (this->m_Whine == nullptr) {
                return;
            }
        }
    }

    {
        float whine_vol = static_cast<float>(this->GetDMixOutput(8, DMX_VOL));

        if (this->m_pEAXCar->GetPhysCar()->GetMaxRPM() > 0.0f) {
            whine_vol *= this->GetPhysRPM() / this->m_pEAXCar->GetPhysCar()->GetMaxRPM();
        } else {
            whine_vol = 0.0f;
        }

        this->m_Whine->SetVOL(static_cast<int>(whine_vol));
        this->m_Whine->SetCOMMON_PARAMETERS_AZIMUTH(this->GetDMixOutput(0, DMX_AZIM));
        this->m_Whine->SetRPM(static_cast<int>(this->GetPhysRPM()));
        this->m_Whine->CommitMemberData();
    }
}

void CARSFX_Shift::PlayDisengageSnd() {
    if (IsSoundEnabled == 0) {
        return;
    }

    float RPM = this->GetPhysRPM();
    if (RPM < 7000.0f) {
        return;
    }

    int SampleID = 0;
    int nVol = this->m_pSweetnersData->Vol_ShiftSweets();
    nVol = nVol * static_cast<int>(ShiftingAttackVolSlope.GetValue(RPM)) >> 15;
    int nDMixOut = this->GetDMixOutput(4, DMX_VOL);
    nVol = nVol * nDMixOut >> 15;

    g_pEAXSound->SetCsisName("SND ShiftDisEngage");
    this->m_Disengage = new Csis::CAR_SWTN(SampleID, this->m_pEAXCar->GetAttributes().CarID(), static_cast<int>(this->GetPhysRPM()), nVol,
                                           this->GetDMixOutput(0, DMX_AZIM), 0, 0);
}

void CARSFX_Shift::PlayEngageSnd() {
    if (IsSoundEnabled == 0) {
        return;
    }

    float RPM = this->m_pShiftCtl->RPM_AtShift;
    if (RPM < 7000.0f) {
        return;
    }

    int SampleID = 1;
    int nVol = this->m_pSweetnersData->Vol_ShiftSweets();
    nVol = nVol * static_cast<int>(ShiftingAttackVolSlope.GetValue(RPM)) >> 15;
    int nDMixOut = this->GetDMixOutput(3, DMX_VOL);
    nVol = nVol * nDMixOut >> 15;

    g_pEAXSound->SetCsisName("SND ShiftDisengage");
    this->m_Engage = new Csis::CAR_SWTN(SampleID, this->m_pEAXCar->GetAttributes().CarID(), static_cast<int>(this->GetPhysRPM()), nVol,
                                        this->GetDMixOutput(0, DMX_AZIM), 0, 0);
}

void CARSFX_Shift::PlayAccelSnd() {
    if (IsSoundEnabled == 0) {
        return;
    }

    float RPM = this->m_pShiftCtl->RPM_AtShift;
    int SampleID = 1;
    int nVol = this->m_pSweetnersData->Vol_ShiftSweets();
    nVol = nVol * static_cast<int>(ShiftingAttackVolSlope.GetValue(RPM)) >> 15;
    int nDMixOut = this->GetDMixOutput(5, DMX_VOL);
    nVol = nVol * nDMixOut >> 15;

    this->m_AccelSnd = new Csis::CAR_SWTN(SampleID, this->m_pEAXCar->GetAttributes().CarID(), static_cast<int>(this->GetPhysRPM()), nVol,
                                          this->GetDMixOutput(0, DMX_AZIM), 0, 0);
}

void CARSFX_Shift::PlayDecelSnd() {
    if (IsSoundEnabled == 0) {
        return;
    }

    float RPM = this->m_pShiftCtl->RPM_AtShift;
    int SampleID = 0;
    int nVol = this->m_pSweetnersData->Vol_ShiftSweets();
    nVol = nVol * static_cast<int>(ShiftingAttackVolSlope.GetValue(RPM)) >> 15;
    int nDMixOut = this->GetDMixOutput(6, DMX_VOL);
    nVol = nVol * nDMixOut >> 15;

    this->m_DeccelSnd = new Csis::CAR_SWTN(SampleID, this->m_pEAXCar->GetAttributes().CarID(), static_cast<int>(this->GetPhysRPM()), nVol,
                                           this->GetDMixOutput(0, DMX_AZIM), 0, 0);
}

// TODO move
extern int g_nArrayCosTable[513];

void CARSFX_Shift::PlayShiftSnd() {
    int tempVol = this->m_pShiftCtl->IsUpshifting() ? this->m_pShiftingPatternData->Up_Vol_Shift() : this->m_pShiftingPatternData->Down_Vol_Shift();

    float RPM = this->GetPhysRPM();
    float RPMDifScale = (static_cast<float>(static_cast<int>(this->GetPhysRPM())) - VOL_SHIFT_MIN_RPM_FOR_SCALE) /
                        (VOL_SHIFT_MAX_RPM_FOR_SCALE - VOL_SHIFT_MIN_RPM_FOR_SCALE);
    RPMDifScale = bClamp(RPMDifScale, 0.0f, 1.0f);
    float ShiftVolScale = RPMDifScale * 0.9f + VOL_SHIFT_MIN_SCALE;

    tempVol = tempVol * g_nArrayCosTable[static_cast<int>(512.0f - ShiftVolScale * 512.0f)] >> 15;

    int nDMixOut = this->m_pShiftCtl->IsUpshifting() ? this->GetDMixOutput(1, DMX_VOL) : this->GetDMixOutput(2, DMX_VOL);
    tempVol = tempVol * nDMixOut >> 15;

    if (this->m_ShiftGear != nullptr) {
        delete this->m_ShiftGear;
        this->m_ShiftGear = nullptr;
    }

    AEMS_SHIFTING_SAMPLES SampleID = this->m_pShiftCtl->IsUpshifting() ? AEMS_SHIFTING_UP : AEMS_SHIFTING_DOWN;
    int CameraView = static_cast<int>(this->m_pEAXCar->GetPOV() == 0);

    g_pEAXSound->SetCsisName("SND: Shift");
    this->m_ShiftGear =
        new Csis::FX_SHIFTING_01(SampleID, tempVol, 0x1000, this->GetDMixOutput(0, DMX_AZIM), Csis::FXSHIFTING01TYPETYPE_SHIFT, CameraView);
}

void CARSFX_Shift::PlayBrakesMashed() {
    if (IsSoundEnabled == 0 || this->m_BrakePedal != nullptr) {
        return;
    }

    int nDMixOut = this->GetDMixOutput(10, DMX_VOL);
    int CameraView = static_cast<int>(this->m_pEAXCar->GetPOV() == 0);

    g_pEAXSound->SetCsisName("SND: BrakeMash");
    this->m_BrakePedal =
        new Csis::FX_SHIFTING_01(2, nDMixOut, 0x1000, this->GetDMixOutput(0, DMX_AZIM), Csis::FXSHIFTING01TYPETYPE_SHIFT, CameraView);

    if (this->m_BrakePedal != nullptr) {
        int refcount = this->m_BrakePedal->GetRefCount();
    }
}

void CARSFX_Shift::ProcessUpdate() {
    if (g_EAXIsPaused()) {
        if (this->m_DeccelSnd != nullptr) {
            this->m_DeccelSnd->SetVOL(0);
            this->m_DeccelSnd->CommitMemberData();
        }

        if (this->m_AccelSnd != nullptr) {
            this->m_AccelSnd->SetVOL(0);
            this->m_AccelSnd->CommitMemberData();
        }

        if (this->m_ShiftGear != nullptr) {
            this->m_ShiftGear->SetVolume(0);
            this->m_ShiftGear->CommitMemberData();
        }

        if (this->m_BrakePedal != nullptr) {
            this->m_BrakePedal->SetVolume(0);
            this->m_BrakePedal->CommitMemberData();
        }

        if (this->m_Disengage != nullptr) {
            this->m_Disengage->SetVOL(0);
            this->m_Disengage->CommitMemberData();
        }

        if (this->m_Engage != nullptr) {
            this->m_Engage->SetVOL(0);
            this->m_Engage->CommitMemberData();
        }

        if (this->m_Whine != nullptr) {
            this->m_Whine->SetVOL(0);
            this->m_Whine->CommitMemberData();
        }
    }
}
