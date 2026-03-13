// TODO
// #include <string.h>

#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <types.h>

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin.h>
#endif

// TODO
extern "C" {
void *memset(void *, int, uintptr_t, ...);
void VMInit(size_t, uintptr_t, size_t);
BOOL VMAlloc(uintptr_t, size_t);
}

int bMemoryAutomaticVerifyPoolIntegrity = 0;   // size: 0x4, address: 0x80416418
int bMemoryRandomFillPattern = 0;              // size: 0x4, address: 0x80416430
BOOL bMemoryTracing = false;                   // size: 0x4, address: 0x80416438
int bMemoryBreakOnAllocationNumber = -1;       // size: 0x4, address: 0x8041643C
int bMemoryAllocationNumber = 0;               // size: 0x4, address: 0x80416440
bMemoryAllocator TheMemoryAllocator;           // size: 0xC, address: 0x8045790C
bMemoryAllocator TheMemoryPersistentAllocator; // size: 0xC, address: 0x8045791C
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
static int found_memory_stomp_once_3842 = 0;

void *bWareMalloc(int size, const char *debug_text, int debug_line, int allocation_params);
void bMemoryInit();

void bFunkGameCube(const char *server_name, unsigned char function_num, const void *param_buffer, long param_size) {}

int GetAlignmentAdjustTop(int address, int alignment, int alignment_offset) {
    return (alignment - (address + alignment_offset & alignment - 1)) % alignment;
}

int GetAlignmentAdjustBottom(int address, int alignment, int alignment_offset) {
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

        if ((bMemoryTracing != 0) && this->DebugTracingEnabled) {
            this->TraceFreeMemory(p, size);
        }

        FreeBlock *free_block = static_cast<FreeBlock *>(p);
        free_block->Size = size;
        free_block->MagicNumber = 0x44443333;

        FreeBlock *insert_point = this->FreeBlockList.GetHead();
        while ((insert_point != this->FreeBlockList.EndOfList()) &&
               (reinterpret_cast<uintptr_t>(insert_point) <= reinterpret_cast<uintptr_t>(free_block))) {
            insert_point = insert_point->GetNext();
        }

        FreeBlock *prev_block = insert_point->GetPrev();
        prev_block->Next = free_block;
        insert_point->Prev = free_block;
        free_block->Prev = prev_block;
        free_block->Next = insert_point;

        int block_size = free_block->Size;
        FreeBlock *next_block = reinterpret_cast<FreeBlock *>(reinterpret_cast<char *>(free_block) + block_size);
        void *fill_end = next_block;
        FreeBlock *fill_start = free_block + 1;

        if (next_block == insert_point) {
            fill_end = next_block + 1;
            free_block->Size = block_size + next_block->Size;
            next_block->Remove();
        }

        prev_block = free_block->GetPrev();
        if ((reinterpret_cast<char *>(prev_block) + prev_block->Size) == reinterpret_cast<char *>(free_block)) {
            prev_block->Size = prev_block->Size + free_block->Size;
            next_block = free_block->GetNext();
            free_block->Remove();
            fill_start = free_block;
            free_block = prev_block;
        }

        if (this->DebugFillEnabled) {
            unsigned int fill_size = reinterpret_cast<char *>(fill_end) - reinterpret_cast<char *>(fill_start);

            if (bMemoryRandomFillPattern == 0) {
                bMemSet(fill_start, 0xee, fill_size);
            } else {
                bMemSet(fill_start, bGetTicker(), fill_size);
            }
        }

        this->Mutex.Unlock();
    }
}

