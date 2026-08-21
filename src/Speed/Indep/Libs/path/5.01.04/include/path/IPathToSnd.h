#ifndef __IPATHSNDH__
#define __IPATHSNDH__ // Decl: 2

#include "path/IPathTrack.h"

namespace Path {

// total size: 0x4
// Decl: 26
struct IPathToSnd {
    IPathToSnd() {} // Decl: 31

#ifdef PATH_SND_EMIT_METHODS
    virtual ~IPathToSnd(); // Decl: 32
#else
    inline virtual ~IPathToSnd() {} // Decl: 32
#endif

    virtual int CreateStreamTrack(IPathTrack **ppPathTrack, int maxRequests, float bufferTime, int byteRate); // Decl: 49

    virtual int CreateBankTrack(IPathTrack **ppPathTrack, int maxsubbanks); // Decl: 68

    virtual void GetDefaultPlayOpts(void *playopts); // Decl: 82

    static IPathToSnd *sndimp; // size: 0x4, address: 0x804FF5FC, Decl: 29
};

}; // namespace Path

#endif
