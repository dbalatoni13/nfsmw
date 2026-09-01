#include <stdlib.h>
#include <types.h>

#include <Allocator/iallocator.h>

namespace Vp6 {

static EA::Allocator::IAllocator *gAllocator = nullptr;

void SetAllocator(EA::Allocator::IAllocator *allocator) {
    gAllocator = allocator;
}

void *Alloc(int size) {
    if (gAllocator != nullptr) {
        return gAllocator->Alloc(size, NULLALLOCTVP);
    }
    return malloc(size);
}

void Free(void *ptr) {
    if (gAllocator != nullptr) {
        gAllocator->Free(ptr, 0);
    } else {
        free(ptr);
    }
}

} // namespace Vp6
