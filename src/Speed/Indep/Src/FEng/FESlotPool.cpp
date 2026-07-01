#include "Speed/Indep/Src/FEng/FESlotPool.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

u8 *FESlotNode::AllocBlock() {
    if (SlotsUsed == FEngSlotsPerBlock) {
        return nullptr;
    }
    u32 Index = 0;
    while (SlotMask[Index] == 0xFF) {
        Index++;
    }

    Index <<= 3;

    while (SlotMask[Index >> 3] >> (Index & 7) & 1) {
        Index++;
    }

    SlotsUsed++;
    SlotMask[Index >> 3] |= (1 << (Index & 7));
    return pData + SlotSize * Index;
}

void FESlotNode::FreeBlock(u8 *pSlot) {
    u32 Index = static_cast<u32>(pSlot - pData) / static_cast<u32>(SlotSize);
    SlotsUsed--;
    SlotMask[Index >> 3] &= ~static_cast<u8>(1 << (Index & 7));
}

u8 *FESlotPool::Alloc() {
    FESlotNode *pNode = static_cast<FESlotNode *>(Slots.GetHead());
    while (pNode && pNode->IsFull()) {
        pNode = pNode->GetNext();
    }
    if (!pNode) {
        pNode = FNEW FESlotNode(SlotSize);
        Slots.AddHead(pNode);
    }
    return pNode->AllocBlock();
}

bool FESlotPool::Free(u8 *pSlot) {
    FESlotNode *pNode = static_cast<FESlotNode *>(Slots.GetHead());
    while (pNode && !pNode->Contains(pSlot)) {
        pNode = pNode->GetNext();
    }

    if (!pNode) {
        return false;
    }

    pNode->FreeBlock(pSlot);
    if (pNode->IsEmpty()) {
        Slots.RemNode(pNode);
        delete pNode;
    }

    return true;
}

u8 *FEMultiPool::Alloc(u32 Size) {
    if (Size == 0) {
        return nullptr;
    }
    FESlotPool *pPool = static_cast<FESlotPool *>(Pools.GetHead());
    while (pPool && pPool->SlotSize != Size) {
        pPool = pPool->GetNext();
    }
    if (!pPool) {
        pPool = FNEW FESlotPool(Size);
        Pools.AddHead(pPool);
    }
    return pPool->Alloc();
}

void FEMultiPool::Free(u8 *pSlot) {
    if (!pSlot) {
        return;
    }
    FESlotPool *pPool = static_cast<FESlotPool *>(Pools.GetHead());
    while (pPool && !pPool->Free(pSlot)) {
        pPool = pPool->GetNext();
    }
    if (pPool->IsEmpty()) {
        Pools.RemNode(pPool);
        delete pPool;
    }
}
