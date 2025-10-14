#ifndef AI_AIAVOIDABLE_H
#define AI_AIAVOIDABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

struct _type_AIAvoidableNeighbors {
    const char *name() {
        return "AIAvoidableNeighbors";
    }
};

// total size: 0x14
class AIAvoidable {
  public:
    virtual ~AIAvoidable();
    virtual bool OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep);

  private:
    struct Grid *mGridNode;                                               // offset 0x0, size 0x4
    UTL::Std::list<AIAvoidable *, _type_AIAvoidableNeighbors> mNeighbors; // offset 0x4, size 0x8
    UTL::COM::IUnknown *mUnk;                                             // offset 0xC, size 0x4
};

#endif
