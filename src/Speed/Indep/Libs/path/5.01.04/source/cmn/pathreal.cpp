#pragma implementation "IPathToReal.h"

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

// El `= 0` explicito no es cosmetico: GCC 2.9 bautiza `_GLOBAL_.I.` con el
// primer global CON inicializador, y una definicion tentativa se difiere al
// final de la TU. Sin el, el simbolo salia `_GLOBAL_.I.__10PathToReal` y el
// objetivo pone `_GLOBAL_.I._16PathToIAllocator.memimp`.
EA::Allocator::IAllocator *PathToIAllocator::memimp = 0;
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
    PathToIAllocator::memimptags = flags;
    Path::memalloc = PathToIAllocator::Alloc;
    Path::memfree = PathToIAllocator::Free;
}





