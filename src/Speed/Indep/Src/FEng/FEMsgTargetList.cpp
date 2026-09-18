#include "Speed/Indep/Src/FEng/FEMsgTargetList.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEObjectCallback.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"

void FEMsgTargetList::Allocate(u32 NewAlloc) {
    if (NewAlloc == 0) {
        if (pTargets) {
            delete[] reinterpret_cast<char *>(pTargets);
        }
        pTargets = nullptr;
        Count = 0;
        Alloc = 0;
    } else if (NewAlloc != Alloc) {
        FEObject **pNewTargets = FNEW FEObject *[NewAlloc];
        if (NewAlloc < Alloc) {
            FEngMemCpy(pNewTargets, pTargets, NewAlloc * sizeof(FEObject *));
        } else {
            FEngMemCpy(pNewTargets, pTargets, Alloc * sizeof(FEObject *));
        }
        if (pTargets) {
            delete[] reinterpret_cast<char *>(pTargets);
        }
        pTargets = pNewTargets;
    }
    Alloc = NewAlloc;
}

void FEMsgTargetList::AppendTarget(FEObject *pObject) {
    if (Count == Alloc) {
        Allocate(Count + 1);
    }
    pTargets[Count++] = pObject;
}

// File: speed/indep/src/feng/FEMsgTargetList.cpp
// total size: 0x14
// Decl: speed/indep/src/feng/FEMsgTargetList.cpp:79
class FECountMsgs : public FEObjectCallback {
  public:
    u32 *pMsgList, *pCountList; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEMsgTargetList.cpp:81
    u32 MsgCount, MsgAlloc;     // offset 0xC, size 0x4, Decl: speed/indep/src/feng/FEMsgTargetList.cpp:82

    FECountMsgs() {}           // Decl: speed/indep/src/feng/FEMsgTargetList.cpp:84
    ~FECountMsgs() override {} // Decl: speed/indep/src/feng/FEMsgTargetList.cpp:85

    bool Callback(struct FEObject *pObj) override; // Decl: speed/indep/src/feng/FEMsgTargetList.cpp:91
};

// total size: 0xC
// Decl: speed/indep/src/feng/FEMsgTargetList.cpp:129
class FEBuildMsgs : public FEObjectCallback {
  public:
    FEMsgTargetList *pList; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEMsgTargetList.cpp:131
    u32 Count;              // offset 0x8, size 0x4, Decl: speed/indep/src/feng/FEMsgTargetList.cpp:132

    FEBuildMsgs() {}

    bool Callback(struct FEObject *pObj) override; // Decl: speed/indep/src/feng/FEMsgTargetList.cpp:137
};

// Decl: speed/indep/src/feng/FEMsgTargetList.cpp:198
// Range: 0x801856DC -> 0x8018571C
// this: r3
FEMsgTargetList *FEPackage::GetMessageTargets(u32 MsgID) {
    for (u32 i = 0; i < NumMsgTargets; i++) {
        if (pMsgTargets[i].GetMsgID() == MsgID) {
            return &pMsgTargets[i];
        }
    }
    return nullptr;
}
