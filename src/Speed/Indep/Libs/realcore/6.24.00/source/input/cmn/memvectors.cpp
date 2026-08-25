#include "../../../../../../../../../include/dol2asm.h"
#include "../../../include/common/realcore/input.h"

namespace RealInput {

SECTION_SDATA EA::Allocator::IAllocator *gAllocator = nullptr;

void ReleaseAllocator() {
    if (gAllocator != nullptr) {
        gAllocator->Release();
        gAllocator = nullptr;
    }
}

void SetAllocator(EA::Allocator::IAllocator *allocator) {
    ReleaseAllocator();
    gAllocator = allocator;
    gAllocator->AddRef();
}

EA::Allocator::IAllocator *GetAllocator() {
    return gAllocator;
}

inline void *RealAlloc(const char *pBlockName, int size, int alignment, int alignmentOffset, int allocHigh) {
    return GetAllocator()->Alloc(
        size,
        EA::TagValuePair(EA::Allocator::ATT_NAME, pBlockName) +
            EA::TagValuePair(EA::Allocator::ATT_ALIGNMENT, alignment) +
            EA::TagValuePair(EA::Allocator::ATT_ALIGNMENT_OFFSET, alignmentOffset) +
            EA::TagValuePair(EA::Allocator::ATT_ALLOC_HIGH, allocHigh));
}

void *AllocateMemSize(const char *pBlockName, int size, int alignment, int alignmentOffset, int flags) {
    return RealAlloc(pBlockName, size, alignment, alignmentOffset, flags);
}

void FreeMemSize(void *pBlock, int size) {
    GetAllocator()->Free(pBlock, size);
}

}
