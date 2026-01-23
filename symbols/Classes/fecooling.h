#ifndef ATTRIBSYS_CLASSES_FECOOLING_H
#define ATTRIBSYS_CLASSES_FECOOLING_H


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

struct fecooling : Instance {
void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "fecooling");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "fecooling");
}

fecooling(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    
}

fecooling(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    
}

fecooling(const fecooling &src) : Instance(src) {
    
}

fecooling(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    
}

~fecooling() {}

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
    return 0x5d417978;
}

const float &NewPaint() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x072ca287, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &NewDecal() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x1f71a77c, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &NewRim() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x4192719a, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &NewWindowTint() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x550807a7, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &NewVinyl() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x55b9418b, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &NewHood() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x678a0c91, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &NewRimPaint() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xa27de5ff, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &NewNumbers() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xa9ef0056, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &NewBodyKit() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xe52d0fb6, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &NewSpoiler() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xe5d29656, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &NewRoofScoop() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xf8be97ef, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
};

} // namespace Gen
} // namespace Attrib

#endif
