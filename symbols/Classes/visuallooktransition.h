#ifndef ATTRIBSYS_CLASSES_VISUALLOOKTRANSITION_H
#define ATTRIBSYS_CLASSES_VISUALLOOKTRANSITION_H


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

struct visuallooktransition : Instance {
void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "visuallooktransition");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "visuallooktransition");
}

visuallooktransition(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    
}

visuallooktransition(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    
}

visuallooktransition(const visuallooktransition &src) : Instance(src) {
    
}

visuallooktransition(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    
}

~visuallooktransition() {}

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
    return 0x0f409aa6;
}

const float &uves_pulse_length() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x0b82d479, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const Attrib::Types::Matrix &uves_extreme() const {
        const Attrib::Types::Matrix *resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(GetAttributePointer(0x0e629621, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(DefaultDataArea(sizeof(Attrib::Types::Matrix)));
        }
        return *resultptr;
    }
        
const Attrib::Types::Matrix &normal() const {
        const Attrib::Types::Matrix *resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(GetAttributePointer(0x0eb3d357, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(DefaultDataArea(sizeof(Attrib::Types::Matrix)));
        }
        return *resultptr;
    }
        
const Attrib::Types::Matrix &uves_pulse() const {
        const Attrib::Types::Matrix *resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(GetAttributePointer(0x334f1e4d, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(DefaultDataArea(sizeof(Attrib::Types::Matrix)));
        }
        return *resultptr;
    }
        
const float &_testvalue() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x4ef07d5d, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &uves_pulse_trigger() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x53769b6c, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const Attrib::Types::Matrix &uves() const {
        const Attrib::Types::Matrix *resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(GetAttributePointer(0x681bef75, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(DefaultDataArea(sizeof(Attrib::Types::Matrix)));
        }
        return *resultptr;
    }
        
};

} // namespace Gen
} // namespace Attrib

#endif
