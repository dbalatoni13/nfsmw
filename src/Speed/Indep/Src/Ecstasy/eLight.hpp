#ifndef ELIGHT_HPP
#define ELIGHT_HPP

// #define CURRENT_ELIGHT_VERSION 4
// #define eLNAMESIZE 32
// #define eLFNAMESIZE 32
// #define MAX_LIGHTS_PER_SOLID 8
// #define CURRENT_ELIGHTFLARE_VERSION 3
// #define CURRENT_ELIGHTMATERIAL_VERSION 3
// #define ELIGHTMATERIAL_SLOTPOOL_SIZE (180 + 150)

#ifdef EA_PLATFORM_GAMECUBE
#include "Speed/GameCube/Src/Ecstasy/eLightPlat.hpp"
#elif defined(EA_PLATFORM_XENON)
#include "Speed/Xenon/Src/Ecstasy/eLightPlat.hpp"
#elif defined(EA_PLATFORM_PLAYSTATION2)
#include "Speed/PSX2/Src/Ecstasy/eLightPlat.hpp"
#elif defined(EA_PLATFORM_WIN32)
#include "Speed/PC/Src/Ecstasy/eLightPlat.hpp"
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/Misc/VolumeTree.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

enum eLightReflexionType {
    REF_NONE = 0,
    REF_TOPO = 1,
    REF_FAST = 2,
};

enum flareType {
    FLARE_NORM = 0,
    FLARE_ENV = 1,
    FLARE_REFLECT = 2,
    FLARE_CAT_P1 = 3,
    FLARE_CAT_P2 = 4,
};

enum eLightFlareType {
    ELF_CAR_HEADLIGHT = 0,
    ELF_CAR_BRAKELIGHT = 1,
    ELF_CAR_TRAFFIC_BRAKELIGHT = 2,
    ELF_CAR_REVERSELIGHT = 3,
    ELF_CAR_FOGLIGHT = 4,
    ELF_CAR_COPLIGHTRED = 5,
    ELF_CAR_COPLIGHTBLUE = 6,
    ELF_CAR_COPLIGHTWHITE = 7,
    ELF_CAR_COPHEADLIGHTRIGHT = 8,
    ELF_CAR_COPHEADLIGHTLEFT = 9,
    ELF_CAR_COPLIGHTBRIGHTRED = 10,
    ELF_CAR_COPLIGHTBRIGHTBLUE = 11,
    ELF_CAR_COPLIGHTORANGE = 12,
    ELF_LAMPPOST = 13,
    ELF_CATSEYE_ORANGE = 14,
    ELF_CATSEYE_RED = 15,
    ELF_CATSEYE_BLUE = 16,
    ELF_BLINKING_AMBER = 17,
    ELF_BLINKING_RED = 18,
    ELF_BLINKING_GREEN = 19,
    ELF_HAND_FLARE = 20,
    ELF_SUN_FLARE = 21,
    ELF_1ST_COPLIGHT = 5,
    ELF_LST_COPLIGHT = 12,
};

enum eLightFlareTextures {
    ESLF_TEX_HEADLIGHT_INNER = 0,
    ESLF_TEX_HEADLIGHT_OUTER = 1,
    ESLF_TEX_HEADLIGHT_GLOW = 2,
    ESLF_NUM_TEXTURES = 3,
};

enum eLightFlareFlags {
    ELF_BIDIRECTIONAL = 1,
    ELF_N_DIRECTIONAL = 2,
    ELF_UNI_DIRECTIONAL = 4,
};

// total size: 0x2C
struct LightFlareParameters {
    float MinSize;         // offset 0x0, size 0x4
    float MaxSize;         // offset 0x4, size 0x4
    float Colour[4];       // offset 0x8, size 0x10
    float Power;           // offset 0x18, size 0x4
    float ZBias;           // offset 0x1C, size 0x4
    float MinScale;        // offset 0x20, size 0x4
    float MaxScale;        // offset 0x24, size 0x4
    unsigned int Texture;  // offset 0x28, size 0x4
};

