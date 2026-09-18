#ifndef __IPATHSNDH__
#define __IPATHSNDH__ // Decl: 2

#include "path/IPathTrack.h"

namespace Path {

// total size: 0x4
// Decl: 26
class IPathToSnd {
  public:
    IPathToSnd() {} // Decl: 31

    virtual ~IPathToSnd() {} // Decl: 32

    virtual int CreateStreamTrack(IPathTrack **ppPathTrack, int maxRequests, float bufferTime, int byteRate) = 0; // Decl: 49

    virtual int CreateBankTrack(IPathTrack **ppPathTrack, int maxsubbanks) = 0; // Decl: 68

    virtual void GetDefaultPlayOpts(void *playopts) = 0; // Decl: 82

#ifdef EA_PLATFORM_GAMECUBE
    // En GameCube vive en .sdata aunque el juego se compila con -G0; en PS2 esta en .data.
    static IPathToSnd *sndimp __attribute__((section(".sdata"))); // 0x804FF5FC, Decl: 29
#else
    static IPathToSnd *sndimp; // Decl: 29
#endif
};

}; // namespace Path

#endif
