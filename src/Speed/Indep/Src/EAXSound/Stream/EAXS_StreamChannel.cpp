#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"

#include "Speed/Indep/bWare/Inc/Strings.hpp"

EAXS_StreamChannel::EAXS_StreamChannel() {}

void EAXS_StreamChannel::InitParams(EAXS_StreamManager *pstrmmgr) {
    this->m_pStrmMgr = pstrmmgr;
    this->m_SChP.StrmState = STRM_UNINIT;
    this->m_SChP.StrmType = STYPE_NONE;

    for (int i = 0; i < 16; i++) {
        this->m_SChP.StrmOffsets[i] = -1;
        this->m_SChP.StrmStartTicks[i] = 0;
    }

    this->m_SChP.StrmRequests = 0;
    this->m_SChP.DataRate = 0;
    bStrCat(this->m_SChP.StrmFileName, "", "");
    this->m_SChP.StrmFile = this->m_SChP.StrmFileName;
    bStrCat(this->m_StreamFileName, "", "");
    this->m_pStrmFileName = this->m_StreamFileName;
    this->m_nBigFileOffset = 0;
    this->m_nStreamOffset = 0;
    this->m_bPlayPosted = false;
}

EAXS_StreamChannel::~EAXS_StreamChannel() {
    DestroyStream();
}

void EAXS_StreamChannel::SetupBigFileStuff(const char *filename, long offset) {
    this->m_nBigFileOffset = 0;
    bStrCpy(this->m_StreamFileName, filename);
    this->m_nStreamOffset = offset;
}

int EAXS_StreamChannel::AddToStrmReq(const char *filename, long offset, int holdtime) {
    SetupBigFileStuff(filename, offset);
    return AddToStream(this->m_pStrmFileName, this->m_nStreamOffset, holdtime);
}

int EAXS_StreamChannel::InitChannel(const int maxChunks, const int maxRequests, const int buffersize,
                                    eSTRMTYPE strmtype) {
    int ret = Create(maxChunks, maxRequests, buffersize);

    if (ret < 0) {
        this->m_SChP.StrmState = STRM_UNINIT;
        this->m_SChP.StrmType = STYPE_NONE;
    } else {
        this->m_SChP.StrmState = STRM_READY;
        this->m_SChP.StrmType = strmtype;
        this->m_pStrmMgr->AddStreamChannel(this, strmtype);
    }
    return ret;
}

void EAXS_StreamChannel::ProcessTrackStreamerOn() {
    this->m_bIsTrackStreamerOn = true;
}

void EAXS_StreamChannel::ProcessTrackStreamerOff() {
    this->m_bIsTrackStreamerOn = false;
}