void *MemoryPool::AllocateMemory(int size, int alignment, int alignment_offset, int start_from_top, int use_best_fit, int *new_size) {
    this->Mutex.Lock();

    unsigned int actual_size = (size + 3U) & 0xfffffffc;
    if (static_cast<int>(actual_size) < 0x10) {
        actual_size = 0x10;
    }

    FreeBlock *best_block = nullptr;
    int best_align_adjust = 0;
    int best_remaining = 0x7fffffff;
    void *mem_bottom;

    if (start_from_top != 0) {
        for (FreeBlock *block = this->FreeBlockList.GetHead(); block != this->FreeBlockList.EndOfList(); block = block->GetNext()) {
            int align_adjust = GetAlignmentAdjustTop(reinterpret_cast<intptr_t>(block), alignment, alignment_offset);
            int remaining = block->Size - (actual_size + align_adjust);

            if (((remaining == 0) || (remaining > 0xf)) && (remaining < best_remaining)) {
                best_remaining = remaining;
                best_align_adjust = align_adjust;
                best_block = block;
                if (use_best_fit == 0) {
                    break;
                }
            }
        }
    } else {
        for (FreeBlock *block = this->FreeBlockList.GetTail(); block != this->FreeBlockList.EndOfList(); block = block->GetPrev()) {
            mem_bottom = reinterpret_cast<char *>(block) + block->Size;
            int align_adjust = GetAlignmentAdjustBottom(reinterpret_cast<intptr_t>(mem_bottom) - actual_size, alignment, alignment_offset);
            int remaining = block->Size - (actual_size + align_adjust);

            if (((remaining == 0) || (remaining > 0xf)) && (remaining < best_remaining)) {
                best_remaining = remaining;
                best_align_adjust = align_adjust;
                best_block = block;
                if (use_best_fit == 0) {
                    break;
                }
            }
        }
    }

    if (best_block == nullptr) {
        this->Mutex.Unlock();
        return nullptr;
    }

    int allocated_size = actual_size + best_align_adjust;

    if (start_from_top == 0) {
        int remaining = best_block->Size - allocated_size;
        mem_bottom = reinterpret_cast<char *>(best_block) + remaining;
        best_block->Size = remaining;

        if (remaining == 0) {
            best_block->Remove();
            this->CheckFancyStompDetector(reinterpret_cast<FreeBlock *>(mem_bottom) + 1, allocated_size - 0x10);
        } else {
            this->CheckFancyStompDetector(mem_bottom, allocated_size);
        }
        best_block = reinterpret_cast<FreeBlock *>(mem_bottom);
    } else {
        this->CheckFancyStompDetector(best_block + 1, allocated_size - 0x10);

        if (best_block->Size != allocated_size) {
            FreeBlock *next_block = reinterpret_cast<FreeBlock *>(reinterpret_cast<char *>(best_block) + allocated_size);
            this->CheckFancyStompDetector(next_block, 0x10);

            int old_size = best_block->Size;
            next_block->MagicNumber = 0x44443333;
            next_block->Size = old_size - allocated_size;

            FreeBlock *old_next = best_block->GetNext();
            best_block->Next = next_block;
            old_next->Prev = next_block;
            next_block->Prev = best_block;
            next_block->Next = old_next;
        }

        best_block->Remove();
    }

    if (this->DebugFillEnabled) {
        if (bMemoryRandomFillPattern != 0) {
            bMemSet(best_block, bGetTicker(), allocated_size);
        } else {
            bMemSet(best_block, 0xaa, allocated_size);
        }
    }

    int amount_allocated = this->AmountAllocated + allocated_size;
    this->NumAllocations++;
    this->TotalNumAllocations++;
    this->AmountAllocated = amount_allocated;
    if (this->MostAmountAllocated < amount_allocated) {
        this->MostAmountAllocated = amount_allocated;
    }
    this->AmountFree = this->PoolSize - this->AmountAllocated;
    this->LeastAmountFree = this->PoolSize - this->MostAmountAllocated;

    this->Mutex.Unlock();
    *new_size = allocated_size;
    return best_block;
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
    for (FreeBlock *free_block = this->FreeBlockList.GetHead(); free_block != this->FreeBlockList.EndOfList();
         free_block = free_block->GetNext()) {
        if ((bIsValidPointer(free_block, 1) == 0) || (bIsValidPointer(free_block->Next, 1) == 0) ||
            (bIsValidPointer(free_block->Prev, 1) == 0) || (free_block->MagicNumber != 0x44443333)) {
            errors++;
        } else {
            if (found_memory_stomp_once_3842 != 0) {
                break;
            }

            if (verify_free_pattern && this->DebugFillEnabled && (bMemoryRandomFillPattern == 0)) {
                int *p = reinterpret_cast<int *>(free_block + 1);
                int *end = reinterpret_cast<int *>(reinterpret_cast<char *>(free_block) + free_block->Size);

                while (p != end) {
                    if (*p != static_cast<int>(0xeeeeeeee)) {
                        found_memory_stomp_once_3842 = 1;
                        errors++;
                        break;
                    }
                    p++;
                }
            }
        }

        if (errors != 0) {
            break;
        }
    }

    if (errors != 0) {
        bBreak();
        this->PrintAllocationsByAddress(0, 0x7fffffff);
    }

    for (AllocationHeader *header = this->AllocationHeaderList.GetHead(); header != this->AllocationHeaderList.EndOfList();
         header = header->GetNext()) {
        if ((bIsValidPointer(header, 1) == 0) || (bIsValidPointer(header->Next, 1) == 0) || (bIsValidPointer(header->Prev, 1) == 0) ||
            (header->MagicNumber != 0x22)) {
            errors++;
        }

        if (errors != 0) {
            bBreak();
            break;
        }
    }

    this->Mutex.Unlock();
}

