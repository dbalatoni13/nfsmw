#include "ScreenEffects.hpp"

#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/Src/World/WeatherMan.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

static unsigned int AccumulationBufferNeedsFlush = 0;
ScreenEffectPaletteDef SE_PaletteFile[EFX_NUMBER];

// TODO move these
extern eModel *pVisibleZoneBoundaryModel;
extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;
extern float GlareFalloff;
extern float GlareFallon;
extern float TUNHEIGHT;
extern int debugflash;
extern TrackPathZone *zoneB[2];

void InitScreenEFX();

ScreenEffectDB::ScreenEffectDB() {
    SE_time = 0.0f;
    for (int i = 0; i < SE_NUM_TYPES; i++) {
        SE_inf[i].active = 0;
        SE_data[i].r = 0.0f;
        SE_data[i].g = 0.0f;
        SE_data[i].b = 0.0f;
        SE_data[i].a = 0.0f;
        for (int j = 0; j < 14; j++) {
            SE_data[i].data[j] = 0.0f;
        }
        SE_data[i].intensity = 0.0f;
        SE_data[i].UpdateFnc = nullptr;
        numType[i] = 0;
    }
    InitScreenEFX();
}
void ScreenEffectDB::Update(float deltatime) {
    SE_time += deltatime;

    for (int i = 0; i < SE_NUM_TYPES; i++) {
        if (IsActive(static_cast<ScreenEffectType>(i)) == 1) {
            SE_inf[i].frameNum++;
            switch (GetController(static_cast<ScreenEffectType>(i))) {
                case SEC_FRAME:
                case SEC_FUNCTION:
                    RemoveScreenEffect(static_cast<ScreenEffectType>(i));
                    break;
                default:
                    break;
            }
        }
    }
}

void ScreenEffectDB::AddScreenEffect(ScreenEffectType type, float intensity, float r, float g, float b) {
    ScreenEffectDef info;

    info.intensity = intensity;
    info.r = r;
    info.g = g;
    info.b = b;
    info.UpdateFnc = nullptr;
    AddScreenEffect(type, &info, 1, SEC_FRAME);
}

void ScreenEffectDB::AddScreenEffect(ScreenEffectType type, ScreenEffectDef *info, unsigned int lock, ScreenEffectControl controller) {
    if (lock != 0) {
        if (info) {
            SE_data[type] = *info;
        }
        numType[type] = 1;
    } else {
        numType[type]++;
        float influence = static_cast<float>(numType[type]) / static_cast<float>(numType[type] + 1);
        float invFluence = 1.0f - influence;

        SE_data[type].r = influence * SE_data[type].r + invFluence * info->r;
        SE_data[type].g = influence * SE_data[type].g + invFluence * info->g;
        SE_data[type].b = influence * SE_data[type].b + invFluence * info->b;
        SE_data[type].a = influence * SE_data[type].a + invFluence * info->a;
        SE_data[type].intensity = influence * SE_data[type].intensity + invFluence * info->intensity;
    }

    SE_inf[type].active = 1;
    if (SE_data[type].UpdateFnc) {
        SE_data[type].UpdateFnc(type, this);
    } else {
        SetController(type, controller);
    }

    if (SE_data[type].intensity < 0.01f) {
        SE_inf[type].active = 0;
    }
}

void ScreenEffectDB::AddPaletteEffect(ScreenEffectPalette palette) {
    AddPaletteEffect(&SE_PaletteFile[palette]);
}

void ScreenEffectDB::AddPaletteEffect(ScreenEffectPaletteDef *palette) {
    for (int i = 0; i < palette->NumEffects; i++) {
        AddScreenEffect(palette->SE_type[i], &palette->SE_Def[i], 1, palette->SE_Controller[i]);
    }
}

void InitScreenEFX() {}

void TickSFX() {
    static uint32 ticS = 0;
    if (IsGameFlowInGame()) {
        if (ticS != eFrameCounter - 1) {
            UpdateAllScreenEFX();
        }
        ticS = eFrameCounter;
    }
}

void UpdateAllScreenEFX() {
    for (int i = 1; i < 3; i++) {
        eView *view = eGetView(i, false);
        if (view->IsActive()) {
            eGetView(i, false)->ScreenEffects->Update(0.033333335f);
            if (debugflash != 0) {
                debugflash = 0;
                eGetView(i, false)->ScreenEffects->AddPaletteEffect(EFX_CAMERA_FLASH);
            }
        }
    }
}

void FlushAccumulationBuffer() {
    AccumulationBufferNeedsFlush = 1;
}

void AccumulationBufferFlushed() {
    AccumulationBufferNeedsFlush = 0;
}

