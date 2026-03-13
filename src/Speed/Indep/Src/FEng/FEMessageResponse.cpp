#include "FEMessageResponse.h"
#include "FEngStandard.h"

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