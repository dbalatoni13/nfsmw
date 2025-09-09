#ifndef ATTRIBSYS_CLASSES_ENGINE_H
#define ATTRIBSYS_CLASSES_ENGINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct engine : Instance {
    struct _LayoutStruct {
        Private _Array_TORQUE;         // offset 0x0, size 0x8
        float TORQUE[9];               // offset 0x8, size 0x24
        Private _Array_SPEED_LIMITER;  // offset 0x2c, size 0x8
        float SPEED_LIMITER[2];        // offset 0x34, size 0x8
        Private _Array_ENGINE_BRAKING; // offset 0x3c, size 0x8
        float ENGINE_BRAKING[3];       // offset 0x44, size 0xc
        float FLYWHEEL_MASS;           // offset 0x50, size 0x4
        float MAX_RPM;                 // offset 0x54, size 0x4
        float RED_LINE;                // offset 0x58, size 0x4
        float IDLE;                    // offset 0x5c, size 0x4
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "engine");
    }

    engine(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~engine() {}

    const float &TORQUE(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_TORQUE.GetLength()) {
            return lp->TORQUE[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_TORQUE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_TORQUE.GetLength();
    }

    const float &SPEED_LIMITER(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_SPEED_LIMITER.GetLength()) {
            return lp->SPEED_LIMITER[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_SPEED_LIMITER() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_SPEED_LIMITER.GetLength();
    }

    const float &ENGINE_BRAKING(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_ENGINE_BRAKING.GetLength()) {
            return lp->ENGINE_BRAKING[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_ENGINE_BRAKING() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_ENGINE_BRAKING.GetLength();
    }

    const float &FLYWHEEL_MASS() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->FLYWHEEL_MASS;
    }

    const float &MAX_RPM() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MAX_RPM;
    }

    const float &RED_LINE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->RED_LINE;
    }

    const float &IDLE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->IDLE;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
