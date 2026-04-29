#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"
#include "Speed/Indep/Src/EAXSound/Stream/GameSpeech.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/NISSFXModule.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/copspeech.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Speech/SpeechFlow.h"

struct CLUMP_ITEMtag {
    unsigned int key;
    unsigned int size;
    unsigned int header;
    unsigned int sample;
};

struct CLUMP_IDX_FILEtag {
    unsigned int numtypes;
    unsigned int numbanks[20];
    unsigned int count;
    CLUMP_ITEMtag item[1];
};

extern int SPCH_AddBank(char *bankHdr);
extern int SPCH_AddEventDB(char *event_dat, unsigned int channel);
extern int SPCH_GetBankPtrMemSize(int num_banks);
extern int SPCH_InitBankMem(int num_banks, char *bank_ptr_mem);
extern "C" bool InteruptedAndNotDelayed__6SpeechPQ26Speech20ScheduledSpeechEvent(Speech::ScheduledSpeechEvent *this_event);

namespace Speech {

Module *Manager::m_SpeechModule[NUM_SPEECH_MODULES] = { 0, 0 };
SPEECH_MODE Manager::m_speechMode = static_cast<SPEECH_MODE>(0);
int Manager::m_numberSpeechBanks = 0;
int Manager::m_SPEECH_initted = 0;
char *Manager::m_SPEECH_bankPtrMem = 0;
int Manager::m_speechDisable = 0;
int Manager::m_gameSpeechInitted = 0;
int Manager::m_NISAudioInitted = 0;
float Manager::m_clock_in_ms = 0.0f;
float Manager::m_timestep = 0.0f;
float Manager::m_deadair = 0.0f;
int Manager::mCurrentEvent = 0;
short Manager::m_frameindex = 0;
float Manager::mProbPlayback = 1.0f;
short Manager::mLastSpeakerID = 0;
SchedSpchEvents Manager::mEvents[4];
SPCHEventList Manager::mEvtHistory;
SpeechHashIDMap Manager::mHashMap;
EventHistory Manager::mGlobalHistory;
SampleReqList Manager::mSampleRequests;
Timer Manager::mSampleReqTimer;

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

namespace {
ScheduledSpeechEvent *sQueuedEvents[256] = { 0 };
int sQueuedEventCount = 0;

void CompactQueuedEvents() {
    int write = 0;
    for (int read = 0; read < sQueuedEventCount; ++read) {
        if (sQueuedEvents[read]) {
            sQueuedEvents[write] = sQueuedEvents[read];
            ++write;
        }
    }
    for (int clear = write; clear < sQueuedEventCount; ++clear) {
        sQueuedEvents[clear] = 0;
    }
    sQueuedEventCount = write;
}

int LoadSpeechBankImpl(CLUMP_IDX_FILEtag *index, int &type, int &number, ::SPEECH_BANK *sb) {
    unsigned int key = static_cast<unsigned int>(type) * 0x1000000 + static_cast<unsigned int>(number);
    int lower = 0;
    int upper = static_cast<int>(index->count) - 1;
    int i = upper >> 1;

    if (index->item[i].key != key) {
        bool done = (i == 0);
        int local_upper = i;
        while (!done && (local_upper != upper)) {
            if (index->item[local_upper].key < key) {
                lower = local_upper;
                local_upper = upper;
            }
            i = (lower + local_upper) >> 1;
            if (index->item[i].key == key) {
                break;
            }
            done = (i == lower);
            upper = local_upper;
            local_upper = i;
        }

        i = local_upper;
        if ((index->item[local_upper].key != key) && (index->item[lower].key != key) && (index->item[upper].key != key)) {
            sb->bank = -1;
            sb->offset = 0;
            sb->mem = 0;
            return -1;
        }
        if (index->item[lower].key == key) {
            i = lower;
        } else if (index->item[upper].key == key) {
            i = upper;
        }
    }

    if (index->item[i].size == 0) {
        sb->offset = 0;
        sb->bank = -1;
        sb->mem = 0;
        return -1;
    }

    sb->mem = reinterpret_cast<char *>(index->numbanks) + (index->item[i].header - 4);
    sb->bank = static_cast<int>(index->item[i].size);
    sb->offset = static_cast<int>(index->item[i].sample);
    return 0;
}

int AddHeadersImpl(char **dest, ::SPEECH_BANK *banks, int numBanks) {
    int size = 0;
    ::SPEECH_BANK *sb = banks;
    ::SPEECH_BANK *end = banks + numBanks;
    while (sb < end) {
        if (sb->mem) {
            size += sb->bank;
        }
        ++sb;
    }

    char *mem = gAudioMemoryManager.AllocateMemoryChar(size, "AUD:Relocated speech headers", false);
    *dest = mem;

    sb = banks;
    while (sb < end) {
        if (sb->mem) {
            bMemCpy(mem, sb->mem, sb->bank);
            sb->mem = mem;
            mem += sb->bank;
            sb->offset = SPCH_AddBank(sb->mem);
        }
        ++sb;
    }
    return 0;
}

} // namespace

SpeechHashIDMap::~SpeechHashIDMap() {}

EventHistory::~EventHistory() {}

SchedSpchEvents::~SchedSpchEvents() {}

SPCHEventList::~SPCHEventList() {}

SpchSampleMap::~SpchSampleMap() {}

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

int Manager::IndirectSpeechEvent(ScheduledSpeechEvent *evt, bool test_only) {
    if (!evt) {
        return -1;
    }

    if (PreValidate(*evt) != 0) {
        return 1;
    }

    if (!test_only) {
        if (sQueuedEventCount < 256) {
            sQueuedEvents[sQueuedEventCount] = evt;
            ++sQueuedEventCount;
        } else {
            return 2;
        }
    }

    return PostValidate(evt, 0U);
}

void Manager::ClearPlayback() {
    mSampleRequests.clear();
    for (int i = 0; i < sQueuedEventCount; ++i) {
        sQueuedEvents[i] = 0;
    }
    sQueuedEventCount = 0;
}

void Manager::Init(SPEECH_MODE mode) {
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

void Manager::Init2() {
    if (IsSoundEnabled == 0 || m_SPEECH_initted != 0) {
        return;
    }

    if (m_speechMode != SPEECH_FRONTEND_MODE) {
        if (m_speechMode == SPEECH_GAME_MODE) {
            if (IsSpeechEnabled && m_SpeechModule[COPSPEECH_MODULE] && m_SpeechModule[COPSPEECH_MODULE]->IsDataLoaded() && !m_gameSpeechInitted) {
                Module *cop_speech = m_SpeechModule[COPSPEECH_MODULE];
                m_numberSpeechBanks += cop_speech->GetNumBanks();
                Csis::System::Subscribe(cop_speech->GetCSIptr());
                Csis::CacheHandlesEvents();
                SPCH_AddEventDB(cop_speech->GetEventDat(), static_cast<unsigned int>(cop_speech->GetChannel()));
                m_gameSpeechInitted = 1;
            }

            if (IsNISAudioEnabled && m_SpeechModule[NISSFX_MODULE] && m_SpeechModule[NISSFX_MODULE]->IsDataLoaded() && !m_NISAudioInitted) {
                Module *nis_speech = m_SpeechModule[NISSFX_MODULE];
                m_numberSpeechBanks += nis_speech->GetNumBanks();
                Csis::System::Subscribe(nis_speech->GetCSIptr());
                Csis::CacheHandlesEventsNIS();
                SPCH_AddEventDB(nis_speech->GetEventDat(), static_cast<unsigned int>(nis_speech->GetChannel()));
                m_NISAudioInitted = 1;
            }

            if ((m_SpeechModule[COPSPEECH_MODULE] && !m_gameSpeechInitted) || (m_SpeechModule[NISSFX_MODULE] && !m_NISAudioInitted)) {
                return;
            }
        } else if (m_speechMode == SPEECH_SPLITSCREEN_MODE) {
            if (IsNISAudioEnabled && m_SpeechModule[NISSFX_MODULE] && m_SpeechModule[NISSFX_MODULE]->IsDataLoaded() && !m_NISAudioInitted) {
                Module *nis_speech = m_SpeechModule[NISSFX_MODULE];
                m_numberSpeechBanks += nis_speech->GetNumBanks();
                Csis::System::Subscribe(nis_speech->GetCSIptr());
                Csis::CacheHandlesEventsNIS();
                SPCH_AddEventDB(nis_speech->GetEventDat(), static_cast<unsigned int>(nis_speech->GetChannel()));
                m_NISAudioInitted = 1;
            }
        }
    }

    int buffSize = SPCH_GetBankPtrMemSize(m_numberSpeechBanks);
    m_SPEECH_bankPtrMem = gAudioMemoryManager.AllocateMemoryChar(buffSize, "AUD:Speech Bank Ptrs", false);
    SPCH_InitBankMem(m_numberSpeechBanks, m_SPEECH_bankPtrMem);

    if (m_speechMode == SPEECH_GAME_MODE) {
        if (IsSpeechEnabled && m_SpeechModule[COPSPEECH_MODULE]) {
            m_SpeechModule[COPSPEECH_MODULE]->LoadBanks();
        }
        if (IsNISAudioEnabled && m_SpeechModule[NISSFX_MODULE]) {
            m_SpeechModule[NISSFX_MODULE]->LoadBanks();
        }
    } else if (m_speechMode == SPEECH_SPLITSCREEN_MODE) {
        if (IsNISAudioEnabled && m_SpeechModule[NISSFX_MODULE]) {
            m_SpeechModule[NISSFX_MODULE]->LoadBanks();
        }
    }

    m_SPEECH_initted = 1;
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

    ServiceFilteredEvents();
    ServiceInterruptEvents();
    CalcProbPlayback();
}

void Manager::SpchLibAbort(const char *, ...) {
    ClearPlayback();
}

int Manager::SampleRequestCallback(SPCHType_SampleRequestData *data) {
    if (!data) {
        return 0;
    }

    int module_index = data->channel;
    if (module_index < 0 || module_index >= NUM_SPEECH_MODULES) {
        module_index = COPSPEECH_MODULE;
    }

    Module *module = m_SpeechModule[module_index];
    if (!module) {
        return 0;
    }

    return static_cast<int>(module->SampleRequestCallback(data));
}

int Manager::LoadSpeechBank(CLUMP_IDX_FILEtag *index, int &type, int &number, SPEECH_BANK *sb) {
    if (!index || !sb) {
        return 0;
    }
    return LoadSpeechBankImpl(index, type, number, sb);
}

int Manager::AddHeaders(char **dest, SPEECH_BANK *banks, int numBanks, Module *module) {
    if (!dest || !banks || !module || (numBanks < 0)) {
        return 0;
    }
    return AddHeadersImpl(dest, banks, numBanks);
}

int Manager::GetTicker() {
    return WorldTimer.GetPackedTime();
}

void Manager::PopulateHashMap() {
    for (int i = 0; i < sQueuedEventCount; ++i) {
        ScheduledSpeechEvent *evt = sQueuedEvents[i];
        if (!evt) {
            continue;
        }
        if (evt->priority > 0xFE) {
            evt->priority = 0xFE;
        }
    }
}

bool Manager::IsCacheable(SPCHType_1_EventID event_id) {
    return static_cast<int>(event_id) >= 0;
}

bool Manager::HasBeenSaid(SPCHType_1_EventID event_id) {
    for (int i = 0; i < sQueuedEventCount; ++i) {
        ScheduledSpeechEvent *evt = sQueuedEvents[i];
        if (evt && evt->ID == event_id) {
            return true;
        }
    }
    return false;
}

bool Manager::ServiceInterruptEvents() {
    bool changed = false;
    for (SPCHSampleRequest *it = mSampleRequests.begin(); it != mSampleRequests.end();) {
        if ((it->owner && InteruptedAndNotDelayed__6SpeechPQ26Speech20ScheduledSpeechEvent(it->owner)) || (it->data.interruptFlag != 0)) {
            it = mSampleRequests.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }
    return changed;
}

} // namespace Speech

namespace Speech {
struct SPEECH_BANK {
    char *mem;
    int bank;
    int offset;
};
}

extern "C" int LoadSpeechBank__Q26Speech7ManagerP17CLUMP_IDX_FILEtagRiT2PQ26Speech11SPEECH_BANK(
    CLUMP_IDX_FILEtag *index,
    int &type,
    int &number,
    Speech::SPEECH_BANK *sb) {
    return Speech::Manager::LoadSpeechBank(index, type, number, reinterpret_cast< ::SPEECH_BANK *>(sb));
}

extern "C" int AddHeaders__Q26Speech7ManagerPPcPQ26Speech11SPEECH_BANKiPQ26Speech6Module(
    char **dest,
    Speech::SPEECH_BANK *banks,
    int numBanks,
    Speech::Module *module) {
    return Speech::Manager::AddHeaders(dest, reinterpret_cast< ::SPEECH_BANK *>(banks), numBanks, module);
}

extern "C" bool InteruptedAndNotDelayed__6SpeechPQ26Speech20ScheduledSpeechEvent(Speech::ScheduledSpeechEvent *this_event) {
    Attrib::Gen::speech pb(static_cast<unsigned int>(this_event->ID), 0, 0);
    if (!pb.interrupt() || ((this_event->flags & 2U) != 0)) {
        return false;
    }
    this_event->flags = static_cast<short>(this_event->flags | 3);
    return true;
}

namespace Speech {

void Manager::ServiceFilteredEvents() {
    for (SPCHSampleRequest *it = mSampleRequests.begin(); it != mSampleRequests.end();) {
        SPCHType_1_EventID id = static_cast<SPCHType_1_EventID>(it->data.eventSpec.eventID);
        if (!IsCacheable(id)) {
            it = mSampleRequests.erase(it);
        } else {
            ++it;
        }
    }
}

bool Manager::RecallSpeechEvent(SPCHType_1_EventID recall_id) {
    for (int i = 0; i < sQueuedEventCount; ++i) {
        if (sQueuedEvents[i] && sQueuedEvents[i]->ID == recall_id) {
            return true;
        }
    }
    return false;
}

void Manager::Expire(ScheduledSpeechEvent *event) {
    if (!event) {
        return;
    }
    event->flags |= 1;
    event->finish_time = WorldTimer;
}

bool Manager::IsQueued(SPCHType_1_EventID evtID, int indices) {
    int found = 0;
    for (int i = 0; i < sQueuedEventCount; ++i) {
        if (sQueuedEvents[i] && sQueuedEvents[i]->ID == evtID) {
            ++found;
            if (found >= indices) {
                return true;
            }
        }
    }
    return false;
}

float Manager::IsEventDead(ScheduledSpeechEvent *evt) {
    if (!evt) {
        return 1.0f;
    }
    if ((evt->flags & 1) != 0) {
        return 1.0f;
    }
    return 0.0f;
}

void Manager::NotifyEventCompletion(ScheduledSpeechEvent *evt, bool playback_complete) {
    if (!evt) {
        return;
    }

    evt->finish_time = WorldTimer;
    if (playback_complete) {
        evt->flags |= 2;
    } else {
        evt->flags |= 1;
    }

    for (int i = 0; i < sQueuedEventCount; ++i) {
        if (sQueuedEvents[i] == evt) {
            sQueuedEvents[i] = 0;
        }
    }
    CompactQueuedEvents();
}

ScheduledSpeechEvent *Manager::GetNextEvent() {
    ScheduledSpeechEvent *best = 0;
    for (int i = 0; i < sQueuedEventCount; ++i) {
        ScheduledSpeechEvent *evt = sQueuedEvents[i];
        if (!evt) {
            continue;
        }
        if (!best || ScheduledSpeechEvent::sort_nested_priority(evt, best)) {
            best = evt;
        }
    }
    return best;
}

int Manager::PostValidate(ScheduledSpeechEvent *evt, unsigned int mask) {
    if (!evt) {
        return 1;
    }

    if ((evt->flags & static_cast<short>(mask)) != 0) {
        return 2;
    }

    if (IsEventDead(evt) > 0.0f) {
        return 3;
    }

    return 0;
}

int Manager::PreValidate(ScheduledSpeechEvent &evt) {
    if (!evt.actor) {
        return 1;
    }
    if (evt.ID == kSPCH1_EventID_MaxEventID) {
        return 2;
    }
    return 0;
}

bool Manager::CanPlayback(Attrib::Gen::speech &) {
    if (mProbPlayback <= 0.0f) {
        return false;
    }
    return true;
}

void Manager::CalcProbPlayback() {
    int queued = sQueuedEventCount;
    int requested = static_cast<int>(mSampleRequests.size());
    int total = queued + requested;
    if (total <= 0) {
        mProbPlayback = 1.0f;
    } else if (total > 12) {
        mProbPlayback = 0.25f;
    } else {
        mProbPlayback = 1.0f - static_cast<float>(total) * 0.05f;
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
    mProbPlayback = 1.0f;
}

void Manager::FlushSpeechForActor(EAXCharacter *actor) {
    if (!actor) {
        return;
    }

    for (int i = 0; i < sQueuedEventCount; ++i) {
        if (sQueuedEvents[i] && sQueuedEvents[i]->actor == actor) {
            sQueuedEvents[i] = 0;
        }
    }
    CompactQueuedEvents();
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
