#ifndef ATTRIBSYS_CLASSES_PURSUITSUPPORT_H
#define ATTRIBSYS_CLASSES_PURSUITSUPPORT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

// TODO
enum AirSupportStrategy {
    HI_PATROL = 0,
    PURSUIT = 1,
    SKID_HIT = 2,
    SPIKE_DROP = 3,
};

enum HeavySupportStrategy {
    E_BRAKE = 1,
    COORDINATED_E_BRAKE = 2,
    RAM = 3,
    HEAVY_ROADBLOCK = 4,
};

enum LeaderSupportStrategy {
    CROSS_FOLLOW = 5,
    CROSS_BRAKE = 6,
    CROSS_PLUS_V_BLOCK = 7,
};

// total size: 0xC
struct AirSupport {
    // Members
    AirSupportStrategy HeliStrategy; // offset 0x0, size 0x4
    unsigned int Chance;             // offset 0x4, size 0x4
    float Duration;                  // offset 0x8, size 0x4
};

// total size: 0x10
struct HeavySupport {
    // Members
    HeavySupportStrategy HeavyStrategy; // offset 0x0, size 0x4
    unsigned int Chance;                // offset 0x4, size 0x4
    float Duration;                     // offset 0x8, size 0x4
    unsigned int ChanceBigSUV;          // offset 0xC, size 0x4
};

// total size: 0x14
struct LeaderSupport {
    // Members
    enum LeaderSupportStrategy LeaderStrategy; // offset 0x0, size 0x4
    unsigned int Chance;                       // offset 0x4, size 0x4
    float Duration;                            // offset 0x8, size 0x4
    unsigned int PriorityChance;               // offset 0xC, size 0x4
    float PriorityTime;                        // offset 0x10, size 0x4
};

namespace Attrib {
namespace Gen {

struct pursuitsupport : Instance {
    void *operator new(size_t bytes) {
        return Attrib::Alloc(bytes, "pursuitsupport");
    }

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "pursuitsupport");
    }

    pursuitsupport(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}

    pursuitsupport(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}

    pursuitsupport(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}

    ~pursuitsupport() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0x77b93104;
    }

    const AirSupport &AirSupportOptions(unsigned int index) const {
        const AirSupport *resultptr = reinterpret_cast<const AirSupport *>(this->GetAttributePointer(0x3c6dbcb3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const AirSupport *>(DefaultDataArea(sizeof(AirSupport)));
        }
        return *resultptr;
    }

    unsigned int Num_AirSupportOptions() const {
        return this->Get(0x3c6dbcb3).GetLength();
    }

    const HeavySupport &HeavySupportOptions(unsigned int index) const {
        const HeavySupport *resultptr = reinterpret_cast<const HeavySupport *>(this->GetAttributePointer(0xa73c3512, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const HeavySupport *>(DefaultDataArea(sizeof(HeavySupport)));
        }
        return *resultptr;
    }

    unsigned int Num_HeavySupportOptions() const {
        return this->Get(0xa73c3512).GetLength();
    }

    const float &MinimumSupportDelay(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xe4e4bc48, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const LeaderSupport &LeaderSupportOptions(unsigned int index) const {
        const LeaderSupport *resultptr = reinterpret_cast<const LeaderSupport *>(this->GetAttributePointer(0xebd53935, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const LeaderSupport *>(DefaultDataArea(sizeof(LeaderSupport)));
        }
        return *resultptr;
    }

    unsigned int Num_LeaderSupportOptions() const {
        return this->Get(0xebd53935).GetLength();
    }
};

} // namespace Gen
} // namespace Attrib

#endif
