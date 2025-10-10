#ifndef ATTRIBSYS_CLASSES_DAMAGESPECS_H
#define ATTRIBSYS_CLASSES_DAMAGESPECS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

// TODO later put such structs into MWAttribUserTypes.h
struct DamageScaleRecord {
    // total size: 0x8
    float VisualScale;   // offset 0x0, size 0x4
    float HitPointScale; // offset 0x4, size 0x4
};

namespace Attrib {
namespace Gen {

struct damagespecs : Instance {
    struct _LayoutStruct {
        DamageScaleRecord DZ_LFRONT; // offset 0x0, size 0x8
        DamageScaleRecord DZ_FRONT;  // offset 0x8, size 0x8
        DamageScaleRecord DZ_BOTTOM; // offset 0x10, size 0x8
        DamageScaleRecord DZ_LEFT;   // offset 0x18, size 0x8
        DamageScaleRecord DZ_RREAR;  // offset 0x20, size 0x8
        DamageScaleRecord DZ_LREAR;  // offset 0x28, size 0x8
        DamageScaleRecord DZ_RIGHT;  // offset 0x30, size 0x8
        DamageScaleRecord DZ_REAR;   // offset 0x38, size 0x8
        DamageScaleRecord DZ_TOP;    // offset 0x40, size 0x8
        DamageScaleRecord DZ_RFRONT; // offset 0x48, size 0x8
        float SHOCK_TIME;            // offset 0x50, size 0x4
        float HP_THRESHOLD;          // offset 0x54, size 0x4
        float SUPPRESS_DIST;         // offset 0x58, size 0x4
        float SHOCK_FORCE;           // offset 0x5c, size 0x4
        float FORCE;                 // offset 0x60, size 0x4
        float HIT_POINTS;            // offset 0x64, size 0x4
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "damagespecs");
    }

    damagespecs(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    damagespecs(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~damagespecs() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0xc1f0b434;
    }

    const DamageScaleRecord &DZ_LFRONT() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DZ_LFRONT;
    }

    const DamageScaleRecord &DZ_FRONT() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DZ_FRONT;
    }

    const DamageScaleRecord &DZ_BOTTOM() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DZ_BOTTOM;
    }

    const DamageScaleRecord &DZ_LEFT() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DZ_LEFT;
    }

    const DamageScaleRecord &DZ_RREAR() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DZ_RREAR;
    }

    const DamageScaleRecord &DZ_LREAR() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DZ_LREAR;
    }

    const DamageScaleRecord &DZ_RIGHT() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DZ_RIGHT;
    }

    const DamageScaleRecord &DZ_REAR() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DZ_REAR;
    }

    const DamageScaleRecord &DZ_TOP() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DZ_TOP;
    }

    const DamageScaleRecord &DZ_RFRONT() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DZ_RFRONT;
    }

    const float &SHOCK_TIME() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SHOCK_TIME;
    }

    const float &HP_THRESHOLD() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->HP_THRESHOLD;
    }

    const float &SUPPRESS_DIST() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SUPPRESS_DIST;
    }

    const float &SHOCK_FORCE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SHOCK_FORCE;
    }

    const float &FORCE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->FORCE;
    }

    const float &HIT_POINTS() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->HIT_POINTS;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
