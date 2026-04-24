#ifndef BWARE_BSLOT_POOL_H
#define BWARE_BSLOT_POOL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "bList.hpp"

enum SlotPoolFlags {
    SLOTPOOL_FLAG_WARN_IF_NONEMPTY_DELETE = 8,
    SLOTPOOL_FLAG_WARN_IF_OVERFLOW = 4,
    SLOTPOOL_FLAG_ZERO_ALLOCATED_MEMORY = 2,
    SLOTPOOL_FLAG_OVERFLOW_IF_FULL = 1,
};
class SlotPoolEntry {
    // total size: 0x4
  public:
    SlotPoolEntry *Next; // offset 0x0, size 0x4
};

class SlotPool : public bTNode<SlotPool> {
    // total size: 0x34

    friend class SlotPoolManager; // only way I can think to allow SlotPoolManager::DeleteSlotPool to access NextSlotPool

    SlotPool *NextSlotPool;    // offset 0x8, size 0x4
    const char *DebugName;     // offset 0xC, size 0x4
    SlotPoolEntry *FreeSlots;  // offset 0x10, size 0x4
    SlotPoolFlags Flags;       // offset 0x14, size 0x4
    int NumAllocatedSlots;     // offset 0x18, size 0x4
    int MostNumAllocatedSlots; // offset 0x1C, size 0x4
    int MemoryPool;            // offset 0x20, size 0x4
    int NumSlots;              // offset 0x24, size 0x4
    int SlotSize;              // offset 0x28, size 0x4
    int TotalNumSlots;         // offset 0x2C, size 0x4
    SlotPoolEntry Slots[1];    // offset 0x30, size 0x4

  public:
    static SlotPool *NewSlotPool(int slot_size, int num_slots, const char *debug_name, int memory_pool);
    static void DeleteSlotPool(SlotPool *slot_pool);

    void FlushSlotPool();
    void ExpandSlotPool(int num_extra_slots);
    int GetSlotNumber(void *p);
    void *GetSlot(int slot_number);
    void *GetAllocatedSlot(int n);
    void CleanupExpandedSlotPools();
    void VerifyPoolIntegrity();
    void *Malloc();
    void *FastMalloc();
    void Free(void *p);
    void *Malloc(int num_slots, void **last_slot);
    void Free(void *first_slot, void *last_slot);

    void SetFlag(SlotPoolFlags flag) {
        Flags = static_cast<SlotPoolFlags>((static_cast<int>(Flags)) | flag);
    }

    void ClearFlag(SlotPoolFlags flag) {
        Flags = static_cast<SlotPoolFlags>((static_cast<int>(Flags)) & ~flag);
    }

    SlotPoolFlags GetFlags() {
        return Flags;
    };

    bool IsInPool(void *p);

    const char *GetName();

    int IsFull() {
        return this->NumAllocatedSlots == this->TotalNumSlots;
    }

    int IsEmpty() {
        return this->NumAllocatedSlots == 0;
    }

    int HasOverflowed() {
        return !(this->TotalNumSlots == this->NumSlots);
    }

    int CountFreeSlots() {
        return this->TotalNumSlots - this->NumAllocatedSlots;
    }

    int CountAllocatedSlots() {
        return this->NumAllocatedSlots;
    }

    int CountTotalSlots() {
        return this->TotalNumSlots;
    }

    int CountMostAllocatedSlots() {
        return this->MostNumAllocatedSlots;
    }
};

class SlotPoolManager {
    // total size: 0xC
    int Initialized;               // offset 0x0, size 0x4
    bTList<SlotPool> SlotPoolList; // offset 0x4, size 0x8

  public:
    SlotPoolManager();
    ~SlotPoolManager();
    SlotPool *NewSlotPool(int slot_size, int num_slots, const char *debug_name, int memory_pool);
    void DeleteSlotPool(SlotPool *slot_pool);
    void PrintAllSlotPools();
    void CleanupExpandedSlotPools();
};

int bCountFreeSlots(SlotPool *slot_pool);
int bCountTotalSlots(SlotPool *slot_pool);
SlotPool *bNewSlotPool(int slot_size, int num_slots, const char *debug_name, int memory_pool);
void bDeleteSlotPool(SlotPool *slot_pool);
int bIsSlotPoolFull(SlotPool *slot_pool);
void *bOMalloc(SlotPool *slot_pool);
void bFree(SlotPool *slot_pool, void *p);
void bFree(SlotPool *slot_pool, void *first_slot, void *last_slot);

extern SlotPool *ePolySlotPool;

extern uint8 *CurrentBufferStart;
extern uint8 *CurrentBufferPos;
extern uint8 *CurrentBufferEnd;

extern SlotPoolManager TheSlotPoolManager;

#endif
