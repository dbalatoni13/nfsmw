#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"

EAXS_StreamChannel::EAXS_StreamChannel() {}

void EAXS_StreamChannel::InitParams(EAXS_StreamManager *pstrmmgr) {
    this->m_pStrmMgr = pstrmmgr;
    this->m_SChP.StrmState = STRM_UNINIT;
    this->m_SChP.StrmType = STYPE_NONE;
    for (int n = 0; n < 16; n++) {
        this->m_SChP.StrmOffsets[n] = -1;
        this->m_SChP.StrmStartTicks[n] = 0;
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
    this->DestroyStream();
}

void EAXS_StreamChannel::SetupBigFileStuff(const char *filename, long offset) {
    this->m_nBigFileOffset = 0;
    bStrCpy(this->m_StreamFileName, filename);
    this->m_nStreamOffset = offset;
}

// STRIPPED
void EAXS_StreamChannel::PostStreamRequest() {}

// STRIPPED
void EAXS_StreamChannel::UnwrapStreamRequest() {}

int EAXS_StreamChannel::AddToStrmReq(const char *filename, long offset, int holdtime) {
    this->SetupBigFileStuff(filename, offset);
    return this->AddToStream(this->m_pStrmFileName, this->m_nStreamOffset, holdtime);
}

// STRIPPED
int EAXS_StreamChannel::PlayStrmReq(const char *filename, long offset) {
    return 0;
}

int EAXS_StreamChannel::InitChannel(int maxChunks, int maxRequests, int buffersize, eSTRMTYPE strmtype) {
    int nhandle = this->Create(maxChunks, maxRequests, buffersize);
    if (nhandle < 0) {
        this->m_SChP.StrmState = STRM_UNINIT;
        this->m_SChP.StrmType = STYPE_NONE;
    } else {
        this->m_SChP.StrmType = strmtype;
        this->m_SChP.StrmState = STRM_READY;
        this->m_pStrmMgr->AddStreamChannel(this, strmtype);
    }
    return nhandle;
}

// STRIPPED
void EAXS_StreamChannel::SetMinStarveTime(int minstarvetime) {}

void EAXS_StreamChannel::ProcessTrackStreamerOn() {
    this->m_bIsTrackStreamerOn = true;
}

void EAXS_StreamChannel::ProcessTrackStreamerOff() {
    this->m_bIsTrackStreamerOn = false;
}

// STRIPPED
int EAXS_StreamChannel::ProcessStreamRequests(bool bExecuteRequests) {
    return 0;
}

// STRIPPED
int EAXS_StreamChannel::GetStrmStatus(SNDSTREAMSTATUS *sss) {
    return 0;
}

// STRIPPED
eSTRMTYPE EAXS_StreamChannel::GetStreamType() {
    return STYPE_NONE;
}

// STRIPPED
eSTRMSTATE EAXS_StreamChannel::GetStreamState() {
    return STRM_UNINIT;
}
