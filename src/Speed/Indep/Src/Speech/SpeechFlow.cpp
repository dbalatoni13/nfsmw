#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"

namespace Speech {

namespace {
Module *sSpeechModules[NUM_SPEECH_MODULES] = { 0, 0 };
}

Module *Manager::GetSpeechModule(int nindex) {
    if (nindex > 1) {
        return 0;
    }
    return sSpeechModules[nindex];
}

int Manager::TestSentenceRuleCallback(EventSpec *, int, int, int) {
    return 0;
}

int Manager::ReparmCallback(int, unsigned int *) {
    return 0;
}

void Manager::SetSentenceRuleCallback(EventSpec *, int, int, int) {}

SPCHType_EventRuleResult Manager::EventRuleCallback(EventSpec *) {
    return kSPCH_EventRule_OK;
}

Module::Module()
    : m_enable(false), //
      m_datID(0), //
      m_projID(0), //
      m_speechBanks(0), //
      m_mixChannel(static_cast<eMasterMixChannel>(0)), //
      m_streamID(0), //
      m_fileNum(0), //
      m_bankHeaders(0), //
      m_numBanks(0), //
      m_flags(0), //
      m_strm(0), //
      m_filename(), //
      mLastEventTimestamp(0), //
      m_pSFXOBJ_Speech(0), //
      m_pSFXOBJ_Moment(0), //
      m_pSFXOBJ_NISStream(0), //
      m_bIsStreamQueued(false) {}

Module::~Module() {}

unsigned int Module::GetBankOffset(int bnum) {
    for (int i = 0; i < m_numBanks; i++) {
        if (m_speechBanks[i].bank == bnum) {
            return static_cast<unsigned int>(m_speechBanks[i].offset);
        }
    }
    return static_cast<unsigned int>(-1);
}

void Module::AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype) {
    m_enable = true;
    if (psfx && psfx->GetGroupID()) {
        return;
    }
    if (sfxtype == SFXOBJ_SPEECH) {
        m_pSFXOBJ_Speech = psfx;
    }
    if (sfxtype == SFXOBJ_NISPROJ_STRMS) {
        m_pSFXOBJ_NISStream = psfx;
    }
    if (sfxtype == SFXOBJ_MOMENT_STRMS) {
        m_pSFXOBJ_Moment = psfx;
    }
}

bool Module::DonePlaying() {
    return m_strm->AlmostDone();
}

void Module::PurgeSpeech() {
    m_strm->PurgeStream();
}

bool Module::PlayStream(int) {
    return true;
}

void Module::UnPause() {
    m_strm->Resume();
}

void Module::ReleaseResource() {
    m_strm->PurgeStream();
}

} // namespace Speech
