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
    // Overrides: IPathToSnd
    ~PathToSnd() override;
    int CreateStreamTrack(IPathTrack **ppPathTrack, int maxRequests, float bufferTime, int byteRate) override;
    int CreateBankTrack(IPathTrack **ppPathTrack, int maxsubbanks) override;
    void GetDefaultPlayOpts(void *playopts) override;
};

// total size: 0x108
// Decl: 53
struct PathTrackSnd : public IPathTrack {
    PathTrackSnd();
    ~PathTrackSnd() override;

    // Overrides: IPathTrack
    void UpdateStatus() override {} // Decl: 57
    int GetPathStatus(PATHSTATUS *psps) override;
    void StreamCache(char *, int) override {} // Decl: 59
    int ReadyForNewRequest() override;
    int TimeBuffered() override;
    int Pause(int pause) override;
    int ModifyHold(int) override {} // Decl: 64
    void SetName(const char *inName) override;
    int GetVolume() override;
    int SetVolume(int volume) override;
    int SetDryLevel(int level) override;
    int SetFXSendLevel(int bus, int level) override;
    int GetPitchMult() override;
    int SetPitchMult(int pitchmult) override;
    int GetStretchMult() override;
    int SetStretchMult(int stretchmult) override;

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
    ~PathTrackSndStream() override;

    virtual int AttachStreamInstance(int streamhandle, char *buffer);
    virtual int DetachStreamInstance(char *&buffer);

    // Overrides: IPathTrack
    int SetVolume(int volume) override;
    int SetDryLevel(int drylevel) override;
    int SetFXSendLevel(int bus, int level) override;
    int SetPitchMult(int pitchmult) override;
    int SetStretchMult(int stretchmult) override;
    int ModifyHold(int holdtime) override;
    void SetFilePath(char *path) override;
    void CheckStatus() override;
    int TimeRemaining(int request) override;
    void StreamCache(char *pcache, int cachesize) override;
    int Play(int node, unsigned int offset, int, int holdtime, unsigned int duration) override;
    int Stop() override;
    int GetPathStatus(PATHSTATUS *psps) override;

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
    ~PathTrackSndBank() override;

    virtual int AttachSubBank(int subbanknum, int bankhandle);

    virtual int DetachSubBank(int subbanknum);

    // Overrides: IPathTrack
    PATHSUBBANKSTATUS *GetSubBankPtr(int subbanknum) override;
    PATHSUBBANKSTATUS *GetAvailSubBankPtr() override;
    int AddSubBank(int subbanknum, void *pbank) override;
    int AddSubBankDone(int subbanknum) override;
    int RemoveSubBank(int subbanknum) override;
    int DetachSubBankHeader(int subbanknum, int status) override;
    void CheckStatus() override;
    int TimeRemaining(int request) override;
    int SetVolume(int volume) override;
    int SetDryLevel(int drylevel) override;
    int SetFXSendLevel(int bus, int level) override;
    int SetPitchMult(int pitchmult) override;
    int SetStretchMult(int stretchmult) override;
    int Play(int node, unsigned int offset, int beat, int, unsigned int duration) override;
    int Stop() override;

  private:
    int mPatchNum;          // offset 0x108, size 0x4, Decl: 248
    int mFXBus;             // offset 0x10C, size 0x4, Decl: 249
    PATHQUEUEINFO mRequest; // offset 0x110, size 0x14, Decl: 250
};

}; // namespace Path

#endif
