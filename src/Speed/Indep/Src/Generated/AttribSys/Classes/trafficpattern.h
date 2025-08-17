#ifndef ATTRIBSYS_CLASSES_TRAFFICPATTERN_H
#define ATTRIBSYS_CLASSES_TRAFFICPATTERN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct trafficpattern : Instance {
    struct _LayoutStruct {
        char CollectionName[4]; // offset 0x0, size 0x4
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "trafficpattern");
    }

    trafficpattern(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~trafficpattern() {}

    const char *CollectionName() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
