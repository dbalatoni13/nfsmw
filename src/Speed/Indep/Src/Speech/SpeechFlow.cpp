#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"
#include "Speed/Indep/Src/EAXSound/Stream/GameSpeech.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/NISSFXModule.hpp"
#include "Speed/Indep/Src/Speech/SpeechFlow.h"

namespace Speech {

Module *Manager::m_SpeechModule[NUM_SPEECH_MODULES] = { 0, 0 };
short Manager::m_frameindex = 0;
short Manager::mLastSpeakerID = 0;
SampleReqList Manager::mSampleRequests;

char *GameSpeech::m_tempCharPtr = 0;
CLUMP_IDX_FILEtag *GameSpeech::m_clumpIdx = 0;
char *GameSpeech::m_csisData = 0;
int GameSpeech::m_channel = 0;
char *GameSpeech::m_eventDat = 0;

char *SED_NISSFX::m_tempCharPtr = 0;
CLUMP_IDX_FILEtag *SED_NISSFX::m_clumpIdx = 0;
char *SED_NISSFX::m_csisData = 0;
int SED_NISSFX::m_channel = 0;
char *SED_NISSFX::m_eventDat = 0;
bool SED_NISSFX::m_dataIsLoaded = false;

SpeechFlow::SpeechFlow()
    : mState(0), //
      mLastState(-1), //
      mBusy(0) {}

SpeechFlow::~SpeechFlow() {}

void SpeechFlow::ChangeStateTo(int new_state) {
    if (new_state != mState) {
        mLastState = mState;
    }
    mState = new_state;
}

int SpeechFlow::GetState() {
    return mState;
}

void SpeechFlow::Reset() {
    ChangeStateTo(-1);
    mBusy = 0;
}

bool SpeechFlow::IsBusy() {
    return mBusy != 0;
}

void SpeechFlow::OnCopAdded(EAXCop *) {}

void SpeechFlow::OnCopRemoved(EAXCop *) {}

Module *Manager::GetSpeechModule(int nindex) {
    if (nindex > 1) {
        return 0;
    }
    return m_SpeechModule[nindex];
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

int Manager::GetGlobalHistoryCount(SPCHType_1_EventID) {
    return 0;
}

void Manager::ScheduleSpeechPartII(unsigned int sample_size, void *sample_data, Csis::InterfaceId &, Csis::FunctionHandle &, EAXCharacter *actor) {
    if (!sample_data || !actor || (sample_size < sizeof(SPCHType_SampleRequestData))) {
        return;
    }

    SPCHSampleRequest req;
    req.data = *reinterpret_cast<SPCHType_SampleRequestData *>(sample_data);
    req.owner = 0;
    req.offset = static_cast<unsigned int>(req.data.sampleOffset);
    req.sample_index = 0xFF;
    mLastSpeakerID = static_cast<short>(actor->GetSpeakerID());
    mSampleRequests.push_back(req);
}

void Manager::ClearPlayback() {
    mSampleRequests.clear();
}

void Manager::Init(int mode) {
    mSampleRequests.clear();
    m_frameindex = 0;
    mLastSpeakerID = 0;

    if (!m_SpeechModule[COPSPEECH_MODULE]) {
        m_SpeechModule[COPSPEECH_MODULE] = new GameSpeech();
    }
    if (m_SpeechModule[COPSPEECH_MODULE]) {
        m_SpeechModule[COPSPEECH_MODULE]->Init(mode);
    }

    if (!m_SpeechModule[NISSFX_MODULE]) {
        m_SpeechModule[NISSFX_MODULE] = new SED_NISSFX();
    }
    if (m_SpeechModule[NISSFX_MODULE]) {
        m_SpeechModule[NISSFX_MODULE]->Init(2);
    }
}

void Manager::Destroy() {
    mSampleRequests.clear();
    for (int i = 0; i < NUM_SPEECH_MODULES; ++i) {
        if (m_SpeechModule[i]) {
            delete m_SpeechModule[i];
            m_SpeechModule[i] = 0;
        }
    }
}

void Manager::Deduce() {
    Update(0.0f);
}

void Manager::Update(float) {
    for (int i = 0; i < NUM_SPEECH_MODULES; ++i) {
        if (m_SpeechModule[i]) {
            m_SpeechModule[i]->Update();
        }
    }
}

void Manager::AttachSFXOBJ(SpeechModuleIndex module, SFX_Base *psb, eSFXOBJ_MAIN_TYPES type) {
    if ((module < 0) || (module >= NUM_SPEECH_MODULES)) {
        return;
    }
    if (m_SpeechModule[module]) {
        m_SpeechModule[module]->AttachSFXOBJ(psb, type);
    }
}

bool Manager::IsPlaying(SpeechModuleIndex module) {
    if ((module < 0) || (module >= NUM_SPEECH_MODULES)) {
        return false;
    }
    return m_SpeechModule[module] && !m_SpeechModule[module]->DonePlaying();
}

bool Manager::IsCopSpeechPlaying(SPCHType_1_EventID) {
    return IsPlaying(COPSPEECH_MODULE);
}

bool Manager::IsCopSpeechBusy() {
    return !mSampleRequests.empty();
}

Timer Manager::GetTimeSinceLastEvent(SpeechModuleIndex) {
    return Timer(0);
}

void Manager::ResetGlobalHistory() {
    mLastSpeakerID = 0;
}

void Manager::FlushSpeechForActor(EAXCharacter *) {}

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

GameSpeech::GameSpeech()
    : mLoadState(), //
      m_currentIntensity(0), //
      m_speechCycle(0), //
      m_pendingList(), //
      m_currEvent(0), //
      m_currEventTime(0), //
      m_currEventClarity(0), //
      m_currEventSpeakerID(0), //
      m_Chirper(0), //
      m_ChirpParams() {}

GameSpeech::~GameSpeech() {
    m_pendingList.clear();
    m_currEvent = 0;
    m_Chirper = 0;
}

void GameSpeech::Init(int channel) {
    m_channel = channel;
    m_currentIntensity = 0;
    m_speechCycle = 0;
    m_currEvent = 0;
    m_currEventTime = 0;
    m_currEventClarity = 0;
    m_currEventSpeakerID = 0;
    m_pendingList.clear();
    mLoadState.clear();
}

void GameSpeech::LoadingCallback(int, int) {}

void GameSpeech::LoadBanks() {}

int GameSpeech::TestSentenceRuleCallback(int, int, int) {
    return 0;
}

int GameSpeech::SetSentenceRuleCallback(int, int, int) {
    return 0;
}

SPCHType_EventRuleResult GameSpeech::EventRuleCallback(int) {
    return kSPCH_EventRule_OK;
}

void GameSpeech::Update() {
    CheckNextEvent();
    IssueSampleRequests();
    UpdateChirps();
    ClearCompletedRequests();
}

void GameSpeech::CheckNextEvent() {
    if (m_currEvent) {
        return;
    }
}

void GameSpeech::IssuePlayback(ScheduledSpeechEvent *nextevent) {
    m_currEvent = nextevent;
    if (nextevent && nextevent->actor) {
        m_currEventSpeakerID = static_cast<short>(nextevent->actor->GetSpeakerID());
    }
}

void GameSpeech::ClearCompletedRequests() {
    for (SpeechSampleVec::iterator it = m_pendingList.begin(); it != m_pendingList.end();) {
        if (*it && (*it)->ready) {
            it = m_pendingList.erase(it);
        } else {
            ++it;
        }
    }
}

void GameSpeech::ReleaseResource() {
    Module::ReleaseResource();
    m_pendingList.clear();
    m_currEvent = 0;
}

unsigned int GameSpeech::SampleRequestCallback(SPCHType_SampleRequestData *data) {
    SpeechSampleData *sample = ::gSpeechCache.GetSample(this, data);
    if (sample) {
        m_pendingList.push_back(sample);
        return 1;
    }
    return 0;
}

void GameSpeech::IssueSampleRequests() {
    for (SpeechSampleVec::iterator it = m_pendingList.begin(); it != m_pendingList.end(); ++it) {
        if (*it && !(*it)->ready) {
            (*it)->ready = true;
            (*it)->t_play = WorldTimer;
        }
    }
}

bool GameSpeech::ShouldPause() {
    return false;
}

void GameSpeech::RadioChirp(unsigned char type) {
    m_ChirpParams.ID = type;
}

void GameSpeech::UpdateChirps() {
    if (m_Chirper && !m_Chirper->IsPlaying()) {
        m_Chirper = 0;
    }
}

int GameSpeech::GetVolForSpeaker(int id) {
    int vol = 0x7fff - id * 64;
    if (vol < 0) {
        return 0;
    }
    return vol;
}

char *GameSpeech::GetCSIptr() {
    return m_csisData;
}

int GameSpeech::GetChannel() {
    return m_channel;
}

char *GameSpeech::GetEventDat() {
    return m_eventDat;
}

bool GameSpeech::IsDataLoaded() {
    return m_csisData != 0;
}

SED_NISSFX::SED_NISSFX()
    : mLoadState(), //
      m_currentIntensity(0), //
      m_moduleIsInitted(false), //
      m_speechCycle(0), //
      m_paused(false), //
      m_SyncObject(), //
      m_backupstrm(0) {}

SED_NISSFX::~SED_NISSFX() {
    ClearStream();
    m_moduleIsInitted = false;
}

void SED_NISSFX::Init(int channel) {
    m_channel = channel;
    m_moduleIsInitted = true;
    m_paused = false;
    m_speechCycle = 0;
    m_currentIntensity = 0;
}

void SED_NISSFX::LoadingCallback(int, int error_status) {
    m_dataIsLoaded = (error_status == 0);
}

void SED_NISSFX::LoadBanks() {}

int SED_NISSFX::TestSentenceRuleCallback(int, int, int) {
    return 0;
}

int SED_NISSFX::SetSentenceRuleCallback(int, int, int) {
    return 0;
}

SPCHType_EventRuleResult SED_NISSFX::EventRuleCallback(int) {
    return kSPCH_EventRule_OK;
}

bool SED_NISSFX::QueStream(eNISSFX_TYPE stream_type, void (*callback)(), bool) {
    m_SyncObject.id = stream_type;
    m_SyncObject.callback = callback;
    m_bIsStreamQueued = true;
    return true;
}

unsigned int SED_NISSFX::SampleRequestCallback(SPCHType_SampleRequestData *data) {
    SpeechSampleData *sample = ::gSpeechCache.GetSample(this, data);
    if (sample) {
        m_SyncObject.qsObject = sample;
        return 1;
    }
    return 0;
}

bool SED_NISSFX::PlayStream(int stream_id) {
    m_SyncObject.handle = stream_id;
    m_bIsStreamQueued = false;
    return true;
}

void SED_NISSFX::ClearStream() {
    m_SyncObject.qsObject = 0;
    m_SyncObject.callback = 0;
    m_SyncObject.id = STRM_NONE;
    m_SyncObject.handle = -1;
    m_SyncObject.holdtime = 0;
}

void SED_NISSFX::Update() {
    if (m_paused) {
        return;
    }

    if (m_bIsStreamQueued) {
        PlayStream(static_cast<int>(m_SyncObject.id));
    }

    if (m_SyncObject.callback && m_SyncObject.qsObject && m_SyncObject.qsObject->ready) {
        m_SyncObject.callback();
        m_SyncObject.callback = 0;
    }
}

} // namespace Speech
