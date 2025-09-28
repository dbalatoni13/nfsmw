#ifndef ATTRIBSYS_CLASSES_BRAKES_H
#define ATTRIBSYS_CLASSES_BRAKES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct brakes : Instance {
    struct _LayoutStruct {
        AxlePair BRAKE_LOCK; // offset 0x0, size 0x8
        AxlePair BRAKES;     // offset 0x8, size 0x8
        float EBRAKE;        // offset 0x10, size 0x4
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "brakes");
    }

    brakes(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~brakes() {}

    const AxlePair &BRAKE_LOCK() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->BRAKE_LOCK;
    }

    const AxlePair &BRAKES() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->BRAKES;
    }

    const float &EBRAKE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->EBRAKE;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
