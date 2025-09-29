#ifndef ATTRIBSYS_CLASSES_NOS_H
#define ATTRIBSYS_CLASSES_NOS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace Attrib {
namespace Gen {

struct nos : Instance {
    struct _LayoutStruct {
        float NOS_DISENGAGE;      // offset 0x0, size 0x4
        float TORQUE_BOOST;       // offset 0x4, size 0x4
        float FLOW_RATE;          // offset 0x8, size 0x4
        float RECHARGE_MIN;       // offset 0xc, size 0x4
        float NOS_CAPACITY;       // offset 0x10, size 0x4
        float RECHARGE_MAX;       // offset 0x14, size 0x4
        float RECHARGE_MAX_SPEED; // offset 0x18, size 0x4
        float RECHARGE_MIN_SPEED; // offset 0x1c, size 0x4
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "nos");
    }

    nos(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    nos(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~nos() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0x77790508;
    }

    const float &NOS_DISENGAGE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->NOS_DISENGAGE;
    }

    const float &TORQUE_BOOST() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->TORQUE_BOOST;
    }

    const float &FLOW_RATE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->FLOW_RATE;
    }

    const float &RECHARGE_MIN() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->RECHARGE_MIN;
    }

    const float &NOS_CAPACITY() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->NOS_CAPACITY;
    }

    const float &RECHARGE_MAX() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->RECHARGE_MAX;
    }

    const float &RECHARGE_MAX_SPEED() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->RECHARGE_MAX_SPEED;
    }

    const float &RECHARGE_MIN_SPEED() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->RECHARGE_MIN_SPEED;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
