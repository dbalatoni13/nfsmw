#ifndef ATTRIBSYS_CLASSES_MUSIC_H
#define ATTRIBSYS_CLASSES_MUSIC_H


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

struct music : Instance {
struct _LayoutStruct {
Attrib::StringKey Artist; // offset 0x0, size 0x10
Attrib::StringKey SongName; // offset 0x10, size 0x10
Attrib::StringKey Album; // offset 0x20, size 0x10
PathEventEnum PathEvent; // offset 0x30, size 0x4
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "music");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "music");
}

music(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

music(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

music(const music &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

music(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~music() {}

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
    return 0x565465f8;
}

const Attrib::StringKey &Artist() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Artist;
}

const Attrib::StringKey &DefPlay() const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x58f80e5e, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &SongName() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SongName;
}

const Attrib::StringKey &Album() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Album;
}

const PathEventEnum &PathEvent() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->PathEvent;
}

};

} // namespace Gen
} // namespace Attrib

#endif
