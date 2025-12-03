#ifndef ATTRIBSYS_CLASSES_ENGINEAUDIO_H
#define ATTRIBSYS_CLASSES_ENGINEAUDIO_H


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

struct engineaudio : Instance {
struct _LayoutStruct {
Attrib::StringKey Filename_GinsuDecel; // offset 0x0, size 0x10
Attrib::StringKey BankName_mainRAM; // offset 0x10, size 0x10
Attrib::StringKey Filename_GinsuAccel; // offset 0x20, size 0x10
RefSpec acceltrans; // offset 0x30, size 0xc
char CollectionName[4]; // offset 0x3c, size 0x4
eENGINE_GROUP EngType; // offset 0x40, size 0x4
unsigned int DECEL_AEMSVol; // offset 0x44, size 0x4
float DECEL_GINSUMix_S_RPM; // offset 0x48, size 0x4
unsigned int GINSU_Decel_MaxRPM; // offset 0x4c, size 0x4
float GINSU_DECEL_FADE_IN; // offset 0x50, size 0x4
float Priority; // offset 0x54, size 0x4
float GINSU_DECEL_FADE_OUT; // offset 0x58, size 0x4
unsigned int GinsuDecelVol; // offset 0x5c, size 0x4
unsigned int GINSU_Decel_MinRPM; // offset 0x60, size 0x4
float DecelDeltaRPMThreshold; // offset 0x64, size 0x4
float AEMSMix_S_RPM; // offset 0x68, size 0x4
float Ginsu_ACL_Neg_L_RPM; // offset 0x6c, size 0x4
float AEMSMix_L_RPM; // offset 0x70, size 0x4
unsigned int GINSUAccelVol; // offset 0x74, size 0x4
float AccelDeltaRPMThreshold; // offset 0x78, size 0x4
float GINSUMix_S_RPM; // offset 0x7c, size 0x4
float DECEL_GINSUMix_L_RPM; // offset 0x80, size 0x4
unsigned int AEMSVol; // offset 0x84, size 0x4
float DECEL_AEMSMix_L_RPM; // offset 0x88, size 0x4
float DECEL_AEMSMix_S_RPM; // offset 0x8c, size 0x4
float GINSUMix_L_RPM; // offset 0x90, size 0x4
unsigned int CarID; // offset 0x94, size 0x4
float MinRPM; // offset 0x98, size 0x4
float MaxRPM; // offset 0x9c, size 0x4
unsigned short Master_Vol; // offset 0xa0, size 0x2
bool MaybeV8; // offset 0xa2, size 0x1
bool Tranny; // offset 0xa3, size 0x1
};

void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "engineaudio");
}

engineaudio(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

engineaudio(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

~engineaudio() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

static Key ClassKey() {
    return 0x50eab0e6;
}

const Attrib::StringKey &BankName_auxRAM(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0x04935eab, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_BankName_auxRAM() const {
            return this->Get(0x04935eab).GetLength();
        }

const Attrib::Types::Matrix &PhysicsRPM_Map(unsigned int index) const {
        const Attrib::Types::Matrix *resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(this->GetAttributePointer(0x07e3c833, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::Types::Matrix *>(DefaultDataArea(sizeof(Attrib::Types::Matrix)));
        }
        return *resultptr;
    }
        
const float &DecelPitchOffset(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x313385dc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const int &Vol_ShiftSweets(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0x34beca33, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const float &Ginsu_ACL_Neg_S_RPM(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x38afe02e, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &Filename_GinsuDecel() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Filename_GinsuDecel;
}

const int &Vol_Sputters(unsigned int index) const {
        const int *resultptr = reinterpret_cast<const int *>(this->GetAttributePointer(0xc1eddd78, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const int *>(DefaultDataArea(sizeof(int)));
        }
        return *resultptr;
    }
        
const unsigned int &GINSU_LowPassCutoff(unsigned int index) const {
        const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(this->GetAttributePointer(0xe3836473, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
        }
        return *resultptr;
    }
        
const Attrib::StringKey &SweetBank(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(this->GetAttributePointer(0xee501c6a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_SweetBank() const {
            return this->Get(0xee501c6a).GetLength();
        }

const Attrib::StringKey &BankName_mainRAM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->BankName_mainRAM;
}

const Attrib::StringKey &Filename_GinsuAccel() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Filename_GinsuAccel;
}

const RefSpec &acceltrans() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->acceltrans;
}

const char*CollectionName() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
}

const eENGINE_GROUP &EngType() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->EngType;
}

const unsigned int &DECEL_AEMSVol() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DECEL_AEMSVol;
}

const float &DECEL_GINSUMix_S_RPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DECEL_GINSUMix_S_RPM;
}

const unsigned int &GINSU_Decel_MaxRPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GINSU_Decel_MaxRPM;
}

const float &GINSU_DECEL_FADE_IN() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GINSU_DECEL_FADE_IN;
}

const float &Priority() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Priority;
}

const float &GINSU_DECEL_FADE_OUT() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GINSU_DECEL_FADE_OUT;
}

const unsigned int &GinsuDecelVol() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GinsuDecelVol;
}

const unsigned int &GINSU_Decel_MinRPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GINSU_Decel_MinRPM;
}

const float &DecelDeltaRPMThreshold() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DecelDeltaRPMThreshold;
}

const float &AEMSMix_S_RPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AEMSMix_S_RPM;
}

const float &Ginsu_ACL_Neg_L_RPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Ginsu_ACL_Neg_L_RPM;
}

const float &AEMSMix_L_RPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AEMSMix_L_RPM;
}

const unsigned int &GINSUAccelVol() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GINSUAccelVol;
}

const float &AccelDeltaRPMThreshold() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AccelDeltaRPMThreshold;
}

const float &GINSUMix_S_RPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GINSUMix_S_RPM;
}

const float &DECEL_GINSUMix_L_RPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DECEL_GINSUMix_L_RPM;
}

const unsigned int &AEMSVol() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AEMSVol;
}

const float &DECEL_AEMSMix_L_RPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DECEL_AEMSMix_L_RPM;
}

const float &DECEL_AEMSMix_S_RPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DECEL_AEMSMix_S_RPM;
}

const float &GINSUMix_L_RPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GINSUMix_L_RPM;
}

const unsigned int &CarID() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CarID;
}

const float &MinRPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MinRPM;
}

const float &MaxRPM() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MaxRPM;
}

const unsigned short &Master_Vol() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Master_Vol;
}

const bool &MaybeV8() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MaybeV8;
}

const bool &Tranny() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Tranny;
}

};

} // namespace Gen
} // namespace Attrib

#endif
