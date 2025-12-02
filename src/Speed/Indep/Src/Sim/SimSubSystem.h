#ifndef SIM_SIMSUBSYSTEM_H
#define SIM_SIMSUBSYSTEM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

namespace Sim {

// total size: 0x14
class SubSystem {
  public:
    void ValidateHeap(bool before, bool initializing);

    static void Init(const UCrc32 &sig) {
        SubSystem *s;
        for (s = mHead; s != nullptr; s = s->mNext) {
            if (s->mSig == sig && s->mInit) {
                s->ValidateHeap(true, true);
                s->mInit();
                s->ValidateHeap(false, true);
                break;
            }
        }
    }

  private:
    static SubSystem *mHead;

    void (*mInit)();    // offset 0x0, size 0x4
    void (*mRestore)(); // offset 0x4, size 0x4
    SubSystem *mNext;   // offset 0x8, size 0x4
    UCrc32 mSig;        // offset 0xC, size 0x4
    const char *mName;  // offset 0x10, size 0x4
};

}; // namespace Sim

#endif
