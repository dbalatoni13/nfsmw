#ifndef ATTRIBSYS_CLASSES_SYSTEM_H
#define ATTRIBSYS_CLASSES_SYSTEM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct system : Instance {
    struct _LayoutStruct {
        Private _Array_SimTasks;        // offset 0x0, size 0x8
        Attrib::StringKey SimTasks[14]; // offset 0x8, size 0xe0
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "system");
    }

    system(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    system(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~system() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0x4e8dce05;
    }

    const Attrib::StringKey &SimSubSystems(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x0dfc2418, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    unsigned int Num_SimSubSystems() const {
        return Get(0x0dfc2418).GetLength();
    }

    const Attrib::StringKey &SimTasks(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(GetLayoutPointer());
        if (index < lp->_Array_SimTasks.GetLength()) {
            return lp->SimTasks[index];
        } else {
            return *reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
    }

    unsigned int Num_SimTasks() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->_Array_SimTasks.GetLength();
    }

    const Attrib::StringKey &SimControls(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0xd474e60a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    unsigned int Num_SimControls() const {
        return Get(0xd474e60a).GetLength();
    }
};

} // namespace Gen
} // namespace Attrib

#endif
