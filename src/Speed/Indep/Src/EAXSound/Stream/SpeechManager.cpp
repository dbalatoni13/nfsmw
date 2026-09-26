#include "Speed/Indep/Src/EAXSound/CSISAllocator.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/GameSpeech.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/NISSFXModule.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyCellCallComplete.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyCellCallStarted.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/Speech/SpeechCache.h"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"
#include "csis/csis.h"

#include <cstdio>
#include <cstdarg>

int VALIDATE_SED_GENERATE = 0;                    // Decl: 63
static unsigned int gSpeechSeed = 0;              // Decl: 64
static const int SPEECH_MAX_BANK_HDR_SIZE = 2048; // Decl: 65

extern int IsNISAudioEnabled;   // Decl: 71
int DEBUG_SPEECHAI = 0;         // Decl: 72
int SPEECH_DISPLAY_HISTORY = 0; // Decl: 73

static const int DUMP_SPEECH_EVENT_LISTS = 0; // Decl: 76
static int DUMP_SPEECH_HISTORY;               // Decl: 77

int EVLOG_ENABLED = 0; // Decl: 79

#define MAX_EVLOG_FILES 99999     // Decl: 84
#define MAX_LEN_EVLOG_FILENAME 20 // Decl: 85

namespace Speech {

Module *Manager::m_SpeechModule[2] = {};              // size: 0x8, Decl: 101
SPEECH_MODE Manager::m_speechMode = SPEECH_NONE_MODE; // Decl: 102
int Manager::m_numberSpeechBanks = 0;                 // Decl: 103
bool Manager::m_SPEECH_initted = false;               // size: 0x1, Decl: 104
char *Manager::m_SPEECH_bankPtrMem = nullptr;         // Decl: 105
bool Manager::m_speechDisable = false;                // size: 0x1, Decl: 106
int Manager::m_gameSpeechInitted = 0;                 // Decl: 107

int Manager::m_NISAudioInitted = 0;                     // Decl: 109
float Manager::m_clock_in_ms = 0.0f;                    // Decl: 110
float Manager::m_timestep = 0.0f;                       // Decl: 111
float Manager::m_deadair = 0.0f;                        // Decl: 112
SchedSpchEvents Manager::mEvents[4];                    // size: 0x50, Decl: 113
SPCHEventList Manager::mEvtHistory;                     // size: 0xC, Decl: 114
SpeechHashIDMap Manager::mHashMap;                      // size: 0x854, Decl: 115
EventHistory Manager::mGlobalHistory;                   // size: 0xC74, Decl: 116
ScheduledSpeechEvent *Manager::mCurrentEvent = nullptr; // Decl: 117
short Manager::m_frameindex = 0;                        // size: 0x2, Decl: 118
float Manager::mProbPlayback = 1.0f;                    // Decl: 119
SampleReqList Manager::mSampleRequests;                 // size: 0x14, Decl: 120
Timer Manager::mSampleReqTimer;                         // Decl: 121
short Manager::mLastSpeakerID = 0;                      // size: 0x2, Decl: 122

template <typename T> void Manager::ScheduleSpeech(T &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) {
    ScheduledSpeechEvent *event = ScheduleSpeechPartII(sizeof(T), &data, iid, fh, actor);
}

ScheduledSpeechEvent *Manager::ScheduleSpeechPartII(unsigned int size, void *data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh,
                                                    EAXCharacter *actor) {
    if ((IsSpeechEnabled == 0) || (m_speechMode == SPEECH_SPLITSCREEN_MODE) || (g_pEAXSound == nullptr) ||
        (g_pEAXSound->GetCurAudioSettings() == nullptr) || (g_pEAXSound->GetCurAudioSettings()->SpeechVol == 0.0f)) {
        unsigned int eventkey = Attrib::StringToLowerCaseKey(iid.pString);
        SPCHType_1_EventID eventID = mHashMap.GetID(eventkey);
        if (eventID == kSPCH1_EventID_CellCall) {
            MNotifyCellCallStarted().Send(UCrc32(UCRC32_Gameplay));
            MNotifyCellCallComplete().Send(UCrc32(UCRC32_Gameplay));
        }
        return nullptr;
    }

    if (gSpeechCache.GetEventPool()->IsFull()) {
        return nullptr;
    }

    Attrib::Key eventkey = Attrib::StringToLowerCaseKey(iid.pString);
    SPCHType_1_EventID eventID = mHashMap.GetID(eventkey);
    Attrib::Gen::speech event_atr(eventkey, 0, nullptr);

    for (int i = 0; i < NUM_ELEMENTS(mEvents); ++i) {
        for (SchedSpchEvents::iterator iter = mEvents[i].begin(); iter != mEvents[i].end(); ++iter) {
            ScheduledSpeechEvent *this_event = *iter;
            if (this_event->ID == eventID) {
                bool obj_updated = ((actor == nullptr) && (this_event->actor == nullptr)) ||
                                   ((actor != nullptr) && (this_event->actor != nullptr) && (this_event->actor != actor));

                if (obj_updated) {
                    this_event->entry_time = WorldTimer;
                    this_event->actor = actor;
                    if (size > 0) {
                        bMemCpy(this_event->GetData(nullptr), data, size);
                    }
                }
                return nullptr;
            }
        }
    }

    if (!event_atr.DoNotDropout()) {
        bool playback = CanPlayback(event_atr);
        if (!playback) {
            return nullptr;
        }
    }

    if (event_atr.Num_RecallList() > 0) {
        for (unsigned int i = 0; i < event_atr.Num_RecallList(); ++i) {
            Attrib::Gen::speech recall(event_atr.RecallList(i).GetCollectionKey(), 0, nullptr);
            SPCHType_1_EventID recall_id = recall.SpeechID();
            if (IsQueued(recall_id, 4)) {
                RecallSpeechEvent(recall_id);
            }
        }
    }

    ScheduledSpeechEvent *event = new (size) ScheduledSpeechEvent();
    if (event == nullptr) {
        return nullptr;
    }

    event->iid = &iid;
    event->fh = &fh;
    event->actor = actor;
    event->ID = eventID;
    event->priority = static_cast<uint8>(event_atr.interrupt() ? event_atr.priority() + 100 : event_atr.priority());
    if (event_atr.InitDelay() > 0.0f) {
        event->flags |= 2;
    }
    mEvents[0].push_back(event);
    if (size > 0) {
        bMemCpy(event->GetData(nullptr), data, size);
    }
    return event;
}

Csis::Result Manager::IndirectSpeechEvent(ScheduledSpeechEvent *evt, bool test_only) {
    if ((IsSpeechEnabled == 0) || (m_speechMode == SPEECH_SPLITSCREEN_MODE) || (evt == nullptr)) {
        return Csis::RESULT_ERR_ALLOCATE;
    }

    Csis::Result result = Csis::Function::Call(evt->fh, evt->GetData(nullptr));
    int rval;

    if (result < Csis::RESULT_OK) {
        evt->fh->Set(evt->iid);
        result = Csis::Function::Call(evt->fh, evt->GetData(nullptr));
    }

    mCurrentEvent = evt;
    if (SPCH_Choose(GetChannel(COPSPEECH_MODULE)) != 0) {
        if (!test_only) {
            evt->assoc_samples_count = static_cast<uint8>(SPCH_Play(GetChannel(COPSPEECH_MODULE)));
            if (evt->assoc_samples_count == 0) {
                result = Csis::RESULT_ERR_ALLOCATE;
            }
        }
    } else {
        result = Csis::RESULT_ERR_NOTFOUND;
    }

    return result;
}

void Manager::Destroy() {
    switch (m_speechMode) {
        case SPEECH_GAME_MODE:
            delete m_SpeechModule[COPSPEECH_MODULE];
            m_SpeechModule[COPSPEECH_MODULE] = nullptr;

            if (m_SpeechModule[NISSFX_MODULE] != nullptr) {
                delete m_SpeechModule[NISSFX_MODULE];
                m_SpeechModule[NISSFX_MODULE] = nullptr;
            }
            break;
        case SPEECH_SPLITSCREEN_MODE:
            if (m_SpeechModule[NISSFX_MODULE] != nullptr) {
                delete m_SpeechModule[NISSFX_MODULE];
                m_SpeechModule[NISSFX_MODULE] = nullptr;
            }
            break;
        case SPEECH_FRONTEND_MODE:
        default:
            break;
    }

    if (m_SPEECH_bankPtrMem != nullptr) {
        gAudioMemoryManager.FreeMemory(m_SPEECH_bankPtrMem);
        m_SPEECH_bankPtrMem = nullptr;
    }
    ClearPlayback();
    mEvtHistory.clear();
    mGlobalHistory.clear();
    mSampleRequests.clear();
}

void Manager::Init(SPEECH_MODE mode) {
    if (IsSoundEnabled == 0) {
        return;
    }

    mEvtHistory.clear();
    for (int ndx = 0; ndx < NUM_ELEMENTS(mEvents); ++ndx) {
        mEvents[ndx].clear();
    }

    mGlobalHistory.clear();
    mSampleRequests.reserve(20);
    mSampleRequests.clear();
    m_numberSpeechBanks = 0;
    SetSpeechMode(mode);

    int rate = SPCH_GetSampleDataRate(22050, 0x10, kSPCH_Compression_MicroTalk);
    SPCHType_ExtVecs *spchVecs = SPCH_GetExtVecs();

    if (gSpeechSeed == 0) {
        gSpeechSeed = g_pEAXSound->Random(-1);
    } else {
        ++gSpeechSeed;
    }

    SPCH_SetMemCallbacks(CSISAllocatorMemAlloc, CSISAllocatorMemFree);
    SPCH_Init(SampleRequestCallback, bAbs(static_cast<int>(gSpeechSeed)), rate);
    spchVecs->spchAbortMessage = SpchLibAbort;
    spchVecs->spchGetTick = GetTicker;
    SPCH_InitRuleCallbacks(TestSentenceRuleCallback, SetSentenceRuleCallback);
    SPCH_InitEventRuleCallback(EventRuleCallback);
    SPCH_InitReparmCallback(ReparmCallback);

    switch (m_speechMode) {
        case SPEECH_GAME_MODE:
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
            break;
        case SPEECH_SPLITSCREEN_MODE:
            m_NISAudioInitted = 0;
            m_gameSpeechInitted = 0;
            if (IsNISAudioEnabled) {
                m_SpeechModule[NISSFX_MODULE] = new ("AUD:NISSFXModule") SED_NISSFX();
                m_SpeechModule[NISSFX_MODULE]->Init(SPEECH_FRONTEND_MODE);
            }
            break;
        case SPEECH_FRONTEND_MODE:
        default:
            break;
    }

    PopulateHashMap();
    mGlobalHistory.Init();
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
                SPCH_AddEventDB(m_SpeechModule[COPSPEECH_MODULE]->GetEventDat(), m_SpeechModule[COPSPEECH_MODULE]->GetChannel());
                m_gameSpeechInitted = 1;
            }

