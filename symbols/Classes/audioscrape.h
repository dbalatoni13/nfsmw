#ifndef ATTRIBSYS_CLASSES_AUDIOSCRAPE_H
#define ATTRIBSYS_CLASSES_AUDIOSCRAPE_H


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

struct audioscrape : Instance {
void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "audioscrape");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "audioscrape");
}

audioscrape(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    
}

audioscrape(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    
}

audioscrape(const audioscrape &src) : Instance(src) {
    
}

audioscrape(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    
}

~audioscrape() {}

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
    return 0x11b47832;
}

const Attrib::StringKey &DESCRIPTION(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x09925106, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_DESCRIPTION() const {
            return Get(0x09925106).GetLength();
        }

const Attrib::StringKey &CSIS_EFFECT() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0xbec30f42, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
};

} // namespace Gen
} // namespace Attrib

#endif
