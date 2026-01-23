#ifndef ATTRIBSYS_CLASSES_EFFECTS_H
#define ATTRIBSYS_CLASSES_EFFECTS_H


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

struct effects : Instance {
struct _LayoutStruct {
RefSpec AudioFX_DEFAULT; // offset 0x0, size 0xc
const char *CollectionName; // offset 0xc, size 0x4
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "effects");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "effects");
}

effects(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

effects(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

effects(const effects &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

effects(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~effects() {}

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
    return 0xebcee74c;
}

const float &InheritVelocity() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x0099cb26, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const bool &FogEnable() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x110882d5, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const UMath::Vector4 &AudioQuadratic() const {
        const UMath::Vector4 *resultptr = reinterpret_cast<const UMath::Vector4 *>(GetAttributePointer(0x15e6552f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector4 *>(DefaultDataArea(sizeof(UMath::Vector4)));
        }
        return *resultptr;
    }
        
const float &VisualCullDist() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x1a2f2b1b, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const RefSpec &AudioFX_DEFAULT() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->AudioFX_DEFAULT;
}

const UMath::Vector4 &ConstantBm() const {
        const UMath::Vector4 *resultptr = reinterpret_cast<const UMath::Vector4 *>(GetAttributePointer(0x2cf0b378, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector4 *>(DefaultDataArea(sizeof(UMath::Vector4)));
        }
        return *resultptr;
    }
        
const float &InScatterMulitply() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x2f70d78a, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const RefSpec &AudioFX_SIDE() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x4c64ed7b, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const RefSpec &AudioFX_FRONT() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x53de9f00, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const float &BrMultiply() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x555fd699, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const RefSpec &AudioFX_TOP() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x74ef4fc8, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const RefSpec &AudioFX_REAR() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x7bc5f444, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const float &AudioCullDist() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x98dba438, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const UMath::Vector4 &ConstantBr() const {
        const UMath::Vector4 *resultptr = reinterpret_cast<const UMath::Vector4 *>(GetAttributePointer(0xa450f56c, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector4 *>(DefaultDataArea(sizeof(UMath::Vector4)));
        }
        return *resultptr;
    }
        
const UMath::Vector4 &EmitterQuadratic() const {
        const UMath::Vector4 *resultptr = reinterpret_cast<const UMath::Vector4 *>(GetAttributePointer(0xa9402c33, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector4 *>(DefaultDataArea(sizeof(UMath::Vector4)));
        }
        return *resultptr;
    }
        
const RefSpec &emittergroup() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xaba86e60, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const float &BmMultiply() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xc8baf5d6, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &FogG() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xdba22a95, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const RefSpec &AudioFX_BOTTOM() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xe82f96cf, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const char*CollectionName() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->CollectionName;
}

};

} // namespace Gen
} // namespace Attrib

#endif