            if (IsNISAudioEnabled && m_SpeechModule[NISSFX_MODULE]->IsDataLoaded() && !m_NISAudioInitted) {
                m_numberSpeechBanks += m_SpeechModule[NISSFX_MODULE]->GetNumBanks();
                Csis::System::Subscribe(m_SpeechModule[NISSFX_MODULE]->GetCSIptr());
                Csis::CacheHandlesEventsNIS();
                SPCH_AddEventDB(m_SpeechModule[NISSFX_MODULE]->GetEventDat(), m_SpeechModule[NISSFX_MODULE]->GetChannel());
                m_NISAudioInitted = 1;
            }

            if (m_SpeechModule[COPSPEECH_MODULE] != nullptr && !m_gameSpeechInitted) {
                return;
            }
            if (m_SpeechModule[NISSFX_MODULE] != nullptr && !m_NISAudioInitted) {
                return;
            }
            break;
        case SPEECH_FRONTEND_MODE:
            break;
        case SPEECH_SPLITSCREEN_MODE:
            if (IsNISAudioEnabled && m_SpeechModule[NISSFX_MODULE]->IsDataLoaded() && !m_NISAudioInitted) {
                m_numberSpeechBanks += m_SpeechModule[NISSFX_MODULE]->GetNumBanks();
                Csis::System::Subscribe(m_SpeechModule[NISSFX_MODULE]->GetCSIptr());
                Csis::CacheHandlesEventsNIS();
                SPCH_AddEventDB(m_SpeechModule[NISSFX_MODULE]->GetEventDat(), m_SpeechModule[NISSFX_MODULE]->GetChannel());
                m_NISAudioInitted = 1;
            }
            break;
        default:
            break;
    }

    int buffSize = SPCH_GetBankPtrMemSize(m_numberSpeechBanks);
    if (m_speechMode == SPEECH_GAME_MODE) {
        m_SPEECH_bankPtrMem = gAudioMemoryManager.AllocateMemoryChar(buffSize, "AUD:Speech Bank Ptrs", false);
    } else {
        m_SPEECH_bankPtrMem = gAudioMemoryManager.AllocateMemoryChar(buffSize, "AUD:Speech Bank Ptrs", false);
    }
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

    m_SPEECH_initted = true;
}

