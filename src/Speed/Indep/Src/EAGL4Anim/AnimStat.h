#ifndef EAGL4ANIM_ANIMSTAT_H
#define EAGL4ANIM_ANIMSTAT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

namespace EAGL4Anim {

// total size: 0x14
struct AnimStat {
    AnimStat() {}

    ~AnimStat() {}

    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    int mNumEvals;     // offset 0x0, size 0x4
    int mNumDeltaSums; // offset 0x4, size 0x4
    int mNumFrames;    // offset 0x8, size 0x4
    int mNumFnCreates; // offset 0xC, size 0x4
    long mNumCycles;   // offset 0x10, size 0x4
};

}; // namespace EAGL4Anim

#endif
