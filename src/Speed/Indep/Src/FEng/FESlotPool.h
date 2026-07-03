#ifndef FESLOTPOOL_H__
#define FESLOTPOOL_H__

#include "FEList.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "types.h"

static const u16 FEngSlotsPerBlock = 32; // size: 0x2, Decl: speed/indep/src/feng/FESlotPool.h:22

// File: speed/indep/src/feng/FESlotPool.h
// total size: 0x18
// Decl: speed/indep/src/feng/FESlotPool.h:28
class FESlotNode : public FEMinNode {
  private:
    u16 SlotSize;   // offset 0xC, size 0x2, Decl: speed/indep/src/feng/FESlotPool.h:30
    u16 SlotsUsed;  // offset 0xE, size 0x2, Decl: speed/indep/src/feng/FESlotPool.h:31
    u8 SlotMask[4]; // offset 0x10, size 0x4, Decl: speed/indep/src/feng/FESlotPool.h:32
    u8 *pData;      // offset 0x14, size 0x4, Decl: speed/indep/src/feng/FESlotPool.h:33

  public:
    FESlotNode(u16 Size) : SlotSize(Size), SlotsUsed(0), pData(nullptr) { // Decl: speed/indep/src/feng/FESlotPool.h:36
        pData = FNEW u8[Size * FEngSlotsPerBlock];
        FEngMemSet(SlotMask, 0, sizeof(SlotMask));
    }

    ~FESlotNode() override { // Decl: speed/indep/src/feng/FESlotPool.h:41
        if (pData) {
            delete[] pData;
        }
    }

    u8 *AllocBlock();
    void FreeBlock(u8 *pSlot);

    bool Contains(u8 *pSlot) { // Decl: speed/indep/src/feng/FESlotPool.h:46
        return pSlot >= pData && pSlot < pData + (SlotSize * FEngSlotsPerBlock);
    }
    bool IsEmpty() { // Decl: speed/indep/src/feng/FESlotPool.h:47
        return SlotsUsed == 0;
    }
    bool IsFull() { // Decl: speed/indep/src/feng/FESlotPool.h:48
        return SlotsUsed == FEngSlotsPerBlock;
    }

    FESlotNode *GetNext() { // Decl: speed/indep/src/feng/FESlotPool.h:50
        return static_cast<FESlotNode *>(FEMinNode::GetNext());
    }
};

// total size: 0x20
// Decl: speed/indep/src/feng/FESlotPool.h:58
class FESlotPool : public FEMinNode {
  private:
    FEMinList Slots; // offset 0xC, size 0x10, Decl: speed/indep/src/feng/FESlotPool.h:60

  public:
    u32 SlotSize; // offset 0x1C, size 0x4, Decl: speed/indep/src/feng/FESlotPool.h:63

    FESlotPool(u32 Size) : SlotSize(Size) {} // Decl: speed/indep/src/feng/FESlotPool.h:65

    u8 *Alloc();
    bool Free(u8 *pSlot);

    bool IsEmpty() { // Decl: speed/indep/src/feng/FESlotPool.h:72
        return Slots.GetNumElements() == 0;
    }

    FESlotPool *GetNext() { // Decl: speed/indep/src/feng/FESlotPool.h:75
        return static_cast<FESlotPool *>(FEMinNode::GetNext());
    }
};

// total size: 0x10
// Decl: speed/indep/src/feng/FESlotPool.h:88
class FEMultiPool {
  private:
    FEMinList Pools; // offset 0x0, size 0x10, Decl: speed/indep/src/feng/FESlotPool.h:90

  public:
    u8 *Alloc(u32 Size);  // Decl: speed/indep/src/feng/FESlotPool.h:93
    void Free(u8 *pSlot); // Decl: speed/indep/src/feng/FESlotPool.h:94
};

#endif
