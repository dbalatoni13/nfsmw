#include "FEMessageResponse.h"
#include "FESlotPool.h"
#include "FEngStandard.h"
#include "ObjectPool.h"

ObjectPool<FEMessageResponse, 64> FEMessageResponse::NodePool;

void* FEMessageResponse::operator new(unsigned int) {
    FEMessageResponse* pNode = NodePool.AllocSingle();
    pNode->Init();
    return pNode;
}

void FEMessageResponse::operator delete(void* pNode) {
    FEMessageResponse* pDeleteNode = static_cast<FEMessageResponse*>(pNode);
    pDeleteNode->~FEMessageResponse();
    NodePool.FreeSingleNoDestroy(pDeleteNode);
}

FEResponse::~FEResponse() {
    ReleaseParam();
}

FEResponse& FEResponse::operator=(FEResponse& rhs) {
    ReleaseParam();
    unsigned long id = rhs.ResponseID;
    ResponseID = id;
    if (FEResponse::HasString(id)) {
        SetParam(reinterpret_cast<const char*>(rhs.ResponseParam));
    } else {
        ResponseParam = rhs.ResponseParam;
    }
    ResponseTarget = rhs.ResponseTarget;
    return *this;
}

void FEResponse::SetParam(const char* pString) {
    ReleaseParam();
    if (pString) {
        unsigned long Len = FEngStrLen(pString);
        char* pPathCopy = FENG_NEW char[Len + 1];
        FEngStrCpy(pPathCopy, pString);
        ResponseParam = reinterpret_cast<unsigned long>(pPathCopy);
    }
}

void FEResponse::ReleaseParam() {
    if (HasString() && ResponseParam) {
        delete[] reinterpret_cast<char*>(ResponseParam);
    }
    ResponseParam = 0;
}

FEMessageResponse::~FEMessageResponse() {
    PurgeResponses();
}

void FEMessageResponse::PurgeResponses() {
    delete[] pResponseList;
    pResponseList = nullptr;
    Count = 0;
}

void FEMessageResponse::SetCount(unsigned long NewCount) {
    if (NewCount != Count) {
        if (NewCount == 0) {
            PurgeResponses();
        } else {
            FEResponse* pNew = new FEResponse[NewCount];
            unsigned long copyCount = Count;
            if (copyCount > NewCount) {
                copyCount = NewCount;
            }
            unsigned long i = 0;
            while (i < copyCount) {
                pNew[i] = pResponseList[i];
                i++;
            }
            delete[] pResponseList;
            pResponseList = pNew;
            Count = NewCount;
        }
    }
}

unsigned long FEMessageResponse::FindResponse(unsigned long ResponseID) const {
    for (unsigned long i = 0; i < Count; i++) {
        if (pResponseList[i].ResponseID == ResponseID) {
            return i;
        }
    }
    return 0xFFFFFFFF;
}

unsigned long FEMessageResponse::FindConditionBranchTarget(unsigned long Index) const {
    if (Index == Count - 1) {
        return Count;
    }
    unsigned long Nest = 1;
    goto body;
    do {
        if (Nest == 0)
            break;
body:
        Index++;
        unsigned long id = pResponseList[Index].ResponseID;
        switch (id) {
        case 0x300:
        case 0x301:
            Nest++;
            break;
        case 0x500:
            if (Nest == 1) {
                Nest = 0;
            }
            break;
        case 0x501:
            Nest--;
            break;
        }
    } while (Index < Count);
    return Index;
}
