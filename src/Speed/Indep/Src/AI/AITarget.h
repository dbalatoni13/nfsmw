#ifndef AI_AITARGET_H
#define AI_AITARGET_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"

#include <types.h>

// total size: 0x40
class AITarget : public bTNode<AITarget> {
  public:
    static void TrackAll();

    ISimable *GetSimable() {
        return mTargetSimable;
    }

    const UMath::Vector3 &GetPosition() const {
        return mTargetPosition;
    }

    void GetForwardVector(UMath::Vector3 &dir) const {
        dir = mTargetDirection;
    }

    bool QueryInterface(IPerpetrator **out) {
        if (mTargetSimable) {
            return mTargetSimable->QueryInterface(out);
        }
        *out = nullptr;
        return false;
    }

  private:
    ISimable *mOwner;                              // offset 0x8, size 0x4
    ALIGN_PS2(16) UMath::Vector3 mTargetPosition;  // offset 0xC, size 0xC
    ISimable *mTargetSimable;                      // offset 0x18, size 0x4
    ALIGN_PS2(16) UMath::Vector3 mTargetDirection; // offset 0x1C, size 0xC
    bool mValid;                                   // offset 0x28, size 0x1
    ALIGN_PS2(16) UMath::Vector3 mDirTo;           // offset 0x2C, size 0xC
    float mDistTo;                                 // offset 0x38, size 0x4
};

#endif
