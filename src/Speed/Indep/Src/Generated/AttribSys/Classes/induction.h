#ifndef ATTRIBSYS_CLASSES_INDUCTION_H
#define ATTRIBSYS_CLASSES_INDUCTION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace Attrib {
namespace Gen {

struct induction : Instance {
    struct _LayoutStruct {
        float LOW_BOOST;       // offset 0x0, size 0x4
        float SPOOL_TIME_DOWN; // offset 0x4, size 0x4
        float VACUUM;          // offset 0x8, size 0x4
        float SPOOL;           // offset 0xc, size 0x4
        float SPOOL_TIME_UP;   // offset 0x10, size 0x4
        float PSI;             // offset 0x14, size 0x4
        float HIGH_BOOST;      // offset 0x18, size 0x4
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "induction");
    }

    induction(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~induction() {}

    const float &LOW_BOOST() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->LOW_BOOST;
    }

    const float &SPOOL_TIME_DOWN() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SPOOL_TIME_DOWN;
    }

    const float &VACUUM() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->VACUUM;
    }

    const float &SPOOL() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SPOOL;
    }

    const float &SPOOL_TIME_UP() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SPOOL_TIME_UP;
    }

    const float &PSI() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->PSI;
    }

    const float &HIGH_BOOST() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->HIGH_BOOST;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
