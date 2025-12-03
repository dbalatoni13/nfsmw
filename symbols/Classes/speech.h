#ifndef ATTRIBSYS_CLASSES_SPEECH_H
#define ATTRIBSYS_CLASSES_SPEECH_H


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

struct speech : Instance {
struct _LayoutStruct {
char CollectionName[4]; // offset 0x0, size 0x4
SPCHType_1_EventID SpeechID; // offset 0x4, size 0x4
float BackTime; // offset 0x8, size 0x4
float expiry; // offset 0xc, size 0x4
float CullingRange; // offset 0x10, size 0x4
float Interval; // offset 0x14, size 0x4
short priority; // offset 0x18, size 0x2
bool DoNotDropout; // offset 0x1a, size 0x1
};

void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "speech");
}

speech(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

speech(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

~speech() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

static Key ClassKey() {
    return 0xc593dd47;
}

const float &MinPlayerSpeed(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x0994e624, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const bool &RadioChirp(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x2976b959, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &RedundancyCheckIsGlobal(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x3db0e0e9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const RefSpec &RecallList(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x4710d6d5, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
unsigned int Num_RecallList() const {
            return this->Get(0x4710d6d5).GetLength();
        }

const bool &OnScreenOnly(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x4b331604, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &Pan(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x4d663bfc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const char &MinHeat(unsigned int index) const {
        const char *resultptr = reinterpret_cast<const char *>(this->GetAttributePointer(0x69603485, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const char *>(DefaultDataArea(sizeof(char)));
        }
        return *resultptr;
    }
        
const char &MaxHeat(unsigned int index) const {
        const char *resultptr = reinterpret_cast<const char *>(this->GetAttributePointer(0x77f20778, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const char *>(DefaultDataArea(sizeof(char)));
        }
        return *resultptr;
    }
        
const float &DeadAir(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x793293dd, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &MaxPlayback(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x7e18c2f8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const bool &Interruptable(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x87930ed2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &cache_SysInit(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0x8b5d5c9b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const char*CollectionName() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
}

const bool &interrupt(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xaecbb4ad, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const bool &cache_OnCreate(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xb6f42d73, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const float &MaxPlayerSpeed(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xb9befde0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const RefSpec &DepFollow(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0xc8c5d475, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
unsigned int Num_DepFollow() const {
            return this->Get(0xc8c5d475).GetLength();
        }

const float &EnforceDeadAir(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xdb3bd2f3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const bool &reqLOS(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xe0241fc1, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }
        
const unsigned char &Clarity(unsigned int index) const {
        const unsigned char *resultptr = reinterpret_cast<const unsigned char *>(this->GetAttributePointer(0xfe3254ea, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned char *>(DefaultDataArea(sizeof(unsigned char)));
        }
        return *resultptr;
    }
        
const float &InitDelay(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xffbdad32, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const SPCHType_1_EventID &SpeechID() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SpeechID;
}

const float &BackTime() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->BackTime;
}

const float &expiry() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->expiry;
}

const float &CullingRange() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CullingRange;
}

const float &Interval() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Interval;
}

const short &priority() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->priority;
}

const bool &DoNotDropout() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DoNotDropout;
}

};

} // namespace Gen
} // namespace Attrib

#endif
