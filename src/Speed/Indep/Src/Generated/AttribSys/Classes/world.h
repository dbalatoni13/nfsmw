#ifndef ATTRIBSYS_CLASSES_WORLD_H
#define ATTRIBSYS_CLASSES_WORLD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct world : Instance {
    struct _LayoutStruct {};

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "world");
    }

    world(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    world(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~world() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0x6d90da55;
    }

    const unsigned int &MAX_NEWTONS(unsigned int index) const {
        const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(this->GetAttributePointer(0x0013821f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
        }
        return *resultptr;
    }

    const unsigned int &MAX_FRAGMENTS(unsigned int index) const {
        const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(this->GetAttributePointer(0x113d4c46, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
        }
        return *resultptr;
    }

    const int &BONDMOVE_RESTART_CLEAR(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x134550a4, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &AIC_BOND_RGADGET(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0x1c778e02, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const int &CHAIN_NEXT_MISSION(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x30f2cd5a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &SPLITMISSION_PREVHALF(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0x375aed88, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const int &NUM_PED_TYPES_GC(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x3c0e521f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const float &MAX_TRAFFIC_SPAWN_DISTANCE(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x3f4a4cec, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const bool &TRAFFIC_LANE_CHANGES(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x4463a62d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const unsigned int &MAX_SMACKABLES(unsigned int index) const {
        const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(this->GetAttributePointer(0x68575d35, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &AIC_BOND_LGADGET(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0x73824203, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const int &VEHICLE_SLOTS_AVAIL(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x80a91138, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const float &TRAFFIC_SPEED(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x811c6606, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &WORLD_TYPE(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0x84da8ef1, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const int &NUM_CHECKPOINTS(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x91ed18e9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &PED_OBJECTS(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0x968e5680, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const float &CONTROLLER_CURVE(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x98b567dc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &CHAIN_NEXT_MISSION_NAME(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0x9df683fb, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const int &NUM_PED_TYPES_XBOX(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xac052d7a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const int &HENCH_SLOTS_AVAIL(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xaea7d039, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const float &PED_SPAWN_RADIUS(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xb60cb556, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &TRAFFIC_TYPES(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0xb7606a9a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    unsigned int Num_TRAFFIC_TYPES() const {
        return this->Get(0xb7606a9a).GetLength();
    }

    const bool &RACE_SCORING(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xd52754da, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const int &ANIM_BANK_NIS_BUFFER(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xdb48b25b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const Attrib::StringKey &SPLITMISSION_NEXTHALF(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0xe05dec39, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    const int &ACCUMULATE_SCORES(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xe2956904, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const bool &SPLITMISSION_CARRYDAMAGE(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xe58865d1, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &CHECK_PLAYER_BEHIND_TRAFFIC(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xe8a7cce2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const float &MIN_TRAFFIC_SPAWN_DISTANCE(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xecd3671d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const int &BONDMOVE_DIE_CLEAR(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xf3542002, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const int &MAX_TRAFFIC(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xfc01dc96, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const int &NUM_PED_TYPES_PS2(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xfc206f2f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }

    const bool &LINEAR_TRACK(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xfd47cfb6, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
