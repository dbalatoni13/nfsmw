#ifndef ATTRIBSYS_CLASSES_TRAFFICPATTERN_H
#define ATTRIBSYS_CLASSES_TRAFFICPATTERN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
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

    trafficpattern(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    trafficpattern(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~trafficpattern() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0x20d08342;
    }

    const Instance &GetBase() const {
        return *this;
    }

    const trafficpattern &operator=(const trafficpattern &rhs) {
        *this = rhs.GetBase();
        return *this;
    }

    const float &SpeedStreet(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x7c44962f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const TrafficPatternRecord &Vehicles(unsigned int index) const {
        const TrafficPatternRecord *resultptr = reinterpret_cast<const TrafficPatternRecord *>(this->GetAttributePointer(0x94e3c795, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const TrafficPatternRecord *>(DefaultDataArea(sizeof(TrafficPatternRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_Vehicles() const {
        return this->Get(0x94e3c795).GetLength();
    }

    const char *CollectionName() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
    }

    const float &SpeedHighway(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x9e404e33, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &SpawnTime(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xbf2fdb5c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
