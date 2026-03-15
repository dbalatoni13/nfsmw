#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"

EAXS_StreamManager *gpEAXS_StrmMgr = nullptr;

bool IsWorldDataStreaming(unsigned int strmhandle) {
    bool bStreamBlock = false;
    unsigned int poolStart = reinterpret_cast<unsigned int>(gAudioMemoryManager.GetMemPoolMem());
    unsigned int poolEnd = poolStart + static_cast<unsigned int>(gAudioMemoryManager.GetMemoryPoolSize());
    int loadingPhase = TheTrackStreamer.GetLoadingPhase();

    if ((strmhandle == 0 || (poolStart < strmhandle && strmhandle < poolEnd)) &&
        (TheCarLoader.IsLoadingInProgress() != 0 || loadingPhase != 0)) {
        bStreamBlock = true;
    }
    return bStreamBlock;
}

EAXS_StreamManager::EAXS_StreamManager() {
    m_nNumStreamsAdded = 0;
    m_nWDRCount = 0;
    m_bIsWDRStreaming = false;
    m_bWasWDRStreaming = false;
    m_nTickStartZoneChange = 0;
    for (int n = 0; n < 4; n++) {
        m_pStrmCh[n] = nullptr;
    }
}

EAXS_StreamManager::~EAXS_StreamManager() {
    for (int n = 0; n < 4; n++) {
        if (m_pStrmCh[n] != nullptr) {
            delete m_pStrmCh[n];
            m_pStrmCh[n] = nullptr;
        }
    }
}

void EAXS_StreamManager::InitializeStreams(eGAMEMODE gamemode) {
    gpEAXS_StrmMgr = this;

    if (static_cast<int>(gamemode) < 0 || static_cast<int>(gamemode) < 3 || gamemode != SNDGM_SPLITSCREEN) {
        EAXS_StreamChannel *speech = new EAXS_StreamChannel();
        m_pStrmCh[0] = speech;
        speech->InitParams(this);
        speech->InitChannel(8, 0x20, 0xC000, STYPE_COPSPEECH);
    }

    EAXS_StreamChannel *music = new EAXS_StreamChannel();
    m_pStrmCh[1] = music;
    music->InitParams(this);
    music->InitChannel(8, 0x20, 0x28000, STYPE_MUSIC);

    EAXS_StreamChannel *nissfx = new EAXS_StreamChannel();
    m_pStrmCh[2] = nissfx;
    nissfx->InitParams(this);
    nissfx->InitChannel(8, 0x20, 0x20000, STYPE_NISSFX);
}

EAXS_StreamChannel *EAXS_StreamManager::GetStreamChannel(int nchannel) {
    if (nchannel < 4) {
        return m_pStrmCh[nchannel];
    }
    return nullptr;
}

void EAXS_StreamManager::AddStreamChannel(EAXS_StreamChannel *pstrmchannel, eSTRMTYPE strmtype) {
    if (m_pStrmCh[strmtype] != nullptr) {
        return;
    }
    m_pStrmCh[strmtype] = pstrmchannel;
}

void EAXS_StreamManager::RemoveStreamChannel(eSTRMTYPE strmtype) {}

void AssignAudioStreamHandle(unsigned int realstrmhandle) {
    unsigned int nStartAudioMemPool = reinterpret_cast<unsigned int>(gAudioMemoryManager.GetMemPoolMem());
    asm volatile("cmplw %0, %1" : : "r"(realstrmhandle), "r"(nStartAudioMemPool));
}
