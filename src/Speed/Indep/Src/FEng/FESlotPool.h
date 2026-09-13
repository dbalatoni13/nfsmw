#ifndef FESLOTPOOL_H__
#define FESLOTPOOL_H__

#include "FEList.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

static const u16 FEngSlotsPerBlock = 32; // size: 0x2, Decl: 22

// total size: 0x18
// Decl: 28
class FESlotNode : public FEMinNode {
  private:
    u16 SlotSize;   // offset 0xC, size 0x2, Decl: 30
    u16 SlotsUsed;  // offset 0xE, size 0x2, Decl: 31
    u8 SlotMask[4]; // offset 0x10, size 0x4, Decl: 32
    u8 *pData;      // offset 0x14, size 0x4, Decl: 33

  public:
    FESlotNode(u16 Size) : SlotSize(Size), SlotsUsed(0), pData(nullptr) { // Decl: 36
        pData = FNEW u8[Size * FEngSlotsPerBlock];
        FEngMemSet(SlotMask, 0, sizeof(SlotMask));
    }

    ~FESlotNode() override { // Decl: 41
        if (pData != nullptr) {
            delete[] pData;
        }
    }

    u8 *AllocBlock();
    void FreeBlock(u8 *pSlot);

    bool Contains(u8 *pSlot) { // Decl: 46
        return pSlot >= pData && pSlot < pData + (SlotSize * FEngSlotsPerBlock);
    }
    bool IsEmpty() { // Decl: 47
        return SlotsUsed == 0;
    }
    bool IsFull() { // Decl: 48
        return SlotsUsed == FEngSlotsPerBlock;
    }

    FESlotNode *GetNext() { // Decl: 50
        return static_cast<FESlotNode *>(FEMinNode::GetNext());
    }
};

// total size: 0x20
// Decl: 58
class FESlotPool : public FEMinNode {
  private:
    FEMinList Slots; // offset 0xC, size 0x10, Decl: 60

  public:
    u32 SlotSize; // offset 0x1C, size 0x4, Decl: 63

    FESlotPool(u32 Size) : SlotSize(Size) {} // Decl: 65

    u8 *Alloc();
    bool Free(u8 *pSlot);

    bool IsEmpty() { // Decl: 72
        return Slots.GetNumElements() == 0;
    }

    FESlotPool *GetNext() { // Decl: 75
        return static_cast<FESlotPool *>(FEMinNode::GetNext());
    }
};

// total size: 0x10
// Decl: 88
class FEMultiPool {
  private:
    FEMinList Pools; // offset 0x0, size 0x10, Decl: 90

  public:
    u8 *Alloc(u32 Size);  // Decl: 93
    void Free(u8 *pSlot); // Decl: 94
};

#endif
