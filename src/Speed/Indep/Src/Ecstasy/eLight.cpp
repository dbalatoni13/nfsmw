#include "eLight.hpp"
#include "Ecstasy.hpp"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Misc/VolumeTree.hpp"
#include "Speed/Indep/Src/World/VisibleSection.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "eMath.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/light_flares_cg.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"

class eDynamicLightPack;

eLightFlarePackHeader RunTimeLightFlarePackHeader;
bTList<eLightFlarePackHeader> LightFlarePackList;
bTList<eLightPack> LightPackList;
bTList<eLightMaterial> LightMaterialList;    // size: 0x8, address: 0x80460DDC
eLightMaterial DefaultLightMaterialData;     // size: 0xA8, address: 0x80460DE4
static eLightMaterial *DefaultLightMaterial = &DefaultLightMaterialData; // size: 0x4, address: 0x8041A63C

// total size: 0xD0
struct eLightQuery {
    eLight *Lights[24];   // offset 0x0, size 0x60
    float Influences[24]; // offset 0x60, size 0x60
    float AmbientR;       // offset 0xC0, size 0x4
    float AmbientG;       // offset 0xC4, size 0x4
    float AmbientB;       // offset 0xC8, size 0x4
    float LightScale;     // offset 0xCC, size 0x4
};

float reflectionStretch = 10.0f;
float MINreflectionStretch = 1.2f;
float MAXreflectionStretch = 10.0f;

eLightMaterial *TweakLightMaterial = 0;
int PrintLightQuery = 0;
float WorldLightDirectionVector[4] = {-0.627587f, -0.550515f, 0.364162f, 1.0f};
uint32 eLightFlareTextureNameHashes[3] = {0x8868656F, 0x88D8E8A2, 0x3394FE62};  // size: 0xC, address: 0x8041A69C

bVector3 LightPositionVFE0(0.0f, -1.25f, -5.0f);
bVector3 LightPositionVFE1(-5.0f, -1.0f, 0.0f);
bVector3 LightPositionVFE2(5.0f, -1.8f, 2.8f);
bVector3 LightPositionFE0(0.0f, -5.0f, 5.0f);
bVector3 LightPositionFE1(4.3f, 2.5f, 2.5f);
bVector3 LightPositionFE2(-4.3f, 2.5f, 2.5f);
bVector3 LightColourFE0(128.0f, 128.0f, 128.0f);
bVector3 LightColourFE1(64.0f, 64.0f, 64.0f);
bVector3 LightColourFE2(64.0f, 64.0f, 64.0f);
bVector3 AmbientColourFE(0.0f, 0.0f, 0.0f);
bVector3 AmbientColourIG(0.0f, 0.0f, 0.0f);
bVector3 LightDirection0(-0.74f, 0.0f, 0.55f);
bVector3 LightColour0(80.0f, 80.0f, 80.0f);

LightFlareParameters SingleLightFlareParameters[22] = {
    {0.0f, 64.0f, {100.0f, 120.0f, 170.0f, 80.0f}, 1.45f, 0.8f, 0.01f, 0.048f, 0},
    {28.0f, 28.0f, {100.0f, 120.0f, 140.0f, 20.0f}, 3.0f, 0.8f, 0.01f, 0.015f, 1},
    {20.0f, 30.0f, {128.0f, 25.0f, 25.0f, 120.0f}, 0.45f, 0.2f, 0.01f, 0.03f, 2},
    {20.0f, 30.0f, {128.0f, 25.0f, 25.0f, 120.0f}, 0.45f, 0.2f, 0.01f, 0.03f, 2},
    {20.0f, 20.0f, {255.0f, 255.0f, 255.0f, 128.0f}, 1.2f, 0.2f, 0.005f, 0.01f, 2},
    {20.0f, 20.0f, {128.0f, 128.0f, 128.0f, 128.0f}, 1.0f, 0.2f, 0.005f, 0.01f, 2},
    {40.0f, 50.0f, {128.0f, 0.0f, 0.0f, 150.0f}, 1.0f, 1.0f, 0.01f, 0.02f, 0},
    {40.0f, 50.0f, {0.0f, 0.0f, 128.0f, 150.0f}, 1.0f, 1.0f, 0.01f, 0.02f, 0},
    {30.0f, 40.0f, {128.0f, 128.0f, 128.0f, 128.0f}, 1.0f, 1.0f, 0.01f, 0.02f, 0},
    {40.0f, 50.0f, {128.0f, 0.0f, 0.0f, 150.0f}, 1.0f, 1.0f, 0.01f, 0.02f, 0},
    {40.0f, 50.0f, {0.0f, 0.0f, 128.0f, 150.0f}, 1.0f, 1.0f, 0.01f, 0.02f, 0},
    {30.0f, 40.0f, {128.0f, 128.0f, 128.0f, 128.0f}, 1.0f, 1.0f, 0.01f, 0.02f, 0},
    {30.0f, 30.0f, {110.0f, 100.0f, 75.0f, 110.0f}, 1.0f, 4.0f, 0.001f, 0.1f, 2},
    {1.0f, 4.0f, {100.0f, 120.0f, 140.0f, 16.0f}, 1.0f, 0.0f, 0.0f, 0.1f, 1},
    {10.0f, 10.0f, {110.0f, 75.0f, 0.0f, 150.0f}, 1.0f, 2.7f, 0.001f, 0.1f, 2},
    {10.0f, 10.0f, {110.0f, 0.0f, 0.0f, 150.0f}, 1.0f, 4.0f, 0.001f, 0.1f, 2},
    {10.0f, 10.0f, {0.0f, 0.0f, 120.0f, 150.0f}, 1.0f, 4.0f, 0.001f, 0.1f, 2},
    {30.0f, 30.0f, {110.0f, 75.0f, 0.0f, 150.0f}, 1.0f, 4.0f, 0.001f, 0.1f, 2},
    {30.0f, 30.0f, {110.0f, 10.0f, 0.0f, 150.0f}, 1.0f, 4.0f, 0.001f, 0.1f, 2},
    {1.0f, 10.0f, {0.0f, 100.5f, 20.0f, 70.0f}, 1.0f, 1.0f, 0.07f, 0.07f, 2},
    {1.0f, 10.0f, {105.0f, 26.5f, 0.0f, 70.0f}, 1.0f, 1.0f, 0.07f, 0.07f, 2},
    {1.0f, 20.0f, {255.0f, 255.5f, 255.0f, 255.0f}, 1.0f, 5.0f, 0.8f, 1.0f, 1},
};

int LightFlareParametersNeedUpdating = 1;

