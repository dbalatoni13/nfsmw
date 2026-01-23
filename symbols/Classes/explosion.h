#ifndef ATTRIBSYS_CLASSES_EXPLOSION_H
#define ATTRIBSYS_CLASSES_EXPLOSION_H


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

struct explosion : Instance {
struct _LayoutStruct {
RefSpec BaseMaterial; // offset 0x0, size 0xc
float triggerThreshold; // offset 0xc, size 0x4
float explosionForceLimit; // offset 0x10, size 0x4
float fallOffUnit; // offset 0x14, size 0x4
float forceMultiplier; // offset 0x18, size 0x4
float damageMultiplier; // offset 0x1c, size 0x4
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "explosion");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "explosion");
}

explosion(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

explosion(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

explosion(const explosion &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

explosion(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~explosion() {}

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
    return 0x6434f1fb;
}

const RefSpec &BaseMaterial() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->BaseMaterial;
}

const float &triggerThreshold() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->triggerThreshold;
}

const float &explosionForceLimit() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->explosionForceLimit;
}

const float &fallOffUnit() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->fallOffUnit;
}

const float &forceMultiplier() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->forceMultiplier;
}

const float &damageMultiplier() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->damageMultiplier;
}

};

} // namespace Gen
} // namespace Attrib

#endif
