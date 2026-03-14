#include "Speed/Indep/Src/FEng/FEKeyTrack.h"
#include "Speed/Indep/Src/FEng/ObjectPool.h"

ObjectPool<FEKeyNode, 256> FEKeyNode::NodePool;

void* FEKeyNode::operator new(unsigned int) {
    FEKeyNode* pNode = NodePool.AllocSingle();
    pNode->Init();
    return pNode;
}

void FEKeyNode::operator delete(void* pNode) {
    NodePool.FreeSingle(static_cast<FEKeyNode*>(pNode));
}

FEKeyNode* FEKeyTrack::GetKeyAt(long tTime) {
    if (tTime > -1) {
        FEKeyNode* pKey = GetFirstDeltaKey();
        if (pKey) {
            FEKeyNode* pPrev;
            do {
                pPrev = pKey;
                if (!pPrev->GetNext()) {
                    return pPrev;
                }
                pKey = pPrev->GetNext();
            } while (pPrev->tTime < tTime);
            return pPrev;
        }
    }
    return GetBaseKey();
}

FEKeyNode* FEKeyTrack::GetDeltaKeyAt(long tTime) {
    FEKeyNode* pKey = GetFirstDeltaKey();
    FEKeyNode* pPrev;
    if (!pKey) {
        pPrev = nullptr;
    } else {
        do {
            pPrev = pKey;
            if (!pPrev->GetNext()) {
                return pPrev;
            }
            pKey = pPrev->GetNext();
        } while (pPrev->tTime < tTime);
    }
    return pPrev;
}
