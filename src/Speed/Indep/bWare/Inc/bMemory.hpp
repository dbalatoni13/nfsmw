#ifndef BWARE_BMEMORY_H
#define BWARE_BMEMORY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "bDebug.hpp"
#include "bList.hpp"

struct FreeBlock : public bTNode<FreeBlock> {
    // total size: 0x10
    int Size;        // offset 0x8, size 0x4
    int MagicNumber; // offset 0xC, size 0x4
};

struct AllocationHeader : public bTNode<AllocationHeader> {
    // total size: 0x14
    unsigned char PoolNum;       // offset 0x8, size 0x1
    unsigned char MagicNumber;   // offset 0x9, size 0x1
    unsigned short FrontPadding; // offset 0xA, size 0x2
    int Size;                    // offset 0xC, size 0x4
    int RequestedSize;           // offset 0x10, size 0x4

    void *GetBottomAddress() {
        return reinterpret_cast<void *>((uintptr_t)this - (uintptr_t)this->FrontPadding);
    }

    void *GetAllocAddress() {
        return &this[1];
    }

    const char *GetDebugText() {
        return "";
    }
};

struct MemoryPool {
    // total size: 0x60
    const char *pDebugName;                        // offset 0x0, size 0x4
    bTList<FreeBlock> FreeBlockList;               // offset 0x4, size 0x8
    bTList<AllocationHeader> AllocationHeaderList; // offset 0xC, size 0x8
    intptr_t InitialAddress;                       // offset 0x14, size 0x4
    int InitialSize;                               // offset 0x18, size 0x4
    int NumAllocations;                            // offset 0x1C, size 0x4
    int TotalNumAllocations;                       // offset 0x20, size 0x4
    int PoolSize;                                  // offset 0x24, size 0x4
    int AmountAllocated;                           // offset 0x28, size 0x4
    int MostAmountAllocated;                       // offset 0x2C, size 0x4
    int AmountFree;                                // offset 0x30, size 0x4
    int LeastAmountFree;                           // offset 0x34, size 0x4
    bool DebugFillEnabled;                         // offset 0x38, size 0x1
    bool DebugTracingEnabled;                      // offset 0x3C, size 0x1
    bMutex Mutex;                                  // offset 0x40, size 0x20

  public:
    void Init(void *memory, int memory_size, const char *debug_name);
    void Close();
    void AddMemory(void *p, int size);
    void RemoveMemory(void *p, int size);
    void FreeMemory(void *p, int size, const char *debug_name);
    void AddFreeMemory(void *p, int size, const char *debug_name);
    void *AllocateMemory(int size, int alignment, int alignment_offset, int start_from_top, int use_best_fit, int *new_size);
    int GetAmountFree();
    int GetLargestFreeBlock();
    void VerifyPoolIntegrity(bool verify_free_pattern);
    int CountAllocations(const char *debug_text);
    void PrintAllocationsByAddress(int from_allocation, int to_allocation);
    void PrintAllocations(int from_allocation, int to_allocation);
    AllocationHeader *FindAllocation(int allocation_num);
    int GetAllocations(void **allocations, int max_allocations);
    void SetFancyStompDetector(void *mem, int mem_size, const char *name);
    bool CheckFancyStompDetector(const void *mem, int mem_size);
    void TraceNewPool();
    void TraceDeletePool();
    void TraceFreeMemory(void *p, int size);
    void TraceRemoveMemory(void *p, int size);
    void TraceAllocateMemory(void *p, int size);
    void UpdateTraceInformation();

    const char *GetName() {
        return this->pDebugName;
    }

    bool SetDebugFill(bool on_off) {
        bool previous = this->DebugFillEnabled;
        this->DebugFillEnabled = on_off;
        return previous;
    }

    bool SetDebugTracing(bool on_off) {
        bool previous = this->DebugTracingEnabled;
        this->DebugTracingEnabled = on_off;
        return previous;
    }

    bool IsInPool(intptr_t address) {
        return address >= this->InitialAddress && address < this->InitialAddress + this->InitialSize;
    }

    void RemoveAllocationHeader(AllocationHeader *allocation_header) {
        this->AllocationHeaderList.Remove(allocation_header);
    }
};

struct MemoryPoolOverrideInfo {
    // total size: 0x20
    const char *Name;                                     // offset 0x0, size 0x4
    void *Pool;                                           // offset 0x4, size 0x4
    int Address;                                          // offset 0x8, size 0x4
    int Size;                                             // offset 0xC, size 0x4
    void *(*Malloc)(void *, int, const char *, int, int); // offset 0x10, size 0x4
    void (*Free)(void *, void *);                         // offset 0x14, size 0x4
    int (*GetAmountFree)(void *);                         // offset 0x18, size 0x4
    int (*GetLargestFreeBlock)(void *);                   // offset 0x1C, size 0x4
};

