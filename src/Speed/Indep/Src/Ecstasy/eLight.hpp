#ifndef ECSTASY_ELIGHT_H
#define ECSTASY_ELIGHT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#ifdef EA_PLATFORM_GAMECUBE
#include "Speed/GameCube/Src/Ecstasy/eLightPlat.hpp"
#elif defined(EA_PLATFORM_XENON)
#include "Speed/Xenon/Src/Ecstasy/eLightPlat.hpp"
#elif defined(EA_PLATFORM_PLAYSTATION2)
#include "Speed/PSX2/Src/Ecstasy/eLightPlat.hpp"
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/Misc/VolumeTree.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

enum eLightReflexionType {
    REF_FAST = 2,
    REF_TOPO = 1,
    REF_NONE = 0,
};

enum flareType {
    FLARE_CAT_P2 = 4,
    FLARE_CAT_P1 = 3,
    FLARE_REFLECT = 2,
    FLARE_ENV = 1,
    FLARE_NORM = 0,
};

class eLightFlare : public bTNode<eLightFlare> {
    // total size: 0x30
  public:
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
};

class eLightFlarePackHeader : public bTNode<eLightFlarePackHeader> {
    // total size: 0x60
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

    void EndianSwap() {}
};

class eLightMaterialData {
    // total size: 0x78
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

class eLightMaterial : public eLightMaterialPlatInterface, public bTNode<eLightMaterial> {
    // total size: 0xA8
  public:
    uint32 NameHash;           // offset 0xC, size 0x4
    uint32 Version;            // offset 0x10, size 0x4
    char Name[28];             // offset 0x14, size 0x1C
    eLightMaterialData feData; // offset 0x30, size 0x78

    void BuildData();

    void EndianSwap() {}
};

class eLight {
    // total size: 0x60
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
};

class eLightPack : public bTNode<eLightPack> {
    // total size: 0x20
  public:
    int16 Version;              // offset 0x8, size 0x2
    int8 EndianSwapped;         // offset 0xA, size 0x1
    int8 Pad;                   // offset 0xB, size 0x1
    int32 ScenerySectionNumber; // offset 0xC, size 0x4
    vAABBTree *LightTree;       // offset 0x10, size 0x4
    int32 NumTreeNodes;         // offset 0x14, size 0x4
    eLight *LightArray;         // offset 0x18, size 0x4
    int32 NumLights;            // offset 0x1C, size 0x4

    void EndianSwap() {}
};

class eLightContext {
  public:
    int32 Type;
};

class eDynamicLightContext : public eLightContext {
    // total size: 0x124
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
    LIGHT_INVALID = -1,
    LIGHT_WORLD_POSITION = 4,
    LIGHT_OPPOSITE_SUN_DIRECTION = 3,
    LIGHT_SUN_DIRECTION = 2,
    LIGHT_CAMERA_SPACE = 1,
    LIGHT_WORLD_SPACE = 0,
};

class eShaperLight {
    // total size: 0x1C
  public:
    ESHAPER_LIGHT_MODE CameraSpace; // offset 0x0, size 0x4
    float Theta;                    // offset 0x4, size 0x4
    float Phi;                      // offset 0x8, size 0x4
    float Red;                      // offset 0xC, size 0x4
    float Green;                    // offset 0x10, size 0x4
    float Blue;                     // offset 0x14, size 0x4
    float Scale;                    // offset 0x18, size 0x4
};

class eShaperLightRig {
    // total size: 0x88
  public:
    uint32 NameHash;        // offset 0x0, size 0x4
    eShaperLight Lights[4]; // offset 0x4, size 0x70
    bVector3 position;      // offset 0x74, size 0x10
    int NumOverideSlots;    // offset 0x84, size 0x4
};

class eSceneryLightContext : public eLightContext {
  public:
    void EndianSwap() {
        bPlatEndianSwap(&Type);
        bPlatEndianSwap(&NumLights);
        bPlatEndianSwap(&LightingContextNumber);

        for (uint32 i = 0; i < NumLights; i++) {
            bPlatEndianSwap(&LocalLights[i].v0);
            bPlatEndianSwap(&LocalLights[i].v1);
            bPlatEndianSwap(&LocalLights[i].v2);
            bPlatEndianSwap(&LocalLights[i].v3);
        }
    }

    char Name[34];
    int16 LightingContextNumber;
    bMatrix4 *LocalLights;
    uint32 NumLights;
};

void elInit();
void UpdateLightFlareParameters();
void eResestLightFlarePool();
eLightFlare *eGetNextLightFlareInPool(uint32 XcludeViewIDs);
int eRenderLightFlare(eView *view, eLightFlare *light_flare, bMatrix4 *local_world, float intensity_scale, enum eLightReflexionType ReflexionAction,
                      flareType destinationType, float RefelectionOverride, uint32 ColourOverRide, float sizescale);
eLightMaterial *elGetLightMaterial(uint32 name_hash);
void eLightUpdateTextures();

#endif
