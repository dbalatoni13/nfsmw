#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"

EAXS_StreamChannel::EAXS_StreamChannel() {}

void EAXS_StreamChannel::InitParams(EAXS_StreamManager *pstrmmgr) {
    m_pStrmMgr = pstrmmgr;
    m_SChP.StrmType = STYPE_NONE;
    m_SChP.StrmState = STRM_UNINIT;
    m_SChP.StrmHandle = -1;
    m_SChP.StrmRequests = 0;
    m_SChP.NumStrmParts = 0;
    m_SChP.MinStarveTime = 0;
    m_SChP.BufferSize = 0;
    m_SChP.DataRate = 0;
    for (int n = 0; n < 16; n++) {
        m_SChP.StrmOffsets[n] = 0;
        m_SChP.StrmStartTicks[n] = 0;
    }
    m_nBigFileOffset = 0;
    m_nStreamOffset = 0;
    m_bPlayPosted = false;
    m_bIsTrackStreamerOn = false;
}

EAXS_StreamChannel::~EAXS_StreamChannel() {
    DestroyStream();
}

void EAXS_StreamChannel::SetupBigFileStuff(const char *filename, long offset) {
    m_nBigFileOffset = 0;
    bStrCpy(m_StreamFileName, filename);
    m_nStreamOffset = offset;
}

int EAXS_StreamChannel::AddToStrmReq(const char *filename, long offset, int holdtime) {
    SetupBigFileStuff(filename, offset);
    return AddToStream(m_pStrmFileName, m_nStreamOffset, holdtime);
}

int EAXS_StreamChannel::InitChannel(int maxChunks, int maxRequests, int buffersize, eSTRMTYPE strmtype) {
    int nhandle = Create(maxChunks, maxRequests, buffersize);
    m_SChP.StrmHandle = nhandle;
    m_SChP.StrmType = strmtype;
    m_SChP.StrmState = STRM_READY;
    m_SChP.StrmRequests = maxRequests;
    m_SChP.BufferSize = buffersize;
    return nhandle;
}

void EAXS_StreamChannel::ProcessTrackStreamerOn() {
    m_bIsTrackStreamerOn = true;
}

void EAXS_StreamChannel::ProcessTrackStreamerOff() {
    m_bIsTrackStreamerOn = false;
}
