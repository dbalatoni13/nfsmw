// TODO
// #include <string.h>

#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <types.h>

#ifdef EA_PLATFORM_GAMECUBE
#include "dolphin/os/OSArena.h"
#include <dolphin.h>
#endif

// TODO
extern "C" {
void *memset(void *, int, uintptr_t, ...);
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

    const char *GetDebugText() {
        return "";
    }

    int GetAllocationNumber() {
#ifdef MILESTONE_OPT
        return *reinterpret_cast<uint16 *>(reinterpret_cast<char *>(this) - FrontPadding);
#else
        return 0;
#endif
    }

    int GetDebugLine() {
#ifdef MILESTONE_OPT
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

// total size: 0x60
class MemoryPool {
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
};

int bMemoryAutomaticVerifyPoolIntegrity = 0;                                          // size: 0x4, address: 0x80416418
int bMemoryRandomFillPattern = 0;                                                     // size: 0x4, address: 0x80416430
BOOL bMemoryTracing = false;                                                          // size: 0x4, address: 0x80416438
int bMemoryBreakOnAllocationNumber = -1;                                              // size: 0x4, address: 0x8041643C
int bMemoryAllocationNumber = 0;                                                      // size: 0x4, address: 0x80416440
bMemoryAllocator TheMemoryAllocator;                                                  // size: 0xC, address: 0x8045790C
EA::Allocator::IAllocator &gMemoryAllocator = TheMemoryAllocator;                     // size: 0x4, address: 0x80457918
bMemoryAllocator TheMemoryPersistentAllocator;                                        // size: 0xC, address: 0x8045791C
EA::Allocator::IAllocator &gMemoryPersistentAllocator = TheMemoryPersistentAllocator; // size: 0x4, address: 0x80457928
// static const int bMemoryEnableFancyStompDetector; // size: 0x4
const char *pTraceDebugText = nullptr;  // size: 0x4, address: 0x80416450
int TraceDebugLine = 0;                 // size: 0x4, address: 0x80416454
int MemoryInitialized = 0;              // size: 0x4, address: 0x80416458
char MemoryPoolMem[16][96];             // size: 0x600, address: 0x8045A20D
MemoryPool *MemoryPools[16];            // size: 0x40, address: 0x8045A810
MemoryPoolInfo MemoryPoolInfoTable[16]; // size: 0x100, address: 0x8045A850
bVirtualMemoryManager eARAMMM;          // size: 0x18, address: 0x8045A950
unsigned int MemoryPoolZeroSize = 0;    // size: 0x4, address: 0x8041645C
int bMemoryPersistentPoolNumber = -1;   // size: 0x4, address: 0x80416460

void *bWareMalloc(int size, const char *debug_text, int debug_line, int allocation_params);

void bFunkGameCube(const char *server_name, unsigned char function_num, const void *param_buffer, long param_size) {}

int GetAlignmentAdjustTop(intptr_t address, int alignment, int alignment_offset) {
    return (alignment - (address + alignment_offset & alignment - 1)) % alignment;
}

int GetAlignmentAdjustBottom(intptr_t address, int alignment, int alignment_offset) {
    return (address + alignment_offset & alignment - 1) % alignment;
}

void MemoryPool::Init(void *memory, int memory_size, const char *debug_name) {
    this->FreeBlockList.InitList();
    this->AllocationHeaderList.InitList();
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
    this->TraceDeletePool();
    this->Mutex.Destroy();
}

void MemoryPool::AddMemory(void *p, int size) {
    this->PoolSize += size;
    this->AddFreeMemory(p, size, this->GetName());
}

void MemoryPool::FreeMemory(void *p, int size, const char *debug_name) {
    this->Mutex.Lock();
    this->AddFreeMemory(p, size, debug_name);
    this->AmountAllocated -= size;
    this->NumAllocations--;
    this->AmountFree = this->PoolSize - this->AmountAllocated;
    this->Mutex.Unlock();
}

void MemoryPool::AddFreeMemory(void *p, int size, const char *debug_name) {
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
                bMemSet(fill_bot, 0xee, fill_top - fill_bot);
            }
        }

        this->Mutex.Unlock();
    }
}

