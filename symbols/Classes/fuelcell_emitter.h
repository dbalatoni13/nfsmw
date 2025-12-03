#ifndef ATTRIBSYS_CLASSES_FUELCELL_EMITTER_H
#define ATTRIBSYS_CLASSES_FUELCELL_EMITTER_H


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

struct fuelcell_emitter : Instance {
struct _LayoutStruct {
UMath::Vector4 VolumeCenter; // offset 0x0, size 0x10
UMath::Vector4 VelocityDelta; // offset 0x10, size 0x10
UMath::Vector4 VolumeExtent; // offset 0x20, size 0x10
UMath::Vector4 VelocityInherit; // offset 0x30, size 0x10
UMath::Vector4 VelocityStart; // offset 0x40, size 0x10
UMath::Vector4 Colour1; // offset 0x50, size 0x10
RefSpec emitteruv; // offset 0x60, size 0xc
float Life; // offset 0x6c, size 0x4
float NumParticlesVariance; // offset 0x70, size 0x4
float GravityStart; // offset 0x74, size 0x4
float HeightStart; // offset 0x78, size 0x4
float GravityDelta; // offset 0x7c, size 0x4
float LengthStart; // offset 0x80, size 0x4
float LengthDelta; // offset 0x84, size 0x4
float LifeVariance; // offset 0x88, size 0x4
float NumParticles; // offset 0x8c, size 0x4
short Spin; // offset 0x90, size 0x2
char zSprite; // offset 0x92, size 0x1
char zContrail; // offset 0x93, size 0x1
};

void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "fuelcell_emitter");
}

fuelcell_emitter(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

fuelcell_emitter(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

~fuelcell_emitter() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

static Key ClassKey() {
    return 0xb267a856;
}

const UMath::Vector4 &VolumeCenter() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->VolumeCenter;
}

const UMath::Vector4 &VelocityDelta() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->VelocityDelta;
}

const UMath::Vector4 &VolumeExtent() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->VolumeExtent;
}

const UMath::Vector4 &VelocityInherit() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->VelocityInherit;
}

const UMath::Vector4 &VelocityStart() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->VelocityStart;
}

const UMath::Vector4 &Colour1() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Colour1;
}

const RefSpec &emitteruv() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->emitteruv;
}

const float &Life() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Life;
}

const float &NumParticlesVariance() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->NumParticlesVariance;
}

const float &GravityStart() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GravityStart;
}

const float &HeightStart() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->HeightStart;
}

const float &GravityDelta() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GravityDelta;
}

const float &LengthStart() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->LengthStart;
}

const float &LengthDelta() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->LengthDelta;
}

const float &LifeVariance() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->LifeVariance;
}

const float &NumParticles() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->NumParticles;
}

const short &Spin() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Spin;
}

const char &zSprite() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->zSprite;
}

const char &zContrail() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->zContrail;
}

};

} // namespace Gen
} // namespace Attrib

#endif
