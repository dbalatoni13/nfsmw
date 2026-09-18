#ifndef __IPATHTRACKH__
#define __IPATHTRACKH__

#include "path/path.h"

// total size: 0x14
// Decl: 9
typedef struct PATHTRACKINFO {
    unsigned int startingsample; // offset 0x0, size 0x4, Decl: 13
    unsigned short numsubbanks;  // offset 0x4, size 0x2, Decl: 14
    unsigned short purgemode;    // offset 0x6, size 0x2, Decl: 15
    unsigned int muschecksum;    // offset 0x8, size 0x4, Decl: 16
    unsigned int maxaram;        // offset 0xC, size 0x4, Decl: 17
    unsigned int maxmram;        // offset 0x10, size 0x4, Decl: 18
} PATHTRACKINFO;

// total size: 0x10
// Decl: 23
typedef struct PATHSUBBANKINFO {
    unsigned int muschecksum; // offset 0x0, size 0x4, Decl: 24
    unsigned int priority;    // offset 0x4, size 0x4, Decl: 25
    unsigned int aramsize;    // offset 0x8, size 0x4, Decl: 26
    unsigned int mramsize;    // offset 0xC, size 0x4, Decl: 27
} PATHSUBBANKINFO;

// total size: 0x28
// Decl: 32
typedef struct PATHSUBBANKSTATUS {
    PATHSUBBANKINFO info; // offset 0x0, size 0x10, Decl: 33
    int subbanknum;       // offset 0x10, size 0x4, Decl: 34
    int ready;            // offset 0x14, size 0x4, Decl: 35
    int external;         // offset 0x18, size 0x4, Decl: 36
    int bankhandle;       // offset 0x1C, size 0x4, Decl: 37
    char *filedata;       // offset 0x20, size 0x4, Decl: 38
    int lastplaytime;     // offset 0x24, size 0x4, Decl: 39
} PATHSUBBANKSTATUS;

// total size: 0x14
// Decl: 43
typedef struct PATHQUEUEINFO {
    int duration;      // offset 0x0, size 0x4, Decl: 44
    int requesthandle; // offset 0x4, size 0x4, Decl: 45
    int timeremaining; // offset 0x8, size 0x4, Decl: 46
    int node;          // offset 0xC, size 0x4, Decl: 47
    int beat;          // offset 0x10, size 0x4, Decl: 48
} PATHQUEUEINFO;

namespace Path {

// total size: 0x6C
// Decl: 75
class IPathTrack {
  public:
    IPathTrack();
    virtual ~IPathTrack();

    virtual void SetTrackInfo(PATHTRACKINFO *info) { mTrackInfo = info; } // Decl: 80

    virtual void UpdateStatus() = 0; // Decl: 86

    virtual int Stop() = 0;

    virtual int Pause(int pause) = 0; // Decl: 92

    virtual void CheckStatus() = 0; // Decl: 97

    virtual int GetPathStatus(PATHSTATUS *psps) = 0; // Decl: 101

    virtual int ReadyForNewRequest() = 0; // Decl: 104

    virtual int TimeRemaining(int request) = 0; // Decl: 107

    virtual int TimeBuffered() = 0; // Decl: 110

    virtual int Play(int node, unsigned int offset, int, int holdtime,
                     unsigned int duration) = 0; // Decl: 114

    virtual int GetVolume() { return mVolume; } // Decl: 117
    virtual int SetVolume(int volume) = 0; // Decl: 118

    virtual int GetDryLevel() { return mDryLevel; } // Decl: 121
    virtual int SetDryLevel(int level) = 0; // Decl: 122

    virtual int GetFXSendLevel(int bus) { return mFXSendLevel; } // Decl: 125
    virtual int SetFXSendLevel(int bus, int level) = 0; // Decl: 126

    virtual int GetPitchMult() = 0;              // Decl: 129
    virtual int SetPitchMult(int pitchmult) = 0; // Decl: 130

    virtual int GetStretchMult() = 0;                // Decl: 133
    virtual int SetStretchMult(int stretchmult) = 0; // Decl: 134

    virtual int GetHandle() { return mHandle; } // Decl: 136
    virtual void *GetPlayOpts() { return mPlayOpts; } // Decl: 137

    virtual void SetName(const char *inName) = 0; // Decl: 139

    virtual void SetFilePath(char *path) {} // Decl: 141

    virtual void StreamCache(char *pcache, int cachesize) = 0; // Decl: 148

    virtual int ModifyHold(int holdtime) = 0; // Decl: 151

    virtual int GetNumSubBanks();
    virtual int GetMaxSubBanks();
    virtual PATHSUBBANKSTATUS *GetSubBankPtr(int subbanknum);
    virtual PATHSUBBANKSTATUS *GetAvailSubBankPtr();
    virtual int AddSubBank(int subbanknum, void *pbank);
    virtual int AddSubBankDone(int subbanknum);
    virtual int DetachSubBankHeader(int subbanknum, int status);
    virtual int RemoveSubBank(int subbanknum);

  protected:
    int mHandle;                  // offset 0x0, size 0x4, Decl: 176
    int mStatusCode;              // offset 0x4, size 0x4, Decl: 177
    int mFXSendLevel;             // offset 0x8, size 0x4, Decl: 178
    int mDryLevel;                // offset 0xC, size 0x4, Decl: 179
    int mVolume;                  // offset 0x10, size 0x4, Decl: 180
    int mMaxRequests;             // offset 0x14, size 0x4, Decl: 181
    int mActiveRequests;          // offset 0x18, size 0x4, Decl: 182
    int mUpdateStatus;            // offset 0x1C, size 0x4, Decl: 183
    void *mPlayOpts;              // offset 0x20, size 0x4, Decl: 184
    PATHSTATUS mStatus;           // offset 0x24, size 0x38, Decl: 185
    PATHTRACKINFO *mTrackInfo;    // offset 0x5C, size 0x4, Decl: 186
    int mMaxSubBanks;             // offset 0x60, size 0x4, Decl: 187
    PATHSUBBANKSTATUS *mSubBanks; // offset 0x64, size 0x4, Decl: 188
};

}; // namespace Path

#endif
