#include "Speed/Indep/Src/FEng/FESlotPool.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

unsigned char* FESlotNode::AllocBlock() {
    if (SlotsUsed == 0x20) {
        return nullptr;
    }
    unsigned char* pMask = SlotMask;
    unsigned long byteIdx = 0;
    while (pMask[byteIdx] == 0xFF) {
        byteIdx++;
    }
    unsigned long bitIdx = byteIdx << 3;
    if (pMask[byteIdx] & 1) {
        do {
            bitIdx++;
        } while ((static_cast<int>(static_cast<unsigned int>(pMask[bitIdx >> 3])) >> (bitIdx & 7) & 1) != 0);
    }
    SlotsUsed++;
    pMask[bitIdx >> 3] |= static_cast<unsigned char>(1 << (bitIdx & 7));
    return pData + SlotSize * bitIdx;
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
    while (pNode && pNode->SlotsUsed == 0x20) {
        pNode = pNode->GetNext();
    }
    if (!pNode) {
        pNode = new (static_cast<FESlotNode*>(FEngMalloc(sizeof(FESlotNode), nullptr, 0))) FESlotNode(static_cast<unsigned short>(SlotSize));
        pNode->pData = static_cast<unsigned char*>(FEngMalloc(static_cast<unsigned long>(pNode->SlotSize) << 5, nullptr, 0));
        FEngMemSet(pNode->SlotMask, 0, 4);
        Slots.AddNode(nullptr, pNode);
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

FESlotPool::~FESlotPool() {
}

unsigned char* FEMultiPool::Alloc(unsigned long Size) {
    if (Size == 0) {
        return nullptr;
    }
    FESlotPool* pPool = static_cast<FESlotPool*>(Pools.GetHead());
    while (pPool) {
        if (pPool->SlotSize == Size) {
            return pPool->Alloc();
        }
        pPool = pPool->GetNext();
    }
    pPool = new (static_cast<FESlotPool*>(FEngMalloc(sizeof(FESlotPool), nullptr, 0))) FESlotPool(Size);
    Pools.AddNode(nullptr, pPool);
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
