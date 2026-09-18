#ifndef MISC_ATTRIB_ALLOC_H
#define MISC_ATTRIB_ALLOC_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>
#include <types.h>

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"

class IAttribAllocator {
  public:
    virtual void *Allocate(std::size_t bytes, const char *name) = 0;
    virtual void Free(void *ptr, std::size_t bytes, const char *name) = 0;
};

// TODO figure out whether we need the ifdefs
class AttribAlloc {
  public:
    static IAttribAllocator *OverrideAllocator(IAttribAllocator *newAllocator);

    static void *Allocate(std::size_t bytes, const char *name) {
        return mAllocator->Allocate(bytes,
#ifdef MILESTONE_BUILD
                                    name
#else
                                    nullptr
#endif
        );
    }

    static void Free(void *ptr, std::size_t bytes, const char *name) {
        mAllocator->Free(ptr, bytes,
#ifdef MILESTONE_BUILD
                         name
#else
                         nullptr
#endif
        );
    }

  private:
    static IAttribAllocator *mAllocator;
};

#endif
