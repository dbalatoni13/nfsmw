//
//
//
//
//
//
//
//
//
//
//
//
#ifndef _STRM_H_
#define _STRM_H_ // Decl: 14

#include "snd/sndo.h"

#define STRM_OK 0 // Decl: 22

#define STRM_ERR_NOT_INITIALISED -1 // Decl: 24
#define STRM_ERR_BUFFER_IN_USE -2   // Decl: 25
#define STRM_ERR_UNKNOWN -3         // Decl: 26
#define STRM_ERR_FILE_NOT_FOUND -4  // Decl: 27
#define STRM_ERR_BAD_PARM -5        // Decl: 28
#define STRM_ERR_STRM_IN_USE -6     // Decl: 29

#define STRM_ALMOST_DONE_TIME 100 // Decl: 33

// total size: 0x20
// Decl: 42
class SndStrmWrapper {
  public:
    SndStrmWrapper(); // Decl: 46

    SndStrmWrapper(const int maxChunks, const int maxRequests, const int buffersize);

    int Create(const int maxChunks, const int maxRequests, const int buffersize); // Decl: 53

    int CreateStream(const int maxChunks, const int maxRequests, char *pmem, const int buffersize,
                     void *pplayopts); // Decl: 55

    ~SndStrmWrapper(); // Decl: 57

    void DestroyStream(); // Decl: 59

    bool IsPlaying(); // Decl: 63

    int Stop(); // Decl: 66

    int Play(const char *filename, long offset); // Decl: 71

    int AddToStream(const char *filename, long offset, int holdtime); // Decl: 76

    int AddToStream(int holdtime, void *paddr, int length, int offset); // Decl: 79

    int ModifyHold(int sndrequesthandle, int holdtime); // Decl: 82

    int SetVol(int vol, bool bramp); // Decl: 84

    int SetAz(int Azimuth); // Decl: 87

    int RampVol(int vol, int time); // Decl: 90

    int GetVol(); // Decl: 95

    int SetFXLevel(int bus, int level); // Decl: 98

    int SetDryLevel(int level); // Decl: 101

    int SetPitchMult(int pitchmult); // Decl: 104

    int SetTimeMult(int timemult); // Decl: 107

    int SetLowPass(int lowpass); // Decl: 110

    int GetStatus(SNDSTREAMSTATUS *sss); // Decl: 113

    int GetRequestStatus(int sndrequesthandle, SNDREQUESTSTATUS *psrs); // Decl: 116

    int GetTimeBuffered(); // Decl: 119

    int GetTimeRemaining(); // Decl: 121
    int GetCurrentTime();   // Decl: 122

    bool AlmostDone();  // Decl: 126
    int IsStrmQueued(); // Decl: 127

    void SetStreamGreedyLevel(int nbytes); // Decl: 130

    void Pause();  // Decl: 137
    void Resume(); // Decl: 138

    int PurgeStream();                                // Decl: 140
    int GetStrmHandle() {}                            // Decl: 141
    char *GetBuffer() {}                              // Decl: 142
    int GetRealStrmHandle() {}                        // Decl: 143
    int GetBufferSize() {}                            // Decl: 144
    void AssignSTRMHANDLE(unsigned int strmhandle) {} // Decl: 145
    int m_StreamID;                                   // offset 0x0, size 0x4, Decl: 146

  private:
    int m_RealStreamBuffer;        // offset 0x4, size 0x4, Decl: 149
    unsigned int m_RealStrmHandle; // offset 0x8, size 0x4, Decl: 150
    char *m_buffer;                // offset 0xC, size 0x4, Decl: 151
    int m_BufferSize;              // offset 0x10, size 0x4, Decl: 152
    int m_handle;                  // offset 0x14, size 0x4, Decl: 153
    int m_vol;                     // offset 0x18, size 0x4, Decl: 154
    float m_fvol;                  // offset 0x1C, size 0x4, Decl: 155
};

#endif
