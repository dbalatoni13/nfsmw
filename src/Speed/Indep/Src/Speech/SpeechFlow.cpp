#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_NISStream.hpp"
#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"
#include "Speed/Indep/Src/EAXSound/Stream/GameSpeech.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/NISSFXModule.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audiosystem.h"
#include "Speed/Indep/Src/EAXSound/snd_gen/copspeech.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/speech.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyCellCallComplete.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyCellCallStarted.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/Speech/SpeechFlow.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
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

enum CompressionType {
    kSPCH_Compression_None = 0,
    kSPCH_Compression_MicroTalk = 1,
    kSPCH_Compression_XA = 2,
};

struct SPCHType_ExtVecs {
    void (*spchAbortMessage)(const char *, ...);
    int (*spchPrint)(const char *, ...);
    int (*spchGetTick)();
};

extern int SPCH_AddBank(char *bankHdr);
extern int SPCH_AddEventDB(char *event_dat, unsigned int channel);
extern int SPCH_Choose(unsigned int inChannel);
extern int SPCH_GetBankPtrMemSize(int num_banks);
extern SPCHType_ExtVecs *SPCH_GetExtVecs();
extern int SPCH_GetSampleDataRate(int sampleRate, int sampleBits, CompressionType type);
extern int SPCH_InitBankMem(int num_banks, char *bank_ptr_mem);
extern int SPCH_Play(unsigned int inChannel);
extern void SPCH_Init(int (*request)(SPCHType_SampleRequestData *), unsigned int seed, int rate);
extern void SPCH_InitEventRuleCallback(SPCHType_EventRuleResult (*callback)(EventSpec *));
extern void SPCH_InitReparmCallback(int (*callback)(int, unsigned int *));
extern void SPCH_InitRuleCallbacks(int (*test)(EventSpec *, int, int, int), void (*set)(EventSpec *, int, int, int));
extern void SPCH_SetMemCallbacks(void *(*memAlloc)(unsigned int), void (*memFree)(void *));
extern void *bMalloc(int size, int allocation_params);
extern EAXS_StreamManager *gpEAXS_StrmMgr;
extern SlotPool *pCsisSlotPools[];
extern Timer RealTimer;
extern float TRACKSTREAMER_BACKLOG_THRESH;
extern int SPEECH_DISPLAY_HISTORY;
extern "C" bool InteruptedAndNotDelayed__6SpeechPQ26Speech20ScheduledSpeechEvent(Speech::ScheduledSpeechEvent *this_event);

static void *CSISAllocatorMemAlloc(unsigned int numBytes);
static void CSISAllocatorMemFree(void *memPtr);
static unsigned int gSpeechSeed;

static void *CSISAllocatorMemAlloc(unsigned int numBytes) {
    (void)numBytes;
    return bOMalloc(pCsisSlotPools[0]);
}

static void CSISAllocatorMemFree(void *memPtr) {
    bFree(pCsisSlotPools[0], memPtr);
}

namespace Speech {

inline SpeechSampleData *SpeechSampleVec::Find(int handle) const {
    for (SpeechSampleData *const *i = begin(); i != end(); ++i) {
        if ((*i)->HSTRM == handle) {
            return *i;
        }
    }

    return 0;
}

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
ScheduledSpeechEvent *Manager::mCurrentEvent = 0;
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

inline SchedSpchEvents::~SchedSpchEvents() {}

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
    if ((IsSpeechEnabled == 0) || (m_speechMode == SPEECH_SPLITSCREEN_MODE) || (evt == 0)) {
        return -2;
    }

    int result = Csis::Function::Call(evt->fh, evt->GetData(0));
    int rval;

    if (result < 0) {
        evt->fh->Set(evt->iid);
        result = Csis::Function::Call(evt->fh, evt->GetData(0));
    }

    mCurrentEvent = evt;
    rval = SPCH_Choose(static_cast<unsigned int>(m_SpeechModule[COPSPEECH_MODULE]->GetChannel()));

    if (rval != 0) {
        if (!test_only) {
            rval = SPCH_Play(static_cast<unsigned int>(m_SpeechModule[COPSPEECH_MODULE]->GetChannel()));
            evt->assoc_samples_count = static_cast<unsigned char>(rval);
            if (evt->assoc_samples_count == 0) {
                result = -2;
            }
        }
    } else {
        result = -5;
    }

    return result;
}

void Manager::ClearPlayback() {
    for (int ndx = 0; ndx < 4; ++ndx) {
        if (mEvents[ndx].size() != 0) {
            for (SchedSpchEvents::iterator i = mEvents[ndx].begin(); i != mEvents[ndx].end(); ++i) {
                ScheduledSpeechEvent *this_event = *i;
                if (this_event != 0) {
                    delete this_event;
                }
            }
            mEvents[ndx].clear();
        }
    }
    mSampleRequests.clear();
}

