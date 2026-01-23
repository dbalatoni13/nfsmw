#ifndef ATTRIBSYS_CLASSES_SHIFTPATTERN_H
#define ATTRIBSYS_CLASSES_SHIFTPATTERN_H


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

struct shiftpattern : Instance {
struct _LayoutStruct {
Attrib::StringKey BankName; // offset 0x0, size 0x10
float Down_Shift_Sound_Delay; // offset 0x10, size 0x4
float Up_Engaging_Attack_Vol; // offset 0x14, size 0x4
unsigned int LFO_RPM_Amp; // offset 0x18, size 0x4
unsigned int LFO_Vol_Amp; // offset 0x1c, size 0x4
unsigned int Down_Engaging_Rise_T; // offset 0x20, size 0x4
unsigned int Up_Engaging_Attack_T; // offset 0x24, size 0x4
unsigned int LFO_RPM_Decay_Time; // offset 0x28, size 0x4
float Down_Reattach_Scale; // offset 0x2c, size 0x4
unsigned int Down_Engaging_Fall_T; // offset 0x30, size 0x4
float Up_Shift_Sound_Delay; // offset 0x34, size 0x4
unsigned int Up_Vol_Shift; // offset 0x38, size 0x4
unsigned int Down_Disengage_Fall_T; // offset 0x3c, size 0x4
unsigned int LFO_Vol_Freq; // offset 0x40, size 0x4
unsigned int Down_Disengage_Fall_RPM; // offset 0x44, size 0x4
unsigned int Down_Vol_Shift; // offset 0x48, size 0x4
unsigned int Down_Engaging_Rise_RPM; // offset 0x4c, size 0x4
unsigned int LFO_RPM_Freq; // offset 0x50, size 0x4
unsigned int LFO_Vol_Decay_Time; // offset 0x54, size 0x4
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "shiftpattern");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "shiftpattern");
}

shiftpattern(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

shiftpattern(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

shiftpattern(const shiftpattern &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

shiftpattern(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~shiftpattern() {}

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
    return 0xdb01b754;
}

const unsigned int &Down_Engaging_Fall_RPM() const {
        const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(GetAttributePointer(0x3e1a0db6, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
        }
        return *resultptr;
    }
        
const Attrib::Types::Matrix &Up_Engage_Curve() const {
        const Attrib::Types::Matrix *resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(GetAttributePointer(0x68da6275, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(DefaultDataArea(sizeof(Attrib::Types::Matrix)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &BankName() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->BankName;
}

const stShiftPair &Up_Engage() const {
        const stShiftPair *resultptr = reinterpret_cast<const stShiftPair *>(GetAttributePointer(0xcb89b8c8, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const stShiftPair *>(DefaultDataArea(sizeof(stShiftPair)));
        }
        return *resultptr;
    }
        
const Attrib::Types::Matrix &Up_DisengageFall_Curve(unsigned int index) const {
        const Attrib::Types::Matrix *resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(GetAttributePointer(0xf040e6b0, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(DefaultDataArea(sizeof(Attrib::Types::Matrix)));
        }
        return *resultptr;
    }
        
unsigned int Num_Up_DisengageFall_Curve() const {
            return Get(0xf040e6b0).GetLength();
        }

const stShiftPair &Up_DisengageFall(unsigned int index) const {
        const stShiftPair *resultptr = reinterpret_cast<const stShiftPair *>(GetAttributePointer(0xf2d90101, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const stShiftPair *>(DefaultDataArea(sizeof(stShiftPair)));
        }
        return *resultptr;
    }
        
unsigned int Num_Up_DisengageFall() const {
            return Get(0xf2d90101).GetLength();
        }

const float &Down_Shift_Sound_Delay() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Down_Shift_Sound_Delay;
}

const float &Up_Engaging_Attack_Vol() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Up_Engaging_Attack_Vol;
}

const unsigned int &LFO_RPM_Amp() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->LFO_RPM_Amp;
}

const unsigned int &LFO_Vol_Amp() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->LFO_Vol_Amp;
}

const unsigned int &Down_Engaging_Rise_T() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Down_Engaging_Rise_T;
}

const unsigned int &Up_Engaging_Attack_T() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Up_Engaging_Attack_T;
}

const unsigned int &LFO_RPM_Decay_Time() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->LFO_RPM_Decay_Time;
}

const float &Down_Reattach_Scale() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Down_Reattach_Scale;
}

const unsigned int &Down_Engaging_Fall_T() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Down_Engaging_Fall_T;
}

const float &Up_Shift_Sound_Delay() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Up_Shift_Sound_Delay;
}

const unsigned int &Up_Vol_Shift() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Up_Vol_Shift;
}

const unsigned int &Down_Disengage_Fall_T() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Down_Disengage_Fall_T;
}

const unsigned int &LFO_Vol_Freq() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->LFO_Vol_Freq;
}

const unsigned int &Down_Disengage_Fall_RPM() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Down_Disengage_Fall_RPM;
}

const unsigned int &Down_Vol_Shift() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Down_Vol_Shift;
}

const unsigned int &Down_Engaging_Rise_RPM() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->Down_Engaging_Rise_RPM;
}

const unsigned int &LFO_RPM_Freq() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->LFO_RPM_Freq;
}

const unsigned int &LFO_Vol_Decay_Time() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->LFO_Vol_Decay_Time;
}

};

} // namespace Gen
} // namespace Attrib

#endif
