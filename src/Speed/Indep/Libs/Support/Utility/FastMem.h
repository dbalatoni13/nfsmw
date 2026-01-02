#ifndef SUPPORT_UTILITY_FASTMEM_H
#define SUPPORT_UTILITY_FASTMEM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMemory.hpp"

// total size: 0x32C
class FastMem {
  public:
    FastMem();
    FastMem(EA::Allocator::IAllocator *allocator, std::size_t bytes, const char *name, unsigned int expansionsize, std::size_t trackingsize);
    void Init();
    void Deinit();
    void *Alloc(std::size_t bytes, const char *kind);
    void Free(void *ptr, std::size_t bytes, const char *kind);
    void DumpRecord();
    bool CreateBlock(std::size_t listIndex);

    static inline void Lock();

  private:
    void *CoreAlloc(std::size_t bytes, const char *kind);
    void CoreFree(void *ptr);
    bool AssignToFree(std::size_t bytes);
    bool SplitOrExpand(std::size_t bytes);

  private:
    FreeBlock *mFreeLists[64];   // offset 0x0, size 0x100
    const char *mName;           // offset 0x100, size 0x4
    unsigned int mExpansionSize; // offset 0x104, size 0x4
    unsigned int mLocks;         // offset 0x108, size 0x4
    bool mInited;                // offset 0x10C, size 0x1
    void *mBlock;                // offset 0x110, size 0x4
    unsigned int mBytes;         // offset 0x114, size 0x4
    unsigned int mUsed;          // offset 0x118, size 0x4
    unsigned int mAlloc[64];     // offset 0x11C, size 0x100
    unsigned int mAvail[64];     // offset 0x21C, size 0x100
    unsigned int mAllocOver;     // offset 0x31C, size 0x4
    AllocDesc *mTrack;           // offset 0x320, size 0x4
    unsigned int mTrackMax;      // offset 0x324, size 0x4
    unsigned int mTrackCount;    // offset 0x328, size 0x4
};

extern FastMem gFastMem;

inline void FastMem::Lock() {
    gFastMem.mLocks++;
}

#endif