unsigned int QueryFlushAccumulationBuffer() {
    return AccumulationBufferNeedsFlush;
}
void DoTinting(eView *view) {
    ScreenEffectDef SE_def;
    unsigned int r;
    unsigned int g;
    unsigned int b;
    float intense;

    if (IsRainDisabled()) {
        return;
    }

    if (view->Precipitation) {
        intense = view->Precipitation->GetCloudIntensity();
    } else {
        intense = 0.0f;
    }

    if (0.0f < intense) {
        if (view->Precipitation) {
            view->Precipitation->GetPrecipFogColour(&r, &g, &b);
        }
        SE_def.r = static_cast<float>(r);
        SE_def.g = static_cast<float>(g);
        SE_def.a = 128.0f;
        SE_def.UpdateFnc = 0;
        SE_def.intensity = intense;
        SE_def.b = static_cast<float>(b);
        view->ScreenEffects->AddScreenEffect(SE_TINT, &SE_def, 1, SEC_FRAME);
    }
}

// UNSOLVED, functionally matching, just regswaps: https://decomp.me/scratch/Ar2tQ
void DoTunnelBloom(eView *view) {
    int vIndex = 1;
    float BaseGlare; // TODO
    if (view->GetID() == EVIEW_PLAYER1) {
        vIndex = 0;
    }

    if (!view->IsActive()) {
        return;
    }

    CameraMover *cameraMover = view->GetCameraMover();
    if (!cameraMover) {
        return;
    }

    CameraAnchor *cameraAnchor = cameraMover->GetAnchor();
    if (!cameraAnchor) {
        return;
    }

    bVector3 *MyCarPos = cameraAnchor->GetGeometryPosition();
    Camera *view_camera = view->GetCamera();
    bVector3 *CameraPosition = view_camera->GetPosition();
    bVector3 *CameraDirection = view_camera->GetDirection();
    bVector2 twoDpos(CameraPosition->x, CameraPosition->y);

    bVector3 endVector;

    static bVector3 lcamPosInside[2];
    static float dataBackup[2][12];
    static GenericRegion *regionB[2] = {};

    bVector3 posScreen;
    TrackPathZone *zone = nullptr;
    TrackPathZone *zoneBP = zoneB[vIndex];
    if (zoneBP && zoneBP->IsPointInside(&twoDpos)) {
        zone = zoneB[vIndex];
    } else {
        zone = TheTrackPathManager.FindZone(&twoDpos, TRACK_PATH_ZONE_TUNNEL, 0);
    }

    if (zone && zone->GetElevation() > MyCarPos->z) {
        bVector2 p0;
        bVector2 p1;
        lcamPosInside[vIndex] = *CameraPosition;
        float angleCos = 0.0f;
        GenericRegion *EndTunnelP = GetClosestRegionInView(view, &endVector, &angleCos);
        if (EndTunnelP) {
            ScreenEffectDef SE_def;
            bVector2 endP(endVector.x, endVector.y);
            float len = zone->GetSegmentNextTo(&endP, &p0, &p1);
            if (len != -1.0f && len < 40.0f) {
                {
                    bVector3 p3(endVector);
                    UMath::Vector3 usPoint;
                    eUnSwizzleWorldVector(p3, *reinterpret_cast<bVector3 *>(&usPoint));

                    float height;
                    WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(usPoint, height, 0);

                    SE_def.data[2] = p0.x + CameraDirection->x;
                    dataBackup[vIndex][0] = SE_def.data[2];

                    SE_def.data[3] = p0.y + CameraDirection->y;
                    dataBackup[vIndex][1] = SE_def.data[3];

                    SE_def.data[4] = height + CameraDirection->z;
                    dataBackup[vIndex][2] = SE_def.data[4];

                    SE_def.data[5] = p1.x + CameraDirection->x;
                    dataBackup[vIndex][3] = SE_def.data[5];

                    SE_def.data[6] = p1.y + CameraDirection->y;
                    dataBackup[vIndex][4] = SE_def.data[6];

                    SE_def.data[7] = height + CameraDirection->z;
                    dataBackup[vIndex][5] = SE_def.data[7];

                    SE_def.data[8] = p0.x + CameraDirection->x;
                    dataBackup[vIndex][6] = SE_def.data[8];

                    SE_def.data[9] = p0.y + CameraDirection->y;
                    dataBackup[vIndex][7] = SE_def.data[9];

                    SE_def.data[10] = height + TUNHEIGHT + CameraDirection->z;
                    dataBackup[vIndex][8] = SE_def.data[10];

                    SE_def.data[11] = p1.x + CameraDirection->x;
                    dataBackup[vIndex][9] = SE_def.data[11];

                    SE_def.data[12] = p1.y + CameraDirection->y;
                    dataBackup[vIndex][10] = SE_def.data[12];

                    SE_def.data[13] = height + TUNHEIGHT + CameraDirection->z;
                    dataBackup[vIndex][11] = SE_def.data[13];

                    SE_def.r = 128.0f;
                    SE_def.g = 128.0f;
                    SE_def.b = 128.0f;
                    SE_def.a = 128.0f;
                    SE_def.UpdateFnc = nullptr;
                }

                if (regionB[vIndex] != EndTunnelP) {
                    view->ScreenEffects->SetDATA(SE_GLARE, 0.0f, 1);
                }
                regionB[vIndex] = EndTunnelP;
                if (zoneB[vIndex] != zone) {
                    view->ScreenEffects->SetDATA(SE_GLARE, 0.0f, 1);
                }

                zoneB[vIndex] = zone;
                {
                    bVector2 r = p0 - twoDpos;
                    bVector2 v(p1.y - p0.y, p0.x - p1.x);
                    bNormalize(&v, &v);
                    len = bDot(&v, &r);
                    len = bAbs(len);
                }

                if (len < 17.0f) {
                    SE_def.data[1] = view->ScreenEffects->GetDATA(SE_GLARE, 1) * (len / 1.7f);
                } else {
                    if (view->ScreenEffects->GetDATA(SE_GLARE, 1) < 1.0f) {
                        SE_def.data[1] = view->ScreenEffects->GetDATA(SE_GLARE, 1) + GlareFallon;
                    } else {
                        SE_def.data[1] = 1.0f;
                    }
                }

                SE_def.intensity = SE_def.data[1];
                view->ScreenEffects->AddScreenEffect(SE_GLARE, &SE_def, 1, SEC_FRAME);

                if (view->Precipitation && 0.0f < view->Precipitation->GetRainIntensity()) {
                    view->Precipitation->IsValidRainCurtainPos = CT_OVERIDE;
                    view->Precipitation->AttachRainCurtain(SE_def.data[8], SE_def.data[9], SE_def.data[10], SE_def.data[11], SE_def.data[12],
                                                           SE_def.data[13], SE_def.data[2], SE_def.data[3], SE_def.data[4], SE_def.data[5],
                                                           SE_def.data[6], SE_def.data[7]);
                }
            }
        }
        return;
    }

    if (0.0f < view->ScreenEffects->GetIntensity(SE_GLARE)) {
        ScreenEffectDef SE_def;
        SE_def.r = 128.0f;
        SE_def.g = 128.0f;
        SE_def.b = 128.0f;
        SE_def.a = 128.0f;
        SE_def.UpdateFnc = nullptr;

        SE_def.intensity = view->ScreenEffects->GetIntensity(SE_GLARE) - GlareFalloff;
        SE_def.data[1] = SE_def.intensity;

        {
            bVector3 midpoint((dataBackup[vIndex][0] + dataBackup[vIndex][3]) * 0.5f, (dataBackup[vIndex][1] + dataBackup[vIndex][4]) * 0.5f,
                              (dataBackup[vIndex][2] + dataBackup[vIndex][5]) * 0.5f);
            bVector3 ToGlare = *CameraPosition - lcamPosInside[vIndex];
            ToGlare += *CameraDirection * 1.0f;

            if (SE_def.intensity > 0.0f) {
                SE_def.data[2] = ToGlare.x + dataBackup[vIndex][0];
                SE_def.data[3] = ToGlare.y + dataBackup[vIndex][1];
                SE_def.data[4] = ToGlare.z + dataBackup[vIndex][2];

                SE_def.data[5] = ToGlare.x + dataBackup[vIndex][3];
                SE_def.data[6] = ToGlare.y + dataBackup[vIndex][4];
                SE_def.data[7] = ToGlare.z + dataBackup[vIndex][5];

                SE_def.data[8] = ToGlare.x + dataBackup[vIndex][6];
                SE_def.data[9] = ToGlare.y + dataBackup[vIndex][7];
                SE_def.data[10] = ToGlare.z + dataBackup[vIndex][8];

                SE_def.data[11] = ToGlare.x + dataBackup[vIndex][9];
                SE_def.data[12] = ToGlare.y + dataBackup[vIndex][10];
                SE_def.data[13] = ToGlare.z + dataBackup[vIndex][11];
                view->ScreenEffects->AddScreenEffect(SE_GLARE, &SE_def, 1, SEC_FRAME);
            }
        }
    }
}