void Manager::AttachSFXOBJ(SpeechModuleIndex idx, SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype) {
    if (m_SpeechModule[idx] != nullptr) {
        m_SpeechModule[idx]->AttachSFXOBJ(psfx, sfxtype);
    }
}

Module *Manager::GetSpeechModule(int nindex) {
    if (nindex < 2) {
        return m_SpeechModule[nindex];
    }
    return nullptr;
}

bool Manager::IsPlaying(SpeechModuleIndex idx) {
    if (m_SpeechModule[idx] != nullptr) {
        EAXS_StreamChannel *strm = m_SpeechModule[idx]->GetStreamChannel();
        if (strm != nullptr) {
            return strm->IsPlaying();
        }
    }
    return false;
}

bool Manager::IsCopSpeechPlaying(SPCHType_1_EventID event_id) {
    if (m_SpeechModule[COPSPEECH_MODULE] != nullptr) {
        GameSpeech *gamespeech = static_cast<GameSpeech *>(m_SpeechModule[COPSPEECH_MODULE]);
        {
            ScheduledSpeechEvent *curr = gamespeech->GetCurrentEvent();
            if (curr != nullptr && curr->ID == event_id) {
                return true;
            }
        }
    }
    return false;
}

bool Manager::IsCopSpeechBusy() {
    bool module_busy = false;
    GameSpeech *gamespeech = static_cast<GameSpeech *>(m_SpeechModule[COPSPEECH_MODULE]);
    bool speech_busy;
    if (gamespeech != nullptr) {
        speech_busy = gamespeech->IsBusy();
        if (speech_busy || !gamespeech->DonePlaying()) {
            module_busy = true;
        }
    }
    if (!module_busy) {
        return false;
    }
    return true;
}

Timer Manager::GetTimeSinceLastEvent(SpeechModuleIndex idx) {
    Timer rval = WorldTimer;
    if (m_SpeechModule[idx] != nullptr && !IsPlaying(idx)) {
        rval = m_SpeechModule[idx]->GetLastEventTimestamp();
    }
    return rval;
}

void Manager::SpchLibAbort(const char *format, ...) {
    static char msg[512];
    va_list arglist;
    va_start(arglist, format);
    vsprintf(msg, format, arglist);
    va_end(arglist);
}

