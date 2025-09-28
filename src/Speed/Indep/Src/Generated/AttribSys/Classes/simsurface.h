#ifndef ATTRIBSYS_CLASSES_SIMSURFACE_H
#define ATTRIBSYS_CLASSES_SIMSURFACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

// TODO move?
struct TireEffectRecord {
    uint32_t TireCondition;
    uint32_t EmitterClass;
    uint32_t EmitterCollection;
    float MinSpeed;
    float MaxSpeed;
};

struct RoadNoiseRecord {
    float Frequency;
    float Amplitude;
    float MinSpeed;
    float MaxSpeed;
};

struct FFBWaveRecord {
    // total size: 0x20
    float Frequency_A; // offset 0x0, size 0x4
    float Amplitude_A; // offset 0x4, size 0x4
    float Offset_A;    // offset 0x8, size 0x4
    float Threshold_A; // offset 0xC, size 0x4
    float Frequency_B; // offset 0x10, size 0x4
    float Amplitude_B; // offset 0x14, size 0x4
    float Offset_B;    // offset 0x18, size 0x4
    float Threshold_B; // offset 0x1C, size 0x4

    bool Sample(float x, float &frequency, float &amplitude, float &offset) const {
        // TODO
    }
};

namespace Attrib {
namespace Gen {

struct simsurface : Instance {
    struct _LayoutStruct {
        Private _Array_TireDriveEffects;      // offset 0x0, size 0x8
        TireEffectRecord TireDriveEffects[3]; // offset 0x8, size 0x3c
        Private _Array_TireSlideEffects;      // offset 0x44, size 0x8
        TireEffectRecord TireSlideEffects[3]; // offset 0x4c, size 0x3c
        Private _Array_TireSlipEffects;       // offset 0x88, size 0x8
        TireEffectRecord TireSlipEffects[3];  // offset 0x90, size 0x3c
        RoadNoiseRecord RenderNoise;          // offset 0xcc, size 0x10
        char CollectionName[4];               // offset 0xdc, size 0x4
        float GROUND_FRICTION;                // offset 0xe0, size 0x4
        float ROLLING_RESISTANCE;             // offset 0xe4, size 0x4
        float WORLD_FRICTION;                 // offset 0xe8, size 0x4
        float DRIVE_GRIP;                     // offset 0xec, size 0x4
        float LATERAL_GRIP;                   // offset 0xf0, size 0x4
        float STICK;                          // offset 0xf4, size 0x4
        unsigned short WheelEffectFrequency;  // offset 0xf8, size 0x2
        unsigned char WheelEffectIntensity;   // offset 0xfa, size 0x1
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "simsurface");
    }

