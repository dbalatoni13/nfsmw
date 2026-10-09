#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <cstdlib>
#include <cstring>
#include <types.h>

#ifdef EA_PLATFORM_XENON
// Architecture setup normally supplied by the SDK's umbrella xtl.h.
#ifndef _PPC_
#define _PPC_
#endif
#include <windef.h>
#include <winbase.h>
#include <xbox.h>

extern bSharedStringPool gSharedStringPool;
#endif

#ifdef EA_PLATFORM_GAMECUBE
#include "dolphin/os/OSArena.h"
#include <dolphin.h>
#endif

// TODO
extern "C" {
void VMInit(size_t, uintptr_t, size_t);
BOOL VMAlloc(uintptr_t, size_t);
}

// total size: 0x14
class AllocationHeader : public bTNode<AllocationHeader> {
  public:
    void *GetBottomAddress() {
        return reinterpret_cast<void *>(reinterpret_cast<char *>(this) - FrontPadding);
    }

    void *GetAllocAddress() {
        return &this[1];
    }

#ifdef EA_PLATFORM_XENON
    __declspec(noinline) const char *GetDebugText() {
        return GetDebugTextInline();
    }

    const char *GetDebugTextInline() {
#else
    const char *GetDebugText() {
#endif
#if defined(MILESTONE_BUILD) && (!defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_BUILD_A124))
        char *allocation_info = reinterpret_cast<char *>(this) - FrontPadding;
#ifdef EA_PLATFORM_XENON
        int index = *reinterpret_cast<int16 *>(allocation_info + 4);
        if (index != -1) {
            bSharedString *shared_string = gSharedStringPool.GetSharedString(index);
            if (shared_string != nullptr) {
                const char *debug_text = shared_string->String;
                if (debug_text != nullptr) {
                    return debug_text;
                }
            }
        }
        return allocation_info + 6;
#else
        const char *debug_text = bGetSharedString(*reinterpret_cast<int16 *>(allocation_info + 4));
        return debug_text != nullptr ? debug_text : allocation_info + 6;
#endif
#else
        return "";
#endif
    }

    int GetAllocationNumber() {
#if defined(MILESTONE_BUILD) && (!defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_BUILD_A124))
        return *reinterpret_cast<uint16 *>(reinterpret_cast<char *>(this) - FrontPadding);
#else
        return 0;
#endif
    }

    int GetDebugLine() {
#if defined(MILESTONE_BUILD) && (!defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_BUILD_A124))
        return *reinterpret_cast<uint16 *>(reinterpret_cast<char *>(this) - FrontPadding + 2);
#else
        return 0;
#endif
    }

    uint8 PoolNum;       // offset 0x8, size 0x1
    uint8 MagicNumber;   // offset 0x9, size 0x1
    uint16 FrontPadding; // offset 0xA, size 0x2
    int32 Size;          // offset 0xC, size 0x4
    int32 RequestedSize; // offset 0x10, size 0x4
};

#ifdef EA_PLATFORM_WIN32
// total size: 0x10
class BorrowedMemoryBlock : public bTNode<BorrowedMemoryBlock> {
  public:
    void *operator new(size_t size) {
        return std::malloc(size);
    }

    BorrowedMemoryBlock(int size);
    ~BorrowedMemoryBlock();
    __declspec(noinline) void Cleanup();

    void *Memory; // offset 0x8, size 0x4
    int Size;     // offset 0xC, size 0x4
};

BorrowedMemoryBlock::BorrowedMemoryBlock(int size) {
    Size = size;
    Memory = std::malloc(size);
}

BorrowedMemoryBlock::~BorrowedMemoryBlock() {
    Cleanup();
}

void BorrowedMemoryBlock::Cleanup() {
    operator delete(Memory);
}
#endif

// total size: 0x64 (Win32), 0x60 (other platforms)
class MemoryPool {
  public:
    void Init(void *memory, int memory_size, const char *debug_name);
    void Close();
    void AddMemory(void *p, int size);
    void RemoveMemory(void *p, int size);
#if defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124)
    void FreeMemory(void *p, int size);
    void AddFreeMemory(void *p, int size);
#else
#ifdef EA_PLATFORM_XENON
    __declspec(noinline)
#endif
    void FreeMemory(void *p, int size, const char *debug_name);
    void AddFreeMemory(void *p, int size, const char *debug_name);
#endif
    void *AllocateMemory(int size, int alignment, int alignment_offset, int start_from_top, int use_best_fit, int *new_size);
    int GetAmountFree();
    int GetLargestFreeBlock();
    void VerifyPoolIntegrity(bool verify_free_pattern);
    int CountAllocations(const char *debug_text);
    void PrintAllocationsByAddress(int from_allocation, int to_allocation);
    void PrintAllocations(int from_allocation, int to_allocation);
    AllocationHeader *FindAllocation(int allocation_num);
    int GetAllocations(void **allocations, int max_allocations);
#if defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124)
    AllocationHeader *GetMemoryDumpStatistics(int &num_allocations, int &total_num_allocations, int &amount_allocated,
                                              int &most_amount_allocated, int &amount_free, const char *&debug_name);
#endif
#ifdef EA_PLATFORM_XENON
    __declspec(noinline)
#endif
    void SetFancyStompDetector(void *mem, int mem_size, const char *name);
#ifdef EA_PLATFORM_XENON
    __declspec(noinline)
#endif
    bool CheckFancyStompDetector(const void *mem, int mem_size);
#ifdef EA_PLATFORM_WIN32
    __declspec(noinline)
#endif
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

    int GetPoolSize() {
        return PoolSize;
    }

    int GetNumAllocations() {
        return NumAllocations;
    }

    bool IsEmpty() {
        return this->PoolSize == 0;
    }

    void AddAllocationHeader(AllocationHeader *allocation_header) {
        this->AllocationHeaderList.AddTail(allocation_header);
    }

    void RemoveAllocationHeader(AllocationHeader *allocation_header) {
        this->AllocationHeaderList.Remove(allocation_header);
    }

