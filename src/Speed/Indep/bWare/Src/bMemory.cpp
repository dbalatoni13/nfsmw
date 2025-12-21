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
unsigned int MemoryPoolZeroSize = 0;    // size: 0x4, address: 0x8041645C
int bMemoryPersistentPoolNumber = -1;   // size: 0x4, address: 0x80416460

void *bWareMalloc(int size, const char *debug_text, int debug_line, int allocation_params);

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

BOOL CheckFlipMemoryByAddress(AllocationHeader *a, AllocationHeader *b) {
    return a->GetBottomAddress() <= b->GetBottomAddress();
}

BOOL CheckFlipMemoryByAllocationNumber(AllocationHeader *a, AllocationHeader *b) {
    return true;
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
        void *dummy = bMALLOC(0x10, "", 0, (pool_num & 0xf) | 0x40);

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
#ifdef TARGET_GC
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

void bVirtualMemoryManager::bVirtualMemoryManager::Alloc() {
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

#if DEBUG
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
    int pool = bMemoryGetPoolNum(allocation_params);
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
    return bMalloc(size, allocation_params);
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
    void *mem = bMALLOC(size, "", 0, 0);
    bInitMemoryPool(bMemoryPersistentPoolNumber, mem, size, "Persistent Pool");
    TheMemoryPersistentAllocator.SetMemoryPool(bMemoryPersistentPoolNumber);
}
