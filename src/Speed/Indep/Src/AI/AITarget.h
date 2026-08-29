//
//
//
//
//
//
//
//
//
//
//
#ifndef __AITARGET_H
#define __AITARGET_H 1

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"

#include <types.h>

// total size: 0x40
// Decl: 42
class AITarget : public bTNode<AITarget> {
  public:
    USE_FASTALLOC(AITarget);

    static void Register(ISimable *who);
    static void UnRegister(ISimable *who);
    static void TrackAll();
    static void Track(const ISimable *who);
    static bool CanAquire(const ISimable *who);

    AITarget(ISimable *owner);
    virtual ~AITarget();

    bool IsValid() const {
        return this->mValid;
    }

    void Aquire(const AITarget *aitarget);
    void Aquire(const UMath::Vector3 &position, const UMath::Vector3 &direction);
    void Aquire(const UMath::Vector3 &position);
    void Aquire(ISimable *target);
    void Clear();

    bool IsTarget(const AITarget *aitarget) const;

    bool IsTarget(const UTL::COM::IUnknown *object) const {
        return UTL::COM::ComparePtr(this->mTargetSimable, object);
    }

    bool IsSimable() const {
        return this->mTargetSimable != nullptr;
    }

    ISimable *GetSimable() const {
        return this->mTargetSimable;
    }

    const UMath::Vector3 &GetPosition() const {
        return this->mTargetPosition;
    }

    const UMath::Vector3 &GetDirection() const {
        return this->mTargetDirection;
    }

    void GetForwardVector(UMath::Vector3 &dir) const {
        dir = this->mTargetDirection;
    }

    float GetSpeed() const;
    const UMath::Vector3 &GetLinearVelocity() const;

    const UMath::Vector3 &GetDirTo() const {
        return this->mDirTo;
    }

    // Decl: 57
    float GetDistTo() const {
        return this->mDistTo;
    }

    // Decl: 60
    template <typename T> bool QueryInterface(T **out) {
        if (this->mTargetSimable) {
            return this->mTargetSimable->QueryInterface(out);
        }
        *out = nullptr;
        return false;
    }

  private:
    void TrackInternal();

    ISimable *mOwner;                         // offset 0x8, size 0x4
    ALIGN_16 UMath::Vector3 mTargetPosition;  // offset 0xC, size 0xC
    ISimable *mTargetSimable;                 // offset 0x18, size 0x4
    ALIGN_16 UMath::Vector3 mTargetDirection; // offset 0x1C, size 0xC
    bool mValid;                              // offset 0x28, size 0x1
    ALIGN_16 UMath::Vector3 mDirTo;           // offset 0x2C, size 0xC
    float mDistTo;                            // offset 0x38, size 0x4
};

#endif
