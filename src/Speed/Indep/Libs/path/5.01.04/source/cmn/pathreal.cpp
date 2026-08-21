#include <Allocator/iallocator.h>
#include <types.h>
#include "pathi.h"
#include "path/PathToReal.h"

struct PathToIAllocator {
    static void *Alloc(int size);
    static void Free(void *pmem);

    static EA::Allocator::IAllocator *memimp;
    static EA::TagValuePair memimptags;
};

EA::Allocator::IAllocator *PathToIAllocator::memimp;
EA::TagValuePair PathToIAllocator::memimptags(0, 0);

PathToReal::PathToReal() {}

PathToReal::~PathToReal() {}

void *PathToIAllocator::Alloc(int size) {
    if (PathToIAllocator::memimp != 0) {
        return PathToIAllocator::memimp->Alloc(size, PathToIAllocator::memimptags);
    }
    return 0;
}

void PathToIAllocator::Free(void *pmem) {
    if (PathToIAllocator::memimp != 0) {
        PathToIAllocator::memimp->Free(pmem, 0);
    }
}

void PATH_setallocator(EA::Allocator::IAllocator *allocator, const EA::TagValuePair &flags) {
    PathToIAllocator::memimp = allocator;
    PathToIAllocator::memimptags.mTag = flags.mTag;
    PathToIAllocator::memimptags.mNext = flags.mNext;
    PathToIAllocator::memimptags.mValue = flags.mValue;
    Path::memalloc = PathToIAllocator::Alloc;
    Path::memfree = PathToIAllocator::Free;
}
