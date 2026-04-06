#ifndef EAXSOUND_STREAM_SNDSTRMWRAPPER_H
#define EAXSOUND_STREAM_SNDSTRMWRAPPER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct SNDPLAYOPTS {
    signed char vol;              // offset 0x0, size 0x1
    signed char bend;             // offset 0x1, size 0x1
    signed char keynum;           // offset 0x2, size 0x1
    signed char velocity;         // offset 0x3, size 0x1
    signed char drylevel;         // offset 0x4, size 0x1
    signed char fxlevel0;         // offset 0x5, size 0x1
    char pad[2];                  // offset 0x6, size 0x2
    unsigned short azimuth;       // offset 0x8, size 0x2
    short elevation;              // offset 0xA, size 0x2
    unsigned short pitchmult;     // offset 0xC, size 0x2
    unsigned short timemult;      // offset 0xE, size 0x2
    unsigned short tempomult;     // offset 0x10, size 0x2
    unsigned short pad2;          // offset 0x12, size 0x2
    unsigned short lowpasscutoff; // offset 0x14, size 0x2
    unsigned short highpasscutoff; // offset 0x16, size 0x2
};

struct SNDSTREAMSTATUS {
    int outstandingrequests;      // offset 0x0, size 0x4
    int currentrequest;           // offset 0x4, size 0x4
    int timebuffered;             // offset 0x8, size 0x4
};

struct SNDREQUESTSTATUS {
    int state;                    // offset 0x0, size 0x4
    int currenttime;              // offset 0x4, size 0x4
    int timetoend;                // offset 0x8, size 0x4
    int timebuffered;             // offset 0xC, size 0x4
};

extern "C" int SNDSTRM_create(SNDPLAYOPTS *pspo, int maxrequests, int maxchunks, void *pmem, int memsize);
extern "C" int SNDSTRM_destroy(int sndstreamhandle);
extern "C" int SNDSTRM_queuefile(int sndstreamhandle, int holdtime, const char *filename, int offset);
extern "C" int SNDSTRM_queuemem(int sndstreamhandle, int holdtime, void *paddr, int offset);
extern "C" int SNDSTRM_purge(int sndStrmHandle);
extern "C" int SNDSTRM_status(int sndstreamhandle, SNDSTREAMSTATUS *psss);
extern "C" int SNDSTRM_requeststatus(int sndrequesthandle, SNDREQUESTSTATUS *psrs);
extern "C" int SNDSTRM_modifyhold(int sndrequesthandle, int holdtime);
extern "C" int SNDSTRM_overhead(int maxrequests, int maxchunks);
extern "C" int SNDSTRM_lowpass(int sndstreamhandle, int lowpasscutoff);
extern "C" int SNDSTRM_pitchmult(int sndstreamhandle, int pitchmult);
extern "C" int SNDSTRM_setgreedylevel(int sndstreamhandle, int greedylevel);
extern "C" int SNDSTRM_setvol(int sndStrmHandle, int sourceChannel, float volume);
extern "C" int SNDSTRM_setazimuth(int sndStrmHandle, int sourceChannel, float azimuth);
extern "C" int SNDSTRM_autovol(int sndStrmHandle, int time, int targetVol);
extern "C" void SNDplaysetdef(SNDPLAYOPTS *pspo);
extern "C" void SNDSYS_entercritical();
extern "C" void SNDSYS_leavecritical();

char *bStrCpy(char *dst, const char *src);
unsigned int bGetTicker();
void bSyncTaskRun();

struct SndStrmWrapper {
    int m_StreamID; // offset 0x00, size 0x4

    SndStrmWrapper();
    ~SndStrmWrapper();
    int Create(int maxChunks, int maxRequests, int buffersize);
    int CreateStream(const int maxChunks, const int maxRequests, char *pmem, const int buffersize, void *paddr);
    bool IsPlaying();
    int Stop();
    int AddToStream(const char *filename, long offset, int holdtime);
    int AddToStream(int holdtime, void *paddr, int length, int offset);
    int ModifyHold(int sndrequesthandle, int holdtime);
    int SetVol(int vol, bool bramp);
    int SetAz(int Azimuth);
    int RampVol(int vol, int time);
    int SetLowPass(int lowpass);
    int GetStatus(SNDSTREAMSTATUS *sss);
    int GetRequestStatus(int sndrequesthandle, SNDREQUESTSTATUS *psrs);
    int GetTimeBuffered();
    int GetTimeRemaining();
    int GetCurrentTime();
    bool AlmostDone();
    void Pause();
    void Resume();
    int PurgeStream();
    void DestroyStream();

    int GetStrmHandle() { return m_handle; }
    char *GetBuffer() { return m_buffer; }

  private:
    int m_RealStreamBuffer;       // offset 0x4, size 0x4
    unsigned int m_RealStrmHandle; // offset 0x8, size 0x4
    char *m_buffer;               // offset 0xC, size 0x4
    int m_BufferSize;             // offset 0x10, size 0x4
    int m_handle;                 // offset 0x14, size 0x4
    int m_vol;                    // offset 0x18, size 0x4
    float m_fvol;                 // offset 0x1C, size 0x4
};

#endif
