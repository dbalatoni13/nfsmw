#include "ScreenEffects.hpp"

#include "Scenery.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WeatherMan.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

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

void TickSFX() {}

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
        if (info) {
            *screen_effect = *info;
        }
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

    float phase = static_cast<float>((static_cast<int>(WorldTimer.GetPackedTime() * 0.00025f * 262144.0f) & 0xffff)) * 6.1035156e-05f;
    for (int i = 0; i < boundary->NumPoints; i++) {
        int next_index = (i + 1) % boundary->NumPoints;
        bVector2 segment;
        segment.x = boundary->Points[next_index].x - boundary->Points[i].x;
        segment.y = boundary->Points[next_index].y - boundary->Points[i].y;

        float segment_length = bLength(&segment);
        bVector2 direction;
        bNormalize(&direction, &segment);
        if (phase < segment_length) {
            do {
                bVector2 point2;
                bScaleAdd(&point2, &boundary->Points[i], &direction, phase);

                TopologyCoordinate topology_coordinate;
                if (topology_coordinate.HasTopology(&point2)) {
                    bVector3 point3(point2.x, point2.y, 0.0f);
                    point3.z = topology_coordinate.GetElevation(&point3, 0, 0, 0);
                    if (view->GetPixelSize(&point3, 1.0f) > 0) {
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
                            PSMTX44Identity(*reinterpret_cast<Mtx44 *>(matrix));
                            matrix->v3.x = point3.x;
                            matrix->v3.y = point3.y;
                            matrix->v3.z = point3.z;
                            matrix->v3.w = 1.0f;
                            reinterpret_cast<eViewRenderShim *>(view)->Render(pVisibleZoneBoundaryModel, matrix, 0, 0, 0, 0);
                        }
                    }
                }

                phase += 4.0f;
            } while (phase < segment_length);
        }

        phase -= segment_length;
    }
}

void DoTunnelBloom(eView *view) {
    if (!view->Active) {
        return;
    }

    unsigned int vIndex = static_cast<unsigned int>(view->GetID() != 1);
    CameraMover *camera_mover = view->GetCameraMover();
    if (!camera_mover) {
        return;
    }

    CameraAnchor *camera_anchor = camera_mover->GetAnchor();
    if (!camera_anchor) {
        return;
    }

    Camera *view_camera = view->GetCamera();

    bVector3 *camera_position = view_camera->GetPosition();
    bVector3 *camera_direction = view_camera->GetDirection();
    bVector2 twoDpos(camera_position->x, camera_position->y);
    float base_glare = 0.0f;

    if (!__tmp_14_27615) {
        int i = 1;
        do {
            bool more = i != 0;
            i -= 1;
            if (!more) {
                break;
            }
        } while (true);
        __tmp_14_27615 = 1;
    }

    TrackPathZone *zone = 0;
    if (zoneB[vIndex] && zoneB[vIndex]->IsPointInside(&twoDpos)) {
        zone = zoneB[vIndex];
    } else {
        zone = TheTrackPathManager.FindZone(&twoDpos, TRACK_PATH_ZONE_TUNNEL, 0);
    }

    float anchor_z = *reinterpret_cast<float *>(reinterpret_cast<char *>(camera_anchor) + 0x20);
    if (!zone || zone->Elevation <= anchor_z) {
        if (base_glare < view->ScreenEffects->SE_data[SE_GLARE].intensity) {
            ScreenEffectDef SE_def;
            bMemSet(&SE_def, 0, sizeof(SE_def));
            SE_def.r = 128.0f;
            SE_def.g = 128.0f;
            SE_def.b = 128.0f;
            SE_def.a = 128.0f;
            SE_def.intensity = view->ScreenEffects->SE_data[SE_GLARE].intensity - GlareFalloff;

            bVector3 posScreen((camera_position->x - lcamPosInside_27614[vIndex].x) + camera_direction->x,
                               (camera_position->y - lcamPosInside_27614[vIndex].y) + camera_direction->y,
                               (camera_position->z - lcamPosInside_27614[vIndex].z) + camera_direction->z);

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
    if (len == -1.0f) {
        return;
    }

    bVector3 p3(endVector);
    eUnSwizzleWorldVector(p3, p3);
    float height = 0.0f;
    WCollisionMgr cmap(0, 3);
    cmap.GetWorldHeightAtPointRigorous(reinterpret_cast<UMath::Vector3 &>(p3), height, 0);

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

    if (regionB_27617[vIndex] != end_tunnel || zoneB[vIndex] != zone) {
        view->ScreenEffects->SE_data[SE_GLARE].data[1] = base_glare;
    }
    regionB_27617[vIndex] = end_tunnel;
    zoneB[vIndex] = zone;

    bVector2 r(p0.x - twoDpos.x, p0.y - twoDpos.y);
    bVector2 v(p1.y - p0.y, p0.x - p1.x);
    bNormalize(&v, &v);
    float dir_dot = bAbs(v.x * r.x + v.y * r.y);
    if (0.8f <= dir_dot) {
        SE_def.intensity = 1.0f;
        if (view->ScreenEffects->SE_data[SE_GLARE].data[1] < 1.0f) {
            SE_def.intensity = view->ScreenEffects->SE_data[SE_GLARE].data[1] + GlareFallon;
        }
    } else {
        SE_def.intensity = view->ScreenEffects->SE_data[SE_GLARE].data[1] * 0.5f * dir_dot;
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
