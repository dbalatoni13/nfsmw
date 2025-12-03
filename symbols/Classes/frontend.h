#ifndef ATTRIBSYS_CLASSES_FRONTEND_H
#define ATTRIBSYS_CLASSES_FRONTEND_H


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

struct frontend : Instance {
struct _LayoutStruct {
RefSpec cam_angle; // offset 0x0, size 0xc
RefSpec default_car; // offset 0xc, size 0xc
char CollectionName[4]; // offset 0x18, size 0x4
float cam_damping; // offset 0x1c, size 0x4
float cam_anim_speed; // offset 0x20, size 0x4
float cam_fov; // offset 0x24, size 0x4
float cam_blur; // offset 0x28, size 0x4
float cam_lookat_x; // offset 0x2c, size 0x4
float cam_lookat_y; // offset 0x30, size 0x4
float cam_lookat_z; // offset 0x34, size 0x4
unsigned int region; // offset 0x38, size 0x4
float cam_orbit_vertical; // offset 0x3c, size 0x4
unsigned int manufacturer; // offset 0x40, size 0x4
float cam_roll_angle; // offset 0x44, size 0x4
int cam_periods; // offset 0x48, size 0x4
int Cost; // offset 0x4c, size 0x4
float cam_orbit_radius; // offset 0x50, size 0x4
float cam_orbit_horizontal; // offset 0x54, size 0x4
bool IsCustomizable; // offset 0x58, size 0x1
unsigned char UnlockedAt; // offset 0x59, size 0x1
bool cam_user_rotate; // offset 0x5a, size 0x1
};

void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "frontend");
}

