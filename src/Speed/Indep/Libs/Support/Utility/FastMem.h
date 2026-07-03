#ifndef __FastMem_h_
#define __FastMem_h_

#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

// TODO
#define FASTMEM_ASSERT(_a_) bAssert(_a_)
#define FASTMEM_ABORT(_a_) bAssert(0)
#define FASTMEM_ASSERT_MESSAGE(_a_, _m_)                                                                                                             \
    // {                                                                                                                                                \
    //     if (!(_a_)) {                                                                                                                                \
    //         bPrintf(_m_);                                                                                                                            \
    //         bAssert(_a_);                                                                                                                            \
    //     }                                                                                                                                            \
    // }
#define FASTMEMDEBUGCODE(CODE)
#define FAST_NAME(NAME) NULL
#define USE_FASTALLOC(CLASSNAME)                                                                                                                     \
    void *operator new(size_t size) {                                                                                                                \
        return (gFastMem.Alloc(size, FAST_NAME(#CLASSNAME)));                                                                                        \
    };                                                                                                                                               \
    void operator delete(void *mem, size_t size) {                                                                                                   \
        if (mem != NULL)                                                                                                                             \
            gFastMem.Free(mem, size, FAST_NAME(#CLASSNAME));                                                                                         \
    };                                                                                                                                               \
    void *operator new(size_t size, const char *name) {                                                                                              \
        return (gFastMem.Alloc(size, FAST_NAME(#CLASSNAME)));                                                                                        \
    };                                                                                                                                               \
    void operator delete(void *mem, const char *name) {                                                                                              \
        FASTMEM_ASSERT_MESSAGE(false, "Do not call this.\n");                                                                                        \
    };

// total size: 0x32C
class FastMem {
    class AllocDesc {
        // total size: 0x10
        unsigned int mIndex; // offset 0x0, size 0x4
        const char *mName;   // offset 0x4, size 0x4
        unsigned int mCount; // offset 0x8, size 0x4
        unsigned int mHigh;  // offset 0xC, size 0x4
    };

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
