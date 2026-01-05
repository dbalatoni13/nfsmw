#ifndef MISC_ATTRIB_ALLOC_H
#define MISC_ATTRIB_ALLOC_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>
#include <types.h>

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
#if DEBUG
                                    name
#else
                                    nullptr
#endif
        );
    }

    static void Free(void *ptr, std::size_t bytes, const char *name) {
        mAllocator->Free(ptr, bytes,
#if DEBUG
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
