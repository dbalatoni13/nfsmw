#ifndef ATTRIBSYS_CLASSES_JUNKMAN_H
#define ATTRIBSYS_CLASSES_JUNKMAN_H


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

struct junkman : Instance {
void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "junkman");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "junkman");
}

junkman(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    
}

junkman(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    
}

junkman(const junkman &src) : Instance(src) {
    
}

junkman(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    
}

~junkman() {}

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
    return 0x171737e9;
}

const JunkmanMod &transmission_package(unsigned int index) const {
        const JunkmanMod *resultptr = reinterpret_cast<const JunkmanMod *>(GetAttributePointer(0x25ae629a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const JunkmanMod *>(DefaultDataArea(sizeof(JunkmanMod)));
        }
        return *resultptr;
    }
        
unsigned int Num_transmission_package() const {
            return Get(0x25ae629a).GetLength();
        }

const JunkmanMod &nos_package(unsigned int index) const {
        const JunkmanMod *resultptr = reinterpret_cast<const JunkmanMod *>(GetAttributePointer(0x452d2634, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const JunkmanMod *>(DefaultDataArea(sizeof(JunkmanMod)));
        }
        return *resultptr;
    }
        
unsigned int Num_nos_package() const {
            return Get(0x452d2634).GetLength();
        }

const JunkmanMod &brakes_package(unsigned int index) const {
        const JunkmanMod *resultptr = reinterpret_cast<const JunkmanMod *>(GetAttributePointer(0x56c63b6f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const JunkmanMod *>(DefaultDataArea(sizeof(JunkmanMod)));
        }
        return *resultptr;
    }
        
unsigned int Num_brakes_package() const {
            return Get(0x56c63b6f).GetLength();
        }

const JunkmanMod &induction_package(unsigned int index) const {
        const JunkmanMod *resultptr = reinterpret_cast<const JunkmanMod *>(GetAttributePointer(0x7546359e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const JunkmanMod *>(DefaultDataArea(sizeof(JunkmanMod)));
        }
        return *resultptr;
    }
        
unsigned int Num_induction_package() const {
            return Get(0x7546359e).GetLength();
        }

const JunkmanMod &engine_package(unsigned int index) const {
        const JunkmanMod *resultptr = reinterpret_cast<const JunkmanMod *>(GetAttributePointer(0x9206efd2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const JunkmanMod *>(DefaultDataArea(sizeof(JunkmanMod)));
        }
        return *resultptr;
    }
        
unsigned int Num_engine_package() const {
            return Get(0x9206efd2).GetLength();
        }

const JunkmanMod &chassis_package(unsigned int index) const {
        const JunkmanMod *resultptr = reinterpret_cast<const JunkmanMod *>(GetAttributePointer(0xb6495c9e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const JunkmanMod *>(DefaultDataArea(sizeof(JunkmanMod)));
        }
        return *resultptr;
    }
        
unsigned int Num_chassis_package() const {
            return Get(0xb6495c9e).GetLength();
        }

const JunkmanMod &tires_package(unsigned int index) const {
        const JunkmanMod *resultptr = reinterpret_cast<const JunkmanMod *>(GetAttributePointer(0xc5860f58, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const JunkmanMod *>(DefaultDataArea(sizeof(JunkmanMod)));
        }
        return *resultptr;
    }
        
unsigned int Num_tires_package() const {
            return Get(0xc5860f58).GetLength();
        }

};

} // namespace Gen
} // namespace Attrib

#endif
