#ifndef ATTRIBSYS_CLASSES_COLLISIONREACTIONS_H
#define ATTRIBSYS_CLASSES_COLLISIONREACTIONS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

struct CollisionReactionRecord {
    // total size: 0x18
    float Elasticity; // offset 0x0, size 0x4
    float RollHeight; // offset 0x4, size 0x4
    float WeightBias; // offset 0x8, size 0x4
    float MassScale;  // offset 0xC, size 0x4
    float StunSpeed;  // offset 0x10, size 0x4
    float StunTime;   // offset 0x14, size 0x4
};

namespace Attrib {
namespace Gen {

struct collisionreactions : Instance {
    struct _LayoutStruct {
        CollisionReactionRecord REARSIDE_REACTION;  // offset 0x0, size 0x18
        CollisionReactionRecord FRONTSIDE_REACTION; // offset 0x18, size 0x18
        CollisionReactionRecord FRONT_REACTION;     // offset 0x30, size 0x18
        CollisionReactionRecord REAR_REACTION;      // offset 0x48, size 0x18
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "collisionreactions");
    }

    collisionreactions(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    collisionreactions(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~collisionreactions() {}

    const collisionreactions &operator=(const collisionreactions &rhs) {
        Instance::operator=(rhs);
        return *this;
    }

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0xb32682f1;
    }

    const CollisionReactionRecord &REARSIDE_REACTION() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->REARSIDE_REACTION;
    }

    const CollisionReactionRecord &FRONTSIDE_REACTION() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->FRONTSIDE_REACTION;
    }

    const CollisionReactionRecord &FRONT_REACTION() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->FRONT_REACTION;
    }

    const CollisionReactionRecord &REAR_REACTION() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->REAR_REACTION;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