void Manager::Init(SPEECH_MODE mode) {
    if (IsSoundEnabled != 0) {
        mEvtHistory.clear();
        for (int ndx = 0; ndx < 4; ++ndx) {
            mEvents[ndx].clear();
        }

        mGlobalHistory.clear();
        mSampleRequests.reserve(20);
        mSampleRequests.clear();
        m_numberSpeechBanks = 0;
        m_speechMode = mode;

        int rate = SPCH_GetSampleDataRate(0x5622, 0x10, kSPCH_Compression_MicroTalk);
        SPCHType_ExtVecs *spchVecs = SPCH_GetExtVecs();

        if (gSpeechSeed == 0) {
            gSpeechSeed = g_pEAXSound->Random(-1);
        } else {
            ++gSpeechSeed;
        }

        SPCH_SetMemCallbacks(CSISAllocatorMemAlloc, CSISAllocatorMemFree);
        SPCH_Init(SampleRequestCallback, static_cast<unsigned int>(bAbs(static_cast<int>(gSpeechSeed))), rate);
        spchVecs->spchAbortMessage = SpchLibAbort;
        spchVecs->spchGetTick = GetTicker;
        SPCH_InitRuleCallbacks(TestSentenceRuleCallback, SetSentenceRuleCallback);
        SPCH_InitEventRuleCallback(EventRuleCallback);
        SPCH_InitReparmCallback(ReparmCallback);

        if (m_speechMode == SPEECH_FRONTEND_MODE) {
            goto init_done;
        }
        if (m_speechMode > SPEECH_FRONTEND_MODE) {
            goto check_splitscreen;
        }
        if (m_speechMode == SPEECH_GAME_MODE) {
            goto game_mode;
        }
        goto init_done;

    check_splitscreen:
        if (m_speechMode == SPEECH_SPLITSCREEN_MODE) {
            goto splitscreen_mode;
        }
        goto init_done;

    game_mode:
        m_gameSpeechInitted = 0;
        m_NISAudioInitted = 0;
        if (IsSpeechEnabled) {
            m_SpeechModule[COPSPEECH_MODULE] = new ("AUD:GameSpeech") GameSpeech();
            m_SpeechModule[COPSPEECH_MODULE]->Init(SPEECH_NONE_MODE);
        }
        if (IsNISAudioEnabled) {
            m_SpeechModule[NISSFX_MODULE] = new ("AUD:NISSFXModule") SED_NISSFX();
            m_SpeechModule[NISSFX_MODULE]->Init(SPEECH_FRONTEND_MODE);
        }
        goto init_done;

    splitscreen_mode:
        m_NISAudioInitted = 0;
        m_gameSpeechInitted = 0;
        if (IsNISAudioEnabled) {
            m_SpeechModule[NISSFX_MODULE] = new ("AUD:NISSFXModule") SED_NISSFX();
            m_SpeechModule[NISSFX_MODULE]->Init(SPEECH_FRONTEND_MODE);
        }

    init_done:
        PopulateHashMap();
        mGlobalHistory.Init();
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
    if (m_speechMode == SPEECH_FRONTEND_MODE) {
        goto cleanup;
    }
    if (m_speechMode > SPEECH_FRONTEND_MODE) {
        goto check_split_destroy;
    }
    if (m_speechMode == SPEECH_GAME_MODE) {
        goto game_destroy;
    }
    goto cleanup;

check_split_destroy:
    if (m_speechMode == SPEECH_SPLITSCREEN_MODE) {
        goto split_destroy;
    }
    goto cleanup;

game_destroy:
    if (m_SpeechModule[COPSPEECH_MODULE]) {
        delete m_SpeechModule[COPSPEECH_MODULE];
    }
    m_SpeechModule[COPSPEECH_MODULE] = 0;
    if (m_SpeechModule[NISSFX_MODULE]) {
        delete m_SpeechModule[NISSFX_MODULE];
        m_SpeechModule[NISSFX_MODULE] = 0;
    }
    goto cleanup;

split_destroy:
    if (m_SpeechModule[NISSFX_MODULE]) {
        delete m_SpeechModule[NISSFX_MODULE];
        m_SpeechModule[NISSFX_MODULE] = 0;
    }

cleanup:
    if (m_SPEECH_bankPtrMem) {
        gAudioMemoryManager.FreeMemory(m_SPEECH_bankPtrMem);
        m_SPEECH_bankPtrMem = 0;
    }
    ClearPlayback();
    mEvtHistory.clear();
    mGlobalHistory.clear();
    mSampleRequests.clear();
}

void Manager::Deduce() {
    SchedSpchEvents deferredEvents;
    bool service;

    if (mEvents[0].size() != 0) {
        for (SchedSpchEvents::iterator i = mEvents[0].begin(); i != mEvents[0].end(); i = mEvents[0].begin()) {
            ScheduledSpeechEvent *this_event = *i;
            SpeechValRtnType keep = static_cast<SpeechValRtnType>(PreValidate(*this_event));

            switch (keep) {
                case kIntEvt:
                    mEvents[3].push_back(this_event);
                    break;
                case kKeepEvt:
                    mEvents[1].push_back(this_event);
                    break;
                case kDeferEvt:
                    deferredEvents.push_back(this_event);
                    break;
                case kDitchEvt:
                case kEvtNotFound:
                default:
                    delete this_event;
                    this_event = 0;
                    break;
            }

            mEvents[0].erase(i);
        }
        mEvents[0].clear();
    }

    if (deferredEvents.size() != 0) {
        SchedSpchEvents::iterator i = deferredEvents.begin();
        while (i != deferredEvents.end()) {
            ScheduledSpeechEvent *deferral = *i;
            if (PostValidate(deferral, 1U) == kDitchEvt) {
                i = deferredEvents.erase(i);
                delete deferral;
                deferral = 0;
            } else {
                Attrib::Gen::speech deferral_atr(mHashMap.GetHash(deferral->ID), 0, 0);
                if (!deferral_atr.interrupt()) {
                    mEvents[0].insert(mEvents[0].begin(), deferral);
                } else {
                    mEvents[3].push_back(deferral);
                }
                ++i;
            }
        }

        deferredEvents.clear();
    }

    ServiceFilteredEvents();
    do {
        service = ServiceInterruptEvents();
    } while (!service);
}

void Manager::Update(float t) {
    static unsigned int max_samplerequests;

    m_timestep = t;
    m_frameindex = 0;

    if (m_speechMode == SPEECH_FRONTEND_MODE) {
        goto done;
    }

    if (m_speechMode < SPEECH_SPLITSCREEN_MODE) {
        if (m_speechMode == SPEECH_GAME_MODE) {
            goto game_mode;
        }
        goto done;
    } else {
        if (m_speechMode == SPEECH_SPLITSCREEN_MODE && m_SpeechModule[NISSFX_MODULE] && IsNISAudioEnabled) {
            m_SpeechModule[NISSFX_MODULE]->Update();
        }
        goto done;
    }

game_mode:
    {
        if (m_SpeechModule[NISSFX_MODULE] && IsNISAudioEnabled) {
            m_SpeechModule[NISSFX_MODULE]->Update();
        }

        if (m_SpeechModule[COPSPEECH_MODULE] && IsSpeechEnabled) {
            m_deadair = g_EAXIsPaused() ? 65535.0f : (WorldTimer - GetTimeSinceLastEvent(COPSPEECH_MODULE)).GetSeconds();

            CalcProbPlayback();

            bool track_streamer_busy = TheTrackStreamer.GetLoadingBacklog() >= TRACKSTREAMER_BACKLOG_THRESH;
            if (mSampleRequests.size() != 0 && !track_streamer_busy) {
                for (int ndx = 0; ndx < 4; ++ndx) {
                    for (SchedSpchEvents::iterator i = mEvents[ndx].begin(); i != mEvents[ndx].end(); ++i) {
                        ScheduledSpeechEvent *this_event = *i;
                        if ((this_event->flags & 1) != 0) {
                            this_event->flags = static_cast<short>(this_event->flags & ~1);
                        }
                    }
                }

                if (mSampleRequests.size() > max_samplerequests) {
                    max_samplerequests = mSampleRequests.size();
                }

                m_SpeechModule[COPSPEECH_MODULE]->SampleRequestCallback(0);
            }

            m_SpeechModule[COPSPEECH_MODULE]->Update();

            if (SPEECH_DISPLAY_HISTORY != 0) {
                SPCHEventList::iterator i = mEvtHistory.begin();
                int y;
                int x;
                for (; i != mEvtHistory.end(); i++) {
                    SPCHType_1_EventID event = *i;
                    History *hist = mGlobalHistory.Find(event);
                    if (hist != 0) {
                        Attrib::Gen::speech speech(mHashMap.GetHash(event), 0, 0);
                        speech.CollectionName();
                    }
                }
            }
        }
    }

done:
    ;
}

void Manager::SpchLibAbort(const char *, ...) {
    ClearPlayback();
}

int Manager::SampleRequestCallback(SPCHType_SampleRequestData *data) {
    if (!m_speechDisable) {
        if (data->channel == 1) {
            goto done;
        }
        if (data->channel > 1) {
            goto check_nis_channel;
        }
        if (data->channel == 0) {
            goto cop_channel;
        }
        goto done;

    check_nis_channel:
        if (data->channel == 2) {
            goto nis_callback;
        }
        goto done;

    cop_channel:
        if (mSampleRequests.begin() == mSampleRequests.end()) {
            mSampleReqTimer = RealTimer;
        }

        SPCHType_1_EventID id = static_cast<SPCHType_1_EventID>(data->eventSpec.eventID);
        if (TheTrackStreamer.GetLoadingBacklog() >= TRACKSTREAMER_BACKLOG_THRESH) {
            mCurrentEvent->flags = static_cast<short>(mCurrentEvent->flags | 1);
        }

        if (gSpeechCache.IsCached(data, false)) {
            if (SpeechSampleData *sample = gSpeechCache.GetSample(m_SpeechModule[COPSPEECH_MODULE], data)) {
                mCurrentEvent->AddSample(sample, 0xff);
            }
        } else {
            if (IsCacheable(id)) {
                SPCHSampleRequest new_req;
                new_req.data = *data;
                new_req.offset = m_SpeechModule[COPSPEECH_MODULE]->GetBankOffset(data->bankNum) + data->sampleOffset;
                new_req.owner = mCurrentEvent;
                new_req.sample_index = mCurrentEvent->ReserveSample();
                mSampleRequests.push_back(new_req);
            } else {
                SpeechSampleData *sample = gSpeechCache.GetUncached(m_SpeechModule[COPSPEECH_MODULE], data);
                mCurrentEvent->AddSample(sample, 0xff);
            }
        }
    }

done:
    return 0;

nis_callback:
    return static_cast<int>(m_SpeechModule[NISSFX_MODULE]->SampleRequestCallback(data));
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
    unsigned int key = static_cast<unsigned int>(type) * 0x1000000 + static_cast<unsigned int>(number);
    int lower = 0;
    int upper = static_cast<int>(index->count) - 1;
    int i = upper >> 1;

    if (index->item[i].key != key) {
        while ((i != lower) && (i != upper)) {
            if (index->item[i].key < key) {
                lower = i;
            } else {
                upper = i;
            }
            i = (lower + upper) >> 1;
            if (index->item[i].key == key) {
                goto found;
            }
        }

        if (index->item[i].key == key) {
            goto found;
        }
        if (index->item[lower].key == key) {
            i = lower;
            goto found;
        } else if (index->item[upper].key == key) {
            i = upper;
            goto found;
        }
        sb->mem = 0;
        sb->offset = 0;
        sb->bank = -1;
        return -1;
    }

found:
    if (index->item[i].size == 0) {
        sb->mem = 0;
        sb->bank = -1;
        sb->offset = 0;
        return -1;
    }

    sb->mem = reinterpret_cast<char *>(index->numbanks) + (index->item[i].header - 4);
    sb->bank = static_cast<int>(index->item[i].size);
    sb->offset = static_cast<int>(index->item[i].sample);
    return 0;
}

extern "C" int AddHeaders__Q26Speech7ManagerPPcPQ26Speech11SPEECH_BANKiPQ26Speech6Module(
    char **dest,
    Speech::SPEECH_BANK *banks,
    int numBanks,
    Speech::Module *module) {
    int size = 0;
    Speech::SPEECH_BANK *sb = banks;
    {
        Speech::SPEECH_BANK *end = banks + numBanks;
        while (sb < end) {
            if (sb->mem) {
                size += sb->bank;
            }
            ++sb;
        }
    }

    char *mem = gAudioMemoryManager.AllocateMemoryChar(size, "AUD:Relocated speech headers", false);
    *dest = mem;

    {
        Speech::SPEECH_BANK *end = banks + numBanks;
        sb = banks;
        while (sb < end) {
            if (sb->mem) {
                bMemCpy(mem, sb->mem, sb->bank);
                sb->mem = mem;
                mem += sb->bank;
                sb->bank = SPCH_AddBank(sb->mem);
            }
            ++sb;
        }
    }
    return 0;
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
    if (mEvents[1].size() != 0) {
        std::sort(mEvents[1].begin(), mEvents[1].end());

        SchedSpchEvents::iterator i = mEvents[1].begin();
        while (i != mEvents[1].end()) {
            bool trimmed = false;
            ScheduledSpeechEvent *this_event = *i;

            if (this_event->assoc_samples_count == 0) {
                int rval = IndirectSpeechEvent(this_event, false);
                if (rval == -2) {
                    goto validate_event;
                }
                if (rval > -2) {
                    goto defer_event;
                }
                if (rval != -5) {
                    goto defer_event;
                }
                delete this_event;
                i = mEvents[1].erase(i);
                trimmed = true;
                goto next_event;

            validate_event:
                if (PostValidate(this_event, 1U) != kDitchEvt) {
                    mEvents[0].insert(mEvents[0].begin(), this_event);
                }
                goto next_event;

            defer_event:
                mEvents[2].push_back(this_event);
            }

        next_event:
            if (!trimmed) {
                ++i;
            }
        }

        mEvents[1].clear();
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

    SoundAI *ai;
    unsigned short speakerID;

    if (evt->actor != 0) {
        speakerID = static_cast<unsigned short>(evt->actor->GetSpeakerID());
    } else {
        speakerID = 0;
    }

    if (speakerID > 1) {
        mLastSpeakerID = speakerID;
    }

    if (!playback_complete) {
        History *hist = mGlobalHistory.Touch(evt->ID, speakerID);
        mEvtHistory.push_front(evt->ID);
        if (mEvtHistory.size() > 10) {
            mEvtHistory.pop_back();
        }
    }

    if (!playback_complete) {
        return;
    }

    MNotifySpeechStatus msg(evt);
    msg.Send(UCrc32(0x20d60dbf));

    if (evt->ID == kSPCH1_EventID_CellCall) {
        MNotifyCellCallComplete msg2;
        msg2.Send(UCrc32(0x20d60dbf));
    }
}

ScheduledSpeechEvent *Manager::GetNextEvent() {
    if (mEvents[2].size() != 0) {
        SchedSpchEvents::iterator iter;

        if (mEvents[2].size() > 1) {
            bool requires_sort = true;
            SchedSpchEvents::iterator start = mEvents[2].begin();
            ScheduledSpeechEvent *head = *start;
            while (start != mEvents[2].end()) {
                if (head->priority <= 100) {
                    break;
                }
                ++start;
                if (start == mEvents[2].end()) {
                    requires_sort = false;
                }
            }
            if (requires_sort) {
                std::sort(start, mEvents[2].end(), ScheduledSpeechEvent::sort_nested_priority);
            }
        }

        iter = mEvents[2].begin();
        do {
            if (iter == mEvents[2].end()) {
                return 0;
            }

            ScheduledSpeechEvent *next = *iter;
            if (next->assoc_samples_prep == 0) {
                short prepared_count = 0;
                for (short i = 0; i < 7; ++i) {
                    SpeechSampleData *stitch = next->assoc_samples[i];
                    if (stitch != 0 && stitch->ready) {
                        ++prepared_count;
                    }
                }

                if (prepared_count == next->assoc_samples_count) {
                    next->assoc_samples_prep = 1;
                    if ((next->flags & 2U) == 0) {
                        next->entry_time = WorldTimer;
                    }
                } else {
                    SpeechValRtnType keep = PostValidate(next, 1U);
                    if (next->assoc_samples_count == 0 || keep == kDitchEvt) {
                        goto remove_event;
                    }

                    {
                        Attrib::Gen::speech event(mHashMap.GetHash(next->ID), 0, 0);
                        if (event.interrupt()) {
                            return 0;
                        }
                    }
                }
            }

        check_prepared:
            if (next->assoc_samples_prep != 0) {
                SpeechValRtnType keep = PostValidate(next, 0xffffffff);
                if (keep != kDitchEvt) {
                    goto check_keep;
                }
                goto remove_event;

            check_keep:
                if (keep == kDeferEvt) {
                    goto defer_next;
                }
                if (keep == kKeepEvt) {
                    return next;
                }
                goto next_loop;
            }
            goto return_null;

        remove_event:
            mEvents[2].erase(iter);
            delete next;
            return 0;

        defer_next:
            if (iter == mEvents[2].end()) {
                return 0;
            }
            ++iter;

        next_loop:
            ;
        } while (true);
    }

return_null:
    return 0;
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

SpeechValRtnType Manager::PreValidate(ScheduledSpeechEvent &evt) {
    if (evt.flags & 1) {
        return kDeferEvt;
    }

    Attrib::Gen::speech event(mHashMap.GetHash(evt.ID), 0, 0);
    SpeechValRtnType rval = kKeepEvt;
    bool is_int = false;

    if (evt.flags & 2) {
        float delay = event.InitDelay();
        float elapsed = (WorldTimer - evt.entry_time).GetSeconds();
        if (elapsed < delay) {
            return kDeferEvt;
        }

        evt.entry_time = WorldTimer;
        evt.flags = static_cast<short>(evt.flags & ~2);
    }

    is_int = event.interrupt();
    if (is_int) {
        rval = PostValidate(&evt, static_cast<unsigned int>(-1));
        if (rval == kKeepEvt) {
            return kIntEvt;
        }
        return rval;
    }

    return kKeepEvt;
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
    mGlobalHistory.Reset();
    mEvtHistory.clear();
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
    ClearCompletedRequests();
    if (m_strm != 0) {
        m_strm->Stop();
    }
    if (m_bankHeaders != 0) {
        gAudioMemoryManager.FreeMemory(m_bankHeaders);
        m_bankHeaders = 0;
    }
    if (m_eventDat != 0) {
        gAudioMemoryManager.FreeMemory(m_eventDat);
        m_eventDat = 0;
    }
    if (m_speechBanks != 0) {
        gAudioMemoryManager.FreeMemory(m_speechBanks);
        m_numBanks = 0;
        m_speechBanks = 0;
    }
    if (m_csisData != 0) {
        Csis::System::Unsubscribe(m_csisData);
        gAudioMemoryManager.FreeMemory(m_csisData);
        m_csisData = 0;
    }
    mLoadState.clear();
    if (m_strm != 0) {
        gpEAXS_StrmMgr->RemoveStreamChannel(STYPE_COPSPEECH);
        m_strm = 0;
    }
    m_pendingList.clear();
    m_pSFXOBJ_Speech = 0;
    if (m_Chirper != 0) {
        delete m_Chirper;
    }
    m_Chirper = 0;
}

void GameSpeech::Init(int channel) {
    m_channel = channel;

    Attrib::Gen::audiosystem *localizedCollection = new Attrib::Gen::audiosystem(g_pEAXSound->GetLocalAttr());
    if (localizedCollection->GetLayoutPointer() == 0) {
        localizedCollection->SetDefaultLayout(sizeof(Attrib::Gen::audiosystem::_LayoutStruct));
    }

    m_strm = gpEAXS_StrmMgr->GetStreamChannel(0);
    m_filename = localizedCollection->BIGPath();

    Attrib::StringKey evtfile(localizedCollection->EVTPath());
    Attrib::StringKey csifile(localizedCollection->CSIPath());
    Attrib::StringKey idxfile(localizedCollection->IDXPath());

    int nfilesize;
    {
        const char *filename = evtfile.GetString();
        if (filename == 0) {
            filename = "";
        }
        nfilesize = bFileSize(filename);
    }

    char **eventDat = &m_eventDat;
    if (nfilesize > 0) {
        *eventDat = gAudioMemoryManager.AllocateMemoryChar(nfilesize, "AUD: Game speech events", false);
    } else {
        *eventDat = 0;
    }

    if (m_eventDat != 0) {
        int thisobj = 0;
        mLoadState.push_back(thisobj);
        const char *filename = evtfile.GetString();
        if (filename == 0) {
            filename = "";
        }
        AddQueuedFile(m_eventDat, filename, 0, nfilesize, LoadingCallback, reinterpret_cast<int>(this), 0);
    }

    {
        const char *filename = idxfile.GetString();
        if (filename == 0) {
            filename = "";
        }
        nfilesize = bFileSize(filename);
    }

    char **tempCharPtr = &m_tempCharPtr;
    if (nfilesize > 0) {
        *tempCharPtr = static_cast<char *>(bMalloc(nfilesize, 0x1040));
    } else {
        *tempCharPtr = 0;
    }

    if (m_tempCharPtr != 0) {
        int thisobj = 1;
        mLoadState.push_back(thisobj);
        const char *filename = idxfile.GetString();
        if (filename == 0) {
            filename = "";
        }
        AddQueuedFile(m_tempCharPtr, filename, 0, nfilesize, LoadingCallback, reinterpret_cast<int>(this), 0);
    }

    {
        const char *filename = csifile.GetString();
        if (filename == 0) {
            filename = "";
        }
        nfilesize = bFileSize(filename);
    }

    char **csisData = &m_csisData;
    if (nfilesize > 0) {
        *csisData = gAudioMemoryManager.AllocateMemoryChar(nfilesize, "AUD: Game speech CSIS data", false);
    } else {
        *csisData = 0;
    }

    if (m_csisData != 0) {
        int thisobj = 2;
        mLoadState.push_back(thisobj);
        const char *filename = csifile.GetString();
        if (filename == 0) {
            filename = "";
        }
        AddQueuedFile(m_csisData, filename, 0, nfilesize, LoadingCallback, reinterpret_cast<int>(this), 0);
    }
}

void GameSpeech::LoadingCallback(int param, int) {
    GameSpeech *obj = reinterpret_cast<GameSpeech *>(param);

    if (!obj->mLoadState.empty()) {
        int loadstate = obj->mLoadState.front();
        switch (loadstate) {
        case 0:
            Manager::GetSpeechModule(COPSPEECH_MODULE)->SetFlag(0x80);
            break;
        case 1: {
            m_clumpIdx = reinterpret_cast<CLUMP_IDX_FILEtag *>(m_tempCharPtr);
            int numbanktypes = m_clumpIdx->numtypes;
            for (int i = 0; i < numbanktypes; i++) {
                obj->m_numBanks += m_clumpIdx->numbanks[i];
            }
            Manager::GetSpeechModule(COPSPEECH_MODULE)->SetFlag(0x100);
            break;
        }
        case 2:
            Manager::GetSpeechModule(COPSPEECH_MODULE)->SetFlag(0x40);
            break;
        default:
            break;
        }
        obj->mLoadState.pop_front();
    }

    if (Manager::GetSpeechModule(COPSPEECH_MODULE)->TestFlag(0x40) && Manager::GetSpeechModule(COPSPEECH_MODULE)->TestFlag(0x100) &&
        Manager::GetSpeechModule(COPSPEECH_MODULE)->TestFlag(0x80)) {
        Manager::GetSpeechModule(COPSPEECH_MODULE)->SetFlag(1);
        Manager::Init2();
    }
}

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

bool Unlocked(SpeechSampleData *data) {
    if (!data->cached) {
        gSpeechCache.TossSample(data);
        return true;
    }

    return data->lock != 0;
}

void GameSpeech::Update() {
    if (TestFlag(2)) {
        return;
    }

    UpdateChirps();
    if (!m_strm || !IsSpeechEnabled || !m_pSFXOBJ_Speech) {
        return;
    }

    int LowPassFilter = m_pSFXOBJ_Speech->GetDMixOutput(0xC, DMX_FREQ);
    m_strm->SetLowPass(LowPassFilter);

    SNDSTREAMSTATUS ss;
    m_strm->GetStatus(&ss);

    static int requests_in_queue = ss.outstandingrequests;
    static int req_timer = 0;

    if (ss.outstandingrequests > 0) {
        float fvol;
        SNDREQUESTSTATUS srs;
        m_flags |= 8;
        requests_in_queue = ss.outstandingrequests;
        int err = m_strm->GetRequestStatus(ss.currentrequest, &srs);
        if (err >= 0) {
            SpeechSampleData *sample;
            if (m_pendingList.size() != 0) {
                sample = m_pendingList.Find(ss.currentrequest);
            } else {
                sample = 0;
            }
            switch (srs.state) {
                case 3:
                    if (sample) {
                        sample->t_play = WorldTimer;
                        sample->lock = false;
                    }
                    req_timer = 0;
                    break;
                case 1:
                case 0: {
                    int nQVol;
                    if (TestFlag(0x20)) {
                        nQVol = 0x7FFF;
                    } else {
                        nQVol = 0;
                    }
                    m_pSFXOBJ_Speech->SetDMIX_Input(2, nQVol);
                    break;
                }
                case 2: {
                    m_currEventTime += srs.currenttime - req_timer;
                    req_timer = srs.currenttime;
                    if ((m_currEventTime > 3000) && !TestFlag(0x10)) {
                        Manager::NotifyEventCompletion(m_currEvent, false);
                        m_flags |= 0x10;
                    }

                    if (TestFlag(0x20)) {
                        m_pSFXOBJ_Speech->SetDMIX_Input(2, 0x7FFF);
                        m_strm->SetAz(m_pSFXOBJ_Speech->GetDMixOutput(0, DMX_AZIM));
                    } else {
                        m_pSFXOBJ_Speech->SetDMIX_Input(2, 0);
                        m_strm->SetAz(0);
                    }

                    int nQVol = GetVolForSpeaker(m_currEventSpeakerID);
                    m_pSFXOBJ_Speech->SetDMIX_Input(3, m_currEventClarity);
                    SndStrmWrapper *strm = m_strm;
                    fvol = static_cast<float>(nQVol);
                    fvol *= 0.01f;
                    fvol *= 32767.0f;
                    if (m_enable) {
                        nQVol = static_cast<int>(fvol);
                    } else {
                        nQVol = 0;
                    }
                    strm->SetVol(nQVol, false);
                    CheckNextEvent();
                    break;
                }
            }
        }

        if (m_pendingList.size() != 0) {
            SpeechSampleData **i = std::remove_if(m_pendingList.begin(), m_pendingList.end(), Unlocked);
            m_pendingList.erase(i, m_pendingList.end());
        }
        mLastEventTimestamp = WorldTimer;
    } else if (ss.outstandingrequests == 0) {
        if (requests_in_queue > 0) {
            if (m_currEvent) {
                if (m_currEvent->finish_time == Timer(0)) {
                    m_currEvent->finish_time = WorldTimer;
                    if (!TestFlag(0x10)) {
                        Manager::NotifyEventCompletion(m_currEvent, false);
                        m_flags |= 0x10;
                    }
                    Manager::NotifyEventCompletion(m_currEvent, true);
                    RadioChirp(1);
                }

                if (Manager::IsEventDead(m_currEvent) < 0.0f) {
                    requests_in_queue = ss.outstandingrequests;
                    mLastEventTimestamp = WorldTimer;
                    if (!m_currEvent) {
                        return;
                    }
                    delete m_currEvent;
                    m_currEvent = 0;
                    for (unsigned int i = 0; i < m_pendingList.size(); ++i) {
                        m_pendingList[i]->lock = false;
                    }
                    m_pendingList.clear();
                    m_flags &= ~0xC;
                } else {
                    m_flags |= 0xC;
                    CheckNextEvent();
                    return;
                }
            } else {
                unsigned int flags = m_flags;
                requests_in_queue = 0;
                m_flags = flags & ~0xC;
            }
        } else {
            CheckNextEvent();
        }
    }
}

void GameSpeech::CheckNextEvent() {
    if (m_currEvent) {
        return;
    }
}

void GameSpeech::IssuePlayback(ScheduledSpeechEvent *nextevent) {
    Attrib::Gen::speech speech(Manager::mHashMap.GetHash(nextevent->ID), 0, 0);

    if (m_pSFXOBJ_Speech) {
        float scale = static_cast<unsigned int>(speech.Clarity());
        scale *= 0.01f;
        scale *= 32767.0f;
        m_currEventClarity = static_cast<int>(scale);
    }

    {
        MNotifySpeechStatus(nextevent).Send(UCrc32("SpeechStatus"));
    }

    if (nextevent && (nextevent->ID == kSPCH1_EventID_CellCall)) {
        MNotifyCellCallStarted().Post(UCrc32(0x20d60dbf));
    }

    if (speech.Pan()) {
        m_flags |= 0x20;
    } else {
        m_flags &= ~0x20;
    }

    if (speech.RadioChirp()) {
        m_flags |= 0x400;
    } else {
        m_flags &= ~0x400;
    }

    unsigned char err_count = 0;
    for (short i = 0; i < nextevent->assoc_samples_count; ++i) {
        SpeechSampleData *stitch = nextevent->assoc_samples[i];
        if (stitch) {
            bool stitch_hdr_check_passes = false;
            const unsigned int snd_chunk_hdr = 0x5343486c;
            if ((stitch->cached == false) || (*reinterpret_cast<const unsigned int *>(stitch->GetData()) == snd_chunk_hdr)) {
                stitch_hdr_check_passes = true;
            } else {
                err_count++;
            }

            if (stitch_hdr_check_passes) {
                stitch->t_load = WorldTimer;
                stitch->lock = true;
                m_pendingList.push_back(stitch);
                if (stitch->cached) {
                    stitch->HSTRM = m_strm->AddToStream(0, stitch->GetData(), 0, 0);
                } else {
                    stitch->HSTRM = m_strm->AddToStrmReq(GetFilename(), stitch->dataoffset, 0);
                }
                m_flags |= 8;
            }
        } else {
            err_count++;
        }
    }

    m_currEventTime = 0;
    m_flags &= ~0x10;

    if (err_count) {
        {
            MNotifySpeechStatus(nextevent).Send(UCrc32(0x20d60dbf));
        }
        if (nextevent->ID == kSPCH1_EventID_CellCall) {
            MNotifyCellCallComplete().Send(UCrc32(0x20d60dbf));
        }
        delete nextevent;
        nextevent = 0;
    }

    m_currEvent = nextevent;
    Manager::Expire(m_currEvent);
    m_currEventSpeakerID = 0;
    if (m_currEvent && m_currEvent->actor) {
        m_currEventSpeakerID = static_cast<short>(m_currEvent->actor->GetSpeakerID());
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
    IssueSampleRequests();
    return 1;
}

void GameSpeech::IssueSampleRequests() {
    SPCHSampleRequest *i;

    if (IsSpeechEnabled) {
        SampleReqList &requests = Manager::GetSampleRequests();
        if (requests.empty()) {
            return;
        }

        if (requests.size() > 1) {
            std::sort(requests.begin(), requests.end());
        }

        for (i = requests.begin(); i != requests.end();) {
            SPCHSampleRequest &req = *i;
            SPCHType_SampleRequestData &data = req.data;
            SpeechSampleData *sample = gSpeechCache.LoadSample(this, &data);

            if (sample) {
                if (req.owner) {
                    req.owner->AddSample(sample, req.sample_index);
                } else if (!sample->cached) {
                    gSpeechCache.TossSample(sample);
                }
            } else if (req.owner) {
                sample = gSpeechCache.GetUncached(this, &data);
                if (gSpeechCache.IsCached(&data, false)) {
                    req.owner->AddSample(sample, req.sample_index);
                }
            }

            i = requests.erase(i);
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
    int vol = 0;
    if (m_pSFXOBJ_Speech) {
        switch (id) {
        case 1:
            vol = m_pSFXOBJ_Speech->GetDMixOutput(0xb, DMX_VOL);
            break;
        case 2:
            vol = m_pSFXOBJ_Speech->GetDMixOutput(0xa, DMX_VOL);
            break;
        case 3:
            vol = m_pSFXOBJ_Speech->GetDMixOutput(3, DMX_VOL);
            break;
        case 4:
            vol = m_pSFXOBJ_Speech->GetDMixOutput(4, DMX_VOL);
            break;
        case 5:
            vol = m_pSFXOBJ_Speech->GetDMixOutput(5, DMX_VOL);
            break;
        case 6:
            vol = m_pSFXOBJ_Speech->GetDMixOutput(6, DMX_VOL);
            break;
        case 7:
            vol = m_pSFXOBJ_Speech->GetDMixOutput(7, DMX_VOL);
            break;
        case 8:
            vol = m_pSFXOBJ_Speech->GetDMixOutput(8, DMX_VOL);
            break;
        default:
            vol = m_pSFXOBJ_Speech->GetDMixOutput(0xe, DMX_VOL);
            if (m_currEvent && (m_currEvent->ID == kSPCH1_EventID_CellCall) && m_currEvent->GetData(nullptr)) {
                Csis::CellCallStruct *data = static_cast<Csis::CellCallStruct *>(m_currEvent->GetData(nullptr));
                if (data && (data->cell_call_bucket == Csis::Type_cell_call_bucket_bucket_01) &&
                    (data->cell_call_number == Csis::Type_cell_call_number_call_07)) {
                    vol += vol;
                }
            }
            break;
        }
    }

    if (TestFlag(0x200)) {
        if (TestFlag(0x20)) {
            return vol;
        }
        return 0;
    } else {
        return vol;
    }
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
    if (IsNISAudioEnabled) {
        m_channel = channel;

        const Attrib::Collection *localizedCollection = g_pEAXSound->GetAttributes().nissfxstreams(0).GetCollection();
        Attrib::Gen::audiosystem *atr = new Attrib::Gen::audiosystem(localizedCollection, 0, 0);

        m_strm = gpEAXS_StrmMgr->GetStreamChannel(2);
        m_filename = atr->BIGPath();

        Attrib::StringKey evtfile(atr->EVTPath());
        Attrib::StringKey csifile(atr->CSIPath());
        Attrib::StringKey idxfile(atr->IDXPath());

        int nfilesize;
        {
            const char *filename = evtfile.GetString();
            if (filename == 0) {
                filename = "";
            }
            nfilesize = bFileSize(filename);
        }
        char **eventDat = &m_eventDat;
        if (nfilesize > 0) {
            *eventDat = gAudioMemoryManager.AllocateMemoryChar(nfilesize, "AUD: SED_NISSFX events", false);
        } else {
            *eventDat = 0;
        }

        if (m_eventDat != 0) {
            int thisobj = 0;
            mLoadState.push_back(thisobj);
            const char *filename = evtfile.GetString();
            if (filename == 0) {
                filename = "";
            }
            AddQueuedFile(m_eventDat, filename, 0, nfilesize, LoadingCallback, reinterpret_cast<int>(this), 0);
        }

        {
            const char *filename = idxfile.GetString();
            if (filename == 0) {
                filename = "";
            }
            nfilesize = bFileSize(filename);
        }
        char **tempCharPtr = &m_tempCharPtr;
        if (nfilesize > 0) {
            *tempCharPtr = static_cast<char *>(bMalloc(nfilesize, 0x1040));
        } else {
            *tempCharPtr = 0;
        }

        if (m_tempCharPtr != 0) {
            int thisobj = 1;
            mLoadState.push_back(thisobj);
            const char *filename = idxfile.GetString();
            if (filename == 0) {
                filename = "";
            }
            AddQueuedFile(m_tempCharPtr, filename, 0, nfilesize, LoadingCallback, reinterpret_cast<int>(this), 0);
        }

        {
            const char *filename = csifile.GetString();
            if (filename == 0) {
                filename = "";
            }
            nfilesize = bFileSize(filename);
        }
        char **csisData = &m_csisData;
        if (nfilesize > 0) {
            *csisData = gAudioMemoryManager.AllocateMemoryChar(nfilesize, "AUD: SED_NISSFX CSIS data", false);
        } else {
            *csisData = 0;
        }

        if (m_csisData != 0) {
            int thisobj = 2;
            mLoadState.push_back(thisobj);
            const char *filename = csifile.GetString();
            if (filename == 0) {
                filename = "";
            }
            AddQueuedFile(m_csisData, filename, 0, nfilesize, LoadingCallback, reinterpret_cast<int>(this), 0);
        }
    }
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
    if (IsNISAudioEnabled && m_strm) {
        int rval;
        m_SyncObject.qsObject = static_cast<SpeechSampleData *>(AllocateMemory(0x30, "NIS SampleData"));
        m_SyncObject.holdtime = -1;
        m_SyncObject.qsObject->cached = false;
        m_SyncObject.qsObject->ready = false;
        m_SyncObject.qsObject->dataoffset = GetBankOffset(data->bankNum) + data->sampleOffset;
        rval = m_strm->AddToStrmReq(GetFilename(), m_SyncObject.qsObject->dataoffset, -1);
        m_SyncObject.handle = rval;
    }
    return 1;
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
    if (m_SyncObject.id < STRM_NIS_RACE_START) {
        return;
    }

    if (IsNISAudioEnabled == 0) {
        return;
    }

    if (m_SyncObject.id == STRM_SFX_MOMENT) {
        SFX_Base *sfxmoment = m_pSFXOBJ_Moment;
        if (sfxmoment) {
            int vol = sfxmoment->GetDMixOutput(*reinterpret_cast<int *>(reinterpret_cast<char *>(sfxmoment) + 0x544), DMX_VOL);
            m_strm->SetVol(vol >> 8, true);
            if (*reinterpret_cast<bool *>(reinterpret_cast<char *>(sfxmoment) + 0x548)) {
                m_strm->SetAz(sfxmoment->GetDMixOutput(0, DMX_AZIM));
            } else {
                m_strm->SetAz(0);
            }
        }
    } else if (m_SyncObject.id == STRM_NIS_RACE_START) {
        if (m_pSFXOBJ_NISStream) {
            int vol = m_pSFXOBJ_NISStream->GetDMixOutput(1, DMX_VOL);
            m_strm->SetVol(vol >> 8, true);
            m_strm->SetAz(0);
        }
    } else if (m_SyncObject.id == STRM_NIS_BUSTED) {
        if (m_pSFXOBJ_NISStream) {
            int vol = m_pSFXOBJ_NISStream->GetDMixOutput(2, DMX_VOL);
            m_strm->SetVol(vol >> 8, true);
            m_strm->SetAz(0);
        }
    } else {
        m_strm->SetVol(100, true);
        m_strm->SetAz(0);
    }

    if (m_SyncObject.holdtime != -1) {
        m_SyncObject.holdtime -= static_cast<int>(SndBase::m_fDeltaTime * 1000.0f);
        if (m_SyncObject.holdtime < 0) {
            m_SyncObject.holdtime = 0;
            FreeMemory(m_SyncObject.qsObject);
            m_SyncObject.qsObject = 0;
        }
    } else if (m_strm->GetTimeBuffered() > 0) {
        if (m_SyncObject.qsObject == 0) {
            goto check_done;
        }

        m_bIsStreamQueued = true;
        if (m_SyncObject.callback && (m_SyncObject.id >= STRM_NIS_RACE_START) &&
            ((m_SyncObject.id < STRM_THUNDER) || (m_SyncObject.id == STRM_SFX_MOMENT)) && (m_SyncObject.holdtime <= 0)) {
            m_SyncObject.callback();
        }
        m_SyncObject.callback = 0;
        FreeMemory(m_SyncObject.qsObject);
        m_SyncObject.qsObject = 0;
    }

check_done:
    if (m_SyncObject.qsObject == 0) {
        if (!m_strm->IsPlaying()) {
            m_SyncObject.id = STRM_NONE;
            if (m_pSFXOBJ_NISStream) {
                static_cast<SFXObj_NISStream *>(m_pSFXOBJ_NISStream)->NISActivityDone();
            }
            mLastEventTimestamp = WorldTimer;
        }
    }
}

} // namespace Speech
