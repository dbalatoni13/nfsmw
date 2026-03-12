#include "AttribAlloc.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"

void *bMalloc(int size, int allocation_params);
void bFree(void *ptr);

IAttribAllocator *AttribAlloc::OverrideAllocator(IAttribAllocator *newAllocator) {
    Attrib::Database::Get().CollectGarbage();
    IAttribAllocator *previous = AttribAlloc::mAllocator;
    AttribAlloc::mAllocator = newAllocator;
    return previous;
}

class DefaultAttribAllocator : public IAttribAllocator {
  public:
    void *Allocate(std::size_t bytes, const char *name) override {
        return gFastMem.Alloc(bytes, name);
    }

    void Free(void *ptr, std::size_t bytes, const char *name) override {
        gFastMem.Free(ptr, bytes, name);
    }
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
