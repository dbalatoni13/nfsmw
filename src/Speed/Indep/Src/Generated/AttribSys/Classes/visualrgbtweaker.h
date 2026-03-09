#ifndef ATTRIBSYS_CLASSES_VISUALRGBTWEAKER_H
#define ATTRIBSYS_CLASSES_VISUALRGBTWEAKER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct visualrgbtweaker : Instance {
    void *operator new(size_t bytes) {
        return Attrib::Alloc(bytes, "visualrgbtweaker");
    }

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "visualrgbtweaker");
    }

    visualrgbtweaker(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}

    visualrgbtweaker(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}

    visualrgbtweaker(const visualrgbtweaker &src) : Instance(src) {}

    visualrgbtweaker(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}

    ~visualrgbtweaker() {}

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
        return 0xaf1837ca;
    }

    const UMath::Matrix4 &blue() const {
        const UMath::Matrix4 *resultptr = reinterpret_cast<const UMath::Matrix4 *>(GetAttributePointer(0x1c641189, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Matrix4 *>(DefaultDataArea(sizeof(UMath::Matrix4)));
        }
        return *resultptr;
    }

    const UMath::Matrix4 &green() const {
        const UMath::Matrix4 *resultptr = reinterpret_cast<const UMath::Matrix4 *>(GetAttributePointer(0x3ffd271c, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Matrix4 *>(DefaultDataArea(sizeof(UMath::Matrix4)));
        }
        return *resultptr;
    }

    const UMath::Matrix4 &red() const {
        const UMath::Matrix4 *resultptr = reinterpret_cast<const UMath::Matrix4 *>(GetAttributePointer(0x5cd90211, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Matrix4 *>(DefaultDataArea(sizeof(UMath::Matrix4)));
        }
        return *resultptr;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
