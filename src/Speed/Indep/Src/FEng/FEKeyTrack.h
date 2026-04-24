#ifndef FENG_FEKEYTRACK_H
#define FENG_FEKEYTRACK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEGenericVal.h"
#include "FERefList.h"
#include "FEngStandard.h"

template <class T, int N> struct ObjectPool;

// total size: 0x20
struct FEKeyNode : public FEMinNode {
    int tTime;        // offset 0xC, size 0x4
    FEGenericVal Val; // offset 0x10, size 0x10

    inline void Init() {
        next = reinterpret_cast<FEMinNode*>(0xABADCAFE);
        prev = reinterpret_cast<FEMinNode*>(0xABADCAFE);
    }
    inline FEKeyNode() {}

    static void* operator new(unsigned int);
    static void operator delete(void* pNode);

    inline FEKeyNode* GetNext() const { return static_cast<FEKeyNode*>(FEMinNode::GetNext()); }
    inline FEKeyNode* GetPrev() const { return static_cast<FEKeyNode*>(FEMinNode::GetPrev()); }
    inline FEGenericVal* GetKeyData() { return &Val; }

    static ObjectPool<FEKeyNode, 256> NodePool;
};

// total size: 0x38
struct FEKeyTrack {
    unsigned char ParamType;    // offset 0x0, size 0x1
    unsigned char ParamSize;    // offset 0x1, size 0x1
    unsigned char InterpType;   // offset 0x2, size 0x1
    unsigned char InterpAction; // offset 0x3, size 0x1
    int Length : 24;            // offset 0x4, size 0x4
    int LongOffset : 8;         // offset 0x4, size 0x4
    FEKeyNode BaseKey;          // offset 0x8, size 0x20
    FERefList DeltaKeys;        // offset 0x28, size 0x10

    inline FEKeyNode* GetBaseKey() { return &BaseKey; }
    inline FEKeyNode* GetFirstDeltaKey() { return static_cast<FEKeyNode*>(DeltaKeys.GetHead()); }
    inline bool IsReference() const { return DeltaKeys.IsReference(); }

    inline FEKeyTrack()
        : ParamType(0) //
        , ParamSize(0) //
        , InterpType(0) //
        , InterpAction(0) //
        , Length(0)
        , LongOffset(0) {
    }

    FEKeyNode* GetKeyAt(long tTime);
    FEKeyNode* GetDeltaKeyAt(long tTime);
    void operator=(FEKeyTrack& Src);
};

#endif