BOOL CheckFlipMemoryByAddress(AllocationHeader *a, AllocationHeader *b) {
    return a->GetBottomAddress() <= b->GetBottomAddress();
}

BOOL CheckFlipMemoryByAllocationNumber(AllocationHeader *a, AllocationHeader *b) {
    return true;
}

void MemoryPool::PrintAllocationsByAddress(int from_allocation, int to_allocation) {
    this->AllocationHeaderList.Sort(CheckFlipMemoryByAddress);
    bReleasePrintf("\nMemoryPool: \"%s\"\n", this->pDebugName);
    bReleasePrintf("AllocationNumber Address      Size    Debug Text (& Line)\n");
    bReleasePrintf("=========================================================\n");

    AllocationHeader *prev = nullptr;
    for (AllocationHeader *header = this->AllocationHeaderList.GetHead(); header != this->AllocationHeaderList.EndOfList(); header = header->GetNext()) {
        AllocationHeader *bottom = reinterpret_cast<AllocationHeader *>(header->GetBottomAddress());
        unsigned short allocation_number = *reinterpret_cast<unsigned short *>(bottom);

        if (allocation_number >= from_allocation && allocation_number < to_allocation) {
            if (prev != nullptr) {
                char *prev_bottom = reinterpret_cast<char *>(prev->GetBottomAddress());
                char *curr_bottom = reinterpret_cast<char *>(bottom);
                if ((curr_bottom - prev_bottom) != prev->Size) {
                    bReleasePrintf("   *** gap in allocations ***");
                }
            }

            bReleasePrintf("    %5d        0x%08x %7d   %s", allocation_number, bottom, header->Size, header->GetDebugText());
            if (header->FrontPadding != 0) {
                bReleasePrintf("*");
            }
            bReleasePrintf("\n");
            prev = header;
        } else {
            prev = nullptr;
        }
    }
}

