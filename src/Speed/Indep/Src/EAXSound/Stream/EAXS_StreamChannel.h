//
//
#ifndef STREAMCHANNEL_H
#define STREAMCHANNEL_H

#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"
#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"
#include "snd/sndo.h"

// Decl: 6
enum eSTRMSTATE {
    STRM_UNINIT = -1,
    STRM_READY = 0,
    STRM_QUEUING = 1,
    STRM_QUEUED = 2,
    STRM_PLAYING = 3,
};

// total size: 0xFC
// Decl: 15
struct StreamChannelParams {
    eSTRMTYPE StrmType;   // offset 0x0, size 0x4, Decl: 16
    eSTRMSTATE StrmState; // offset 0x4, size 0x4, Decl: 17

    int StrmHandle;                  // offset 0x8, size 0x4, Decl: 19
    int StrmRequests;                // offset 0xC, size 0x4, Decl: 20
    int NumStrmParts;                // offset 0x10, size 0x4, Decl: 21
    int MinStarveTime;               // offset 0x14, size 0x4, Decl: 22
    int BufferSize;                  // offset 0x18, size 0x4, Decl: 23
    int DataRate;                    // offset 0x1C, size 0x4, Decl: 24
    int StrmOffsets[16];             // offset 0x20, size 0x40, Decl: 25
    unsigned int StrmStartTicks[16]; // offset 0x60, size 0x40, Decl: 26
    char StrmFileName[64];           // offset 0xA0, size 0x40, Decl: 27
    char *StrmFile;                  // offset 0xE0, size 0x4, Decl: 28
    SNDPLAYOPTS StrmPlayOpts;        // offset 0xE4, size 0x18
};

// total size: 0x178
// Decl: 43
class EAXS_StreamChannel : public SndStrmWrapper {
  public:
    EAXS_StreamChannel();

    void InitParams(EAXS_StreamManager *pstrmmgr);

    virtual ~EAXS_StreamChannel();

    int CreateStreamChannel();

    int InitChannel(const int maxChunks, const int maxRequests, const int buffersize, eSTRMTYPE strmtype); // Decl: 54

    virtual int InitChannel(const int maxChunks, const int maxRequests, char *pmem, const int buffersize, eSTRMTYPE strmtype) {
        return 0;
    }

    int PlayStrmReq(const char *filename, long int offset);

    int AddToStrmReq(const char *filename, long int offset, int holdtime);

    int ProcessStreamRequests(bool bExecuteRequests);

    virtual void ProcessTrackStreamerOn();

    virtual void ProcessTrackStreamerOff();

    // Decl: 67
    int GetMinStarveTime() {
        return this->m_SChP.MinStarveTime;
    }

    void SetMinStarveTime(int minstarvetime);

    void SetupBigFileStuff(const char *filename, long int offset);

    void PostStreamRequest();

    void UnwrapStreamRequest();

    eSTRMTYPE GetStreamType();

    eSTRMSTATE GetStreamState();

    int GetStrmStatus(SNDSTREAMSTATUS *sss);

    virtual void SetQueueReqInitiated(bool binit) {} // Decl: 80

    EAXS_StreamManager *m_pStrmMgr; // offset 0x20, size 0x4, Decl: 82
    StreamChannelParams m_SChP;     // offset 0x24, size 0xFC, Decl: 83

  protected:
    int m_nBigFileOffset; // offset 0x120, size 0x4, Decl: 86
    int m_nStreamOffset;  // offset 0x124, size 0x4, Decl: 87

  private:
    char m_StreamFileName[64]; // offset 0x128, size 0x40, Decl: 90

    const char *m_pStrmFileName; // offset 0x168, size 0x4, Decl: 94

    bool m_bPlayPosted;        // offset 0x16C, size 0x1, Decl: 97
    bool m_bIsTrackStreamerOn; // offset 0x170, size 0x1, Decl: 98
};

#endif
