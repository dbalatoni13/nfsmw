#include "Speed/Indep/Src/EAXSound/Stream/SpeechModule.hpp"

namespace Speech {

Module::Module()
    : m_speechBanks(nullptr),    //
      m_bankHeaders(nullptr),    //
      m_numBanks(0),             //
      m_flags(0),                //
      m_strm(nullptr),           //
      mLastEventTimestamp(0),    //
      m_pSFXOBJ_Speech(nullptr), //
      m_pSFXOBJ_Moment(nullptr), //
      m_pSFXOBJ_NISStream(nullptr) {}

Module::~Module() {}

unsigned int Module::GetBankOffset(int bnum) {
    for (int i = 0; i < this->m_numBanks; i++) {
        if (this->m_speechBanks[i].bank == bnum) {
            return static_cast<unsigned int>(this->m_speechBanks[i].offset);
        }
    }
    return static_cast<unsigned int>(-1);
}

void Module::AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype) {
    this->m_enable = true;
    if (psfx != nullptr) {
        if (psfx->GetGroupID()) {
            return;
        }
        if (sfxtype == SFXOBJ_SPEECH) {
            this->m_pSFXOBJ_Speech = psfx;
        }
        if (sfxtype == SFXOBJ_NISPROJ_STRMS) {
            this->m_pSFXOBJ_NISStream = psfx;
        }
        if (sfxtype == SFXOBJ_MOMENT_STRMS) {
            this->m_pSFXOBJ_Moment = psfx;
        }
    } else {
        switch (sfxtype) {
            case SFXOBJ_SPEECH:
                this->m_pSFXOBJ_Speech = nullptr;
                break;
            case SFXOBJ_NISPROJ_STRMS:
                this->m_pSFXOBJ_NISStream = nullptr;
                break;
            case SFXOBJ_MOMENT_STRMS:
                this->m_pSFXOBJ_Moment = nullptr;
                break;
            default:
                break;
        }
    }
}

bool Module::DonePlaying() {
    return this->m_strm->AlmostDone();
}

void Module::PurgeSpeech() {
    this->m_strm->PurgeStream();
}

bool Module::PlayStream(int stream_id) {
    return true;
}

void Module::UnPause() {
    this->m_strm->Resume();
}

void Module::ReleaseResource() {
    this->m_strm->PurgeStream();
}

} // namespace Speech
