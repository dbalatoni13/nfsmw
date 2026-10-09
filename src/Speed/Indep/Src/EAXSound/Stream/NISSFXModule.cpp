#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_MomentStrm.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_NISStream.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/NISSFXModule.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audiosystem.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"

int DBGPRNT_NIS = 0; // Decl: 30

namespace Speech {

char *SED_NISSFX::m_tempCharPtr = nullptr;        // Decl: 35
CLUMP_IDX_FILE *SED_NISSFX::m_clumpIdx = nullptr; // Decl: 36
char *SED_NISSFX::m_csisData = nullptr;           // Decl: 37
int SED_NISSFX::m_channel = 0;                    // Decl: 38
char *SED_NISSFX::m_eventDat = nullptr;           // Decl: 39
bool SED_NISSFX::m_dataIsLoaded = false;

// TODO not initializer list? (on PC)
SED_NISSFX::SED_NISSFX()
    : m_moduleIsInitted(false), //
      m_speechCycle(0),         //
      m_paused(false) {
    this->mLoadState.clear();
    if (this->m_pSFXOBJ_NISStream != nullptr) {
        static_cast<SFXObj_NISStream *>(this->m_pSFXOBJ_NISStream)->NISActivityDone();
    }
    this->m_strm = nullptr;
}

SED_NISSFX::~SED_NISSFX() {
    if (this->m_strm != nullptr) {
        this->m_strm->Stop();
    }
    if (this->m_bankHeaders != nullptr) {
        gAudioMemoryManager.FreeMemory(this->m_bankHeaders);
        this->m_bankHeaders = nullptr;
    }
    if (m_eventDat != nullptr) {
        gAudioMemoryManager.FreeMemory(m_eventDat);
        m_eventDat = nullptr;
    }
    if (this->m_speechBanks != nullptr) {
        gAudioMemoryManager.FreeMemory(this->m_speechBanks);
        this->m_speechBanks = nullptr;
        this->m_numBanks = 0;
    }
    if (m_csisData != nullptr) {
        Csis::System::Unsubscribe(m_csisData);
        this->FreeMemory(m_csisData);
        m_csisData = nullptr;
    }
    m_dataIsLoaded = false;
    if (this->m_SyncObject.qsObject != nullptr) {
        gAudioMemoryManager.FreeMemory(this->m_SyncObject.qsObject);
        this->m_SyncObject.qsObject = nullptr;
    }
    this->mLoadState.clear();
    if (this->m_strm != nullptr) {
        gpEAXS_StrmMgr->RemoveStreamChannel(STYPE_NISSFX);
        this->m_strm = nullptr;
    }
    if (this->m_SyncObject.qsObject != nullptr) {
        this->m_SyncObject.qsObject->lock = false;
    }
}

void SED_NISSFX::Init(int channel) {
    if (!IsNISAudioEnabled) {
        return;
    }

    m_channel = channel;

    const Attrib::Collection *localizedCollection = g_pEAXSound->GetAttributes().nissfxstreams().GetCollection();
    Attrib::Gen::audiosystem *atr = new Attrib::Gen::audiosystem(localizedCollection, 0, nullptr);

    this->m_strm = gpEAXS_StrmMgr->GetStreamChannel(2);
    this->m_filename = atr->BIGPath();

    Attrib::StringKey evtfile(atr->EVTPath());
    Attrib::StringKey csifile(atr->CSIPath());
    Attrib::StringKey idxfile(atr->IDXPath());
    int thisobj;

    int nfilesize = bFileSize(evtfile.GetString());
    m_eventDat = nfilesize > 0 ? gAudioMemoryManager.AllocateMemoryChar(nfilesize, "AUD: SED_NISSFX events", false) : nullptr;

    if (m_eventDat != nullptr) {
        this->mLoadState.push_back(0);
        AddQueuedFile(m_eventDat, evtfile.GetString(), 0, nfilesize, LoadingCallback, reinterpret_cast<intptr_t>(this), nullptr);
    }

    nfilesize = bFileSize(idxfile.GetString());
    m_tempCharPtr = nfilesize > 0 ? static_cast<char *>(bMalloc(nfilesize, nullptr, 0, 0x1040)) : nullptr;

    if (m_tempCharPtr != nullptr) {
        this->mLoadState.push_back(1);
        AddQueuedFile(m_tempCharPtr, idxfile.GetString(), 0, nfilesize, LoadingCallback, reinterpret_cast<intptr_t>(this), nullptr);
    }

    nfilesize = bFileSize(csifile.GetString());
    m_csisData = nfilesize > 0 ? gAudioMemoryManager.AllocateMemoryChar(nfilesize, "AUD: SED_NISSFX CSIS data", false) : nullptr;

    if (m_csisData != nullptr) {
        this->mLoadState.push_back(2);
        AddQueuedFile(m_csisData, csifile.GetString(), 0, nfilesize, LoadingCallback, reinterpret_cast<intptr_t>(this), nullptr);
    }
}

void SED_NISSFX::LoadingCallback(int param, int error_status) {
    int numbanktypes;
    int numbanks;
    SED_NISSFX *obj = reinterpret_cast<SED_NISSFX *>(param);

    if (!obj->mLoadState.empty()) {
        int loadstate = obj->mLoadState.front();
        switch (loadstate) {
            case 0:
                break;
            case 1: {
                m_clumpIdx = reinterpret_cast<CLUMP_IDX_FILEtag *>(m_tempCharPtr);
                numbanktypes = m_clumpIdx->numtypes;
                for (int i = 0; i < numbanktypes; i++) {
                    obj->m_numBanks += m_clumpIdx->numbanks[i];
                }
                break;
            }
            case 2:
                m_dataIsLoaded = true;
                Manager::Init2();
                break;
            default:
                break;
        }
        obj->mLoadState.pop_front();
    }
}

void SED_NISSFX::LoadBanks() {
    if (!IsNISAudioEnabled) {
        return;
    }
    int type = 1;
    int structsize = sizeof(SPEECH_BANK);
    this->m_speechBanks =
        static_cast<SPEECH_BANK *>(gAudioMemoryManager.AllocateMemory(this->m_numBanks * structsize, "AUD: Game speech banks", false));
    int i = 0;
    while (i < this->m_numBanks) {
        Manager::LoadSpeechBank(m_clumpIdx, type, i, &this->m_speechBanks[i]);
        i++;
    }
    Manager::AddHeaders(&this->m_bankHeaders, this->m_speechBanks, this->m_numBanks, this);
    bFree(m_clumpIdx);
    m_clumpIdx = nullptr;
}

int SED_NISSFX::TestSentenceRuleCallback(int eventID, int ruleID, int parmValue) {
    return 0;
}

int SED_NISSFX::SetSentenceRuleCallback(int eventID, int ruleID, int parmValue) {
    return 0;
}

SPCHType_EventRuleResult SED_NISSFX::EventRuleCallback(int eventID) {
    return kSPCH_EventRule_OK;
}

bool SED_NISSFX::QueStream(eNISSFX_TYPE stream_type, void (*callback)(), bool trigger_play_after_callback) {
    int numreq;
    if (this->m_SyncObject.qsObject != nullptr) {
        if (stream_type > STRM_NIS_BUSTED) {
            return true;
        }
        this->m_strm->PurgeStream();
        this->FreeMemory(this->m_SyncObject.qsObject);
        this->m_SyncObject.qsObject = nullptr;
    }
    if (this->m_strm->IsPlaying() && ((stream_type == STRM_NIS_RACE_START) || (stream_type == STRM_SFX_MOMENT))) {
        if (stream_type != STRM_NIS_BUSTED) {
            this->m_strm->Stop();
        }
        if ((stream_type == STRM_SFX_MOMENT) && (this->m_pSFXOBJ_NISStream != nullptr)) {
            static_cast<SFXObj_NISStream *>(this->m_pSFXOBJ_NISStream)->NISActivityDone();
        }
    }
    this->m_SyncObject.callback = callback;
    this->m_SyncObject.qsObject = nullptr;
    this->m_SyncObject.id = stream_type;
    this->m_SyncObject.handle = -1;
    this->m_bIsStreamQueued = false;
    if (SPCH_Play(2) == 0) {
        return false;
    }
    return true;
}

unsigned int SED_NISSFX::SampleRequestCallback(SPCHType_SampleRequestData *data) {
    int rval;
    if (IsNISAudioEnabled && (this->m_strm != nullptr)) {
        this->m_SyncObject.qsObject = static_cast<SpeechSampleData *>(this->AllocateMemory(sizeof(SpeechSampleData), "NIS SampleData"));
        this->m_SyncObject.holdtime = -1;
        this->m_SyncObject.qsObject->cached = false;
        this->m_SyncObject.qsObject->ready = false;
        this->m_SyncObject.qsObject->dataoffset = this->GetBankOffset(data->bankNum) + data->sampleOffset;
        rval = this->m_strm->AddToStrmReq(this->GetFilename(), this->m_SyncObject.qsObject->dataoffset, -1);
        this->m_SyncObject.handle = rval;
    }
    return 1;
}

bool SED_NISSFX::PlayStream(int stream_id) {
    if (this->m_strm != nullptr) {
        this->m_strm->ModifyHold(this->m_SyncObject.handle, 0);
        this->m_bIsStreamQueued = false;
        if (this->m_SyncObject.qsObject != nullptr) {
            gAudioMemoryManager.FreeMemory(this->m_SyncObject.qsObject);
            this->m_SyncObject.qsObject = nullptr;
        }
        return true;
    }
    return false;
}

void SED_NISSFX::ClearStream() {
    if (this->m_SyncObject.qsObject != nullptr) {
        this->m_SyncObject.holdtime = 0;
        this->FreeMemory(this->m_SyncObject.qsObject);
        this->m_SyncObject.qsObject = nullptr;
    }
}

void SED_NISSFX::Update() {
    if (this->m_SyncObject.id < STRM_NIS_RACE_START) {
        return;
    }

    if (!IsNISAudioEnabled) {
        return;
    }

    switch (this->m_SyncObject.id) {
        case STRM_SFX_MOMENT: {
            SFXObj_MomentStrm *sfxmoment = static_cast<SFXObj_MomentStrm *>(this->m_pSFXOBJ_Moment);
            if (sfxmoment != nullptr) {
                int vol = sfxmoment->GetDMixOutput(sfxmoment->VolSlot, DMX_VOL) >> 8;
                this->m_strm->SetVol(vol, true);
                if (sfxmoment->m_IsPositioned) {
                    this->m_strm->SetAz(sfxmoment->GetDMixOutput(0, DMX_AZIM));
                } else {
                    this->m_strm->SetAz(0);
                }
            }
            break;
        }
        case STRM_NIS_RACE_START:
            if (this->m_pSFXOBJ_NISStream != nullptr) {
                int vol = this->m_pSFXOBJ_NISStream->GetDMixOutput(1, DMX_VOL) >> 8;
                this->m_strm->SetVol(vol, true);
                this->m_strm->SetAz(0);
            }
            break;
        case STRM_NIS_BUSTED:
            if (this->m_pSFXOBJ_NISStream != nullptr) {
                int vol = this->m_pSFXOBJ_NISStream->GetDMixOutput(2, DMX_VOL) >> 8;
                this->m_strm->SetVol(vol, true);
                this->m_strm->SetAz(0);
            }
            break;
        default:
            this->m_strm->SetVol(100, true);
            this->m_strm->SetAz(0);
            break;
    }

    if (this->m_SyncObject.holdtime != -1) {
        this->m_SyncObject.holdtime -= static_cast<int>(SndBase::m_fDeltaTime * 1000.0f);
        if (this->m_SyncObject.holdtime < 0) {
            this->m_SyncObject.holdtime = 0;
            this->FreeMemory(this->m_SyncObject.qsObject);
            this->m_SyncObject.qsObject = nullptr;
        }
    } else if (this->m_strm->GetTimeBuffered() > 0) {
        if (this->m_SyncObject.qsObject != nullptr) {
            this->m_bIsStreamQueued = true;
            if ((this->m_SyncObject.callback != nullptr) && (this->m_SyncObject.id >= STRM_NIS_RACE_START) &&
                ((this->m_SyncObject.id <= STRM_NIS_BUSTED) || (this->m_SyncObject.id == STRM_SFX_MOMENT)) && (this->m_SyncObject.holdtime <= 0)) {
                this->m_SyncObject.callback();
            }
            this->m_SyncObject.callback = nullptr;
            this->FreeMemory(this->m_SyncObject.qsObject);
            this->m_SyncObject.qsObject = nullptr;
        }
    }

    if (this->m_SyncObject.qsObject == nullptr) {
        if (!this->m_strm->IsPlaying()) {
            this->m_SyncObject.id = STRM_NONE;
            if (this->m_pSFXOBJ_NISStream != nullptr) {
                static_cast<SFXObj_NISStream *>(this->m_pSFXOBJ_NISStream)->NISActivityDone();
            }
            this->mLastEventTimestamp = WorldTimer;
        }
    }
}

}; // namespace Speech
