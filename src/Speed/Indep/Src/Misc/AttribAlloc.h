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
    virtual void *Allocate(std::size_t bytes, const char *name);
    virtual void Free(void *ptr, std::size_t bytes, const char *name);
};

class AttribAlloc {
  public:
    static IAttribAllocator *OverrideAllocator(IAttribAllocator *newAllocator);

    static void *Allocate(std::size_t bytes, const char *name) {
        return mAllocator->Allocate(bytes,
#ifdef MILESTONE_OPT
                                    name
#else
                                    nullptr
#endif
        );
    }

    static void Free(void *ptr, std::size_t bytes, const char *name) {
        mAllocator->Free(ptr, bytes,
#ifdef MILESTONE_OPT
                         name
#else
                         nullptr
#endif
        );
    }

  private:
    static IAttribAllocator *mAllocator;
};

class HighAttribAlloc : public IAttribAllocator {
  public:
    void *Allocate(std::size_t bytes, const char *name) override {
        if (bytes < 0x401) {
            return gFastMem.Alloc(bytes, name);
        } else {
            return bMalloc(bytes, 0x40);
        }
    }

    void Free(void *ptr, std::size_t bytes, const char *name) override {
        if (bytes < 0x401) {
            gFastMem.Free(ptr, bytes, name);
        } else {
            bFree(ptr);
        }
    }
};

#endif
