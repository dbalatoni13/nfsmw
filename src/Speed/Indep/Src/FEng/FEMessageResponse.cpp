#include "FEMessageResponse.h"
#include "FEngStandard.h"
#include "ObjectPool.h"

FEResponse::~FEResponse() {
    ReleaseParam();
}

FEResponse &FEResponse::operator=(FEResponse &rhs) {
    ReleaseParam();
    ResponseID = rhs.ResponseID;
    if (FEResponse::HasString(ResponseID)) {
        SetParam(reinterpret_cast<const char *>(rhs.ResponseParam));
    } else {
        ResponseParam = rhs.ResponseParam;
    }
    ResponseTarget = rhs.ResponseTarget;
    return *this;
}

void FEResponse::SetParam(const char *pString) {
    ReleaseParam();
    if (pString) {
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
    FEMessageResponse *pDeleteNode = static_cast<FEMessageResponse *>(pNode);
    pDeleteNode->~FEMessageResponse();
    NodePool.FreeSingle(pDeleteNode);
}

void FEMessageResponse::PurgeResponses() {
    delete[] pResponseList;
    pResponseList = nullptr;
    Count = 0;
}

void FEMessageResponse::SetCount(u32 NewCount) {
    if (NewCount != Count) {
        if (NewCount == 0) {
            PurgeResponses();
        } else {
            FEResponse *pNewList = FNEW FEResponse[NewCount];
            u32 copyCount = Count;
            if (copyCount > NewCount) {
                copyCount = NewCount;
            }
            u32 i = 0;
            while (i < copyCount) {
                pNewList[i] = pResponseList[i];
                i++;
            }
            delete[] pResponseList;
            pResponseList = pNewList;
            Count = NewCount;
        }
    }
}

u32 FEMessageResponse::FindResponse(u32 ResponseID) const {
    for (u32 i = 0; i < Count; i++) {
        if (pResponseList[i].ResponseID == ResponseID) {
            return i;
        }
    }
    return 0xFFFFFFFF;
}

u32 FEMessageResponse::FindConditionBranchTarget(u32 Index) const {
    u32 Nest = Count;
    if (Index != Nest - 1) {
        u32 Result = 1;
        do {
            Index++;
            switch (pResponseList[Index].ResponseID) {
                case MR_IfScriptEquals:
                case MR_IfScriptNotEquals:
                    Result++;
                    break;
                case MR_Else:
                    if (Result == 1) {
                        Result = 0;
                    }
                    break;
                case MR_EndIf:
                    Result--;
                    break;
            }
        } while (Index < Nest && Result != 0);
        return Index;
    }
    return Nest;
}
