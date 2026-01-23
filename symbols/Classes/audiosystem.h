#ifndef ATTRIBSYS_CLASSES_AUDIOSYSTEM_H
#define ATTRIBSYS_CLASSES_AUDIOSYSTEM_H


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

struct audiosystem : Instance {
struct _LayoutStruct {
Attrib::StringKey CSIPath; // offset 0x0, size 0x10
Attrib::StringKey BIGPath; // offset 0x10, size 0x10
Attrib::StringKey IDXPath; // offset 0x20, size 0x10
Attrib::StringKey EVTPath; // offset 0x30, size 0x10
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "audiosystem");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "audiosystem");
}

audiosystem(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

audiosystem(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

audiosystem(const audiosystem &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

audiosystem(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~audiosystem() {}

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
    return 0xd3c18f03;
}

const Attrib::StringKey &AEMS_RNBanks() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x0663aaf3, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const RefSpec &FESpeech() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x174f218e, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &EvtSys(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x4166fa80, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_EvtSys() const {
            return Get(0x4166fa80).GetLength();
        }

const RefSpec &InGameSpeech() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x4f085f82, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const RefSpec &LicensedMusic() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x5611db83, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const RefSpec &PFMapping(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0x737714de, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
unsigned int Num_PFMapping() const {
            return Get(0x737714de).GetLength();
        }

const Attrib::StringKey &AEMS_EnvBanks() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x7e6ba5c8, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &AEMS_NOSBanks(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x8aca07f9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_AEMS_NOSBanks() const {
            return Get(0x8aca07f9).GetLength();
        }

const Attrib::StringKey &AEMS_WNBanks(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x916aa05d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_AEMS_WNBanks() const {
            return Get(0x916aa05d).GetLength();
        }

const Attrib::StringKey &AEMS_FEBanks(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x9849f8df, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_AEMS_FEBanks() const {
            return Get(0x9849f8df).GetLength();
        }

const Attrib::StringKey &AEMS_SkidBanks(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0xbd6f7135, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_AEMS_SkidBanks() const {
            return Get(0xbd6f7135).GetLength();
        }

const RefSpec &Locales(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xc243117c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
unsigned int Num_Locales() const {
            return Get(0xc243117c).GetLength();
        }

const Attrib::StringKey &AEMS_MiscBanks(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0xe48df448, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_AEMS_MiscBanks() const {
            return Get(0xe48df448).GetLength();
        }

const RefSpec &nissfxstreams() const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(GetAttributePointer(0xe6e5c2a3, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &AEMS_StitchBanks(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0xeb1fd1bf, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_AEMS_StitchBanks() const {
            return Get(0xeb1fd1bf).GetLength();
        }

const Attrib::StringKey &AEMS_SparkBanks(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0xf44ae8ed, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_AEMS_SparkBanks() const {
            return Get(0xf44ae8ed).GetLength();
        }

const Attrib::StringKey &CSIPath() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->CSIPath;
}

const Attrib::StringKey &BIGPath() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->BIGPath;
}

const Attrib::StringKey &IDXPath() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->IDXPath;
}

const Attrib::StringKey &EVTPath() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->EVTPath;
}

};

} // namespace Gen
} // namespace Attrib

#endif
