#ifndef FENG_FEMSGTARGETLIST_H
#define FENG_FEMSGTARGETLIST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct FEObject;

// total size: 0x10
struct FEMsgTargetList {
    unsigned long MsgID;       // offset 0x0, size 0x4
    unsigned long Alloc;       // offset 0x4, size 0x4
    unsigned long Count;       // offset 0x8, size 0x4
    FEObject** pTargets;       // offset 0xC, size 0x4

    inline FEMsgTargetList() : MsgID(0), Alloc(0), Count(0), pTargets(nullptr) {}
    inline FEMsgTargetList(unsigned long NewID) : MsgID(NewID), Alloc(0), Count(0), pTargets(nullptr) {}
    inline ~FEMsgTargetList() {
        if (pTargets) {
            delete[] pTargets;
        }
    }

    inline void SetMsgID(unsigned long NewID) { MsgID = NewID; }
    inline unsigned long GetMsgID() const { return MsgID; }
    inline unsigned long GetCount() const { return Count; }
    inline FEObject* GetTarget(unsigned long Index) { return pTargets[Index]; }
    inline const FEObject* GetTarget(unsigned long Index) const { return pTargets[Index]; }

    void Allocate(unsigned long NewAlloc);
    void AppendTarget(FEObject* pObject);
};

#endif
