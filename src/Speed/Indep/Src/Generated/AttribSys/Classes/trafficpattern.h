#ifndef ATTRIBSYS_CLASSES_TRAFFICPATTERN_H
#define ATTRIBSYS_CLASSES_TRAFFICPATTERN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

struct TrafficPatternRecord {
    // total size: 0x18
    Attrib::RefSpec Vehicle;   // offset 0x0, size 0xC
    float Rate;                // offset 0xC, size 0x4
    unsigned int MaxInstances; // offset 0x10, size 0x4
    unsigned int Percent;      // offset 0x14, size 0x4
};

namespace Attrib {
namespace Gen {

struct trafficpattern : Instance {
    struct _LayoutStruct {
        char CollectionName[4]; // offset 0x0, size 0x4
    };

    const trafficpattern &operator=(const Instance &rhs);

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "trafficpattern");
    }

    trafficpattern(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~trafficpattern() {}

    static unsigned int ClassKey();

    const char *CollectionName() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
    }

    const Instance &GetBase() const {
        return *this;
    }

    const trafficpattern &operator=(const trafficpattern &rhs) {
        *this = rhs.GetBase();
        return *this;
    }

    const TrafficPatternRecord &Vehicles(unsigned int index) const {
        // TODO "Vehicles"
        const TrafficPatternRecord *resultptr = reinterpret_cast<const TrafficPatternRecord *>(this->GetAttributePointer(0x94e3c795, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const TrafficPatternRecord *>(DefaultDataArea(sizeof(TrafficPatternRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_Vehicles() const {
        // TODO "Vehicles"
        return this->Get(0x94e3c795).GetLength();
    }
};

} // namespace Gen
} // namespace Attrib

#endif
