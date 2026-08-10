#include "Speed/Indep/Src/EAXSound/SFX_Common.hpp"

DEFINE_CREATABLE(0x40, SFX_Common, SndBase);

SFX_Common::SFX_Common() {
    this->mMsgMiscSound = Hermes::Handler::Create<MMiscSound, SFX_Common, SFX_Common>(this, &SFX_Common::MsgPlayMiscSound, "Snd", 0);
    this->m_pcsisCameraShot = nullptr;
    this->m_pUves = nullptr;
#ifndef EA_BUILD_A124
    this->m_pPursuitBreakStart = nullptr;
    this->m_pPursuitBreakEnd = nullptr;
#endif
}

SFX_Common::~SFX_Common() {
    if (this->mMsgMiscSound != nullptr) {
        Hermes::Handler::Destroy(this->mMsgMiscSound);
    }

    delete this->m_pcsisCameraShot;
    this->m_pcsisCameraShot = nullptr;

    delete this->m_pUves;
    this->m_pUves = nullptr;

#ifndef EA_BUILD_A124
    delete this->m_pPursuitBreakStart;
    this->m_pPursuitBreakStart = nullptr;

    delete this->m_pPursuitBreakEnd;
    this->m_pPursuitBreakEnd = nullptr;
#endif
}

void SFX_Common::AttachController(SFXCTL *psfxctl) {}

void SFX_Common::Destroy() {}

// UNSOLVED because of Csis::FX_UVES
void SFX_Common::MsgPlayMiscSound(const MMiscSound &message) {
    this->SetDMIX_Input(message.GetSoundID(), 0x7FFF);

    switch (message.GetSoundID()) {
        case 1:
            if (this->m_pUves == nullptr) {
                this->m_pUves = new Csis::FX_UVES(0, 0, 0, 0, 0, 0);
            }
            break;
        case 2:
            if (this->m_pcsisCameraShot == nullptr) {
                this->m_pcsisCameraShot = new Csis::FX_Camera(0, this->GetDMixOutput(0, DMX_VOL), 0, 0, 0, 0, 0, 0);
            }
            break;
#ifndef EA_BUILD_A124
        case 3:
            delete this->m_pPursuitBreakStart;
            this->m_pPursuitBreakStart = new Csis::FX_UVES(2, 0, 0, 0, 0, 0);
            this->m_pPursuitBreakStart->GetRefCount();
            break;
        case 4:
            delete this->m_pPursuitBreakEnd;
            this->m_pPursuitBreakEnd = new Csis::FX_UVES(1, 0, 0, 0, 0, 0);
            this->m_pPursuitBreakEnd->GetRefCount();
            break;
#endif
        default:
            break;
    }
}

void SFX_Common::UpdateParams(float t) {}

void SFX_Common::ProcessUpdate() {
    if (this->m_pcsisCameraShot != nullptr) {
        if (this->m_pcsisCameraShot->GetRefCount() < 2) {
            delete this->m_pcsisCameraShot;
            this->m_pcsisCameraShot = nullptr;
        }

        if (this->m_pcsisCameraShot != nullptr) {
            this->m_pcsisCameraShot->SetVolume(this->GetDMixOutput(0, DMX_VOL));
            this->m_pcsisCameraShot->CommitMemberData();
        }
    }

    if (this->m_pUves != nullptr) {
        if (this->m_pUves->GetRefCount() < 2) {
            delete this->m_pUves;
            this->m_pUves = nullptr;
        }

        if (this->m_pUves != nullptr) {
            this->m_pUves->SetVolume(this->GetDMixOutput(2, DMX_VOL));
            this->m_pUves->CommitMemberData();
        }
    }

#ifndef EA_BUILD_A124
    if (this->m_pPursuitBreakStart != nullptr) {
        if (this->m_pPursuitBreakStart->GetRefCount() < 2) {
            delete this->m_pPursuitBreakStart;
            this->m_pPursuitBreakStart = nullptr;
        }

        if (this->m_pPursuitBreakStart != nullptr) {
            this->m_pPursuitBreakStart->SetVolume(this->GetDMixOutput(3, DMX_VOL));
            this->m_pPursuitBreakStart->CommitMemberData();
        }
    }

    if (this->m_pPursuitBreakEnd != nullptr) {
        if (this->m_pPursuitBreakEnd->GetRefCount() < 2) {
            delete this->m_pPursuitBreakEnd;
            this->m_pPursuitBreakEnd = nullptr;
        }

        if (this->m_pPursuitBreakEnd != nullptr) {
            this->m_pPursuitBreakEnd->SetVolume(this->GetDMixOutput(4, DMX_VOL));
            this->m_pPursuitBreakEnd->CommitMemberData();
        }
    }
#endif

    // TODO 64 bit what size is that?
    bMemSet(this->GetOutputBlockPtr(), 0, 0x14);
}
