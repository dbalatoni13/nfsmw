#ifndef ATTRIBSYS_CLASSES_CAMERAINFO_H
#define ATTRIBSYS_CLASSES_CAMERAINFO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

// TODO move?
// total size: 0x24
struct POV {
    short Type;           // offset 0x0, size 0x2
    unsigned short Angle; // offset 0x2, size 0x2
    float Lag;            // offset 0x4, size 0x4
    float Height;         // offset 0x8, size 0x4
    float LatOffset;      // offset 0xC, size 0x4
    unsigned short Fov;   // offset 0x10, size 0x2
    float Stiffness;      // offset 0x14, size 0x4
    short AllowTilting;   // offset 0x18, size 0x2
    float pad4[2];        // offset 0x1C, size 0x8
};

namespace Attrib {
namespace Gen {

struct camerainfo : Instance {
    struct _LayoutStruct {
        Private _Array_STIFFNESS;  // offset 0x0, size 0x8
        float STIFFNESS[2];        // offset 0x8, size 0x8
        Private _Array_LATOFFSET;  // offset 0x10, size 0x8
        float LATOFFSET[2];        // offset 0x18, size 0x8
        Private _Array_LAG;        // offset 0x20, size 0x8
        float LAG[2];              // offset 0x28, size 0x8
        Private _Array_FOV;        // offset 0x30, size 0x8
        float FOV[2];              // offset 0x38, size 0x8
        Private _Array_HEIGHT;     // offset 0x40, size 0x8
        float HEIGHT[2];           // offset 0x48, size 0x8
        Private _Array_ANGLE;      // offset 0x50, size 0x8
        float ANGLE[2];            // offset 0x58, size 0x8
        char CollectionName[4];    // offset 0x60, size 0x4
        Private _Array_TILTING;    // offset 0x64, size 0x8
        bool TILTING[2];           // offset 0x6c, size 0x2
        Private _Array_SELECTABLE; // offset 0x6e, size 0x8
        bool SELECTABLE[2];        // offset 0x76, size 0x2
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "camerainfo");
    }

    camerainfo(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    camerainfo(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~camerainfo() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0x93c171e4;
    }

    const float &STIFFNESS(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_STIFFNESS.GetLength()) {
            return lp->STIFFNESS[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_STIFFNESS() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_STIFFNESS.GetLength();
    }

    const float &LATOFFSET(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_LATOFFSET.GetLength()) {
            return lp->LATOFFSET[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_LATOFFSET() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_LATOFFSET.GetLength();
    }

    const float &LAG(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_LAG.GetLength()) {
            return lp->LAG[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_LAG() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_LAG.GetLength();
    }

    const float &FOV(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_FOV.GetLength()) {
            return lp->FOV[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_FOV() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_FOV.GetLength();
    }

    const float &HEIGHT(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_HEIGHT.GetLength()) {
            return lp->HEIGHT[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_HEIGHT() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_HEIGHT.GetLength();
    }

    const float &ANGLE(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_ANGLE.GetLength()) {
            return lp->ANGLE[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_ANGLE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_ANGLE.GetLength();
    }

    const char *CollectionName() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
    }

    const bool &TILTING(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_TILTING.GetLength()) {
            return lp->TILTING[index];
        } else {
            return *reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
    }

    unsigned int Num_TILTING() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_TILTING.GetLength();
    }

    const bool &SELECTABLE(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_SELECTABLE.GetLength()) {
            return lp->SELECTABLE[index];
        } else {
            return *reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
    }

    unsigned int Num_SELECTABLE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_SELECTABLE.GetLength();
    }
};

} // namespace Gen
} // namespace Attrib

#endif
