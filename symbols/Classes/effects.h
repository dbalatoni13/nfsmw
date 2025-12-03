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
char CollectionName[4]; // offset 0xc, size 0x4
};

void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "effects");
}

effects(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

effects(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

~effects() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

static Key ClassKey() {
    return 0xebcee74c;
}

const float &InheritVelocity(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x0099cb26, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const bool &FogEnable(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x110882d5, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const UMath::Vector4 &AudioQuadratic(unsigned int index) const {
        const UMath::Vector4 *resultptr = reinterpret_cast<const UMath::Vector4 *>(this->GetAttributePointer(0x15e6552f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector4 *>(DefaultDataArea(sizeof(UMath::Vector4)));
        }
        return *resultptr;
    }
        
const float &VisualCullDist(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x1a2f2b1b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const RefSpec &AudioFX_DEFAULT() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AudioFX_DEFAULT;
}

const UMath::Vector4 &ConstantBm(unsigned int index) const {
        const UMath::Vector4 *resultptr = reinterpret_cast<const UMath::Vector4 *>(this->GetAttributePointer(0x2cf0b378, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector4 *>(DefaultDataArea(sizeof(UMath::Vector4)));
        }
        return *resultptr;
    }
        
const float &InScatterMulitply(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x2f70d78a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const RefSpec &AudioFX_SIDE(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x4c64ed7b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const RefSpec &AudioFX_FRONT(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x53de9f00, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const float &BrMultiply(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x555fd699, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const RefSpec &AudioFX_TOP(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x74ef4fc8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const RefSpec &AudioFX_REAR(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x7bc5f444, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const float &AudioCullDist(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x98dba438, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const UMath::Vector4 &ConstantBr(unsigned int index) const {
        const UMath::Vector4 *resultptr = reinterpret_cast<const UMath::Vector4 *>(this->GetAttributePointer(0xa450f56c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector4 *>(DefaultDataArea(sizeof(UMath::Vector4)));
        }
        return *resultptr;
    }
        
const UMath::Vector4 &EmitterQuadratic(unsigned int index) const {
        const UMath::Vector4 *resultptr = reinterpret_cast<const UMath::Vector4 *>(this->GetAttributePointer(0xa9402c33, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector4 *>(DefaultDataArea(sizeof(UMath::Vector4)));
        }
        return *resultptr;
    }
        
const RefSpec &emittergroup(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0xaba86e60, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const float &BmMultiply(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xc8baf5d6, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &FogG(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xdba22a95, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const RefSpec &AudioFX_BOTTOM(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0xe82f96cf, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const char*CollectionName() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
}

};

} // namespace Gen
} // namespace Attrib

#endif
