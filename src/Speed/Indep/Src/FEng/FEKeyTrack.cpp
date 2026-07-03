#include "Speed/Indep/Src/FEng/FEKeyTrack.h"
#include "Speed/Indep/Src/FEng/FERefList.h"
#include "Speed/Indep/Src/FEng/ObjectPool.h"
#include "types.h"

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

    } else {
        FEKeyNode *pSrcKey = Src.GetFirstDeltaKey();
        while (pSrcKey) {
            FEKeyNode *pKey = new FEKeyNode();
            pKey->tTime = pSrcKey->tTime;
            pKey->Val = pSrcKey->Val;
            DeltaKeys.AddTail(pKey);
            pSrcKey = pSrcKey->GetNext();
        }
    }
}

u32 FEKeyTrack::ComputeSize() {}

void *FEKeyNode::operator new(size_t) {
    FEKeyNode *pNode = NodePool.AllocSingle();
    return pNode;
}

void FEKeyNode::operator delete(void *pNode) {
    static_cast<FEKeyNode *>(pNode)->~FEKeyNode();
    NodePool.FreeSingle(static_cast<FEKeyNode *>(pNode));
}

FEKeyNode *FEKeyTrack::GetKeyAt(i32 tTime) {
    if (tTime < 0) {
        return GetBaseKey();
    }

    FEKeyNode *pKey = GetFirstDeltaKey();

    if (!pKey) {
        return GetBaseKey();
    }

    while (pKey->GetNext()) {
        if (pKey->GetKeyData()->tTime >= tTime) {
            break;
        }

        pKey = pKey->GetNext();
    };

    return pKey;
}

FEKeyNode *FEKeyTrack::GetDeltaKeyAt(i32 tTime) {
    FEKeyNode *pKey = GetFirstDeltaKey();
    if (!pKey) {
        return nullptr;
    }

    while (pKey->GetNext()) {
        if (pKey->GetKeyData()->tTime >= tTime) {
            break;
        }

        pKey = pKey->GetNext();
    };

    return pKey;
}