LightFlareParameters *LightFlareParameterIndicies[22][2] = {
    {&SingleLightFlareParameters[0], &SingleLightFlareParameters[1]},
    {&SingleLightFlareParameters[2], 0},
    {&SingleLightFlareParameters[3], 0},
    {&SingleLightFlareParameters[4], 0},
    {&SingleLightFlareParameters[5], 0},
    {&SingleLightFlareParameters[6], 0},
    {&SingleLightFlareParameters[7], 0},
    {&SingleLightFlareParameters[8], 0},
    {&SingleLightFlareParameters[0], &SingleLightFlareParameters[1]},
    {&SingleLightFlareParameters[0], &SingleLightFlareParameters[1]},
    {&SingleLightFlareParameters[9], 0},
    {&SingleLightFlareParameters[10], 0},
    {&SingleLightFlareParameters[11], 0},
    {&SingleLightFlareParameters[12], 0},
    {&SingleLightFlareParameters[14], 0},
    {&SingleLightFlareParameters[15], 0},
    {&SingleLightFlareParameters[16], 0},
    {&SingleLightFlareParameters[17], 0},
    {&SingleLightFlareParameters[18], 0},
    {&SingleLightFlareParameters[19], 0},
    {&SingleLightFlareParameters[20], 0},
    {&SingleLightFlareParameters[21], 0},
};
int32 DrawLightFlares = 1;

static const float FlareFalloffDistanceNORM = 32000.0f;
static const float FlareCutoffDistanceNORM = 40000.0f;
static const float FlareFalloffDistanceENV = 3900.0f;
static const float FlareCutoffDistanceENV = 4000.0f;

extern TextureInfo *eLightFlareTextureInfos[3];
extern uint32 PoolOfFlaresXcludeView[50];
float intensity;

int LoaderLights(bChunk *bchunk);
int UnloaderLights(bChunk *bchunk);

bChunkLoader bChunkLoaderLightMaterial(0x00135200, LoaderLights, UnloaderLights);
bChunkLoader bChunkLoaderLightFlares(0x80135100, LoaderLights, UnloaderLights);
bChunkLoader bChunkLoaderLights(0x80135000, LoaderLights, UnloaderLights);

bTList<eDynamicLightPack> DynamicLightPackList;

eShaperLightRig ShaperLightsGCSpecial(
    LIGHT_WORLD_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    LIGHT_SUN_DIRECTION, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.2f,
    LIGHT_OPPOSITE_SUN_DIRECTION, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.2f,
    LIGHT_CAMERA_SPACE, 240.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.2f);

