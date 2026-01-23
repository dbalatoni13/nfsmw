#ifndef ATTRIBSYS_CLASSES_INFRACTIONS_H
#define ATTRIBSYS_CLASSES_INFRACTIONS_H


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

struct infractions : Instance {
struct _LayoutStruct {
unsigned int amount; // offset 0x0, size 0x4
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "infractions");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "infractions");
}

infractions(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

infractions(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

infractions(const infractions &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

infractions(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~infractions() {}

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
    return 0x2870de90;
}

const unsigned int &amount() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->amount;
}

};

} // namespace Gen
} // namespace Attrib

#endif