extern LightFlareParameters SingleLightFlareParameters[22];
extern int LightFlareParametersNeedUpdating;

// total size: 0x30
class eLightFlare : public bTNode<eLightFlare> {
  public:
    USE_FASTALLOC(eLightFlare);

    uint32 NameHash;            // offset 0x8, size 0x4
    uint32 ColourTint;          // offset 0xC, size 0x4
    float PositionX;            // offset 0x10, size 0x4
    float PositionY;            // offset 0x14, size 0x4
    float PositionZ;            // offset 0x18, size 0x4
    float ReflectPosZ;          // offset 0x1C, size 0x4
    float DirectionX;           // offset 0x20, size 0x4
    float DirectionY;           // offset 0x24, size 0x4
    float DirectionZ;           // offset 0x28, size 0x4
    int8 Type;                  // offset 0x2C, size 0x1
    int8 Flags;                 // offset 0x2D, size 0x1
    int16 ScenerySectionNumber; // offset 0x2E, size 0x2

    bVector3 *GetPosition() {
        return (bVector3 *)&this->PositionX;
    }

    bVector3 *GetDirection() {
        return (bVector3 *)&this->DirectionX;
    }

    void EndianSwap() {
        unsigned int temp_color;

        bPlatEndianSwap(&this->NameHash);
        bPlatEndianSwap(&this->Type);
        bPlatEndianSwap(&this->Flags);
        bPlatEndianSwap(&this->PositionX);
        bPlatEndianSwap(&this->PositionY);
        bPlatEndianSwap(&this->PositionZ);
        bPlatEndianSwap(&this->ReflectPosZ);
        bPlatEndianSwap(&this->DirectionX);
        bPlatEndianSwap(&this->DirectionY);
        bPlatEndianSwap(&this->DirectionZ);
        bPlatEndianSwap(&this->ColourTint);
        bPlatEndianSwap(&this->ScenerySectionNumber);

        temp_color = PlatConvertColor(this->ColourTint);
        this->ColourTint = temp_color;
    }
};

// total size: 0x60
class eLightFlarePackHeader : public bTNode<eLightFlarePackHeader> {
  public:
    uint32 Version;                     // offset 0x8, size 0x4
    uint32 NameHash;                    // offset 0xC, size 0x4
    char Name[32];                      // offset 0x10, size 0x20
    bVector3 BBoxMin;                   // offset 0x30, size 0x10
    bVector3 BBoxMax;                   // offset 0x40, size 0x10
    uint16 NumLightFlares;              // offset 0x50, size 0x2
    int8 EndianSwapped;                 // offset 0x52, size 0x1
    int8 Pad;                           // offset 0x53, size 0x1
    uint32 ScenerySectionNumber;        // offset 0x54, size 0x4
    bTList<eLightFlare> LightFlareList; // offset 0x58, size 0x8

    void EndianSwap() {
        if (!this->EndianSwapped) {
            this->EndianSwapped = 1;

            bPlatEndianSwap(&this->Version);
            bPlatEndianSwap(&this->NameHash);
            bPlatEndianSwap(&this->BBoxMin);
            bPlatEndianSwap(&this->BBoxMax);
            bPlatEndianSwap(&this->NumLightFlares);
            bPlatEndianSwap(&this->ScenerySectionNumber);
        }
    }
};

