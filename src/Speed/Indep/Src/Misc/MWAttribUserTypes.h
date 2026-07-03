#ifndef _mwattribusertypes_h_
#define _mwattribusertypes_h_

#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

#include "Speed/Indep/Src/AI/aireflectedtypes.h"
#include "Speed/Indep/Src/Ecstasy/GenTypes/EffectParticleAnimation.h"
#include "Speed/Indep/Src/Frontend/FEReflected.hpp"
#include "Speed/Indep/Src/Ecstasy/GenTypes/EffectParticleConstraint.h"
#include "Speed/Indep/Src/Generated/TexturePacks/texenumgen_particletextures.h"
#include "Speed/Indep/Src/Input/InputDefParser.h"
#include "Speed/Indep/Src/EAXSound/EAXSoundEnums.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Enums.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_EnumAttributes.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/copspeech.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/NISAudio.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/STITCH_COL.h"
#include "Speed/Indep/Src/Gameplay/GReflected.h"
#include "Speed/Indep/Src/Sim/SimSurfaceTypes.h"

// total size: 0x8
struct DamageScaleRecord {
    float VisualScale;   // offset 0x0, size 0x4
    float HitPointScale; // offset 0x4, size 0x4
};

// total size: 0x20
struct ControllerDataRecord {
    Attrib::StringKey mDeviceID; // offset 0x0, size 0x10
    InputUpdateType mUpdateType; // offset 0x10, size 0x4
    float mLowerDZ;              // offset 0x14, size 0x4
    float mUpperDZ;              // offset 0x18, size 0x4

    ControllerDataRecord &operator=(const ControllerDataRecord &_ctor_arg) {}
};

// total size: 0x20
struct EffectLinkageRecord {
    Attrib::RefSpec mSurface; // offset 0x0, size 0xC
    Attrib::RefSpec mEffect;  // offset 0xC, size 0xC
    float mMinSpeed;          // offset 0x18, size 0x4
    float mMaxSpeed;          // offset 0x1C, size 0x4

    EffectLinkageRecord &operator=(const EffectLinkageRecord &_ctor_arg) {}
    EffectLinkageRecord(const EffectLinkageRecord &_ctor_arg) {}

    void mSurfaceClean() {}

    void mEffectClean() {}
};

// total size: 0x20
struct FFBWaveRecord {
    float Frequency_A; // offset 0x0, size 0x4
    float Amplitude_A; // offset 0x4, size 0x4
    float Offset_A;    // offset 0x8, size 0x4
    float Threshold_A; // offset 0xC, size 0x4
    float Frequency_B; // offset 0x10, size 0x4
    float Amplitude_B; // offset 0x14, size 0x4
    float Offset_B;    // offset 0x18, size 0x4
    float Threshold_B; // offset 0x1C, size 0x4

    bool Sample(float x, float &frequency, float &amplitude, float &offset) const {}
};

// total size: 0xC
struct CarBodyMotion {
    float DegPerG;   // offset 0x0, size 0x4
    float MaxGs;     // offset 0x4, size 0x4
    float DegPerSec; // offset 0x8, size 0x4
};

// total size: 0x8
struct AxlePair {
    float Front; // offset 0x0, size 0x4
    float Rear;  // offset 0x4, size 0x4

    float At(int index) const {
        return (&Front)[index];
    }
};

// total size: 0x10
struct RoadNoiseRecord {
    RoadNoiseRecord() : Frequency(0.0f), Amplitude(0.0f), MinSpeed(0.0f), MaxSpeed(0.0f) {}

    RoadNoiseRecord(float frequency, float amplitude, float minspeed, float maxspeed)
        : Frequency(frequency), Amplitude(amplitude), MinSpeed(minspeed), MaxSpeed(maxspeed) {}

    RoadNoiseRecord(const RoadNoiseRecord &rhs) {}

    float Frequency; // offset 0x0, size 0x4
    float Amplitude; // offset 0x4, size 0x4
    float MinSpeed;  // offset 0x8, size 0x4
    float MaxSpeed;  // offset 0xC, size 0x4
};

// total size: 0xC
struct JunkmanMod {
    Attrib::Key ClassKey;      // offset 0x0, size 0x4
    Attrib::Key DefinitionKey; // offset 0x4, size 0x4
    float Scale;               // offset 0x8, size 0x4
};

// total size: 0x18
struct TrafficPatternRecord {
    Attrib::RefSpec Vehicle;             // offset 0x0, size 0xC
    EA::Reflection::Float Rate;          // offset 0xC, size 0x4
    EA::Reflection::UInt32 MaxInstances; // offset 0x10, size 0x4
    EA::Reflection::UInt32 Percent;      // offset 0x14, size 0x4

    // TrafficPatternRecord &operator=(const TrafficPatternRecord &_ctor_arg) {}

    // TrafficPatternRecord(const TrafficPatternRecord &_ctor_arg) {}
};

// total size: 0x8
struct ParticleAnimationInfo {
    EffectParticleAnimation AnimType;       // offset 0x0, size 0x4
    EA::Reflection::UInt8 FPS;              // offset 0x4, size 0x1
    EA::Reflection::UInt8 RandomStartFrame; // offset 0x5, size 0x1
};

// total size: 0x14
struct TireEffectRecord {
    Attrib::RefSpec mEmitter;        // offset 0x0, size 0xC
    EA::Reflection::Float mMinSpeed; // offset 0xC, size 0x4
    EA::Reflection::Float mMaxSpeed; // offset 0x10, size 0x4

    TireEffectRecord &operator=(const TireEffectRecord &_ctor_arg) {}

    TireEffectRecord(const TireEffectRecord &_ctor_arg) {}
};

// total size: 0x8
struct ParticleTextureRecord {
    eTEG_ParticleTextures mEnum; // offset 0x0, size 0x4
    uint32_t mIndex;             // offset 0x4, size 0x4
};

// total size: 0x18
struct CollisionReactionRecord {
    float Elasticity; // offset 0x0, size 0x4
    float RollHeight; // offset 0x4, size 0x4
    float WeightBias; // offset 0x8, size 0x4
    float MassScale;  // offset 0xC, size 0x4
    float StunSpeed;  // offset 0x10, size 0x4
    float StunTime;   // offset 0x14, size 0x4

    CollisionReactionRecord() {}
};

#endif
