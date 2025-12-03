#ifndef ATTRIBSYS_CLASSES_ECAR_H
#define ATTRIBSYS_CLASSES_ECAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

// TODO move?
// total size: 0xC
struct CarBodyMotion {
    float DegPerG;   // offset 0x0, size 0x4
    float MaxGs;     // offset 0x4, size 0x4
    float DegPerSec; // offset 0x8, size 0x4
};

namespace Attrib {
namespace Gen {

struct ecar : Instance {
    struct _LayoutStruct {
        Private _Array_TireOffsets;              // offset 0x0, size 0x8
        UMath::Vector4 TireOffsets[4];           // offset 0x8, size 0x40
        Private _Array_TireSkidWidthKitScale;    // offset 0x50, size 0x8
        UMath::Vector2 TireSkidWidthKitScale[7]; // offset 0x58, size 0x38
        Private _Array_SkidFX;                   // offset 0x90, size 0x8
        float SkidFX[2];                         // offset 0x98, size 0x8
        CarBodyMotion BodyRoll;                  // offset 0xa0, size 0xc
        CarBodyMotion BodySquat;                 // offset 0xac, size 0xc
        CarBodyMotion BodyDive;                  // offset 0xb8, size 0xc
        Private _Array_SlipFX;                   // offset 0xc4, size 0x8
        float SlipFX[2];                         // offset 0xcc, size 0x8
        Private _Array_TireSkidWidth;            // offset 0xd4, size 0x8
        float TireSkidWidth[4];                  // offset 0xdc, size 0x10
        char CollectionName[4];                  // offset 0xec, size 0x4
        float CamberFront;                       // offset 0xf0, size 0x4
        float ReflectionOffset;                  // offset 0xf4, size 0x4
        float CamberRear;                        // offset 0xf8, size 0x4
        float RideHeight;                        // offset 0xfc, size 0x4
        Private _Array_KitWheelOffsetRear;       // offset 0x100, size 0x8
        unsigned char KitWheelOffsetRear[6];     // offset 0x108, size 0x6
        Private _Array_KitWheelOffsetFront;      // offset 0x10e, size 0x8
        unsigned char KitWheelOffsetFront[6];    // offset 0x116, size 0x6
        char WheelSpokeCount;                    // offset 0x11c, size 0x1
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "ecar");
    }

    ecar(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ecar(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~ecar() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    static Key ClassKey() {
        return 0xa5b543b7;
    }

    const RefSpec &CameraInfo_Close(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x0c2da793, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const RefSpec &CameraInfo_SuperFar(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x10204a90, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const float &RoadNoise(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x13eb7da2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const RefSpec &LIGHT_COPRED(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x16437af7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const RefSpec &LIGHT_COPWHITE(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x1ee00adf, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const float &WheelWell(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x24cb15ea, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &ShiftAngle(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x24e25513, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const RefSpec &DamageEffect(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x30b2997b, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const float &WheelHopScale(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x44e52948, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const RefSpec &CameraInfo_Drift(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x4b675dc8, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const float &EngineRev(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x564773be, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const RefSpec &CameraInfo_Bumper(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x585517f3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const UMath::Vector4 &TireOffsets(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_TireOffsets.GetLength()) {
            return lp->TireOffsets[index];
        } else {
            return *reinterpret_cast<const UMath::Vector4 *>(DefaultDataArea(sizeof(UMath::Vector4)));
        }
    }

    unsigned int Num_TireOffsets() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_TireOffsets.GetLength();
    }

    const RefSpec &NOSEffect(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x60cec115, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const float &SteerSpeed(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x79356463, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &EngineVibrationFreq(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x80e657ff, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const RefSpec &LIGHT_COPBLUE(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0x82094cc9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const float &FECompressions(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x8d5beb72, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    unsigned int Num_FECompressions() const {
        return this->Get(0x8d5beb72).GetLength();
    }

    const float &MaxTireSteer(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xa9633fde, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &ExtraRearTireOffset(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xb1304fde, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const RefSpec &MissShiftEffect(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0xb699b7be, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const RefSpec &CameraInfo_Far(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0xccf03cb3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const RefSpec &CameraInfo_Hood(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0xd74c1435, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const RefSpec &CameraInfo_Pursuit(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0xd76a6fad, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const bool &IsSkinned(unsigned int index) const {
        const bool *resultptr = reinterpret_cast<const bool *>(this->GetAttributePointer(0xd9102c65, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const RefSpec &EngineBlownEffect(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0xd9cca9a3, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const float &ShiftSpeed(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xda27cd35, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &EngineRevAngle(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xe2c63383, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &EngineVibrationMin(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xe91f59bd, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &ExtraPitch(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xe95257c2, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &FrontCamber(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xeed9ca5d, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const RefSpec &DeathEffect(unsigned int index) const {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(this->GetAttributePointer(0xf7b59fc7, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const RefSpec *>(DefaultDataArea(sizeof(RefSpec)));
        }
        return *resultptr;
    }

    const float &EngineVibrationMax(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xf947fe58, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const float &EngineRevSpeed(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xfaad3e61, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    const UMath::Vector2 &TireSkidWidthKitScale(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_TireSkidWidthKitScale.GetLength()) {
            return lp->TireSkidWidthKitScale[index];
        } else {
            return *reinterpret_cast<const UMath::Vector2 *>(DefaultDataArea(sizeof(UMath::Vector2)));
        }
    }

    unsigned int Num_TireSkidWidthKitScale() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_TireSkidWidthKitScale.GetLength();
    }

    const float &SkidFX(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_SkidFX.GetLength()) {
            return lp->SkidFX[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_SkidFX() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_SkidFX.GetLength();
    }

    const CarBodyMotion &BodyRoll() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->BodyRoll;
    }

    const CarBodyMotion &BodySquat() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->BodySquat;
    }

    const CarBodyMotion &BodyDive() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->BodyDive;
    }

    const float &SlipFX(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_SlipFX.GetLength()) {
            return lp->SlipFX[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_SlipFX() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_SlipFX.GetLength();
    }

    const float &TireSkidWidth(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_TireSkidWidth.GetLength()) {
            return lp->TireSkidWidth[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
    }

    unsigned int Num_TireSkidWidth() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_TireSkidWidth.GetLength();
    }

    const char *CollectionName() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
    }

    const float &CamberFront() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CamberFront;
    }

    const float &ReflectionOffset() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->ReflectionOffset;
    }

    const float &CamberRear() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CamberRear;
    }

    const float &RideHeight() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->RideHeight;
    }

    const unsigned char &KitWheelOffsetRear(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_KitWheelOffsetRear.GetLength()) {
            return lp->KitWheelOffsetRear[index];
        } else {
            return *reinterpret_cast<const unsigned char *>(DefaultDataArea(sizeof(unsigned char)));
        }
    }

    unsigned int Num_KitWheelOffsetRear() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_KitWheelOffsetRear.GetLength();
    }

    const unsigned char &KitWheelOffsetFront(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_KitWheelOffsetFront.GetLength()) {
            return lp->KitWheelOffsetFront[index];
        } else {
            return *reinterpret_cast<const unsigned char *>(DefaultDataArea(sizeof(unsigned char)));
        }
    }

    unsigned int Num_KitWheelOffsetFront() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_KitWheelOffsetFront.GetLength();
    }

    const char &WheelSpokeCount() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->WheelSpokeCount;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
