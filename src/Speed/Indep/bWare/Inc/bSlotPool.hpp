#ifndef BWARE_BSLOT_POOL_H
#define BWARE_BSLOT_POOL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "./bList.hpp"

enum SlotPoolFlags {
    SLOTPOOL_FLAG_WARN_IF_NONEMPTY_DELETE = 8,
    SLOTPOOL_FLAG_WARN_IF_OVERFLOW = 4,
    SLOTPOOL_FLAG_ZERO_ALLOCATED_MEMORY = 2,
    SLOTPOOL_FLAG_OVERFLOW_IF_FULL = 1,
};
struct SlotPoolEntry {
    // total size: 0x4
    SlotPoolEntry *Next; // offset 0x0, size 0x4
};

struct SlotPool : public bTNode<SlotPool> {
    // total size: 0x34
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

    void SetFlag(SlotPoolFlags flag) {}

    void ClearFlag(SlotPoolFlags flag) {
        Flags = static_cast<SlotPoolFlags>((static_cast<int>(Flags)) & ~flag);
    }

    SlotPoolFlags GetFlags();

    bool IsInPool(void *p);

    const char *GetName();

    int IsFull();

    int IsEmpty();

    int HasOverflowed();

    int CountFreeSlots();

    int CountAllocatedSlots();

    int CountTotalSlots();

    int CountMostAllocatedSlots();
};

SlotPool *bNewSlotPool(int slot_size, int num_slots, const char *debug_name, int memory_pool);

extern struct SlotPool *ePolySlotPool;

extern unsigned char *CurrentBufferStart;
extern unsigned char *CurrentBufferPos;
extern unsigned char *CurrentBufferEnd;

#endif