// total size: 0x78
class eLightMaterialData {
  public:
    float DiffuseMinScale;  // offset 0x0, size 0x4
    float DiffuseMinR;      // offset 0x4, size 0x4
    float DiffuseMinG;      // offset 0x8, size 0x4
    float DiffuseMinB;      // offset 0xC, size 0x4
    float DiffuseMaxScale;  // offset 0x10, size 0x4
    float DiffuseMaxR;      // offset 0x14, size 0x4
    float DiffuseMaxG;      // offset 0x18, size 0x4
    float DiffuseMaxB;      // offset 0x1C, size 0x4
    float DiffuseMinA;      // offset 0x20, size 0x4
    float DiffuseMaxA;      // offset 0x24, size 0x4
    float SpecularPower;    // offset 0x28, size 0x4
    float SpecularMinScale; // offset 0x2C, size 0x4
    float SpecularMinR;     // offset 0x30, size 0x4
    float SpecularMinG;     // offset 0x34, size 0x4
    float SpecularMinB;     // offset 0x38, size 0x4
    float SpecularMaxScale; // offset 0x3C, size 0x4
    float SpecularMaxR;     // offset 0x40, size 0x4
    float SpecularMaxG;     // offset 0x44, size 0x4
    float SpecularMaxB;     // offset 0x48, size 0x4
    float EnvmapPower;      // offset 0x4C, size 0x4
    float EnvmapMinScale;   // offset 0x50, size 0x4
    float EnvmapMinR;       // offset 0x54, size 0x4
    float EnvmapMinG;       // offset 0x58, size 0x4
    float EnvmapMinB;       // offset 0x5C, size 0x4
    float EnvmapMaxScale;   // offset 0x60, size 0x4
    float EnvmapMaxR;       // offset 0x64, size 0x4
    float EnvmapMaxG;       // offset 0x68, size 0x4
    float EnvmapMaxB;       // offset 0x6C, size 0x4
    float MetallicScale;    // offset 0x70, size 0x4
    float SpecularHotSpot;  // offset 0x74, size 0x4
};

// total size: 0xA8
class eLightMaterial : public eLightMaterialPlatInterface, public bTNode<eLightMaterial> {
  public:
    uint32 NameHash;           // offset 0xC, size 0x4
    uint32 Version;            // offset 0x10, size 0x4
    char Name[28];             // offset 0x14, size 0x1C
    eLightMaterialData feData; // offset 0x30, size 0x78

    void BuildData();

    void EndianSwap() {
        bPlatEndianSwap(&this->NameHash);
        bPlatEndianSwap(&this->Version);

        {
            eLightMaterialData *light_material_data = &this->feData;

            bPlatEndianSwap(&light_material_data->DiffuseMinScale);
            bPlatEndianSwap(&light_material_data->DiffuseMinR);
            bPlatEndianSwap(&light_material_data->DiffuseMinG);
            bPlatEndianSwap(&light_material_data->DiffuseMinB);
            bPlatEndianSwap(&light_material_data->DiffuseMaxScale);
            bPlatEndianSwap(&light_material_data->DiffuseMaxR);
            bPlatEndianSwap(&light_material_data->DiffuseMaxG);
            bPlatEndianSwap(&light_material_data->DiffuseMaxB);
            bPlatEndianSwap(&light_material_data->DiffuseMinA);
            bPlatEndianSwap(&light_material_data->DiffuseMaxA);
            bPlatEndianSwap(&light_material_data->SpecularPower);
            bPlatEndianSwap(&light_material_data->SpecularMinScale);
            bPlatEndianSwap(&light_material_data->SpecularMinR);
            bPlatEndianSwap(&light_material_data->SpecularMinG);
            bPlatEndianSwap(&light_material_data->SpecularMinB);
            bPlatEndianSwap(&light_material_data->SpecularMaxScale);
            bPlatEndianSwap(&light_material_data->SpecularMaxR);
            bPlatEndianSwap(&light_material_data->SpecularMaxG);
            bPlatEndianSwap(&light_material_data->SpecularMaxB);
            bPlatEndianSwap(&light_material_data->EnvmapPower);
            bPlatEndianSwap(&light_material_data->EnvmapMinScale);
            bPlatEndianSwap(&light_material_data->EnvmapMinR);
            bPlatEndianSwap(&light_material_data->EnvmapMinG);
            bPlatEndianSwap(&light_material_data->EnvmapMinB);
            bPlatEndianSwap(&light_material_data->EnvmapMaxScale);
            bPlatEndianSwap(&light_material_data->EnvmapMaxR);
            bPlatEndianSwap(&light_material_data->EnvmapMaxG);
            bPlatEndianSwap(&light_material_data->EnvmapMaxB);
            bPlatEndianSwap(&light_material_data->MetallicScale);
            bPlatEndianSwap(&light_material_data->SpecularHotSpot);
        }
    }
};

