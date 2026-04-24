#include "Speed/Indep/Src/FEng/FEKeyTrack.h"
#include "Speed/Indep/Src/FEng/ObjectPool.h"

ObjectPool<FEKeyNode, 256> FEKeyNode::NodePool;

void* FEKeyNode::operator new(unsigned int) {
    FEKeyNode* pNode = NodePool.AllocSingle();
    return pNode;
}

void FEKeyNode::operator delete(void* pNode) {
    FEKeyNode* pDeleteNode = static_cast<FEKeyNode*>(pNode);
    pDeleteNode->~FEKeyNode();
    NodePool.FreeSingleNoDestroy(pDeleteNode);
}

FEKeyNode* FEKeyTrack::GetKeyAt(long tTime) {
    if (tTime < 0) {
        return GetBaseKey();
    }
    FEKeyNode* pPrev = GetFirstDeltaKey();
    if (!pPrev) {
        return GetBaseKey();
    }
    for (;;) {
        FEKeyNode* pKey = pPrev->GetNext();
        if (!pKey) {
            break;
        }
        if (pPrev->tTime >= tTime) {
            break;
        }
        pPrev = pKey;
    }
    return pPrev;
}

FEKeyNode* FEKeyTrack::GetDeltaKeyAt(long tTime) {
    FEKeyNode* pPrev = GetFirstDeltaKey();
    if (!pPrev) {
        return nullptr;
    }
    for (;;) {
        FEKeyNode* pKey = pPrev->GetNext();
        if (!pKey) {
            break;
        }
        if (pPrev->tTime >= tTime) {
            break;
        }
        pPrev = pKey;
    }
    return pPrev;
}

void FEKeyTrack::operator=(FEKeyTrack& Src) {
    FEKeyNode* pKey;
    while ((pKey = static_cast<FEKeyNode*>(DeltaKeys.RemHead())) != nullptr) {
        delete pKey;
    }

    ParamType = Src.ParamType;
    ParamSize = Src.ParamSize;
    InterpType = Src.InterpType;
    InterpAction = Src.InterpAction;
    Length = Src.Length;
    LongOffset = Src.LongOffset;
    BaseKey.tTime = Src.BaseKey.tTime;
    BaseKey.Val = Src.BaseKey.Val;

    if (Src.IsReference()) {
        DeltaKeys.ReferenceList(Src.DeltaKeys.GetRefSource());
    } else {
        FEKeyNode* pSrcKey = Src.GetFirstDeltaKey();
        while (pSrcKey) {
            pKey = new FEKeyNode();
            pKey->tTime = pSrcKey->tTime;
            pKey->Val = pSrcKey->Val;
            DeltaKeys.AddTail(pKey);
            pSrcKey = pSrcKey->GetNext();
        }
    }
}
