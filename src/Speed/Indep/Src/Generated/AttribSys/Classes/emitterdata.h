#ifndef ATTRIBSYS_CLASSES_EMITTERDATA_H
#define ATTRIBSYS_CLASSES_EMITTERDATA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// TODO where to?
enum EffectParticleAnimation {
    ANIMATE_PARTICLE_16x16 = 16,
    ANIMATE_PARTICLE_8x8 = 8,
    ANIMATE_PARTICLE_4x4 = 4,
    ANIMATE_PARTICLE_2x2 = 2,
    ANIMATE_PARTICLE_NONE = 0,
};

enum eTEG_ParticleTextures {
    FX_GLASS01_BLEND_ANIM = -1969148833,
    FX_MARKER_BG = -851099962,
    FX_WORLD_CHOP_SHOP = -2064169978,
    FX_WORLD_SAFEHOUSE = -2032504090,
    FX_WORLD_PURSUITBREAKER = -1879965509,
    FX_WORLD_CARLOT = 163126824,
    FX_DEBRIS01_BLEND_ANIM = 120945470,
    FX_GRASS01_BLEND_ANIM = -71534491,
    FX_RIVAL_RACE = -293691724,
    FX_MODE_ICON_SPRINT = -1643960056,
    FX_MODE_ICON_LAP_KNOCKOUT = 1306638962,
    FX_MODE_ICON_DRAG = 1362561478,
    FX_MODE_ICON_CIRCUIT = 2121085947,
    FX_MILESTONE_TOLLBOOTH = 404183618,
    FX_MILESTONE_SPEEDTRAP = 1691873715,
    FX_HIDE_CAR_ICON = 230982803,
    FX_LEAF01_BLEND_ANIM = 1469650045,
    FX_BIRD01_BLEND_ANIM = 757640838,
    FX_SPARK01_ADDITIVE_ANIM = -364005141,
    FX_RINGWAVEA = 968934912,
    FX_WATER03_BLEND = -169175322,
    FX_WATER02_BLEND = -1460643291,
    FX_WATER01_BLEND = 1542856036,
    FX_SPARK05_ADDITIVE = -712556789,
    FX_SPARK04_ADDITIVE = -780358166,
    FX_SPARK03_ADDITIVE = -848159543,
    FX_SPARK02_ADDITIVE = -915960920,
    FX_SPARK01_ADDITIVE = -983762297,
    FX_SMK06_BLEND = 59032177,
    FX_SMK05_BLEND = -1232435792,
    FX_SMK04_BLEND = 1771063535,
    FX_SMK04_ADDITIVE = -1865088908,
    FX_SMK03_BLEND = 479595566,
    FX_SMK02_BLEND = -811872403,
    FX_SMK01_BLEND = -2103340372,
    FX_LEAF01_BLEND = -1320529191,
    FX_GLOW02_ADDITIVE = -2055114144,
    FX_GLOW01_ADDITIVE = -2122915521,
    FX_FIRE03_ADDITIVE = -1316719474,
    FX_FIRE02_BLEND = -1103468920,
    FX_FIRE02_ADDITIVE = -1384520851,
    FX_FIRE01_BLEND = 1900030407,
    FX_FIRE01_ADDITIVE = -1452322228,
    FX_DEBRIS02_BLEND = -1637582405,
    FX_BEAN_BLEND = -1921749642,
    FX_BEAN_ADDITIVE = -297751653,
};

struct ParticleAnimationInfo {
    // total size: 0x8
    EffectParticleAnimation AnimType; // offset 0x0, size 0x4
    unsigned char FPS;                // offset 0x4, size 0x1
    unsigned char RandomStartFrame;   // offset 0x5, size 0x1
};

struct ParticleTextureRecord {
    // total size: 0x8
    eTEG_ParticleTextures mEnum; // offset 0x0, size 0x4
    unsigned int mIndex;         // offset 0x4, size 0x4
};

enum EffectParticleConstraint {
    CONSTRAIN_PARTICLE_CAMERA = 8,
    CONSTRAIN_PARTICLE_YZ_AXIS = 3,
    CONSTRAIN_PARTICLE_XZ_AXIS = 5,
    CONSTRAIN_PARTICLE_XY_AXIS = 6,
    CONSTRAIN_PARTICLE_NONE = 0,
};

