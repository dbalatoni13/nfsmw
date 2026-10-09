#include "FastMem.h"

// total size: 0x10
class AllocDesc {
    unsigned int mIndex; // offset 0x0, size 0x4
    const char *mName;   // offset 0x4, size 0x4
    unsigned int mCount; // offset 0x8, size 0x4
    unsigned int mHigh;  // offset 0xC, size 0x4
};

FastMem::FastMem() {}

// STRIPPED
// FastMem::FastMem(EA::Allocator::IAllocator *allocator, unsigned int bytes, const char *name, unsigned int expansionsize, unsigned int trackingsize)
// {}

void FastMem::Init() {}

// STRIPPED
void FastMem::Deinit() {}

void *FastMem::Alloc(size_t bytes, const char *kind) {}

void FastMem::Free(void *ptr, size_t bytes, const char *kind) {
    if (bytes > 0x400) {
        CoreFree(ptr);
    } else {
        size_t listIndex = (bytes - 1) / 16;
        FreeBlock *freeBlk = reinterpret_cast<FreeBlock *>(ptr);
        freeBlk->mNext = this->mFreeLists[listIndex]; // TODO bug? shouldn't this be FastMem::FreeBlock::mNext?
        this->mFreeLists[listIndex] = freeBlk;
    }
}
