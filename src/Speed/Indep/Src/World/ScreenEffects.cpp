#include "ScreenEffects.hpp"

#include "Scenery.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/Src/World/WeatherMan.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

static unsigned int AccumulationBufferNeedsFlush = 0;
ScreenEffectPaletteDef SE_PaletteFile[EFX_NUMBER];

class eViewRenderShim : public eView {
  public:
    void Render(eModel *model, bMatrix4 *matrix, eLightContext *light_context, unsigned int a4, unsigned int a5, unsigned int a6);
};

extern eModel *pVisibleZoneBoundaryModel;
extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;
extern float GlareFalloff;
extern float GlareFallon;
extern float TUNHEIGHT;
extern int debugflash;
extern TrackPathZone *zoneB[2];

static inline UMath::Vector3 &bConvertToBond(UMath::Vector3 &dest, const bVector3 &v) {
    bConvertToBond(*reinterpret_cast<bVector3 *>(&dest), v);
    return dest;
}

static int __tmp_14_27615;
static bVector3 lcamPosInside_27614[2];
static float dataBackup_27616[2][12];
static GenericRegion *regionB_27617[2];
static unsigned int ticS_27592;

class WWorldPosTopologyShim : public WWorldPos {
  public:
    WWorldPosTopologyShim(float yOffset)
        : WWorldPos(yOffset) {
        fFace.fPt0 = UMath::Vector3::kZero;
        fFace.fPt1 = UMath::Vector3::kZero;
        fFace.fPt2 = UMath::Vector3::kZero;
        fFace.fSurface.fSurface = 0;
        fFace.fSurface.fFlags = 0;
    }
};

void InitScreenEFX();