frontend(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

frontend(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    this->SetDefaultLayout(sizeof(_LayoutStruct));
}

~frontend() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

static Key ClassKey() {
    return 0x85885722;
}

const type_bStringHash &p_tires_2(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x01e6ddf1, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_tires_2() const {
            return this->Get(0x01e6ddf1).GetLength();
        }

const type_bStringHash &p_induction_1(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x0e141cde, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_induction_1() const {
            return this->Get(0x0e141cde).GetLength();
        }

const FECarPartInfo &feCarPartInfo(unsigned int index) const {
        const FECarPartInfo *resultptr = reinterpret_cast<const FECarPartInfo *>(this->GetAttributePointer(0x1002ea65, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const FECarPartInfo *>(DefaultDataArea(sizeof(FECarPartInfo)));
        }
        return *resultptr;
    }
        
unsigned int Num_feCarPartInfo() const {
            return this->Get(0x1002ea65).GetLength();
        }

const type_bStringHash &p_tires_4(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x16b700d6, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_tires_4() const {
            return this->Get(0x16b700d6).GetLength();
        }

const type_bStringHash &p_transmission_1(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x1e823f0b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_transmission_1() const {
            return this->Get(0x1e823f0b).GetLength();
        }

const RefSpec &cam_angle() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_angle;
}

const char &NameId(unsigned int index) const {
        const char *resultptr = reinterpret_cast<const char *>(this->GetAttributePointer(0x2c3c7feb, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const char *>(DefaultDataArea(sizeof(char)));
        }
        return *resultptr;
    }
        
unsigned int Num_NameId() const {
            return this->Get(0x2c3c7feb).GetLength();
        }

const type_bStringHash &p_suspension_1(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x37ea2169, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_suspension_1() const {
            return this->Get(0x37ea2169).GetLength();
        }

const eUnlockableEntity &feCarPartName(unsigned int index) const {
        const eUnlockableEntity *resultptr = reinterpret_cast<const eUnlockableEntity *>(this->GetAttributePointer(0x3b874697, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const eUnlockableEntity *>(DefaultDataArea(sizeof(eUnlockableEntity)));
        }
        return *resultptr;
    }
        
const type_bStringHash &p_induction_2(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x4d3b62f3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_induction_2() const {
            return this->Get(0x4d3b62f3).GetLength();
        }

const type_bStringHash &p_engine_3(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x4f56a655, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_engine_3() const {
            return this->Get(0x4f56a655).GetLength();
        }

const type_bStringHash &p_engine_1(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x512303af, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_engine_1() const {
            return this->Get(0x512303af).GetLength();
        }

const type_bStringHash &p_brakes_2(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x70b14851, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_brakes_2() const {
            return this->Get(0x70b14851).GetLength();
        }

const type_bStringHash &p_transmission_2(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x79c8d7e9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_transmission_2() const {
            return this->Get(0x79c8d7e9).GetLength();
        }

const type_bStringHash &p_nitrous_1(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x7f6e85a3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_nitrous_1() const {
            return this->Get(0x7f6e85a3).GetLength();
        }

const type_bStringHash &p_engine_4(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x85ab21da, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_engine_4() const {
            return this->Get(0x85ab21da).GetLength();
        }

const type_bStringHash &p_nitrous_4(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x8da087a4, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_nitrous_4() const {
            return this->Get(0x8da087a4).GetLength();
        }

const type_bStringHash &p_brakes_3(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x8e8b78e1, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_brakes_3() const {
            return this->Get(0x8e8b78e1).GetLength();
        }

const type_bStringHash &p_tires_3(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0x92378a0a, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_tires_3() const {
            return this->Get(0x92378a0a).GetLength();
        }

const type_bStringHash &p_transmission_3(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0xa1b53a33, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_transmission_3() const {
            return this->Get(0xa1b53a33).GetLength();
        }

const type_bStringHash &p_nitrous_3(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0xa459ecef, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_nitrous_3() const {
            return this->Get(0xa459ecef).GetLength();
        }

const type_bStringHash &p_brakes_4(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0xb4df5439, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_brakes_4() const {
            return this->Get(0xb4df5439).GetLength();
        }

const unsigned int &WarningLevel(unsigned int index) const {
        const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(this->GetAttributePointer(0xb54d4a41, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
        }
        return *resultptr;
    }
        
unsigned int Num_WarningLevel() const {
            return this->Get(0xb54d4a41).GetLength();
        }

const eUnlockableEntity &CarPartName(unsigned int index) const {
        const eUnlockableEntity *resultptr = reinterpret_cast<const eUnlockableEntity *>(this->GetAttributePointer(0xb668787d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const eUnlockableEntity *>(DefaultDataArea(sizeof(eUnlockableEntity)));
        }
        return *resultptr;
    }
        
const type_bStringHash &p_induction_4(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0xb6be1d52, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_induction_4() const {
            return this->Get(0xb6be1d52).GetLength();
        }

const type_bStringHash &p_nitrous_2(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0xd810d2dc, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_nitrous_2() const {
            return this->Get(0xd810d2dc).GetLength();
        }

const type_bStringHash &p_engine_2(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0xdb8a8a1d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_engine_2() const {
            return this->Get(0xdb8a8a1d).GetLength();
        }

const type_bStringHash &p_suspension_3(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0xe321687d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_suspension_3() const {
            return this->Get(0xe321687d).GetLength();
        }

const type_bStringHash &p_brakes_1(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0xe4af1260, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_brakes_1() const {
            return this->Get(0xe4af1260).GetLength();
        }

const type_bStringHash &p_suspension_2(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0xe5650914, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_suspension_2() const {
            return this->Get(0xe5650914).GetLength();
        }

const type_bStringHash &p_induction_3(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0xea7f3fe4, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_induction_3() const {
            return this->Get(0xea7f3fe4).GetLength();
        }

const type_bStringHash &p_tires_1(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0xf0c7c400, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_tires_1() const {
            return this->Get(0xf0c7c400).GetLength();
        }

const type_bStringHash &p_transmission_4(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0xf424c06d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_transmission_4() const {
            return this->Get(0xf424c06d).GetLength();
        }

const float &RapSheetRanks(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xf9a7d5f7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }
        
unsigned int Num_RapSheetRanks() const {
            return this->Get(0xf9a7d5f7).GetLength();
        }

const type_bStringHash &p_suspension_4(unsigned int index) const {
        const type_bStringHash *resultptr = reinterpret_cast<const type_bStringHash *>(this->GetAttributePointer(0xfb1ef23f, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const type_bStringHash *>(DefaultDataArea(sizeof(type_bStringHash)));
        }
        return *resultptr;
    }
        
unsigned int Num_p_suspension_4() const {
            return this->Get(0xfb1ef23f).GetLength();
        }

const RefSpec &default_car() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->default_car;
}

const char*CollectionName() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
}

const float &cam_damping() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_damping;
}

const float &cam_anim_speed() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_anim_speed;
}

const float &cam_fov() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_fov;
}

const float &cam_blur() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_blur;
}

const float &cam_lookat_x() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_lookat_x;
}

const float &cam_lookat_y() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_lookat_y;
}

const float &cam_lookat_z() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_lookat_z;
}

const unsigned int &region() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->region;
}

const float &cam_orbit_vertical() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_orbit_vertical;
}

const unsigned int &manufacturer() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->manufacturer;
}

const float &cam_roll_angle() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_roll_angle;
}

const int &cam_periods() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_periods;
}

const int &Cost() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Cost;
}

const float &cam_orbit_radius() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_orbit_radius;
}

const float &cam_orbit_horizontal() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_orbit_horizontal;
}

const bool &IsCustomizable() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->IsCustomizable;
}

const unsigned char &UnlockedAt() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->UnlockedAt;
}

const bool &cam_user_rotate() const {
    return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->cam_user_rotate;
}

};

} // namespace Gen
} // namespace Attrib

#endif
