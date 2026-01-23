#ifndef ATTRIBSYS_CLASSES_RIGIDBODYSPECS_H
#define ATTRIBSYS_CLASSES_RIGIDBODYSPECS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct rigidbodyspecs : Instance {
    struct _LayoutStruct {
        UMath::Vector4 COLLISION_BOX_PAD;   // offset 0x0, size 0x10
        UMath::Vector4 DRAG;                // offset 0x10, size 0x10
        UMath::Vector4 WORLD_MOMENT_SCALE;  // offset 0x20, size 0x10
        UMath::Vector4 OBJ_MOMENT_SCALE;    // offset 0x30, size 0x10
        UMath::Vector4 GROUND_ELASTICITY;   // offset 0x40, size 0x10
        UMath::Vector4 OBJ_ELASTICITY;      // offset 0x50, size 0x10
        UMath::Vector4 DRAG_ANGULAR;        // offset 0x60, size 0x10
        UMath::Vector4 WALL_ELASTICITY;     // offset 0x70, size 0x10
        UMath::Vector4 GROUND_MOMENT_SCALE; // offset 0x80, size 0x10
        UMath::Vector4 CG;                  // offset 0x90, size 0x10
        Attrib::StringKey BASE_MATERIAL;    // offset 0xa0, size 0x10
        Attrib::StringKey DEFAULT_COL_BOX;  // offset 0xb0, size 0x10
        Private _Array_OBJ_FRICTION;        // offset 0xc0, size 0x8
        float OBJ_FRICTION[2];              // offset 0xc8, size 0x8
        Private _Array_WALL_FRICTION;       // offset 0xd0, size 0x8
        float WALL_FRICTION[2];             // offset 0xd8, size 0x8
        Private _Array_GROUND_FRICTION;     // offset 0xe0, size 0x8
        float GROUND_FRICTION[2];           // offset 0xe8, size 0x8
        float GRAVITY;                      // offset 0xf0, size 0x4
        float NATURAL_ANGULAR_DAMPING;      // offset 0xf4, size 0x4
        float SLEEP_VELOCITY;               // offset 0xf8, size 0x4
        bool NO_GROUND_COLLISIONS;          // offset 0xfc, size 0x1
        bool IMMOBILE_OBJECT_COLLISIONS;    // offset 0xfd, size 0x1
        bool NO_WORLD_COLLISIONS;           // offset 0xfe, size 0x1
        bool INSTANCE_COLLISIONS_3D;        // offset 0xff, size 0x1
        bool NO_OBJ_COLLISIONS;             // offset 0x100, size 0x1
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "rigidbodyspecs");
    }

    rigidbodyspecs(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    rigidbodyspecs(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    rigidbodyspecs(const rigidbodyspecs &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    rigidbodyspecs(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~rigidbodyspecs() {}

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
        return 0x7c90bb38;
    }

    const UMath::Vector4 &COLLISION_BOX_PAD() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->COLLISION_BOX_PAD;
    }

    const UMath::Vector4 &DRAG() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DRAG;
    }

    const UMath::Vector4 &WORLD_MOMENT_SCALE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->WORLD_MOMENT_SCALE;
    }

    const UMath::Vector4 &OBJ_MOMENT_SCALE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->OBJ_MOMENT_SCALE;
    }

    const UMath::Vector4 &GROUND_ELASTICITY() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GROUND_ELASTICITY;
    }

    const UMath::Vector4 &OBJ_ELASTICITY() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->OBJ_ELASTICITY;
    }

    const UMath::Vector4 &DRAG_ANGULAR() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DRAG_ANGULAR;
    }

    const UMath::Vector4 &WALL_ELASTICITY() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->WALL_ELASTICITY;
    }

    const UMath::Vector4 &GROUND_MOMENT_SCALE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GROUND_MOMENT_SCALE;
    }

    const UMath::Vector4 &CG() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CG;
    }

    const Attrib::StringKey &BASE_MATERIAL() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->BASE_MATERIAL;
    }

    const Attrib::StringKey &DEFAULT_COL_BOX() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DEFAULT_COL_BOX;
    }

    const float &OBJ_FRICTION(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_OBJ_FRICTION.GetLength()) {
            return lp->OBJ_FRICTION[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_OBJ_FRICTION() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_OBJ_FRICTION.GetLength();
    }

    const float &WALL_FRICTION(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_WALL_FRICTION.GetLength()) {
            return lp->WALL_FRICTION[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_WALL_FRICTION() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_WALL_FRICTION.GetLength();
    }

    const float &GROUND_FRICTION(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_GROUND_FRICTION.GetLength()) {
            return lp->GROUND_FRICTION[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_GROUND_FRICTION() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_GROUND_FRICTION.GetLength();
    }

    const float &GRAVITY() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GRAVITY;
    }

    const float &NATURAL_ANGULAR_DAMPING() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->NATURAL_ANGULAR_DAMPING;
    }

    const float &SLEEP_VELOCITY() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SLEEP_VELOCITY;
    }

    const bool &NO_GROUND_COLLISIONS() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->NO_GROUND_COLLISIONS;
    }

    const bool &IMMOBILE_OBJECT_COLLISIONS() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->IMMOBILE_OBJECT_COLLISIONS;
    }

    const bool &NO_WORLD_COLLISIONS() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->NO_WORLD_COLLISIONS;
    }

    const bool &INSTANCE_COLLISIONS_3D() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->INSTANCE_COLLISIONS_3D;
    }

    const bool &NO_OBJ_COLLISIONS() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->NO_OBJ_COLLISIONS;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
