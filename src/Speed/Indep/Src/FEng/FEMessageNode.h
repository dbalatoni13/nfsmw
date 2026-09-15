#ifndef FEMESSAGENODE_H_
#define FEMESSAGENODE_H_

#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"

// total size: 0x20
// Decl: 21
class FEMessageNode : public FEMinNode {
  public:
    FEMessageNode() {}
    // ~FEMessageNode() override {} // Decl: 21

    FEObject *pMsgFrom;      // offset 0xC, size 0x4, Decl: 23
    FEObject *pMsgTarget;    // offset 0x10, size 0x4, Decl: 24
    FEPackage *pFromPackage; // offset 0x14, size 0x4, Decl: 25
    u32 MsgID;               // offset 0x18, size 0x4, Decl: 26
    u32 ControlMask;         // offset 0x1C, size 0x4, Decl: 27
};

#endif
