#ifndef BWARE_BMEMORY_H
#define BWARE_BMEMORY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "bDebug.hpp"
#include "bList.hpp"

// total size: 0x10
class FreeBlock : public bTNode<FreeBlock> {
  public:
    FreeBlock *GetTop() {
        return reinterpret_cast<FreeBlock *>(&reinterpret_cast<char *>(this)[Size]);
    }

    FreeBlock *GetBot() {
        return &this[1];
    }

    int Size;        // offset 0x8, size 0x4
    int MagicNumber; // offset 0xC, size 0x4
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

// total size: 0x10
struct MemoryPoolInfo {
    bool NumberReserved;                  // offset 0x0, size 0x1
    bool TopMeansLargerAddress;           // offset 0x4, size 0x1
    int OverflowPoolNumber;               // offset 0x8, size 0x4
    MemoryPoolOverrideInfo *OverrideInfo; // offset 0xC, size 0x4
};

// total size: 0x24
struct bMemoryTraceNewPoolPacket {
    intptr_t PoolID; // offset 0x0, size 0x4
    char Name[32];   // offset 0x4, size 0x20
};

// total size: 0x44
struct bMemoryTraceAllocatePacket {
    intptr_t PoolID;        // offset 0x0, size 0x4
    intptr_t MemoryAddress; // offset 0x4, size 0x4
    int Size;               // offset 0x8, size 0x4
    int DebugLine;          // offset 0xC, size 0x4
    int AllocationNumber;   // offset 0x10, size 0x4
    char DebugText[48];     // offset 0x14, size 0x30
};

// total size: 0x4
struct bMemoryTraceDeletePoolPacket {
    intptr_t PoolID; // offset 0x0, size 0x4
};

// total size: 0xC
struct bMemoryTraceFreePacket {
    intptr_t PoolID;        // offset 0x0, size 0x4
    intptr_t MemoryAddress; // offset 0x4, size 0x4
    int Size;               // offset 0x8, size 0x4
};

// total size: 0x18
struct bVirtualMemoryManager {
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
    bMemoryAllocator()
        : mRefcount(1) //
          ,
          PoolNumber(0) {}

    virtual void *Alloc(size_t size, const EA::TagValuePair &flags);
    virtual void Free(void *pBlock, size_t size);
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

void bMemoryInit();
unsigned int GetVirtualMemoryAllocParams();
void bInitMemoryPool(int pool_num, void *mem, int mem_size, const char *debug_name);
int GetVirtualMemoryPoolNumber();
int bGetMemoryPoolNum(const char *memory_pool_name);
void bReserveMemoryPool(int pool_num);
void bMemoryCreatePersistentPool(int size);
int bGetFreeMemoryPoolNum();
int bLargestMalloc(int allocation_params);
void bVerifyPoolIntegrity(int pool);
void bMemoryPrintAllocationsByAddress(int pool_num, int from_allocation, int to_allocation);
int bCountFreeMemory(int pool);
int bMemoryGetAllocationNumber();
void bCloseMemoryPool(int pool_num);
void bMemorySetOverflowPoolNumber(int pool_num, int overflow_pool_number);
void bSetMemoryPoolOverrideInfo(int pool_num, MemoryPoolOverrideInfo *override_info);

void *bWareMalloc(int size, const char *debug_text, int debug_line, int allocation_params);

#endif
