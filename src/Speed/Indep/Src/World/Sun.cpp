#include "Sun.hpp"
#include "Rain.hpp"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "TimeOfDay.hpp"
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

void SetCurrentSunInfo(uint32 name_hash) {
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
    uint32 name_hash = bStringHash("MIDDAY");
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
    SunTextures[0] = GetTextureInfo(BINHASH(SUNCENTER), 1, 0);
    SunTextures[1] = GetTextureInfo(BINHASH(SUNHALO), 1, 0);
    SunTextures[2] = GetTextureInfo(BINHASH(SUNMAJORRAYS), 1, 0);
    SunTextures[3] = GetTextureInfo(BINHASH(SUNMINORRAYS), 1, 0);
    SunTextures[4] = GetTextureInfo(BINHASH(SUNRING), 1, 0);
}

void SunTrackUnloader(void) {
    SunTextures[0] = nullptr;
    SunTextures[1] = nullptr;
    SunTextures[2] = nullptr;
    SunTextures[3] = nullptr;
    SunTextures[4] = nullptr;
}

void RenderSunAsFlare() {
    bVector3 position3d;

    if (SunInfo) {
        position3d.x = SunInfo->PositionX;
        position3d.y = SunInfo->PositionY;
        position3d.z = SunInfo->PositionZ;

        bVector3 CamPos = *eGetView(1, false)->GetCamera()->GetPosition();
        bVector3 ToSun = position3d;
        float recipdistance2sun = 1.0f / bLength(position3d);

        ToSun *= recipdistance2sun;
        ToSun *= 40.0f;
        position3d = CamPos + ToSun;
        eLightFlare *eLightFlare = eGetNextLightFlareInPool(0x3e);
        if (eLightFlare) {
            eLightFlare->Flags = 2;
            eLightFlare->PositionX = position3d.x;
            eLightFlare->PositionY = position3d.y;
            eLightFlare->PositionZ = position3d.z;
            eLightFlare->Type = 21; // eLightFlareType::ELF_SUN_FLARE, or eSingleLightFlares::ESLF_SUN_FLARE
        }

        position3d.x = SunInfo->PositionX;
        position3d.y = SunInfo->PositionY;
        position3d.z = SunInfo->PositionZ;

        ToSun = position3d;
        ToSun *= recipdistance2sun;
        ToSun *= 60.0f;
        position3d = CamPos + ToSun;
        eLightFlare = eGetNextLightFlareInPool(0x3f0036);
        if (eLightFlare) {
            eLightFlare->Flags = 2;
            eLightFlare->PositionX = position3d.x;
            eLightFlare->PositionY = position3d.y;
            eLightFlare->PositionZ = position3d.z;
            eLightFlare->Type = 21; // eLightFlareType::ELF_SUN_FLARE, or eSingleLightFlares::ESLF_SUN_FLARE
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
