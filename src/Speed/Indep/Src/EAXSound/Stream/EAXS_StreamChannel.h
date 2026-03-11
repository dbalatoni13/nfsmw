#ifndef EAXSOUND_STREAM_EAXS_STREAMCHANNEL_H
#define EAXSOUND_STREAM_EAXS_STREAMCHANNEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"

enum eSTRMTYPE {
    STYPE_NONE = -1,
    STYPE_COPSPEECH = 0,
    STYPE_MUSIC = 1,
    STYPE_NISSFX = 2,
    STYPE_AUX = 3,
    STYPE_MAX = 4,
};

enum eSTRMSTATE {
    STRM_UNINIT = -1,
    STRM_READY = 0,
    STRM_QUEUING = 1,
    STRM_QUEUED = 2,
    STRM_PLAYING = 3,
};

struct StreamChannelParams {
    eSTRMTYPE StrmType;
    eSTRMSTATE StrmState;
    int StrmHandle;
    int StrmRequests;
    int NumStrmParts;
    int MinStarveTime;
    int BufferSize;
    int DataRate;
    int StrmOffsets[16];
    unsigned int StrmStartTicks[16];
    char StrmFileName[64];
    char *StrmFile;
    SNDPLAYOPTS StrmPlayOpts;
};

char *bStrCat(char *dest, const char *src1, const char *src2);

struct EAXS_StreamManager;

struct EAXS_StreamChannel : public SndStrmWrapper {
    EAXS_StreamChannel();
    void InitParams(EAXS_StreamManager *pstrmmgr);
    virtual ~EAXS_StreamChannel();
    virtual int InitChannel(int maxChunks, int maxRequests, char *pmem, int buffersize, eSTRMTYPE strmtype) {
        return 0;
    }
    virtual void ProcessTrackStreamerOn();
    virtual void ProcessTrackStreamerOff();
    virtual void SetQueueReqInitiated(bool binit) {}
    int InitChannel(int maxChunks, int maxRequests, int buffersize, eSTRMTYPE strmtype);
    void SetupBigFileStuff(const char *filename, long offset);
    int AddToStrmReq(const char *filename, long offset, int holdtime);

    EAXS_StreamManager *m_pStrmMgr;
    StreamChannelParams m_SChP;

  protected:
    int m_nBigFileOffset;
    int m_nStreamOffset;

  private:
    char m_StreamFileName[64];
    char *m_pStrmFileName;
    bool m_bPlayPosted;
    bool m_bIsTrackStreamerOn;
};

#endif
