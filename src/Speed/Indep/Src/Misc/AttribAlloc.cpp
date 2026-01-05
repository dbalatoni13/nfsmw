#include "AttribAlloc.h"

IAttribAllocator *AttribAlloc::OverrideAllocator(IAttribAllocator *newAllocator) {
    Attrib::Database::Get().CollectGarbage();
    IAttribAllocator *previous = AttribAlloc::mAllocator;
    AttribAlloc::mAllocator = newAllocator;
    return previous;
}
