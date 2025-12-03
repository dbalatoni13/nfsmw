#ifndef ATTRIBSYS_CLASSES_AUD_MOMENT_STRM_H
#define ATTRIBSYS_CLASSES_AUD_MOMENT_STRM_H


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

struct aud_moment_strm : Instance {
struct _LayoutStruct {
Csis::Type_SoundFX_Type stream; // offset 0x0, size 0x4
Csis::Type_SoundFX_Param param; // offset 0x4, size 0x4
eVOL_MOMENT VolSlot; // offset 0x8, size 0x4
char strmpriority; // offset 0xc, size 0x1
};

void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "aud_moment_strm");
}

aud_moment_strm(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

aud_moment_strm(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

~aud_moment_strm() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

static Key ClassKey() {
    return 0xd2410816;
}

const bool &CanInterupt(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xb34fc11d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &IsPositioned(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xc4e7f7c2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const Csis::Type_SoundFX_Type &stream() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->stream;
}

const Csis::Type_SoundFX_Param &param() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->param;
}

const eVOL_MOMENT &VolSlot() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->VolSlot;
}

const char &strmpriority() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->strmpriority;
}

};

} // namespace Gen
} // namespace Attrib

#endif
