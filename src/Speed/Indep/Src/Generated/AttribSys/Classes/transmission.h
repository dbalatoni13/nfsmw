#ifndef ATTRIBSYS_CLASSES_TRANSMISSION_H
#define ATTRIBSYS_CLASSES_TRANSMISSION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct transmission : Instance {
    struct _LayoutStruct {
        Private _Array_GEAR_RATIO;      // offset 0x0, size 0x8
        float GEAR_RATIO[9];            // offset 0x8, size 0x24
        Private _Array_DIFFERENTIAL;    // offset 0x2c, size 0x8
        float DIFFERENTIAL[3];          // offset 0x34, size 0xc
        Private _Array_GEAR_EFFICIENCY; // offset 0x40, size 0x8
        float GEAR_EFFICIENCY[9];       // offset 0x48, size 0x24
        float TORQUE_CONVERTER;         // offset 0x6c, size 0x4
        float TORQUE_SPLIT;             // offset 0x70, size 0x4
        float CLUTCH_SLIP;              // offset 0x74, size 0x4
        float OPTIMAL_SHIFT;            // offset 0x78, size 0x4
        float SHIFT_SPEED;              // offset 0x7c, size 0x4
        float FINAL_GEAR;               // offset 0x80, size 0x4
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "transmission");
    }

    transmission(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~transmission() {}

    const float &GEAR_RATIO(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_GEAR_RATIO.GetLength()) {
            return lp->GEAR_RATIO[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_GEAR_RATIO() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_GEAR_RATIO.GetLength();
    }

    const float &DIFFERENTIAL(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_DIFFERENTIAL.GetLength()) {
            return lp->DIFFERENTIAL[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_DIFFERENTIAL() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_DIFFERENTIAL.GetLength();
    }

    const float &GEAR_EFFICIENCY(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_GEAR_EFFICIENCY.GetLength()) {
            return lp->GEAR_EFFICIENCY[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_GEAR_EFFICIENCY() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_GEAR_EFFICIENCY.GetLength();
    }

    const float &TORQUE_CONVERTER() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->TORQUE_CONVERTER;
    }

    const float &TORQUE_SPLIT() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->TORQUE_SPLIT;
    }

    const float &CLUTCH_SLIP() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CLUTCH_SLIP;
    }

    const float &OPTIMAL_SHIFT() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->OPTIMAL_SHIFT;
    }

    const float &SHIFT_SPEED() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SHIFT_SPEED;
    }

    const float &FINAL_GEAR() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->FINAL_GEAR;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
