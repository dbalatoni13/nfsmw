#include "AttribAlloc.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

static DefaultAttribAllocator sDefaultAttribAlloc;
IAttribAllocator *AttribAlloc::mAllocator = &sDefaultAttribAlloc;

DefaultAttribAllocator::DefaultAttribAllocator() {}

void *DefaultAttribAllocator::Allocate(std::size_t bytes, const char *name) {
    return bMalloc(bytes, name, 0, 0);
}

void DefaultAttribAllocator::Free(void *ptr, std::size_t bytes, const char *name) {
    bFree(ptr);
}

IAttribAllocator *AttribAlloc::OverrideAllocator(IAttribAllocator *newAllocator) {
    Attrib::Database::Get().CollectGarbage();
    IAttribAllocator *previous = AttribAlloc::mAllocator;
    AttribAlloc::mAllocator = newAllocator;
    return previous;
}
