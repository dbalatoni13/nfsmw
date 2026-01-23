#ifndef ATTRIBSYS_CLASSES_LIGHTSHADERS_H
#define ATTRIBSYS_CLASSES_LIGHTSHADERS_H


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

struct lightshaders : Instance {
void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "lightshaders");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "lightshaders");
}

lightshaders(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    
}

lightshaders(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    
}

lightshaders(const lightshaders &src) : Instance(src) {
    
}

lightshaders(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    
}

~lightshaders() {}

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
    return 0xbc8c1aa3;
}

const bool &useVertexColour() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x2de05384, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &shadername() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x39004977, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &name() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0xba41a46a, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
};

} // namespace Gen
} // namespace Attrib

#endif
