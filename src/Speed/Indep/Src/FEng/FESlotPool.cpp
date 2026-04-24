#include "Speed/Indep/Src/FEng/FESlotPool.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

unsigned char* FESlotNode::AllocBlock() {
    if (SlotsUsed == 0x20) {
        return nullptr;
    }
    unsigned long Index = 0;
    while (SlotMask[Index] == 0xFF) {
        Index++;
    }
    Index <<= 3;
    if (SlotMask[Index >> 3] & 1) {
        do {
            Index++;
        } while ((SlotMask[Index >> 3] >> (Index & 7)) & 1);
    }
    SlotsUsed++;
    SlotMask[Index >> 3] |= static_cast<unsigned char>(1 << (Index & 7));
    return pData + SlotSize * Index;
}

void FESlotNode::FreeBlock(unsigned char* pSlot) {
    unsigned long idx = static_cast<unsigned long>(pSlot - pData) / static_cast<unsigned long>(SlotSize);
    SlotsUsed--;
    unsigned long byteIdx = idx >> 3;
    unsigned long bitIdx = idx & 7;
    SlotMask[byteIdx] &= ~static_cast<unsigned char>(1 << bitIdx);
}

unsigned char* FESlotPool::Alloc() {
    FESlotNode* pNode = static_cast<FESlotNode*>(Slots.GetHead());
    while (pNode && pNode->IsFull()) {
        pNode = pNode->GetNext();
    }
    if (!pNode) {
        pNode = FENG_NEW FESlotNode(static_cast<unsigned short>(SlotSize));
        pNode->pData = static_cast<unsigned char*>(FEngMalloc(static_cast<unsigned long>(pNode->SlotSize) << 5, nullptr, 0));
        FEngMemSet(pNode->SlotMask, 0, 4);
        Slots.AddHead(pNode);
    }
    return pNode->AllocBlock();
}

bool FESlotPool::Free(unsigned char* pSlot) {
    FESlotNode* pNode = static_cast<FESlotNode*>(Slots.GetHead());
    while (pNode) {
        bool contains = false;
        if (reinterpret_cast<unsigned long>(pSlot) >= reinterpret_cast<unsigned long>(pNode->pData)) {
            contains = reinterpret_cast<unsigned long>(pSlot) < reinterpret_cast<unsigned long>(pNode->pData) + static_cast<unsigned long>(pNode->SlotSize) * 0x20;
        }
        if (contains) {
            break;
        }
        pNode = pNode->GetNext();
    }
    if (!pNode) {
        return false;
    }
    pNode->FreeBlock(pSlot);
    if (pNode->SlotsUsed == 0) {
        Slots.RemNode(pNode);
        delete pNode;
    }
    return true;
}

FESlotNode::~FESlotNode() {
    if (pData) {
        delete[] pData;
    }
}

unsigned char* FEMultiPool::Alloc(unsigned long Size) {
    if (Size == 0) {
        return nullptr;
    }
    FESlotPool* pPool = static_cast<FESlotPool*>(Pools.GetHead());
    while (pPool && pPool->SlotSize != Size) {
        pPool = pPool->GetNext();
    }
    if (!pPool) {
        pPool = FENG_NEW FESlotPool(Size);
        Pools.AddHead(pPool);
    }
    return pPool->Alloc();
}

void FEMultiPool::Free(unsigned char* pSlot) {
    if (!pSlot) {
        return;
    }
    FESlotPool* pPool = static_cast<FESlotPool*>(Pools.GetHead());
    while (pPool && !pPool->Free(pSlot)) {
        pPool = pPool->GetNext();
    }
    if (pPool->IsEmpty()) {
        Pools.RemNode(pPool);
        delete pPool;
    }
}
