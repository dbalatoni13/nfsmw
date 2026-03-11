#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"

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