    simsurface(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~simsurface() {}

    const FFBWaveRecord &FFB_SKID(unsigned int index) const {
        const FFBWaveRecord *resultptr = reinterpret_cast<const FFBWaveRecord *>(this->GetAttributePointer(0x0c149044, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const FFBWaveRecord *>(DefaultDataArea(sizeof(FFBWaveRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FFB_SKID() const {
        return this->Get(0x0c149044).GetLength();
    }

    // TODO
    // const FXROADNOISE_LOOP &Aud_Roadnoise_LOOP(unsigned int index) const {
    //     const FXROADNOISE_LOOP *resultptr = reinterpret_cast<const FXROADNOISE_LOOP *>(this->GetAttributePointer(0x2907c135, index));
    //     if (!resultptr) {
    //         resultptr = reinterpret_cast<const FXROADNOISE_LOOP *>(DefaultDataArea(sizeof(FXROADNOISE_LOOP)));
    //     }
    //     return *resultptr;
    // }

    // TODO
    // const SurfaceEffectType &WheelSurfaceEffect(unsigned int index) const {
    //     const SurfaceEffectType *resultptr = reinterpret_cast<const SurfaceEffectType *>(this->GetAttributePointer(0x46226745, index));
    //     if (!resultptr) {
    //         resultptr = reinterpret_cast<const SurfaceEffectType *>(DefaultDataArea(sizeof(SurfaceEffectType)));
    //     }
    //     return *resultptr;
    // }

    const UMath::Vector4 &DEBUG_COLOUR(unsigned int index) const {
        const UMath::Vector4 *resultptr = reinterpret_cast<const UMath::Vector4 *>(this->GetAttributePointer(0x740d3125, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector4 *>(DefaultDataArea(sizeof(UMath::Vector4)));
        }
        return *resultptr;
    }

    const FFBWaveRecord &FFB_SLIP(unsigned int index) const {
        const FFBWaveRecord *resultptr = reinterpret_cast<const FFBWaveRecord *>(this->GetAttributePointer(0x8fd11d27, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const FFBWaveRecord *>(DefaultDataArea(sizeof(FFBWaveRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FFB_SLIP() const {
        return this->Get(0x8fd11d27).GetLength();
    }

    const float &MATERIAL_STRENGTH(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0x9fca0b40, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    // TODO
    // const FXROADNOISE_TRANSITION &Aud_RoadNoise_TransON(unsigned int index) const {
    //     const FXROADNOISE_TRANSITION *resultptr = reinterpret_cast<const FXROADNOISE_TRANSITION *>(this->GetAttributePointer(0xb4c1b2cf, index));
    //     if (!resultptr) {
    //         resultptr = reinterpret_cast<const FXROADNOISE_TRANSITION *>(DefaultDataArea(sizeof(FXROADNOISE_TRANSITION)));
    //     }
    //     return *resultptr;
    // }

    const FFBWaveRecord &FFB_ROLL(unsigned int index) const {
        const FFBWaveRecord *resultptr = reinterpret_cast<const FFBWaveRecord *>(this->GetAttributePointer(0xba1297da, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const FFBWaveRecord *>(DefaultDataArea(sizeof(FFBWaveRecord)));
        }
        return *resultptr;
    }

    unsigned int Num_FFB_ROLL() const {
        return this->Get(0xba1297da).GetLength();
    }

    // const FXROADNOISE_TRANSITION &Aud_RoadNoise_TransOFF(unsigned int index) const {
    //     const FXROADNOISE_TRANSITION *resultptr = reinterpret_cast<const FXROADNOISE_TRANSITION *>(this->GetAttributePointer(0xcdf83544, index));
    //     if (!resultptr) {
    //         resultptr = reinterpret_cast<const FXROADNOISE_TRANSITION *>(DefaultDataArea(sizeof(FXROADNOISE_TRANSITION)));
    //     }
    //     return *resultptr;
    // }

    const unsigned int &RSNMCHUNK_FERESNAMES(unsigned int index) const {
        const unsigned int *resultptr = reinterpret_cast<const unsigned int *>(this->GetAttributePointer(0xd377b339, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const unsigned int *>(DefaultDataArea(sizeof(unsigned int)));
        }
        return *resultptr;
    }

    const float &CAMERA_NOISE(unsigned int index) const {
        const float *resultptr = reinterpret_cast<const float *>(this->GetAttributePointer(0xf0c9e498, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        return *resultptr;
    }

    unsigned int Num_CAMERA_NOISE() const {
        return this->Get(0xf0c9e498).GetLength();
    }

    const TireEffectRecord &TireDriveEffects(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_TireDriveEffects.GetLength()) {
            return lp->TireDriveEffects[index];
        } else {
            return *reinterpret_cast<const TireEffectRecord *>(DefaultDataArea(sizeof(TireEffectRecord)));
        }
    }

    unsigned int Num_TireDriveEffects() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_TireDriveEffects.GetLength();
    }

    const TireEffectRecord &TireSlideEffects(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_TireSlideEffects.GetLength()) {
            return lp->TireSlideEffects[index];
        } else {
            return *reinterpret_cast<const TireEffectRecord *>(DefaultDataArea(sizeof(TireEffectRecord)));
        }
    }

    unsigned int Num_TireSlideEffects() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_TireSlideEffects.GetLength();
    }

    const TireEffectRecord &TireSlipEffects(unsigned int index) const {
        const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer());
        if (index < lp->_Array_TireSlipEffects.GetLength()) {
            return lp->TireSlipEffects[index];
        } else {
            return *reinterpret_cast<const TireEffectRecord *>(DefaultDataArea(sizeof(TireEffectRecord)));
        }
    }

    unsigned int Num_TireSlipEffects() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->_Array_TireSlipEffects.GetLength();
    }

    const RoadNoiseRecord &RenderNoise() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->RenderNoise;
    }

    const char *CollectionName() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
    }

    const float &GROUND_FRICTION() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->GROUND_FRICTION;
    }

    const float &ROLLING_RESISTANCE() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->ROLLING_RESISTANCE;
    }

    const float &WORLD_FRICTION() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->WORLD_FRICTION;
    }

    const float &DRIVE_GRIP() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->DRIVE_GRIP;
    }

    const float &LATERAL_GRIP() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->LATERAL_GRIP;
    }

    const float &STICK() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->STICK;
    }

    const unsigned short &WheelEffectFrequency() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->WheelEffectFrequency;
    }

    const unsigned char &WheelEffectIntensity() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->WheelEffectIntensity;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
