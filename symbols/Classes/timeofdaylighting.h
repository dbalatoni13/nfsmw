#ifndef ATTRIBSYS_CLASSES_TIMEOFDAYLIGHTING_H
#define ATTRIBSYS_CLASSES_TIMEOFDAYLIGHTING_H


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

struct timeofdaylighting : Instance {
struct _LayoutStruct {
UMath::Vector4 SpecularColour; // offset 0x0, size 0x10
UMath::Vector4 DiffuseColour; // offset 0x10, size 0x10
UMath::Vector4 AmbientColour; // offset 0x20, size 0x10
UMath::Vector4 FogHazeColour; // offset 0x30, size 0x10
UMath::Vector4 FixedFunctionSkyColor; // offset 0x40, size 0x10
float FogDistanceScale; // offset 0x50, size 0x4
float FogHazeColourScale; // offset 0x54, size 0x4
float FogSkyColourScale; // offset 0x58, size 0x4
float EnvSkyBrightness; // offset 0x5c, size 0x4
float CarSpecScale; // offset 0x60, size 0x4
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "timeofdaylighting");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "timeofdaylighting");
}

timeofdaylighting(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

timeofdaylighting(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

timeofdaylighting(const timeofdaylighting &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

timeofdaylighting(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~timeofdaylighting() {}

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
    return 0x399ed882;
}

const float &FogInLightScatter() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x00756239, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const UMath::Vector4 &FogSkyColour() const {
        const UMath::Vector4 *resultptr = reinterpret_cast<const UMath::Vector4 *>(GetAttributePointer(0x1d76ea96, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector4 *>(DefaultDataArea(sizeof(UMath::Vector4)));
        }
        return *resultptr;
    }
        
const float &FogSunFalloff() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x2ef8a8bf, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const UMath::Vector4 &SpecularColour() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SpecularColour;
}

const UMath::Vector4 &DiffuseColour() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->DiffuseColour;
}

const UMath::Vector4 &AmbientColour() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->AmbientColour;
}

const UMath::Vector4 &FogHazeColour() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->FogHazeColour;
}

const UMath::Vector4 &FixedFunctionSkyColor() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->FixedFunctionSkyColor;
}

const float &FogDistanceScale() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->FogDistanceScale;
}

const float &FogHazeColourScale() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->FogHazeColourScale;
}

const float &FogSkyColourScale() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->FogSkyColourScale;
}

const float &EnvSkyBrightness() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->EnvSkyBrightness;
}

const float &CarSpecScale() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->CarSpecScale;
}

};

} // namespace Gen
} // namespace Attrib

#endif
