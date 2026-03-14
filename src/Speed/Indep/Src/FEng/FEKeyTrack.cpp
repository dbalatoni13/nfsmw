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