void *MemoryPool::AllocateMemory(int size, int alignment, int alignment_offset, int start_from_top, int use_best_fit, int *new_size) {
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

            if (((amount_leftover == 0) || (amount_leftover > 0xf)) && (amount_leftover < best_amount_leftover)) {
                best_amount_leftover = amount_leftover;
                best_alignment_adjust = alignment_adjust;
                best_free_block = f;
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

            if (((amount_leftover == 0) || (amount_leftover > 15)) && (amount_leftover < best_amount_leftover)) {
                best_amount_leftover = amount_leftover;
                best_alignment_adjust = alignment_adjust;
                best_free_block = f;
                if (use_best_fit == 0) {
                    break;
                }
            }
        }
    }

    if (best_free_block == nullptr) {
        this->Mutex.Unlock();
        return nullptr;
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

        if (best_free_block->Size != size) {
            FreeBlock *new_free_block = reinterpret_cast<FreeBlock *>(reinterpret_cast<char *>(best_free_block) + size);
            CheckFancyStompDetector(new_free_block, sizeof(FreeBlock));

            new_free_block->Size = best_free_block->Size - size;
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

    // TODO milestone
    if (bIsBFunkAvailable()) {
        // TraceAllocateMemory()
    }

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
    int amount_free = 0;

    this->Mutex.Lock();
    for (FreeBlock *f = this->FreeBlockList.GetHead(); f != this->FreeBlockList.EndOfList(); f = f->GetNext()) {
        amount_free += f->Size;
    }
    this->Mutex.Unlock();
    return amount_free;
}

int MemoryPool::GetLargestFreeBlock() {
    int largest_block = 0;

    this->Mutex.Lock();
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
        bBreak();
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

int CheckFlipMemoryByAddress(AllocationHeader *a, AllocationHeader *b) {
    return a->GetBottomAddress() <= b->GetBottomAddress();
}

int CheckFlipMemoryByAllocationNumber(AllocationHeader *a, AllocationHeader *b) {
    return true;
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
            if (prev_header) {
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

bool MemoryPool::CheckFancyStompDetector(const void *mem, int mem_size) {
    return false;
}

void MemoryPool::TraceNewPool() {
    bMemoryTraceNewPoolPacket packet;
    packet.PoolID = reinterpret_cast<uintptr_t>(this);
    bMemSet(packet.Name, 0, sizeof(packet.Name));
    if (this->pDebugName) {
        bStrNCpy(packet.Name, this->pDebugName, sizeof(packet.Name) - 1);
    }
    bFunkGameCube("CODEINE", 25, &packet, sizeof(packet));
}

void MemoryPool::TraceDeletePool() {
    bMemoryTraceDeletePoolPacket packet;
    packet.PoolID = reinterpret_cast<uintptr_t>(this);
    bFunkGameCube("CODEINE", 26, &packet, sizeof(packet));
}

void MemoryPool::TraceFreeMemory(void *p, int size) {
    bMemoryTraceFreePacket packet;
    // TODO
    bMemoryTraceFreePacket *fake_match = &packet;

    memset(fake_match, 0, sizeof(packet));
    packet.PoolID = reinterpret_cast<uintptr_t>(this);
    packet.MemoryAddress = reinterpret_cast<uintptr_t>(p);
    packet.Size = size;
    bFunkGameCube("CODEINE", 27, fake_match, sizeof(packet));
}

int bGetFreeMemoryPoolNum() {
    for (int pool_num = 0; pool_num < 16; pool_num++) {
        MemoryPoolInfo *info = &MemoryPoolInfoTable[pool_num];
        if (info->NumberReserved == 0 && !MemoryPools[pool_num] && !info->OverrideInfo) {
            return pool_num;
        }
    }
    return -1;
}

void bReserveMemoryPool(int pool_num) {
    MemoryPoolInfo *info = &MemoryPoolInfoTable[pool_num];
    info->NumberReserved = 1;
}

void bSetMemoryPoolOverrideInfo(int pool_num, MemoryPoolOverrideInfo *override_info) {
    MemoryPoolInfo *info = &MemoryPoolInfoTable[pool_num];
    info->OverrideInfo = override_info;
    info->OverflowPoolNumber = -1;
}

void bInitMemoryPool(int pool_num, void *mem, int mem_size, const char *debug_name) {
    MemoryPoolInfo *info = &MemoryPoolInfoTable[pool_num];
    info->TopMeansLargerAddress = 0;
    info->OverflowPoolNumber = -1;
    MemoryPools[pool_num] = reinterpret_cast<MemoryPool *>(MemoryPoolMem[pool_num]);
    reinterpret_cast<MemoryPool *>(MemoryPoolMem[pool_num])->Init(mem, mem_size, debug_name);
    if (pool_num == 0) {
        MemoryPoolZeroSize = mem_size;
    }
}

void bCloseMemoryPool(int pool_num) {
    MemoryPools[pool_num]->Close();
    MemoryPools[pool_num] = nullptr;
}

bool bSetMemoryPoolDebugFill(int pool_num, bool on_off) {
    return MemoryPools[pool_num]->SetDebugFill(on_off);
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
    this->bIsValid = false;
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
        MemoryInitialized = true;
    }
#else
    // TODO
#endif
}

#ifdef MILESTONE_OPT
void *bMalloc(int size, const char *debug_text, int debug_line, int allocation_params) {
    return bWareMalloc(size, debug_text, debug_line, allocation_params);
}
#else
void *bMalloc(int size, int allocation_params) {
    return bWareMalloc(size, nullptr, 0, allocation_params);
}
#endif

// TODO variable names
void *bWareMalloc(int size, const char *debug_text, int debug_line, int allocation_params) {
    int pool_num = bMemoryGetPoolNum(allocation_params);
    MemoryPoolInfo *info = &MemoryPoolInfoTable[pool_num];

    if (info->OverflowPoolNumber != -1) {
        int overflow_allocation_params = (allocation_params & ~0xf) | (info->OverflowPoolNumber & 0xf);
        if (bLargestMalloc(overflow_allocation_params) > size) {
            info = &MemoryPoolInfoTable[info->OverflowPoolNumber];
            allocation_params = overflow_allocation_params;
        }
    }

    if (info->TopMeansLargerAddress != 0) {
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
        if (bMemoryAllocationNumber == (bMemoryAllocationNumber / bMemoryAutomaticVerifyPoolIntegrity) * bMemoryAutomaticVerifyPoolIntegrity) {
            bVerifyPoolIntegrity(bMemoryGetPoolNum(allocation_params));
        }
    }

    // TODO rename
    int alignment = bMemoryGetAlignment(allocation_params);
    if (alignment == 0) {
        alignment = 16;
    }

    // TODO rename
    int allocation_header_offset = bMemoryGetAlignmentOffset(allocation_params) + 0x14;
    int new_size;
    pool_num = bMemoryGetPoolNum(allocation_params);
    MemoryPool *pool = MemoryPools[pool_num];
    void *memory =
        pool->AllocateMemory(size + 0x14, alignment, allocation_header_offset, allocation_params & 0x40, allocation_params & 0x80, &new_size);

    if (memory != nullptr) {
        int padding = 0;
        int front_padding; // TODO
        if (allocation_params & 0x40) {
            padding = GetAlignmentAdjustTop(reinterpret_cast<intptr_t>(memory), alignment, allocation_header_offset);
        }

        AllocationHeader *header = reinterpret_cast<AllocationHeader *>(reinterpret_cast<char *>(memory) + padding);
        pool->AddAllocationHeader(header);

        header->PoolNum = pool_num;
        header->MagicNumber = 0x22;
        header->FrontPadding = padding;

        header->Size = new_size;
        header->RequestedSize = size;
        if (bMemoryAllocationNumber == bMemoryBreakOnAllocationNumber) {
            bBreak();
        }

        bMemoryAllocationNumber++;
        return &header[1];
    }

    bReleasePrintf("ERROR:  Out of memory in pool %s allocating %s (size = %d).  Largest possible = %d  Total = %d", pool->GetName(), debug_text,
                   size, bLargestMalloc(allocation_params), bCountFreeMemory(pool_num));
    bMemoryPrintAllocationsByAddress(pool_num, 0, 0x7fffffff);
    bBreak();
    return nullptr;
}

void bFree(void *ptr) {
    if (!ptr) {
        return;
    }
    for (int n = 0; n < 16; n++) {
        MemoryPoolOverrideInfo *override_info = MemoryPoolInfoTable[n].OverrideInfo;
        intptr_t address = reinterpret_cast<intptr_t>(ptr);
        if (override_info && (address >= override_info->Address) && (address < override_info->Address + override_info->Size)) {
            int pool_num = 1;

            while (pool_num < 16) {
                MemoryPool *pool = MemoryPools[pool_num];

                if (!pool || !pool->IsInPool(address)) {
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
    int pool_num = header->PoolNum;
    MemoryPool *pool = MemoryPools[pool_num];
    char debug_name[32];
    memset(debug_name, 0, sizeof(debug_name));
    if (bMemoryAutomaticVerifyPoolIntegrity && (bMemoryAllocationNumber % bMemoryAutomaticVerifyPoolIntegrity == 0)) {
        bVerifyPoolIntegrity(pool_num);
    }
    if (header->MagicNumber != 0x22) {
        bBreak();
    } else {
        void *allocated_pointer = header->GetBottomAddress();
        header->MagicNumber = 0;
        pool->RemoveAllocationHeader(header);
        pool->FreeMemory(allocated_pointer, header->Size, debug_name);
    }
}

size_t bGetMallocSize(const void *ptr) {
    if (ptr) {
        const AllocationHeader *header = &static_cast<const AllocationHeader *>(ptr)[-1];
        return header->RequestedSize;
    }
    return 0;
}

int bGetMallocPool(void *ptr) {
    if (ptr) {
        AllocationHeader *header = &static_cast<AllocationHeader *>(ptr)[-1];
        return header->PoolNum;
    }
    return 0;
}

const char *bGetMallocName(void *ptr) {
    if (ptr) {
        AllocationHeader *header = &static_cast<AllocationHeader *>(ptr)[-1];
        return header->GetDebugText();
    }
    return nullptr;
}

int bCountFreeMemory(int pool) {
    if (!MemoryPools[pool]) {
        MemoryPoolOverrideInfo *override_info = MemoryPoolInfoTable[pool].OverrideInfo;
        if (override_info) {
            return override_info->GetAmountFree(override_info->Pool);
        } else {
            return 0;
        }
    }
    return MemoryPools[pool]->GetAmountFree();
}

int bLargestMalloc(int allocation_params) {
    if (!MemoryPools[allocation_params & 0xfU]) {
        MemoryPoolOverrideInfo *override_info = MemoryPoolInfoTable[allocation_params & 0xfU].OverrideInfo;
        if (override_info) {
            return override_info->GetLargestFreeBlock(override_info->Pool);
        } else {
            return 0;
        }
    }
    int pool = MemoryPools[bMemoryGetPoolNum(allocation_params)]->GetLargestFreeBlock() - 0x5c;
    int alignment = bMemoryGetAlignment(allocation_params);
    if (alignment == 0) {
        alignment = 16;
    }
    if (alignment < 128) {
        alignment = 128;
    }
    int largest_malloc = pool - alignment;
    if (largest_malloc < 0) {
        largest_malloc = 0;
    }
    return largest_malloc;
}

void bVerifyPoolIntegrity(int pool) {
    if (MemoryPools[pool]) {
        bool verify_free_pattern = true;
        MemoryPools[pool]->VerifyPoolIntegrity(verify_free_pattern);
    }
}

int bGetMemoryPoolNum(const char *memory_pool_name) {
    for (int pool_num = 0; pool_num < 16; pool_num++) {
        if (MemoryPools[pool_num]) {
            if (bStrICmp(MemoryPools[pool_num]->GetName(), memory_pool_name) == 0) {
                return pool_num;
            }
        }
        MemoryPoolOverrideInfo *override_info = MemoryPoolInfoTable[pool_num].OverrideInfo;
        if (override_info && bStrICmp(override_info->Name, memory_pool_name) == 0) {
            return pool_num;
        }
    }
    return -1;
}

int bMemoryGetAllocationNumber() {
    return bMemoryAllocationNumber;
}

void bMemoryPrintAllocations(int pool_num, int from_allocation, int to_allocation) {
    MemoryPools[pool_num]->PrintAllocations(from_allocation, to_allocation);
}

void bMemoryPrintAllocationsByAddress(int pool_num, int from_allocation, int to_allocation) {
    if (MemoryPools[pool_num]) {
        MemoryPools[pool_num]->PrintAllocationsByAddress(from_allocation, to_allocation);
    }
}

int bMemoryGetAllocations(int pool_num, void **allocations, int max_allocations) {
    if (MemoryPools[pool_num]) {
        return MemoryPools[pool_num]->GetAllocations(allocations, max_allocations);
    }
    return 0;
}

static char bMemoryDebugStringNoName[8] = "NO_NAME";

// UNSOLVED
void *bMemoryAllocator::Alloc(unsigned int size, const EA::TagValuePair &flags) {
    // TODO magic numbers (flags)
    int allocation_params = 0x40;
    const EA::TagValuePair *p = &flags;

    for (; p; p = p->mNext) {
        switch (p->mTag) {
            case 0:
            case 1:
                break;
            case 2:
                allocation_params |= (p->mValue.mSize & 0x1ffc) << 6;
                break;
            case 3:
                allocation_params |= (p->mValue.mSize & 0x1ffc) << 0x11;
                break;
            case 4:
                allocation_params &= ~0x40;
                break;
        }
    }
    return bMalloc(size, bMemoryDebugStringNoName, 0, allocation_params);
}

void bMemoryAllocator::Free(void *pBlock, unsigned int size) {
    bFree(pBlock);
}

int bMemoryAllocator::AddRef() {
    return ++this->mRefcount;
}

int bMemoryAllocator::Release() {
    this->mRefcount--;
    if (this->mRefcount < 1) {
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
