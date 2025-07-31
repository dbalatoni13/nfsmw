#include "Sun.hpp"
#include "Rain.hpp"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "World.hpp"

SunChunkInfo *SunInfoTable;
int NumSunInfo;
SunChunkInfo *SunInfo;
TextureInfo *SunTextures[5];
bVector3 SunPosition;
float WorldLightDirectionVector[4];

int LoaderSun(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_SUN_INFOS) {
        return 0;
    }
    SunInfoTable = reinterpret_cast<SunChunkInfo *>(chunk->GetAlignedData(16));
    NumSunInfo = chunk->GetAlignedSize(16) / sizeof(SunChunkInfo);
    for (int n = 0; n < NumSunInfo; n++) {
        SunChunkInfo *sun_info = &SunInfoTable[n];
        bPlatEndianSwap(reinterpret_cast<int *>(sun_info));
        bPlatEndianSwap(&sun_info->NameHash);
        bPlatEndianSwap(&sun_info->PositionX);
        bPlatEndianSwap(&sun_info->PositionY);
        bPlatEndianSwap(&sun_info->PositionZ);
        bPlatEndianSwap(&sun_info->CarShadowPositionX);
        bPlatEndianSwap(&sun_info->CarShadowPositionY);
        bPlatEndianSwap(&sun_info->CarShadowPositionZ);

        for (int layer_num = 0; layer_num < 6; layer_num++) {
            bPlatEndianSwap(reinterpret_cast<int *>(&sun_info->SunLayers[layer_num]));
            bPlatEndianSwap(reinterpret_cast<int *>(&sun_info->SunLayers[layer_num].AlphaType));
            bPlatEndianSwap(&sun_info->SunLayers[layer_num].IntensityScale);
            bPlatEndianSwap(&sun_info->SunLayers[layer_num].Size);
            bPlatEndianSwap(&sun_info->SunLayers[layer_num].OffsetX);
            bPlatEndianSwap(&sun_info->SunLayers[layer_num].OffsetY);
            bPlatEndianSwap(&sun_info->SunLayers[layer_num].Angle);
            bPlatEndianSwap(&sun_info->SunLayers[layer_num].SweepAngleAmount);
        }
    }
    return 1;
}

void SetCurrentSunInfo(unsigned int name_hash) {
    SunInfo = nullptr;
    if (name_hash != 0) {
        for (int n = 0; n < NumSunInfo; n++) {
            SunChunkInfo *sun_info = &SunInfoTable[n];
            if (SunInfoTable[n].NameHash == name_hash) {
                SunInfo = sun_info;
            }
        }
    }
    if (!SunInfo) {
        SunPosition.x = 0.0f;
        SunPosition.y = 0.0f;
        SunPosition.z = 0.0f;
        return;
    }
    if (SunInfo->Version != 2) {
        SunInfo = nullptr;
        SunPosition.x = 0.0f;
        SunPosition.y = 0.0f;
        SunPosition.z = 0.0f;
    } else {
        SunPosition.x = SunInfo->PositionX;
        SunPosition.y = SunInfo->PositionY;
        SunPosition.z = SunInfo->PositionZ;
    }
    bNormalize(reinterpret_cast<bVector3 *>(WorldLightDirectionVector), &SunPosition);
    WorldLightDirectionVector[3] = 0.0f;
}

void SetCurrentSunInfo() {
    unsigned int name_hash = bStringHash("MIDDAY");
    if (GetCurrentTimeOfDay() == eTOD_SUNSET) {
        name_hash = bStringHash("SUNSET");
    }
    SetCurrentSunInfo(name_hash);
}

int UnloaderSun(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_SUN_INFOS) {
        return 0;
    }
    SunInfoTable = nullptr;
    NumSunInfo = 0;
    return 1;
}

