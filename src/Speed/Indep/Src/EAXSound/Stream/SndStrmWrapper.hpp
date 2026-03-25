#ifndef EAXSOUND_STREAM_SNDSTRMWRAPPER_H
#define EAXSOUND_STREAM_SNDSTRMWRAPPER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct SNDPLAYOPTS {
    signed char vol;
    signed char bend;
    signed char keynum;
    signed char velocity;
    signed char drylevel;
    signed char fxlevel0;
    char pad[2];
    unsigned short azimuth;
    short elevation;
    unsigned short pitchmult;
    unsigned short timemult;
    unsigned short tempomult;
    unsigned short pad2;
    unsigned short lowpasscutoff;
    unsigned short highpasscutoff;
};

struct SNDSTREAMSTATUS {
    int outstandingrequests;
    int currentrequest;
    int timebuffered;
};

struct SNDREQUESTSTATUS {
    int state;
    int currenttime;
    int timetoend;
    int timebuffered;
};

extern "C" {
int SNDSTRM_create(SNDPLAYOPTS *pspo, int maxrequests, int maxchunks, void *pmem, int memsize);
int SNDSTRM_destroy(int sndstreamhandle);
int SNDSTRM_queuefile(int sndstreamhandle, int holdtime, const char *filename, int offset);
int SNDSTRM_queuemem(int sndstreamhandle, int holdtime, void *paddr, int offset);
int SNDSTRM_purge(int sndStrmHandle);
int SNDSTRM_status(int sndstreamhandle, SNDSTREAMSTATUS *psss);
int SNDSTRM_requeststatus(int sndrequesthandle, SNDREQUESTSTATUS *psrs);
int SNDSTRM_modifyhold(int sndrequesthandle, int holdtime);
int SNDSTRM_overhead(int maxrequests, int maxchunks);
int SNDSTRM_lowpass(int sndstreamhandle, int lowpasscutoff);
int SNDSTRM_pitchmult(int sndstreamhandle, int pitchmult);
int SNDSTRM_setgreedylevel(int sndstreamhandle, int greedylevel);
int SNDSTRM_setvol(int sndStrmHandle, int sourceChannel, float volume);
int SNDSTRM_setazimuth(int sndStrmHandle, int sourceChannel, float azimuth);
int SNDSTRM_autovol(int sndStrmHandle, int time, int targetVol);
void SNDplaysetdef(SNDPLAYOPTS *pspo);
void SNDSYS_entercritical();
void SNDSYS_leavecritical();
}

char *bStrCpy(char *dst, const char *src);
unsigned int bGetTicker();
void bSyncTaskRun();

struct SndStrmWrapper {
    int m_StreamID; // offset 0x00, size 0x4

    SndStrmWrapper();
    ~SndStrmWrapper();
    int Create(int maxChunks, int maxRequests, int buffersize);
    int CreateStream(int maxChunks, int maxRequests, char *pmem, int buffersize, void *paddr);
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
    int m_RealStreamBuffer;
    unsigned int m_RealStrmHandle;
    char *m_buffer;
    int m_BufferSize;
    int m_handle;
    int m_vol;
    float m_fvol;
};

#endif