// total size: 0x60
class eLight {
  public:
    uint32 NameHash;            // offset 0x0, size 0x4
    uint8 Type;                 // offset 0x4, size 0x1
    uint8 AttenuationType;      // offset 0x5, size 0x1
    uint8 Shape;                // offset 0x6, size 0x1
    uint8 State;                // offset 0x7, size 0x1
    uint32 ExcludeNameHash;     // offset 0x8, size 0x4
    uint32 Colour;              // offset 0xC, size 0x4
    float PositionX;            // offset 0x10, size 0x4
    float PositionY;            // offset 0x14, size 0x4
    float PositionZ;            // offset 0x18, size 0x4
    float Size;                 // offset 0x1C, size 0x4
    float DirectionX;           // offset 0x20, size 0x4
    float DirectionY;           // offset 0x24, size 0x4
    float DirectionZ;           // offset 0x28, size 0x4
    float Intensity;            // offset 0x2C, size 0x4
    float FarStart;             // offset 0x30, size 0x4
    float FarEnd;               // offset 0x34, size 0x4
    float Falloff;              // offset 0x38, size 0x4
    int16 ScenerySectionNumber; // offset 0x3C, size 0x2
    char Name[34];              // offset 0x3E, size 0x22

    void EndianSwap() {
        bPlatEndianSwap(&this->NameHash);
        bPlatEndianSwap(&this->Type);
        bPlatEndianSwap(&this->AttenuationType);
        bPlatEndianSwap(&this->Shape);
        bPlatEndianSwap(&this->State);
        bPlatEndianSwap(&this->ExcludeNameHash);
        bPlatEndianSwap(&this->Colour);
        bPlatEndianSwap(&this->PositionX);
        bPlatEndianSwap(&this->PositionY);
        bPlatEndianSwap(&this->PositionZ);
        bPlatEndianSwap(&this->Size);
        bPlatEndianSwap(&this->DirectionX);
        bPlatEndianSwap(&this->DirectionY);
        bPlatEndianSwap(&this->DirectionZ);
        bPlatEndianSwap(&this->Intensity);
        bPlatEndianSwap(&this->FarStart);
        bPlatEndianSwap(&this->FarEnd);
        bPlatEndianSwap(&this->Falloff);
        bPlatEndianSwap(&this->ScenerySectionNumber);
    }
};

// total size: 0x20
class eLightPack : public bTNode<eLightPack> {
  public:
    int16 Version;              // offset 0x8, size 0x2
    int8 EndianSwapped;         // offset 0xA, size 0x1
    int8 Pad;                   // offset 0xB, size 0x1
    int32 ScenerySectionNumber; // offset 0xC, size 0x4
    vAABBTree *LightTree;       // offset 0x10, size 0x4
    int32 NumTreeNodes;         // offset 0x14, size 0x4
    eLight *LightArray;         // offset 0x18, size 0x4
    int32 NumLights;            // offset 0x1C, size 0x4

    void EndianSwap() {
        if (!this->EndianSwapped) {
            this->EndianSwapped = 1;

            bPlatEndianSwap(&this->Version);
            bPlatEndianSwap(&this->NumTreeNodes);
            bPlatEndianSwap(&this->NumLights);
            bPlatEndianSwap(&this->ScenerySectionNumber);
        }
    }
};

