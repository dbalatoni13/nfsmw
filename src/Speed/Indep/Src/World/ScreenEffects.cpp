#include "ScreenEffects.hpp"

#include "Scenery.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorldPos.h"
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

void InitScreenEFX() {}

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
        bMemSet(&SE_data[i], 0, sizeof(SE_data[i]));
        numType[i] = 0;
    }
    InitScreenEFX();
}

void TickSFX() {
    if (TheGameFlowManager.IsInGame()) {
        if (ticS_27592 != eFrameCounter - 1) {
            UpdateAllScreenEFX();
        }
        ticS_27592 = eFrameCounter;
    }
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

float TopologyCoordinate::GetElevation(const bVector3 *position, TerrainType *terrain_type, bVector3 *normal, bool *valid) {
    UMath::Vector3 world_position;
    UMath::Vector4 face_point;
    WWorldPos world_pos(0.025f);

    (void)terrain_type;
    (void)normal;

    world_position.x = position->x;
    world_position.y = -position->y;
    world_position.z = position->z;
    world_pos.Update(world_position, face_point, true, 0, true);
    if (valid) {
        *valid = world_pos.OnValidFace();
    }
    if (world_pos.OnValidFace()) {
        return world_pos.HeightAtPoint(world_position);
    }
    return position->z;
}

bool TopologyCoordinate::HasTopology(const bVector2 *position) {
    bVector3 test_position;
    bool valid;

    test_position.x = position->x;
    test_position.y = position->y;
    test_position.z = 99999.1015625f;
    GetElevation(&test_position, 0, 0, &valid);
    return valid;
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
    bVector2 *twoDpos_ptr = &twoDpos;

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
    if (zoneBP && zoneBP->IsPointInside(twoDpos_ptr)) {
        zone = zoneB[vIndex];
    } else {
        zone = TheTrackPathManager.FindZone(twoDpos_ptr, TRACK_PATH_ZONE_TUNNEL, 0);
    }

    if (zone && zone->Elevation > my_car_pos->z) {
        lcamPosInside_27614[vIndex] = *camera_position;
        float angleCos = 0.0f;
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
        WCollisionMgr collision_mgr(0, 3);
        collision_mgr.GetWorldHeightAtPointRigorous(usPoint, height, 0);

        float point0_x = p0.x + camera_direction->x;
        float point0_y = p0.y + camera_direction->y;
        float point0_z = height + camera_direction->z;
        float point1_x = p1.x + camera_direction->x;
        float point1_y = p1.y + camera_direction->y;
        float point1_z = height + camera_direction->z;
        float point2_x = p0.x + camera_direction->x;
        float point2_y = p0.y + camera_direction->y;
        float point2_z = height + TUNHEIGHT + camera_direction->z;
        float point3_x = p1.x + camera_direction->x;
        float point3_y = p1.y + camera_direction->y;
        float point3_z = height + TUNHEIGHT + camera_direction->z;

        dataBackup_27616[kTunnelPoint0X][vIndex] = point0_x;
        dataBackup_27616[kTunnelPoint0Y][vIndex] = point0_y;
        dataBackup_27616[kTunnelPoint0Z][vIndex] = point0_z;
        dataBackup_27616[kTunnelPoint1X][vIndex] = point1_x;
        dataBackup_27616[kTunnelPoint1Y][vIndex] = point1_y;
        dataBackup_27616[kTunnelPoint1Z][vIndex] = point1_z;
        dataBackup_27616[kTunnelPoint2X][vIndex] = point2_x;
        dataBackup_27616[kTunnelPoint2Y][vIndex] = point2_y;
        dataBackup_27616[kTunnelPoint2Z][vIndex] = point2_z;
        dataBackup_27616[kTunnelPoint3X][vIndex] = point3_x;
        dataBackup_27616[kTunnelPoint3Y][vIndex] = point3_y;
        dataBackup_27616[kTunnelPoint3Z][vIndex] = point3_z;

        ScreenEffectDef SE_def;
        bMemSet(&SE_def, 0, sizeof(SE_def));
        SE_def.r = 128.0f;
        SE_def.g = 128.0f;
        SE_def.b = 128.0f;
        SE_def.a = 128.0f;
        SE_def.data[0] = point0_x;
        SE_def.data[1] = point0_y;
        SE_def.data[2] = point0_z;
        SE_def.data[3] = point1_x;
        SE_def.data[4] = point1_y;
        SE_def.data[5] = point1_z;
        SE_def.data[6] = point2_x;
        SE_def.data[7] = point2_y;
        SE_def.data[8] = point2_z;
        SE_def.data[9] = point3_x;
        SE_def.data[10] = point3_y;
        SE_def.data[11] = point3_z;

        if (regionB_27617[vIndex] != end_tunnel) {
            view->ScreenEffects->SE_data[SE_GLARE].data[1] = 0.0f;
        }
        regionB_27617[vIndex] = end_tunnel;
        if (zoneB[vIndex] != zone) {
            view->ScreenEffects->SE_data[SE_GLARE].data[1] = 0.0f;
        }
        zoneB[vIndex] = zone;

        bVector2 r;
        bVector2 v;
        r.x = p0.x - twoDpos.x;
        r.y = p0.y - twoDpos.y;
        v.x = p1.y - p0.y;
        v.y = p0.x - p1.x;
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

        if (view->Precipitation && 0.0f < view->Precipitation->GetRainIntensity()) {
            view->Precipitation->IsValidRainCurtainPos = CT_OVERIDE;
            view->Precipitation->AttachRainCurtain(
                point2_x, point2_y, point2_z, point3_x, point3_y, point3_z, point0_x, point0_y
            );
        }
        return;
    }

    if (0.0f < view->ScreenEffects->SE_data[SE_GLARE].intensity) {
        ScreenEffectDef SE_def;
        float pos_screen_x;
        float pos_screen_y;
        float pos_screen_z;

        bMemSet(&SE_def, 0, sizeof(SE_def));
        SE_def.r = 128.0f;
        SE_def.g = 128.0f;
        SE_def.b = 128.0f;
        SE_def.a = 128.0f;
        SE_def.intensity = view->ScreenEffects->SE_data[SE_GLARE].intensity - GlareFalloff;
        pos_screen_x = camera_position->x - lcamPosInside_27614[vIndex].x;
        pos_screen_y = camera_position->y - lcamPosInside_27614[vIndex].y;
        pos_screen_z = camera_position->z - lcamPosInside_27614[vIndex].z;
        pos_screen_x += camera_direction->x;
        pos_screen_y += camera_direction->y;
        pos_screen_z += camera_direction->z;

        if (0.0f < SE_def.intensity) {
            SE_def.data[0] = pos_screen_x + dataBackup_27616[kTunnelPoint0X][vIndex];
            SE_def.data[1] = pos_screen_y + dataBackup_27616[kTunnelPoint0Y][vIndex];
            SE_def.data[2] = pos_screen_z + dataBackup_27616[kTunnelPoint0Z][vIndex];
            SE_def.data[3] = pos_screen_x + dataBackup_27616[kTunnelPoint1X][vIndex];
            SE_def.data[4] = pos_screen_y + dataBackup_27616[kTunnelPoint1Y][vIndex];
            SE_def.data[5] = pos_screen_z + dataBackup_27616[kTunnelPoint1Z][vIndex];
            SE_def.data[6] = pos_screen_x + dataBackup_27616[kTunnelPoint2X][vIndex];
            SE_def.data[7] = pos_screen_y + dataBackup_27616[kTunnelPoint2Y][vIndex];
            SE_def.data[8] = pos_screen_z + dataBackup_27616[kTunnelPoint2Z][vIndex];
            SE_def.data[9] = pos_screen_x + dataBackup_27616[kTunnelPoint3X][vIndex];
            SE_def.data[10] = pos_screen_y + dataBackup_27616[kTunnelPoint3Y][vIndex];
            SE_def.data[11] = pos_screen_z + dataBackup_27616[kTunnelPoint3Z][vIndex];
            view->ScreenEffects->AddScreenEffect(SE_GLARE, &SE_def, 1, SEC_FRAME);
            AccumulationBufferNeedsFlush = 1;
        }
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

void FlushAccumulationBuffer() {
    AccumulationBufferNeedsFlush = 1;
}

void AccumulationBufferFlushed() {
    AccumulationBufferNeedsFlush = 0;
}

unsigned int QueryFlushAccumulationBuffer() {
    return AccumulationBufferNeedsFlush;
}
