// librealshapez.a(allocator.cpp)

#include "realshape.hpp"

#include "Speed/Indep/Libs/allocator/1.5.0/Allocator/iallocator.h"

namespace RealShape {

// El almacenamiento de _Q29RealShape9MemObject.sAllocator (.sdata) ya lo pone
// realcore/systemvars.cpp; aqui solo se referencia, para no duplicarlo.

void GraphObject::SetAllocator(EA::Allocator::IAllocator *allocator) {
    MemObject::sAllocator = allocator;
}

void *MemObject::sAlloc(const char *name, unsigned int size, int alignment, int alignOffset, int allocHigh, char *file,
                        int line) {
    return sAllocator->Alloc(size, EA::TagValuePair(EA::Allocator::ATT_NAME, (const void *)name) +
                                       EA::TagValuePair(EA::Allocator::ATT_ALIGNMENT, alignment) +
                                       EA::TagValuePair(EA::Allocator::ATT_ALIGNMENT_OFFSET, alignOffset) +
                                       EA::TagValuePair(EA::Allocator::ATT_ALLOC_HIGH, allocHigh) +
                                       EA::TagValuePair(EA::Allocator::ATT_FILE, (const void *)file) +
                                       EA::TagValuePair(EA::Allocator::ATT_LINE, line));
}

void MemObject::sFree(void *ptr) {
    sAllocator->Free(ptr, 0);
}

} // namespace RealShape