struct MemoryPoolInfo {
    // total size: 0x10
    bool NumberReserved;                  // offset 0x0, size 0x1
    bool TopMeansLargerAddress;           // offset 0x4, size 0x1
    int OverflowPoolNumber;               // offset 0x8, size 0x4
    MemoryPoolOverrideInfo *OverrideInfo; // offset 0xC, size 0x4
};

struct bMemoryTraceNewPoolPacket {
    // total size: 0x24
    uintptr_t PoolID; // offset 0x0, size 0x4
    char Name[32];    // offset 0x4, size 0x20
};

struct bMemoryTraceDeletePoolPacket {
    // total size: 0x4
    uintptr_t PoolID; // offset 0x0, size 0x4
};

struct bMemoryTraceFreePacket {
    // total size: 0xC
    uintptr_t PoolID;        // offset 0x0, size 0x4
    uintptr_t MemoryAddress; // offset 0x4, size 0x4
    int Size;                // offset 0x8, size 0x4
};

struct bVirtualMemoryManager {
    // total size: 0x18
    uintptr_t mVirtualBaseAddr; // offset 0x0, size 0x4
    uintptr_t mARamBaseAddr;    // offset 0x4, size 0x4
    unsigned int mARamSize;     // offset 0x8, size 0x4
    uintptr_t mMRamBaseAddr;    // offset 0xC, size 0x4
    unsigned int mMRamSize;     // offset 0x10, size 0x4
    unsigned int bIsValid;      // offset 0x14, size 0x4

    void Init();
    void Quit();
    void Alloc();
};

// TODO move
namespace EA {
struct TagValuePair {
    // total size: 0xC
    unsigned int mTag; // offset 0x0, size 0x4
    union {
        int mInt;              // offset 0x0, size 0x4
        unsigned int mSize;    // offset 0x0, size 0x4
        float mFloat;          // offset 0x0, size 0x4
        const void *mPointer;  // offset 0x0, size 0x4
    } mValue;                  // offset 0x4, size 0x4
    const TagValuePair *mNext; // offset 0x8, size 0x4
};

namespace Allocator {
struct IAllocator {
    virtual void *Alloc(unsigned int size, const TagValuePair &flags) = 0;
    virtual void Free(void *pBlock, unsigned int size) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual ~IAllocator() {}
};
} // namespace Allocator
}; // namespace EA

class bMemoryAllocator : public EA::Allocator::IAllocator {
    // total size: 0xC
    int mRefcount;  // offset 0x4, size 0x4
    int PoolNumber; // offset 0x8, size 0x4

  public:
    virtual void *Alloc(unsigned int size, const EA::TagValuePair &flags);
    virtual void Free(void *pBlock, unsigned int size);
    virtual int AddRef();
    virtual int Release();

    void SetMemoryPool(int pool_number) {
        this->PoolNumber = pool_number;
    }
};

struct AllocDesc {
    // total size: 0x10
    unsigned int mIndex; // offset 0x0, size 0x4
    const char *mName;   // offset 0x4, size 0x4
    unsigned int mCount; // offset 0x8, size 0x4
    unsigned int mHigh;  // offset 0xC, size 0x4
};

struct FastMem {
    // total size: 0x32C
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

    FastMem();
    FastMem(EA::Allocator::IAllocator *allocator, unsigned int bytes, const char *name, unsigned int expansionsize, unsigned int trackingsize);
    void Init();
    void Deinit();
    void *Alloc(unsigned int bytes, const char *kind);
    void Free(void *ptr, unsigned int bytes, const char *kind);
    void *CoreAlloc(unsigned int bytes, const char *kind);
    void CoreFree(void *ptr);
    bool SplitOrExpand(unsigned int bytes);
    bool AssignToFree(unsigned int bytes);
    bool CreateBlock(unsigned int listIndex);
    void DumpRecord();
};

extern FastMem gFastMem;

extern MemoryPool *MemoryPools[16];

unsigned int GetVirtualMemoryAllocParams();
int GetVirtualMemoryPoolNumber();
int bGetMemoryPoolNum(const char *memory_pool_name);
int bLargestMalloc(int allocation_params);
void bVerifyPoolIntegrity(int pool);

inline int bMemoryGetPoolNum(int allocation_params) {
    return MemoryPools[allocation_params & 0xf]->GetLargestFreeBlock() - 0x5c;
}

inline int bMemoryGetAlignment(int allocation_params) {
    int alignment = allocation_params >> 6 & 0x1ffc;
    if (alignment == 0) {
        alignment = 0x10;
    }
    if (alignment < 0x80) {
        alignment = 0x80;
    }
    return alignment;
}

#endif