// UNSOLVED
void MemoryPool::PrintAllocations(int from_allocation, int to_allocation) {
    this->AllocationHeaderList.Sort(CheckFlipMemoryByAllocationNumber);
    bReleasePrintf("\nMemoryPool: \"%s\"\n");
    bReleasePrintf("AllocationNumber Address      Size    Debug Text (& Line)\n");
    bReleasePrintf("=========================================================\n");
    for (AllocationHeader *header = this->AllocationHeaderList.GetHead(); header != this->AllocationHeaderList.EndOfList();
         header = header->GetNext()) {
        if ((from_allocation < 1) && (to_allocation > 0)) {
            bReleasePrintf("    %5d        0x%08x %7d   %s");
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
        void *dummy = bMalloc(0x10, "TODO", __LINE__, (pool_num & 0xf) | 0x40);

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

#ifdef MILESTONE_OPT
void *bMalloc(int size, const char *debug_text, int debug_line, int allocation_params) {
    return bWareMalloc(size, debug_text, debug_line, allocation_params);
}
#else
void *bMalloc(int size, int allocation_params) {
    return bWareMalloc(size, nullptr, 0, allocation_params);
}
#endif

// TODO
void *bWareMalloc(int size, const char *debug_text, int debug_line, int allocation_params) {
    MemoryPoolInfo *info = &MemoryPoolInfoTable[allocation_params & 0xf];

    if (info->OverflowPoolNumber != -1) {
        int overflow_params = (allocation_params & ~0xf) | (info->OverflowPoolNumber & 0xf);
        if (bLargestMalloc(overflow_params) > size) {
            info = &MemoryPoolInfoTable[info->OverflowPoolNumber];
            allocation_params = overflow_params;
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
        if (bMemoryAllocationNumber ==
            (bMemoryAllocationNumber / bMemoryAutomaticVerifyPoolIntegrity) * bMemoryAutomaticVerifyPoolIntegrity) {
            bVerifyPoolIntegrity(allocation_params & 0xf);
        }
    }

    int alignment = allocation_params >> 6 & 0x1ffc;
    if (alignment == 0) {
        alignment = 0x10;
    }

    int pool_num = allocation_params & 0xf;
    MemoryPool *pool = MemoryPools[pool_num];
    int allocation_header_offset = (allocation_params >> 0x11 & 0x1ffc) + 0x14;
    int new_size;
    void *memory =
        pool->AllocateMemory(size + 0x14, alignment, allocation_header_offset, allocation_params & 0x40, allocation_params & 0x80, &new_size);

    if (memory != nullptr) {
        int front_padding = 0;
        if ((allocation_params & 0x40) != 0) {
            front_padding = GetAlignmentAdjustTop(reinterpret_cast<intptr_t>(memory), alignment, allocation_header_offset);
        }

        AllocationHeader *tail = pool->AllocationHeaderList.GetTail();
        AllocationHeader *header = reinterpret_cast<AllocationHeader *>(reinterpret_cast<char *>(memory) + front_padding);
        AllocationHeader *head = pool->AllocationHeaderList.EndOfList();

        tail->Next = header;
        pool->AllocationHeaderList.HeadNode.Prev = header;
        header->Prev = tail;
        header->Next = head;
        header->PoolNum = pool_num;
        header->MagicNumber = 0x22;
        header->FrontPadding = front_padding;

        bool break_on_allocation = bMemoryAllocationNumber == bMemoryBreakOnAllocationNumber;
        header->RequestedSize = size;
        header->Size = new_size;
        if (break_on_allocation) {
            bBreak();
        }

        bMemoryAllocationNumber++;
        return &header[1];
    }

    bReleasePrintf("ERROR:  Out of memory in pool %s allocating %s (size = %d).  Largest possible = %d  Total = %d", pool->GetName(),
                   debug_text, size, bLargestMalloc(allocation_params), bCountFreeMemory(pool_num));
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
    return bMalloc(size, "TODO", __LINE__, allocation_params);
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

void bMemoryInit() {
    if (MemoryInitialized == 0) {
        int main_pool_size = PlatformMemoryINIT();
        void *arena_lo = OSGetArenaLo();
        void *arena_hi = OSGetArenaHi();

        OSSetArenaLo(OSInitAlloc(arena_lo, arena_hi, 1));
        eARAMMM.Init();
        eARAMMM.Alloc();

        uintptr_t heap_lo = (reinterpret_cast<uintptr_t>(OSGetArenaLo()) + 0x1fU) & 0xffffffe0;
        uintptr_t heap_hi = reinterpret_cast<uintptr_t>(OSGetArenaHi()) & 0xffffffe0;
        int available_size = heap_hi - heap_lo;

        if ((main_pool_size < 1) || (available_size <= main_pool_size)) {
            main_pool_size = available_size;
        }

        OSHeapHandle heap = OSCreateHeap(reinterpret_cast<void *>(heap_lo), reinterpret_cast<void *>(heap_hi));
        OSSetCurrentHeap(heap);
        OSSetArenaLo(reinterpret_cast<void *>(heap_hi));

        if (bMemoryTracing != 0) {
            bFunkGameCube("CODEINE", 24, 0, 0);
        }

        void *main_memory = OSAllocFromHeap(heap, main_pool_size - 0x40000);
        bInitMemoryPool(0, main_memory, main_pool_size - 0x40000, "Main Pool");
        bInitMemoryPool(GetVirtualMemoryPoolNumber(), reinterpret_cast<void *>(eARAMMM.mVirtualBaseAddr), eARAMMM.mARamSize,
                        GetVirtualMemoryPoolName());
        MemoryInitialized = 1;
    }
}

void bMemoryCreatePersistentPool(int size) {
    bMemoryPersistentPoolNumber = bGetFreeMemoryPoolNum();
    void *mem = bMalloc(size, "TODO", __LINE__, 0);
    bInitMemoryPool(bMemoryPersistentPoolNumber, mem, size, "Persistent Pool");
    TheMemoryPersistentAllocator.SetMemoryPool(bMemoryPersistentPoolNumber);
}
