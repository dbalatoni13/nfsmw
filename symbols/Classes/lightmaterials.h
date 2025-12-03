#ifndef ATTRIBSYS_CLASSES_LIGHTMATERIALS_H
#define ATTRIBSYS_CLASSES_LIGHTMATERIALS_H


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

struct lightmaterials : Instance {
struct _LayoutStruct {
UMath::Vector4 GrassColour; // offset 0x0, size 0x10
UMath::Vector4 diffuseColour; // offset 0x10, size 0x10
UMath::Vector4 specularColour; // offset 0x20, size 0x10
Attrib::StringKey shader; // offset 0x30, size 0x10
float GrassGamma; // offset 0x40, size 0x4
float specularFacing; // offset 0x44, size 0x4
float LODRamp; // offset 0x48, size 0x4
float Scruff; // offset 0x4c, size 0x4
float LowNoiseSpace; // offset 0x50, size 0x4
float NoiseMipMapBias; // offset 0x54, size 0x4
float LODStart; // offset 0x58, size 0x4
float diffuseFacing; // offset 0x5c, size 0x4
float LowNoiseIntensity; // offset 0x60, size 0x4
float MaxShells; // offset 0x64, size 0x4
float specularPower; // offset 0x68, size 0x4
float DiffuseMipMapBias; // offset 0x6c, size 0x4
float GrassHeight; // offset 0x70, size 0x4
float diffusePower; // offset 0x74, size 0x4
float DiffuseSpace; // offset 0x78, size 0x4
float specularGrazing; // offset 0x7c, size 0x4
float diffuseGrazing; // offset 0x80, size 0x4
float parallaxHeight; // offset 0x84, size 0x4
unsigned char PunchThroughAlphaRef; // offset 0x88, size 0x1
};

void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "lightmaterials");
}

lightmaterials(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

lightmaterials(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

~lightmaterials() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

static Key ClassKey() {
    return 0xd32a743f;
}

const Attrib::StringKey &opacityMap(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0x15fa3308, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &refractionMap(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0x1dafcfc0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &specularColourMap(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0x2569fd5b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const bool &useVertexColour(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x2de05384, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &NoiseSpace(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x3a417eae, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &Brightness(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x42ff3188, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &Ambient(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x483b6e25, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &glossMap(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0x5439f9c7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &reflectionMap(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0x95c10731, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const float &Smoothness(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xb94011a2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const RefSpec &shaderspec(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0xcb31c94d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const float &LightingCone(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xe0985f58, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &illuminationMap(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0xf3d46f50, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &filterColourMap(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0xfd165dc3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const UMath::Vector4 &GrassColour() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GrassColour;
}

const UMath::Vector4 &diffuseColour() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->diffuseColour;
}

const UMath::Vector4 &specularColour() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->specularColour;
}

const Attrib::StringKey &shader() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->shader;
}

const float &GrassGamma() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GrassGamma;
}

const float &specularFacing() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->specularFacing;
}

const float &LODRamp() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->LODRamp;
}

const float &Scruff() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Scruff;
}

const float &LowNoiseSpace() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->LowNoiseSpace;
}

const float &NoiseMipMapBias() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->NoiseMipMapBias;
}

const float &LODStart() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->LODStart;
}

const float &diffuseFacing() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->diffuseFacing;
}

const float &LowNoiseIntensity() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->LowNoiseIntensity;
}

const float &MaxShells() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MaxShells;
}

const float &specularPower() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->specularPower;
}

const float &DiffuseMipMapBias() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DiffuseMipMapBias;
}

const float &GrassHeight() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GrassHeight;
}

const float &diffusePower() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->diffusePower;
}

const float &DiffuseSpace() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DiffuseSpace;
}

const float &specularGrazing() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->specularGrazing;
}

const float &diffuseGrazing() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->diffuseGrazing;
}

const float &parallaxHeight() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->parallaxHeight;
}

const unsigned char &PunchThroughAlphaRef() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->PunchThroughAlphaRef;
}

};

} // namespace Gen
} // namespace Attrib

#endif
