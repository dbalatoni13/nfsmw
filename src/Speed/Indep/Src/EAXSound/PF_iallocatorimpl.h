#ifndef EAXSOUND_PF_IALLOCATORIMPL_H
#define EAXSOUND_PF_IALLOCATORIMPL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Allocator/iallocator.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"

class PF_Allocator : public EA::Allocator::IAllocator {
  public:
    PF_Allocator() : mRefcount(1) {}

    virtual ~PF_Allocator() {}
    virtual void *Alloc(unsigned int size, const EA::TagValuePair &flags);
    virtual void Free(void *pBlock, unsigned int size) {
        gAudioMemoryManager.FreeMemory(pBlock);
    }
    virtual int AddRef() {
        return ++mRefcount;
    }
    virtual int Release() {
        if (--mRefcount < 1) {
            if (this) {
                delete this;
            }
            return 0;
        }
        return mRefcount;
    }

    int mRefcount; // offset 0x4, size 0x4
};

extern PF_Allocator gPF_MemoryAllocator;

#endif
