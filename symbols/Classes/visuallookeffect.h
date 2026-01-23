#ifndef ATTRIBSYS_CLASSES_VISUALLOOKEFFECT_H
#define ATTRIBSYS_CLASSES_VISUALLOOKEFFECT_H


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

struct visuallookeffect : Instance {
struct _LayoutStruct {
Attrib::Types::Matrix graph; // offset 0x0, size 0x40
float _testvalue; // offset 0x40, size 0x4
float length; // offset 0x44, size 0x4
float magnitude; // offset 0x48, size 0x4
float heattrigger; // offset 0x4c, size 0x4
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "visuallookeffect");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "visuallookeffect");
}

visuallookeffect(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

visuallookeffect(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

visuallookeffect(const visuallookeffect &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

visuallookeffect(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~visuallookeffect() {}

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
    return 0x6ab2d241;
}

const Attrib::Types::Matrix &graph() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->graph;
}

const float &radialblur_uvoffset() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x8bc39288, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &radialblur_scale() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xd0b003f4, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &_testvalue() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->_testvalue;
}

const float &length() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->length;
}

const float &magnitude() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->magnitude;
}

const float &heattrigger() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->heattrigger;
}

};

} // namespace Gen
} // namespace Attrib

#endif