int Manager::SampleRequestCallback(SPCHType_SampleRequestData *data) {
    if (SpeechDisabled()) {
        return 0;
    }

    switch (data->channel) {
        case 0: {
            if (mSampleRequests.empty()) {
                mSampleReqTimer = RealTimer;
            }

            unsigned short event_id = data->eventSpec.eventID;
            SpeechSampleData *sample;
            bool track_streamer_busy = TheTrackStreamer.GetLoadingBacklog() >= TRACKSTREAMER_BACKLOG_THRESH;
            if (track_streamer_busy) {
                mCurrentEvent->flags |= 1;
            }

            if (gSpeechCache.IsCached(data, false)) {
                sample = gSpeechCache.GetSample(m_SpeechModule[COPSPEECH_MODULE], data);
                if (sample != nullptr) {
                    mCurrentEvent->AddSample(sample, 0xff);
                }
            } else {
                if (IsCacheable(static_cast<SPCHType_1_EventID>(event_id))) {
                    SPCHSampleRequest new_req;
                    new_req.data = *data;
                    new_req.offset = m_SpeechModule[COPSPEECH_MODULE]->GetBankOffset(data->bankNum) + data->sampleOffset;
                    new_req.owner = mCurrentEvent;
                    new_req.sample_index = mCurrentEvent->ReserveSample();
                    mSampleRequests.push_back(new_req);
                } else {
                    sample = gSpeechCache.GetUncached(m_SpeechModule[COPSPEECH_MODULE], data);
                    mCurrentEvent->AddSample(sample, 0xff);
                }
            }
            break;
        }
        case 1:
            break;
        case 2:
            return static_cast<int>(m_SpeechModule[NISSFX_MODULE]->SampleRequestCallback(data));
        default:
            break;
    }

    return 0;
}

int Manager::TestSentenceRuleCallback(EventSpec *eventInfo, int ruleID, int parmValue, int userNum) {
    return 0;
}

int Manager::ReparmCallback(int ruleID, unsigned int *parms) {
    return 0;
}

void Manager::SetSentenceRuleCallback(EventSpec *eventInfo, int ruleID, int parmValue, int userNum) {}

SPCHType_EventRuleResult Manager::EventRuleCallback(EventSpec *eventInfo) {
    return kSPCH_EventRule_OK;
}

int Manager::LoadSpeechBank(CLUMP_IDX_FILEtag *index, int &type, int &number, SPEECH_BANK *sb) {
    unsigned int key = type * 0x1000000 + number;
    int lower = 0;
    int upper = index->count - 1;
    int i = upper >> 1;

    while ((index->item[i].key != key) && (i != lower) && (i != upper)) {
        if (index->item[i].key < key) {
            lower = i;
        } else {
            upper = i;
        }
        i = (lower + upper) >> 1;
    }

    if (index->item[i].key != key) {
        if (index->item[lower].key == key) {
            i = lower;
        } else if (index->item[upper].key == key) {
            i = upper;
        } else {
            sb->mem = nullptr;
            sb->offset = 0;
            sb->bank = -1;
            return -1;
        }
    }

    if (index->item[i].size == 0) {
        sb->mem = nullptr;
        sb->bank = -1;
        sb->offset = 0;
        return -1;
    }

    sb->mem = reinterpret_cast<char *>(index->numbanks) + (index->item[i].header - 4);
    sb->bank = index->item[i].size;
    sb->offset = index->item[i].sample;
    return 0;
}

int Manager::AddHeaders(char **dest, SPEECH_BANK *banks, int numBanks, Module *module) {
    int size = 0;
    char *mem;
    SPEECH_BANK *sb = banks;
    Timer t_validate;
    while (sb < banks + numBanks) {
        if (sb->mem != nullptr) {
            size += sb->bank;
        }
        ++sb;
    }

    mem = *dest = gAudioMemoryManager.AllocateMemoryChar(size, "AUD:Relocated speech headers", false);

    sb = banks;
    while (sb < banks + numBanks) {
        if (sb->mem != nullptr) {
            bMemCpy(mem, sb->mem, sb->bank);
            sb->mem = mem;
            mem += sb->bank;
            sb->bank = SPCH_AddBank(sb->mem);
        }
        ++sb;
    }
    return 0;
}

int Manager::GetTicker() {
    m_clock_in_ms = FramesToSeconds(RealTimeFrames) * 1000.0f;
    return static_cast<int>(m_clock_in_ms);
}

void Manager::Update(float t) {
    static unsigned int max_samplerequests = 0;

    m_timestep = t;
    m_frameindex = 0;

    switch (m_speechMode) {
        case SPEECH_FRONTEND_MODE:
            break;
        case SPEECH_SPLITSCREEN_MODE:
            if (m_SpeechModule[NISSFX_MODULE] != nullptr && IsNISAudioEnabled) {
                m_SpeechModule[NISSFX_MODULE]->Update();
            }
            break;
        case SPEECH_GAME_MODE:
            if (m_SpeechModule[NISSFX_MODULE] != nullptr && IsNISAudioEnabled) {
                m_SpeechModule[NISSFX_MODULE]->Update();
            }

            if (m_SpeechModule[COPSPEECH_MODULE] != nullptr && IsSpeechEnabled) {
                m_deadair = g_EAXIsPaused() ? 65535.0f : (WorldTimer - GetTimeSinceLastEvent(COPSPEECH_MODULE)).GetSeconds();

                CalcProbPlayback();

                {
                    bool track_streamer_busy = TheTrackStreamer.GetLoadingBacklog() >= TRACKSTREAMER_BACKLOG_THRESH;
                    if (mSampleRequests.size() > 0 && !track_streamer_busy) {
                        for (int ndx = 0; ndx < 4; ++ndx) {
                            for (SchedSpchEvents::iterator i = mEvents[ndx].begin(); i != mEvents[ndx].end(); ++i) {
                                ScheduledSpeechEvent *this_event = *i;
                                if ((this_event->flags & 1) != 0) {
                                    this_event->flags &= ~1;
                                }
                            }
                        }

                        if (mSampleRequests.size() > max_samplerequests) {
                            max_samplerequests = mSampleRequests.size();
                        }

                        m_SpeechModule[COPSPEECH_MODULE]->SampleRequestCallback(nullptr);
                    }
                }

                m_SpeechModule[COPSPEECH_MODULE]->Update();

                if (SPEECH_DISPLAY_HISTORY != 0) {
                    SPCHEventList::iterator i = mEvtHistory.begin();
                    int y;
                    int x;
                    for (; i != mEvtHistory.end(); i++) {
                        SPCHType_1_EventID event = *i;
                        History *hist = mGlobalHistory.Find(event);
                        if (hist != nullptr) {
                            Attrib::Gen::speech speech(mHashMap.GetHash(event), 0, nullptr);
                            speech.CollectionName();
                        }
                    }
                }
            }
            break;
        default:
            break;
    }
}

