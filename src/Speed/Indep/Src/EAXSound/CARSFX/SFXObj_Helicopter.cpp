#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Helicopter.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

DEFINE_CREATABLE(0xb0000, SFXObj_Helicopter, SndBase);

SFXObj_Helicopter::SFXObj_Helicopter() : CARSFX() {
    this->m_HeliAemsData.hELI_LowPass = 16000;
    this->m_pCsisHeli = nullptr;
    this->m_HeliAemsData.hELI_Azmuth = 0;
    this->m_HeliAemsData.hELI_Distance = 0;
    this->m_HeliAemsData.hELI_FX_Dry = 0;
    this->m_HeliAemsData.hELI_FX_Wet = 0;
    this->m_HeliAemsData.hELI_ID = 0;
    this->m_HeliAemsData.hELI_HiPass = 0;
    this->m_HeliAemsData.hELI_Pitch_Offset = 0;
    this->m_HeliAemsData.hELI_Speed = 0;
    this->m_HeliAemsData.hELI_Stop = 0;
    this->m_HeliAemsData.hELI_Volume = 0;
    this->m_HeliAemsData.hELI_Rotation = 0;
    this->m_pHeliCtl = nullptr;
}

SFXObj_Helicopter::~SFXObj_Helicopter() {
    this->Destroy();
}

void SFXObj_Helicopter::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void SFXObj_Helicopter::InitSFX() {
    g_pEAXSound->SetCsisName(this);
    this->m_pCsisHeli = new Csis::FX_Helicopter(
        this->m_HeliAemsData.hELI_ID, this->m_HeliAemsData.hELI_Pitch_Offset, this->m_HeliAemsData.hELI_Stop, this->m_HeliAemsData.hELI_Volume,
        this->m_HeliAemsData.hELI_Azmuth, this->m_HeliAemsData.hELI_Speed, this->m_HeliAemsData.hELI_Distance, this->m_HeliAemsData.hELI_LowPass,
        this->m_HeliAemsData.hELI_HiPass, this->m_HeliAemsData.hELI_FX_Dry, this->m_HeliAemsData.hELI_FX_Wet, this->m_HeliAemsData.hELI_Rotation);
    SndBase::InitSFX();
}

void SFXObj_Helicopter::Detach() {
    this->Destroy();
}

int SFXObj_Helicopter::GetController(int Index) {
    if (Index == 0) {
        return 0;
    }
    return -1;
}

void SFXObj_Helicopter::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 0) {
        this->m_pHeliCtl = static_cast<SFXCTL_Helicopter *>(psfxctl);
    }
}

void SFXObj_Helicopter::Destroy() {
    delete this->m_pCsisHeli;
    this->m_pCsisHeli = nullptr;
}

void SFXObj_Helicopter::UpdateParams(float t) {}

void SFXObj_Helicopter::ProcessUpdate() {
    if (this->m_pHeliCtl != nullptr && this->m_pHeliCtl->m_pHeliState != nullptr) {
        if (this->m_pHeliCtl->m_pHeliState->IsSimUpdating() == true) {
            this->m_HeliAemsData.hELI_Speed = static_cast<int>(this->m_pHeliCtl->m_fspeed);
            this->m_HeliAemsData.hELI_Distance = static_cast<int>(this->m_pHeliCtl->m_fdist);

            int nazim = this->GetDMixOutput(0, DMX_AZIM);

            if (this->m_HeliAemsData.hELI_Distance > 100) {
                this->m_HeliAemsData.hELI_Distance = 100;
            }

            this->m_HeliAemsData.hELI_Rotation = this->m_pHeliCtl->m_Rotation;

            if (nazim - 0x4001U < 0x7FFF) {
                this->m_HeliAemsData.hELI_Distance = -this->m_HeliAemsData.hELI_Distance;
            }

            if (this->m_pCsisHeli == nullptr) {
                return;
            }

            this->m_pCsisHeli->SetHELI_Azmuth(nazim);
            this->m_pCsisHeli->SetHELI_Distance(this->m_HeliAemsData.hELI_Distance);
            this->m_pCsisHeli->SetHELI_Volume(this->GetDMixOutput(1, DMX_VOL));
            this->m_pCsisHeli->SetHELI_FX_Dry(this->GetDMixOutput(1, DMX_VOL));
            this->m_pCsisHeli->SetHELI_Speed(this->m_HeliAemsData.hELI_Speed);
            this->m_pCsisHeli->SetHELI_Pitch_Offset(this->GetDMixOutput(2, DMX_PITCH));
            this->m_pCsisHeli->SetHELI_Rotation(this->m_HeliAemsData.hELI_Rotation);
            this->m_pCsisHeli->CommitMemberData();
        } else if (this->m_pCsisHeli != nullptr) {
            this->m_pCsisHeli->SetHELI_Volume(0);
            this->m_pCsisHeli->SetHELI_FX_Dry(0);
            this->m_pCsisHeli->CommitMemberData();
        }
    } else if (this->m_pCsisHeli != nullptr) {
        this->m_pCsisHeli->SetHELI_Volume(0);
        this->m_pCsisHeli->SetHELI_FX_Dry(0);
        this->m_pCsisHeli->CommitMemberData();
    }
}
