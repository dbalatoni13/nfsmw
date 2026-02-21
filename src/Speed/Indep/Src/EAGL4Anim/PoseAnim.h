#ifndef EAGL4ANIM_POSEANIM_H
#define EAGL4ANIM_POSEANIM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"

namespace EAGL4Anim {

// total size: 0x10
struct PoseAnim : public AnimMemoryMap {
    static int ComputeSize(int numKeys) {}

    int GetNumFrames() const {}

    static int GetFnOffset() {}

    void *GetFnLocation() {}

    static void InitAnimMemoryMap(AnimMemoryMap *anim);

    unsigned short mNumKeys;     // offset 0x4, size 0x2
    unsigned char mPaletteIndex; // offset 0x6, size 0x1
    unsigned char mPadding[1];   // offset 0x7, size 0x1
    unsigned char *mPoseIndices; // offset 0x8, size 0x4
    unsigned short *mTimes;      // offset 0xC, size 0x4
};

}; // namespace EAGL4Anim

#endif
