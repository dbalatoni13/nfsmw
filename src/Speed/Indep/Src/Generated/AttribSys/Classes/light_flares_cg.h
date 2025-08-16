#ifndef ATTRIBSYS_CLASSES_LIGHT_FLARES_CG_H
#define ATTRIBSYS_CLASSES_LIGHT_FLARES_CG_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace Attrib {
namespace Gen {

struct light_flares_cg : Instance {
    struct _LayoutStruct {
        UMath::Vector4 colour;      // offset 0x0, size 0x10
        float MinSize;              // offset 0x10, size 0x4
        float MinScale;             // offset 0x14, size 0x4
        float Power;                // offset 0x18, size 0x4
        unsigned int flare_texture; // offset 0x1c, size 0x4
        float Maxscale;             // offset 0x20, size 0x4
        float MaxSize;              // offset 0x24, size 0x4
        float ZBias;                // offset 0x28, size 0x4
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "light_flares_cg");
    }

    light_flares_cg(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~light_flares_cg() {}

    UMath::Vector4 &colour() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->colour;
    }

    float &MinSize() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MinSize;
    }

    float &MinScale() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MinScale;
    }

    float &Power() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Power;
    }

    unsigned int &flare_texture() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->flare_texture;
    }

    float &Maxscale() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Maxscale;
    }

    float &MaxSize() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MaxSize;
    }

    float &ZBias() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->ZBias;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
