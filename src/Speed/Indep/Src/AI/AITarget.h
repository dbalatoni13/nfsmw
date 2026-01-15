#ifndef AI_AITARGET_H
#define AI_AITARGET_H

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
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
    static bool CanAquire(const ISimable *who);

    void *operator new(size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    AITarget(ISimable *owner);
    void Clear();
    void Aquire(ISimable *target);
    void Aquire(const UMath::Vector3 &position);
    void Aquire(const UMath::Vector3 &position, const UMath::Vector3 &direction);
    void Aquire(const AITarget *aitarget);
    bool IsTarget(const AITarget *aitarget) const;
    float GetSpeed() const;
    const UMath::Vector3 &GetLinearVelocity() const;
    void TrackInternal();

    virtual ~AITarget();

    ISimable *GetSimable() {
        return mTargetSimable;
    }

    const UMath::Vector3 &GetPosition() const {
        return mTargetPosition;
    }

    void GetForwardVector(UMath::Vector3 &dir) const {
        dir = mTargetDirection;
    }

    bool IsValid() const {
        return mValid;
    }

    template <typename T> bool QueryInterface(T **out) {
        if (mTargetSimable) {
            return mTargetSimable->QueryInterface(out);
        }
        *out = nullptr;
        return false;
    }

  private:
    ISimable *mOwner;                         // offset 0x8, size 0x4
    ALIGN_16 UMath::Vector3 mTargetPosition;  // offset 0xC, size 0xC
    ISimable *mTargetSimable;                 // offset 0x18, size 0x4
    ALIGN_16 UMath::Vector3 mTargetDirection; // offset 0x1C, size 0xC
    bool mValid;                              // offset 0x28, size 0x1
    ALIGN_16 UMath::Vector3 mDirTo;           // offset 0x2C, size 0xC
    float mDistTo;                            // offset 0x38, size 0x4
};

#endif
