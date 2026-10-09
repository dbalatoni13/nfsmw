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
#ifndef ATTRIBALLOC_H__
#define ATTRIBALLOC_H__

#include <cstddef>
#include <types.h>

// Decl: 19
class IAttribAllocator {
  public:
    virtual void *Allocate(std::size_t bytes, const char *name);
    virtual void Free(void *ptr, std::size_t bytes, const char *name);
};

// TODO figure out whether we need the ifdefs
// Decl: 36
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
    static IAttribAllocator *mAllocator; // Decl: 59
};

#endif
