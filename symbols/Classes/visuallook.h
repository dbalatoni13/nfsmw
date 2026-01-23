#ifndef ATTRIBSYS_CLASSES_VISUALLOOK_H
#define ATTRIBSYS_CLASSES_VISUALLOOK_H


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

struct visuallook : Instance {
struct _LayoutStruct {
Attrib::Types::Matrix DetailMapCurve; // offset 0x0, size 0x40
Attrib::Types::Matrix BlackBloomCurve; // offset 0x40, size 0x40
Attrib::Types::Matrix ColourBloomCurve; // offset 0x80, size 0x40
UMath::Vector4 ColourBloomTint; // offset 0xc0, size 0x10
float ColourBloomIntensity; // offset 0xd0, size 0x4
float Desaturation; // offset 0xd4, size 0x4
float DetailMapIntensity; // offset 0xd8, size 0x4
float BlackBloomIntensity; // offset 0xdc, size 0x4
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "visuallook");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "visuallook");
}

visuallook(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

visuallook(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

visuallook(const visuallook &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

visuallook(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~visuallook() {}

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
    return 0x339f7d3d;
}

const Attrib::Types::Matrix &DetailMapCurve() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->DetailMapCurve;
}

const Attrib::Types::Matrix &BlackBloomCurve() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->BlackBloomCurve;
}

const Attrib::Types::Matrix &ColourBloomCurve() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->ColourBloomCurve;
}

const UMath::Vector4 &ColourBloomTint() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->ColourBloomTint;
}

const float &ColourBloomIntensity() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->ColourBloomIntensity;
}

const float &Desaturation() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Desaturation;
}

const float &DetailMapIntensity() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->DetailMapIntensity;
}

const float &BlackBloomIntensity() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->BlackBloomIntensity;
}

};

} // namespace Gen
} // namespace Attrib

#endif
