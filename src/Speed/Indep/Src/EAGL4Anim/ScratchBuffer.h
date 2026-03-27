#ifndef EAGL4ANIM_SCRATCHBUFFER_H
#define EAGL4ANIM_SCRATCHBUFFER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

namespace EAGL4Anim {

// total size: 0xC
class ScratchBuffer {
  public:
    ScratchBuffer() {}

    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    void *GetBuffer() {
        return mBuffer;
    }

    unsigned int GetSize() const {
        return mSize;
    }

    void AllocateBuffer(unsigned int bufferSize);

    void FreeBuffer();

    static ScratchBuffer &GetScratchBuffer(int i);

    static void FreeScratchBuffers();

  private:
    void *mBuffer;      // offset 0x0, size 0x4
    unsigned int mSize; // offset 0x4, size 0x4
    int mRefCount;      // offset 0x8, size 0x4
};

class ScratchBufferHelper {
  private:
    friend class ScratchBuffer;
    static ScratchBuffer mScratchBuffers[3]; // size: 0x24
};

}; // namespace EAGL4Anim

#endif