eShaperLightRig ShaperLightsDefault(
    LIGHT_WORLD_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    LIGHT_SUN_DIRECTION, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.2f,
    LIGHT_OPPOSITE_SUN_DIRECTION, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.2f,
    LIGHT_CAMERA_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

eShaperLightRig ShaperLightsBackRoom(
    LIGHT_WORLD_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    LIGHT_WORLD_SPACE, 90.0f, 48.0f, 1.0f, 1.0f, 0.95f, 0.98f,
    LIGHT_WORLD_SPACE, 134.0f, 185.0f, 0.83f, 0.83f, 1.0f, 0.5f,
    LIGHT_WORLD_SPACE, 27.0f, 175.0f, 0.83f, 0.83f, 1.0f, 0.35f);

eShaperLightRig ShaperLightsCarLot(
    LIGHT_WORLD_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    LIGHT_WORLD_SPACE, -90.0f, 48.0f, 1.0f, 1.0f, 0.95f, 0.98f,
    LIGHT_WORLD_SPACE, -134.0f, 185.0f, 0.83f, 0.83f, 1.0f, 0.5f,
    LIGHT_WORLD_SPACE, -27.0f, 175.0f, 0.83f, 0.83f, 1.0f, 0.35f);

eShaperLightRig ShaperLightsCShop(
    LIGHT_WORLD_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    LIGHT_WORLD_SPACE, 90.0f, 48.0f, 1.0f, 1.0f, 0.95f, 0.98f,
    LIGHT_WORLD_SPACE, 134.0f, 185.0f, 0.83f, 0.83f, 1.0f, 0.5f,
    LIGHT_WORLD_SPACE, 27.0f, 175.0f, 0.83f, 0.83f, 1.0f, 0.35f);

eShaperLightRig ShaperLightsQRace(
    LIGHT_WORLD_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    LIGHT_WORLD_SPACE, 90.0f, 48.0f, 1.0f, 1.0f, 0.95f, 0.98f,
    LIGHT_WORLD_SPACE, 134.0f, 185.0f, 0.83f, 0.83f, 1.0f, 0.5f,
    LIGHT_WORLD_SPACE, 27.0f, 175.0f, 0.83f, 0.83f, 1.0f, 0.35f);

eShaperLightRig ShaperLightsSafehouse(
    LIGHT_WORLD_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    LIGHT_WORLD_SPACE, 90.0f, 48.0f, 1.0f, 1.0f, 0.95f, 0.98f,
    LIGHT_WORLD_SPACE, 134.0f, 185.0f, 0.83f, 0.83f, 1.0f, 0.5f,
    LIGHT_WORLD_SPACE, 27.0f, 175.0f, 0.83f, 0.83f, 1.0f, 0.35f);

eShaperLightRig ShaperLightsCarsInGame(
    LIGHT_WORLD_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    LIGHT_SUN_DIRECTION, 0.0f, 350.0f, 0.85f, 0.9f, 1.0f, 1.5f,
    LIGHT_SUN_DIRECTION, 50.0f, 220.0f, 0.55f, 0.6f, 1.0f, 0.7f,
    LIGHT_SUN_DIRECTION, 310.0f, 220.0f, 0.65f, 0.7f, 1.0f, 0.9f);

eShaperLightRig ShaperLightsCharacters(
    LIGHT_WORLD_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    LIGHT_CAMERA_SPACE, 122.9f, 194.0f, 0.5f, 0.5f, 0.5f, 0.53f,
    LIGHT_CAMERA_SPACE, 28.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.59f,
    LIGHT_CAMERA_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

eShaperLightRig ShaperLightsCharactersBackup(
    LIGHT_WORLD_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    LIGHT_SUN_DIRECTION, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    LIGHT_CAMERA_SPACE, 30.1f, 0.0f, 1.0f, 1.0f, 1.0f, 0.63f,
    LIGHT_CAMERA_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

eShaperLightRig ShaperLightsScenery(
    LIGHT_WORLD_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    LIGHT_SUN_DIRECTION, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.2f,
    LIGHT_OPPOSITE_SUN_DIRECTION, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.2f,
    LIGHT_CAMERA_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

eShaperLightRig ShaperLightsWorldObjects(
    LIGHT_WORLD_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    LIGHT_SUN_DIRECTION, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.2f,
    LIGHT_OPPOSITE_SUN_DIRECTION, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.2f,
    LIGHT_CAMERA_SPACE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

static inline void _ecs_r60_probe(eDynamicLightContext *a, const eDynamicLightContext *b) { *a = *b; }

int LoaderLights(bChunk *bchunk) {
    if (bchunk->GetID() == BCHUNK_LIGHT_MATERIALS) {
        eLightMaterial *light_material = reinterpret_cast<eLightMaterial *>(bchunk->GetData());

        light_material->EndianSwap();

        if (light_material->Version == 3) {
            light_material->CreatePlatInfo();

            light_material->BuildData();

            LightMaterialList.AddTail(light_material);

            if (light_material->NameHash == STRINGHASH_DEFAULT) {
                DefaultLightMaterial = light_material;
            }
        }
        return 1;
    }

    if (bchunk->GetID() == BCHUNK_LIGHT_FLARES_PACK) {
        bChunk *chunk = bchunk->GetFirstChunk();
        bChunk *last_chunk = bchunk->GetLastChunk();
        eLightFlarePackHeader *pack_header = nullptr;
        bool header_needed_endian_swap = false;

        while (chunk != last_chunk) {
            switch (chunk->GetID()) {
            case BCHUNK_LIGHT_FLARE_PACK_HEADER:
                pack_header = reinterpret_cast<eLightFlarePackHeader *>(chunk->GetAlignedData(16));

                header_needed_endian_swap = pack_header->EndianSwapped == 0;
                pack_header->EndianSwap();

                if (pack_header->Version != 3) {
                    pack_header = nullptr;
                    break;
                }

                pack_header->LightFlareList.InitList();
                break;

            case BCHUNK_LIGHT_FLARE_PACK_FLARES:
                if (!pack_header) {
                    break;
                }

                {
                    int num_flares = pack_header->NumLightFlares;
                    eLightFlare *light_flare = reinterpret_cast<eLightFlare *>(chunk->GetAlignedData(16));

                    while (num_flares) {
                        if (header_needed_endian_swap) {
                            light_flare->EndianSwap();
                        }

                        pack_header->LightFlareList.AddTail(light_flare);
                        light_flare++;

                        num_flares--;
                    }
                }
                break;
            }
            chunk = chunk->GetNext();
        }

        if (pack_header) {
            LightFlarePackList.AddTail(pack_header);

            VisibleSectionUserInfo *user_info = TheVisibleSectionManager.AllocateUserInfo(pack_header->ScenerySectionNumber);
            user_info->pLightFlarePack = pack_header;
        }
        return 1;
    }

    if (bchunk->GetID() == BCHUNK_LIGHT_SOURCES_PACK) {
        bChunk *chunk = bchunk->GetFirstChunk();
        bChunk *last_chunk = bchunk->GetLastChunk();
        eLightPack *light_pack = nullptr;
        bool header_needed_endian_swap = false;

        while (chunk != last_chunk) {
            switch (chunk->GetID()) {
            case BCHUNK_LIGHT_SOURCE_PACK_HEADER:
                light_pack = reinterpret_cast<eLightPack *>(chunk->GetAlignedData(16));

                header_needed_endian_swap = light_pack->EndianSwapped == 0;
                light_pack->EndianSwap();

                if (light_pack->Version != 4) {
                    return 1;
                }
                break;

            case BCHUNK_LIGHT_SOURCE_PACK_TREE: {
                vAABBTree *tree = reinterpret_cast<vAABBTree *>(chunk->GetAlignedData(16));

                light_pack->LightTree = tree;
                tree->NodeArray = reinterpret_cast<vAABB *>(tree + 1);

                if (header_needed_endian_swap) {
                    tree->SwapEndian();
                }
                break;
            }

            case BCHUNK_LIGHT_SOURCE_PACK_LIGHTS:
                light_pack->LightArray = reinterpret_cast<eLight *>(chunk->GetAlignedData(16));

                if (header_needed_endian_swap) {
                    for (int i = 0; i < light_pack->NumLights; i++) {
                        light_pack->LightArray[i].EndianSwap();
                    }
                }
                break;
            }
            chunk = chunk->GetNext();
        }

        if (light_pack) {
            LightPackList.AddTail(light_pack);

            VisibleSectionUserInfo *user_info = TheVisibleSectionManager.AllocateUserInfo(light_pack->ScenerySectionNumber);
            user_info->pLightPack = light_pack;
        }
        return 1;
    }
    return 0;
}

// UNSOLVED but identical
int UnloaderLights(bChunk *bchunk) {
    if (bchunk->GetID() == BCHUNK_LIGHT_MATERIALS) {
        eLightMaterial *light_material = reinterpret_cast<eLightMaterial *>(bchunk->GetData());
        if (light_material->Version == 3) {
            light_material->Remove();
            if (light_material->NameHash == STRINGHASH_DEFAULT) {
                DefaultLightMaterial = &DefaultLightMaterialData;
            }
        }
        return 1;
    }
    if (bchunk->GetID() == BCHUNK_LIGHT_FLARES_PACK) {
        bChunk *chunk = bchunk->GetFirstChunk();
        bChunk *last_chunk = bchunk->GetLastChunk();
        eLightFlarePackHeader *pack_header;
        while (chunk != last_chunk) {
            if (chunk->GetID() == BCHUNK_LIGHT_FLARE_PACK_HEADER) {
                pack_header = reinterpret_cast<eLightFlarePackHeader *>(chunk->GetAlignedData(16));
                if (pack_header->Version != 3) {
                    return 1;
                }
                pack_header->Remove();

                VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(pack_header->ScenerySectionNumber);
                user_info->pLightFlarePack = nullptr;
                TheVisibleSectionManager.UnallocateUserInfo(pack_header->ScenerySectionNumber);
                break;
            }
            chunk = chunk->GetNext();
        }
        return 1;
    }
    if (bchunk->GetID() == BCHUNK_LIGHT_SOURCES_PACK) {
        bChunk *chunk = bchunk->GetFirstChunk();
        bChunk *last_chunk = bchunk->GetLastChunk();
        while (chunk != last_chunk) {
            if (chunk->GetID() == BCHUNK_LIGHT_SOURCE_PACK_HEADER) {
                eLightPack *light_pack = reinterpret_cast<eLightPack *>(chunk->GetAlignedData(16));
                if (light_pack->Version != 4) {
                    return 1;
                }
                light_pack->Remove();

                VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(light_pack->ScenerySectionNumber);
                user_info->pLightPack = nullptr;
                TheVisibleSectionManager.UnallocateUserInfo(light_pack->ScenerySectionNumber);
                break;
            }
        }
        return 1;
    }
    return 0;
}

void SetSelectCarLighting(int, float, int) {}

void SphericalToCartesian(bVector3 *v, float theta, float phi, float radius) {
    float cos_theta;
    float sin_theta;
    float cos_phi;
    float sin_phi;

    bSinCos(&sin_theta, &cos_theta, bDegToAng(theta));
    bSinCos(&sin_phi, &cos_phi, bDegToAng(phi));
    v->x = radius * cos_theta * sin_phi;
    v->y = radius * sin_theta * sin_phi;
    v->z = radius * cos_phi;
}

void CartesianToSpherical(bVector3 *v, float x, float y, float z) {
    float r;
    float t;
    float p;

    r = bSqrt(x * x + y * y + z * z);
    t = bATan(x, y);
    p = bACos(z / r);
    v->x = t;
    v->y = p;
    v->z = r;
}

void elRotateLightContext(eDynamicLightContext *new_light_context, eDynamicLightContext *light_context, bMatrix4 *rotate_matrix) {
    bMatrix4 invrotate_matrix;
    bVector4 light_directions[3];

    *new_light_context = *light_context;

    eInvertRotationMatrix(&invrotate_matrix, rotate_matrix);
    light_directions[0].x = light_context->LocalDirectionMatrix.v0.x;
    light_directions[0].y = light_context->LocalDirectionMatrix.v1.x;
    light_directions[0].z = light_context->LocalDirectionMatrix.v2.x;

    light_directions[1].x = light_context->LocalDirectionMatrix.v0.y;
    light_directions[1].y = light_context->LocalDirectionMatrix.v1.y;
    light_directions[1].z = light_context->LocalDirectionMatrix.v2.y;

    light_directions[2].x = light_context->LocalDirectionMatrix.v0.z;
    light_directions[2].y = light_context->LocalDirectionMatrix.v1.z;
    light_directions[2].z = light_context->LocalDirectionMatrix.v2.z;

    light_directions[0].w = 0.0f;
    light_directions[1].w = 0.0f;
    light_directions[2].w = 0.0f;

    eMulVector(&light_directions[0], &invrotate_matrix, &light_directions[0]);
    eMulVector(&light_directions[1], &invrotate_matrix, &light_directions[1]);
    eMulVector(&light_directions[2], &invrotate_matrix, &light_directions[2]);

    new_light_context->LocalDirectionMatrix.v0.x = light_directions[0].x;
    new_light_context->LocalDirectionMatrix.v1.x = light_directions[0].y;
    new_light_context->LocalDirectionMatrix.v2.x = light_directions[0].z;

    new_light_context->LocalDirectionMatrix.v0.y = light_directions[1].x;
    new_light_context->LocalDirectionMatrix.v1.y = light_directions[1].y;
    new_light_context->LocalDirectionMatrix.v2.y = light_directions[1].z;

    new_light_context->LocalDirectionMatrix.v0.z = light_directions[2].x;
    new_light_context->LocalDirectionMatrix.v1.z = light_directions[2].y;
    new_light_context->LocalDirectionMatrix.v2.z = light_directions[2].z;
}

int elSetupEnvMap(eDynamicLightContext *light_context, bMatrix4 *local_world, bMatrix4 *world_view, bVector4 *camera_world_position) {
    bMatrix4 world_local;

    eInvertRotationMatrix(&world_local, local_world);
    if (camera_world_position) {
        bVector4 v;
        v.x = camera_world_position->x - local_world->v3.x;
        v.y = camera_world_position->y - local_world->v3.y;
        v.z = camera_world_position->z - local_world->v3.z;
        v.w = 1.0f;
        eMulVector(&v, &world_local, &v);

        bool in_front_end = IsGameFlowInFrontEnd();
        float distance_min = 2.78f;
        float distance_max = in_front_end ? 8.0f : 2.78f;
        float distance = bLength(&v);

        if (distance < distance_min) {
            distance = distance_min;
        } else if (distance > distance_max) {
            distance = distance_max;
        }

        v.w = 0.0f;
        bNormalize(&v, &v);
        bScale(&v, &v, distance);
        v.w = 1.0f;
        light_context->LocalEyePosition = v;
        light_context->LocalEyePosition.w = 1.0f;
    }
    if (world_view && camera_world_position) {
        bMatrix4 *envmap_matrix = &light_context->EnvMapMatrix;
        bVector4 *camera_local_space;
        eMulMatrix(envmap_matrix, local_world, world_view);

        envmap_matrix->v3.x = light_context->LocalEyePosition.x;
        envmap_matrix->v3.y = light_context->LocalEyePosition.y;
        envmap_matrix->v3.z = light_context->LocalEyePosition.z;

        envmap_matrix->v0.w = 0.0f;
        envmap_matrix->v1.w = 0.0f;
        envmap_matrix->v2.w = 0.0f;
        envmap_matrix->v3.w = 0.0f;
    }
    return 1;
}

void elResetLightContext(eDynamicLightContext *light_context) {
    bMemSet(light_context, 0, sizeof(*light_context));
}

int elSetupLights(eDynamicLightContext *light_context /* r27 */, eShaperLightRig *shaper_lights /* r26 */, bVector3 *local_pos /* r22 */,
                  bMatrix4 *local_world /* r1+0x4B8 */, bMatrix4 *world_view /* r4 */, eView *view /* r8 */) {
    ProfileNode profile_node;

    if (!light_context) {
        return -1;
    }

    bMatrix4 view_world;
    bMatrix4 *camera_matrix = world_view;
    const int max_lights = 19;

    if (view) {
        Camera *camera = view->GetCamera();
        if (camera) {
            camera_matrix = camera->GetCameraMatrix();
        }
    }

    eInvertRotationMatrix(&view_world, camera_matrix);

    bVector3 light_positions[max_lights];
    bVector3 light_directions[max_lights];
    bVector4 light_pos_colours[max_lights];
    int num_shaper_lights = 4;

    for (int i = 1; i < num_shaper_lights; i++) {
        bVector3 *light_position = &light_positions[i - 1];

        switch (shaper_lights->Lights[i].CameraSpace) {
        case LIGHT_WORLD_SPACE:
            SphericalToCartesian(light_position, shaper_lights->Lights[i].Theta + 180.0f,
                                 90.0f - shaper_lights->Lights[i].Phi, 1.0f);
            break;

        case LIGHT_WORLD_POSITION:
            *light_position = shaper_lights->position;
            *light_position -= *local_pos;
            break;

        case LIGHT_CAMERA_SPACE: {
            SphericalToCartesian(light_position, shaper_lights->Lights[i].Theta + 180.0f,
                                 90.0f - shaper_lights->Lights[i].Phi, 1.0f);

            float x = light_position->x;
            float y = light_position->y;
            float z = light_position->z;

            light_position->x = -y;
            light_position->y = -z;
            light_position->z = x;

            bVector4 light_dir(light_position->x, light_position->y, light_position->z, 0.0f);

            eMulVector(&light_dir, &view_world, &light_dir);

            light_position->x = light_dir.x;
            light_position->y = light_dir.y;
            light_position->z = light_dir.z;
            break;
        }

        case LIGHT_SUN_DIRECTION:
        case LIGHT_OPPOSITE_SUN_DIRECTION: {
            float *sunDirection = WorldLightDirectionVector;
            bVector3 spherical_sun_direction;

            if (shaper_lights->Lights[i].CameraSpace == LIGHT_SUN_DIRECTION) {
                CartesianToSpherical(&spherical_sun_direction, sunDirection[0], sunDirection[1], sunDirection[2]);
            } else {
                CartesianToSpherical(&spherical_sun_direction, -sunDirection[0], -sunDirection[1], -sunDirection[2]);
            }

            spherical_sun_direction.x = bAngToDeg((bAngle)spherical_sun_direction.x);
            spherical_sun_direction.y = bAngToDeg((bAngle)spherical_sun_direction.y);

            spherical_sun_direction.y += 90.0f;

            spherical_sun_direction.x += shaper_lights->Lights[i].Theta;
            spherical_sun_direction.y += shaper_lights->Lights[i].Phi;

            SphericalToCartesian(light_position, spherical_sun_direction.x + 180.0f, 90.0f - spherical_sun_direction.y, 1.0f);
            break;
        }
        }

        float pos_scale = shaper_lights->Lights[i].Scale;

        light_pos_colours[i - 1].x = shaper_lights->Lights[i].Red * pos_scale * 255.0f;
        light_pos_colours[i - 1].y = shaper_lights->Lights[i].Green * pos_scale * 255.0f;
        light_pos_colours[i - 1].z = shaper_lights->Lights[i].Blue * pos_scale * 255.0f;
        light_pos_colours[i - 1].w = 0.0f;
    }

    int num_lights = 0;

    // El original tiene este bloque con num_lights == 0: el codigo se borra entero
    // (el volcado DWARF le da rango de longitud cero) pero light_query SIGUE
    // reservando sus 0xD0 bytes de marco en r1+0x3E8, y de ahi sale el tamano de
    // marco 0x560. Borrarlo baja la funcion del 100% al 97,86%.
    {
        eLightQuery light_query;
        float car_x;
        float car_y;
        float car_z;

        for (int i = 0; i < num_lights; i++) {
            bVector3 *light_pos = (bVector3 *)&light_query.Lights[i]->PositionX;
            unsigned int light_col = light_query.Lights[i]->Colour;
            float r = ((light_col >> 16) & 0xFF) * light_query.Influences[i];
            float g = ((light_col >> 8) & 0xFF) * light_query.Influences[i];
            float b = (light_col & 0xFF) * light_query.Influences[i];

            {
                float cossine = bCos(light_query.Lights[i]->Falloff);
                bVector3 *myPos = light_pos;
                bVector3 compVector;
                bVector3 *dirVec = (bVector3 *)&light_query.Lights[i]->DirectionX;
                float dotP;
                float modifier;

                compVector.x = myPos->x - car_x;
                compVector.y = myPos->y - car_y;
                compVector.z = myPos->z - car_z;

                dotP = compVector.x * dirVec->x + compVector.y * dirVec->y + compVector.z * dirVec->z;
                modifier = (dotP - cossine) / (1.0f - cossine);

                light_pos_colours[i].x += r * modifier;
                light_pos_colours[i].y += g * modifier;
                light_pos_colours[i].z += b * modifier;
            }
        }
    }

    if (local_world) {
        bMatrix4 world_local;
        eInvertRotationMatrix(&world_local, local_world);

        for (int i = 0; i < num_shaper_lights - 1; i++) {
            eMulVector(&light_positions[i], &world_local, &light_positions[i]);
        }
    }

    for (int i = 0; i < num_shaper_lights - 1; i++) {
        bNormalize(&light_directions[i], &light_positions[i]);
        bScale(&light_pos_colours[i], &light_pos_colours[i], 1.0f / 255.0f);
    }

    light_context->LocalDirectionMatrix.v0.x = light_directions[0].x;
    light_context->LocalDirectionMatrix.v1.x = light_directions[0].y;
    light_context->LocalDirectionMatrix.v2.x = light_directions[0].z;

    light_context->LocalDirectionMatrix.v0.y = light_directions[1].x;
    light_context->LocalDirectionMatrix.v1.y = light_directions[1].y;
    light_context->LocalDirectionMatrix.v2.y = light_directions[1].z;

    light_context->LocalDirectionMatrix.v0.z = light_directions[2].x;
    light_context->LocalDirectionMatrix.v1.z = light_directions[2].y;
    light_context->LocalDirectionMatrix.v2.z = light_directions[2].z;

    light_context->LocalColourMatrix.v0.x = light_pos_colours[0].x;
    light_context->LocalColourMatrix.v0.y = light_pos_colours[0].y;
    light_context->LocalColourMatrix.v0.z = light_pos_colours[0].z;

    light_context->LocalColourMatrix.v1.x = light_pos_colours[1].x;
    light_context->LocalColourMatrix.v1.y = light_pos_colours[1].y;
    light_context->LocalColourMatrix.v1.z = light_pos_colours[1].z;

    light_context->LocalColourMatrix.v2.x = light_pos_colours[2].x;
    light_context->LocalColourMatrix.v2.y = light_pos_colours[2].y;
    light_context->LocalColourMatrix.v2.z = light_pos_colours[2].z;

    return 1;
}

int elSetupLightContext(eDynamicLightContext *light_context /* r31 */, eShaperLightRig *shaper_lights /* r28 */, bMatrix4 *local_world /* r30 */,
                        bMatrix4 *world_view /* r27 */, bVector4 *camera_world_position /* r29 */, eView *view /* r26 */) {
    // ProfileNode profile_node;

    if (!light_context || !local_world || !shaper_lights) {
        return 0;
    } else {
        elResetLightContext(light_context);
        elSetupEnvMap(light_context, local_world, world_view, camera_world_position);
        elSetupLights(light_context, shaper_lights, reinterpret_cast<bVector3 *>(&local_world->v3), local_world, world_view, view);
        return 1;
    }
}

int elCloneLightContext(eDynamicLightContext *light_context, bMatrix4 *local_world, bMatrix4 *world_view, bVector4 *camera_world_position,
                        eView *view, eDynamicLightContext *old_context) {
    elRotateLightContext(light_context, old_context, local_world);
    elSetupEnvMap(light_context, local_world, world_view, camera_world_position);
    return 1;
}

void UpdateLightFlareParameters() {
    if (!LightFlareParametersNeedUpdating) {
        return;
    }

    if (!Attrib::Database::IsInitialized()) {
        return;
    }

    LightFlareParametersNeedUpdating = 0;

    unsigned int keyList[22] = {0x356F692A, 0xA95AB5AD, 0xB60DAC69, 0xD364E8AC, 0x30EA58B4, 0x35CE359C, 0xF51BF17F, 0xDDE4B816,
                                0x44C51B58, 0x93007A08, 0x0299E454, 0x47CB2AD7, 0xE9FAF022, 0x4FF2B81C, 0x96147A22, 0x37B7C081,
                                0x8412720E, 0x1E7FDC6E, 0x20FC956E, 0xD06FC6BB, 0xBF79F943, 0x55EAA808};

    for (int i = 0; i < 21; i++) {
        Attrib::Gen::light_flares_cg flare_coll(keyList[i], 0, nullptr);
        UMath::Vector4 col = flare_coll.colour();

        SingleLightFlareParameters[i].Colour[0] = col.x * 255.0f;
        SingleLightFlareParameters[i].Colour[1] = col.y * 255.0f;
        SingleLightFlareParameters[i].Colour[2] = col.z * 255.0f;
        SingleLightFlareParameters[i].Colour[3] = col.w * 255.0f;

        SingleLightFlareParameters[i].MaxScale = flare_coll.Maxscale();
        SingleLightFlareParameters[i].MinScale = flare_coll.MinScale();

        SingleLightFlareParameters[i].MaxSize = flare_coll.MaxSize();
        SingleLightFlareParameters[i].MinSize = flare_coll.MinSize();

        SingleLightFlareParameters[i].Power = flare_coll.Power();
        SingleLightFlareParameters[i].ZBias = flare_coll.ZBias();

        SingleLightFlareParameters[i].Texture = flare_coll.flare_texture();
    }
}

float indep_fpow(float n, float p) {
    return ePowf(n, p);
}

eLightFlare PoolLightFlareList[50]; // size: 0x960, address: 0x804615F4
uint32 PoolOfFlaresXcludeView[50];  // size: 0xC8, address: 0x8046B140
int ActivePoolIndex = 0;                // size: 0x4, address: 0x8041AB30

eLightFlare *eGetNextLightFlareInPool(uint32 XcludeViewIDs /* r7 */) {
    if (ActivePoolIndex == 50) {
        return nullptr;
    }
    PoolOfFlaresXcludeView[ActivePoolIndex] = XcludeViewIDs;
    return &PoolLightFlareList[ActivePoolIndex++];
}

void eInitLightFlarePool() {
    static int done = false;
    int PoolIndex;

    if (done) {
        return;
    }
    done = true;
    PoolIndex = 0;
    for (int i = 0; i < 50; i++) {
        eLightFlare *light_flare = &PoolLightFlareList[PoolIndex];
        light_flare->Flags = 0;
        light_flare->DirectionX = 0.0f;
        light_flare->DirectionY = 0.0f;
        light_flare->DirectionZ = -1.0f;
        light_flare->ReflectPosZ = 999.0f;
        PoolIndex++;
    }
}

int blink[4] = {0, 0, 0, 0}; // size: 0x10, address: 0x8041AB38

// UNSOLVED
void eRenderLightFlarePool(eView *view /* r30 */) {
    bMatrix4 *local_world = eGetIdentityMatrix();
    uint32 vid = 1 << view->GetID();
    for (int i = 0; i < ActivePoolIndex; i++) {
        float intensity_scale = 1.0f;
        eLightFlare *light_flare = &PoolLightFlareList[i];
        if ((light_flare->Type == 18) && (blink[0] == 0)) {
            continue;
        }
        int amBlink = bAbs((int)light_flare->PositionX % 3);

        if (((light_flare->Type != 17) || (blink[amBlink] != 0)) && (light_flare->Type != 19 || (blink[2] != 0))) {
            if (light_flare->Type == 21) {
                intensity_scale = GetSunIntensity(eGetView(1, false));
            }
            if ((vid & PoolOfFlaresXcludeView[i]) == 0) {
                if ((view->GetID() == EVIEW_FIRST_RVM) || (view->GetID() > EVIEW_SHADOWMATTE && (view->GetID() < NUM_EVIEWS))) {
                    eRenderLightFlare(view, light_flare, local_world, intensity_scale, REF_NONE, FLARE_ENV, 0.0f, 0, 1.0f);
                } else {
                    eRenderLightFlare(view, light_flare, local_world, 1.0f, REF_NONE,
                                      view->GetID() == EVIEW_FIRST_PLAYER ? FLARE_CAT_P1 : FLARE_CAT_P2, 0.0f, 0, 1.0f);
                }
            }
        }
    }
}

void eResestLightFlarePool() {
    static float time[3] = {0.0f, 0.0f, 0.0f};

    time[0] += WorldTimeElapsed;
    time[1] += WorldTimeElapsed;
    time[2] += WorldTimeElapsed;
    if (time[0] > 0.5f) {
        time[0] = 0.0f;
        blink[0] = ~blink[0];
    }
    if (time[1] > 0.4f) {
        time[1] = 0.0f;
        blink[1] = ~blink[1];
    }
    if (time[2] > 0.45f) {
        time[2] = 0.0f;
        blink[2] = ~blink[2];
    }
    ActivePoolIndex = 0;
}

float LightFlareEnvmapExpandSize = 2.0f;
float FlareRot = 0.0f;
float FlareSweep = 240.0f;
float RainInTheHeadlights = 0.96f;
TextureInfo *eLightFlareTextureInfos[3]; // size: 0xC, address: 0x8046B134

void eLightUpdateTextures() {
    for (uint32 i = 0; i < 3; i++) {
        eLightFlareTextureInfos[i] = GetTextureInfo(eLightFlareTextureNameHashes[i], 0, 0);
    }
}

static inline int eViewIsVisible(eView *view, const bVector3 *position, bMatrix4 *local_world) {
    return view->GetVisibleStateSB(position, local_world) != EVISIBLESTATE_NOT;
}

int eRenderLightFlare(eView *view, eLightFlare *light_flare, bMatrix4 *local_world, float intensity_scale,
                      eLightReflexionType ReflexionAction, flareType destinationType, float RefelectionOverride,
                      uint32 ColourOverRide, float sizescale) {
    if (!DrawLightFlares) {
        return 0;
    }

    if (destinationType == FLARE_REFLECT) {
        reflectionStretch = view->Precipitation
                                ? MINreflectionStretch +
                                      (MAXreflectionStretch - MINreflectionStretch) * view->Precipitation->GetRoadDampness()
                                : MINreflectionStretch;
    }

    Camera *camera = view->GetCamera();

    if (light_flare == 0 || local_world == 0 || camera == 0) {
        return 0;
    }

    int flare_type = light_flare->Type;
    int flare_flags = light_flare->Flags;

    if (RefelectionOverride != 0.0f) {
        bVector3 pos(light_flare->PositionX, light_flare->PositionY, light_flare->PositionZ);
        eMulVector(&pos, local_world, &pos);

        light_flare->ReflectPosZ = RefelectionOverride - (pos.z - RefelectionOverride);
    } else if (light_flare->ReflectPosZ == 999.0f || ReflexionAction == REF_TOPO) {
        bVector3 pos(light_flare->PositionX, light_flare->PositionY, light_flare->PositionZ);
        UMath::Vector3 usPoint;

        eMulVector(&pos, local_world, &pos);
        eUnSwizzleWorldVector(pos, (bVector3 &)usPoint);

        if (IsGameFlowInGame()) {
            float height;
            WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(usPoint, height, 0);
            light_flare->ReflectPosZ = height - (pos.z - height);
        } else {
            light_flare->ReflectPosZ = pos.z;
        }
    }

    bVector3 flare_position = *light_flare->GetPosition();
    bMatrix4 flocal_world = *local_world;
    bVector3 world_position;
    bVector3 flare_direction;
    LightFlareParameters *flare_params;

    if (destinationType == FLARE_REFLECT) {
        bVector3 aabb_min;
        bVector3 aabb_max;

        if (light_flare->ReflectPosZ == 999.0f || light_flare->ReflectPosZ == 9999.0f) {
            return 0;
        }

        bMulMatrix(&world_position, &flocal_world, &flare_position);

        world_position.z = light_flare->ReflectPosZ;

        bInitializeBoundingBox(&aabb_min, &aabb_max, &world_position);

        bExpandBoundingBox(&aabb_min, &aabb_max, &world_position, reflectionStretch);

        if (view->GetVisibleState(&aabb_min, &aabb_max, 0) == EVISIBLESTATE_NOT) {
            return 0;
        }

        flare_direction = *light_flare->GetDirection();
        flare_direction.z = -flare_direction.z;
    } else if (destinationType == FLARE_ENV) {
        bVector3 aabb_min;
        bVector3 aabb_max;

        bMulMatrix(&world_position, &flocal_world, &flare_position);

        float expandSize = LightFlareEnvmapExpandSize;

        bInitializeBoundingBox(&aabb_min, &aabb_max, &world_position);
        bExpandBoundingBox(&aabb_min, &aabb_max, &world_position, expandSize);

        if (view->GetVisibleState(&aabb_min, &aabb_max, 0) == EVISIBLESTATE_NOT) {
            return 0;
        }

        flare_direction = *light_flare->GetDirection();
    } else {
        if (destinationType == FLARE_CAT_P1 || destinationType == FLARE_CAT_P2) {
            if (flare_type != ELF_BLINKING_AMBER && flare_type != ELF_BLINKING_RED && flare_type != ELF_HAND_FLARE &&
                flare_type != ELF_BLINKING_GREEN) {
                CameraMover *cameraMover = view->GetCameraMover();

                if (cameraMover) {
                    CameraAnchor *cameraAnchor = cameraMover->GetAnchor();

                    if (cameraAnchor) {
                        bVector3 catVect = *light_flare->GetPosition() - *cameraAnchor->GetGeometryPosition();
                        bNormalize(&catVect, &catVect);

                        const bVector3 *FV = cameraAnchor->GetForwardVector();
                        float CatAngle = bDot(FV, &catVect);

                        if (CatAngle >= RainInTheHeadlights) {
                            intensity_scale = (CatAngle - RainInTheHeadlights) / (1.0f - RainInTheHeadlights);
                        } else {
                            return 0;
                        }
                    }
                }
            }
        }

        bMulMatrix(&world_position, &flocal_world, &flare_position);

        if (!eViewIsVisible(view, &world_position, 0)) {
            return 0;
        }

        flare_direction = *light_flare->GetDirection();
    }

    bVector3 camera_position = *camera->GetPosition();
    bVector3 normal_view_dir = camera_position - world_position;
    float flare_dist = bDot(normal_view_dir, normal_view_dir);

    float falloff_scale = 1.0f;
    float FlareFalloffDist = FlareFalloffDistanceNORM;
    float FlareCutoffDist = FlareCutoffDistanceNORM;

    if (destinationType == FLARE_ENV) {
        FlareFalloffDist = FlareFalloffDistanceENV;
        FlareCutoffDist = FlareCutoffDistanceENV;
    }

    if (flare_dist > FlareCutoffDist) {
        return 0;
    }

    if (flare_dist > FlareFalloffDist) {
        falloff_scale -= (flare_dist - FlareFalloffDist) / (FlareCutoffDist - FlareFalloffDist);
    }

    bNormalize(&normal_view_dir, &normal_view_dir);

    bVector3 world_direction;
    bMatrix4 tempM = *local_world;
    tempM.v3 = bVector4(0.0f, 0.0f, 0.0f, 0.0f);

    bMulMatrix(&world_direction, &tempM, &flare_direction);

    float angle = bDot(normal_view_dir, world_direction);

    if (flare_flags & ELF_BIDIRECTIONAL) {
        angle = bAbs(angle);
    } else if (angle < 0.0f) {
        angle = 0.0f;
    }

    bMatrix4 *world_view = camera->GetCameraMatrix();
    bVector3 basis_x;
    bVector3 basis_y;
    bVector3 basis_z;

    basis_x.x = world_view->v0.x;
    basis_x.y = world_view->v1.x;
    basis_x.z = world_view->v2.x;
    basis_y.x = world_view->v0.y;
    basis_y.y = world_view->v1.y;
    basis_y.z = world_view->v2.y;
    basis_z.x = world_view->v0.z;
    basis_z.y = world_view->v1.z;
    basis_z.z = world_view->v2.z;

    bMatrix4 fworld_view = *world_view;
    float camera_h = view->H;
    bVector3 view_position;

    bMulMatrix(&view_position, &fworld_view, &world_position);

    float scale_amount = view_position.z / camera_h;

    bVector3 tworld_position;
    bVector3 tnormal_cam_dir;

    tworld_position = world_position;
    tnormal_cam_dir = normal_view_dir;

    for (int i = 0; i < 2; i++) {
        flare_params = LightFlareParameterIndicies[flare_type][i];

        if (flare_params) {
            TextureInfo *texture_info;

            if (destinationType == FLARE_REFLECT) {
                texture_info = GetTextureInfo(0x3394FE62, 0, 0);
            } else {
                texture_info = eLightFlareTextureInfos[flare_params->Texture];
            }

            if (texture_info == 0) {
                continue;
            }

            intensity = intensity_scale;

            if (flare_flags & ELF_UNI_DIRECTIONAL) {
                intensity *= indep_fpow(angle, flare_params->Power);

                if (intensity > 1.0f) {
                    intensity = 1.0f;
                }
            }

            float flare_size = flare_params->MinSize + (flare_params->MaxSize - flare_params->MinSize) * intensity;

            if (flare_size <= 0.0f) {
                continue;
            }

            bVector3 world_position;

            bScaleAdd(&world_position, &tworld_position, &tnormal_cam_dir, flare_params->ZBias);

            if (scale_amount < flare_params->MinScale) {
                scale_amount = flare_params->MinScale;
            }
            if (scale_amount > flare_params->MaxScale) {
                scale_amount = flare_params->MaxScale;
            }

            flare_size *= scale_amount;
            flare_size *= sizescale;

            ePoly poly;
            float px, py, pz;

            float fr = flare_params->Colour[0] * intensity;
            float fg = flare_params->Colour[1] * intensity;
            float fb = flare_params->Colour[2] * intensity;
            float fa = flare_params->Colour[3] * intensity * falloff_scale;

            bMatrix4 *local_world = eGetIdentityMatrix();
            uint32 abgr;

            if (destinationType == FLARE_NORM) {
                bMatrix4 RMat;
                bVector4 position2d;

                bNormalize(&basis_z, &basis_z);

                view->GetScreenPosition((bVector3 *)&position2d, &tworld_position);
                FlareRot = FlareSweep * (position2d.x / (float)view->GetPixelWidth());

                eCreateAxisRotationMatrix(&RMat, basis_z, bDegToAng(FlareRot));
                eMulVector(&basis_x, &RMat, &basis_x);
                eMulVector(&basis_y, &RMat, &basis_y);
            }

            basis_x *= flare_size;
            basis_y *= flare_size;

            if (destinationType == FLARE_REFLECT) {
                if (light_flare->PositionZ - light_flare->ReflectPosZ < 4.0f) {
                    basis_y *= reflectionStretch * 0.5f;
                } else {
                    basis_y *= reflectionStretch;
                }
            }

            if (ColourOverRide != 0) {
                uint32 a = (int)fa;

                abgr = ColourOverRide | a;
            } else {
                uint32 r = (int)fr;
                uint32 g = (int)fg;
                uint32 b = (int)fb;
                uint32 a = (int)fa;

                abgr = a | (b << 8) | (g << 16) | (r << 24);
            }

            *(uint32 *)poly.Colours[0] = abgr;
            *(uint32 *)poly.Colours[1] = abgr;
            *(uint32 *)poly.Colours[2] = abgr;
            *(uint32 *)poly.Colours[3] = abgr;

            poly.Vertices[0] = world_position + (-basis_x - basis_y);
            poly.Vertices[1] = world_position + (basis_x - basis_y);
            poly.Vertices[2] = world_position + (basis_x + basis_y);
            poly.Vertices[3] = world_position + (-basis_x + basis_y);

            view->Render(&poly, texture_info, local_world, 0, 0.0f);
        }
    }

    return 1;
}

void eRenderWorldLightFlares(eView *view /* r26 */, flareType type /* r24 */) {
    bMatrix4 *local_world;
    int num_visible;
    int num_scenery_culled;
    int total;
    DrivableScenerySection *scenery_section;
    bVector2 *camera_position;

    if (DrawLightFlares != 0 && IsGameFlowInGame()) {
        camera_position = reinterpret_cast<bVector2 *>(view->GetCamera()->GetPosition());
        local_world = eGetIdentityMatrix();
        scenery_section = TheVisibleSectionManager.FindDrivableSection(camera_position);
        if (!scenery_section) {
            return;
        }
        for (int i = 0; i < scenery_section->NumVisibleSections; i++) {
            VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(scenery_section->GetVisibleSection(i));
            if (user_info) {
                eLightFlarePackHeader *pack = user_info->pLightFlarePack;
                if (pack && view->GetVisibleState(&pack->BBoxMin, &pack->BBoxMax, nullptr) != EVISIBLESTATE_NOT) {
                    for (eLightFlare *light_flare = pack->LightFlareList.GetHead(); light_flare != pack->LightFlareList.EndOfList();
                         light_flare = light_flare->GetNext()) {
                        eRenderLightFlare(view, light_flare, local_world, 1.0f, REF_NONE, type, 0.0f, 0, 1.0f);
                    }
                }
            }
        }
    }
}

void RestoreShaperRig(eShaperLightRig *ShaperRigP, uint32 slot, eShaperLightRig *ShaperRigBP) {
    ShaperRigP->Lights[slot].CameraSpace = ShaperRigBP->Lights[slot].CameraSpace;
    ShaperRigP->Lights[slot].Red = ShaperRigBP->Lights[slot].Red;
    ShaperRigP->Lights[slot].Green = ShaperRigBP->Lights[slot].Green;
    ShaperRigP->Lights[slot].Blue = ShaperRigBP->Lights[slot].Blue;
    ShaperRigP->Lights[slot].Scale = ShaperRigBP->Lights[slot].Scale;
    ShaperRigP->Lights[slot].Theta = ShaperRigBP->Lights[slot].Theta;
    ShaperRigP->Lights[slot].Phi = ShaperRigBP->Lights[slot].Phi;
}

void AddQuickDynamicLight(eShaperLightRig *ShaperRigP, uint32 slot, float r, float g, float b, float intensity, bVector3 *position) {
    uint32 mask = 1 << (slot - 2);

    ShaperRigP->NumOverideSlots |= mask;
    ShaperRigP->position = *position;

    ShaperRigP->Lights[slot].CameraSpace = LIGHT_WORLD_POSITION;
    ShaperRigP->Lights[slot].Red = r;
    ShaperRigP->Lights[slot].Green = g;
    ShaperRigP->Lights[slot].Blue = b;
    ShaperRigP->Lights[slot].Scale = intensity;
}

void AdjustQuickDynamicLight(eShaperLightRig *ShaperRigP, bVector3 *MyPosition) {
    for (int i = 0; i < 0; i++) {
        uint32 slot;
        uint32 mask;
        if (0 == 0) {
            bVector3 direction = ShaperRigP->position - *MyPosition;
            bVector3 spherical_direction;
            bVector3 idk;
        }
    }
}

void eLightMaterial::BuildData() {
    this->UpdatePlatInfo();
}

eLightMaterial *elGetLightMaterial(uint32 name_hash /* r10 */) {
    for (eLightMaterial *elm = LightMaterialList.GetHead(); elm != LightMaterialList.EndOfList(); elm = elm->GetNext()) {
        if (elm->NameHash == name_hash) {
            return elm;
        }
    }
    return DefaultLightMaterial;
}

eLightMaterial *elGetDefaultLightMaterial() {
    return DefaultLightMaterial;
}

void elInit() {
    bMemSet(DefaultLightMaterial, 0, sizeof(*DefaultLightMaterial));
    elInitPlat();
    eInitLightFlarePool();
}

void elBeginFrame() {}
