#ifndef ATTRIBSYS_CLASSES_MILESTONETYPES_H
#define ATTRIBSYS_CLASSES_MILESTONETYPES_H


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

struct milestonetypes : Instance {
struct _LayoutStruct {
const char *CollectionName; // offset 0x0, size 0x4
int MilestoneType; // offset 0x4, size 0x4
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "milestonetypes");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "milestonetypes");
}

milestonetypes(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

milestonetypes(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

milestonetypes(const milestonetypes &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

milestonetypes(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~milestonetypes() {}

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
    return 0xe4c3d904;
}

const char*CollectionName() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->CollectionName;
}

const bool &ResetWhenPursuitStarts() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xab4a196f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const int &LocalizationTag() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xdb89ab5c, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &MilestoneType() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MilestoneType;
}

};

} // namespace Gen
} // namespace Attrib

#endif