namespace Attrib {
namespace Gen {

struct emitterdata : Instance {
    struct _LayoutStruct {
        UMath::Vector4 VelocityStart;            // offset 0x0, size 0x10
        UMath::Vector4 AccelDelta;               // offset 0x10, size 0x10
        UMath::Vector4 RelativeAngle;            // offset 0x20, size 0x10
        UMath::Vector4 VelocityDelta;            // offset 0x30, size 0x10
        UMath::Vector4 Size;                     // offset 0x40, size 0x10
        UMath::Vector4 KeyPositions;             // offset 0x50, size 0x10
        UMath::Vector4 VolumeExtent;             // offset 0x60, size 0x10
        UMath::Vector4 AccelStart;               // offset 0x70, size 0x10
        UMath::Vector4 VolumeCenter;             // offset 0x80, size 0x10
        ParticleAnimationInfo TextureAnimation;  // offset 0x90, size 0x8
        ParticleTextureRecord Texture;           // offset 0x98, size 0x8
        char CollectionName[4];                  // offset 0xa0, size 0x4
        float InitialAngleRange;                 // offset 0xa4, size 0x4
        float Drag;                              // offset 0xa8, size 0x4
        unsigned int Color1;                     // offset 0xac, size 0x4
        unsigned int Color3;                     // offset 0xb0, size 0x4
        unsigned int Color4;                     // offset 0xb4, size 0x4
        float OnCycle;                           // offset 0xb8, size 0x4
        float Life;                              // offset 0xbc, size 0x4
        float NumParticlesVariance;              // offset 0xc0, size 0x4
        float NumParticles;                      // offset 0xc4, size 0x4
        EffectParticleConstraint AxisConstraint; // offset 0xc8, size 0x4
        float OnCycleVariance;                   // offset 0xcc, size 0x4
        float OffCycleVariance;                  // offset 0xd0, size 0x4
        float FarClip;                           // offset 0xd4, size 0x4
        float OffCycle;                          // offset 0xd8, size 0x4
        float RotationVariance;                  // offset 0xdc, size 0x4
        int MotionLive;                          // offset 0xe0, size 0x4
        unsigned int Color2;                     // offset 0xe4, size 0x4
        float LifeVariance;                      // offset 0xe8, size 0x4
        float MotionInherit;                     // offset 0xec, size 0x4
        float Speed;                             // offset 0xf0, size 0x4
        float StartDelay;                        // offset 0xf4, size 0x4
        float SpreadAngle;                       // offset 0xf8, size 0x4
        int RandomRotationDirection;             // offset 0xfc, size 0x4
        float SpeedVariance;                     // offset 0x100, size 0x4
        float MotionInheritVariance;             // offset 0x104, size 0x4
        float Gravity;                           // offset 0x108, size 0x4
        int SpreadAsDisc;                        // offset 0x10c, size 0x4
        char AlphaToKillAt;                      // offset 0x110, size 0x1
        bool NoKillAtAlpha;                      // offset 0x114, size 0x1
        bool IsOneShot;                          // offset 0x118, size 0x1
        bool StartDelayRandomVariance;           // offset 0x11C, size 0x1
        bool EliminateUnnecessaryRandomness;     // offset 0x120, size 0x1
    };

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "emitterdata");
    }

    emitterdata(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        this->SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~emitterdata() {}

    const UMath::Vector4 &VelocityStart() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->VelocityStart;
    }

    const UMath::Vector4 &AccelDelta() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AccelDelta;
    }

    const UMath::Vector4 &RelativeAngle() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->RelativeAngle;
    }

    const UMath::Vector4 &VelocityDelta() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->VelocityDelta;
    }

    const UMath::Vector4 &Size() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Size;
    }

    const UMath::Vector4 &KeyPositions() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->KeyPositions;
    }

    const UMath::Vector4 &VolumeExtent() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->VolumeExtent;
    }

    const UMath::Vector4 &AccelStart() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AccelStart;
    }

    const UMath::Vector4 &VolumeCenter() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->VolumeCenter;
    }

    const ParticleAnimationInfo &TextureAnimation() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->TextureAnimation;
    }

    const ParticleTextureRecord &Texture() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Texture;
    }

    const char *CollectionName() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->CollectionName;
    }

    const float &InitialAngleRange() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->InitialAngleRange;
    }

    const float &Drag() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Drag;
    }

    const unsigned int &Color1() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Color1;
    }

    const unsigned int &Color3() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Color3;
    }

    const unsigned int &Color4() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Color4;
    }

    const float &OnCycle() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->OnCycle;
    }

    const float &Life() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Life;
    }

    const float &NumParticlesVariance() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->NumParticlesVariance;
    }

    const float &NumParticles() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->NumParticles;
    }

    const EffectParticleConstraint &AxisConstraint() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AxisConstraint;
    }

    const float &OnCycleVariance() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->OnCycleVariance;
    }

    const float &OffCycleVariance() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->OffCycleVariance;
    }

    const float &FarClip() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->FarClip;
    }

    const float &OffCycle() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->OffCycle;
    }

    const float &RotationVariance() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->RotationVariance;
    }

    const int &MotionLive() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MotionLive;
    }

    const unsigned int &Color2() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Color2;
    }

    const float &LifeVariance() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->LifeVariance;
    }

    const float &MotionInherit() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MotionInherit;
    }

    const float &Speed() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Speed;
    }

    const float &StartDelay() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->StartDelay;
    }

    const float &SpreadAngle() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SpreadAngle;
    }

    const int &RandomRotationDirection() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->RandomRotationDirection;
    }

    const float &SpeedVariance() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SpeedVariance;
    }

    const float &MotionInheritVariance() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->MotionInheritVariance;
    }

    const float &Gravity() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->Gravity;
    }

    const int &SpreadAsDisc() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->SpreadAsDisc;
    }

    const char &AlphaToKillAt() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->AlphaToKillAt;
    }

    const bool &NoKillAtAlpha() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->NoKillAtAlpha;
    }

    const bool &IsOneShot() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->IsOneShot;
    }

    const bool &StartDelayRandomVariance() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->StartDelayRandomVariance;
    }

    const bool &EliminateUnnecessaryRandomness() const {
        return reinterpret_cast<_LayoutStruct *>(this->GetLayoutPointer())->EliminateUnnecessaryRandomness;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
