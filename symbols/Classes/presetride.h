#ifndef ATTRIBSYS_CLASSES_PRESETRIDE_H
#define ATTRIBSYS_CLASSES_PRESETRIDE_H


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

struct presetride : Instance {
void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "presetride");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "presetride");
}

presetride(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    
}

presetride(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    
}

presetride(const presetride &src) : Instance(src) {
    
}

presetride(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    
}

~presetride() {}

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
    return 0x27e73952;
}

const int &transmission() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x07a7a3e5, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &brakes() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x36350867, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &chassis() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xafa210f0, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &nos() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xb1669f64, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &tires() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xbd38d1ca, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &induction() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xc92a0142, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &engine() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xf1f5fbc7, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
};

} // namespace Gen
} // namespace Attrib

#endif
