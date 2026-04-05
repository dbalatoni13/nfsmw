#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"

extern int INCREASE_MUSICSTREAM_BLOCKS;
extern int INCREASE_NISSFXSTRM_BLOCKS;

EAXS_StreamManager *gpEAXS_StrmMgr = nullptr;

bool IsWorldDataStreaming(unsigned int strmhandle) {
    bool bStreamBlock = false;
    if (strmhandle != 0) {
        unsigned int poolStart = reinterpret_cast<unsigned int>(gAudioMemoryManager.GetMemPoolMem());
        unsigned int poolEnd = poolStart + static_cast<unsigned int>(gAudioMemoryManager.GetMemoryPoolSize());
        if (strmhandle <= poolStart || strmhandle >= poolEnd) {
            return bStreamBlock;
        }
    }
    if (TheCarLoader.IsLoadingInProgress() != 0) {
        bStreamBlock = true;
    } else {
        bool isStreaming = TheTrackStreamer.GetLoadingPhase() != 0;
        if (isStreaming) {
            bStreamBlock = true;
        }
    }
    return bStreamBlock;
}

void AssignAudioStreamHandle(unsigned int realstrmhandle) {
    register unsigned int nEndAudioMemPool asm("r0");
    unsigned int nStartAudioMemPool;
    nStartAudioMemPool = reinterpret_cast<unsigned int>(gAudioMemoryManager.GetMemoryPoolStart());
    if (false) {
        nEndAudioMemPool = nStartAudioMemPool + static_cast<unsigned int>(gAudioMemoryManager.GetMemoryPoolSize());
        {
            unsigned int buffstart = nStartAudioMemPool;
            unsigned int buffsize = nEndAudioMemPool - buffstart;
            (void)buffsize;
        }
        (void)nEndAudioMemPool;
    }
    asm volatile("cmplw %0, %1" : : "r"(realstrmhandle), "r"(nStartAudioMemPool));
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
        if (m_pStrmCh[n]) {
            delete m_pStrmCh[n];
            m_pStrmCh[n] = nullptr;
        }
    }
}

void EAXS_StreamManager::InitializeStreams(eGAMEMODE gamemode) {
    gpEAXS_StrmMgr = this;

    if (static_cast<int>(gamemode) >= 0) {
        if (static_cast<int>(gamemode) > 2) {
            if (gamemode == SNDGM_SPLITSCREEN) {
                m_pStrmCh[1] = new (__FILE__, __LINE__) EAXS_StreamChannel;
                m_pStrmCh[1]->InitParams(this);
                m_pStrmCh[1]->InitChannel(8, 0x20, INCREASE_MUSICSTREAM_BLOCKS * 0x8000 + 0x18000, STYPE_MUSIC);

                m_pStrmCh[2] = new (__FILE__, __LINE__) EAXS_StreamChannel;
                m_pStrmCh[2]->InitParams(this);
                m_pStrmCh[2]->InitChannel(8, 0x20, INCREASE_NISSFXSTRM_BLOCKS * 0x8000 + 0x10000, STYPE_NISSFX);
                return;
            }
        }
    }

    m_pStrmCh[0] = new (__FILE__, __LINE__) EAXS_StreamChannel;
    m_pStrmCh[0]->InitParams(this);
    m_pStrmCh[0]->InitChannel(8, 0x20, 0xC000, STYPE_COPSPEECH);

    m_pStrmCh[1] = new (__FILE__, __LINE__) EAXS_StreamChannel;
    m_pStrmCh[1]->InitParams(this);
    m_pStrmCh[1]->InitChannel(8, 0x20, INCREASE_MUSICSTREAM_BLOCKS * 0x8000 + 0x18000, STYPE_MUSIC);

    m_pStrmCh[2] = new (__FILE__, __LINE__) EAXS_StreamChannel;
    m_pStrmCh[2]->InitParams(this);
    m_pStrmCh[2]->InitChannel(8, 0x20, INCREASE_NISSFXSTRM_BLOCKS * 0x8000 + 0x10000, STYPE_NISSFX);
}

EAXS_StreamChannel *EAXS_StreamManager::GetStreamChannel(int nchannel) {
    if (nchannel < 4) {
        return m_pStrmCh[nchannel];
    }
    return nullptr;
}

void EAXS_StreamManager::AddStreamChannel(EAXS_StreamChannel *pstrmchannel, eSTRMTYPE strmtype) {
    if (m_pStrmCh[strmtype]) {
        return;
    }
    m_pStrmCh[strmtype] = pstrmchannel;
}

void EAXS_StreamManager::RemoveStreamChannel(eSTRMTYPE strmtype) {}
