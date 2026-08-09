#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Siren.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SoundCollision.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

DEFINE_CREATABLE(0x40110, CARSFX_Siren, SndBase);

CARSFX_Siren::CARSFX_Siren()
    : CARSFX(), //
      mSiren(nullptr) {}

CARSFX_Siren::~CARSFX_Siren() {
    this->Destroy();
}

void CARSFX_Siren::Detach() {
    this->Destroy();
}

void CARSFX_Siren::Destroy() {
    delete this->mSiren;
    this->mSiren = nullptr;
}

int CARSFX_Siren::GetController(int Index) {
    return -1;
}

void CARSFX_Siren::AttachController(SFXCTL *psfxctl) {}

void CARSFX_Siren::SetupSFX(CSTATE_Base *_StateBase) {
    this->SndBase::SetupSFX(_StateBase);
}

void CARSFX_Siren::InitSFX() {
    if (IsSoundEnabled != 0) {
        this->SndBase::InitSFX();
        this->bIgnoreHealth = this->GetPhysCar()->mHealth < 0.01f;
        this->m_SirenState = Sound::SIREN_OFF;
        this->m_PrevSirenState = Sound::SIREN_OFF;
        this->tSirenState = g_pEAXSound->Random(2.0f) + 6.0f;
    }
}

void CARSFX_Siren::UpdateParams(float t) {
    this->SndBase::UpdateParams(t);
    EAX_CarState *thisCar = this->GetPhysCar();
    if (thisCar != nullptr) {
        float t_death = (WorldTimer - this->mT_death).GetSeconds();
        Sound::SirenState state = thisCar->GetSirenState();

        if (state == Sound::SIREN_OFF || (this->m_SirenState == Sound::SIREN_DIE && t_death > 10.0f)) {
            this->Disable();
        } else {
            this->Enable();
        }

        if (state == Sound::SIREN_OFF) {
            if (this->IsEnabled()) {
                this->Disable();
            }
            return;
        }

        if (!this->IsEnabled()) {
            this->Enable();
        }
    }
}

// UNSOLVED
void CARSFX_Siren::ProcessUpdate() {
    if (this->IsEnabled()) {
        this->SetDMIX_Input(5, 0x7FFF);

        if (this->mSiren == nullptr) {
            g_pEAXSound->SetCsisName(this);
            this->mSiren = new Csis::SIREN(0, 0, 0, 0, 0, 25000, 0, 0x7FFF, 0);
            if (this->mSiren == nullptr) {
                return;
            }
        }

        {
            this->mSiren->SetVOL(this->GetDMixOutput(1, DMX_VOL));
            this->mSiren->SetPAN(this->GetDMixOutput(0, DMX_AZIM));
            this->mSiren->SetPITCH_OFFS(this->GetDMixOutput(3, DMX_PITCH) - 0x1000);

            int CurDist = static_cast<int>(Sound::DistanceToView(this->GetPhysCar()->GetPosition()) * 12.8f);
            this->mSiren->SetDISTANCE(CurDist);

            this->m_PrevSirenState = this->m_SirenState;
            this->m_SirenState = this->UpdateSirenState(this->m_pStateBase->GetDeltaTime());

            int input = 0;
            switch (this->m_SirenState) {
                case Sound::SIREN_OFF:
                    this->Disable();
                    break;
                case Sound::SIREN_WAIL:
                case Sound::SIREN_YELP:
                case Sound::SIREN_SCREAM:
                    input = this->m_SirenState;
                    this->mT_death = WorldTimer;
                    break;
                case Sound::SIREN_DIE:
                    input = Sound::SIREN_DIE;
                    if (this->m_PrevSirenState != Sound::SIREN_DIE) {
                        this->mT_death = WorldTimer;
                    }
                    break;
                default:
                    break;
            }

            this->mSiren->SetTYPE(input);
            this->mSiren->CommitMemberData();
        }
    } else {
        this->SetDMIX_Input(5, 0);

        if (this->mSiren == nullptr) {
            return;
        }

        delete this->mSiren;
        this->mSiren = nullptr;
    }
}

Sound::SirenState CARSFX_Siren::UpdateSirenState(float t) {
    this->tSirenState = this->tSirenState - t;
    if (this->tSirenState < 0.0f || this->GetPhysCar()->GetSirenState() == Sound::SIREN_SCREAM) {
        this->tSirenState = g_pEAXSound->Random(3.0f);
        return this->GetPhysCar()->GetSirenState();
    }
    return this->m_SirenState;
}
