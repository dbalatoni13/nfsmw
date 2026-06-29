#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"
#include "Speed/Indep/Src/EAXSound/Stream/GameSpeech.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/NISSFXModule.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/copspeech.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyCellCallComplete.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyCellCallStarted.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Speech/SpeechFlow.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

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

ScheduledSpeechEvent *Manager::ScheduleSpeechPartII(unsigned int size, void *data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) {
    unsigned int eventkey;
    SPCHType_1_EventID eventID;

    if ((IsSpeechEnabled == 0) || (m_speechMode == SPEECH_SPLITSCREEN_MODE) || (g_pEAXSound == 0) ||
        (g_pEAXSound->GetCurAudioSettings() == 0) || (g_pEAXSound->GetCurAudioSettings()->SpeechVol == 0.0f)) {
        eventkey = Attrib::StringToLowerCaseKey(iid.pString);
        eventID = mHashMap.GetID(eventkey);
        if (eventID == kSPCH1_EventID_CellCall) {
            MNotifyCellCallStarted msg;
            msg.Send(UCrc32(0x20d60dbf));
            MNotifyCellCallComplete msg2;
            msg2.Send(UCrc32(0x20d60dbf));
        }
    } else {
        SlotPool *pool = gSpeechCache.GetEventPool();
        if (pool->IsFull()) {
            return 0;
        }

        eventkey = Attrib::StringToLowerCaseKey(iid.pString);
        eventID = mHashMap.GetID(eventkey);
        Attrib::Gen::speech event_atr(eventkey, 0, 0);
        ScheduledSpeechEvent *event;
        Timer now = WorldTimer;

        for (int i = 0; i < 4; ++i) {
            for (SchedSpchEvents::iterator iter = mEvents[i].begin(); iter != mEvents[i].end(); ++iter) {
                ScheduledSpeechEvent *this_event = *iter;
                if (this_event->ID == eventID) {
                    bool obj_updated = false;
                    if (actor == 0) {
                        if (this_event->actor == 0) {
                            obj_updated = true;
                        }
                    } else if ((this_event->actor != 0) && (this_event->actor != actor)) {
                        obj_updated = true;
                    }

                    if (obj_updated) {
                        this_event->actor = actor;
                        this_event->entry_time = now;
                        if (size != 0) {
                            bMemCpy(this_event->GetData(0), data, size);
                        }
                    }
                    goto cleanup;
                }
            }
        }

        if (event_atr.DoNotDropout() || CanPlayback(event_atr)) {
            if (event_atr.Num_RecallList() != 0) {
                for (unsigned int i = 0; i < event_atr.Num_RecallList(); ++i) {
                    Attrib::Gen::speech recall(event_atr.RecallList(i).GetCollectionKey(), 0, 0);
                    SPCHType_1_EventID recall_id = recall.SpeechID();
                    if (IsQueued(recall_id, 4)) {
                        RecallSpeechEvent(recall_id);
                    }
                }
            }

            event = new (size) ScheduledSpeechEvent();
            if (event != 0) {
                goto have_event;
            }
        }
    cleanup:
        return 0;
    have_event:
        event->iid = &iid;
        event->fh = &fh;
        event->actor = actor;
        event->ID = eventID;
        if (!event_atr.interrupt()) {
            event->priority = static_cast<unsigned char>(event_atr.priority());
        } else {
            event->priority = static_cast<unsigned char>(event_atr.priority() + 100);
        }
        if (event_atr.InitDelay() > 0.0f) {
            event->flags |= 2;
        }
        mEvents[0].push_back(event);
        if (size != 0) {
            bMemCpy(event->GetData(0), data, size);
        }
        return event;
    }
    return 0;
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
    if (IsSoundEnabled == 0) {
        return;
    }

    switch (m_speechMode) {
        case SPEECH_GAME_MODE:
            if (IsSpeechEnabled && m_SpeechModule[COPSPEECH_MODULE]->IsDataLoaded() && !m_gameSpeechInitted) {
                m_numberSpeechBanks += m_SpeechModule[COPSPEECH_MODULE]->GetNumBanks();
                Csis::System::Subscribe(m_SpeechModule[COPSPEECH_MODULE]->GetCSIptr());
                Csis::CacheHandlesEvents();
                SPCH_AddEventDB(m_SpeechModule[COPSPEECH_MODULE]->GetEventDat(), static_cast<unsigned int>(m_SpeechModule[COPSPEECH_MODULE]->GetChannel()));
                m_gameSpeechInitted = 1;
            }

            if (IsNISAudioEnabled && m_SpeechModule[NISSFX_MODULE]->IsDataLoaded() && !m_NISAudioInitted) {
                m_numberSpeechBanks += m_SpeechModule[NISSFX_MODULE]->GetNumBanks();
                Csis::System::Subscribe(m_SpeechModule[NISSFX_MODULE]->GetCSIptr());
                Csis::CacheHandlesEventsNIS();
                SPCH_AddEventDB(m_SpeechModule[NISSFX_MODULE]->GetEventDat(), static_cast<unsigned int>(m_SpeechModule[NISSFX_MODULE]->GetChannel()));
                m_NISAudioInitted = 1;
            }

            if ((m_SpeechModule[COPSPEECH_MODULE] && !m_gameSpeechInitted) || (m_SpeechModule[NISSFX_MODULE] && !m_NISAudioInitted)) {
                return;
            }
            break;
        case SPEECH_SPLITSCREEN_MODE:
            if (IsNISAudioEnabled && m_SpeechModule[NISSFX_MODULE]->IsDataLoaded() && !m_NISAudioInitted) {
                m_numberSpeechBanks += m_SpeechModule[NISSFX_MODULE]->GetNumBanks();
                Csis::System::Subscribe(m_SpeechModule[NISSFX_MODULE]->GetCSIptr());
                Csis::CacheHandlesEventsNIS();
                SPCH_AddEventDB(m_SpeechModule[NISSFX_MODULE]->GetEventDat(), static_cast<unsigned int>(m_SpeechModule[NISSFX_MODULE]->GetChannel()));
                m_NISAudioInitted = 1;
            }
            break;
    }

    m_SPEECH_bankPtrMem = gAudioMemoryManager.AllocateMemoryChar(SPCH_GetBankPtrMemSize(m_numberSpeechBanks), "AUD:Speech Bank Ptrs", false);
    SPCH_InitBankMem(m_numberSpeechBanks, m_SPEECH_bankPtrMem);

    if (m_speechMode == SPEECH_GAME_MODE) {
        if (IsSpeechEnabled) {
            m_SpeechModule[COPSPEECH_MODULE]->LoadBanks();
        }
        if (IsNISAudioEnabled) {
            m_SpeechModule[NISSFX_MODULE]->LoadBanks();
        }
    } else if (m_speechMode == SPEECH_SPLITSCREEN_MODE) {
        if (IsNISAudioEnabled) {
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
    SchedSpchEvents deferredEvents;

    while (!mEvents[0].empty()) {
        SchedSpchEvents::iterator it = mEvents[0].begin();
        ScheduledSpeechEvent *this_event = *it;
        mEvents[0].erase(it);

        int keep = PreValidate(*this_event);
        if (keep == 0) {
            mEvents[1].push_back(this_event);
        } else if (keep == 1) {
            mEvents[3].push_back(this_event);
        } else if (keep == 4) {
            deferredEvents.push_back(this_event);
        } else {
            delete this_event;
        }
    }

    for (SchedSpchEvents::iterator i = deferredEvents.begin(); i != deferredEvents.end();) {
        ScheduledSpeechEvent *deferral = *i;
        if (PostValidate(deferral, 1U) != 0) {
            i = deferredEvents.erase(i);
            delete deferral;
            continue;
        }

        Attrib::Gen::speech deferral_atr(mHashMap.GetHash(deferral->ID), 0, 0);
        if (!deferral_atr.interrupt()) {
            mEvents[0].push_back(deferral);
        } else {
            mEvents[3].push_back(deferral);
        }

        i = deferredEvents.erase(i);
    }

    ServiceFilteredEvents();
    while (!ServiceInterruptEvents()) {
    }
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
    if (mEvents[3].size() != 0) {
        if (mEvents[3].size() > 1) {
            std::sort(mEvents[3].begin(), mEvents[3].end());
        }

        SchedSpchEvents::iterator priority_event_iter = mEvents[3].begin();
        ScheduledSpeechEvent *priority_event = *priority_event_iter;
        bool interruptable = true;
        GameSpeech *gamespeech = static_cast<GameSpeech *>(m_SpeechModule[COPSPEECH_MODULE]);
        ScheduledSpeechEvent *curr_event = gamespeech->m_currEvent;

        if (curr_event != 0) {
            Attrib::Gen::speech pb(mHashMap.GetHash(curr_event->ID), 0, 0);
            bool curr_is_interrupt = pb.interrupt();
            interruptable = pb.Interruptable();
            if (gamespeech->TestFlag(4)) {
                interruptable = true;
            }
            if (curr_is_interrupt && (priority_event->priority > curr_event->priority)) {
                interruptable = true;
            }
        }

        if (interruptable) {
            if (mEvents[1].size() != 0) {
                mEvents[0] = mEvents[1];
                mEvents[1].clear();
            }

            int rval = IndirectSpeechEvent(priority_event, false);
            if (rval == -2) {
                goto allocated_interrupt;
            }
            if (rval > -2) {
                goto queue_interrupt;
            }
            if (rval == -5) {
                delete priority_event;
                mEvents[3].erase(priority_event_iter);
                return false;
            }

        allocated_interrupt:
            if (PostValidate(priority_event, 1U) != kDitchEvt) {
                mEvents[0].insert(mEvents[0].begin(), priority_event);
                mEvents[3].erase(priority_event_iter);
                return true;
            }

        queue_interrupt:
            if (mEvents[2].size() != 0) {
                SchedSpchEvents::iterator i =
                    std::remove_if(mEvents[2].begin(), mEvents[2].end(), InteruptedAndNotDelayed__6SpeechPQ26Speech20ScheduledSpeechEvent);
                mEvents[2].erase(i, mEvents[2].end());
            }
            mEvents[2].insert(mEvents[2].begin(), priority_event);
        } else {
            mEvents[0] = mEvents[3];
        }

        mEvents[3].clear();
    }
    return true;
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

SpeechValRtnType Manager::PostValidate(ScheduledSpeechEvent *evt, unsigned int mask) {
    if (evt == 0) {
        return kKeepEvt;
    }

    const History *history = mGlobalHistory.Find(evt->ID);
    Attrib::Gen::speech event(mHashMap.GetHash(evt->ID), 0, 0);
    SoundAI *ai = SoundAI::Get();
    float elapsed;
    float expiry;
    float last_heard;
    short curr_heat;
    float pspeed;

    if (evt->flags & 2) {
        float delay = event.InitDelay();
        elapsed = (WorldTimer - evt->entry_time).GetSeconds();
        if (elapsed >= delay) {
            evt->entry_time = WorldTimer;
            evt->flags = static_cast<short>(evt->flags & ~2);
        } else {
            return kDitchEvt;
        }
    }

    elapsed = (WorldTimer - evt->entry_time).GetSeconds();
    expiry = event.expiry();
    if (elapsed >= expiry && (mask & 1)) {
        return kDitchEvt;
    }

    if (event.MaxPlayback() > -1 && (mask & 2)) {
        if (mGlobalHistory.GetCount(evt->ID) > event.MaxPlayback()) {
            return kDitchEvt;
        }
    }

    if (mask & 4) {
        if (event.DeadAir() > 0.0f && m_deadair < event.DeadAir()) {
            return kDeferEvt;
        }
    }

    if (history->count != 0 && (mask & 8)) {
        last_heard = (WorldTimer - history->time).GetSeconds();
        if (last_heard < event.Interval()) {
            return kDeferEvt;
        }
    }

    if (evt->actor != 0) {
        if (evt->actor->GetDistance() > event.CullingRange() && (mask & 0x10)) {
            return kDeferEvt;
        }
    }

    if (event.Num_DepFollow() != 0 && (mask & 0x20)) {
        bool pass = false;

        if (event.BackTime() > 0.0f) {
            unsigned int i;
            for (i = 0; i < event.Num_DepFollow(); ++i) {
                Attrib::Gen::speech dependency(event.DepFollow(i).GetCollectionKey(), 0, 0);
                Timer elapsed = WorldTimer - mGlobalHistory.GetTime(dependency.SpeechID());
                int count = mGlobalHistory.GetCount(dependency.SpeechID());
                bool dep_is_playing = IsCopSpeechPlaying(dependency.SpeechID());
                float t_elapsed = elapsed.GetSeconds();

                if ((t_elapsed < event.BackTime() && count > 0) || dep_is_playing) {
                    pass = true;
                    break;
                }
            }
        } else if (mEvtHistory.size() != 0) {
            SPCHType_1_EventID last_evt_id = mEvtHistory.front();
            unsigned int last_hist_key = mHashMap.GetHash(last_evt_id);
            unsigned int i;

            for (i = 0; i < event.Num_DepFollow(); ++i) {
                Attrib::Gen::speech dependency(event.DepFollow(i).GetCollectionKey(), 0, 0);
                bool dep_is_playing = IsCopSpeechPlaying(dependency.SpeechID());
                unsigned int dep_key = dependency.GetCollection();

                if (dep_key == last_hist_key || dep_is_playing) {
                    pass = true;
                    break;
                }
            }
        }

        if (!pass) {
            return kDeferEvt;
        }
    }

    if (event.OnScreenOnly() && (mask & 0x40) && evt->actor != 0) {
        IRenderable *render = 0;
        ISimable *simable = ISimable::FindInstance(evt->actor->GetHandle());

        if (simable != 0) {
            simable->QueryInterface(&render);
        }
        if (render != 0 && !render->InView()) {
            return kDeferEvt;
        }
    }

    if (event.reqLOS() && (mask & 0x80) && evt->actor != 0 && !evt->actor->HasLOS()) {
        return kDeferEvt;
    }

    curr_heat = static_cast<short>(ai->GetHeat());
    if (event.MaxHeat() < curr_heat || curr_heat < event.MinHeat()) {
        if (mask & 0x100) {
            return kDeferEvt;
        }
    }

    pspeed = bAbs(ai->GetPlayerSpeed());
    if (pspeed < event.MinPlayerSpeed() || event.MaxPlayerSpeed() < pspeed) {
        if (mask & 0x200) {
            return kDeferEvt;
        }
    }

    return kKeepEvt;
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
