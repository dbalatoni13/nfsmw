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

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "aud_moment_strm");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "aud_moment_strm");
}

aud_moment_strm(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

aud_moment_strm(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

aud_moment_strm(const aud_moment_strm &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

aud_moment_strm(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~aud_moment_strm() {}

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
    return 0xd2410816;
}

const bool &CanInterupt() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xb34fc11d, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &IsPositioned() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xc4e7f7c2, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const Csis::Type_SoundFX_Type &stream() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->stream;
}

const Csis::Type_SoundFX_Param &param() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->param;
}

const eVOL_MOMENT &VolSlot() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->VolSlot;
}

const char &strmpriority() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->strmpriority;
}

};

} // namespace Gen
} // namespace Attrib

#endif
