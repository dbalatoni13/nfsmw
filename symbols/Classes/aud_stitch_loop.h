#ifndef ATTRIBSYS_CLASSES_AUD_STITCH_LOOP_H
#define ATTRIBSYS_CLASSES_AUD_STITCH_LOOP_H


#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct aud_stitch_loop : Instance {
void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "aud_stitch_loop");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "aud_stitch_loop");
}

aud_stitch_loop(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    
}

aud_stitch_loop(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    
}

aud_stitch_loop(const aud_stitch_loop &src) : Instance(src) {
    
}

aud_stitch_loop(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    
}

~aud_stitch_loop() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

void Change(const RefSpec &refspec) {
    Instance::Change(refspec);
}

static Key ClassKey() {
    return 0x3473edcd;
}

const STICH_COLLISION_TYPE &stitch() const {
        const STICH_COLLISION_TYPE *resultptr = reinterpret_cast<const STICH_COLLISION_TYPE *>(GetAttributePointer(0x1553cd23, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const STICH_COLLISION_TYPE *>(DefaultDataArea(sizeof(STICH_COLLISION_TYPE)));
        }
        return *resultptr;
    }
        
const int &overlap() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xad8c27f5, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
};

} // namespace Gen
} // namespace Attrib

#endif