  private:
    const char *pDebugName;                        // offset 0x0, size 0x4
    bTList<FreeBlock> FreeBlockList;               // offset 0x4, size 0x8
    bTList<AllocationHeader> AllocationHeaderList; // offset 0xC, size 0x8
#ifdef EA_PLATFORM_WIN32
    bTList<BorrowedMemoryBlock> BorrowedMemoryBlockList; // offset 0x14, size 0x8
    intptr_t InitialAddress;                       // offset 0x1C, size 0x4
    int InitialSize;                               // offset 0x20, size 0x4
    int NumAllocations;                            // offset 0x24, size 0x4
    int TotalNumAllocations;                       // offset 0x28, size 0x4
    int PoolSize;                                  // offset 0x2C, size 0x4
    int AmountAllocated;                           // offset 0x30, size 0x4
    int MostAmountAllocated;                       // offset 0x34, size 0x4
    int AmountFree;                                // offset 0x38, size 0x4
    int LeastAmountFree;                           // offset 0x3C, size 0x4
    bool DebugFillEnabled;                         // offset 0x40, size 0x1
    bool DebugTracingEnabled;                      // offset 0x41, size 0x1
    bMutex Mutex;                                  // offset 0x44, size 0x20
#else
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
#endif
};

#ifdef EA_PLATFORM_WIN32
typedef char MemoryPoolSizeMustBe0x64[(sizeof(MemoryPool) == 0x64) ? 1 : -1];
#elif defined(EA_PLATFORM_GAMECUBE) || defined(EA_PLATFORM_PLAYSTATION2)
typedef char MemoryPoolSizeMustBe0x60[(sizeof(MemoryPool) == 0x60) ? 1 : -1];
#endif

int bMemoryAutomaticVerifyPoolIntegrity = 0; // size: 0x4, address: 0x80416418
int bMemoryPrintEachAllocation = 0;
#ifdef EA_PLATFORM_WIN32
int EnableCleanupBorrowedMemoryBlock = 1;
#else
int EnableCleanupBorrowedMemoryBlock = 0;
#endif
int BorrowMemoryBlockMinSize = 0x19000;
int bMemoryRandomFillPattern = 0; // size: 0x4, address: 0x80416430
#ifdef EA_PLATFORM_XENON
// Reconstructed name for the retail stomp-diagnostic breakpoint control.
int bMemoryBreakOnStomp = 0;
#endif
int bMemoryUseSharedStrings = 1;
int bMemoryTracing = 0;                                                               // size: 0x4, address: 0x80416438
#if defined(MILESTONE_BUILD) && (defined(EA_PLATFORM_XENON) || (defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124)))
// Reconstructed names for the retail allocation-observer globals.
void (*bMemoryAllocationCallback)(int pool_num, void *memory, int size, const char *debug_text) = nullptr;
void (*bMemoryFreeCallback)(int pool_num, void *memory, int size, const char *debug_text) = nullptr;
#endif
int bMemoryBreakOnAllocationNumber = -1;                                              // size: 0x4, address: 0x8041643C
int bMemoryAllocationNumber = 0;                                                      // size: 0x4, address: 0x80416440
bMemoryAllocator TheMemoryAllocator;                                                  // size: 0xC, address: 0x8045790C
EA::Allocator::IAllocator &gMemoryAllocator = TheMemoryAllocator;                     // size: 0x4, address: 0x80457918
bMemoryAllocator TheMemoryPersistentAllocator;                                        // size: 0xC, address: 0x8045791C
EA::Allocator::IAllocator &gMemoryPersistentAllocator = TheMemoryPersistentAllocator; // size: 0x4, address: 0x80457928
// static const int bMemoryEnableFancyStompDetector; // size: 0x4

#ifdef EA_PLATFORM_GAMECUBE
void bFunkGameCube(const char *server_name, unsigned char function_num, const void *param_buffer, long param_size) {}
#endif

// STRIPPED
bool ShouldPrintThisAllocation(void *mem, const char *debug_name) {}

int GetAlignmentAdjustTop(intptr_t address, int alignment, int alignment_offset) {
    return (alignment - (address + alignment_offset & alignment - 1)) % alignment;
}

int GetAlignmentAdjustBottom(intptr_t address, int alignment, int alignment_offset) {
    return (address + alignment_offset & alignment - 1) % alignment;
}

void MemoryPool::Init(void *memory, int memory_size, const char *debug_name) {
    this->FreeBlockList.InitList();
    this->AllocationHeaderList.InitList();
#ifdef EA_PLATFORM_WIN32
    this->BorrowedMemoryBlockList.InitList();
#endif
    this->InitialAddress = reinterpret_cast<uintptr_t>(memory);
    this->InitialSize = memory_size;
    this->NumAllocations = 0;
    this->TotalNumAllocations = 0;
    this->PoolSize = 0;
    this->AmountAllocated = 0;
    this->MostAmountAllocated = 0;
    this->AmountFree = 0;
    this->LeastAmountFree = 0;
    this->Mutex.Create();
    this->DebugFillEnabled = true;
    this->DebugTracingEnabled = true;
    this->pDebugName = debug_name;
    if (bMemoryTracing) {
        this->TraceNewPool();
    }
    this->AddMemory(memory, memory_size);
}

void MemoryPool::Close() {
    if (this->NumAllocations > 0) {
        this->PrintAllocationsByAddress(0, 0x7fffffff);
        bBreak();
    }
#ifdef EA_PLATFORM_WIN32
    while (this->BorrowedMemoryBlockList.GetHead() != this->BorrowedMemoryBlockList.EndOfList()) {
        BorrowedMemoryBlock *block = this->BorrowedMemoryBlockList.RemoveHead();
        block->Cleanup();
        ::operator delete(block);
    }
#else
    this->TraceDeletePool();
#endif
    this->Mutex.Destroy();
}

void MemoryPool::AddMemory(void *p, int size) {
    this->PoolSize += size;
#if defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124)
    this->AddFreeMemory(p, size);
#else
    this->AddFreeMemory(p, size, this->GetName());
#endif
}

// STRIPPED
void MemoryPool::RemoveMemory(void *p, int size) {
    this->Mutex.Lock();

    FreeBlock *free_block = static_cast<FreeBlock *>(p);
    for (FreeBlock *f = this->FreeBlockList.GetHead(); f != this->FreeBlockList.EndOfList(); f = f->GetNext()) {
        if (f == free_block && f->Size == size) {
            this->FreeBlockList.Remove(f);
            this->PoolSize -= size;
            this->AmountFree = this->PoolSize - this->AmountAllocated;
            if (bMemoryTracing && this->DebugTracingEnabled) {
                this->TraceRemoveMemory(p, size);
            }
            break;
        }
    }

    this->Mutex.Unlock();
}

#if defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124)
void MemoryPool::FreeMemory(void *p, int size) {
#else
void MemoryPool::FreeMemory(void *p, int size, const char *debug_name) {
#endif
    this->Mutex.Lock();
#if defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124)
    this->AddFreeMemory(p, size);
#else
    this->AddFreeMemory(p, size, debug_name);
#endif
    int amount_allocated = (this->AmountAllocated -= size);
    this->AmountFree = this->PoolSize - amount_allocated;
    this->NumAllocations--;
    this->Mutex.Unlock();
}

#if defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124)
void MemoryPool::AddFreeMemory(void *p, int size) {
#else
void MemoryPool::AddFreeMemory(void *p, int size, const char *debug_name) {
#endif
    if (size != 0) {
        this->Mutex.Lock();

        if (bMemoryTracing && this->DebugTracingEnabled) {
            TraceFreeMemory(p, size);
        }

        FreeBlock *new_free_block = static_cast<FreeBlock *>(p);
        new_free_block->Size = size;
        new_free_block->MagicNumber = 0x44443333;

        FreeBlock *f = this->FreeBlockList.GetHead();
        while (f != this->FreeBlockList.EndOfList() && f <= new_free_block) {
            f = f->GetNext();
        }

        this->FreeBlockList.AddBefore(f, new_free_block);

        // a GetTop inline missing
        char *fill_bot = reinterpret_cast<char *>(new_free_block + 1);
        char *fill_top = reinterpret_cast<char *>(new_free_block->GetTop());
        if (fill_top == reinterpret_cast<char *>(f)) {
            fill_top = reinterpret_cast<char *>(f + 1);
            new_free_block->Size += f->Size;
            this->FreeBlockList.Remove(f);
        }

        f = new_free_block->GetPrev();
        if (f->GetTop() == new_free_block) {
            fill_bot = reinterpret_cast<char *>(new_free_block);
            f->Size += new_free_block->Size;
            this->FreeBlockList.Remove(new_free_block);
            new_free_block = f;
        }

        if (this->DebugFillEnabled) {
            if (bMemoryRandomFillPattern != 0) {
                bMemSet(fill_bot, bGetTicker(), fill_top - fill_bot);
            } else {
#ifdef EA_PLATFORM_XENON
                this->SetFancyStompDetector(fill_bot, fill_top - fill_bot, debug_name);
#else
                bMemSet(fill_bot, 0xee, fill_top - fill_bot);
#endif
            }
        }

#ifdef EA_PLATFORM_WIN32
        if (EnableCleanupBorrowedMemoryBlock) {
            for (BorrowedMemoryBlock *block = this->BorrowedMemoryBlockList.GetHead();
                 block != this->BorrowedMemoryBlockList.EndOfList(); block = block->GetNext()) {
                if ((block->Memory == new_free_block) && (block->Size == new_free_block->Size)) {
                    this->PoolSize -= block->Size;
                    this->FreeBlockList.Remove(new_free_block);
                    this->BorrowedMemoryBlockList.Remove(block);
                    delete block;
                    break;
                }
            }
        }
#endif

        this->Mutex.Unlock();
    }
}

void *MemoryPool::AllocateMemory(int size, int alignment, int alignment_offset, int start_from_top, int use_best_fit, int *new_size) {
#ifdef EA_PLATFORM_WIN32
RETRY:
#endif
    this->Mutex.Lock();

    size = (size + 3) & ~3;
    if (static_cast<int>(size) < 16) {
        size = 16;
    }

    FreeBlock *best_free_block = nullptr;
    int best_alignment_adjust = 0;
    int best_amount_leftover = 0x7fffffff;

    if (start_from_top != 0) {
        for (FreeBlock *f = this->FreeBlockList.GetHead(); f != this->FreeBlockList.EndOfList(); f = f->GetNext()) {
            int alignment_adjust = GetAlignmentAdjustTop(reinterpret_cast<intptr_t>(f), alignment, alignment_offset);
            int amount_leftover = f->Size - (size + alignment_adjust);

#ifdef EA_PLATFORM_WIN32
            if (((amount_leftover == 0) || (amount_leftover >= 16)) && (amount_leftover < best_amount_leftover)) {
#else
            if (((amount_leftover == 0) || (amount_leftover > 0xf)) && (amount_leftover < best_amount_leftover)) {
#endif
                best_amount_leftover = amount_leftover;
#if defined(EA_PLATFORM_WIN32) || defined(EA_PLATFORM_XENON) || defined(EA_BUILD_A124)
                best_free_block = f;
                best_alignment_adjust = alignment_adjust;
#else
                best_alignment_adjust = alignment_adjust;
                best_free_block = f;
#endif
                if (use_best_fit == 0) {
                    break;
                }
            }
        }
    } else {
        for (FreeBlock *f = this->FreeBlockList.GetTail(); f != this->FreeBlockList.EndOfList(); f = f->GetPrev()) {
            int alignment_adjust =
                GetAlignmentAdjustBottom(reinterpret_cast<intptr_t>(reinterpret_cast<char *>(f) + f->Size) - size, alignment, alignment_offset);
            int amount_leftover = f->Size - (size + alignment_adjust);

#ifdef EA_PLATFORM_WIN32
            if (((amount_leftover == 0) || (amount_leftover >= 16)) && (amount_leftover < best_amount_leftover)) {
#else
            if (((amount_leftover == 0) || (amount_leftover > 15)) && (amount_leftover < best_amount_leftover)) {
#endif
#if defined(EA_PLATFORM_WIN32) || defined(EA_PLATFORM_XENON) || defined(EA_BUILD_A124)
                best_free_block = f;
#endif
                best_amount_leftover = amount_leftover;
                best_alignment_adjust = alignment_adjust;
#if defined(EA_PLATFORM_GAMECUBE) || (defined(EA_PLATFORM_PLAYSTATION2) && !defined(EA_BUILD_A124))
                best_free_block = f;
#endif
                if (use_best_fit == 0) {
                    break;
                }
            }
        }
    }

    if (best_free_block == nullptr) {
        this->Mutex.Unlock();
#ifdef EA_PLATFORM_WIN32
        int borrowed_size = size + alignment + 16;
        if (borrowed_size < BorrowMemoryBlockMinSize) {
            borrowed_size = BorrowMemoryBlockMinSize;
        }

        BorrowedMemoryBlock *block = new BorrowedMemoryBlock(borrowed_size);

        if (block->Memory == nullptr) {
            operator delete(block->Memory);
            operator delete(block);
            return nullptr;
        }

        this->AddMemory(block->Memory, block->Size);
        this->BorrowedMemoryBlockList.AddTail(block);
        goto RETRY;
#else
        return nullptr;
#endif
    }

    size += best_alignment_adjust;

    void *mem_bottom;
    if (start_from_top == 0) {
        mem_bottom = reinterpret_cast<char *>(best_free_block->GetTop()) - size;
        best_free_block->Size -= size;

        if (best_free_block->Size == 0) {
            FreeBlockList.Remove(best_free_block);
            CheckFancyStompDetector(reinterpret_cast<FreeBlock *>(mem_bottom) + 1, size - sizeof(FreeBlock));
        } else {
            CheckFancyStompDetector(mem_bottom, size);
        }
    } else {
        mem_bottom = best_free_block;
        CheckFancyStompDetector(best_free_block + 1, size - sizeof(FreeBlock));

#if defined(EA_PLATFORM_WIN32) || defined(EA_PLATFORM_XENON) || defined(EA_BUILD_A124)
        int amount_leftover = best_free_block->Size - size;
        if (amount_leftover != 0) {
#else
        if (best_free_block->Size != size) {
#endif
            FreeBlock *new_free_block = reinterpret_cast<FreeBlock *>(reinterpret_cast<char *>(best_free_block) + size);
            CheckFancyStompDetector(new_free_block, sizeof(FreeBlock));

#if defined(EA_PLATFORM_WIN32) || defined(EA_PLATFORM_XENON) || defined(EA_BUILD_A124)
            new_free_block->Size = amount_leftover;
#else
            new_free_block->Size = best_free_block->Size - size;
#endif
            new_free_block->MagicNumber = 0x44443333;

            FreeBlockList.AddAfter(best_free_block, new_free_block);
        }

        FreeBlockList.Remove(best_free_block);
    }

    if (this->DebugFillEnabled) {
        if (bMemoryRandomFillPattern != 0) {
            bMemSet(mem_bottom, bGetTicker(), size);
        } else {
            bMemSet(mem_bottom, 0xaa, size);
        }
    }

#ifdef EA_PLATFORM_PLAYSTATION2
    if (bMemoryTracing && this->DebugTracingEnabled && bIsBFunkAvailable()) {
        this->TraceAllocateMemory(mem_bottom, size);
    }
#else
    if (bIsBFunkAvailable()) {
        // TraceAllocateMemory()
    }
#endif

    this->NumAllocations++;
    this->TotalNumAllocations++;
    this->AmountAllocated += size;
    if (this->AmountAllocated > this->MostAmountAllocated) {
        this->MostAmountAllocated = this->AmountAllocated;
    }
    this->AmountFree = this->PoolSize - this->AmountAllocated;
    this->LeastAmountFree = this->PoolSize - this->MostAmountAllocated;

    this->Mutex.Unlock();
    *new_size = size;
    return mem_bottom;
}

int MemoryPool::GetAmountFree() {
    this->Mutex.Lock();
    int amount_free = 0;
    for (FreeBlock *f = this->FreeBlockList.GetHead(); f != this->FreeBlockList.EndOfList(); f = f->GetNext()) {
        amount_free += f->Size;
    }
    this->Mutex.Unlock();
    return amount_free;
}

int MemoryPool::GetLargestFreeBlock() {
    this->Mutex.Lock();
    int largest_block = 0;
    for (FreeBlock *f = this->FreeBlockList.GetHead(); f != this->FreeBlockList.EndOfList(); f = f->GetNext()) {
        if (f->Size > largest_block) {
            largest_block = f->Size;
        }
    }
    this->Mutex.Unlock();
    return largest_block;
}

void MemoryPool::VerifyPoolIntegrity(bool verify_free_pattern) {
    this->Mutex.Lock();

    int errors = 0;
    FreeBlock *previous_f = this->FreeBlockList.GetHead();
    FreeBlock *f;
    for (f = previous_f; f != this->FreeBlockList.EndOfList(); f = f->GetNext()) {
        if (!bIsValidPointer(f, 1) || !bIsValidPointer(f->GetNext(), 1) || !bIsValidPointer(f->GetPrev(), 1) || (f->MagicNumber != 0x44443333)) {
            errors++;
        } else {
            static bool found_memory_stomp_once = false;
            if (found_memory_stomp_once) {
                break;
            }

            if (verify_free_pattern && this->DebugFillEnabled && (bMemoryRandomFillPattern == 0)) {
                int *bot = reinterpret_cast<int *>(f + 1);
                int *top = reinterpret_cast<int *>(f->GetTop());

                while (bot != top) {
                    if (*bot != static_cast<int>(0xeeeeeeee)) {
                        found_memory_stomp_once = true;
                        errors++;
                        break;
                    }
                    bot++;
                }
            }
        }

        if (errors != 0) {
            break;
        }
    }

    if (errors != 0) {
#ifdef EA_PLATFORM_WIN32
        __asm int 3
#else
        bBreak();
#endif
        PrintAllocationsByAddress(0, 0x7fffffff);
        if (errors != 0) {
            goto asd;
        }
    }

    // TODO dwarf (previous_header)
    for (AllocationHeader *header = this->AllocationHeaderList.GetHead(); header != this->AllocationHeaderList.EndOfList();
         header = header->GetNext()) {
        if (!bIsValidPointer(header, 1) || !bIsValidPointer(header->GetNext(), 1) || !bIsValidPointer(header->GetPrev(), 1) ||
            (header->MagicNumber != 0x22)) {
            errors++;
        }

        if (errors != 0) {
            bBreak();
            break;
        }
    }

asd:
    this->Mutex.Unlock();
}

const char *pTraceDebugText = nullptr;  // size: 0x4, address: 0x80416450
int TraceDebugLine = 0;                 // size: 0x4, address: 0x80416454
int MemoryInitialized = 0;              // size: 0x4, address: 0x80416458
char MemoryPoolMem[16][sizeof(MemoryPool)];
MemoryPool *MemoryPools[16];            // size: 0x40, address: 0x8045A810
MemoryPoolInfo MemoryPoolInfoTable[16]; // size: 0x100, address: 0x8045A850
bVirtualMemoryManager eARAMMM;          // size: 0x18, address: 0x8045A950
unsigned int MemoryPoolZeroSize = 0;    // size: 0x4, address: 0x8041645C
int bMemoryPersistentPoolNumber = -1;   // size: 0x4, address: 0x80416460

int MemoryPool::CountAllocations(const char *debug_text) {
    this->Mutex.Lock();
    int count = 0;
    for (AllocationHeader *header = this->AllocationHeaderList.GetHead();
         header != this->AllocationHeaderList.EndOfList(); header = header->GetNext()) {
        if (bMatchNameWithWildcard(debug_text, header->GetDebugText())) {
            count += header->Size;
        }
    }
    this->Mutex.Unlock();
    return count;
}

int CheckFlipMemoryByAddress(AllocationHeader *a, AllocationHeader *b) {
    return static_cast<int>(a->GetBottomAddress() <= b->GetBottomAddress());
}

int CheckFlipMemoryByAllocationNumber(AllocationHeader *a, AllocationHeader *b) {
#if defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124)
    return static_cast<int>(b->GetAllocationNumber() >= a->GetAllocationNumber());
#else
    return 1;
#endif
}

void MemoryPool::PrintAllocationsByAddress(int from_allocation, int to_allocation) {
    this->AllocationHeaderList.Sort(CheckFlipMemoryByAddress);
    bReleasePrintf("\nMemoryPool: \"%s\"\n", GetName());
    bReleasePrintf("AllocationNumber Address      Size    Debug Text (& Line)\n");
    bReleasePrintf("=========================================================\n");

    AllocationHeader *header;
    AllocationHeader *prev_header = nullptr;
    int free_mem;
    for (header = this->AllocationHeaderList.GetHead(); header != this->AllocationHeaderList.EndOfList(); header = header->GetNext()) {
        if (header->GetAllocationNumber() < from_allocation) {
            prev_header = nullptr;
        } else if (header->GetAllocationNumber() < to_allocation) {
            if (prev_header != nullptr) {
                unsigned char *prev_header_bot = static_cast<unsigned char *>(prev_header->GetBottomAddress());
                unsigned char *header_bot = static_cast<unsigned char *>(header->GetBottomAddress());
                int block_size = header_bot - prev_header_bot;
                int gap_size = block_size - prev_header->Size;
                if (gap_size != 0) {
                    bReleasePrintf("     *************** FREE    %6d **************\n", gap_size);
                }
            }

            bReleasePrintf("    %5d        0x%08x %7d   %s", header->GetAllocationNumber(), header->GetBottomAddress(), header->Size,
                           header->GetDebugText());
            if (header->GetDebugLine() != 0) {
                bReleasePrintf(", %d", header->GetDebugLine());
            }
            bReleasePrintf("\n");
            prev_header = header;
        } else {
            prev_header = nullptr;
        }
    }
}

void MemoryPool::PrintAllocations(int from_allocation, int to_allocation) {
    this->AllocationHeaderList.Sort(CheckFlipMemoryByAllocationNumber);
    bReleasePrintf("\nMemoryPool: \"%s\"\n", this->GetName());
    bReleasePrintf("AllocationNumber Address      Size    Debug Text (& Line)\n");
    bReleasePrintf("=========================================================\n");

    for (AllocationHeader *header = this->AllocationHeaderList.GetHead(); header != this->AllocationHeaderList.EndOfList();
         header = header->GetNext()) {
        if ((header->GetAllocationNumber() >= from_allocation) && (header->GetAllocationNumber() < to_allocation)) {
            bReleasePrintf("    %5d        0x%08x %7d   %s", header->GetAllocationNumber(), header->GetBottomAddress(), header->Size,
                           header->GetDebugText());
            if (header->GetDebugLine() != 0) {
                bReleasePrintf(", %d", header->GetDebugLine());
            }
            bReleasePrintf("\n");
        }
    }
}

AllocationHeader *MemoryPool::FindAllocation(int allocation_num) {
    for (AllocationHeader *header = this->AllocationHeaderList.GetHead();
         header != this->AllocationHeaderList.EndOfList(); header = header->GetNext()) {
        if (header->GetAllocationNumber() == allocation_num) {
            return header;
        }
    }
    return nullptr;
}

int MemoryPool::GetAllocations(void **allocations, int max_allocations) {
    this->AllocationHeaderList.Sort(CheckFlipMemoryByAddress);
    int num_allocations = 0;
    for (AllocationHeader *header = this->AllocationHeaderList.GetHead(); header != this->AllocationHeaderList.EndOfList();
         header = header->GetNext()) {
        if (num_allocations < max_allocations) {
            allocations[num_allocations] = header->GetAllocAddress();
            num_allocations++;
        }
    }
    return num_allocations;
}

void MemoryPool::SetFancyStompDetector(void *mem, int mem_size, const char *name) {
#ifdef EA_PLATFORM_XENON
    if (name == nullptr) {
        name = "NULL";
    }
    int name_pos = 0;
    for (int n = 0; n < mem_size; n += 4) {
        unsigned char *pmem = static_cast<unsigned char *>(mem) + n;
        unsigned char c0 = name[name_pos++];
        if (c0 == 0) {
            name_pos = 0;
        }
        unsigned char c1 = name[name_pos++];
        if (c1 == 0) {
            name_pos = 0;
        }
        unsigned char c2 = name[name_pos++];
        if (c2 == 0) {
            name_pos = 0;
        }
        unsigned char checksum = c0 + c1 + c2;
        pmem[0] = c0;
        pmem[1] = c1;
        pmem[2] = c2;
        pmem[3] = checksum;
    }
#endif
}

bool MemoryPool::CheckFancyStompDetector(const void *mem, int mem_size) {
#ifdef EA_PLATFORM_XENON
    if (this->DebugFillEnabled && bMemoryRandomFillPattern == 0) {
        const unsigned char *mem8 = static_cast<const unsigned char *>(mem);
        for (int n = 0; n < mem_size; n += 4) {
            const unsigned char *pmem = mem8 + n;
            unsigned char c0 = pmem[0];
            unsigned char c1 = pmem[1];
            unsigned char c2 = pmem[2];
            unsigned char checksum = c0 + c1 + c2;
            if (checksum != pmem[3]) {
                bMilestonePrintf("\nERROR:  FancyStompDetector detected stomp at 0x%08x in memory pool %s\n", pmem, this->pDebugName);
                bMilestonePrintf("        Bytes 0,1,2 = previous owner (ASCII).  Byte 3 = checksum\n");
                bMilestonePrintf("\n");
                int start_pos = (n & ~15) - 32;
                int end_pos = start_pos + 96;
                for (int pos = start_pos; pos < end_pos; pos += 16) {
                    bMilestonePrintf("        0x%08X : ", mem8 + pos);
                    for (int i = 0; i < 16; i++) {
                        bMilestonePrintf(" %02X", mem8[pos + i]);
                    }
                    bMilestonePrintf("  ");
                    for (int i = 0; i < 16; i++) {
                        if ((i & 3) != 3) {
                            unsigned char c = mem8[pos + i];
                            if (c < 32 || c >= 128) {
                                c = '.';
                            }
                            bMilestonePrintf("%c", c);
                        }
                    }
                    bMilestonePrintf("\n");
                }
                this->PrintAllocationsByAddress(0, 0x7fffffff);
                static int seen_yellow_screen;
                seen_yellow_screen++;
                if (bMemoryBreakOnStomp) {
                    bBreak();
                }
                return true;
            }
        }
    }
#endif
    return false;
}

void MemoryPool::TraceNewPool() {
    bMemoryTraceNewPoolPacket packet;
    packet.PoolID = reinterpret_cast<uintptr_t>(this);
    bMemSet(packet.Name, 0, sizeof(packet.Name));
    if (this->pDebugName != nullptr) {
        bStrNCpy(packet.Name, this->pDebugName, sizeof(packet.Name) - 1);
    }
    // TODO probably macro instead of ifdefs here
    // TODO apply macro for "25"
#ifdef EA_PLATFORM_GAMECUBE
    bFunkGameCube("CODEINE", 25, &packet, sizeof(packet));
#else
    bFunkCallASync("CODEINE", 25, &packet, sizeof(packet));
#endif
}

void MemoryPool::TraceDeletePool() {
    bMemoryTraceDeletePoolPacket packet;
    packet.PoolID = reinterpret_cast<uintptr_t>(this);
#ifdef EA_PLATFORM_GAMECUBE
    bFunkGameCube("CODEINE", 26, &packet, sizeof(packet));
#else
    bFunkCallASync("CODEINE", 26, &packet, sizeof(packet));
#endif
}

void MemoryPool::TraceFreeMemory(void *p, int size) {
    bMemoryTraceFreePacket packet = {0};

    packet.PoolID = reinterpret_cast<uintptr_t>(this);
    packet.MemoryAddress = reinterpret_cast<uintptr_t>(p);
    packet.Size = size;
#ifdef EA_PLATFORM_GAMECUBE
    bFunkGameCube("CODEINE", 27, &packet, sizeof(packet));
#else
    bFunkCallASync("CODEINE", 27, &packet, sizeof(packet));
#endif
}

void MemoryPool::TraceRemoveMemory(void *p, int size) {
    bMemoryTraceRemovePacket packet = {0};
    packet.PoolID = reinterpret_cast<uintptr_t>(this);
    packet.MemoryAddress = reinterpret_cast<uintptr_t>(p);
    packet.Size = size;
#ifdef EA_PLATFORM_GAMECUBE
    bFunkGameCube("CODEINE", 29, &packet, sizeof(packet));
#else
    bFunkCallASync("CODEINE", 29, &packet, sizeof(packet));
#endif
}

// STRIPPED
void TrapMissingMemoryTraces(int size) {}

void MemoryPool::TraceAllocateMemory(void *p, int size) {
    bMemoryTraceAllocatePacket packet = {0};
    packet.PoolID = reinterpret_cast<uintptr_t>(this);
    packet.MemoryAddress = reinterpret_cast<uintptr_t>(p);
    packet.Size = size;
    packet.DebugLine = TraceDebugLine;
    packet.AllocationNumber = bMemoryAllocationNumber;
    bMemSet(packet.DebugText, 0, sizeof(packet.DebugText));
    if (pTraceDebugText != nullptr) {
        bStrNCpy(packet.DebugText, pTraceDebugText, sizeof(packet.DebugText) - 1);
    }

    int packet_size = sizeof(packet) - (sizeof(packet.DebugText) - 1 - bStrLen(packet.DebugText));
#ifdef EA_PLATFORM_GAMECUBE
    bFunkGameCube("CODEINE", 28, &packet, packet_size);
#else
    bFunkCallASync("CODEINE", 28, &packet, packet_size);
#endif

    if (pTraceDebugText == nullptr || pTraceDebugText[0] == '\0') {
        TrapMissingMemoryTraces(size);
    }
    pTraceDebugText = nullptr;
    TraceDebugLine = 0;
}

void MemoryPool::UpdateTraceInformation() {
    if (!this->DebugTracingEnabled || !bIsBFunkAvailable()) {
        return;
    }

    this->TraceNewPool();
    for (FreeBlock *block = this->FreeBlockList.GetHead(); block != this->FreeBlockList.EndOfList(); block = block->GetNext()) {
        this->TraceFreeMemory(block, block->Size);
    }

    int saved_allocation_number = bMemoryAllocationNumber;
    for (AllocationHeader *header = this->AllocationHeaderList.GetHead(); header != this->AllocationHeaderList.EndOfList();
         header = header->GetNext()) {
#if !defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_BUILD_A124)
        char *allocation_info = reinterpret_cast<char *>(header) - header->FrontPadding;
        pTraceDebugText = bGetSharedString(*reinterpret_cast<int16 *>(allocation_info + 4));
        if (pTraceDebugText == nullptr) {
            pTraceDebugText = allocation_info + 6;
        }
        TraceDebugLine = 0;
        bMemoryAllocationNumber = header->GetAllocationNumber();
#endif
        this->TraceAllocateMemory(header->GetBottomAddress(), header->Size);
    }
    bMemoryAllocationNumber = saved_allocation_number;
}

int bGetFreeMemoryPoolNum() {
    for (int pool_num = 0; pool_num < 16; pool_num++) {
        MemoryPoolInfo *info = &MemoryPoolInfoTable[pool_num];
        if (!info->NumberReserved && (MemoryPools[pool_num] == nullptr) && (info->OverrideInfo == nullptr)) {
            return pool_num;
        }
    }
    return -1;
}

void bReserveMemoryPool(int pool_num) {
    MemoryPoolInfo *info = &MemoryPoolInfoTable[pool_num];
    info->NumberReserved = true;
}

void bSetMemoryPoolOverrideInfo(int pool_num, MemoryPoolOverrideInfo *override_info) {
    MemoryPoolInfo *info = &MemoryPoolInfoTable[pool_num];
    info->OverrideInfo = override_info;
    info->OverflowPoolNumber = -1;
}

void bInitMemoryPool(int pool_num, void *mem, int mem_size, const char *debug_name) {
    MemoryPoolInfo *info = &MemoryPoolInfoTable[pool_num];
    info->TopMeansLargerAddress = false;
    info->OverflowPoolNumber = -1;
    MemoryPools[pool_num] = reinterpret_cast<MemoryPool *>(MemoryPoolMem[pool_num]);
    reinterpret_cast<MemoryPool *>(MemoryPoolMem[pool_num])->Init(mem, mem_size, debug_name);
#ifdef EA_PLATFORM_GAMECUBE
    if (pool_num == 0) {
        MemoryPoolZeroSize = mem_size;
    }
#endif
}

void bCloseMemoryPool(int pool_num) {
    MemoryPools[pool_num]->Close();
    MemoryPools[pool_num] = nullptr;
}

bool bSetMemoryPoolDebugFill(int pool_num, bool on_off) {
    return MemoryPools[pool_num]->SetDebugFill(on_off);
}

void bAddToMemoryPool(int pool_num, void *mem, int mem_size) {
    MemoryPools[pool_num]->AddMemory(mem, mem_size);
}

void bRemoveFromMemoryPool(int pool_num, void *mem, int mem_size) {
    MemoryPools[pool_num]->RemoveMemory(mem, mem_size);
}

bool bSetMemoryPoolDebugTracing(int pool_num, bool on_off) {
    bool previous;

    if (!on_off) {
        void *dummy = bMalloc(0x10, "Tracing disabled for this pool", 0, (pool_num & 0xf) | 0x40);

        previous = MemoryPools[pool_num]->SetDebugTracing(false);
        bFree(dummy);
    } else {
        previous = MemoryPools[pool_num]->SetDebugTracing(on_off);
    }
    return previous;
}

void bSetMemoryPoolTopDirection(int pool_num, bool top_means_larger_address) {
    MemoryPoolInfo *info = &MemoryPoolInfoTable[pool_num];
    info->TopMeansLargerAddress = top_means_larger_address;
}

void bMemorySetOverflowPoolNumber(int pool_num, int overflow_pool_number) {
    MemoryPoolInfo *info = &MemoryPoolInfoTable[pool_num];
    info->OverflowPoolNumber = overflow_pool_number;
}

int PlatformMemoryINIT() {
    return -1;
}

void bVirtualMemoryManager::Init() {
#ifdef EA_PLATFORM_GAMECUBE
    this->bIsValid = FALSE;
    this->mVirtualBaseAddr = 0x7e000000;
    this->mMRamBaseAddr = reinterpret_cast<uintptr_t>(OSGetArenaLo());
    this->mMRamSize = 0x80000;
    const uintptr_t end_of_audio_aram = ARGetBaseAddress() + 0x8010ffU & ~0xfff;
    this->mARamSize = 0x600000;
    this->mARamBaseAddr = end_of_audio_aram;
    VMInit(this->mMRamSize, end_of_audio_aram, this->mARamSize);
#else
// TODO
#endif
}

// STRIPPED
void bVirtualMemoryManager::Quit() {}

void bVirtualMemoryManager::Alloc() {
    this->bIsValid = VMAlloc(this->mVirtualBaseAddr, this->mARamSize);
}

const char *GetVirtualMemoryPoolName() {
    return "NGC_VirtualMemory";
}

int GetVirtualMemoryPoolNumber() {
    return 15;
}

unsigned int GetVirtualMemoryAllocParams() {
    return (GetVirtualMemoryPoolNumber() & 0xf) | 0x400;
}

#ifdef EA_PLATFORM_PLAYSTATION2
extern "C" int PS2MemorySize;
extern "C" int _HeapBasePS2;
extern "C" int _HeapSizePS2;

// These are retail linker-address operands, not dereferenced heap globals.
asm(".text\n\t"
    ".align 3\n\t"
    ".set noreorder\n\t"
    ".set nomacro\n\t"
    ".globl GetPS2HeapSize__Fv\n\t"
    ".ent GetPS2HeapSize__Fv\n\t"
    "GetPS2HeapSize__Fv:\n\t"
    "lui $3, 1\n\t"
#ifdef EA_BUILD_A124
    "lui $2, %hi(0x0068c4dc)\n\t"
#else
    "lui $2, %hi(0x005d6ed0)\n\t"
#endif
    "addiu $6, $3, -32768\n\t"
    "addiu $5, $0, -1\n\t"
#ifdef EA_BUILD_A124
    "addiu $7, $2, %lo(0x0068c4dc)\n\t"
#else
    "addiu $7, $2, %lo(0x005d6ed0)\n\t"
#endif
    "addiu $4, $0, 16384\n\t"
    "xor $2, $6, $5\n\t"
    "lui $3, 0x0200\n\t"
    "movz $6, $4, $2\n\t"
    "addiu $2, $3, -32768\n\t"
    "bne $2, $5, .LGetPS2HeapSize_return\n\t"
    "nop\n\t"
    "lui $2, %hi(PS2MemorySize)\n\t"
    "lw $3, %lo(PS2MemorySize)($2)\n\t"
    "subu $2, $3, $6\n\t"
    ".LGetPS2HeapSize_return:\n\t"
    "jr $31\n\t"
    "subu $2, $2, $7\n\t"
    ".end GetPS2HeapSize__Fv\n\t"
    ".set macro\n\t"
    ".set reorder\n\t");
int GetPS2HeapSize();
#endif

void bMemoryInit() {
#ifdef EA_PLATFORM_GAMECUBE
    void *arenaLo;
    void *arenaHi;
    if (!MemoryInitialized) {
        int nPMem = PlatformMemoryINIT();
        arenaLo = OSGetArenaLo();
        arenaHi = OSGetArenaHi();

        OSSetArenaLo(OSInitAlloc(arenaLo, arenaHi, 1));
        eARAMMM.Init();
        eARAMMM.Alloc();

        arenaLo = OSGetArenaLo();
        arenaHi = OSGetArenaHi();
        arenaLo = reinterpret_cast<void *>((reinterpret_cast<uintptr_t>(arenaLo) + 0x1f) & 0xffffffe0);
        arenaHi = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(arenaHi) & 0xffffffe0);
        int nBMem = static_cast<char *>(arenaHi) - static_cast<char *>(arenaLo);

        if ((nPMem < 1) || (nBMem <= nPMem)) {
            nPMem = nBMem;
        }

        OSSetCurrentHeap(OSCreateHeap(arenaLo, arenaHi));
        OSSetArenaLo(arenaHi);

        if (bMemoryTracing != 0) {
            bFunkGameCube("CODEINE", 24, nullptr, 0);
        }

        int bware_memory_size = nPMem - 0x40000;
        void *main_pool = OSAlloc(bware_memory_size);
        bInitMemoryPool(0, main_pool, bware_memory_size, "Main Pool");
        // TODO
        void *heap_bot;
        void *heap_top;
        int pool_number;
        int pool_num_VM = GetVirtualMemoryPoolNumber();
        bInitMemoryPool(pool_num_VM, reinterpret_cast<void *>(eARAMMM.mVirtualBaseAddr), eARAMMM.mARamSize, GetVirtualMemoryPoolName());
        MemoryInitialized = TRUE;
    }
#elif defined(EA_PLATFORM_XENON)
    if (!MemoryInitialized) {
        MEMORYSTATUS memory_before;
        MEMORYSTATUS memory_after;
        GlobalMemoryStatus(&memory_before);
        void *memory = XPhysicalAlloc(0x15900000, MAXULONG_PTR, 0, MEM_LARGE_PAGES | PAGE_READWRITE);
        GlobalMemoryStatus(&memory_after);
        bInitMemoryPool(0, memory, 0x15900000, "Main Pool");
        MemoryInitialized = TRUE;
    }
#elif defined(EA_PLATFORM_WIN32)
    if (!MemoryInitialized) {
        bInitMemoryPool(0, nullptr, 0, "Main Pool");
        MemoryInitialized = TRUE;
    }
#elif defined(EA_PLATFORM_PLAYSTATION2)
    if (!MemoryInitialized) {
        int size = GetPS2HeapSize() - 0x8000;
        void *memory = std::malloc(size);
        if (bMemoryTracing) {
            bFunkCallASync("CODEINE", 24, nullptr, 0);
        }
        bInitMemoryPool(0, memory, size, "Main Pool");
        _HeapBasePS2 = reinterpret_cast<int>(memory);
        _HeapSizePS2 = size;
        MemoryInitialized = TRUE;
    }
#endif
}

void bMemoryUpdateTraceInformation() {
    if (!bMemoryTracing) {
        return;
    }

#ifdef EA_PLATFORM_GAMECUBE
    bFunkGameCube("CODEINE", 24, nullptr, 0);
#else
    bFunkCallASync("CODEINE", 24, nullptr, 0);
#endif
    for (int pool_num = 0; pool_num < BMEMORY_MAX_POOLS; ++pool_num) {
        if (MemoryPools[pool_num] != nullptr) {
            MemoryPools[pool_num]->UpdateTraceInformation();
        }
    }
}

#ifdef EA_PLATFORM_XENON
void *bWareMalloc(int size, const char *debug_text, int debug_line, int allocation_params) {
    return bMalloc(size, debug_text, debug_line, allocation_params);
}
#elif defined(MILESTONE_BUILD) && (!defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_BUILD_A124))
void *bMalloc(int size, const char *debug_text, int debug_line, int allocation_params) {
    return bWareMalloc(size, debug_text, debug_line, allocation_params);
}
#else
void *bMalloc(int size, int allocation_params) {
    return bWareMalloc(size, nullptr, 0, allocation_params);
}
#endif

// TODO variable names
#ifdef EA_PLATFORM_XENON
void *bMalloc(int size, const char *debug_text, int debug_line, int allocation_params) {
#else
void *bWareMalloc(int size, const char *debug_text, int debug_line, int allocation_params) {
#endif
    int pool_num = bMemoryGetPoolNum(allocation_params);
    MemoryPoolInfo *info = &MemoryPoolInfoTable[pool_num];

    if (info->OverflowPoolNumber != -1) {
        int overflow_allocation_params = (allocation_params & ~0xf) | (info->OverflowPoolNumber & 0xf);
        if (bLargestMalloc(overflow_allocation_params) > size) {
            info = &MemoryPoolInfoTable[info->OverflowPoolNumber];
            allocation_params = overflow_allocation_params;
        }
    }

    if (info->TopMeansLargerAddress) {
        allocation_params ^= 0x40;
    }

    MemoryPoolOverrideInfo *override_info = info->OverrideInfo;
    if (override_info != nullptr) {
        return override_info->Malloc(override_info->Pool, size, debug_text, debug_line, allocation_params);
    }

    pTraceDebugText = debug_text;
    TraceDebugLine = debug_line;
    if (MemoryInitialized == 0) {
        bMemoryInit();
    }

    if (bMemoryAutomaticVerifyPoolIntegrity != 0) {
#ifdef EA_PLATFORM_WIN32
        if ((bMemoryAllocationNumber % bMemoryAutomaticVerifyPoolIntegrity) == 0) {
#else
        if (bMemoryAllocationNumber == (bMemoryAllocationNumber / bMemoryAutomaticVerifyPoolIntegrity) * bMemoryAutomaticVerifyPoolIntegrity) {
#endif
            bVerifyPoolIntegrity(bMemoryGetPoolNum(allocation_params));
        }
    }

    // TODO rename
    int alignment = bMemoryGetAlignment(allocation_params);
    if (alignment == 0) {
        alignment = 16;
    }

#if defined(MILESTONE_BUILD) && (!defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_BUILD_A124))
    // The milestone allocators keep debug information before the header.
    int shared_string_index = -1;
#ifdef EA_PLATFORM_XENON
    const char *shared_debug_text = bAllocateSharedString(debug_text);
    shared_string_index = bGetSharedStringIndex(shared_debug_text);
#else
    if (bMemoryUseSharedStrings) {
        const char *shared_debug_text = bAllocateSharedString(debug_text);
        if (shared_debug_text != nullptr) {
            shared_string_index = bGetSharedStringIndex(shared_debug_text);
        }
    }
#endif
    int debug_info_size = 8;
    if (shared_string_index == -1) {
        debug_info_size = (bStrLen(debug_text) + 10) & ~3;
    }
    int allocation_header_offset = bMemoryGetAlignmentOffset(allocation_params) + debug_info_size + 0x14;
#else
    int allocation_header_offset = bMemoryGetAlignmentOffset(allocation_params) + 0x14;
#endif
    int new_size;
    pool_num = bMemoryGetPoolNum(allocation_params);
    MemoryPool *pool = MemoryPools[pool_num];
#if defined(MILESTONE_BUILD) && (!defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_BUILD_A124))
    void *memory = pool->AllocateMemory(size + debug_info_size + 0x14, alignment, allocation_header_offset, allocation_params & 0x40,
                                        allocation_params & 0x80, &new_size);
#else
    void *memory =
        pool->AllocateMemory(size + 0x14, alignment, allocation_header_offset, allocation_params & 0x40, allocation_params & 0x80, &new_size);
#endif

    if (memory != nullptr) {
        int padding = 0;
        if (allocation_params & 0x40) {
            padding = GetAlignmentAdjustTop(reinterpret_cast<intptr_t>(memory), alignment, allocation_header_offset);
        }
#if defined(MILESTONE_BUILD) && (!defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_BUILD_A124))
        padding += debug_info_size;
#endif

        AllocationHeader *header = reinterpret_cast<AllocationHeader *>(reinterpret_cast<char *>(memory) + padding);
        pool->AddAllocationHeader(header);

        header->PoolNum = pool_num;
        header->MagicNumber = 0x22;
        header->FrontPadding = padding;

        header->Size = new_size;
        header->RequestedSize = size;
#if defined(MILESTONE_BUILD) && (!defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_BUILD_A124))
        char *allocation_info = static_cast<char *>(memory);
        *reinterpret_cast<uint16 *>(allocation_info) = bMemoryAllocationNumber;
        *reinterpret_cast<uint16 *>(allocation_info + 2) = debug_line;
        *reinterpret_cast<int16 *>(allocation_info + 4) = shared_string_index;
        allocation_info[6] = '\0';
        if (debug_text != nullptr && shared_string_index == -1) {
            bStrCpy(allocation_info + 6, debug_text);
        }
        bMemSet(allocation_info + debug_info_size, 0xdd, header->FrontPadding - debug_info_size);
#ifdef EA_PLATFORM_XENON
        if (bMemoryAllocationCallback != nullptr) {
            bMemoryAllocationCallback(header->PoolNum, &header[1], size, header->GetDebugText());
        }
#elif defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124)
        if (bMemoryAllocationCallback != nullptr) {
            bMemoryAllocationCallback(header->PoolNum, &header[1], size, allocation_info + 6);
        }
#endif
#endif
#ifdef EA_PLATFORM_XENON
        if (bMemoryAllocationNumber == -1) {
#else
        if (bMemoryAllocationNumber == bMemoryBreakOnAllocationNumber) {
#endif
            bBreak();
        }

        bMemoryAllocationNumber++;
        return &header[1];
    }

    bReleasePrintf("ERROR:  Out of memory in pool %s allocating %s (size = %d).  Largest possible = %d  Total = %d", pool->GetName(), debug_text,
                   size, bLargestMalloc(allocation_params), bCountFreeMemory(pool_num));
#if defined(EA_PLATFORM_WIN32) || defined(EA_PLATFORM_XENON)
    bBreak();
#endif
    bMemoryPrintAllocationsByAddress(pool_num, 0, 0x7fffffff);
    bBreak();
    return nullptr;
}

#if defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124)
// A124 retail free path, including override detection and observer lifetime.
asm(
    ".text\n\t"
    ".align 3\n\t"
    ".set noreorder\n\t"
    ".set nomacro\n\t"
    ".globl bFree__FPv\n\t"
    ".ent bFree__FPv\n\t"
    "bFree__FPv:\n\t"
    "addiu $29, $29, -0x60\n\t"
    "sd $17, 0x10($29)\n\t"
    "sd $31, 0x50($29)\n\t"
    "daddu $17, $4, $0\n\t"
    "sd $20, 0x40($29)\n\t"
    "sd $19, 0x30($29)\n\t"
    "sd $18, 0x20($29)\n\t"
    "beqz $17, .LA124_bFree_2718\n\t"
    "sd $16, 0x0($29)\n\t"
    "lui $2, %hi(MemoryPoolInfoTable)\n\t"
    "lui $3, %hi(MemoryPools)\n\t"
    "addiu $2, $2, %lo(MemoryPoolInfoTable)\n\t"
    "addiu $3, $3, %lo(MemoryPools)\n\t"
    "addiu $12, $2, 0xC\n\t"
    "daddu $11, $0, $0\n\t"
    "addiu $13, $0, 0x10\n\t"
    ".align 2\n\t"
    ".LA124_bFree_2558:\n\t"
    "lw $9, 0x0($12)\n\t"
    "beql $9, $0, .LA124_bFree_25EC\n\t"
    "addiu $11, $11, 0x1\n\t"
    "lw $4, 0x4($9)\n\t"
    "slt $2, $17, $4\n\t"
    "bnel $2, $0, .LA124_bFree_25EC\n\t"
    "addiu $11, $11, 0x1\n\t"
    "lw $2, 0x8($9)\n\t"
    "addu $2, $4, $2\n\t"
    "slt $2, $17, $2\n\t"
    "beqz $2, .LA124_bFree_25E8\n\t"
    "addiu $8, $0, 0x1\n\t"
    "addiu $10, $0, 0x1\n\t"
    "b .LA124_bFree_259C\n\t"
    "addiu $7, $3, 0x4\n\t"
    "nop\n\t"
    ".align 2\n\t"
    ".LA124_bFree_2598:\n\t"
    "addiu $8, $8, 0x1\n\t"
    ".align 2\n\t"
    ".LA124_bFree_259C:\n\t"
    "slti $2, $8, 0x10\n\t"
    "beqz $2, .LA124_bFree_25E0\n\t"
    "nop\n\t"
    "lw $4, 0x0($7)\n\t"
    "beql $4, $0, .LA124_bFree_2598\n\t"
    "addiu $7, $7, 0x4\n\t"
    "lw $5, 0x14($4)\n\t"
    "slt $2, $17, $5\n\t"
    "bnez $2, .LA124_bFree_25D8\n\t"
    "daddu $6, $0, $0\n\t"
    "lw $2, 0x18($4)\n\t"
    "daddu $6, $10, $0\n\t"
    "addu $2, $5, $2\n\t"
    "slt $2, $17, $2\n\t"
    "movz $6, $0, $2\n\t"
    ".align 2\n\t"
    ".LA124_bFree_25D8:\n\t"
    "beql $6, $0, .LA124_bFree_2598\n\t"
    "addiu $7, $7, 0x4\n\t"
    ".align 2\n\t"
    ".LA124_bFree_25E0:\n\t"
    "beql $8, $13, .LA124_bFree_26D0\n\t"
    "lw $4, 0x0($9)\n\t"
    ".align 2\n\t"
    ".LA124_bFree_25E8:\n\t"
    "addiu $11, $11, 0x1\n\t"
    ".align 2\n\t"
    ".LA124_bFree_25EC:\n\t"
    "slti $2, $11, 0x10\n\t"
    "bnez $2, .LA124_bFree_2558\n\t"
    "addiu $12, $12, 0x10\n\t"
    "addiu $16, $17, -0x14\n\t"
    "lui $2, %hi(bMemoryFreeCallback)\n\t"
    "lhu $4, 0xA($16)\n\t"
    "addiu $20, $2, %lo(bMemoryFreeCallback)\n\t"
    "lw $3, %lo(bMemoryFreeCallback)($2)\n\t"
    "beqz $3, .LA124_bFree_2648\n\t"
    "subu $19, $16, $4\n\t"
    "lh $4, 0x4($19)\n\t"
    "jal bGetSharedString__Fi\n\t"
    "lbu $18, 0x8($16)\n\t"
    "bnez $2, .LA124_bFree_2634\n\t"
    "daddu $7, $2, $0\n\t"
    "lhu $2, 0xA($16)\n\t"
    "subu $2, $16, $2\n\t"
    "addiu $7, $2, 0x6\n\t"
    ".align 2\n\t"
    ".LA124_bFree_2634:\n\t"
    "lw $2, 0x0($20)\n\t"
    "daddu $4, $18, $0\n\t"
    "lw $6, 0x10($16)\n\t"
    "jalr $2\n\t"
    "daddu $5, $17, $0\n\t"
    ".align 2\n\t"
    ".LA124_bFree_2648:\n\t"
    "lh $4, 0x4($19)\n\t"
    "addiu $2, $0, -0x1\n\t"
    "beq $4, $2, .LA124_bFree_266C\n\t"
    "lui $2, %hi(bMemoryAutomaticVerifyPoolIntegrity)\n\t"
    "jal bGetSharedString__Fi\n\t"
    "nop\n\t"
    "jal bFreeSharedString__FPCc\n\t"
    "daddu $4, $2, $0\n\t"
    "lui $2, %hi(bMemoryAutomaticVerifyPoolIntegrity)\n\t"
    ".align 2\n\t"
    ".LA124_bFree_266C:\n\t"
    "lw $3, %lo(bMemoryAutomaticVerifyPoolIntegrity)($2)\n\t"
    "beqz $3, .LA124_bFree_26A0\n\t"
    "lbu $17, 0x8($16)\n\t"
    "beql $3, $0, .LA124_bFree_2680\n\t"
    "break 0, 7\n\t"
    ".align 2\n\t"
    ".LA124_bFree_2680:\n\t"
    "lui $4, %hi(bMemoryAllocationNumber)\n\t"
    "lw $2, %lo(bMemoryAllocationNumber)($4)\n\t"
    "div $0, $2, $3\n\t"
    "mfhi $3\n\t"
    "bnel $3, $0, .LA124_bFree_26A4\n\t"
    "lbu $3, 0x9($16)\n\t"
    "jal bVerifyPoolIntegrity__Fi\n\t"
    "daddu $4, $17, $0\n\t"
    ".align 2\n\t"
    ".LA124_bFree_26A0:\n\t"
    "lbu $3, 0x9($16)\n\t"
    ".align 2\n\t"
    ".LA124_bFree_26A4:\n\t"
    "addiu $2, $0, 0x22\n\t"
    "beq $3, $2, .LA124_bFree_26E4\n\t"
    "lui $2, %hi(MemoryPools)\n\t"
    "ld $31, 0x50($29)\n\t"
    "ld $20, 0x40($29)\n\t"
    "ld $19, 0x30($29)\n\t"
    "ld $18, 0x20($29)\n\t"
    "ld $17, 0x10($29)\n\t"
    "ld $16, 0x0($29)\n\t"
    "j bBreak__Fv\n\t"
    "addiu $29, $29, 0x60\n\t"
    ".align 2\n\t"
    ".LA124_bFree_26D0:\n\t"
    "lw $2, 0x10($9)\n\t"
    "jalr $2\n\t"
    "daddu $5, $17, $0\n\t"
    "b .LA124_bFree_271C\n\t"
    "ld $31, 0x50($29)\n\t"
    ".align 2\n\t"
    ".LA124_bFree_26E4:\n\t"
    "sll $3, $17, 2\n\t"
    "addiu $2, $2, %lo(MemoryPools)\n\t"
    "sb $0, 0x9($16)\n\t"
    "addu $3, $3, $2\n\t"
    "lw $7, 0x0($16)\n\t"
    "lw $2, 0x4($16)\n\t"
    "lhu $5, 0xA($16)\n\t"
    "sw $7, 0x0($2)\n\t"
    "lw $4, 0x0($3)\n\t"
    "subu $5, $16, $5\n\t"
    "lw $6, 0xC($16)\n\t"
    "jal FreeMemory__10MemoryPoolPvi\n\t"
    "sw $2, 0x4($7)\n\t"
    ".align 2\n\t"
    ".LA124_bFree_2718:\n\t"
    "ld $31, 0x50($29)\n\t"
    ".align 2\n\t"
    ".LA124_bFree_271C:\n\t"
    "ld $20, 0x40($29)\n\t"
    "ld $19, 0x30($29)\n\t"
    "ld $18, 0x20($29)\n\t"
    "ld $17, 0x10($29)\n\t"
    "ld $16, 0x0($29)\n\t"
    "jr $31\n\t"
    "addiu $29, $29, 0x60\n\t"
    ".end bFree__FPv\n\t"
    ".set macro\n\t"
    ".set reorder\n\t");
#else
void bFree(void *ptr) {
    if (ptr == nullptr) {
        return;
    }
    for (int n = 0; n < 16; n++) {
        MemoryPoolOverrideInfo *override_info = MemoryPoolInfoTable[n].OverrideInfo;
        intptr_t address = reinterpret_cast<intptr_t>(ptr);
        if ((override_info != nullptr) && (address >= override_info->Address) && (address < override_info->Address + override_info->Size)) {
            int pool_num = 1;

            while (pool_num < 16) {
                MemoryPool *pool = MemoryPools[pool_num];

                if ((pool == nullptr) || !pool->IsInPool(address)) {
                    pool_num++;
                } else {
                    break;
                }
            }
            if (pool_num == 16) {
                override_info->Free(override_info->Pool, ptr);
                return;
            }
        }
    }
    AllocationHeader *header = &static_cast<AllocationHeader *>(ptr)[-1];
#if !defined(MILESTONE_BUILD) || defined(EA_PLATFORM_XENON) || (defined(EA_PLATFORM_PLAYSTATION2) && !defined(EA_BUILD_A124))
    int pool_num = header->PoolNum;
    MemoryPool *pool = MemoryPools[pool_num];
#endif
#ifdef EA_PLATFORM_XENON
    char debug_name[32] = "";
#else
    char debug_name[32] = {};
#endif
#if defined(MILESTONE_BUILD) && (!defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_BUILD_A124))
    void *allocated_pointer = header->GetBottomAddress();
#ifdef EA_PLATFORM_XENON
    bSafeStrCpy(debug_name, header->GetDebugTextInline(), sizeof(debug_name));
#endif
#if defined(EA_PLATFORM_XENON) || (defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124))
    if (bMemoryFreeCallback != nullptr) {
#ifdef EA_PLATFORM_XENON
        bMemoryFreeCallback(header->PoolNum, ptr, header->RequestedSize, header->GetDebugTextInline());
#else
        bMemoryFreeCallback(header->PoolNum, ptr, header->RequestedSize, header->GetDebugText());
#endif
    }
#endif
    int16 shared_string_index = *reinterpret_cast<int16 *>(static_cast<char *>(allocated_pointer) + 4);
    if (shared_string_index != -1) {
#ifdef EA_PLATFORM_XENON
        bSharedString *shared_string = gSharedStringPool.GetSharedString(shared_string_index);
        bFreeSharedString(shared_string != nullptr ? shared_string->String : nullptr);
#else
        bFreeSharedString(bGetSharedString(shared_string_index));
#endif
    }
#ifndef EA_PLATFORM_XENON
    int pool_num = header->PoolNum;
#endif
#endif
    if (bMemoryAutomaticVerifyPoolIntegrity && (bMemoryAllocationNumber % bMemoryAutomaticVerifyPoolIntegrity == 0)) {
        bVerifyPoolIntegrity(pool_num);
    }
    if (header->MagicNumber != 0x22) {
#ifdef EA_PLATFORM_WIN32
        __asm int 3
#else
        bBreak();
#endif
    } else {
#ifdef EA_PLATFORM_WIN32
        uint16 front_padding = header->FrontPadding;
        void *allocated_pointer = reinterpret_cast<char *>(header) - front_padding;
#elif !defined(MILESTONE_BUILD) || defined(EA_PLATFORM_XENON) || (defined(EA_PLATFORM_PLAYSTATION2) && !defined(EA_BUILD_A124))
        void *allocated_pointer = header->GetBottomAddress();
#endif
#if defined(MILESTONE_BUILD) && !defined(EA_PLATFORM_XENON) && (!defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_BUILD_A124))
        MemoryPool *pool = MemoryPools[pool_num];
#endif
        header->MagicNumber = 0;
        pool->RemoveAllocationHeader(header);
#if defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124)
        pool->FreeMemory(allocated_pointer, header->Size);
#else
        pool->FreeMemory(allocated_pointer, header->Size, debug_name);
#endif

#ifdef EA_PLATFORM_WIN32
        if (MemoryInitialized && (MemoryPools[0]->GetNumAllocations() == 0)) {
            MemoryPools[0]->Close();
            MemoryPools[0] = nullptr;
            MemoryInitialized = 0;
        }
#endif
    }
}

#endif

size_t bGetMallocSize(const void *ptr) {
    if (ptr != nullptr) {
        const AllocationHeader *header = &static_cast<const AllocationHeader *>(ptr)[-1];
        return header->RequestedSize;
    }
    return 0;
}

int bGetMallocPool(void *ptr) {
    if (ptr != nullptr) {
        AllocationHeader *header = &static_cast<AllocationHeader *>(ptr)[-1];
        return header->PoolNum;
    }
    return 0;
}

int bGetMallocNumber(void *ptr) {
    if (ptr != nullptr) {
        AllocationHeader *header = &static_cast<AllocationHeader *>(ptr)[-1];
        return header->GetAllocationNumber();
    }
    return 0;
}

const char *bGetMallocName(void *ptr) {
    if (ptr != nullptr) {
        AllocationHeader *header = &static_cast<AllocationHeader *>(ptr)[-1];
        return header->GetDebugText();
    }
    return nullptr;
}

int bCountFreeMemory(int pool) {
    if (MemoryPools[pool] == nullptr) {
        MemoryPoolOverrideInfo *override_info = MemoryPoolInfoTable[pool].OverrideInfo;
        if (override_info != nullptr) {
            return override_info->GetAmountFree(override_info->Pool);
        } else {
            return 0;
        }
    }
    return MemoryPools[pool]->GetAmountFree();
}

int bGetPoolSize(int pool) {
    MemoryPool *memory_pool = MemoryPools[pool];
    if (memory_pool == nullptr) {
        return 0;
    }
    return memory_pool->GetPoolSize();
}

int bLargestMalloc(int allocation_params) {
#if defined(EA_PLATFORM_WIN32) || defined(EA_PLATFORM_PLAYSTATION2)
    int pool_num = allocation_params & 0xfU;
    MemoryPool *memory_pool = MemoryPools[pool_num];
    if (memory_pool == nullptr) {
        MemoryPoolOverrideInfo *override_info = MemoryPoolInfoTable[pool_num].OverrideInfo;
        if (override_info != nullptr) {
            return override_info->GetLargestFreeBlock(override_info->Pool);
        } else {
            return 0;
        }
    }
    if (pool_num == 0) {
        return 0x6300000;
    }
    int pool = memory_pool->GetLargestFreeBlock() - 0x5c;
#else
    if (MemoryPools[allocation_params & 0xfU] == nullptr) {
        MemoryPoolOverrideInfo *override_info = MemoryPoolInfoTable[allocation_params & 0xfU].OverrideInfo;
        if (override_info != nullptr) {
            return override_info->GetLargestFreeBlock(override_info->Pool);
        } else {
            return 0;
        }
    }
    int pool = MemoryPools[bMemoryGetPoolNum(allocation_params)]->GetLargestFreeBlock() - 0x5c;
#endif
    int alignment = bMemoryGetAlignment(allocation_params);
    if (alignment == 0) {
        alignment = 16;
    }
#if defined(EA_PLATFORM_WIN32) || defined(EA_PLATFORM_PLAYSTATION2)
    int effective_alignment = alignment;
    if (effective_alignment < 128) {
        effective_alignment = 128;
    }
    int largest_malloc = pool - effective_alignment;
#else
    if (alignment < 128) {
        alignment = 128;
    }
    int largest_malloc = pool - alignment;
#endif
    if (largest_malloc < 0) {
        largest_malloc = 0;
    }
    return largest_malloc;
}

#if defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124)
AllocationHeader *MemoryPool::GetMemoryDumpStatistics(int &num_allocations, int &total_num_allocations, int &amount_allocated,
                                                      int &most_amount_allocated, int &amount_free, const char *&debug_name) {
    num_allocations = this->NumAllocations;
    debug_name = this->pDebugName;
    total_num_allocations = this->TotalNumAllocations;
    amount_allocated = this->AmountAllocated;
    most_amount_allocated = this->MostAmountAllocated;
    amount_free = this->AmountFree;
    return this->AllocationHeaderList.GetHead();
}

void *bMemoryGetStatistics(int pool_num, int &num_allocations, int &total_num_allocations, int &amount_allocated,
                           int &most_amount_allocated, int &amount_free, int &largest_malloc, const char *&debug_name) {
    MemoryPool *memory_pool = MemoryPools[pool_num];
    if (memory_pool != nullptr) {
        AllocationHeader *first_allocation = memory_pool->GetMemoryDumpStatistics(num_allocations, total_num_allocations, amount_allocated,
                                                                                most_amount_allocated, amount_free, debug_name);
        largest_malloc = bLargestMalloc(pool_num);
        return first_allocation;
    }
    return nullptr;
}
#endif

void bVerifyPoolIntegrity(int pool) {
    if (MemoryPools[pool] != nullptr) {
        bool verify_free_pattern = true;
        MemoryPools[pool]->VerifyPoolIntegrity(verify_free_pattern);
    }
}

const char *bGetMemoryPoolName(int pool_num) {
    if (MemoryPools[pool_num] != nullptr) {
        return MemoryPools[pool_num]->GetName();
    }

#ifndef EA_BUILD_A124
    MemoryPoolOverrideInfo *override_info = MemoryPoolInfoTable[pool_num].OverrideInfo;
    if (override_info != nullptr) {
        return override_info->Name;
    }
#endif

    return nullptr;
}

int bGetMemoryPoolNum(const char *memory_pool_name) {
    for (int pool_num = 0; pool_num < 16; pool_num++) {
        if (MemoryPools[pool_num] != nullptr) {
            if (bStrICmp(MemoryPools[pool_num]->GetName(), memory_pool_name) == 0) {
                return pool_num;
            }
        }
#ifndef EA_BUILD_A124
        MemoryPoolOverrideInfo *override_info = MemoryPoolInfoTable[pool_num].OverrideInfo;
        if ((override_info != nullptr) && bStrICmp(override_info->Name, memory_pool_name) == 0) {
            return pool_num;
        }
#endif
    }
    return -1;
}

int bMemoryCountAllocations(const char *debug_text, int pool_num) {
    if (MemoryPools[pool_num] != nullptr) {
        return MemoryPools[pool_num]->CountAllocations(debug_text);
    }
    return 0;
}

int bMemoryGetAllocationNumber() {
    return bMemoryAllocationNumber;
}

void bMemoryPrintAllocations(int pool_num, int from_allocation, int to_allocation) {
    MemoryPools[pool_num]->PrintAllocations(from_allocation, to_allocation);
}

void bMemoryPrintAllocationsByAddress(int pool_num, int from_allocation, int to_allocation) {
    if (MemoryPools[pool_num] != nullptr) {
        MemoryPools[pool_num]->PrintAllocationsByAddress(from_allocation, to_allocation);
    }
}

void *bMemoryFindAllocation(int pool_num, int allocation_num) {
    if (MemoryPools[pool_num] != nullptr) {
        AllocationHeader *header = MemoryPools[pool_num]->FindAllocation(allocation_num);
        if (header != nullptr) {
            return header->GetAllocAddress();
        }
    }
    return nullptr;
}

int bMemoryGetAllocations(int pool_num, void **allocations, int max_allocations) {
    if (MemoryPools[pool_num] != nullptr) {
        return MemoryPools[pool_num]->GetAllocations(allocations, max_allocations);
    }
    return 0;
}

#ifdef EA_BUILD_A124
static char bMemoryDebugStringNoName[8] = "NO_NAME";
#endif

void *bMemoryAllocator::Alloc(size_t size, const EA::TagValuePair &flags) {

#ifdef EA_BUILD_A124
    char *name = bMemoryDebugStringNoName;
#else
    char *name;
#endif
    int allocation_params = BMEMORY_TOP_BIT;
    const EA::TagValuePair *p = &flags;
    void *ptr;

    while (p != nullptr) {
        switch (p->mTag) {
            case 1:
                if (p->mValue.mPointer != nullptr) {
                    name = static_cast<char *>(const_cast<void *>(p->mValue.mPointer));
                }
                break;
            case 2:
                allocation_params |= BMEMORY_ALIGNMENT(p->mValue.mInt);
                break;
            case 3:
                allocation_params |= BMEMORY_ALIGNMENT_OFFSET(p->mValue.mInt);
                break;
            case 4:
                allocation_params &= ~BMEMORY_TOP_BIT;
                break;
        }
        p = p->mNext;
    }
    return bMalloc(size, name, 0, allocation_params);
}

void *bMemoryAllocator::Alloc(size_t size) {
#ifdef MILESTONE_BUILD
    return bMalloc(static_cast<int>(size), "bMemoryAllocator", 0, this->PoolNumber);
#else
    return bWareMalloc(static_cast<int>(size), nullptr, 0, 0);
#endif
}

void bMemoryAllocator::Free(void *pBlock, size_t size) {
    bFree(pBlock);
}

int bMemoryAllocator::AddRef() {
    return ++this->mRefcount;
}

int bMemoryAllocator::Release() {
    this->mRefcount--;
    if (this->mRefcount <= 0) {
        delete this;
        return 0;
    }
    return this->mRefcount;
}

void bMemoryCreatePersistentPool(int size) {
    bMemoryPersistentPoolNumber = bGetFreeMemoryPoolNum();
    void *mem = bMalloc(size, "Persistent Memory Pool", 0, 0);
    bInitMemoryPool(bMemoryPersistentPoolNumber, mem, size, "Persistent Pool");
    TheMemoryPersistentAllocator.SetMemoryPool(bMemoryPersistentPoolNumber);
}
