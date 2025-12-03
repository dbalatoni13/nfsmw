#ifndef ATTRIBSYS_CLASSES_ACCELTRANS_H
#define ATTRIBSYS_CLASSES_ACCELTRANS_H


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

struct acceltrans : Instance {
struct _LayoutStruct {
unsigned int AccelFromIdle_PEAK_T; // offset 0x0, size 0x4
unsigned int AccelFromIdle_RESUME_T; // offset 0x4, size 0x4
unsigned int AccelFromIdle_PEAK_RPM; // offset 0x8, size 0x4
float AccelFromIdle_PEAK_VOL; // offset 0xc, size 0x4
};

void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "acceltrans");
}

acceltrans(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

acceltrans(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

~acceltrans() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

static Key ClassKey() {
    return 0xff77f451;
}

const unsigned int &AccelFromIdle_INTERUPT_T(unsigned int index) const {
        const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(this->GetAttributePointer(0x49fb8ce5, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
        }
        return *resultptr;
    }
        
const unsigned int &AccelFromIdle_PEAK_T() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AccelFromIdle_PEAK_T;
}

const unsigned int &AccelFromIdle_RESUME_T() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AccelFromIdle_RESUME_T;
}

const unsigned int &AccelFromIdle_PEAK_RPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AccelFromIdle_PEAK_RPM;
}

const float &AccelFromIdle_PEAK_VOL() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AccelFromIdle_PEAK_VOL;
}

};

} // namespace Gen
} // namespace Attrib

#endif
