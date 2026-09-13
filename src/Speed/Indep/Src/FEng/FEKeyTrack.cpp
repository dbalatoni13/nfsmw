#include "Speed/Indep/Src/FEng/FEKeyTrack.h"
#include "Speed/Indep/Src/FEng/FERefList.h"
#include "Speed/Indep/Src/FEng/ObjectPool.h"

ObjectPool<FEKeyNode, 256> FEKeyNode::NodePool;

void FEKeyTrack::operator=(FEKeyTrack &Src) {
    DeltaKeys.Purge();

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
        return;
    }

    FEKeyNode *pKey;
    FEKeyNode *pSrcKey = Src.GetFirstDeltaKey();
    while (pSrcKey != nullptr) {
        pKey = new FEKeyNode();
        pKey->tTime = pSrcKey->tTime;
        pKey->Val = pSrcKey->Val;
        DeltaKeys.AddTail(pKey);
        pSrcKey = pSrcKey->GetNext();
    }
}

u32 FEKeyTrack::ComputeSize() {
    u32 Size;
}

void *FEKeyNode::operator new(size_t) {
    FEKeyNode *pNode = NodePool.AllocSingle();
    pNode->Init();
    return pNode;
}

void FEKeyNode::operator delete(void *pNode) {
    static_cast<FEKeyNode *>(pNode)->~FEKeyNode();
    NodePool.FreeSingle(static_cast<FEKeyNode *>(pNode));
}

FEKeyNode *FEKeyTrack::GetKeyAt(i32 tTime) {
    if (tTime < 0) {
        return &BaseKey;
    }

    FEKeyNode *pKey = GetFirstDeltaKey();

    if (pKey == nullptr) {
        return &BaseKey;
    }

    while (pKey->GetNext() != nullptr) {
        if (pKey->GetKeyData()->tTime >= tTime) {
            break;
        }

        pKey = pKey->GetNext();
    };

    return pKey;
}

FEKeyNode *FEKeyTrack::GetDeltaKeyAt(i32 tTime) {
    FEKeyNode *pKey = GetFirstDeltaKey();
    if (pKey == nullptr) {
        return nullptr;
    }

    while (pKey->GetNext() != nullptr) {
        if (pKey->GetKeyData()->tTime >= tTime) {
            break;
        }

        pKey = pKey->GetNext();
    };

    return pKey;
}
