#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Nitrous.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"

DEFINE_CREATABLE(0x20050, CARSFX_Nitrous, SndBase);

CARSFX_Nitrous::CARSFX_Nitrous() : CARSFX() {
    this->m_NitrousControl = nullptr;
    this->eNitrousState = SFX_NITROUS_NONE;
    this->m_NitrousPurge = nullptr;
    this->m_pShiftCtl = nullptr;
}

CARSFX_Nitrous::~CARSFX_Nitrous() {
    Destroy();
}

int CARSFX_Nitrous::GetController(int Index) {
    switch (Index) {
        case 0:
            return 7;
        case 1:
            return 2;
        default:
            return -1;
    }
}

void CARSFX_Nitrous::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
        case 7:
            this->m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
            break;
        case 2:
            this->m_pShiftCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
            break;
    }
}

void CARSFX_Nitrous::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_UGL = this->m_pEAXCar->m_NOSUGL;
    this->m_NitrousPitchBoost.Initialize(0.0f, 0.0f, 1, LINEAR);
}

void CARSFX_Nitrous::InitSFX() {
    SndBase::InitSFX();
    this->Enable();
    int nTurboID;
}

#ifdef CLANGD_DAMNIT
extern int IsSoundEnabled; // Decl: zEAXSound2.cpp: 19
#endif

int CARSFX_Nitrous::Play(int type, int Vol, int Azimuth) {
    if (!this->IsEnabled() || IsSoundEnabled != 1) {
        return 0;
    } else {
        int nDMixOut = this->GetDMixOutput(1, DMX_VOL);

        if (this->m_NitrousControl != nullptr) {
            delete this->m_NitrousControl;
            this->m_NitrousControl = nullptr;
        }

        g_pEAXSound->SetCsisName(this);
        this->m_NitrousControl = new Csis::FX_NITROUS(type, nDMixOut, this->GetDMixOutput(0, DMX_AZIM), 0, 0, 25000, 0, 0x7FFF, 0);

        int m_NitrousControl_ref = this->m_NitrousControl->GetRefCount();

        if (INIS::Exists()) {
            this->SetDMIX_Input(1, 0);
        } else {
            this->SetDMIX_Input(1, 0x7FFF);
        }
    }

    return 0;
}

void CARSFX_Nitrous::Stop() {
    this->SetDMIX_Input(1, 0);

    if (this->m_NitrousControl != nullptr) {
        this->m_NitrousControl->SetNIT_STOP(1);
        this->m_NitrousControl->CommitMemberData();
    }
}

void CARSFX_Nitrous::PlayPurge() {
    this->GetPhysCar()->SetNosEmptyFlag(false);

    if (this->m_NitrousPurge != nullptr) {
        delete this->m_NitrousPurge;
        this->m_NitrousPurge = nullptr;
    }

    g_pEAXSound->SetCsisName(this);
    this->m_NitrousPurge = new Csis::FX_PURGE(0, 0, 0, 0, 0x61A8, 0, 0x7FFF, 0);

    if (INIS::Exists()) {
        this->SetDMIX_Input(2, 0);
    } else {
        this->SetDMIX_Input(2, 0x7FFF);
    }
}

// STRIPPED
void CARSFX_Nitrous::StopPurge() {}

void CARSFX_Nitrous::Destroy() {
    delete this->m_NitrousControl;
    this->m_NitrousControl = nullptr;

    delete this->m_NitrousPurge;
    this->m_NitrousPurge = nullptr;
}

void CARSFX_Nitrous::UpdateParams(float t) {
    if (this->GetGroupID() != 2) {
        return;
    }

    SndBase::UpdateParams(t);
    if (!this->IsEnabled()) {
        return;
    }

    int nSTATE = this->m_pEAXCar->GetPhysCar()->GetNosPercentageLeft();
    unsigned int uNOSHash = bStringHash("GB_NOS");
    EAX_CarState *FAKE = this->m_pEAXCar->GetPhysCar();

    switch (this->eNitrousState) {
        case SFX_NITROUS_NONE:
            if (this->m_pEAXCar->GetPhysCar()->GetNitroFlag()) {
                this->m_pEAXCar->GetPhysCar()->GetNosPercentageLeft();
                this->GetPhysCar()->IsLocalPlayerCar();

                this->eNitrousState = SFX_NITROUS_ON;
                this->Play(0, 0, this->m_pEAXCar->GetRotation());
                this->m_NitrousPitchBoost.Initialize(0.0f, 1.0f, 400, LINEAR);
            }
            break;

        case SFX_NITROUS_ON:
            if (!this->m_pEAXCar->GetPhysCar()->GetNitroFlag()) {
                if (this->m_NitrousControl != nullptr) {
                    this->Stop();
                }

                this->m_NitrousPitchBoost.Initialize(this->m_NitrousPitchBoost.GetValue(), 0.0f, 1000, LINEAR);

                this->eNitrousState = SFX_NITROUS_NONE;
            }
            break;
    }

    this->GetStateBase();

    if (this->GetPhysCar()->GetNosEmptyFlag() && this->m_NitrousPurge == nullptr) {
        this->PlayPurge();
    }
}

void CARSFX_Nitrous::ProcessUpdate() {
    if (this->m_NitrousControl != nullptr) {
        if (INIS::Exists()) {
            this->SetDMIX_Input(1, 0);
            this->SetDMIX_Input(2, 0);
        }

        if (this->m_NitrousControl->GetRefCount() < 2) {
            delete this->m_NitrousControl;
            this->m_NitrousControl = nullptr;
        } else {
            int nDMixOut;

            this->m_NitrousControl->SetNIT_volume(this->GetDMixOutput(1, DMX_VOL));
            this->m_NitrousControl->SetNIT_azimuth(this->GetDMixOutput(0, DMX_AZIM));

            int Pitch = this->GetDMixOutput(3, DMX_PITCH);
            this->m_NitrousControl->SetPitch(Pitch);

            nDMixOut = this->GetDMixOutput(4, DMX_FREQ);
            this->m_NitrousControl->SetFilter_Effects_LoPass(nDMixOut);
            this->m_NitrousControl->CommitMemberData();
        }
    }

    if (this->m_NitrousPurge != nullptr) {
        if (this->m_NitrousPurge->GetRefCount() < 2) {
            delete this->m_NitrousPurge;
            this->m_NitrousPurge = nullptr;
        } else {
            int Pitch = 0x1000;

            this->m_NitrousPurge->SetPURGE_volume(this->GetDMixOutput(2, DMX_VOL));
            this->m_NitrousPurge->SetPURGE_azimuth(this->GetDMixOutput(0, DMX_AZIM));
            this->m_NitrousPurge->SetPURGE_pitch(Pitch);
            this->m_NitrousPurge->CommitMemberData();
        }
    }
}
