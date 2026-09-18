#ifndef FEMSGTARGETLIST_H_
#define FEMSGTARGETLIST_H_

#include <types.h>
#include "Speed/Indep/Src/FEng/FEObject.h"

// File: speed/indep/src/feng/FEMsgTargetList.h
// total size: 0x10
// Decl: speed/indep/src/feng/FEMsgTargetList.h:13
class FEMsgTargetList {
  private:
    u32 MsgID;           // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEMsgTargetList.h:15
    u32 Alloc, Count;    // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEMsgTargetList.h:16
    FEObject **pTargets; // offset 0xC, size 0x4, Decl: speed/indep/src/feng/FEMsgTargetList.h:17

  public:
    FEMsgTargetList() : MsgID(0), Alloc(0), Count(0), pTargets(nullptr) {} // Decl: speed/indep/src/feng/FEMsgTargetList.h:20
    FEMsgTargetList(u32 NewID) : MsgID(NewID), Alloc(0), Count(0), pTargets(nullptr) {}
    ~FEMsgTargetList() {
        if (pTargets) {
            delete[] pTargets;
        }
    }

    void SetMsgID(u32 NewID) {
        MsgID = NewID;
    }

    u32 GetMsgID() const {
        return MsgID;
    }

    void Allocate(u32 NewAlloc);

    u32 GetCount() const {
        return Count;
    }

    void AppendTarget(FEObject *pObject); // Decl: speed/indep/src/feng/FEMsgTargetList.h:30

    FEObject *GetTarget(u32 Index) {
        return pTargets[Index];
    }

    const FEObject *GetTarget(u32 Index) const {
        return pTargets[Index];
    }
};

#endif
