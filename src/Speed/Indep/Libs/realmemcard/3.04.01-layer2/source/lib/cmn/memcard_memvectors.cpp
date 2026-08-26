#include "../../../include/common/realmemcard/memcard_interface.h"

namespace Realmc {

static EA::Allocator::IAllocator *gAllocator = nullptr;

void SetMemAllocator(EA::Allocator::IAllocator *allocator) {
    gAllocator = allocator;
}

void *AllocateMemSize(const char *pBlockName, int size, int align, int, int) {
    return gAllocator->Alloc(
        size,
        EA::TagValuePair(EA::Allocator::ATT_NAME, pBlockName) +
            EA::TagValuePair(EA::Allocator::ATT_ALIGNMENT, align));
}

void FreeMemSize(void *pBlock, int size) {
    gAllocator->Free(pBlock, size);
}

} // namespace Realmc