void Manager::PopulateHashMap() {
    if (mHashMap.empty()) {
        const Attrib::Class *speechevents = Attrib::Database::Get().GetClass(0xc593dd47);
        unsigned int eventkey = speechevents->GetFirstCollection();
        while (eventkey != 0) {
            Attrib::Gen::speech event_collection(eventkey, 0, nullptr);
            SPCHType_1_EventID id;
            event_collection.SpeechID(id);
            mHashMap.Add(eventkey, id);
            eventkey = speechevents->GetNextCollection(eventkey);
        }
    }

    for (unsigned int i = 0; i < mHashMap.size(); ++i) {
        SpeechEventPair &data = mHashMap[i];
        SPCHType_1_EventID test_id = mHashMap.GetID(data.hash);
        unsigned int test_hash = mHashMap.GetHash(data.id);
    }
}

bool Manager::IsCacheable(SPCHType_1_EventID event_id) {
    Attrib::Key key = mHashMap.GetHash(event_id);
    if (key != 0) {
        Attrib::Gen::speech event(key, 0, nullptr);
        return event.cache_OnCreate();
    }
    return false;
}

bool Manager::HasBeenSaid(SPCHType_1_EventID event_id) {
    SPCHEventList::iterator i = mEvtHistory.begin();
    for (; i != mEvtHistory.end(); i++) {
        SPCHType_1_EventID event = *i;
        if (event == event_id) {
            return true;
        }
    }
    return false;
}

static bool InteruptedAndNotDelayed(ScheduledSpeechEvent *this_event) {
    Attrib::Gen::speech pb(Manager::GetHashIDMap().GetHash(this_event->ID), 0, nullptr);
    if (pb.interrupt() && (this_event->flags & 2) == 0) {
        delete this_event;
        return true;
    }
    return false;
}

