#include "ScreenEffects.hpp"

#include "Scenery.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WeatherMan.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

static unsigned int AccumulationBufferNeedsFlush = 0;
ScreenEffectPaletteDef SE_PaletteFile[EFX_NUMBER];

struct TerrainType;
struct eLightContext;

class TopologyCoordinate {
  public:
    bool HasTopology(const bVector2 *point);
    float GetElevation(const bVector3 *point, TerrainType *terrain_type, bVector3 *normal, bool *valid);

  private:
    int mData[2];
};

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
extern TrackPathZone *zoneB[2];

static int __tmp_14_27615;
static bVector3 lcamPosInside_27614[2];
static float dataBackup_27616[12][2];
static GenericRegion *regionB_27617[2];
static unsigned int ticS_27592;

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

void TickSFX() {
    if (TheGameFlowManager.IsInGame()) {
        if (ticS_27592 != eFrameCounter - 1) {
            UpdateAllScreenEFX();
        }
        ticS_27592 = eFrameCounter;
    }
}

void ScreenEffectDB::AddScreenEffect(ScreenEffectType type, float intensity, float r, float g, float b) {
    ScreenEffectDef info;

    info.r = r;
    info.g = g;
    info.b = b;
    info.a = 0.0f;
    info.intensity = intensity;
    bMemSet(info.data, 0, sizeof(info.data));
    info.UpdateFnc = 0;
    AddScreenEffect(type, &info, 1, SEC_FRAME);
}

void ScreenEffectDB::AddScreenEffect(ScreenEffectType type, ScreenEffectDef *info, unsigned int lock,
                                     ScreenEffectControl controller) {
    ScreenEffectDef *screen_effect = &SE_data[type];
    if (lock == 0) {
        unsigned int previous_count = numType[type];
        unsigned int current_count = previous_count + 1;
        float blend = static_cast<float>(current_count) / static_cast<float>(previous_count + 2);
        float inverse_blend = 1.0f - blend;

        numType[type] = current_count;
        screen_effect->r = blend * screen_effect->r + inverse_blend * info->r;
        screen_effect->g = blend * screen_effect->g + inverse_blend * info->g;
        screen_effect->b = blend * screen_effect->b + inverse_blend * info->b;
        screen_effect->a = blend * screen_effect->a + inverse_blend * info->a;
        screen_effect->intensity = blend * screen_effect->intensity + inverse_blend * info->intensity;
    } else {
        bMemCpy(screen_effect, info, sizeof(*screen_effect));
        numType[type] = 1;
    }

    SE_inf[type].active = 1;
    if (!screen_effect->UpdateFnc) {
        SE_inf[type].Controller = controller;
    } else {
        screen_effect->UpdateFnc(type, this);
    }

    if (screen_effect->intensity < 0.01f) {
        SE_inf[type].active = 0;
    }
}

void ScreenEffectDB::AddPaletteEffect(ScreenEffectPaletteDef *palette) {
    for (int i = 0; i < palette->NumEffects; i++) {
        AddScreenEffect(palette->SE_type[i], &palette->SE_Def[i], 1, palette->SE_Controller[i]);
    }
}

void ScreenEffectDB::AddPaletteEffect(ScreenEffectPalette palette) {
    AddPaletteEffect(&SE_PaletteFile[palette]);
}

void RenderVisibleSectionBoundary(VisibleSectionBoundary *boundary, eView *view) {
    if (!boundary || !view || boundary->NumPoints <= 0) {
        return;
    }

    float perimeter = 0.0f;
    bVector3 position(0.0f, 0.0f, 0.0f);
    TopologyCoordinate topology_coordinate;
    float pos = static_cast<float>((static_cast<int>(WorldTimer.GetPackedTime() * 0.00025f * 262144.0f) & 0xffff)) * 6.1035156e-05f;
    int point_number;

    for (point_number = 0; point_number < boundary->GetNumPoints(); point_number++) {
        bVector2 normal = *boundary->GetPoint((point_number + 1) % boundary->GetNumPoints()) - *boundary->GetPoint(point_number);
        float length = bLength(&normal);

        bNormalize(&normal, &normal);
        if (pos < length) {
            do {
                bScaleAdd(reinterpret_cast<bVector2 *>(&position), boundary->GetPoint(point_number), &normal, pos);

                if (topology_coordinate.HasTopology(reinterpret_cast<bVector2 *>(&position))) {
                    position.z = 9999.0f;
                    position.z = topology_coordinate.GetElevation(&position, 0, 0, 0);
                    if (view->GetPixelSize(&position, 1.0f) > 0) {
                        unsigned char *matrix_memory = CurrentBufferPos;
                        unsigned char *next_buffer_pos = CurrentBufferPos + sizeof(bMatrix4);
                        if (CurrentBufferEnd <= next_buffer_pos) {
                            FrameMallocFailed = 1;
                            FrameMallocFailAmount += sizeof(bMatrix4);
                            matrix_memory = 0;
                            next_buffer_pos = CurrentBufferPos;
                        }

                        CurrentBufferPos = next_buffer_pos;
                        if (matrix_memory) {
                            bMatrix4 *matrix = reinterpret_cast<bMatrix4 *>(matrix_memory);
                            bIdentity(matrix);
                            bCopy(&matrix->v3, &position, 1.0f);
                            reinterpret_cast<eViewRenderShim *>(view)->Render(pVisibleZoneBoundaryModel, matrix, 0, 0, 0, 0);
                        }
                    }
                }

                pos += 4.0f;
            } while (pos < length);
        }

        pos -= length;
    }
}

