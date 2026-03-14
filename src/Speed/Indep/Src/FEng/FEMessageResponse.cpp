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
    NodePool.FreeSingle(static_cast<FEMessageResponse*>(pNode));
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
        int len = FEngStrLen(pString);
        char* pCopy = static_cast<char*>(FEngMalloc(len + 1, 0, 0));
        FEngStrCpy(pCopy, pString);
        ResponseParam = reinterpret_cast<unsigned long>(pCopy);
    }
}

void FEResponse::ReleaseParam() {
    if (FEResponse::HasString(ResponseID) && ResponseParam) {
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
            if (NewCount < Count) {
                copyCount = NewCount;
            }
            unsigned long i = 0;
            if (copyCount != 0) {
                do {
                    pNew[i] = pResponseList[i];
                    i++;
                } while (i < copyCount);
            }
            PurgeResponses();
            Count = NewCount;
            pResponseList = pNew;
        }
    }
}

unsigned long FEMessageResponse::FindResponse(unsigned long ResponseID) const {
    unsigned long count = Count;
    for (unsigned long i = 0; i < count; i++) {
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
    int depth = 1;
    for (;;) {
        Index++;
        unsigned long id = pResponseList[Index].ResponseID;
        switch (id) {
        case 0x300:
        case 0x301:
            depth++;
            break;
        case 0x500:
            if (depth == 1) {
                depth = 0;
            }
            break;
        case 0x501:
            depth--;
            break;
        }
        if (Index >= Count) {
            break;
        }
        if (depth == 0) {
            break;
        }
    }
    return Index;
}