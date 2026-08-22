#include <Allocator/iallocator.h>
#include <types.h>
#include "pathi.h"
#define PATH_REAL_EMIT_METHODS
#include "path/PathToReal.h"
#undef PATH_REAL_EMIT_METHODS

struct PathToIAllocator {
    static void *Alloc(int size);
    static void Free(void *pmem);

    static EA::Allocator::IAllocator *memimp;
    static EA::TagValuePair memimptags;
};

EA::Allocator::IAllocator *PathToIAllocator::memimp;
EA::TagValuePair PathToIAllocator::memimptags(NULLALLOCTVP);

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
    PathToIAllocator::memimptags.mValue = flags.mValue;
    PathToIAllocator::memimptags.mNext = flags.mNext;
    Path::memalloc = PathToIAllocator::Alloc;
    Path::memfree = PathToIAllocator::Free;
}

void Path::IPathToReal::SetAbortMessageFunc(PATHAbortMsgFunc f) {
    this->pathabortmsg = f;
}

void Path::IPathToReal::SetDebugPrintFunc(PATHDebugPrintFunc f) {
    this->pathprintf = f;
}

void Path::IPathToReal::SetLogPrintFunc(PATHDebugPrintFunc f) {
    this->pathlogf = f;
}

void Path::IPathToReal::SetSynchMode(Path::SynchMode mode) {
    this->synchmode = mode;
}

Path::SynchMode Path::IPathToReal::GetSynchMode() {
    return this->synchmode;
}