void DoTunnelBloom(eView *view) {
    if (!view->IsActive()) {
        return;
    }

    int vIndex = view->GetID() != 1;
    float base_glare = 0.0f;
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
        bool more;
        do {
            more = i != 0;
            i -= 1;
        } while (more);
        __tmp_14_27615 = 1;
    }

    TrackPathZone *zone = 0;
    TrackPathZone *zoneBP = zoneB[vIndex];
    if (zoneBP && zoneBP->IsPointInside(&twoDpos)) {
        zone = zoneBP;
    } else {
        zone = TheTrackPathManager.FindZone(&twoDpos, TRACK_PATH_ZONE_TUNNEL, 0);
    }

    if (!zone || zone->Elevation <= my_car_pos->z) {
        if (base_glare < view->ScreenEffects->SE_data[SE_GLARE].intensity) {
            ScreenEffectDef SE_def;
            bMemSet(&SE_def, 0, sizeof(SE_def));
            SE_def.r = 128.0f;
            SE_def.g = 128.0f;
            SE_def.b = 128.0f;
            SE_def.a = 128.0f;
            SE_def.intensity = view->ScreenEffects->SE_data[SE_GLARE].intensity - GlareFalloff;

            bVector3 posScreen(*camera_position - lcamPosInside_27614[vIndex]);
            posScreen += *camera_direction;

            if (base_glare < SE_def.intensity) {
                SE_def.data[0] = posScreen.x + dataBackup_27616[kTunnelPoint0X][vIndex];
                SE_def.data[1] = posScreen.y + dataBackup_27616[kTunnelPoint0Y][vIndex];
                SE_def.data[2] = posScreen.z + dataBackup_27616[kTunnelPoint0Z][vIndex];
                SE_def.data[3] = posScreen.x + dataBackup_27616[kTunnelPoint1X][vIndex];
                SE_def.data[4] = posScreen.y + dataBackup_27616[kTunnelPoint1Y][vIndex];
                SE_def.data[5] = posScreen.z + dataBackup_27616[kTunnelPoint1Z][vIndex];
                SE_def.data[6] = posScreen.x + dataBackup_27616[kTunnelPoint2X][vIndex];
                SE_def.data[7] = posScreen.y + dataBackup_27616[kTunnelPoint2Y][vIndex];
                SE_def.data[8] = posScreen.z + dataBackup_27616[kTunnelPoint2Z][vIndex];
                SE_def.data[9] = posScreen.x + dataBackup_27616[kTunnelPoint3X][vIndex];
                SE_def.data[10] = posScreen.y + dataBackup_27616[kTunnelPoint3Y][vIndex];
                SE_def.data[11] = posScreen.z + dataBackup_27616[kTunnelPoint3Z][vIndex];
                view->ScreenEffects->AddScreenEffect(SE_GLARE, &SE_def, 1, SEC_FRAME);
                AccumulationBufferNeedsFlush = 1;
            }
        }
        return;
    }

    lcamPosInside_27614[vIndex] = *camera_position;
    float angleCos = base_glare;
    bVector3 endVector;
    GenericRegion *end_tunnel = GetClosestRegionInView(view, &endVector, &angleCos);
    if (!end_tunnel) {
        return;
    }

    bVector2 endP(endVector.x, endVector.y);
    bVector2 p0;
    bVector2 p1;
    float len = zone->GetSegmentNextTo(&endP, &p0, &p1);
    if (len == -1.0f || len >= 40.0f) {
        return;
    }

    UMath::Vector3 usPoint;
    usPoint.x = -endVector.y;
    usPoint.y = endVector.z;
    usPoint.z = endVector.x;
    float height = 0.0f;
    WCollisionMgr cmap(0, 3);
    cmap.GetWorldHeightAtPointRigorous(usPoint, height, 0);

    dataBackup_27616[kTunnelPoint0X][vIndex] = p0.x + camera_direction->x;
    dataBackup_27616[kTunnelPoint0Y][vIndex] = p0.y + camera_direction->y;
    dataBackup_27616[kTunnelPoint0Z][vIndex] = height + camera_direction->z;
    dataBackup_27616[kTunnelPoint1X][vIndex] = p1.x + camera_direction->x;
    dataBackup_27616[kTunnelPoint1Y][vIndex] = p1.y + camera_direction->y;
    dataBackup_27616[kTunnelPoint1Z][vIndex] = height + camera_direction->z;
    dataBackup_27616[kTunnelPoint2X][vIndex] = p0.x + camera_direction->x;
    dataBackup_27616[kTunnelPoint2Y][vIndex] = p0.y + camera_direction->y;
    dataBackup_27616[kTunnelPoint2Z][vIndex] = height + TUNHEIGHT + camera_direction->z;
    dataBackup_27616[kTunnelPoint3X][vIndex] = p1.x + camera_direction->x;
    dataBackup_27616[kTunnelPoint3Y][vIndex] = p1.y + camera_direction->y;
    dataBackup_27616[kTunnelPoint3Z][vIndex] = height + TUNHEIGHT + camera_direction->z;

    ScreenEffectDef SE_def;
    bMemSet(&SE_def, 0, sizeof(SE_def));
    SE_def.r = 128.0f;
    SE_def.g = 128.0f;
    SE_def.b = 128.0f;
    SE_def.a = 128.0f;
    SE_def.data[0] = dataBackup_27616[kTunnelPoint0X][vIndex];
    SE_def.data[1] = dataBackup_27616[kTunnelPoint0Y][vIndex];
    SE_def.data[2] = dataBackup_27616[kTunnelPoint0Z][vIndex];
    SE_def.data[3] = dataBackup_27616[kTunnelPoint1X][vIndex];
    SE_def.data[4] = dataBackup_27616[kTunnelPoint1Y][vIndex];
    SE_def.data[5] = dataBackup_27616[kTunnelPoint1Z][vIndex];
    SE_def.data[6] = dataBackup_27616[kTunnelPoint2X][vIndex];
    SE_def.data[7] = dataBackup_27616[kTunnelPoint2Y][vIndex];
    SE_def.data[8] = dataBackup_27616[kTunnelPoint2Z][vIndex];
    SE_def.data[9] = dataBackup_27616[kTunnelPoint3X][vIndex];
    SE_def.data[10] = dataBackup_27616[kTunnelPoint3Y][vIndex];
    SE_def.data[11] = dataBackup_27616[kTunnelPoint3Z][vIndex];

    if (regionB_27617[vIndex] != end_tunnel) {
        view->ScreenEffects->SE_data[SE_GLARE].data[1] = base_glare;
    }
    regionB_27617[vIndex] = end_tunnel;
    if (zoneB[vIndex] != zone) {
        view->ScreenEffects->SE_data[SE_GLARE].data[1] = base_glare;
    }
    zoneB[vIndex] = zone;

    bVector2 r(p0.x - twoDpos.x, p0.y - twoDpos.y);
    bVector2 v(p1.y - p0.y, p0.x - p1.x);
    bNormalize(&v, &v);
    float dir_dot = bAbs(v.x * r.x + v.y * r.y);
    if (17.0f <= dir_dot) {
        SE_def.intensity = 1.0f;
        if (view->ScreenEffects->SE_data[SE_GLARE].data[1] < 1.0f) {
            SE_def.intensity = view->ScreenEffects->SE_data[SE_GLARE].data[1] + GlareFallon;
        }
    } else {
        SE_def.intensity = view->ScreenEffects->SE_data[SE_GLARE].data[1] * 0.05882353f * dir_dot;
    }

    view->ScreenEffects->SE_data[SE_GLARE].data[1] = SE_def.intensity;
    view->ScreenEffects->AddScreenEffect(SE_GLARE, &SE_def, 1, SEC_FRAME);
    AccumulationBufferNeedsFlush = 1;

    if (view->Precipitation && base_glare < view->Precipitation->GetRainIntensity()) {
        view->Precipitation->IsValidRainCurtainPos = CT_OVERIDE;
        view->Precipitation->AttachRainCurtain(dataBackup_27616[kTunnelPoint2X][vIndex], dataBackup_27616[kTunnelPoint2Y][vIndex],
                                               dataBackup_27616[kTunnelPoint2Z][vIndex], dataBackup_27616[kTunnelPoint3X][vIndex],
                                               dataBackup_27616[kTunnelPoint3Y][vIndex], dataBackup_27616[kTunnelPoint3Z][vIndex],
                                               dataBackup_27616[kTunnelPoint0X][vIndex], dataBackup_27616[kTunnelPoint0Y][vIndex]);
    }
}

void DoTinting(eView *view) {
    if (!view || !view->ScreenEffects) {
        return;
    }

    ScreenEffectDef *tint = &view->ScreenEffects->SE_data[SE_TINT];
    if (tint->intensity > 0.0f) {
        tint->a = tint->intensity;
    }
}

void UpdateAllScreenEFX() {
    for (int view_index = 1; view_index <= 2; view_index++) {
        eView *view = eGetView(view_index, false);
        DoTunnelBloom(view);
        DoTinting(view);
    }
}

void AccumulationBufferFlushed() {
    AccumulationBufferNeedsFlush = 0;
}

unsigned int QueryFlushAccumulationBuffer() {
    return AccumulationBufferNeedsFlush;
}
