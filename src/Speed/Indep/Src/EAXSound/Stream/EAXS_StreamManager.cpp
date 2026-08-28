#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"

int INCREASE_MUSICSTREAM_BLOCKS = 0; // size: 0x4, address: 0x80417904, Decl: 53
int INCREASE_NISSFXSTRM_BLOCKS = 0;  // size: 0x4, address: 0x80417908, Decl: 54

#define USE_MANAGED_STREAMS 0      // Decl: 58
#define EAXS_MAXCHANNELREQUESTS 8; // Decl: 59

#define COPSPEECH_REQUESTS 8               // Decl: 176
#define COPSPEECH_MAXCHUNKS 32             // Decl: 177
#define COPSPEECH_BUFFERSIZE 4 * 12 * 1024 // Decl: 178

#define MUSIC_REQUESTS 8               // Decl: 180
#define MUSIC_MAXCHUNKS 32             // Decl: 181
#define MUSIC_BUFFERSIZE 4 * 24 * 1024 // Decl: 182

#define NISSFX_REQUESTS 8               // Decl: 184
#define NISSFX_MAXCHUNKS 32             // Decl: 185
#define NISSFX_BUFFERSIZE 4 * 16 * 1024 // Decl: 186

extern int IsSoundEnabled; // size: 0x4, Decl: 219

EAXS_StreamManager *gpEAXS_StrmMgr = nullptr; // size: 0x4, address: 0x8041790C, Decl: 221

// Decl: 226
bool IsWorldDataStreaming(uintptr_t strmhandle) {
    bool bStreamBlock = false;
    if (strmhandle == 0) {
        if (TheCarLoader.IsLoadingInProgress() != 0) {
            bStreamBlock = true;
        } else if (TheTrackStreamer.IsLoadingInProgressNonRepeatable()) {
            bStreamBlock = true;
        }
    } else {
        uintptr_t nStartAudioMemPool = reinterpret_cast<uintptr_t>(gAudioMemoryManager.GetMemoryPoolStart());
        uintptr_t nEndAudioMemPool = nStartAudioMemPool + gAudioMemoryManager.GetMemoryPoolSize();
        if (strmhandle > nStartAudioMemPool && strmhandle < nEndAudioMemPool) {
            if (TheCarLoader.IsLoadingInProgress() != 0) {
                bStreamBlock = true;
            } else if (TheTrackStreamer.IsLoadingInProgressNonRepeatable()) {
                bStreamBlock = true;
            }
        }
    }
    return bStreamBlock;
}

int GN_HACK_STREAM_TYPE = -1; // size: 0x4, address: 0xFFFFFFFF, Decl: 253

// UNSOLVED, the empty branch doesn't get emitted
void AssignAudioStreamHandle(uintptr_t realstrmhandle) {
    uintptr_t nStartAudioMemPool = reinterpret_cast<uintptr_t>(gAudioMemoryManager.GetMemoryPoolStart());
    uintptr_t nEndAudioMemPool = nStartAudioMemPool + gAudioMemoryManager.GetMemoryPoolSize();
    if (realstrmhandle == nStartAudioMemPool) {
        uintptr_t buffstart = nStartAudioMemPool;
        uintptr_t buffsize = nEndAudioMemPool - buffstart;
    }
}

EAXS_StreamManager::EAXS_StreamManager() {
    this->m_nNumStreamsAdded = 0;
    this->m_nWDRCount = 0;
    this->m_bIsWDRStreaming = false;
    this->m_bWasWDRStreaming = false;
    this->m_nTickStartZoneChange = 0;
    for (int n = 0; n < 4; n++) {
        this->m_pStrmCh[n] = nullptr;
    }
}

EAXS_StreamManager::~EAXS_StreamManager() {
    for (int n = 0; n < 4; n++) {
        if (this->m_pStrmCh[n] != nullptr) {
            delete this->m_pStrmCh[n];
            this->m_pStrmCh[n] = nullptr;
        }
    }
}

void EAXS_StreamManager::InitializeStreams(eGAMEMODE gamemode) {
    gpEAXS_StrmMgr = this;

    switch (gamemode) {
        case SNDGM_FREEROAM:
        case SNDGM_RACE:
        case SNDGM_FRONTEND:
            break;
        case SNDGM_SPLITSCREEN:
            this->m_pStrmCh[1] = new ("AUD:PF MUSIC Stream Channel", 0) EAXS_StreamChannel;
            this->m_pStrmCh[1]->InitParams(this);
            this->m_pStrmCh[1]->InitChannel(MUSIC_REQUESTS, MUSIC_MAXCHUNKS, INCREASE_MUSICSTREAM_BLOCKS * 32768 + MUSIC_BUFFERSIZE, STYPE_MUSIC);

            this->m_pStrmCh[2] = new ("AUD:NIS/SFX Stream Channel", 0) EAXS_StreamChannel;
            this->m_pStrmCh[2]->InitParams(this);
            this->m_pStrmCh[2]->InitChannel(NISSFX_REQUESTS, NISSFX_MAXCHUNKS, INCREASE_NISSFXSTRM_BLOCKS * 32768 + NISSFX_BUFFERSIZE, STYPE_NISSFX);
            return;
    }

    this->m_pStrmCh[0] = new ("AUD:Speech Stream Channel", 0) EAXS_StreamChannel;
    this->m_pStrmCh[0]->InitParams(this);
    this->m_pStrmCh[0]->InitChannel(COPSPEECH_REQUESTS, COPSPEECH_MAXCHUNKS, COPSPEECH_BUFFERSIZE, STYPE_COPSPEECH);

    this->m_pStrmCh[1] = new ("AUD:PF MUSIC Stream Channel", 0) EAXS_StreamChannel;
    this->m_pStrmCh[1]->InitParams(this);
    this->m_pStrmCh[1]->InitChannel(MUSIC_REQUESTS, MUSIC_MAXCHUNKS, INCREASE_MUSICSTREAM_BLOCKS * 32768 + MUSIC_BUFFERSIZE, STYPE_MUSIC);

    this->m_pStrmCh[2] = new ("AUD:NIS/SFX Stream Channel", 0) EAXS_StreamChannel;
    this->m_pStrmCh[2]->InitParams(this);
    this->m_pStrmCh[2]->InitChannel(NISSFX_REQUESTS, NISSFX_MAXCHUNKS, INCREASE_NISSFXSTRM_BLOCKS * 32768 + NISSFX_BUFFERSIZE, STYPE_NISSFX);
}

EAXS_StreamChannel *EAXS_StreamManager::GetStreamChannel(int nchannel) {
    if (nchannel < 4) {
        return this->m_pStrmCh[nchannel];
    }
    return nullptr;
}

// STRIPPED
void EAXS_StreamManager::UpdateTrackStreamerTransition(bool bstate) {}

// STRIPPED
void EAXS_StreamManager::UpdateStreams() {}

void EAXS_StreamManager::AddStreamChannel(EAXS_StreamChannel *pstrmchannel, eSTRMTYPE strmtype) {
    if (this->m_pStrmCh[strmtype] == nullptr) {
        this->m_pStrmCh[strmtype] = pstrmchannel;
    }
}

void EAXS_StreamManager::RemoveStreamChannel(eSTRMTYPE strmtype) {}

// STRIPPED
void EAXS_StreamManager::ThinkAboutIt() {}

// STRIPPED
void EAXS_StreamManager::SetupWorldDataTransitionData() {}
