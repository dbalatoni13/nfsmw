#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "dolphin/types.h"

SlotPoolManager TheSlotPoolManager;

SlotPool *bNewSlotPool(int slot_size, int num_slots, const char *debug_name, int memory_pool) {
    return TheSlotPoolManager.NewSlotPool(slot_size, num_slots, debug_name, memory_pool);
}

void bDeleteSlotPool(SlotPool *slot_pool) {
    TheSlotPoolManager.DeleteSlotPool(slot_pool);
}

void *bMalloc(SlotPool *slot_pool) {
    return slot_pool->FastMalloc();
}

void *bOMalloc(SlotPool *slot_pool) {
    return slot_pool->Malloc();
}

void bFree(SlotPool *slot_pool, void *p) {
    slot_pool->Free(p);
}

int bIsSlotPoolFull(SlotPool *slot_pool) {
    return slot_pool->IsFull();
}

int bCountFreeSlots(SlotPool *slot_pool) {
    return slot_pool->CountFreeSlots();
}

int bCountTotalSlots(SlotPool *slot_pool) {
    return slot_pool->CountTotalSlots();
}

SlotPool *SlotPool::NewSlotPool(int slot_size, int num_slots, const char *debug_name, int memory_pool) {
    // TODO figure out magic number 0x30, 0x4 less than sizeof(SlotPool)
    SlotPool *slot_pool = reinterpret_cast<SlotPool *>(bMALLOC(slot_size * num_slots + 0x30, "", 0, memory_pool));
    if (slot_pool) {
        slot_pool->SlotSize = slot_size;
        slot_pool->MemoryPool = memory_pool;
        slot_pool->DebugName = debug_name;
        slot_pool->NumSlots = num_slots;
        slot_pool->TotalNumSlots = num_slots;
        slot_pool->Flags = static_cast<SlotPoolFlags>(SLOTPOOL_FLAG_WARN_IF_NONEMPTY_DELETE | SLOTPOOL_FLAG_WARN_IF_OVERFLOW |
                                                      SLOTPOOL_FLAG_ZERO_ALLOCATED_MEMORY | SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
        slot_pool->FreeSlots = nullptr;
        slot_pool->NextSlotPool = nullptr;
        slot_pool->NumAllocatedSlots = 0;
        slot_pool->MostNumAllocatedSlots = 0;
        slot_pool->FlushSlotPool();
    }
    return slot_pool;
}

void SlotPool::DeleteSlotPool(SlotPool *slot_pool) {
    if (slot_pool) {
        bFree(slot_pool);
    }
}

void SlotPool::FlushSlotPool() {
    SlotPool *slot_pool = this;
    while (slot_pool) {
        int slot_size = slot_pool->SlotSize;
        int num_slots;
        slot_pool->FreeSlots = nullptr;
        if (slot_pool->NumSlots != 0) {
            num_slots = slot_pool->NumSlots - 1;
            SlotPoolEntry *slot = slot_pool->Slots;
            slot_pool->FreeSlots = slot;
            for (int n = 0; n < num_slots; n++) {
                slot = slot->Next = reinterpret_cast<SlotPoolEntry *>(reinterpret_cast<char *>(&slot->Next) + slot_size);
            }
            slot->Next = nullptr;
        }
        slot_pool = slot_pool->NextSlotPool;
    }
    this->NumAllocatedSlots = 0;
}

// UNSOLVED
void SlotPool::ExpandSlotPool(int num_extra_slots) {
    int uVar2;

    SlotPool *new_slot_pool = NewSlotPool(this->SlotSize, num_extra_slots, this->DebugName, this->MemoryPool);
    new_slot_pool->Flags = this->Flags;
    SlotPool *pSVar1 = this->NextSlotPool;
    SlotPool *last_slot_pool = this;
    while (pSVar1) {
        last_slot_pool = last_slot_pool->NextSlotPool;
        pSVar1 = last_slot_pool->NextSlotPool;
    }
    last_slot_pool->NextSlotPool = new_slot_pool;
    uVar2 = (num_extra_slots - 1) * this->SlotSize;
    if (uVar2 < 0) {
        uVar2 += 3;
    }
    *(SlotPoolEntry **)((char *)new_slot_pool + 0x30 + (uVar2 & ~3)) = this->FreeSlots;
    this->FreeSlots = new_slot_pool->FreeSlots;
    new_slot_pool->FreeSlots = nullptr;
    this->TotalNumSlots += num_extra_slots;
}

int SlotPool::GetSlotNumber(void *p) {
    int base_slot_num = 0;
    SlotPool *slot_pool = this;
    while (slot_pool) {
        int slot_num = reinterpret_cast<uintptr_t>((char *)p + ((char *)-0x30 - (char *)slot_pool)) / this->SlotSize;
        if ((slot_num >= 0) && (slot_num < slot_pool->NumSlots)) {
            return base_slot_num + slot_num;
        }
        base_slot_num += slot_pool->NumSlots;
        slot_pool = slot_pool->NextSlotPool;
    }
    return -1;
}

void *SlotPool::GetSlot(int slot_number) {
    SlotPool *slot_pool = this;
    while (slot_pool) {
        if ((slot_number >= 0) && (slot_number < slot_pool->NumSlots)) {
            return (reinterpret_cast<char *>(slot_pool) + 0x30 + slot_number * this->SlotSize);
        }
        slot_number -= slot_pool->NumSlots;
        slot_pool = slot_pool->NextSlotPool;
    }
    return nullptr;
}

void *SlotPool::GetAllocatedSlot(int n) {
    char *allocated_table = static_cast<char *>(bMALLOC(this->TotalNumSlots, "0", 0, 0));
    bMemSet(allocated_table, 1, this->TotalNumSlots);
    for (SlotPoolEntry *slot = this->FreeSlots; slot; slot = slot->Next) {
        int slot_number = this->GetSlotNumber(slot);
        allocated_table[slot_number] = 0;
    }
    void *found_slot = nullptr;
    for (int i = 0; i < this->TotalNumSlots; i++) {
        if (allocated_table[i] != 0) {
            if (n == 0) {
                found_slot = this->GetSlot(i);
                break;
            }
            n--;
        }
    }
    bFree(allocated_table);
    return found_slot;
}

// UNSOLVED
void SlotPool::CleanupExpandedSlotPools() {
    SlotPool *pSVar1;
    SlotPoolEntry **ppSVar2;
    SlotPoolEntry **p;
    SlotPoolEntry *slot;
    SlotPool *slot_pool = this->NextSlotPool;
    SlotPool *previous_slot_pool;

    if (slot_pool) {
        while (slot_pool) {
            slot_pool->NumAllocatedSlots = slot_pool->NumSlots;
            slot_pool = slot_pool->NextSlotPool;
        }
        for (slot = this->FreeSlots; slot; slot = slot->Next) {
            for (SlotPool *slot_pool = this->NextSlotPool; slot_pool; slot_pool = slot_pool->NextSlotPool) {
                int slot_number = slot_pool->GetSlotNumber(slot);
                if ((slot_number >= 0) && (slot_number < slot_pool->NumSlots)) {
                    slot_pool->NumAllocatedSlots--;
                    break;
                }
            }
        }
        SlotPool *next_slot_pool = this->NextSlotPool;
        previous_slot_pool = this;
        while (pSVar1 = slot_pool, slot_pool = next_slot_pool, slot_pool) {
            next_slot_pool = slot_pool->NextSlotPool;
            if (slot_pool->NumAllocatedSlots == 0) {
                p = &this->FreeSlots;
                for (SlotPoolEntry *next_slot = this->FreeSlots; ppSVar2 = p, p = &next_slot->Next, p;) {
                    next_slot = *p;
                    int slot_number = slot_pool->GetSlotNumber(p);
                    if ((slot_number >= 0) && (slot_number < slot_pool->NumSlots)) {
                        *ppSVar2 = next_slot;
                        p = ppSVar2;
                    }
                }
                this->TotalNumSlots -= slot_pool->NumSlots;
                previous_slot_pool->NextSlotPool = slot_pool->NextSlotPool;
                SlotPool::DeleteSlotPool(slot_pool);
            }
            previous_slot_pool = slot_pool;
            slot_pool = pSVar1;
        }
    }
}

void *SlotPool::Malloc() {
    if (!this->FreeSlots && (this->Flags & SLOTPOOL_FLAG_OVERFLOW_IF_FULL)) {
        int num_extra_slots = this->NumSlots;
        if (num_extra_slots < 0) {
            num_extra_slots += 3;
        }
        this->ExpandSlotPool((num_extra_slots >> 2) + 1);
    }
    SlotPoolEntry *slot = static_cast<SlotPoolEntry *>(this->FastMalloc());
    if (slot && (this->Flags & SLOTPOOL_FLAG_ZERO_ALLOCATED_MEMORY)) {
        int num_words = this->SlotSize >> 2;
        for (int n = 0; n < num_words; n++) {
            slot[n].Next = nullptr;
        }
    }
    return slot;
}

void *SlotPool::FastMalloc() {
    SlotPoolEntry *slot = this->FreeSlots;
    int num_allocated = this->NumAllocatedSlots + 1;
    if (slot) {
        int most_allocated = this->MostNumAllocatedSlots;
        SlotPoolEntry *next_free_slot = slot->Next;
        if (num_allocated > most_allocated) {
            this->MostNumAllocatedSlots = num_allocated;
        }
        this->NumAllocatedSlots = num_allocated;
        this->FreeSlots = next_free_slot;
    }
    return slot;
}

void SlotPool::Free(void *p) {
    int num_allocated_slots = this->NumAllocatedSlots;
    *reinterpret_cast<SlotPoolEntry **>(p) = this->FreeSlots;
    this->NumAllocatedSlots = num_allocated_slots - 1;
    SlotPoolEntry *slot = static_cast<SlotPoolEntry *>(p);
    this->FreeSlots = slot;
}

void *SlotPool::Malloc(int num_slots, void **last_slot) {
    SlotPoolEntry *slot;
    SlotPoolEntry *first_slot;
    int n;

    if (num_slots == 0) {
        return nullptr;
    }
    if (this->CountFreeSlots() < num_slots) {
        if ((this->Flags & SLOTPOOL_FLAG_OVERFLOW_IF_FULL) == 0) {
            return nullptr;
        }
        n = this->NumSlots;
        if (n < 0) {
            n += 3;
        }
        int num_extra_slots = (n >> 2) + 1;
        if (num_extra_slots < num_slots) {
            num_extra_slots = num_slots;
        }
        this->ExpandSlotPool(num_extra_slots);
    }
    first_slot = this->FreeSlots;
    n = 0;
    slot = first_slot;
    if (!first_slot) {
        return nullptr;
    }
    while (n < num_slots - 1) {
        slot = slot->Next;
        n++;
        if (!slot) {
            return nullptr;
        }
    }
    if (!slot) {
        return nullptr;
    }
    this->FreeSlots = slot->Next;
    slot->Next = nullptr;
    if (last_slot) {
        *last_slot = slot;
    }
    int num_extra_slots = this->NumAllocatedSlots + num_slots;
    this->NumAllocatedSlots = num_extra_slots;
    if (num_extra_slots > this->MostNumAllocatedSlots) {
        this->MostNumAllocatedSlots = num_extra_slots;
    }
    if (this->Flags & SLOTPOOL_FLAG_ZERO_ALLOCATED_MEMORY) {
        for (SlotPoolEntry *slot = first_slot; slot; slot = slot->Next) {
            int num_words = this->SlotSize >> 2;
            for (int n = 1; n < num_words; n++) {
                slot[n].Next = nullptr;
            }
        }
    }
    return first_slot;
}

SlotPoolManager::SlotPoolManager() {
    this->Initialized = true;
}

SlotPool *SlotPoolManager::NewSlotPool(int slot_size, int num_slots, const char *debug_name, int memory_pool) {
    SlotPool *new_slot_pool = SlotPool::NewSlotPool((slot_size + 3) & ~3, num_slots, debug_name, memory_pool);
    if (!new_slot_pool) {
        return nullptr;
    }
    if (this->Initialized == true) {
        this->SlotPoolList.AddTail(new_slot_pool);
    }
    return new_slot_pool;
}

// UNSOLVED
void SlotPoolManager::DeleteSlotPool(SlotPool *slot_pool) {
    if (!slot_pool) {
        return;
    }
    if (this->Initialized) {
        this->SlotPoolList.Remove(slot_pool);
    }
    if ((slot_pool->Flags & SLOTPOOL_FLAG_WARN_IF_NONEMPTY_DELETE) && (slot_pool->CountAllocatedSlots() > 0)) {
        void *leaky_slot = slot_pool->GetAllocatedSlot(0);
        slot_pool->GetSlotNumber(leaky_slot);
    }
    for (SlotPool *next_pool = slot_pool; next_pool; next_pool = next_pool->NextSlotPool) {
        SlotPool::DeleteSlotPool(next_pool);
    }
}

void SlotPoolManager::CleanupExpandedSlotPools() {
    for (SlotPool *slot_pool = this->SlotPoolList.GetHead(); slot_pool != this->SlotPoolList.EndOfList(); slot_pool = slot_pool->GetNext()) {
        slot_pool->CleanupExpandedSlotPools();
    }
}
