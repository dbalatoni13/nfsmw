#ifndef EAGL4ANIM_FNANIMFACTORY_H
#define EAGL4ANIM_FNANIMFACTORY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimMemoryMap.h"
#include "FnAnimMemoryMap.h"

namespace EAGL4Anim {

// total size: 0x1
class FnAnimFactory {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    bool Init(AnimMemoryMap *anim) const {}

    FnAnimMemoryMap *Borrow(AnimMemoryMap *anim) {}

    FnAnim *Create(AnimTypeId animTypeId) const {}

    FnAnimMemoryMap *Create(AnimMemoryMap *anim) const {}

    void Delete(FnAnim *fnAnim) const {}

    static FnAnimFactory *mpFactory; // size: 0x4, address: 0x8041711C
};

}; // namespace EAGL4Anim

#endif
