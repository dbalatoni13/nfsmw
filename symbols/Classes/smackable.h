#ifndef ATTRIBSYS_CLASSES_SMACKABLE_H
#define ATTRIBSYS_CLASSES_SMACKABLE_H


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

struct smackable : Instance {
struct _LayoutStruct {
Attrib::StringKey EventSequencer; // offset 0x0, size 0x10
char CollectionName[4]; // offset 0x10, size 0x4
float MASS; // offset 0x14, size 0x4
float DETACH_FORCE; // offset 0x18, size 0x4
bool NO_CAR_EFFECT; // offset 0x1c, size 0x1
bool IsWooshable; // offset 0x1d, size 0x1
};

void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "smackable");
}

smackable(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

smackable(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

~smackable() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

static Key ClassKey() {
    return 0xce70d7db;
}

const EffectLinkageRecord &OnHitObject(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(this->GetAttributePointer(0x18915735, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }
        
unsigned int Num_OnHitObject() const {
            return this->Get(0x18915735).GetLength();
        }

const float &ExplosionEffect(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x360552da, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const float &DROPOUT(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x44f1273b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
unsigned int Num_DROPOUT() const {
            return this->Get(0x44f1273b).GetLength();
        }

const bool &start_sequencer(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x5739788b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &EventSequencer() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->EventSequencer;
}

const float &RESPAWN_TIME(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x5f84f834, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const unsigned int &COST_TO_STATE(unsigned int index) const {
        const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(this->GetAttributePointer(0x6db7d192, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
        }
        return *resultptr;
    }
        
const bool &AI_AVOIDABLE(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x6e4de905, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const EffectLinkageRecord &OnScrapeWorld(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(this->GetAttributePointer(0x7100960c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }
        
unsigned int Num_OnScrapeWorld() const {
            return this->Get(0x7100960c).GetLength();
        }

const bool &no_trigger(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x73c58cbf, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const RefSpec &rigidbodyspecs(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x7c90bb38, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const eDRIVE_BY_TYPE &WooshType(unsigned int index) const {
        const eDRIVE_BY_TYPE *resultptr = reinterpret_cast<const eDRIVE_BY_TYPE *>(this->GetAttributePointer(0x7e744600, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const eDRIVE_BY_TYPE *>(DefaultDataArea(sizeof(eDRIVE_BY_TYPE)));
        }
        return *resultptr;
    }
        
const float &AUTO_SIMPLIFY(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xb5c0dac8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const bool &ALLOW_OFF_WORLD(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xbee139f1, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &KILL_OFF_SCREEN(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xc141f7f8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const EffectLinkageRecord &OnDetached(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(this->GetAttributePointer(0xd177bdaa, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }
        
unsigned int Num_OnDetached() const {
            return this->Get(0xd177bdaa).GetLength();
        }

const int &CAN_SIMPLIFY(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xd1d3909a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const EffectLinkageRecord &OnHitGround(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(this->GetAttributePointer(0xd9c6cdfd, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }
        
unsigned int Num_OnHitGround() const {
            return this->Get(0xd9c6cdfd).GetLength();
        }

const Attrib::StringKey &BEHAVIORS(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0xda5f19f9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_BEHAVIORS() const {
            return this->Get(0xda5f19f9).GetLength();
        }

const EffectLinkageRecord &OnScrapeObject(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(this->GetAttributePointer(0xdacb1c11, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }
        
unsigned int Num_OnScrapeObject() const {
            return this->Get(0xdacb1c11).GetLength();
        }

const EffectLinkageRecord &OnScrapeGround(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(this->GetAttributePointer(0xdb823931, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }
        
unsigned int Num_OnScrapeGround() const {
            return this->Get(0xdb823931).GetLength();
        }

const EffectLinkageRecord &OnHitWorld(unsigned int index) const {
        const EffectLinkageRecord *resultptr = reinterpret_cast<const EffectLinkageRecord *>(this->GetAttributePointer(0xe3167336, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const EffectLinkageRecord *>(DefaultDataArea(sizeof(EffectLinkageRecord)));
        }
        return *resultptr;
    }
        
unsigned int Num_OnHitWorld() const {
            return this->Get(0xe3167336).GetLength();
        }

const bool &CAMERA_AVOIDABLE(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xe9d83d0c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &SimplePhysics(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xee0011e3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const UMath::Vector3 &MOMENT(unsigned int index) const {
        const UMath::Vector3 *resultptr = reinterpret_cast<const UMath::Vector3 *>(this->GetAttributePointer(0xfb19212f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector3 *>(DefaultDataArea(sizeof(UMath::Vector3)));
        }
        return *resultptr;
    }
        
const char*CollectionName() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
}

const float &MASS() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MASS;
}

const float &DETACH_FORCE() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DETACH_FORCE;
}

const bool &NO_CAR_EFFECT() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->NO_CAR_EFFECT;
}

const bool &IsWooshable() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->IsWooshable;
}

};

} // namespace Gen
} // namespace Attrib

#endif
