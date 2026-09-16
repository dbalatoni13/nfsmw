#ifndef __FastMem_h_
#define __FastMem_h_

#include "Allocator/iallocator.h"

// TODO
#define FASTMEM_ASSERT(_a_) bAssert(_a_)
#define FASTMEM_ABORT(_a_) bAssert(0)
#define FASTMEM_ASSERT_MESSAGE(_a_, _m_)
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
// Decl: 36
class FastMem {
  public:
    FastMem();
    FastMem(EA::Allocator::IAllocator *allocator, size_t bytes, const char *name, unsigned int expansionsize, size_t trackingsize);
    void Init();
    void Deinit();
    void *Alloc(size_t bytes, const char *kind);
    void Free(void *ptr, size_t bytes, const char *kind);
    void DumpRecord();
    bool CreateBlock(size_t listIndex);

    static inline void Lock();

    // total size: 0x4
    // Decl: 54
    class DumpRecorder {
      public:
        DumpRecorder() {}
        virtual void ReportBlockKind(const char *name, unsigned int bytes, unsigned int count, unsigned int highwater);
        virtual void ReportBlockSize(unsigned int size, unsigned int freeblocks, unsigned int usedblocks);
    };

  private:
    void *CoreAlloc(size_t bytes, const char *kind); // Decl: 71
    void CoreFree(void *ptr);                        // Decl: 72

    bool AssignToFree(size_t bytes);  // Decl: 76
    bool SplitOrExpand(size_t bytes); // Decl: 77

    // total size: 0x4
    // Decl: 81
    struct FreeBlock {
        FreeBlock *mNext; // offset 0x0, size 0x4
    };

    FreeBlock *mFreeLists[64]; // offset 0x0, size 0x100, Decl: 84

    const char *mName;           // offset 0x100, size 0x4, Decl: 86
    unsigned int mExpansionSize; // offset 0x104, size 0x4, Decl: 87
    unsigned int mLocks;         // offset 0x108, size 0x4, Decl: 88

    bool mInited;        // offset 0x10C, size 0x1, Decl: 90
    void *mBlock;        // offset 0x110, size 0x4, Decl: 91
    unsigned int mBytes; // offset 0x114, size 0x4, Decl: 92
    unsigned int mUsed;  // offset 0x118, size 0x4, Decl: 93

    unsigned int mAlloc[64]; // offset 0x11C, size 0x100, Decl: 96
    unsigned int mAvail[64]; // offset 0x21C, size 0x100, Decl: 97
    unsigned int mAllocOver; // offset 0x31C, size 0x4, Decl: 98

    class AllocDesc *mTrack;  // offset 0x320, size 0x4, Decl: 101
    unsigned int mTrackMax;   // offset 0x324, size 0x4, Decl: 102
    unsigned int mTrackCount; // offset 0x328, size 0x4, Decl: 103
};

extern FastMem gFastMem; // Decl: 108

inline void FastMem::Lock() {
    gFastMem.mLocks++;
}

#endif