enum TunnelBloomDataIndex {
    kTunnelPoint0X = 0,
    kTunnelPoint0Y = 1,
    kTunnelPoint0Z = 2,
    kTunnelPoint1X = 3,
    kTunnelPoint1Y = 4,
    kTunnelPoint1Z = 5,
    kTunnelPoint2X = 6,
    kTunnelPoint2Y = 7,
    kTunnelPoint2Z = 8,
    kTunnelPoint3X = 9,
    kTunnelPoint3Y = 10,
    kTunnelPoint3Z = 11,
};

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
        SE_data[i].UpdateFnc = 0;
        numType[i] = 0;
    }
    InitScreenEFX();
}
void ScreenEffectDB::Update(float deltatime) {
    SE_time += deltatime;

    for (int i = 0; i < SE_NUM_TYPES; i++) {
        if (SE_inf[i].active == 1) {
            SE_inf[i].frameNum += 1;
            ScreenEffectControl controller = SE_inf[i].Controller;
            if (controller == SEC_FRAME || controller == SEC_FUNCTION) {
                SE_inf[i].active = 0;
                numType[i] = 0;
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
    info.UpdateFnc = 0;
    AddScreenEffect(type, &info, 1, SEC_FRAME);
}

void ScreenEffectDB::AddScreenEffect(ScreenEffectType type, ScreenEffectDef *info, unsigned int lock,
                                     ScreenEffectControl controller) {
    if (lock != 0) {
        if (info) {
            SE_data[type] = *info;
        }
        numType[type] = 1;
    } else {
        float influence;
        float invFluence;

        numType[type] += 1;
        influence = static_cast<float>(numType[type]) / static_cast<float>(numType[type] + 1);
        invFluence = 1.0f - influence;

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
    if (TheGameFlowManager.IsInGame()) {
        if (ticS_27592 != eFrameCounter - 1) {
            UpdateAllScreenEFX();
        }
        ticS_27592 = eFrameCounter;
    }
}

void UpdateAllScreenEFX() {
    for (int i = 1; i <= 2; i++) {
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

void DoTunnelBloom(eView *view) {
    int vIndex = 1;
    if (view->GetID() == 1) {
        vIndex = 0;
    }

    if (!view->IsActive()) {
        return;
    }

    CameraMover *camera_mover = view->GetCameraMover();
    if (!camera_mover) {
        return;
    }

    CameraAnchor *camera_anchor = camera_mover->GetAnchor();
    if (!camera_anchor) {
        return;
    }

    bVector3 *my_car_pos = camera_anchor->GetGeometryPosition();
    Camera *view_camera = view->GetCamera();
    bVector3 *camera_position = view_camera->GetPosition();
    bVector3 *camera_direction = view_camera->GetDirection();
    bVector2 twoDpos(camera_position->x, camera_position->y);

    if (!__tmp_14_27615) {
        int i = 1;
        do {
            i -= 1;
        } while (i + 1 != 0);
        __tmp_14_27615 = 1;
    }

    TrackPathZone *zone = 0;
    bVector3 endVector;
    bVector3 posScreen;
    TrackPathZone *zoneBP = zoneB[vIndex];
    if (zoneBP && zoneBP->IsPointInside(&twoDpos)) {
        zone = zoneB[vIndex];
    } else {
        zone = TheTrackPathManager.FindZone(&twoDpos, TRACK_PATH_ZONE_TUNNEL, 0);
    }

    if (zone && zone->GetElevation() > my_car_pos->z) {
        lcamPosInside_27614[vIndex] = *camera_position;
        float angleCos = 0.0f;
        GenericRegion *end_tunnel = GetClosestRegionInView(view, &endVector, &angleCos);
        if (!end_tunnel) {
            return;
        }

        ScreenEffectDef SE_def;
        bVector2 endP(endVector.x, endVector.y);
        bVector2 p0;
        bVector2 p1;
        float len = zone->GetSegmentNextTo(&endP, &p0, &p1);
        if (len == -1.0f || len >= 40.0f) {
            return;
        }

        bVector3 p3(endVector);
        UMath::Vector3 usPoint;
        bConvertToBond(usPoint, p3);
        float height = 0.0f;
        WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(usPoint, height, 0);

        dataBackup_27616[vIndex][kTunnelPoint0X] = p0.x + camera_direction->x;
        dataBackup_27616[vIndex][kTunnelPoint0Y] = p0.y + camera_direction->y;
        dataBackup_27616[vIndex][kTunnelPoint0Z] = height + camera_direction->z;
        dataBackup_27616[vIndex][kTunnelPoint1X] = p1.x + camera_direction->x;
        dataBackup_27616[vIndex][kTunnelPoint1Y] = p1.y + camera_direction->y;
        dataBackup_27616[vIndex][kTunnelPoint1Z] = height + camera_direction->z;
        dataBackup_27616[vIndex][kTunnelPoint2X] = p0.x + camera_direction->x;
        dataBackup_27616[vIndex][kTunnelPoint2Y] = p0.y + camera_direction->y;
        dataBackup_27616[vIndex][kTunnelPoint2Z] = height + TUNHEIGHT + camera_direction->z;
        dataBackup_27616[vIndex][kTunnelPoint3X] = p1.x + camera_direction->x;
        dataBackup_27616[vIndex][kTunnelPoint3Y] = p1.y + camera_direction->y;
        dataBackup_27616[vIndex][kTunnelPoint3Z] = height + TUNHEIGHT + camera_direction->z;

        SE_def.r = 128.0f;
        SE_def.g = 128.0f;
        SE_def.b = 128.0f;
        SE_def.a = 128.0f;
        SE_def.UpdateFnc = 0;
        SE_def.data[0] = dataBackup_27616[vIndex][kTunnelPoint0X];
        SE_def.data[1] = dataBackup_27616[vIndex][kTunnelPoint0Y];
        SE_def.data[2] = dataBackup_27616[vIndex][kTunnelPoint0Z];
        SE_def.data[3] = dataBackup_27616[vIndex][kTunnelPoint1X];
        SE_def.data[4] = dataBackup_27616[vIndex][kTunnelPoint1Y];
        SE_def.data[5] = dataBackup_27616[vIndex][kTunnelPoint1Z];
        SE_def.data[6] = dataBackup_27616[vIndex][kTunnelPoint2X];
        SE_def.data[7] = dataBackup_27616[vIndex][kTunnelPoint2Y];
        SE_def.data[8] = dataBackup_27616[vIndex][kTunnelPoint2Z];
        SE_def.data[9] = dataBackup_27616[vIndex][kTunnelPoint3X];
        SE_def.data[10] = dataBackup_27616[vIndex][kTunnelPoint3Y];
        SE_def.data[11] = dataBackup_27616[vIndex][kTunnelPoint3Z];

        if (regionB_27617[vIndex] != end_tunnel) {
            view->ScreenEffects->SetDATA(SE_GLARE, 0.0f, 1);
        }
        regionB_27617[vIndex] = end_tunnel;
        if (zoneB[vIndex] != zone) {
            view->ScreenEffects->SetDATA(SE_GLARE, 0.0f, 1);
        }
        zoneB[vIndex] = zone;

        bVector2 r = p0 - twoDpos;
        bVector2 v(p1.y - p0.y, p0.x - p1.x);
        bNormalize(&v, &v);
        float dir_dot = bAbs(bDot(&v, &r));
        if (dir_dot < 17.0f) {
            SE_def.intensity = view->ScreenEffects->GetDATA(SE_GLARE, 1) * 0.05882353f * dir_dot;
        } else {
            SE_def.intensity = 1.0f;
            if (view->ScreenEffects->GetDATA(SE_GLARE, 1) < 1.0f) {
                SE_def.intensity = view->ScreenEffects->GetDATA(SE_GLARE, 1) + GlareFallon;
            }
        }

        view->ScreenEffects->SetDATA(SE_GLARE, SE_def.intensity, 1);
        view->ScreenEffects->AddScreenEffect(SE_GLARE, &SE_def, 1, SEC_FRAME);
        AccumulationBufferNeedsFlush = 1;

        if (view->Precipitation && 0.0f < view->Precipitation->GetRainIntensity()) {
            view->Precipitation->IsValidRainCurtainPos = CT_OVERIDE;
            view->Precipitation->AttachRainCurtain(
                SE_def.data[6],
                SE_def.data[7],
                SE_def.data[8],
                SE_def.data[9],
                SE_def.data[10],
                SE_def.data[11],
                SE_def.data[0],
                SE_def.data[1],
                SE_def.data[2],
                SE_def.data[3],
                SE_def.data[4],
                SE_def.data[5]
            );
        }
        return;
    }

    if (0.0f < view->ScreenEffects->GetIntensity(SE_GLARE)) {
        ScreenEffectDef SE_def;
        bVector3 midpoint(
            dataBackup_27616[vIndex][kTunnelPoint0X],
            dataBackup_27616[vIndex][kTunnelPoint0Y],
            dataBackup_27616[vIndex][kTunnelPoint0Z]
        );
        bVector3 ToGlare;
        float BaseGlare = view->ScreenEffects->GetIntensity(SE_GLARE) - GlareFalloff;

        midpoint += bVector3(
            dataBackup_27616[vIndex][kTunnelPoint1X],
            dataBackup_27616[vIndex][kTunnelPoint1Y],
            dataBackup_27616[vIndex][kTunnelPoint1Z]
        );
        midpoint *= 0.5f;

        SE_def.r = 128.0f;
        SE_def.g = 128.0f;
        SE_def.b = 128.0f;
        SE_def.a = 128.0f;
        SE_def.UpdateFnc = 0;
        SE_def.intensity = BaseGlare;
        ToGlare = *camera_position - lcamPosInside_27614[vIndex];
        ToGlare += *camera_direction;

        if (0.0f < BaseGlare) {
            SE_def.data[0] = ToGlare.x + dataBackup_27616[vIndex][kTunnelPoint0X];
            SE_def.data[1] = ToGlare.y + dataBackup_27616[vIndex][kTunnelPoint0Y];
            SE_def.data[2] = ToGlare.z + dataBackup_27616[vIndex][kTunnelPoint0Z];
            SE_def.data[3] = ToGlare.x + dataBackup_27616[vIndex][kTunnelPoint1X];
            SE_def.data[4] = ToGlare.y + dataBackup_27616[vIndex][kTunnelPoint1Y];
            SE_def.data[5] = ToGlare.z + dataBackup_27616[vIndex][kTunnelPoint1Z];
            SE_def.data[6] = ToGlare.x + dataBackup_27616[vIndex][kTunnelPoint2X];
            SE_def.data[7] = ToGlare.y + dataBackup_27616[vIndex][kTunnelPoint2Y];
            SE_def.data[8] = ToGlare.z + dataBackup_27616[vIndex][kTunnelPoint2Z];
            SE_def.data[9] = ToGlare.x + dataBackup_27616[vIndex][kTunnelPoint3X];
            SE_def.data[10] = ToGlare.y + dataBackup_27616[vIndex][kTunnelPoint3Y];
            SE_def.data[11] = ToGlare.z + dataBackup_27616[vIndex][kTunnelPoint3Z];
            view->ScreenEffects->AddScreenEffect(SE_GLARE, &SE_def, 1, SEC_FRAME);
            AccumulationBufferNeedsFlush = 1;
        }
    }
}
