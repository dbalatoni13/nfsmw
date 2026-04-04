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
    eSTRMTYPE StrmType;                // offset 0x0, size 0x4
    eSTRMSTATE StrmState;              // offset 0x4, size 0x4
    int StrmHandle;                    // offset 0x8, size 0x4
    int StrmRequests;                  // offset 0xC, size 0x4
    int NumStrmParts;                  // offset 0x10, size 0x4
    int MinStarveTime;                 // offset 0x14, size 0x4
    int BufferSize;                    // offset 0x18, size 0x4
    int DataRate;                      // offset 0x1C, size 0x4
    int StrmOffsets[16];               // offset 0x20, size 0x40
    unsigned int StrmStartTicks[16];   // offset 0x60, size 0x40
    char StrmFileName[64];             // offset 0xA0, size 0x40
    char *StrmFile;                    // offset 0xE0, size 0x4
    SNDPLAYOPTS StrmPlayOpts;          // offset 0xE4, size 0x18
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

    EAXS_StreamManager *m_pStrmMgr;    // offset 0x20, size 0x4
    StreamChannelParams m_SChP;        // offset 0x24, size 0xFC

  protected:
    int m_nBigFileOffset;              // offset 0x120, size 0x4
    int m_nStreamOffset;               // offset 0x124, size 0x4

  private:
    char m_StreamFileName[64];         // offset 0x128, size 0x40
    const char *m_pStrmFileName;       // offset 0x168, size 0x4
    bool m_bPlayPosted;                // offset 0x16C, size 0x1
    bool m_bIsTrackStreamerOn;         // offset 0x170, size 0x1
};

#endif