bool Manager::ServiceInterruptEvents() {
    if (mEvents[3].size() > 0) {
        if (mEvents[3].size() > 1) {
            std::sort(mEvents[3].begin(), mEvents[3].end());
        }

        SchedSpchEvents::iterator priority_event_iter = mEvents[3].begin();
        ScheduledSpeechEvent *priority_event = *priority_event_iter;
        bool interruptable = true;
        GameSpeech *gamespeech = static_cast<GameSpeech *>(m_SpeechModule[COPSPEECH_MODULE]);
        ScheduledSpeechEvent *curr_event = gamespeech->GetCurrentEvent();

        if (curr_event != nullptr) {
            Attrib::Gen::speech pb(mHashMap.GetHash(curr_event->ID), 0, nullptr);
            bool curr_is_interrupt = pb.interrupt();
            interruptable = pb.Interruptable();
            if (gamespeech->IsForcingSilence()) {
                interruptable = true;
            }
            if (curr_is_interrupt && (priority_event->priority > curr_event->priority)) {
                interruptable = true;
            }
        }

        if (interruptable) {
            if (mEvents[1].size() > 0) {
                mEvents[0] = mEvents[1];
                mEvents[1].clear();
            }

            Csis::Result rval = IndirectSpeechEvent(priority_event, false);

            // TODO switch
            if (rval != Csis::RESULT_ERR_ALLOCATE) {
                if (rval <= Csis::RESULT_ERR_ALLOCATE) {
                    if (rval == Csis::RESULT_ERR_NOTFOUND) {
                        delete priority_event;
                        mEvents[3].erase(priority_event_iter);
                        return false;
                    }
                }
            } else {
                if (PostValidate(priority_event, 1) != kDitchEvt) {
                    mEvents[0].insert(mEvents[0].begin(), priority_event);
                    mEvents[3].erase(priority_event_iter);
                    return true;
                }
            }
            if (mEvents[2].size() > 0) {
                SchedSpchEvents::iterator i = std::remove_if(mEvents[2].begin(), mEvents[2].end(), InteruptedAndNotDelayed);
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

void Manager::ServiceFilteredEvents() {
    if (mEvents[1].size() > 0) {
        std::sort(mEvents[1].begin(), mEvents[1].end());

        SchedSpchEvents::iterator i = mEvents[1].begin();
        while (i != mEvents[1].end()) {
            bool trimmed = false;
            ScheduledSpeechEvent *this_event = *i;

            if (this_event->assoc_samples_count < 1) {
                Csis::Result rval = IndirectSpeechEvent(this_event, false);
                switch (rval) {
                    case Csis::RESULT_ERR_NOTFOUND:
                        delete this_event;
                        i = mEvents[1].erase(i);
                        trimmed = true;
                        break;
                    case Csis::RESULT_ERR_ALLOCATE:
                        if (PostValidate(this_event, 1) != kDitchEvt) {
                            mEvents[0].insert(mEvents[0].begin(), this_event);
                        }
                        break;
                    case Csis::RESULT_OK:
                    default:
                        mEvents[2].push_back(this_event);
                        break;
                }
            }

            if (false) {
                int y;
                int x;
                mEvents[0].size();
                gSpeechCache.GetEventPool()->CountAllocatedSlots();
                gSpeechCache.GetEventPool()->CountFreeSlots();
                mEvents[1].size();
            }

            if (!trimmed) {
                ++i;
            }
        }

        mEvents[1].clear();
    }
}

void Manager::Deduce() {
    SchedSpchEvents deferredEvents;
    if (mEvents[0].size() > 0) {
        SchedSpchEvents::iterator i = mEvents[0].begin();
        while (i != mEvents[0].end()) {
            ScheduledSpeechEvent *this_event = *i;
            SpeechValRtnType keep = static_cast<SpeechValRtnType>(PreValidate(*this_event));

            if (false) {
                int y;
                int x;
                mEvents[0].size();
                gSpeechCache.GetEventPool()->CountAllocatedSlots();
                gSpeechCache.GetEventPool()->CountFreeSlots();
                mEvents[1].size();
            }

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
                    this_event = nullptr;
                    break;
            }

            mEvents[0].erase(i);
            i = mEvents[0].begin();
        }
        if (false) {
            mEvents[0].empty();
        }
        mEvents[0].clear();
    }

    if (deferredEvents.size() > 0) {
        SchedSpchEvents::iterator i = deferredEvents.begin();
        for (; i != deferredEvents.end(); ++i) {
            ScheduledSpeechEvent *deferral = *i;
            if (PostValidate(deferral, 1) == kDitchEvt) {
                deferredEvents.erase(i);
                delete deferral;
                deferral = nullptr;
            } else {
                Attrib::Gen::speech deferral_atr(mHashMap.GetHash(deferral->ID), 0, nullptr);
                if (!deferral_atr.interrupt()) {
                    mEvents[0].insert(mEvents[0].begin(), deferral);
                } else {
                    mEvents[3].push_back(deferral);
                }
            }
        }

        deferredEvents.clear();
    }

    ServiceFilteredEvents();
    bool service;
    do {
        service = ServiceInterruptEvents();
    } while (!service);

    if (false) {
        int y;
        int x;
        mEvents[0].size();
        gSpeechCache.GetEventPool()->CountAllocatedSlots();
        gSpeechCache.GetEventPool()->CountFreeSlots();
        mEvents[1].size();
    }
}

void Manager::ClearPlayback() {
    for (int ndx = 0; ndx < 4; ++ndx) {
        if (mEvents[ndx].size() > 0) {
            SchedSpchEvents::iterator i = mEvents[ndx].begin();
            for (; i != mEvents[ndx].end(); ++i) {
                ScheduledSpeechEvent *this_event = *i;
                if (this_event != nullptr) {
                    delete this_event;
                }
            }
            mEvents[ndx].clear();
        }
    }
    mSampleRequests.clear();
}

bool Manager::RecallSpeechEvent(SPCHType_1_EventID recall_id) {
    bool found_event = false;
    for (int ndx = 0; ndx < 4; ++ndx) {
        if (mEvents[ndx].size() > 0) {
            for (SchedSpchEvents::iterator i = mEvents[ndx].begin(); i != mEvents[ndx].end();) {
                ScheduledSpeechEvent *this_event = *i;
                if (this_event->ID == recall_id) {
                    delete this_event;
                    i = mEvents[ndx].erase(i);
                    found_event = true;
                } else {
                    ++i;
                }
            }
        }
    }
    return found_event;
}

void Manager::ResetGlobalHistory() {
    mGlobalHistory.Reset();
    mEvtHistory.clear();
}

void Manager::Expire(ScheduledSpeechEvent *event) {
    if (event == nullptr) {
        return;
    }
    for (SchedSpchEvents::iterator j = mEvents[2].begin(); j != mEvents[2].end(); ++j) {
        if (*j == event) {
            mEvents[2].erase(j);
            return;
        }
    }
}

bool Manager::IsQueued(SPCHType_1_EventID evtID, int indices) {
    int start_index = 0;
    if (indices != 4) {
        start_index = indices;
    } else {
        start_index = 0;
    }
    int end_index = 4;
    if (indices != 4) {
        end_index = indices + 1;
    }
    for (int ndx = start_index; ndx < end_index; ++ndx) {
        if (mEvents[ndx].size() > 0) {
            for (SchedSpchEvents::iterator i = mEvents[ndx].begin(); i != mEvents[ndx].end(); ++i) {
                ScheduledSpeechEvent *this_event = *i;
                if (this_event->ID == evtID) {
                    return true;
                }
            }
        }
    }
    return false;
}

float Manager::IsEventDead(ScheduledSpeechEvent *evt) {
    Attrib::Gen::speech event(mHashMap.GetHash(evt->ID), 0, nullptr);
    float enforce_time = event.EnforceDeadAir();
    if (enforce_time <= 0.0f) {
        return -1.0f;
    }

    float elapsed = (WorldTimer - evt->finish_time).GetSeconds();

    return elapsed >= enforce_time ? -1.0f : enforce_time - elapsed;
}

void Manager::NotifyEventCompletion(ScheduledSpeechEvent *evt, bool playback_complete) {
    if (evt == nullptr) {
        return;
    }

    SoundAI *ai = SoundAI::Get();
    unsigned short speakerID = evt->actor != nullptr ? static_cast<unsigned short>(evt->actor->GetSpeakerID()) : 0;

    mLastSpeakerID = speakerID > 1 ? speakerID : mLastSpeakerID;

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

    MNotifySpeechStatus(evt).Send(UCrc32(UCRC32_Gameplay));

    if (evt->ID == kSPCH1_EventID_CellCall) {
        MNotifyCellCallComplete().Send(UCrc32(UCRC32_Gameplay));
    }
}

ScheduledSpeechEvent *Manager::GetNextEvent() {
    if (mEvents[2].size() > 0) {
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

        SchedSpchEvents::iterator iter = mEvents[2].begin();
        while (iter != mEvents[2].end()) {
            ScheduledSpeechEvent *next = *iter;
            if (next->assoc_samples_prep == 0) {
                short prepared_count = 0;
                for (short i = 0; i < MAX_STITCH_SAMPLES_PER_EVENT; ++i) {
                    SpeechSampleData *stitch = next->assoc_samples[i];
                    if (stitch != nullptr && stitch->ready) {
                        ++prepared_count;
                    }
                }

                if (prepared_count == next->assoc_samples_count) {
                    next->assoc_samples_prep = 1;
                    if ((next->flags & 2) == 0) {
                        next->entry_time = WorldTimer;
                    }
                } else {
                    SpeechValRtnType keep = PostValidate(next, 1);
                    if (next->assoc_samples_count == 0 || keep == kDitchEvt) {
                        mEvents[2].erase(iter);
                        delete next;
                        return nullptr;
                    }

                    Attrib::Gen::speech event(mHashMap.GetHash(next->ID), 0, nullptr);
                    if (event.interrupt()) {
                        return nullptr;
                    }
                }
            }

            if (next->assoc_samples_prep != 0) {
                SpeechValRtnType keep = PostValidate(next, 0xffffffff);
                if (keep == kDitchEvt) {
                    mEvents[2].erase(iter);
                    delete next;
                    return nullptr;
                }
                if (keep == kDeferEvt) {
                    if (iter == mEvents[2].end()) {
                        return nullptr;
                    }
                    ++iter;
                } else if (keep == kKeepEvt) {
                    return next;
                }
            } else {
                return nullptr;
            }
        }
    }

    return nullptr;
}

SpeechValRtnType Manager::PostValidate(ScheduledSpeechEvent *evt, unsigned int mask) {
    if (evt != nullptr) {
        const History *history = mGlobalHistory.Find(evt->ID);
        Attrib::Gen::speech event(mHashMap.GetHash(evt->ID), 0, nullptr);
        SoundAI *ai = SoundAI::Get();

        if (evt->flags & 2) {
            float delay = event.InitDelay();
            float elapsed = (WorldTimer - evt->entry_time).GetSeconds();
            if (elapsed < delay) {
                return kDeferEvt;
            }
            evt->entry_time = WorldTimer;
            evt->flags &= ~2;
        }

        float elapsed = (WorldTimer - evt->entry_time).GetSeconds();
        float expiry = event.expiry();
        if (elapsed > expiry) {
            if (mask & 1) {
                return kDitchEvt;
            }
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

        float last_heard;
        if (history->count > 0 && (mask & 8)) {
            last_heard = (WorldTimer - history->time).GetSeconds();
            if (last_heard < event.Interval()) {
                return kDeferEvt;
            }
        }

        if (evt->actor != nullptr) {
            if (evt->actor->GetDistance() > event.CullingRange() && (mask & 0x10)) {
                return kDeferEvt;
            }
        }

        if (event.Num_DepFollow() > 0 && (mask & 0x20)) {
            short pass = 0;

            if (event.BackTime() > 0.0f) {
                for (unsigned int i = 0; i < event.Num_DepFollow(); ++i) {
                    Attrib::Gen::speech dependency(event.DepFollow(i).GetCollectionKey(), 0, nullptr);
                    Timer elapsed = mGlobalHistory.GetTime(dependency.SpeechID());
                    int count = mGlobalHistory.GetCount(dependency.SpeechID());
                    bool dep_is_playing = IsCopSpeechPlaying(dependency.SpeechID());
                    float t_elapsed = (WorldTimer - elapsed).GetSeconds();

                    if ((t_elapsed < event.BackTime() && count > 0) || dep_is_playing) {
                        ++pass;
                        break;
                    }
                }
            } else if (mEvtHistory.size() > 0) {
                SPCHType_1_EventID last_evt_id = mEvtHistory.front();
                Attrib::Key last_hist_key = mHashMap.GetHash(last_evt_id);
                for (unsigned int i = 0; i < event.Num_DepFollow(); ++i) {
                    Attrib::Gen::speech dependency(event.DepFollow(i).GetCollectionKey(), 0, nullptr);
                    bool dep_is_playing = IsCopSpeechPlaying(dependency.SpeechID());
                    Attrib::Key dep_key = dependency.GetCollection();

                    if (dep_key == last_hist_key || dep_is_playing) {
                        ++pass;
                        break;
                    }
                }
            }

            if (pass == 0) {
                return kDeferEvt;
            }
        }

        if (event.OnScreenOnly() && (mask & 0x40) && evt->actor != nullptr) {
            IRenderable *render = nullptr;
            ISimable *simable = ISimable::FindInstance(evt->actor->GetHandle());

            if (simable != nullptr) {
                simable->QueryInterface(&render);
            }
            if (render != nullptr && !render->InView()) {
                return kDeferEvt;
            }
        }

        if (event.reqLOS() && (mask & 0x80) && evt->actor != nullptr && !evt->actor->HasLOS()) {
            return kDeferEvt;
        }

        short curr_heat = ai->GetHeat();
        if (curr_heat > event.MaxHeat() || curr_heat < event.MinHeat()) {
            if (mask & 0x100) {
                return kDeferEvt;
            }
        }

        float pspeed = bAbs(ai->GetPlayerSpeed());
        if (pspeed < event.MinPlayerSpeed() || pspeed > event.MaxPlayerSpeed()) {
            if (mask & 0x200) {
                return kDeferEvt;
            }
        }

        return kKeepEvt;
    }
    return kDitchEvt;
}

SpeechValRtnType Manager::PreValidate(ScheduledSpeechEvent &evt) {
    if (evt.flags & 1) {
        return kDeferEvt;
    }

    Attrib::Gen::speech event(mHashMap.GetHash(evt.ID), 0, nullptr);
    SpeechValRtnType rval = kKeepEvt;

    if (evt.flags & 2) {
        float delay = event.InitDelay();
        float elapsed = (WorldTimer - evt.entry_time).GetSeconds();
        if (elapsed < delay) {
            return kDeferEvt;
        }

        evt.entry_time = WorldTimer;
        evt.flags = static_cast<short>(evt.flags & ~2);
    }

    bool is_int = event.interrupt();
    if (is_int) {
        rval = PostValidate(&evt, 0xffffffff);
        if (rval == kKeepEvt) {
            return kIntEvt;
        }
        return rval;
    }

    // TODO
    if (false) {
        event.~speech();
    }
    return kKeepEvt;
}

bool Manager::CanPlayback(Attrib::Gen::speech &event_attribs) {
    if (GetHistory().GetCount(event_attribs.SpeechID()) == 0) {
        return true;
    }

    static Attrib::Gen::speechtune tune(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
    if (!tune.IsValid()) {
        tune.ChangeWithDefault(0);
    }

    float sim_prob = Sim::GetRandom().SimRandom_FloatRange(1.0f);
    float prob_offset = tune.PriorityWeight() * event_attribs.priority();
    float calc_prob = mProbPlayback + prob_offset;
    return sim_prob <= mProbPlayback;
}

int Manager::FlushSpeechForActor(EAXCharacter *actor) {
    if (actor == nullptr) {
        return 0;
    }

    int num_flushed = 0;
    for (int ndx = 0; ndx < NUM_ELEMENTS(mEvents); ++ndx) {
        if (mEvents[ndx].size() > 0) {
            SchedSpchEvents::iterator i = mEvents[ndx].begin();
            while (i != mEvents[ndx].end()) {
                ScheduledSpeechEvent *this_event = *i;
                if ((this_event->actor != nullptr) && (this_event->actor == actor)) {
                    this_event->actor = nullptr;
                    num_flushed++;
                } else {
                    ++i;
                }
            }
        }
    }

    if ((mCurrentEvent != nullptr) && (mCurrentEvent->actor == actor)) {
        mCurrentEvent->actor = nullptr;
    }

    GameSpeech *gamespeech = static_cast<GameSpeech *>(m_SpeechModule[COPSPEECH_MODULE]);
    ScheduledSpeechEvent *curr_event = gamespeech->GetCurrentEvent();
    if ((curr_event != nullptr) && (curr_event->actor != nullptr) && (curr_event->actor == actor)) {
        curr_event->actor = nullptr;
    }
    return num_flushed;
}

void Manager::CalcProbPlayback() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }
    float basic_prob = 0.0f;
    if ((ai->GetPursuitDuration() >= ai->GetTune().SpeechDropoffRamp().x) && (ai->GetPursuitDuration() <= ai->GetTune().SpeechDropoffRamp().y)) {
        basic_prob = 1.0f - (ai->GetPursuitDuration() - ai->GetTune().SpeechDropoffRamp().x) /
                                (ai->GetTune().SpeechDropoffRamp().y - ai->GetTune().SpeechDropoffRamp().x);
    } else if (ai->GetPursuitDuration() < ai->GetTune().SpeechDropoffRamp().x) {
        basic_prob = 1.0f;
    } else if (ai->GetPursuitDuration() > ai->GetTune().SpeechDropoffRamp().y) {
        basic_prob = 0.0f;
    }
    mProbPlayback = basic_prob;
}

}; // namespace Speech
