#ifndef FENG_FESLOTPOOL_H
#define FENG_FESLOTPOOL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEList.h"

// total size: 0x18
struct FESlotNode : public FEMinNode {
    unsigned short SlotSize;    // offset 0xC, size 0x2
    unsigned short SlotsUsed;   // offset 0xE, size 0x2
    unsigned char SlotMask[4];  // offset 0x10, size 0x4
    unsigned char* pData;       // offset 0x14, size 0x4

    inline FESlotNode(unsigned short Size)
        : SlotSize(Size) //
        , SlotsUsed(0) //
        , pData(nullptr) {
    }

    inline bool IsEmpty() { return SlotsUsed == 0; }
    inline bool IsFull() { return SlotsUsed == 0x20; }
    inline FESlotNode* GetNext() { return static_cast<FESlotNode*>(FEMinNode::GetNext()); }

    ~FESlotNode() override;
    unsigned char* AllocBlock();
    void FreeBlock(unsigned char* pSlot);
};

// total size: 0x20
struct FESlotPool : public FEMinNode {
    FEMinList Slots;       // offset 0xC, size 0x10
    unsigned long SlotSize; // offset 0x1C, size 0x4

    inline FESlotPool(unsigned long Size) : SlotSize(Size) {}
    inline bool IsEmpty() { return Slots.GetNumElements() == 0; }
    inline FESlotPool* GetNext() { return static_cast<FESlotPool*>(FEMinNode::GetNext()); }

    unsigned char* Alloc();
    bool Free(unsigned char* pSlot);
};

// total size: 0x10
struct FEMultiPool {
    FEMinList Pools; // offset 0x0, size 0x10

    unsigned char* Alloc(unsigned long Size);
    void Free(unsigned char* pSlot);
};

#endif
