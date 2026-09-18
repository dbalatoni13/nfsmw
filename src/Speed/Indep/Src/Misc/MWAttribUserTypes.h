#ifndef _mwattribusertypes_h_
#define _mwattribusertypes_h_

#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

#include "Speed/Indep/Src/AI/aireflectedtypes.h"
#include "Speed/Indep/Src/Ecstasy/GenTypes/EffectParticleAnimation.h"
#include "Speed/Indep/Src/Ecstasy/GenTypes/EffectParticleConstraint.h"
#include "Speed/Indep/Src/Generated/TexturePacks/texenumgen_particletextures.h"
#include "Speed/Indep/Src/Input/InputDefParser.h"
#include "Speed/Indep/Src/EAXSound/EAXSoundEnums.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Enums.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_EnumAttributes.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/copspeech.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/NISAudio.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/P2temp.h"
#include "Speed/Indep/Src/EAXSound/SND_GEN/STITCH_COL.h"
#include "Speed/Indep/Src/Gameplay/GReflected.h"
#include "Speed/Indep/Src/Sim/SimSurfaceTypes.h"

// Tipos que solo referencia frontend.h, y solo desde los typedef TypeOf_* y
// accesores por valor. No aparecen en ningun _LayoutStruct, asi que su forma
// no afecta a los offsets de las clases generadas.
//
// eUnlockableEntity si existe en el original: sale mangleado como
// 17eUnlockableEntity en UnlockUnlockableThing, MarkUnlockableThingSeen y
// DoesCategoryHaveNewUnlock. Sus valores todavia no se han sacado.
enum eUnlockableEntity {
    UNLOCKABLE_THING_UNKNOWN = 0,
    UNLOCKABLE_THING_CUSTOMIZE_PARTS = 1,
    UNLOCKABLE_THING_CUSTOMIZE_PERFORMANCE = 2,
    UNLOCKABLE_THING_CUSTOMIZE_VISUAL = 3,
    UNLOCKABLE_THING_PUT_TIRES = 4,
    UNLOCKABLE_THING_PUT_BRAKES = 5,
    UNLOCKABLE_THING_PUT_CHASSIS = 6,
    UNLOCKABLE_THING_PUT_TRANSMISSION = 7,
    UNLOCKABLE_THING_PUT_ENGINE = 8,
    UNLOCKABLE_THING_PUT_INDUCTION = 9,
    UNLOCKABLE_THING_PUT_NOS = 10,
    UNLOCKABLE_THING_BODY_KIT = 11,
    UNLOCKABLE_THING_SPOILERS = 12,
    UNLOCKABLE_THING_RIM_BRANDS = 13,
    UNLOCKABLE_THING_HOODS = 14,
    UNLOCKABLE_THING_ROOF_SCOOPS = 15,
    UNLOCKABLE_THING_LICENSE_PLATE = 16,
    UNLOCKABLE_THING_CUSTOM_HUD = 17,
    UNLOCKABLE_THING_WINDOW_TINT = 18,
    UNLOCKABLE_THING_PAINT_METALLIC = 19,
    UNLOCKABLE_THING_PAINT_PEARL = 20,
    UNLOCKABLE_THING_PAINT_GLOSS = 21,
    UNLOCKABLE_THING_PAINT_STOCK = 22,
    UNLOCKABLE_THING_PAINTABLE_BODY = 23,
    UNLOCKABLE_THING_PAINTABLE_RIMS = 24,
    UNLOCKABLE_THING_RIM_BRAND_5_ZIGEN = 25,
    UNLOCKABLE_THING_RIM_BRAND_ADR = 26,
    UNLOCKABLE_THING_RIM_BRAND_BBS = 27,
    UNLOCKABLE_THING_RIM_BRAND_ENKEI = 28,
    UNLOCKABLE_THING_RIM_BRAND_KONIG = 29,
    UNLOCKABLE_THING_RIM_BRAND_LOWENHART = 30,
    UNLOCKABLE_THING_RIM_BRAND_RACING_HART = 31,
    UNLOCKABLE_THING_RIM_BRAND_OZ = 32,
    UNLOCKABLE_THING_RIM_BRAND_VOLK = 33,
    UNLOCKABLE_THING_RIM_BRAND_ROJA = 34,
    UNLOCKABLE_VINYLS_GROUP_FLAME = 35,
    UNLOCKABLE_VINYLS_GROUP_TRIBAL = 36,
    UNLOCKABLE_VINYLS_GROUP_STRIPE = 37,
    UNLOCKABLE_VINYLS_GROUP_RACING_FLAG = 38,
    UNLOCKABLE_VINYLS_GROUP_NATIONAL_FLAG = 39,
    UNLOCKABLE_VINYLS_GROUP_BODY = 40,
    UNLOCKABLE_VINYLS_GROUP_UNIQUE = 41,
    UNLOCKABLE_VINYLS_GROUP_CONTEST = 42,
    UNLOCKABLE_DECAL_NUMBERS = 43,
    UNLOCKABLE_DECAL_WINDSHIELD = 44,
    UNLOCKABLE_DECAL_REAR_WINDOW = 45,
    UNLOCKABLE_DECAL_LEFT_DOOR = 46,
    UNLOCKABLE_DECAL_RIGHT_DOOR = 47,
    UNLOCKABLE_DECAL_LEFT_QP = 48,
    UNLOCKABLE_DECAL_RIGHT_QP = 49,
    UNLOCKABLE_DECAL_HOOD = 50,
    UNLOCKABLE_DECAL_SLOT_1 = 51,
    UNLOCKABLE_DECAL_SLOT_2 = 52,
    UNLOCKABLE_DECAL_SLOT_3 = 53,
    UNLOCKABLE_DECAL_SLOT_4 = 54,
    UNLOCKABLE_DECAL_SLOT_5 = 55,
    UNLOCKABLE_DECAL_SLOT_6 = 56,
    NUM_UNLOCKABLES = 57,
};

