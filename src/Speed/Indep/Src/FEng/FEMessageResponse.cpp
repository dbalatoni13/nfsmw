#include "FEMessageResponse.h"
#include "FEngStandard.h"
#include "ObjectPool.h"

FEResponse::~FEResponse() {
    ReleaseParam();
}

FEResponse &FEResponse::operator=(FEResponse &rhs) {
    ReleaseParam();
    ResponseID = rhs.ResponseID;
    if (HasString()) {
        SetParam(reinterpret_cast<const char *>(rhs.ResponseParam));
    } else {
        SetParam(rhs.ResponseParam);
    }
    ResponseTarget = rhs.ResponseTarget;
    return *this;
}

void FEResponse::SetParam(const char *pString) {
    ReleaseParam();
    if (pString != nullptr) {
        u32 Len = FEngStrLen(pString);
        char *pPathCopy = FNEW char[Len + 1];
        FEngStrCpy(pPathCopy, pString);
        ResponseParam = reinterpret_cast<u32>(pPathCopy);
    }
}

void FEResponse::ReleaseParam() {
    if (HasString() && ResponseParam) {
        delete[] reinterpret_cast<char *>(ResponseParam);
    }
    ResponseParam = 0;
}

ObjectPool<FEMessageResponse, 64> FEMessageResponse::NodePool;

FEMessageResponse::~FEMessageResponse() {
    PurgeResponses();
}

void *FEMessageResponse::operator new(size_t) {
    FEMessageResponse *pNode = NodePool.AllocSingle();
    pNode->Init();
    return pNode;
}

void FEMessageResponse::operator delete(void *pNode) {
    static_cast<FEMessageResponse *>(pNode)->~FEMessageResponse();
    NodePool.FreeSingle(static_cast<FEMessageResponse *>(pNode));
}

void FEMessageResponse::PurgeResponses() {
    delete[] pResponseList;
    pResponseList = nullptr;
    Count = 0;
}

void FEMessageResponse::SetCount(u32 NewCount) {
    if (NewCount == Count) {
        return;
    }
    if (NewCount == 0) {
        PurgeResponses();
        return;
    }
    FEResponse *pNewList = FNEW FEResponse[NewCount];
    u32 i;
    u32 CopyCount = Count;
    if (CopyCount > NewCount) {
        CopyCount = NewCount;
    }
    i = 0;
    while (i < CopyCount) {
        pNewList[i] = pResponseList[i];
        i++;
    }
    delete[] pResponseList;
    pResponseList = pNewList;
    Count = NewCount;
}

u32 FEMessageResponse::FindResponse(u32 CommandID) const {
    for (u32 i = 0; i < Count; i++) {
        if (pResponseList[i].ResponseID == CommandID) {
            return i;
        }
    }
    return 0xFFFFFFFF;
}

// UNSOLVED
u32 FEMessageResponse::FindConditionBranchTarget(u32 Index) const {
    u32 Nest = 1;
    u32 Result = Count;
    if (Index != Result - 1) {
        do {
            Index++;
            switch (pResponseList[Index].ResponseID) {
                case MR_IfScriptEquals:
                case MR_IfScriptNotEquals:
                    Nest++;
                    break;
                case MR_Else:
                    if (Nest == 1) {
                        Nest = 0;
                    }
                    break;
                case MR_EndIf:
                    Nest--;
                    break;
            }
        } while (Index < Result && Nest != 0);
        return Index;
    }
    return Result;
}
