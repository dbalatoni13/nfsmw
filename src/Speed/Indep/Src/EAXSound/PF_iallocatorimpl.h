#ifndef INCLUDED_pf_iallocatorimpl_H
#define INCLUDED_pf_iallocatorimpl_H

#include "Allocator/iallocator.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"

// total size: 0x8
// Decl: 21
class PF_Allocator : public EA::Allocator::IAllocator {
  public:
    // Decl: 23
    PF_Allocator() : mRefcount(1) {}
    ~PF_Allocator() override {} // Decl: 24

    // Overrides: IAllocator

    void *Alloc(size_t size, const EA::TagValuePair &flags) override {
        return gAudioMemoryManager.AllocateMemory(size, "AUD: Pathfinder alloc", true);
    }

    void *Alloc(size_t size) {}

    // Decl: 71
    void Free(void *pBlock, size_t size) override {
        gAudioMemoryManager.FreeMemory(pBlock);
    }

    // Decl: 76
    int AddRef() override {
        return ++mRefcount;
    }

    // Decl: 81
    int Release() override {
        if (--mRefcount < 1) {
            if (this != nullptr) {
                delete this;
            }
            return 0;
        }
        return mRefcount;
    }

  private:
    int mRefcount; // offset 0x4, size 0x4, Decl: 95
};

#endif