void SunTrackLoader(void) {
    SetCurrentSunInfo();
    SunTextures[0] = GetTextureInfo(BinHash<'S', 'U', 'N', 'C', 'E', 'N', 'T', 'E', 'R'>::value, 1, 0);
    SunTextures[1] = GetTextureInfo(BinHash<'S', 'U', 'N', 'H', 'A', 'L', 'O'>::value, 1, 0);
    SunTextures[2] = GetTextureInfo(BinHash<'S', 'U', 'N', 'M', 'A', 'J', 'O', 'R', 'R', 'A', 'Y', 'S'>::value, 1, 0);
    SunTextures[3] = GetTextureInfo(BinHash<'S', 'U', 'N', 'M', 'I', 'N', 'O', 'R', 'R', 'A', 'Y', 'S'>::value, 1, 0);
    SunTextures[4] = GetTextureInfo(BinHash<'S', 'U', 'N', 'R', 'I', 'N', 'G'>::value, 1, 0);
}

void SunTrackUnloader(void) {
    SunTextures[0] = nullptr;
    SunTextures[1] = nullptr;
    SunTextures[2] = nullptr;
    SunTextures[3] = nullptr;
    SunTextures[4] = nullptr;
}

// UNSOLVED
void RenderSunAsFlare() {
    bVector3 sp8; // position3d

    if (SunInfo) {
        float fVar1;
        float fVar2;
        float fVar3;
        float fVar4;
        float fVar5;
        float fVar6;
        float fVar7;
        bVector3 local_38;

        sp8.x = SunInfo->PositionX;
        sp8.y = SunInfo->PositionY;
        sp8.z = SunInfo->PositionZ;

        bVector3 sp18(*eGetView(1, false)->GetCamera()->GetPosition());
        bVector3 sp28; // ToSun
        float var_r31; // recipdistance2sun

        fVar6 = 1.0f / bLength(sp28);
        local_38.x = fVar3 * fVar6 * 40.0f;
        sp28.pad = fVar5 + fVar2 * fVar6 * 40.0f;
        sp28.z = fVar4 + fVar1 * fVar6 * 40.0f;
        // bVector3((bVector3 *)&local_38.z, (bVector3 *)&local_28.z);
        fVar3 = sp28.x;
        fVar2 = local_38.pad;
        fVar1 = local_38.z;
        eLightFlare *light_flare = eGetNextLightFlareInPool(0x3e);
        if (light_flare) {
            light_flare->Flags = 2;
            light_flare->PositionX = fVar1;
            light_flare->PositionY = fVar2;
            light_flare->Type = 21;
            light_flare->PositionZ = fVar3;
        }
        sp28.z = fVar4 + SunInfo->PositionX * fVar6 * 60.0f;
        sp28.pad = fVar5 + SunInfo->PositionY * fVar6 * 60.0f;
        local_38.x = SunInfo->PositionZ * fVar6 * 60.0f;
        // bVector3((bVector3 *)&local_38.z, (bVector3 *)&local_28.z);
        light_flare = eGetNextLightFlareInPool(0x3f0036);
        if (light_flare) {
            light_flare->Flags = 2;
            light_flare->PositionX = local_38.z;
            light_flare->PositionY = local_38.pad;
            light_flare->Type = 21;
            light_flare->PositionZ = sp28.x;
        }
    }
}

float GetSunIntensity(eView *view) {
    float amount = 1.0f;
    if (view->Precipitation) {
        amount = 1.0f - view->Precipitation->GetCloudIntensity();
    }
    if (AmIinATunnel(view, 1) != 0) {
        amount = 0.0f;
    }
    return amount;
}

void GetSunPos(eView *view, float *x, float *y, float *z) {
    if (!SunInfo) {
        return;
    }
    if (view) {
        Camera *view_camera = view->GetCamera();
        bVector3 CamPosWORLD(*view_camera->GetPosition());
        *x = SunInfo->PositionX + CamPosWORLD.x;
        *y = SunInfo->PositionY + CamPosWORLD.y;
        *z = SunInfo->PositionZ + CamPosWORLD.z;
    } else {
        *x = SunInfo->PositionX;
        *y = SunInfo->PositionY;
        *z = SunInfo->PositionZ;
    }
}
