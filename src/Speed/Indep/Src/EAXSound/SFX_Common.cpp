#include "./SFX_Common.hpp"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/MAIN_AEMS.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audiosystem.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

SndBase::TypeInfo SFX_Common::s_TypeInfo = { 0x40, "SFX_Common", &SndBase::s_TypeInfo, SFX_Common::CreateObject };

SndBase::TypeInfo *SFX_Common::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFX_Common::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFX_Common::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFX_Common::GetStaticTypeInfo()->typeName, false) SFX_Common();
    }
    return new (SFX_Common::GetStaticTypeInfo()->typeName, true) SFX_Common();
}

SFX_Common::SFX_Common() {
    mMsgMiscSound =
        Hermes::Handler::Create<MMiscSound, SFX_Common, SFX_Common>(this, &SFX_Common::MsgPlayMiscSound, "Snd", 0);
    m_pcsisCameraShot = nullptr;
    m_pUves = nullptr;
    m_pPursuitBreakStart = nullptr;
    m_pPursuitBreakEnd = nullptr;
}

SFX_Common::~SFX_Common() {
    if (mMsgMiscSound) {
        Hermes::Handler::Destroy(mMsgMiscSound);
    }

    if (m_pcsisCameraShot) {
        delete m_pcsisCameraShot;
    }

    m_pcsisCameraShot = nullptr;

    if (m_pUves) {
        delete m_pUves;
    }

    m_pUves = nullptr;

    if (m_pPursuitBreakStart) {
        delete m_pPursuitBreakStart;
    }

    m_pPursuitBreakStart = nullptr;

    if (m_pPursuitBreakEnd) {
        delete m_pPursuitBreakEnd;
    }

    m_pPursuitBreakEnd = nullptr;
}

void SFX_Common::AttachController(SFXCTL *psfxctl) {}

void SFX_Common::Destroy() {}

void SFX_Common::MsgPlayMiscSound(const MMiscSound &message) {
    SetDMIX_Input(message.GetSoundID(), 0x7FFF);

    switch (message.GetSoundID()) {
    case 1:
        if (!m_pUves) {
            m_pUves = new FX_UVES(0, 0, 0, 0, 0, 0);
        }
        break;
    case 2:
        if (!m_pcsisCameraShot) {
            m_pcsisCameraShot = new FX_Camera(0, GetDMixOutput(0, DMX_VOL), 0, 0, 0, 0, 0, 0);
        }
        break;
    case 3:
        delete m_pPursuitBreakStart;
        m_pPursuitBreakStart = new FX_UVES(2, 0, 0, 0, 0, 0);
        m_pPursuitBreakStart->GetRefCount();
        break;
    case 4:
        delete m_pPursuitBreakEnd;
        m_pPursuitBreakEnd = new FX_UVES(1, 0, 0, 0, 0, 0);
        m_pPursuitBreakEnd->GetRefCount();
        break;
    default:
        break;
    }
}
void SFX_Common::UpdateParams(float t) {}

void SFX_Common::ProcessUpdate() {
    if (m_pcsisCameraShot) {
        if (m_pcsisCameraShot->GetRefCount() < 2) {
            delete m_pcsisCameraShot;
            m_pcsisCameraShot = nullptr;
        }

        if (m_pcsisCameraShot) {
            m_pcsisCameraShot->SetVolume(GetDMixOutput(0, DMX_VOL));
            m_pcsisCameraShot->CommitMemberData();
        }
    }

    if (m_pUves) {
        if (m_pUves->GetRefCount() < 2) {
            delete m_pUves;
            m_pUves = nullptr;
        }

        if (m_pUves) {
            m_pUves->SetVolume(GetDMixOutput(2, DMX_VOL));
            m_pUves->CommitMemberData();
        }
    }

    if (m_pPursuitBreakStart) {
        if (m_pPursuitBreakStart->GetRefCount() < 2) {
            delete m_pPursuitBreakStart;
            m_pPursuitBreakStart = nullptr;
        }

        if (m_pPursuitBreakStart) {
            m_pPursuitBreakStart->SetVolume(GetDMixOutput(3, DMX_VOL));
            m_pPursuitBreakStart->CommitMemberData();
        }
    }

    if (m_pPursuitBreakEnd) {
        if (m_pPursuitBreakEnd->GetRefCount() < 2) {
            delete m_pPursuitBreakEnd;
            m_pPursuitBreakEnd = nullptr;
        }

        if (m_pPursuitBreakEnd) {
            m_pPursuitBreakEnd->SetVolume(GetDMixOutput(4, DMX_VOL));
            m_pPursuitBreakEnd->CommitMemberData();
        }
    }

    bMemSet(GetOutputBlockPtr(), '\0', 0x14);
}

void SFX_Common::SetupLoadData() {
    LoadAsset(g_pEAXSound->GetAttributes().AEMS_FEBanks(1), SNDPATH_GLOBAL, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE,
              true);
    if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
        LoadAsset(g_pEAXSound->GetAttributes().AEMS_FEBanks(0), SNDPATH_FE, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE,
                  true);
    }
}