// total size: 0x68 (DWARF de zAnim y zEcstasy; en PS2 salen sus destructores)
class eDynamicLight : public bTNode<eDynamicLight>, public eLight {};

// total size: 0x34
class eDynamicLightPack : public bTNode<eDynamicLightPack> {
  public:
    eDynamicLightPack() {}

    char Name[32];                          // offset 0x8, size 0x20
    int Enabled;                            // offset 0x28, size 0x4
    bTList<eDynamicLight> DynamicLightList; // offset 0x2C, size 0x8
};

class eLightContext {
  public:
    int32 Type;
};

// total size: 0x124
class eDynamicLightContext : public eLightContext {
  public:
    bMatrix4 LocalColourMatrix;    // offset 0x4, size 0x40
    bMatrix4 LocalDirectionMatrix; // offset 0x44, size 0x40
    bMatrix4 LocalLightPositions;  // offset 0x84, size 0x40
    bVector4 LocalEyePosition;     // offset 0xC4, size 0x10
    int32 NumLights;               // offset 0xD4, size 0x4
    float SunlightIntensity;       // offset 0xD8, size 0x4
    int32 pad1;                    // offset 0xDC, size 0x4
    int32 pad2;                    // offset 0xE0, size 0x4
    bMatrix4 EnvMapMatrix;         // offset 0xE4, size 0x40
};

enum ESHAPER_LIGHT_MODE {
    LIGHT_INVALID = 0xFFFFFFFF,
    LIGHT_WORLD_POSITION = 4,
    LIGHT_OPPOSITE_SUN_DIRECTION = 3,
    LIGHT_SUN_DIRECTION = 2,
    LIGHT_CAMERA_SPACE = 1,
    LIGHT_WORLD_SPACE = 0,
};

// total size: 0x1C
class eShaperLight {
  public:
    ESHAPER_LIGHT_MODE CameraSpace; // offset 0x0, size 0x4
    float Theta;                    // offset 0x4, size 0x4
    float Phi;                      // offset 0x8, size 0x4
    float Red;                      // offset 0xC, size 0x4
    float Green;                    // offset 0x10, size 0x4
    float Blue;                     // offset 0x14, size 0x4
    float Scale;                    // offset 0x18, size 0x4
};

// total size: 0x88
class eShaperLightRig {
  public:
    eShaperLightRig() {}

    eShaperLightRig(ESHAPER_LIGHT_MODE l0_camera_space, float l0_theta, float l0_phi, float l0_red, float l0_green, float l0_blue, float l0_scale, ESHAPER_LIGHT_MODE l1_camera_space, float l1_theta, float l1_phi, float l1_red, float l1_green, float l1_blue, float l1_scale, ESHAPER_LIGHT_MODE l2_camera_space, float l2_theta, float l2_phi, float l2_red, float l2_green, float l2_blue, float l2_scale, ESHAPER_LIGHT_MODE l3_camera_space, float l3_theta, float l3_phi, float l3_red, float l3_green, float l3_blue, float l3_scale) {
        position = bVector3(0.0f, 0.0f, 0.0f);
        NameHash = 0;
        NumOverideSlots = 0;
        Lights[0].CameraSpace = l0_camera_space; Lights[0].Theta = l0_theta; Lights[0].Phi = l0_phi; Lights[0].Red = l0_red; Lights[0].Green = l0_green; Lights[0].Blue = l0_blue; Lights[0].Scale = l0_scale;
        Lights[1].CameraSpace = l1_camera_space; Lights[1].Theta = l1_theta; Lights[1].Phi = l1_phi; Lights[1].Red = l1_red; Lights[1].Green = l1_green; Lights[1].Blue = l1_blue; Lights[1].Scale = l1_scale;
        Lights[2].CameraSpace = l2_camera_space; Lights[2].Theta = l2_theta; Lights[2].Phi = l2_phi; Lights[2].Red = l2_red; Lights[2].Green = l2_green; Lights[2].Blue = l2_blue; Lights[2].Scale = l2_scale;
        Lights[3].CameraSpace = l3_camera_space; Lights[3].Theta = l3_theta; Lights[3].Phi = l3_phi; Lights[3].Red = l3_red; Lights[3].Green = l3_green; Lights[3].Blue = l3_blue; Lights[3].Scale = l3_scale;
    }

