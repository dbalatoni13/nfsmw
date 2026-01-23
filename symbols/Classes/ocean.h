#ifndef ATTRIBSYS_CLASSES_OCEAN_H
#define ATTRIBSYS_CLASSES_OCEAN_H


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

struct ocean : Instance {
void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "ocean");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "ocean");
}

ocean(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    
}

ocean(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    
}

ocean(const ocean &src) : Instance(src) {
    
}

ocean(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    
}

~ocean() {}

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
    return 0x093d7c56;
}

const UMath::Vector4 &layer2_specular_colour() const {
        const UMath::Vector4 *resultptr = reinterpret_cast<const UMath::Vector4 *>(GetAttributePointer(0x19be3d84, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector4 *>(DefaultDataArea(sizeof(UMath::Vector4)));
        }
        return *resultptr;
    }
        
const UMath::Vector2 &layer1_tile_xy() const {
        const UMath::Vector2 *resultptr = reinterpret_cast<const UMath::Vector2 *>(GetAttributePointer(0x270e8256, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector2 *>(DefaultDataArea(sizeof(UMath::Vector2)));
        }
        return *resultptr;
    }
        
const UMath::Vector2 &layer2_tile_xy() const {
        const UMath::Vector2 *resultptr = reinterpret_cast<const UMath::Vector2 *>(GetAttributePointer(0x8813b330, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector2 *>(DefaultDataArea(sizeof(UMath::Vector2)));
        }
        return *resultptr;
    }
        
const UMath::Vector4 &layer1_specular_colour() const {
        const UMath::Vector4 *resultptr = reinterpret_cast<const UMath::Vector4 *>(GetAttributePointer(0xd9e53379, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector4 *>(DefaultDataArea(sizeof(UMath::Vector4)));
        }
        return *resultptr;
    }
        
const float &reflection_amount() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xe956e716, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
};

} // namespace Gen
} // namespace Attrib

#endif