// Los dos siguientes solo viven en el esquema de atributos y no dejan simbolo,
// asi que su contenido esta sin confirmar.
typedef EA::Reflection::UInt32 type_bStringHash;

enum eFEPartUpgradeLevels {
    PART_UPGRADE_LEVEL_UNSPECIFIED = -1,
    PART_UPGRADE_LEVEL_STOCK = 0,
    PART_UPGRADE_LEVEL_1 = 1,
    PART_UPGRADE_LEVEL_2 = 2,
    PART_UPGRADE_LEVEL_3 = 3,
    PART_UPGRADE_LEVEL_4 = 4,
    PART_UPGRADE_LEVEL_5 = 5,
    PART_UPGRADE_LEVEL_6 = 6,
    PART_UPGRADE_LEVEL_UNIQUE = 7,
    NUM_PART_UPGRADE_LEVELS = 8,
};

struct FECarPartInfo {
    eFEPartUpgradeLevels Level; // offset 0x0, size 0x4, Decl: speed/indep/src/Frontend/FEReflected.hpp:296
    float Rep;                  // offset 0x4, size 0x4
    float Cost;                 // offset 0x8, size 0x4, Decl: speed/indep/src/Frontend/FEReflected.hpp:299
};

// total size: 0x8
// Decl: 24
struct DamageScaleRecord {
    float VisualScale;   // offset 0x0, size 0x4
    float HitPointScale; // offset 0x4, size 0x4
};

// total size: 0x20
// Decl: 30
struct ControllerDataRecord {
    Attrib::StringKey mDeviceID; // offset 0x0, size 0x10
    InputUpdateType mUpdateType; // offset 0x10, size 0x4
    float mLowerDZ;              // offset 0x14, size 0x4
    float mUpperDZ;              // offset 0x18, size 0x4
};

// total size: 0x20
// Decl: 44
struct EffectLinkageRecord {
    Attrib::RefSpec mSurface; // offset 0x0, size 0xC
    Attrib::RefSpec mEffect;  // offset 0xC, size 0xC
    float mMinSpeed;          // offset 0x18, size 0x4
    float mMaxSpeed;          // offset 0x1C, size 0x4

    // Decl: 50
    void mSurfaceClean() {}

    // Decl: 51
    void mEffectClean() {}
};

// total size: 0x20
// Decl: 54
struct FFBWaveRecord {
    float Frequency_A; // offset 0x0, size 0x4
    float Amplitude_A; // offset 0x4, size 0x4
    float Offset_A;    // offset 0x8, size 0x4
    float Threshold_A; // offset 0xC, size 0x4
    float Frequency_B; // offset 0x10, size 0x4
    float Amplitude_B; // offset 0x14, size 0x4
    float Offset_B;    // offset 0x18, size 0x4
    float Threshold_B; // offset 0x1C, size 0x4, Decl: 62

    // Decl: 64
    bool Sample(float x, float &frequency, float &amplitude, float &offset) const {}
};

// total size: 0xC
// Decl: 79
struct CarBodyMotion {
    float DegPerG;   // offset 0x0, size 0x4
    float MaxGs;     // offset 0x4, size 0x4
    float DegPerSec; // offset 0x8, size 0x4
};

// total size: 0x8
// Decl: 87
struct AxlePair {
    AxlePair() : Front(0.0f), Rear(0.0f) {}

    float Front; // offset 0x0, size 0x4
    float Rear;  // offset 0x4, size 0x4

    // Decl: 93
    float At(int index) const {
        return (&Front)[index];
    }
};

// total size: 0x10
// Decl: 98
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
// Decl: 113
struct JunkmanMod {
    Attrib::Key ClassKey;      // offset 0x0, size 0x4
    Attrib::Key DefinitionKey; // offset 0x4, size 0x4
    float Scale;               // offset 0x8, size 0x4
};

// total size: 0x18
// Decl: 121
struct TrafficPatternRecord {
    Attrib::RefSpec Vehicle;             // offset 0x0, size 0xC
    EA::Reflection::Float Rate;          // offset 0xC, size 0x4
    EA::Reflection::UInt32 MaxInstances; // offset 0x10, size 0x4
    EA::Reflection::UInt32 Percent;      // offset 0x14, size 0x4
};

// total size: 0x8
// Decl: 131
struct ParticleAnimationInfo {
    EffectParticleAnimation AnimType;       // offset 0x0, size 0x4
    EA::Reflection::UInt8 FPS;              // offset 0x4, size 0x1
    EA::Reflection::UInt8 RandomStartFrame; // offset 0x5, size 0x1
};

// total size: 0x14
// Decl: 139
struct TireEffectRecord {
    Attrib::RefSpec mEmitter;        // offset 0x0, size 0xC
    EA::Reflection::Float mMinSpeed; // offset 0xC, size 0x4
    EA::Reflection::Float mMaxSpeed; // offset 0x10, size 0x4
};

// total size: 0x8
// Decl: 146
struct ParticleTextureRecord {
    eTEG_ParticleTextures mEnum; // offset 0x0, size 0x4
    uint32_t mIndex;             // offset 0x4, size 0x4
};

// total size: 0x18
// Decl: 164
struct CollisionReactionRecord {
    CollisionReactionRecord() {}

    float Elasticity; // offset 0x0, size 0x4
    float RollHeight; // offset 0x4, size 0x4
    float WeightBias; // offset 0x8, size 0x4
    float MassScale;  // offset 0xC, size 0x4
    float StunSpeed;  // offset 0x10, size 0x4
    float StunTime;   // offset 0x14, size 0x4
};

#endif
