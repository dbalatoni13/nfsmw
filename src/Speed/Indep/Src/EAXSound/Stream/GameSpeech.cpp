#include "Speed/Indep/Src/EAXSound/Stream/GameSpeech.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyCellCallComplete.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyCellCallStarted.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/Speech/SpeechCache.h"

int ENFORCE_IO_MODE = 0;                   // Decl: 46
float TRACKSTREAMER_BACKLOG_THRESH = 1.0f; // Decl: 47

namespace Speech {

char *GameSpeech::m_tempCharPtr = nullptr;           // Decl: 58
CLUMP_IDX_FILEtag *GameSpeech::m_clumpIdx = nullptr; // Decl: 59
char *GameSpeech::m_csisData = nullptr;              // Decl: 60
int GameSpeech::m_channel = 0;                       // Decl: 61
char *GameSpeech::m_eventDat = nullptr;              // Decl: 62

// TODO temporary A124 hack. figure it out
#ifndef EA_BUILD_A124
GameSpeech::GameSpeech()
    : mLoadState(),            //
      m_speechCycle(0),        //
      m_pendingList(),         //
      m_currEvent(nullptr),    //
      m_currEventTime(0),      //
      m_currEventSpeakerID(0), //
      m_Chirper(nullptr),      //
      m_ChirpParams() {
    this->mLoadState.clear();
    if (this->m_strm == nullptr && IsSpeechEnabled) {
        this->m_pendingList.reserve(7);
    }
    this->m_ChirpParams.Vol = 0;
    this->m_ChirpParams.Az = 0;
    this->m_ChirpParams.Pitch = 0x1000;
}

GameSpeech::~GameSpeech() {
    this->ClearCompletedRequests();
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
        gAudioMemoryManager.FreeMemory(m_csisData);
        m_csisData = nullptr;
    }
    this->mLoadState.clear();
    if (this->m_strm != nullptr) {
        gpEAXS_StrmMgr->RemoveStreamChannel(STYPE_COPSPEECH);
        this->m_strm = nullptr;
    }
    m_pendingList.clear();
    this->m_pSFXOBJ_Speech = nullptr;

    delete this->m_Chirper;
    this->m_Chirper = nullptr;
}
#endif

void GameSpeech::Init(int channel) {
    short errval = 0;
    m_channel = channel;

    Attrib::Gen::audiosystem *localizedCollection = new Attrib::Gen::audiosystem(g_pEAXSound->GetLocalAttr());

    this->m_strm = gpEAXS_StrmMgr->GetStreamChannel(errval);
    this->m_filename = localizedCollection->BIGPath();

    Attrib::StringKey evtfile(localizedCollection->EVTPath());
    Attrib::StringKey csifile(localizedCollection->CSIPath());
    Attrib::StringKey idxfile(localizedCollection->IDXPath());
    int thisobj;
    int nfilesize = bFileSize(evtfile.GetString());
    m_eventDat = nfilesize > 0 ? gAudioMemoryManager.AllocateMemoryChar(nfilesize, "AUD: Game speech events", false) : nullptr;

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
    m_csisData = nfilesize > 0 ? gAudioMemoryManager.AllocateMemoryChar(nfilesize, "AUD: Game speech CSIS data", false) : nullptr;

    if (m_csisData != nullptr) {
        this->mLoadState.push_back(2);
        AddQueuedFile(m_csisData, csifile.GetString(), 0, nfilesize, LoadingCallback, reinterpret_cast<intptr_t>(this), nullptr);
    }
}

