#ifndef __PATHSNDH__
#define __PATHSNDH__ // Decl: 2

#include "path/IPathToReal.h"
#include "snd/sndo.h"
#include "path/IPathToSnd.h"

namespace Path {

// total size: 0x4
// Decl: 22
class PathToSnd : public IPathToSnd {
  public:
    void *operator new(PATH_SIZE_T size);
    void operator delete(void *ptr);

    PathToSnd();
    // Overrides: IPathToSnd (virtual)
    ~PathToSnd();
    int CreateStreamTrack(IPathTrack **ppPathTrack, int maxRequests, float bufferTime, int byteRate);
    int CreateBankTrack(IPathTrack **ppPathTrack, int maxsubbanks);
    void GetDefaultPlayOpts(void *playopts);
};

// total size: 0x108
// Decl: 53
struct PathTrackSnd : public IPathTrack {
    PathTrackSnd();
    ~PathTrackSnd();

    // Overrides: IPathTrack
    void UpdateStatus() { mUpdateStatus = 1; } // Decl: 57
    int GetPathStatus(PATHSTATUS *psps);
    void StreamCache(char *, int) {} // Decl: 59
    int ReadyForNewRequest();
    int TimeBuffered();
    int Pause(int pause);
    int ModifyHold(int) { return -1; } // Decl: 64
    void SetName(const char *inName);
    int GetVolume();
    int SetVolume(int volume);
    int SetDryLevel(int level);
    int SetFXSendLevel(int bus, int level);
    int GetPitchMult();
    int SetPitchMult(int pitchmult);
    int GetStretchMult();
    int SetStretchMult(int stretchmult);

  protected:
    char mName[16];            // offset 0x6C, size 0x10, Decl: 77
    char mPath[128];           // offset 0x7C, size 0x80, Decl: 78
    SNDPLAYOPTS *mSndPlayOpts; // offset 0xFC, size 0x4, Decl: 79
    int mPitchMult;            // offset 0x100, size 0x4, Decl: 80
    char *mBuffer;             // offset 0x104, size 0x4, Decl: 81
};

// total size: 0x114
// Decl: 86
class PathTrackSndStream : public PathTrackSnd {
  public:
    void *operator new(size_t size);
    void operator delete(void *ptr);

    PathTrackSndStream(int maxrequests);
    ~PathTrackSndStream();

    virtual int AttachStreamInstance(int streamhandle, char *buffer);
    virtual int DetachStreamInstance(char *&buffer);

    // Overrides: IPathTrack
    int SetVolume(int volume);
    int SetDryLevel(int drylevel);
    int SetFXSendLevel(int bus, int level);
    int SetPitchMult(int pitchmult);
    int SetStretchMult(int stretchmult);
    int ModifyHold(int holdtime);
    void SetFilePath(char *path);
    void CheckStatus();
    int TimeRemaining(int request);
    void StreamCache(char *pcache, int cachesize);
    int Play(int node, unsigned int offset, int, int holdtime, unsigned int duration);
    int Stop();
    int GetPathStatus(PATHSTATUS *psps);

  private:
    PATHQUEUEINFO *mRequests; // offset 0x108, size 0x4, Decl: 163
    char *mCacheMem;          // offset 0x10C, size 0x4, Decl: 164
    int mCacheSize;           // offset 0x110, size 0x4, Decl: 165
};

// total size: 0x124
// Decl: 170
class PathTrackSndBank : public PathTrackSnd {
  public:
    void *operator new(PATH_SIZE_T size);
    void operator delete(void *ptr);

    PathTrackSndBank(int maxsubbanks);
    ~PathTrackSndBank();

    virtual int AttachSubBank(int subbanknum, int bankhandle);

    virtual int DetachSubBank(int subbanknum);

    // Overrides: IPathTrack
    PATHSUBBANKSTATUS *GetSubBankPtr(int subbanknum);
    PATHSUBBANKSTATUS *GetAvailSubBankPtr();
    int AddSubBank(int subbanknum, void *pbank);
    int AddSubBankDone(int subbanknum);
    int RemoveSubBank(int subbanknum);
    int DetachSubBankHeader(int subbanknum, int status);
    void CheckStatus();
    int TimeRemaining(int request);
    int SetVolume(int volume);
    int SetDryLevel(int drylevel);
    int SetFXSendLevel(int bus, int level);
    int SetPitchMult(int pitchmult);
    int SetStretchMult(int stretchmult);
    int Play(int node, unsigned int offset, int beat, int, unsigned int duration);
    int Stop();

  private:
    int mPatchNum;          // offset 0x108, size 0x4, Decl: 248
    int mFXBus;             // offset 0x10C, size 0x4, Decl: 249
    PATHQUEUEINFO mRequest; // offset 0x110, size 0x14, Decl: 250
};

}; // namespace Path

#endif
