#ifndef ATTRIBSYS_CLASSES_TURBOSFX_H
#define ATTRIBSYS_CLASSES_TURBOSFX_H


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

struct turbosfx : Instance {
struct _LayoutStruct {
Attrib::StringKey BankName; // offset 0x0, size 0x10
unsigned int Vol_Spool; // offset 0x10, size 0x4
float ChargeTime; // offset 0x14, size 0x4
unsigned int Vol_Blowoff1; // offset 0x18, size 0x4
unsigned int Vol_Blowoff2; // offset 0x1c, size 0x4
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "turbosfx");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "turbosfx");
}

turbosfx(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

turbosfx(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

turbosfx(const turbosfx &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

turbosfx(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~turbosfx() {}

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
    return 0x55624a85;
}

const Attrib::StringKey &BankName() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->BankName;
}

const float &Leak_Rate() const {
        const float *resultptr = reinterpret_cast<const float *>(GetAttributePointer(0xfdf3bc20, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const unsigned int &Vol_Spool() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Vol_Spool;
}

const float &ChargeTime() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->ChargeTime;
}

const unsigned int &Vol_Blowoff1() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Vol_Blowoff1;
}

const unsigned int &Vol_Blowoff2() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Vol_Blowoff2;
}

};

} // namespace Gen
} // namespace Attrib

#endif
