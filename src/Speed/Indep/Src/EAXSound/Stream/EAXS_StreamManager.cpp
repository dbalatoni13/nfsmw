#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"

#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"
#include "Speed/Indep/Src/EAXSound/EAXSoundTypes.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"

#define COPSPEECH_STREAMBUFFERSIZE 49152
#define MUSIC_STREAMBUFFERSIZE 98304
#define NISSFX_STREAMBUFFERSIZE 65536
#define STREAMBLOCK_SIZE 32768

int INCREASE_MUSICSTREAM_BLOCKS = 0;
int INCREASE_NISSFXSTRM_BLOCKS = 0;
EAXS_StreamManager *gpEAXS_StrmMgr = nullptr;

Sound::SongInfoList Songs;

bool IsWorldDataStreaming(unsigned int strmhandle);
void AssignAudioStreamHandle(unsigned int strmhandle);

bool IsWorldDataStreaming(unsigned int strmhandle) {
    bool streaming = false;

    if (strmhandle != 0) {
        unsigned int poolbase = reinterpret_cast<unsigned int>(gAudioMemoryManager.m_pMemoryPoolMem);
        unsigned int poolend = poolbase + static_cast<unsigned int>(gAudioMemoryManager.m_memoryPoolSize);

        if (strmhandle > poolbase && strmhandle < poolend) {
            if (TheCarLoader.IsLoadingInProgress() != 0 || TheTrackStreamer.IsLoadingInProgressNonRepeatable()) {
                streaming = true;
            }
        }
    } else if (TheCarLoader.IsLoadingInProgress() != 0 || TheTrackStreamer.IsLoadingInProgressNonRepeatable()) {
        streaming = true;
    }

    return streaming;
}

void AssignAudioStreamHandle(unsigned int realstrmhandle) {
    unsigned int nStartAudioMemPool;
    unsigned int nEndAudioMemPool;

    nStartAudioMemPool = reinterpret_cast<unsigned int>(*(void *volatile *)&gAudioMemoryManager.m_pMemoryPoolMem);
    nEndAudioMemPool = nStartAudioMemPool + static_cast<unsigned int>(gAudioMemoryManager.m_memoryPoolSize);
    if (realstrmhandle > nStartAudioMemPool && realstrmhandle < nEndAudioMemPool) {
        unsigned int buffstart;
        unsigned int buffsize;
        buffstart = realstrmhandle;
        buffsize = nEndAudioMemPool;
        nEndAudioMemPool = realstrmhandle;
    }
}

EAXS_StreamManager::EAXS_StreamManager() {
    this->m_nNumStreamsAdded = 0;
    this->m_nWDRCount = 0;
    this->m_bIsWDRStreaming = false;
    this->m_bWasWDRStreaming = false;
    this->m_nTickStartZoneChange = 0;

    for (int i = 0; i < 4; i++) {
        this->m_pStrmCh[i] = nullptr;
    }
}

EAXS_StreamManager::~EAXS_StreamManager() {
    for (int i = 0; i < 4; i++) {
        if (this->m_pStrmCh[i] != nullptr) {
            delete this->m_pStrmCh[i];
            this->m_pStrmCh[i] = nullptr;
        }
    }
}

void EAXS_StreamManager::InitializeStreams(eGAMEMODE gamemode) {
    gpEAXS_StrmMgr = this;

    switch (gamemode) {
        case SNDGM_SPLITSCREEN:
            this->m_pStrmCh[STYPE_MUSIC] = new ("AUD:PF MUSIC Stream Channel", __LINE__) EAXS_StreamChannel();
            this->m_pStrmCh[STYPE_MUSIC]->InitParams(this);
            this->m_pStrmCh[STYPE_MUSIC]->InitChannel(8, 32,
                                                      MUSIC_STREAMBUFFERSIZE + INCREASE_MUSICSTREAM_BLOCKS * STREAMBLOCK_SIZE,
                                                      STYPE_MUSIC);

            this->m_pStrmCh[STYPE_NISSFX] = new ("AUD:NIS/SFX Stream Channel", __LINE__) EAXS_StreamChannel();
            this->m_pStrmCh[STYPE_NISSFX]->InitParams(this);
            this->m_pStrmCh[STYPE_NISSFX]->InitChannel(8, 32,
                                                       NISSFX_STREAMBUFFERSIZE + INCREASE_NISSFXSTRM_BLOCKS * STREAMBLOCK_SIZE,
                                                       STYPE_NISSFX);
            break;

        case SNDGM_FREEROAM:
        case SNDGM_RACE:
        case SNDGM_FRONTEND:
        default:
            this->m_pStrmCh[STYPE_COPSPEECH] = new ("AUD:Speech Stream Channel", __LINE__) EAXS_StreamChannel();
            this->m_pStrmCh[STYPE_COPSPEECH]->InitParams(this);
            this->m_pStrmCh[STYPE_COPSPEECH]->InitChannel(8, 32, COPSPEECH_STREAMBUFFERSIZE, STYPE_COPSPEECH);

            this->m_pStrmCh[STYPE_MUSIC] = new ("AUD:PF MUSIC Stream Channel", __LINE__) EAXS_StreamChannel();
            this->m_pStrmCh[STYPE_MUSIC]->InitParams(this);
            this->m_pStrmCh[STYPE_MUSIC]->InitChannel(8, 32,
                                                      MUSIC_STREAMBUFFERSIZE + INCREASE_MUSICSTREAM_BLOCKS * STREAMBLOCK_SIZE,
                                                      STYPE_MUSIC);

            this->m_pStrmCh[STYPE_NISSFX] = new ("AUD:NIS/SFX Stream Channel", __LINE__) EAXS_StreamChannel();
            this->m_pStrmCh[STYPE_NISSFX]->InitParams(this);
            this->m_pStrmCh[STYPE_NISSFX]->InitChannel(8, 32,
                                                       NISSFX_STREAMBUFFERSIZE + INCREASE_NISSFXSTRM_BLOCKS * STREAMBLOCK_SIZE,
                                                       STYPE_NISSFX);
            break;
    }
}

EAXS_StreamChannel *EAXS_StreamManager::GetStreamChannel(int nchannel) {
    if (nchannel < 4) {
        return this->m_pStrmCh[nchannel];
    }
    return nullptr;
}

void EAXS_StreamManager::AddStreamChannel(EAXS_StreamChannel *pstrmchannel, eSTRMTYPE strmtype) {
    if (this->m_pStrmCh[strmtype] == nullptr) {
        this->m_pStrmCh[strmtype] = pstrmchannel;
    }
}

void EAXS_StreamManager::RemoveStreamChannel(eSTRMTYPE strmtype) {}
