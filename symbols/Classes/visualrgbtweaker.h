#ifndef ATTRIBSYS_CLASSES_VISUALRGBTWEAKER_H
#define ATTRIBSYS_CLASSES_VISUALRGBTWEAKER_H


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

struct visualrgbtweaker : Instance {
struct _LayoutStruct {
};

void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "visualrgbtweaker");
}

visualrgbtweaker(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

visualrgbtweaker(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

~visualrgbtweaker() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

static Key ClassKey() {
    return 0xaf1837ca;
}

const Attrib::Types::Matrix &blue(unsigned int index) const {
        const Attrib::Types::Matrix *resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(this->GetAttributePointer(0x1c641189, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(DefaultDataArea(sizeof(Attrib::Types::Matrix)));
        }
        return *resultptr;
    }
        
const Attrib::Types::Matrix &green(unsigned int index) const {
        const Attrib::Types::Matrix *resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(this->GetAttributePointer(0x3ffd271c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(DefaultDataArea(sizeof(Attrib::Types::Matrix)));
        }
        return *resultptr;
    }
        
const Attrib::Types::Matrix &red(unsigned int index) const {
        const Attrib::Types::Matrix *resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(this->GetAttributePointer(0x5cd90211, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(DefaultDataArea(sizeof(Attrib::Types::Matrix)));
        }
        return *resultptr;
    }
        
};

} // namespace Gen
} // namespace Attrib

#endif
