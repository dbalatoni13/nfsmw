#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"

EAXS_StreamChannel::EAXS_StreamChannel() {}

void EAXS_StreamChannel::InitParams(EAXS_StreamManager *pstrmmgr) {
    m_pStrmMgr = pstrmmgr;
    m_SChP.StrmState = STRM_UNINIT;
    m_SChP.StrmType = STYPE_NONE;
    for (int n = 0; n < 16; n++) {
        m_SChP.StrmOffsets[n] = -1;
        m_SChP.StrmStartTicks[n] = 0;
    }
    m_SChP.StrmRequests = 0;
    m_SChP.DataRate = 0;
    bStrCat(m_SChP.StrmFileName, "", "");
    m_SChP.StrmFile = m_SChP.StrmFileName;
    bStrCat(m_StreamFileName, "", "");
    m_pStrmFileName = m_StreamFileName;
    m_nBigFileOffset = 0;
    m_nStreamOffset = 0;
    m_bPlayPosted = false;
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

void EAXS_StreamChannel::ProcessTrackStreamerOn() {
    m_bIsTrackStreamerOn = true;
}

void EAXS_StreamChannel::ProcessTrackStreamerOff() {
    m_bIsTrackStreamerOn = false;
}

int EAXS_StreamChannel::InitChannel(int maxChunks, int maxRequests, int buffersize, eSTRMTYPE strmtype) {
    int nhandle = Create(maxChunks, maxRequests, buffersize);
    if (nhandle < 0) {
        m_SChP.StrmState = STRM_UNINIT;
        m_SChP.StrmType = STYPE_NONE;
    } else {
        m_SChP.StrmType = strmtype;
        m_SChP.StrmState = STRM_READY;
        m_pStrmMgr->AddStreamChannel(this, strmtype);
    }
    return nhandle;
}
