#include "AttribAlloc.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

IAttribAllocator *AttribAlloc::OverrideAllocator(IAttribAllocator *newAllocator) {
    Attrib::Database::Get().CollectGarbage();
    IAttribAllocator *previous = AttribAlloc::mAllocator;
    AttribAlloc::mAllocator = newAllocator;
    return previous;
}

// TODO find correct location
class DefaultAttribAllocator : public IAttribAllocator {
  public:
    void *Allocate(std::size_t bytes, const char *name) override {
        return gFastMem.Alloc(bytes, name);
    }

    void Free(void *ptr, std::size_t bytes, const char *name) override {
        gFastMem.Free(ptr, bytes, name);
    }
};

static DefaultAttribAllocator sDefaultAttribAlloc;
