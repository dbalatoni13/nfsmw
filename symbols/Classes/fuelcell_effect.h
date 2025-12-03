#ifndef ATTRIBSYS_CLASSES_FUELCELL_EFFECT_H
#define ATTRIBSYS_CLASSES_FUELCELL_EFFECT_H


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

struct fuelcell_effect : Instance {
struct _LayoutStruct {
bool doTest; // offset 0x0, size 0x1
};

void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "fuelcell_effect");
}

fuelcell_effect(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

fuelcell_effect(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

~fuelcell_effect() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

static Key ClassKey() {
    return 0x6f5943f1;
}

const bool &doTest() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->doTest;
}

const RefSpec &NGEmitter(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0xb0d98a89, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
unsigned int Num_NGEmitter() const {
            return this->Get(0xb0d98a89).GetLength();
        }

};

} // namespace Gen
} // namespace Attrib

#endif
