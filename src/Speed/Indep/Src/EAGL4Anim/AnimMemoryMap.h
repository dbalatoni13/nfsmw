#ifndef EAGL4ANIM_ANIMMEMORYMAP_H
#define EAGL4ANIM_ANIMMEMORYMAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimTypeId.h"
#include "Attribute.h"

namespace EAGL4Anim {

// total size: 0x4
class AnimMemoryMap {
  public:
    AnimMemoryMap() {}

    ~AnimMemoryMap() {}

    AnimTypeId GetType() const {
        return mAnimTypeId;
    }

    void SetType(AnimTypeId t) {
        mAnimTypeId = t;
    }

    unsigned short GetTargetCheckSum() const {
        return mTargetCheckSum;
    }

    void SetTargetCheckSum(unsigned short cs) {
        mTargetCheckSum = cs;
    }

    const AttributeBlock *GetAttributes() const;

    static void InitAnimMemoryMap() {}

  protected:
    AnimTypeId mAnimTypeId;         // offset 0x0, size 0x2
    unsigned short mTargetCheckSum; // offset 0x2, size 0x2
};

}; // namespace EAGL4Anim

#endif
