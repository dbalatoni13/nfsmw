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

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "aivehicle");
    }

    aivehicle(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    aivehicle(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
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

    const float &TETHER_WEIGHT() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->TETHER_WEIGHT;
    }

    const Csis::Type_subject_battalion &DetachmentID() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DetachmentID;
    }

    const float &MAXIMUM_AI_SPEED() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MAXIMUM_AI_SPEED;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
