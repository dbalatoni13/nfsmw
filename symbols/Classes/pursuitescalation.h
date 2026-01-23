#ifndef ATTRIBSYS_CLASSES_PURSUITESCALATION_H
#define ATTRIBSYS_CLASSES_PURSUITESCALATION_H


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

struct pursuitescalation : Instance {
void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "pursuitescalation");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "pursuitescalation");
}

pursuitescalation(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    
}

pursuitescalation(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    
}

pursuitescalation(const pursuitescalation &src) : Instance(src) {
    
}

pursuitescalation(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    
}

~pursuitescalation() {}

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
    return 0xd6d4330b;
}

const RefSpec &racetable(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x2283ecaf, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
unsigned int Num_racetable() const {
            return Get(0x2283ecaf).GetLength();
        }

const RefSpec &heattable(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xd4b0cc11, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
unsigned int Num_heattable() const {
            return Get(0xd4b0cc11).GetLength();
        }

const RefSpec &supportracetable(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xe5332008, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
unsigned int Num_supportracetable() const {
            return Get(0xe5332008).GetLength();
        }

const RefSpec &supporttable(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xf3918f68, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
unsigned int Num_supporttable() const {
            return Get(0xf3918f68).GetLength();
        }

};

} // namespace Gen
} // namespace Attrib

#endif
