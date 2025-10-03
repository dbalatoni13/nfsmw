#ifndef ATTRIBSYS_CLASSES_EMITTERGROUP_H
#define ATTRIBSYS_CLASSES_EMITTERGROUP_H

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

struct emittergroup : Instance {
    struct _LayoutStruct {
        Private _Array_Emitters;           // offset 0x0, size 0x8
        RefSpec Emitters[6];               // offset 0x8, size 0x48
        Private _Array_IntensityRanges;    // offset 0x50, size 0x8
        UMath::Vector2 IntensityRanges[5]; // offset 0x58, size 0x28
        char CollectionName[4];            // offset 0x80, size 0x4
        float FarClip;                     // offset 0x84, size 0x4
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "emittergroup");
    }

    emittergroup(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    emittergroup(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~emittergroup() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0xaba86e60;
    }

    const RefSpec &Emitters(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_Emitters.GetLength()) {
            return lp->Emitters[index];
        } else {
            return *reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
    }

    unsigned int Num_Emitters() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_Emitters.GetLength();
    }

    const UMath::Vector2 &IntensityRanges(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_IntensityRanges.GetLength()) {
            return lp->IntensityRanges[index];
        } else {
            return *reinterpret_cast<const UMath::Vector2 *>(DefaultDataArea(sizeof(UMath::Vector2)));
        }
    }

    unsigned int Num_IntensityRanges() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_IntensityRanges.GetLength();
    }

    float &FarClip() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->FarClip;
    }
};
} // namespace Gen
} // namespace Attrib

#endif
