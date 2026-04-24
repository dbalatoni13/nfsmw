#ifndef FENG_FEMESSAGERESPONSE_H
#define FENG_FEMESSAGERESPONSE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEList.h"
#include "FEngStandard.h"

template <class T, int N> struct ObjectPool;

// total size: 0xC
struct FEResponse {
    unsigned long ResponseID;     // offset 0x0, size 0x4
    unsigned long ResponseParam;  // offset 0x4, size 0x4
    unsigned long ResponseTarget; // offset 0x8, size 0x4

    inline FEResponse() : ResponseID(0), ResponseParam(0), ResponseTarget(0) {}

    static inline void* operator new[](unsigned int size) {
        return FEngMalloc(size, nullptr, 0);
    }

    static inline bool HasString(unsigned long ResponseID) {
        return (ResponseID - 0x200u < 5) && (ResponseID != 0x203);
    }

    inline bool HasString() const { return HasString(ResponseID); }

    inline void SetID(unsigned long ID) { ResponseID = ID; }
    inline void SetParam(unsigned long Value) { ResponseParam = Value; }

    ~FEResponse();
    FEResponse& operator=(FEResponse& rhs);
    void SetParam(const char* pString);
    void ReleaseParam();
};

// total size: 0x18
struct FEMessageResponse : public FEMinNode {
    unsigned long MsgID;             // offset 0xC, size 0x4
    unsigned long Count;             // offset 0x10, size 0x4
    FEResponse* pResponseList;       // offset 0x14, size 0x4

    inline void Init() {
        MsgID = 0;
        Count = 0;
        pResponseList = nullptr;
    }
    inline FEMessageResponse() : MsgID(0), Count(0), pResponseList(nullptr) {}
    ~FEMessageResponse() override;

    static void* operator new(unsigned int);
    static void operator delete(void* pNode);

    void PurgeResponses();
    void SetCount(unsigned long NewCount);
    unsigned long FindResponse(unsigned long CommandID) const;
    unsigned long FindConditionBranchTarget(unsigned long Index) const;

    inline unsigned long GetMsgID() const { return MsgID; }
    inline void SetMsgID(unsigned long NewID) { MsgID = NewID; }
    inline unsigned long GetCount() const { return Count; }
    inline FEResponse* GetResponse(int Index) const { return &pResponseList[Index]; }
    inline FEMessageResponse* GetNext() { return static_cast<FEMessageResponse*>(FEMinNode::GetNext()); }
    inline FEMessageResponse* GetPrev() { return static_cast<FEMessageResponse*>(FEMinNode::GetPrev()); }

    static ObjectPool<FEMessageResponse, 64> NodePool;
};

#endif
