#ifndef MISC_ATTRIB_ALLOC_H
#define MISC_ATTRIB_ALLOC_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

class IAttribAllocator {
  public:
    virtual void *Allocate(std::size_t bytes, const char *name);
    virtual void Free(void *ptr, std::size_t bytes, const char *name);
};

class AttribAlloc {
  public:
    static void *Allocate(std::size_t bytes, const char *name) {
        return mAllocator->Allocate(bytes, name);
    }

    static void Free(void *ptr, std::size_t bytes, const char *name) {
        // TODO is the check here or higher up in the chain?
        if (ptr) {
            mAllocator->Free(ptr, bytes, name);
        }
    }

  private:
    static IAttribAllocator *mAllocator;
};

#endif