    uint32 NameHash;        // offset 0x0, size 0x4
    eShaperLight Lights[4]; // offset 0x4, size 0x70
    bVector3 position;      // offset 0x74, size 0x10
    int NumOverideSlots;    // offset 0x84, size 0x4
};

class eSceneryLightContext : public eLightContext {
  public:
    void EndianSwap() {
#ifndef EA_BUILD_A124
        bPlatEndianSwap(&Type);
        bPlatEndianSwap(&NumLights);
        bPlatEndianSwap(&LightingContextNumber);
#endif

        for (uint32 i = 0; i < NumLights; i++) {
#ifndef EA_BUILD_A124
            bPlatEndianSwap(&LocalLights[i].v0);
            bPlatEndianSwap(&LocalLights[i].v1);
            bPlatEndianSwap(&LocalLights[i].v2);
            bPlatEndianSwap(&LocalLights[i].v3);
#endif
        }
    }

    char Name[34];
    int16 LightingContextNumber;
    bMatrix4 *LocalLights;
    uint32 NumLights;
};

void elInit();
int elSetupLights(eDynamicLightContext *light_context, eShaperLightRig *shaper_lights, bVector3 *local_pos, bMatrix4 *local_world,
                  bMatrix4 *world_view, eView *view);
void elResetLightContext(eDynamicLightContext *light_context);
int elCloneLightContext(eDynamicLightContext *light_context, bMatrix4 *local_world, bMatrix4 *world_view, bVector4 *camera_world_position,
                        eView *view, eDynamicLightContext *old_context);
void UpdateLightFlareParameters();
void eResestLightFlarePool();
void eRenderWorldLightFlares(eView *view, flareType type);
eLightFlare *eGetNextLightFlareInPool(uint32 XcludeViewIDs);
int eRenderLightFlare(eView *view, eLightFlare *light_flare, bMatrix4 *local_world, float intensity_scale, enum eLightReflexionType ReflexionAction,
                      flareType destinationType, float RefelectionOverride, uint32 ColourOverRide, float sizescale);
eLightMaterial *elGetLightMaterial(uint32 name_hash);
void eLightUpdateTextures();
void AdjustQuickDynamicLight(eShaperLightRig *ShaperRigP, bVector3 *MyPosition);
int elSetupLightContext(eDynamicLightContext *light_context, eShaperLightRig *shaper_lights, bMatrix4 *local_world, bMatrix4 *world_view,
                        bVector4 *camera_world_position, eView *view);
void RestoreShaperRig(eShaperLightRig *ShaperRigP, uint32 slot, eShaperLightRig *ShaperRigBP);
void AddQuickDynamicLight(eShaperLightRig *ShaperRigP, uint32 slot, float r, float g, float b, float intensity, bVector3 *position);

extern int PrintLightQuery;

extern eShaperLightRig ShaperLightsBackRoom;
extern eShaperLightRig ShaperLightsCarLot;
extern eShaperLightRig ShaperLightsDefault;
extern eShaperLightRig ShaperLightsScenery;
extern eShaperLightRig ShaperLightsWorldObjects;
extern eShaperLightRig ShaperLightsCarsInGame;
extern eShaperLightRig ShaperLightsCShop;
extern eShaperLightRig ShaperLightsCharacters;
extern eShaperLightRig ShaperLightsCharactersBackup;
extern eShaperLightRig ShaperLightsQRace;
extern eShaperLightRig ShaperLightsSafehouse;

#endif
