#ifndef ATTRIBSYS_CLASSES_AIVEHICLE_H
#define ATTRIBSYS_CLASSES_AIVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// TOOD move

namespace Csis {

enum Type_subject_battalion {
    Type_subject_battalion_All_Units = 128,
    Type_subject_battalion_Air_Support = 64,
    Type_subject_battalion_Rhino_Units = 32,
    Type_subject_battalion_Super_Pursuit = 16,
    Type_subject_battalion_Alpine = 8,
    Type_subject_battalion_City = 4,
    Type_subject_battalion_Coastal = 2,
    Type_subject_battalion_Rosewood = 1,
};

};

namespace Attrib {
namespace Gen {

struct aivehicle : Instance {
    struct _LayoutStruct {
        float TETHER_WEIGHT;                       // offset 0x0, size 0x4
        Csis::Type_subject_battalion DetachmentID; // offset 0x4, size 0x4
        float MAXIMUM_AI_SPEED;                    // offset 0x8, size 0x4
    };

    void *operator new(size_t bytes) {
        return Attrib::Alloc(bytes, "aivehicle");
    }

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "aivehicle");
    }

    aivehicle(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    aivehicle(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    aivehicle(const aivehicle &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    aivehicle(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~aivehicle() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0x22515733;
    }

    // const AICollisionReactionRecord &PlayerCollisions(unsigned int index) const {
    //     const AICollisionReactionRecord *resultptr = reinterpret_cast<const AICollisionReactionRecord *>(GetAttributePointer(0x489212da, index));
    //     if (!resultptr) {
    //         resultptr = reinterpret_cast<const AICollisionReactionRecord *>(DefaultDataArea(sizeof(AICollisionReactionRecord)));
    //     }
    //     return *resultptr;
    // }

    unsigned int Num_PlayerCollisions() const {
        return Get(0x489212da).GetLength();
    }

    const RefSpec &PlayerCollisionsDefault(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x75a2bcd7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const float &TETHER_WEIGHT() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->TETHER_WEIGHT;
    }

    const int &RepPointsForDestroying(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xc07c8040, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    unsigned int Num_RepPointsForDestroying() const {
        return Get(0xc07c8040).GetLength();
    }

    const float &AccelerationMultiplier() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xcc320329, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &CostToStateForDestroying(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xce1cedfe, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const float &TopSpeedMultiplier() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xec57e16b, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const Csis::Type_subject_battalion &DetachmentID() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->DetachmentID;
    }

    const float &MAXIMUM_AI_SPEED() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MAXIMUM_AI_SPEED;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