void GameSpeech::LoadingCallback(intptr_t param, int error_status) {
    int numbanktypes;
    int numbanks;
    GameSpeech *obj = reinterpret_cast<GameSpeech *>(param);

    if (!obj->mLoadState.empty()) {
        int loadstate = obj->mLoadState.front();
        switch (loadstate) {
            case 0:
                Manager::GetSpeechModule(COPSPEECH_MODULE)->SetFlag(0x80);
                break;
            case 1: {
                m_clumpIdx = reinterpret_cast<CLUMP_IDX_FILEtag *>(m_tempCharPtr);
                numbanktypes = m_clumpIdx->numtypes;
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

void GameSpeech::LoadBanks() {
    int type = 1;
    int structsize = sizeof(SPEECH_BANK);
    this->m_speechBanks =
        static_cast<SPEECH_BANK *>(gAudioMemoryManager.AllocateMemory(this->m_numBanks * structsize, "AUD: Game speech banks", false));
    for (int i = 0; i < this->m_numBanks; i++) {
        Manager::LoadSpeechBank(m_clumpIdx, type, i, &this->m_speechBanks[i]);
    }
    Manager::AddHeaders(&this->m_bankHeaders, this->m_speechBanks, this->m_numBanks, this);
    bFree(m_clumpIdx);
    m_clumpIdx = nullptr;
}

int GameSpeech::TestSentenceRuleCallback(int eventID, int ruleID, int parmValue) {
    return 0;
}

int GameSpeech::SetSentenceRuleCallback(int eventID, int ruleID, int parmValue) {
    return 0;
}

SPCHType_EventRuleResult GameSpeech::EventRuleCallback(int eventID) {
    return kSPCH_EventRule_OK;
}

static bool Unlocked(SpeechSampleData *data) {
    if (data->cached) {
        return data->lock == false;
    }

    gSpeechCache.TossSample(data);
    return true;
}

// TODO temporary A124 hack. figure it out
#ifndef EA_BUILD_A124
void GameSpeech::Update() {
    if (!this->TestFlag(2)) {
        this->UpdateChirps();
        if ((this->m_strm == nullptr) || !IsSpeechEnabled || (this->m_pSFXOBJ_Speech == nullptr)) {
            return;
        }

        int LowPassFilter = this->m_pSFXOBJ_Speech->GetDMixOutput(0xC, DMX_FREQ);
        this->m_strm->SetLowPass(LowPassFilter);

        SNDSTREAMSTATUS ss;
        this->m_strm->GetStatus(&ss);

        static int requests_in_queue = ss.outstandingrequests;
        static int req_timer = 0;

        if (ss.outstandingrequests > 0) {
            float fvol;
            SNDREQUESTSTATUS srs;
            this->SetFlag(8);
            requests_in_queue = ss.outstandingrequests;
            int err = this->m_strm->GetRequestStatus(ss.currentrequest, &srs);
            if (err >= 0) {
                SpeechSampleData *sample;
                if (this->m_pendingList.size() > 0) {
                    sample = this->m_pendingList.Find(ss.currentrequest);
                } else {
                    sample = nullptr;
                }
                switch (srs.state) {
                    case 3:
                        if (sample != nullptr) {
                            sample->t_play = WorldTimer;
                            sample->Unlock();
                        }
                        req_timer = 0;
                        break;
                    case 1:
                    case 0:
                        this->m_pSFXOBJ_Speech->SetDMIX_Input(2, this->TestFlag(0x20) ? 0x7FFF : 0);
                        break;
                    case 2: {
                        int nQVol;
                        this->m_currEventTime += srs.currenttime - req_timer;
                        req_timer = srs.currenttime;
                        if ((this->m_currEventTime > 3000) && !this->TestFlag(0x10)) {
                            Manager::NotifyEventCompletion(this->m_currEvent, false);
                            this->SetFlag(0x10);
                        }

                        if (this->TestFlag(0x20)) {
                            this->m_pSFXOBJ_Speech->SetDMIX_Input(2, 0x7FFF);
                            this->m_strm->SetAz(this->m_pSFXOBJ_Speech->GetDMixOutput(0, DMX_AZIM));
                        } else {
                            this->m_pSFXOBJ_Speech->SetDMIX_Input(2, 0);
                            this->m_strm->SetAz(0);
                        }

                        nQVol = this->GetVolForSpeaker(this->m_currEventSpeakerID);
                        this->m_pSFXOBJ_Speech->SetDMIX_Input(3, this->m_currEventClarity);
                        fvol = static_cast<float>(nQVol);
                        fvol *= 0.01f;
                        fvol *= 32767.0f;
                        this->m_strm->SetVol(this->m_enable ? static_cast<int>(fvol) : 0, false);
                        this->CheckNextEvent();
                        break;
                    }
                }
            }

            if (this->m_pendingList.size() > 0) {
                SpeechSampleVec::iterator i = std::remove_if(this->m_pendingList.begin(), this->m_pendingList.end(), Unlocked);
                this->m_pendingList.erase(i, m_pendingList.end());
            }
            this->mLastEventTimestamp = WorldTimer;
        } else if (ss.outstandingrequests == 0) {
            if (requests_in_queue > 0) {
                if (this->m_currEvent != nullptr) {
                    if (this->m_currEvent->finish_time == Timer(0)) {
                        this->m_currEvent->finish_time = WorldTimer;
                        if (!this->TestFlag(0x10)) {
                            Manager::NotifyEventCompletion(this->m_currEvent, false);
                            this->SetFlag(0x10);
                        }
                        Manager::NotifyEventCompletion(this->m_currEvent, true);
                        this->RadioChirp(1);
                    }

                    if (Manager::IsEventDead(this->m_currEvent) < 0.0f) {
                        requests_in_queue = ss.outstandingrequests;
                        this->mLastEventTimestamp = WorldTimer;
                        if (this->m_currEvent == nullptr) {
                            return;
                        }
                        delete this->m_currEvent;
                        this->m_currEvent = nullptr;
                        for (unsigned int i = 0; i < this->m_pendingList.size(); ++i) {
                            this->m_pendingList[i]->Unlock();
                        }
                        this->m_pendingList.clear();
                        this->ClearFlag(4);
                        this->ClearFlag(8);
                    } else {
                        this->SetFlag(4);
                        this->SetFlag(8);
                        this->CheckNextEvent();
                        return;
                    }
                } else {
                    requests_in_queue = 0;
                    this->ClearFlag(4);
                    this->ClearFlag(8);
                }
            } else {
                this->CheckNextEvent();
            }
        }
    }
}

void GameSpeech::CheckNextEvent() {
    ScheduledSpeechEvent *nextevent = Manager::GetNextEvent();

    if (nextevent != nullptr) {
        if (nextevent->priority <= 100) {
            if (!this->TestFlag(8)) {
                this->IssuePlayback(nextevent);
                this->RadioChirp(0);
            }
        } else {
            this->ReleaseResource();
            this->IssuePlayback(nextevent);
            this->RadioChirp(0);
        }
    }
}

void GameSpeech::IssuePlayback(ScheduledSpeechEvent *nextevent) {
    Attrib::Gen::speech speech(Manager::GetHashIDMap().GetHash(nextevent->ID), 0, nullptr);

    if (this->m_pSFXOBJ_Speech != nullptr) {
        float scale = static_cast<unsigned int>(speech.Clarity());
        scale *= 0.1f;
        scale *= 32767.0f;
        this->m_currEventClarity = static_cast<int>(scale);
    }

    {
        MNotifySpeechStatus(nextevent).Send(UCrc32("PlayStart"));
    }

    if ((nextevent != nullptr) && (nextevent->ID == kSPCH1_EventID_CellCall)) {
        MNotifyCellCallStarted().Post(UCrc32(UCRC32_Gameplay));
    }

    if (speech.Pan()) {
        this->SetFlag(0x20);
    } else {
        this->ClearFlag(0x20);
    }

    if (speech.RadioChirp()) {
        this->SetFlag(0x400);
    } else {
        this->ClearFlag(0x400);
    }

    unsigned char err_count = 0;
    for (short i = 0; i < nextevent->assoc_samples_count; ++i) {
        SpeechSampleData *stitch = nextevent->assoc_samples[i];
        if (stitch != nullptr) {
            bool stitch_hdr_check_passes = false;
            const unsigned int snd_chunk_hdr = 0x5343486c; // TODO magic
            if ((stitch->cached == false) || (*reinterpret_cast<const unsigned int *>(stitch->GetData()) == snd_chunk_hdr)) {
                stitch_hdr_check_passes = true;
            } else {
                err_count++;
            }

            if (stitch_hdr_check_passes) {
                stitch->t_load = WorldTimer;
                stitch->Lock();
                this->m_pendingList.push_back(stitch);
                if (stitch->cached) {
                    stitch->HSTRM = this->m_strm->AddToStream(0, stitch->GetData(), 0, 0);
                } else {
                    stitch->HSTRM = this->m_strm->AddToStrmReq(this->GetFilename(), stitch->dataoffset, 0);
                }
                this->SetFlag(8);
            }
        } else {
            err_count++;
        }
    }

    this->m_currEventTime = 0;
    this->ClearFlag(0x10);

    if (err_count) {
        {
            MNotifySpeechStatus(nextevent).Send(UCrc32(UCRC32_Gameplay));
        }
        if (nextevent->ID == kSPCH1_EventID_CellCall) {
            MNotifyCellCallComplete().Send(UCrc32(UCRC32_Gameplay));
        }
        delete nextevent;
        nextevent = nullptr;
    }

    this->m_currEvent = nextevent;
    Manager::Expire(this->m_currEvent);
    this->m_currEventSpeakerID = 0;
    if ((this->m_currEvent != nullptr) && (this->m_currEvent->actor != nullptr)) {
        this->m_currEventSpeakerID = static_cast<short>(this->m_currEvent->actor->GetSpeakerID());
    }
}

void GameSpeech::ClearCompletedRequests() {
    if ((this->m_strm != nullptr) && this->m_strm->IsPlaying()) {
        this->m_strm->Stop();
    }

    if (this->m_pendingList.size() > 0) {
        SpeechSampleVec::iterator i = std::remove_if(this->m_pendingList.begin(), this->m_pendingList.end(), Unlocked);
        this->m_pendingList.erase(i, this->m_pendingList.end());
        for (unsigned int i = 0; i < this->m_pendingList.size(); ++i) {
            SpeechSampleData *sample = this->m_pendingList[i];
            if (sample != nullptr) {
                sample->Unlock();
            }
        }
    }

    this->m_pendingList.clear();
    delete this->m_currEvent;
    this->m_currEvent = nullptr;
    this->ClearFlag(4);
    this->ClearFlag(8);
    this->ClearFlag(0x10);
}

#endif

void GameSpeech::ReleaseResource() {
    Module::ReleaseResource();
    this->ClearCompletedRequests();
}

unsigned int GameSpeech::SampleRequestCallback(SPCHType_SampleRequestData *data) {
    this->IssueSampleRequests();
    return 1;
}

void GameSpeech::IssueSampleRequests() {
    if (!IsSpeechEnabled) {
        return;
    }

    SampleReqList &requests = Manager::GetSampleRequests();
    SampleReqList::iterator i;
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

        if (sample != nullptr) {
            if (req.owner != nullptr) {
                req.owner->AddSample(sample, req.sample_index);
            } else if (!sample->cached) {
                gSpeechCache.TossSample(sample);
            }
        } else if (req.owner != nullptr) {
            sample = gSpeechCache.GetUncached(this, &data);
            if (gSpeechCache.IsCached(&data, false)) {
                req.owner->AddSample(sample, req.sample_index);
            }
        }

        i = requests.erase(i);
    }
}

void GameSpeech::RadioChirp(unsigned char type) {
    if ((this->m_pSFXOBJ_Speech != nullptr) && (this->m_Chirper == nullptr) && this->TestFlag(0x400)) {
        int stich_id;
        if (type > 0) {
            if (bRandom(1.0f) > 0.67f) {
                stich_id = bRandom(4);
            } else {
                stich_id = bRandom(15) + 7;
            }
        } else {
            stich_id = bRandom(6) + 4;
        }

        SND_Stich *chirpData = &g_pEAXSound->GetStichPlayer()->GetStich(STICH_TYPE_STATIC, stich_id);
        this->m_Chirper = new cStichWrapper(*chirpData);
        this->m_Chirper->Play(&this->m_ChirpParams);
    }
}

void GameSpeech::UpdateChirps() {
    if (this->m_Chirper != nullptr) {
        if (!this->m_Chirper->IsPlaying()) {
            delete this->m_Chirper;
            this->m_Chirper = nullptr;
        } else {
            this->m_ChirpParams.Vol = this->m_pSFXOBJ_Speech->GetDMixOutput(0xD, DMX_VOL);
            this->m_Chirper->Update(&this->m_ChirpParams);
        }
    }
}

int GameSpeech::GetVolForSpeaker(int id) {
    int vol = 0;
    if (this->m_pSFXOBJ_Speech != nullptr) {
        switch (id) {
            case 1:
                vol = this->m_pSFXOBJ_Speech->GetDMixOutput(11, DMX_VOL);
                break;
            case 2:
                vol = this->m_pSFXOBJ_Speech->GetDMixOutput(10, DMX_VOL);
                break;
            case 3:
                vol = this->m_pSFXOBJ_Speech->GetDMixOutput(3, DMX_VOL);
                break;
            case 4:
                vol = this->m_pSFXOBJ_Speech->GetDMixOutput(4, DMX_VOL);
                break;
            case 5:
                vol = this->m_pSFXOBJ_Speech->GetDMixOutput(5, DMX_VOL);
                break;
            case 6:
                vol = this->m_pSFXOBJ_Speech->GetDMixOutput(6, DMX_VOL);
                break;
            case 7:
                vol = this->m_pSFXOBJ_Speech->GetDMixOutput(7, DMX_VOL);
                break;
            case 8:
                vol = this->m_pSFXOBJ_Speech->GetDMixOutput(8, DMX_VOL);
                break;
            default:
                vol = this->m_pSFXOBJ_Speech->GetDMixOutput(0xe, DMX_VOL);
                if ((this->m_currEvent != nullptr) && (this->m_currEvent->ID == kSPCH1_EventID_CellCall) &&
                    (this->m_currEvent->GetData(nullptr) != nullptr)) {
                    Csis::CellCallStruct *data = static_cast<Csis::CellCallStruct *>(this->m_currEvent->GetData(nullptr));
                    if ((data != nullptr) && (data->cell_call_bucket == Csis::Type_cell_call_bucket_bucket_01) &&
                        (data->cell_call_number == Csis::Type_cell_call_number_call_07)) {
                        bClamp(vol += vol, 0, 0x7fff);
                    }
                }
                break;
        }
    }

    if (!this->TestFlag(0x200)) {
        return vol;
    }
    if (this->TestFlag(0x20)) {
        return vol;
    }
    return 0;
}

}; // namespace Speech
