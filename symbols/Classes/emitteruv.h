#ifndef ATTRIBSYS_CLASSES_EMITTERUV_H
#define ATTRIBSYS_CLASSES_EMITTERUV_H


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

struct emitteruv : Instance {
struct _LayoutStruct {
float EndV; // offset 0x0, size 0x4
float StartU; // offset 0x4, size 0x4
float EndU; // offset 0x8, size 0x4
float StartV; // offset 0xc, size 0x4
};

void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "emitteruv");
}

emitteruv(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

emitteruv(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

~emitteruv() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

static Key ClassKey() {
    return 0xe4983a7d;
}

const float &EndV() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->EndV;
}

const float &StartU() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->StartU;
}

const float &EndU() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->EndU;
}

const float &StartV() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->StartV;
}

};

} // namespace Gen
} // namespace Attrib

#endif
