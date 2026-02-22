#ifndef EAGL4ANIM_FNANIMMEMORYMAP_H
#define EAGL4ANIM_FNANIMMEMORYMAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "FnAnim.h"

namespace EAGL4Anim {

// total size: 0x10
class FnAnimMemoryMap : public FnAnim {
  public:
    FnAnimMemoryMap();

    // Overrides: FnAnimSuper
    ~FnAnimMemoryMap() override;

    virtual void SetAnimMemoryMap(AnimMemoryMap *anim);

    virtual AnimMemoryMap *GetAnimMemoryMap();

    virtual const AnimMemoryMap *GetAnimMemoryMap() const;

    // Overrides: FnAnim
    unsigned short GetTargetCheckSum() const override;

  protected:
    AnimMemoryMap *mpAnim; // offset 0xC, size 0x4
};

}; // namespace EAGL4Anim

#endif
