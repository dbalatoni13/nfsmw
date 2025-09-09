#ifndef ATTRIBSYS_CLASSES_TIRES_H
#define ATTRIBSYS_CLASSES_TIRES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct tires : Instance {
    struct _LayoutStruct {
        Private _Array_YAW_CONTROL; // offset 0x0, size 0x8
        float YAW_CONTROL[4];       // offset 0x8, size 0x10
        AxlePair GRIP_SCALE;        // offset 0x18, size 0x8
        AxlePair DYNAMIC_GRIP;      // offset 0x20, size 0x8
        AxlePair ASPECT_RATIO;      // offset 0x28, size 0x8
        AxlePair RIM_SIZE;          // offset 0x30, size 0x8
        AxlePair STATIC_GRIP;       // offset 0x38, size 0x8
        AxlePair SECTION_WIDTH;     // offset 0x40, size 0x8
        float STEERING;             // offset 0x48, size 0x4
        float YAW_SPEED;            // offset 0x4c, size 0x4
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "tires");
    }

    tires(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~tires() {}

    const float &YAW_CONTROL(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_YAW_CONTROL.GetLength()) {
            return lp->YAW_CONTROL[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_YAW_CONTROL() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_YAW_CONTROL.GetLength();
    }

    const AxlePair &GRIP_SCALE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GRIP_SCALE;
    }

    const AxlePair &DYNAMIC_GRIP() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DYNAMIC_GRIP;
    }

    const AxlePair &ASPECT_RATIO() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->ASPECT_RATIO;
    }

    const AxlePair &RIM_SIZE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->RIM_SIZE;
    }

    const AxlePair &STATIC_GRIP() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->STATIC_GRIP;
    }

    const AxlePair &SECTION_WIDTH() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SECTION_WIDTH;
    }

    const float &STEERING() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->STEERING;
    }

    const float &YAW_SPEED() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->YAW_SPEED;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
