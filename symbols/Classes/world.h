#ifndef ATTRIBSYS_CLASSES_WORLD_H
#define ATTRIBSYS_CLASSES_WORLD_H


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

struct world : Instance {
void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "world");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "world");
}

world(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    
}

world(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    
}

world(const world &src) : Instance(src) {
    
}

world(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    
}

~world() {}

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
    return 0x6d90da55;
}

const unsigned int &MAX_NEWTONS() const {
        const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(GetAttributePointer(0x0013821f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
        }
        return *resultptr;
    }
        
const unsigned int &MAX_FRAGMENTS() const {
        const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(GetAttributePointer(0x113d4c46, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
        }
        return *resultptr;
    }
        
const int &BONDMOVE_RESTART_CLEAR() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x134550a4, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &AIC_BOND_RGADGET() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x1c778e02, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const int &CHAIN_NEXT_MISSION() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x30f2cd5a, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &SPLITMISSION_PREVHALF() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x375aed88, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const int &NUM_PED_TYPES_GC() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x3c0e521f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &MAX_TRAFFIC_SPAWN_DISTANCE() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x3f4a4cec, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const bool &TRAFFIC_LANE_CHANGES() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0x4463a62d, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const unsigned int &MAX_SMACKABLES() const {
        const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(GetAttributePointer(0x68575d35, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &AIC_BOND_LGADGET() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x73824203, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const int &VEHICLE_SLOTS_AVAIL() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x80a91138, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &TRAFFIC_SPEED() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x811c6606, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &WORLD_TYPE() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x84da8ef1, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const int &NUM_CHECKPOINTS() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0x91ed18e9, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &PED_OBJECTS() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x968e5680, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const float &CONTROLLER_CURVE() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0x98b567dc, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &CHAIN_NEXT_MISSION_NAME() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x9df683fb, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const int &NUM_PED_TYPES_XBOX() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xac052d7a, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &HENCH_SLOTS_AVAIL() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xaea7d039, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &PED_SPAWN_RADIUS() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xb60cb556, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &TRAFFIC_TYPES(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0xb7606a9a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_TRAFFIC_TYPES() const {
            return Get(0xb7606a9a).GetLength();
        }

const bool &RACE_SCORING() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xd52754da, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const int &ANIM_BANK_NIS_BUFFER() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xdb48b25b, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &SPLITMISSION_NEXTHALF() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0xe05dec39, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const int &ACCUMULATE_SCORES() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xe2956904, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const bool &SPLITMISSION_CARRYDAMAGE() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xe58865d1, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &CHECK_PLAYER_BEHIND_TRAFFIC() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xe8a7cce2, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &MIN_TRAFFIC_SPAWN_DISTANCE() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xecd3671d, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &BONDMOVE_DIE_CLEAR() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xf3542002, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &MAX_TRAFFIC() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xfc01dc96, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const int &NUM_PED_TYPES_PS2() const {
        const int *resultptr = reinterpret_cast<const int *>(GetAttributePointer(0xfc206f2f, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const bool &LINEAR_TRACK() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xfd47cfb6, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
};

} // namespace Gen
} // namespace Attrib

#endif
