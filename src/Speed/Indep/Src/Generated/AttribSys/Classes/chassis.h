#ifndef ATTRIBSYS_CLASSES_CHASSIS_H
#define ATTRIBSYS_CLASSES_CHASSIS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace Attrib {
namespace Gen {

struct chassis : Instance {
    struct _LayoutStruct {
        AxlePair SHOCK_DIGRESSION;    // offset 0x0, size 0x8
        AxlePair SPRING_PROGRESSION;  // offset 0x8, size 0x8
        AxlePair TRAVEL;              // offset 0x10, size 0x8
        AxlePair RIDE_HEIGHT;         // offset 0x18, size 0x8
        AxlePair TRACK_WIDTH;         // offset 0x20, size 0x8
        AxlePair SHOCK_EXT_STIFFNESS; // offset 0x28, size 0x8
        AxlePair SHOCK_STIFFNESS;     // offset 0x30, size 0x8
        AxlePair SPRING_STIFFNESS;    // offset 0x38, size 0x8
        AxlePair SHOCK_VALVING;       // offset 0x40, size 0x8
        AxlePair SWAYBAR_STIFFNESS;   // offset 0x48, size 0x8
        float ROLL_CENTER;            // offset 0x50, size 0x4
        float WHEEL_BASE;             // offset 0x54, size 0x4
        float SHOCK_BLOWOUT;          // offset 0x58, size 0x4
        float AERO_CG;                // offset 0x5c, size 0x4
        float RENDER_MOTION;          // offset 0x60, size 0x4
        float FRONT_AXLE;             // offset 0x64, size 0x4
        float AERO_COEFFICIENT;       // offset 0x68, size 0x4
        float FRONT_WEIGHT_BIAS;      // offset 0x6c, size 0x4
        float DRAG_COEFFICIENT;       // offset 0x70, size 0x4
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "chassis");
    }

    chassis(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    chassis(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~chassis() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0xafa210f0;
    }

    const AxlePair &SHOCK_DIGRESSION() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SHOCK_DIGRESSION;
    }

    const AxlePair &SPRING_PROGRESSION() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SPRING_PROGRESSION;
    }

    const AxlePair &TRAVEL() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->TRAVEL;
    }

    const AxlePair &RIDE_HEIGHT() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->RIDE_HEIGHT;
    }

    const AxlePair &TRACK_WIDTH() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->TRACK_WIDTH;
    }

    const AxlePair &SHOCK_EXT_STIFFNESS() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SHOCK_EXT_STIFFNESS;
    }

    const AxlePair &SHOCK_STIFFNESS() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SHOCK_STIFFNESS;
    }

    const AxlePair &SPRING_STIFFNESS() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SPRING_STIFFNESS;
    }

    const AxlePair &SHOCK_VALVING() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SHOCK_VALVING;
    }

    const AxlePair &SWAYBAR_STIFFNESS() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SWAYBAR_STIFFNESS;
    }

    const float &ROLL_CENTER() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->ROLL_CENTER;
    }

    const float &WHEEL_BASE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->WHEEL_BASE;
    }

    const float &SHOCK_BLOWOUT() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SHOCK_BLOWOUT;
    }

    const float &AERO_CG() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AERO_CG;
    }

    const float &RENDER_MOTION() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->RENDER_MOTION;
    }

    const float &FRONT_AXLE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->FRONT_AXLE;
    }

    const float &AERO_COEFFICIENT() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AERO_COEFFICIENT;
    }

    const float &FRONT_WEIGHT_BIAS() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->FRONT_WEIGHT_BIAS;
    }

    const float &DRAG_COEFFICIENT() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DRAG_COEFFICIENT;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
