#include "FastMem.h"

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
        size_t listIndex = (bytes - 1) / sizeof(FreeBlock);
        FreeBlock *freeBlk = reinterpret_cast<FreeBlock *>(ptr);
        freeBlk->Next = mFreeLists[listIndex];
        mFreeLists[listIndex] = freeBlk;
    }
}
