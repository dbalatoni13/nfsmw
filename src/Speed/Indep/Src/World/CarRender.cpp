#include "./CarRender.hpp"
#include "Interfaces/IVehicleDamageBehaviour.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/DefragFixer.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/ecar.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/World/Sun.hpp"
#include "Speed/Indep/Src/World/VehicleFragmentConn.h"
#include "Speed/Indep/Src/World/VehicleRenderConn.h"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bVector.hpp"

namespace BuildRegion {
bool IsEurope();
}

float culldiv = 12000.0f;
static const CarFXPosInfo FXMarkerNameHashMappings[28] = {
    {{0, 0, 0, 0}, 255},
    {{0, 0, 0, 0}, 255},
    {{0, 0, 0, 0}, 255},
    {{0, 0, 0, 0}, 255},
    {{0, 0, 0, 0}, 255},
    {{0, 0, 0, 0}, 255},
    {{0, 0, 0, 0}, 255},
    {{0, 0, 0, 0}, 255},
    {{0, 0, 0, 0}, 255},
    {{0, 0, 0, 0}, 255},
    {{0xA5B28001, 0xBCF8A18B, 0xBD7CF15E, 0}, 3},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
    {{0, 0, 0, 0}, 0},
};

SlotPool *CarEmitterPositionSlotPool = nullptr;
const int MAX_CAR_PART_MODELS = 250;
SlotPool *CarPartModelPool = nullptr;
extern void *gINISInstance asm("_Q33UTL11Collectionst9Singleton1Z4INIS_mInstance");

struct eEnvMap {
    void UpdateCameras(bVector3 *viewer_world_position, bVector3 *envmap_world_position);
};

eSolid *eFindSolid(unsigned int name_hash);
void GetUsedCarTextureInfo(UsedCarTextureInfo *used_texture_info, RideInfo *ride_info, int front_end_only);
extern float copm;
extern float copt;
extern int copModulo;
extern float cpr;
extern float cpb;
extern float cpw;
extern float copoffsetr;
extern float copoffsetb;
extern float copoffsetw;
extern float enX;
extern float enY;
extern float enZ;
extern float hOffX;
extern float hOffY;
extern float hRad1x;
extern float hRad2x;
extern float hRad1y;
extern float hRad2y;
extern float hRad0x;
extern float hRad3x;
extern float hRad0y;
extern float hRad3y;
extern unsigned int TireFaceIt;
float TireFace(bMatrix4 *matrix, eView *view);
extern unsigned int CarReplacementDecalHash[CarRenderInfo::REPLACETEX_DECAL_NUM];
extern unsigned int hcL;
extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;
extern int DrawCars;
extern int DrawCarShadow;
extern float WorldTimeElapsed;
extern int ForceCarLOD;
extern int ForceTireLOD;
extern int ForceReverselightsOn;
extern int TweakKitWheelOffsetFront;
extern int TweakKitWheelOffsetRear;
extern int ForceBrakelightsOn;
extern int ForceHeadlightsOn;
extern int iRam8047ff04;
extern float lbl_8040AA60;
extern float lbl_8040AA68;
extern float lbl_8040AA6C;
extern bVector3 EnvMapEyeOffset;
extern bVector3 EnvMapCamOffset;
extern float WheelStandardWidth;
extern float WheelStandardRadius;
extern float lbl_8040AD70;
extern float lbl_8040AD74;
extern float lbl_8040AD78;
extern float lbl_8040AD7C;
extern float lbl_8040AD80;
extern float lbl_8040AD84;
extern float lbl_8040AD88;
extern float lbl_8040AD8C;
extern float lbl_8040AD90;
extern float lbl_8040AD94;
extern float lbl_8040AD98;
extern float lbl_8040AD9C;
extern float lbl_8040ADA0;
extern float lbl_8040ADA4;
extern float lbl_8040ADA8;
extern float lbl_8040ADAC;
extern float lbl_8040ADB0;
extern float lbl_8040ADB4;
extern float lbl_8040ADB8;
extern float lbl_8040ADBC;
extern float lbl_8040ADC0;
extern float lbl_8040ADC4;
extern float lbl_8040ADC8;
extern float lbl_8040ADCC;
extern float lbl_8040ADD0;
extern float lbl_8040ADD4;
extern float lbl_8040ADD8;
extern float lbl_8040ADDC;
extern float lbl_8040ADE0;
extern float lbl_8040ADE4;
extern float lbl_8040ADE8;
extern float lbl_8040ADEC;
extern float lbl_8040ADF0;
extern float lbl_8040ADF4;
extern float lbl_8040ADF8;
extern float lbl_8040ADFC;
extern float lbl_8040AE00;
extern float lbl_8040AE04;
extern float copWhitemul;
extern int gTWEAKER_NISLightEnabled;
extern float gTWEAKER_NISLightIntensity;
extern float gTWEAKER_NISLightPosX;
extern float gTWEAKER_NISLightPosY;
extern float gTWEAKER_NISLightPosZ;
extern unsigned int Lightslot;
extern int PrintLightQuery;
extern int PrintQueryLightMat;
extern int EnableEnvMap;
extern eShaperLightRig ShaperLightsBackRoom;
extern eShaperLightRig ShaperLightsCarLot;
extern eShaperLightRig ShaperLightsCarsInGame;
extern eShaperLightRig ShaperLightsCShop;
extern eShaperLightRig ShaperLightsCharacters;
extern eShaperLightRig ShaperLightsCharactersBackup;
extern eShaperLightRig ShaperLightsQRace;
extern eShaperLightRig ShaperLightsSafehouse;
extern bMatrix4 hack_man_matrix;
extern bVector3 hull_Origin asm("hull_Origin");
extern bVector3 hull_Normal asm("hull_Normal");
extern bVector3 hullVertArray1[16] asm("hullVertArray1");
extern bVector3 hullVertArray2[16] asm("hullVertArray2");
extern bVector3 hullVertArray3[48] asm("hullVertArray3");
extern bVector4 PointCloud[16] asm("PointCloud");
extern bVector3 *P[17] asm("P");
int ch2d(float **P, int n);
extern float FancyCarShadowEdgeMult;
extern float car_elevation;
extern float car_elevation_scale;
extern int dshad;
extern bVector3 cs_lightV asm("cs_lightV");
extern float cs_OneOverZ asm("cs_OneOverZ");
extern int counter_31665 asm("counter.31665");
extern int counter_31669 asm("counter.31669");
extern float heliScale;
extern bVector4 feposoff;
extern CarTypeInfo *CarTypeInfoArray;
extern void RestoreShaperRig(eShaperLightRig *ShaperRigP, unsigned int slot, eShaperLightRig *ShaperRigBP);
extern void AddQuickDynamicLight(eShaperLightRig *ShaperRigP, unsigned int slot, float r, float g, float b, float intensity, bVector3 *position);
extern int bBoundingBoxIsInside(const bVector3 *bbox_min, const bVector3 *bbox_max, const bVector3 *point, float extra_width);

void sh_Setup(bVector3 *car_pos);

void sh_Setup(bVector3 *car_pos) {
    bVector3 light_pos;
    bVector3 light_delta;
    float light_length;
    unsigned short shadow_angle;

    if (SunInfo == 0) {
        light_pos.x = lbl_8040AD80;
        light_pos.y = lbl_8040AD84;
        light_pos.z = lbl_8040AD88;
    } else {
        light_pos.x = SunInfo->CarShadowPositionX;
        light_pos.y = SunInfo->CarShadowPositionY;
        light_pos.z = SunInfo->CarShadowPositionZ;
    }

    light_delta.x = car_pos->x - light_pos.x;
    light_delta.y = car_pos->y - light_pos.y;
    light_delta.z = car_pos->z - light_pos.z;

    cs_lightV.z = light_delta.z;
    cs_lightV.x = light_delta.x;
    cs_lightV.y = light_delta.y;

    light_length = lbl_8040AD84;
    float xy_length_sq = light_delta.x * light_delta.x + light_delta.y * light_delta.y;
    if (lbl_8040AD8C < xy_length_sq) {
        light_length = bSqrt(xy_length_sq);
    }

    shadow_angle = bATan(light_length, -cs_lightV.z);
    if (shadow_angle < 4000) {
        float sin_angle;
        float cos_angle;
        float abs_y;

        bSinCos(&sin_angle, &cos_angle, 4000);
        xy_length_sq = cs_lightV.z * cs_lightV.z + cs_lightV.x * cs_lightV.x + cs_lightV.y * cs_lightV.y;
        light_length = lbl_8040AD84;
        if (lbl_8040AD8C < xy_length_sq) {
            light_length = bSqrt(xy_length_sq);
        }

        abs_y = bAbs(cs_lightV.y);
        cs_lightV.z = -light_length * sin_angle;
        cs_lightV.y = (cs_lightV.y / (bAbs(cs_lightV.x) + abs_y)) * light_length * cos_angle;
        cs_lightV.x = (cs_lightV.x / (bAbs(cs_lightV.x) + abs_y)) * light_length * cos_angle;
    }

    cs_OneOverZ = lbl_8040AD84;
    if (cs_lightV.z != lbl_8040AD84) {
        cs_OneOverZ = lbl_8040AD94 / cs_lightV.z;
    }
}

static float const CarBodyLodSwapSize[] = {120.0f, 25.0f, 20.0f, 10.0f, 0.0f};
static float const TrafficCarBodyLodSwapSize[] = {20.0f, 10.0f, 4.0f, 0.0f, 0.0f};

namespace {

void Render(eViewPlatInterface *view, eModel *model, bMatrix4 *local_to_world, eLightContext *light_context, unsigned int flags,
            unsigned int exc_flag);
void Render(eViewPlatInterface *view, ePoly *poly, TextureInfo *texture_info, bMatrix4 *matrix, int accurate, float z_bias);
int eSmoothNormals(eModel **model_table, int num_models);
eEnvMap *eGetEnvMap();
bool eBeginStrip(TextureInfo *a, int b, bMatrix4 *c);
bool eEndStrip(eView *view);
void eAddVertex(const bVector3 &v);
void eAddColour(unsigned int colour);
void eAddUV(float u, float v);
bool exBeginStrip(TextureInfo *tex, int a, bMatrix4 *mat);
void exAddVertex(const bVector3 &v);
void exAddColour(unsigned int colour);
void exAddUV(float u, float v);
bool exEndStrip(eView *view);
int CarPart_GetAppliedAttributeIParam(CarPart *part, unsigned int namehash, int default_value) asm("GetAppliedAttributeIParam__7CarPartUii");
int CarPart_HasAppliedAttribute(CarPart *part, unsigned int namehash) asm("HasAppliedAttribute__7CarPartUi");
unsigned int CarPart_GetAppliedAttributeUParam(CarPart *part, unsigned int namehash, unsigned int default_value)
    asm("GetAppliedAttributeUParam__7CarPartUiUi");
unsigned int CarPart_GetModelNameHash(CarPart *part, int model_num, int lod) asm("GetModelNameHash__7CarPartii");

template <typename T> struct bSNodeLayout {
    T *Next;
};

template <typename T> struct bSListLayout {
    T *Head;
    T *Tail;
};

struct CarRenderUsedCarTextureInfoLayout {
    unsigned int TexturesToLoadPerm[87];
    unsigned int TexturesToLoadTemp[87];
    int NumTexturesToLoadPerm;
    int NumTexturesToLoadTemp;
    unsigned int MappedSkinHash;
    unsigned int MappedSkinBHash;
    unsigned int MappedGlobalHash;
    unsigned int MappedWheelHash;
    unsigned int MappedSpinnerHash;
    unsigned int MappedBadging;
    unsigned int MappedSpoilerHash;
    unsigned int MappedRoofScoopHash;
    unsigned int MappedDashSkinHash;
    unsigned int MappedLightHash[11];
    unsigned int MappedTireHash;
    unsigned int MappedRimHash;
    unsigned int MappedRimBlurHash;
    unsigned int MappedLicensePlateHash;
    unsigned int ReplaceSkinHash;
    unsigned int ReplaceSkinBHash;
    unsigned int ReplaceGlobalHash;
    unsigned int ReplaceWheelHash;
    unsigned int ReplaceSpinnerHash;
    unsigned int ReplaceSpoilerHash;
    unsigned int ReplaceRoofScoopHash;
    unsigned int ReplaceDashSkinHash;
    unsigned int ReplaceHeadlightHash[3];
    unsigned int ReplaceHeadlightGlassHash[3];
    unsigned int ReplaceBrakelightHash[3];
    unsigned int ReplaceBrakelightGlassHash[3];
    unsigned int ReplaceReverselightHash[3];
    unsigned int ShadowHash;
};

struct CarRenderRideInfoLayout {
    CarType Type;
    char InstanceIndex;
    char HasDash;
    char CanBeVertexDamaged;
    char SkinType;
    CARPART_LOD mMinLodLevel;
    CARPART_LOD mMaxLodLevel;
    CARPART_LOD mMinFELodLevel;
    CARPART_LOD mMaxFELodLevel;
    CARPART_LOD mMaxLicenseLodLevel;
    CARPART_LOD mMinTrafficDiffuseLodLevel;
    CARPART_LOD mMinShadowLodLevel;
    CARPART_LOD mMaxShadowLodLevel;
    CARPART_LOD mMaxTireLodLevel;
    CARPART_LOD mMaxBrakeLodLevel;
    CARPART_LOD mMaxSpoilerLodLevel;
    CARPART_LOD mMaxRoofScoopLodLevel;
    CARPART_LOD mMinReflectionLodLevel;
};

struct FrontEndRenderingCarLayout {
    bNode Node;
    RideInfo mRideInfo;
};

struct CameraPositionAccess {
    char pad[0x50];
    float x;
    float y;
    float z;
};

template <typename T> void InitSList(bSList<T> &list) {
    bSListLayout<T> &layout = reinterpret_cast<bSListLayout<T> &>(list);
    T *end = list.EndOfList();

    layout.Head = end;
    layout.Tail = end;
}

} // namespace

inline void *CarEmitterPosition::operator new(unsigned int size) {
    return bOMalloc(CarEmitterPositionSlotPool);
}

inline void CarEmitterPosition::operator delete(void *ptr) {
    bFree(CarEmitterPositionSlotPool, ptr);
}

inline CarEmitterPosition::CarEmitterPosition(ePositionMarker *position_marker) {
    PositionMarker = position_marker;
    X = position_marker->Matrix.v3.x;
    Y = position_marker->Matrix.v3.y;
    Z = position_marker->Matrix.v3.z;
}

inline CarEmitterPosition::CarEmitterPosition(float x, float y, float z) {
    PositionMarker = nullptr;
    X = x;
    Y = y;
    Z = z;
}

template <> inline CarEmitterPosition *bSList<CarEmitterPosition>::EndOfList() {
    return reinterpret_cast<CarEmitterPosition *>(this);
}

template <> inline bSList<CarEmitterPosition>::bSList() {
    Head = EndOfList();
    Tail = EndOfList();
}

template <> inline CarEmitterPosition *bSList<CarEmitterPosition>::AddTail(CarEmitterPosition *node) {
    CarEmitterPosition *prev_tail = Tail;

    Tail = node;
    prev_tail->Next = node;
    node->Next = EndOfList();
    return node;
}

bool GetNumCarEffectMarkerHashes(CarEffectPosition fx_pos, int &count_out) {
    bool position_marker_based_fxpos = false;
    count_out = 0;
    if (FXMarkerNameHashMappings[fx_pos].marker_count != 0xFF) {
        count_out = FXMarkerNameHashMappings[fx_pos].marker_count;
        position_marker_based_fxpos = true;
    }

    return position_marker_based_fxpos;
}

const unsigned int *GetCarEffectMarkerHashes(CarEffectPosition fx_pos) {
    return reinterpret_cast<const unsigned int *>(&FXMarkerNameHashMappings[fx_pos].marker_count);
}

CarPartCullingPlaneInfo CarPartCullingPlaneInfoTable[11];
const CarPartCullingPlaneInfo *pCurrentPartCullingPlaneInfo = nullptr;
extern "C" void __5ePoly(ePoly *);

void CarPartCuller::InitPart(eCullableCarParts type, const bVector3 *position) {
    CarPartCullingPlaneInfo *plane_info = &CarPartCullingPlaneInfoTable[type];
    CarPartInfo *part_info = &this->CarPartInfoTable[type];

    for (int n = 0; n < plane_info->NumPlanes; n++) {
        if (bAbs(1.0f - bLength(plane_info->Normal[n])) > 0.01f) {
            bNormalize(&plane_info->Normal[n], &plane_info->Normal[n]);
        }
    }

    part_info->Position = *position;
}

void CarPartCuller::CullParts(bVector3 *camera_eye, unsigned short stang) {
    ProfileNode profile_node;
    bVector3 Modcamera_eye = *camera_eye;
    bVector3 *unModcamera_eyeP = &Modcamera_eye;
    bMatrix4 turnMat;

    if (stang != 0) {
        eIdentity(&turnMat);
        eRotateZ(&turnMat, &turnMat, -stang);
        eMulVector(&Modcamera_eye, &turnMat, camera_eye);
    }

    for (eCullableCarParts type = CULLABLE_CAR_PART_TIRE_FL; type < NUM_CULLABLE_CAR_PARTS; type = (eCullableCarParts)((int)type + 1)) {
        bool visible = true;
        CarPartCullingPlaneInfo *plane_info = &CarPartCullingPlaneInfoTable[type];
        CarPartInfo *part_info = &this->CarPartInfoTable[type];
        float omodifier = plane_info->NormalDistance[0];

        if (IsGameFlowInFrontEnd())
            continue;

        camera_eye = (type - 4 > 1) ? unModcamera_eyeP : &Modcamera_eye;

        if (type == 1) {
            if (stang > 0x8000) {
                stang = -stang;
            }
            plane_info->NormalDistance[0] = omodifier + (stang / culldiv);
        } else {
            plane_info->NormalDistance[0] = omodifier;
        }

        if (plane_info->NumPlanes > 0) {
            bVector3 v = *camera_eye - part_info->Position;
            int debug_print = 0;

            if (plane_info->Polarity == CULLING_POLARITY_ANY_VISIBLE) {
                if (plane_info->NumPlanes < debug_print) {
                    debug_print = plane_info->NumPlanes;
                    int n = 0;

                    for (; n < debug_print; n++) {
                        bVector3 normal = plane_info->Normal[n];
                        if (pCurrentPartCullingPlaneInfo != nullptr) {
                            bNormalize(&normal, &normal);
                        }
                        debug_print = plane_info->NumPlanes;

                        float distance = bDot(&v, &normal) - plane_info->Normal[n].x;
                        if (distance < 0.0f)
                            break;
                    }

                    if (n == debug_print) {
                        visible = false;
                    }
                    break;
                }
            } else if (plane_info->Polarity == CULLING_POLARITY_ALL_MUST_BE_VISIBLE) {
                for (int n = 0; n < plane_info->NumPlanes; n++) {
                    bVector3 normal = plane_info->Normal[n];
                    if (pCurrentPartCullingPlaneInfo != nullptr) {
                        bNormalize(&normal, &normal);
                    }

                    float distance = bDot(&v, &normal) - plane_info->Normal[n].x;
                    if (distance >= 0.0f) {
                        visible = false;
                        break;
                    }
                }
            }
        }

        part_info->IsVisible = visible;
        plane_info->NormalDistance[0] = omodifier;

        if (visible) {
            part_info->NumCulledVisible++;
        } else {
            part_info->NumCulledNotVisible++;
        }
    }
}

int NISCopCarDriverVisible = 0;
int NISRaceDriverVisible = 1;

void SetNISRaceDriverVisible(int visible /* r3 */) {
    NISRaceDriverVisible = visible;
}

bMatrix4 CarScaleMatrix;
CarEffectParam CarEffectParameters[29] = {
    {"CARFX_NONE", 0},
    {"CARFX_DRIVE_OVER", 0},
    {"CARFX_DRIVE_OVER2", 0},
    {"CARFX_SKID_SMOKE", 0},
    {"CARFX_TIRE_SPEW", 0},
    {"CARFX_BOTTOM_OUT", 0},
    {"CARFX_DAM_RADIATOR", 0},
    {"CARFX_DAM_ENGINE", 0},
    {"CARFX_BLOWN_TIRE", 0},
    {"CARFX_DRIVE_ON_FLAT_TIRE", 0},
    {"CARFX_NITRO", 0},
    {"CARFX_EXHAUST_SMOKE", 0},
    {"CARFX_EXHAUST_BLOWOFF", 0},
    {"CARFX_NOS_BLOWOFF", 0},
    {"CARFX_BREAK_SIDE_MIRROR_LEFT", 0},
    {"CARFX_BREAK_SIDE_MIRROR_RIGHT", 0},
    {"CARFX_BREAK_LICENSE_PLATE_FRONT", 0},
    {"CARFX_BREAK_LICENSE_PLATE_RIGHT", 0},
    {"CARFX_BREAK_HEADLIGHT_LEFT", 0},
    {"CARFX_BREAK_HEADLIGHT_RIGHT", 0},
    {"CARFX_BREAK_BRAKELIGHT_LEFT", 0},
    {"CARFX_BREAK_BRAKELIGHT_RIGHT", 0},
    {"CARFX_BREAK_BRAKELIGHT_CENTRE", 0},
    {"CARFX_BREAK_WINDSHIELD", 0},
    {"CARFX_BREAK_WINDOW_REAR", 0},
    {"CARFX_BREAK_WINDOW_LEFT_FRONT", 0},
    {"CARFX_BREAK_WINDOW_LEFT_REAR", 0},
    {"CARFX_BREAK_WINDOW_RIGHT_FRONT", 0},
    {"CARFX_BREAK_WINDOW_RIGHT_REAR", 0},
};

ePointSprite3D TestSprite;

bMatrix4 LeftTireRotateZMatrix;
bMatrix4 LeftTireMirrorMatrix;

TextureInfo *pTextureInfo2PlayerHeadlight1;

void InitCarRender() {
    CarPartModelPool = bNewSlotPool(0x18, 0x400, "CarPartModelPool", 0);

    eIdentity(&LeftTireRotateZMatrix);
    eRotateZ(&LeftTireRotateZMatrix, &LeftTireRotateZMatrix, 0x8000);

    eIdentity(&LeftTireMirrorMatrix);
    LeftTireMirrorMatrix.v0.x = 1.0f;
    LeftTireMirrorMatrix.v1.y = -1.0f;
    LeftTireMirrorMatrix.v2.z = 1.0f;
}

void InitCarEffects() {
    CarEmitterPositionSlotPool = bNewSlotPool(0x14, MAX_CAR_PART_MODELS, "CarEmitterPositionSlotPool", GetVirtualMemoryAllocParams());
    for (int i = 0; i < 0x1D; i++) {
        CarEffectParameters[i].NameHash = bStringHash(CarEffectParameters[i].Name);
    }

    eIdentity(&CarScaleMatrix);

    TestSprite.X = 0.0f;
    TestSprite.Y = 0.0f;
    TestSprite.Z = 0.0f;
    TestSprite.Radius = 1.0f;
    TestSprite.S0 = 0.0f;
    TestSprite.T0 = 0.0f;
    TestSprite.S1 = 1.0f;
    TestSprite.T1 = 1.0f;
    TestSprite.Cos = 1.0f;
    TestSprite.Sin = 0.0f;
    TestSprite.Colour = 0x80808080;
}

void CloseCarEffects() {
    bDeleteSlotPool(CarEmitterPositionSlotPool);
}

eModel StandardCubeModel;
eModel StandardDebugModel;

void InitStandardModels() {
    StandardCubeModel.Init(0xC7395A8);
    StandardDebugModel.Init(bStringHash("DEBUG_LOD_CUBE"));
}

bTList<FrontEndRenderingCar> FrontEndRenderingCarList;

FrontEndRenderingCar::FrontEndRenderingCar(RideInfo *ride_info, int view_id) {
    this->RenderInfo = nullptr;

    this->ReInit(ride_info);
    this->ViewID = view_id;

    bFill(&this->Position, 0.0f, 0.0f, 0.0f);

    eIdentity(&this->BodyMatrix);
    eIdentity(&this->TireMatrices[0]);
    eIdentity(&this->TireMatrices[1]);
    eIdentity(&this->TireMatrices[2]);
    eIdentity(&this->TireMatrices[3]);
    eIdentity(&this->BrakeMatrices[0]);
    eIdentity(&this->BrakeMatrices[1]);
    eIdentity(&this->BrakeMatrices[2]);
    eIdentity(&this->BrakeMatrices[3]);

    this->OverrideModel = nullptr;
    this->Visible = 1;
    this->nPasses = 1;
    this->Reflection = 0;
    this->LightsOn = 0;
    this->CopLightsOn = 0;

    FrontEndRenderingCarList.AddTail(this);
}

bool FrontEndRenderingCar::LookupWheelPosition(unsigned int index, bVector4 *position) {
    if (this->RenderInfo != nullptr && position != nullptr) {
        this->RenderInfo->GetAttributes().TireOffsets(*reinterpret_cast<UMath::Vector4 *>(position), index);

        position->z += this->RenderInfo->GetAttributes().FECompressions(index / 2);
        position->w = 1.0f;

        return true;
    } else {
        return false;
    }
}

bool FrontEndRenderingCar::LookupWheelRadius(unsigned int index /* r4 */, float &radius /* r5 */) {
    if (this->RenderInfo == nullptr)
        return false;

    UMath::Vector4 pos;
    this->RenderInfo->GetAttributes().TireOffsets(pos, index);
    radius = pos.w;

    return true;
}

void FrontEndRenderingCar::ReInit(struct RideInfo *ride_info /* r31 */) {
    if (this->RenderInfo != nullptr) {
        CarRenderInfo *info;

        delete this->RenderInfo;
        this->RenderInfo = nullptr;
    }

    if (ride_info != nullptr) {

        this->mRideInfo = *ride_info;
        this->RenderInfo = ::new (__FILE__, __LINE__) CarRenderInfo(&this->mRideInfo);
    }
}

FrontEndRenderingCar::~FrontEndRenderingCar() {
    eWaitUntilRenderingDone();

    if (this->RenderInfo != nullptr) {
        delete this->RenderInfo;
        this->RenderInfo = nullptr;
    }

    if (this->OverrideModel != nullptr) {
        delete this->OverrideModel;
        this->OverrideModel = nullptr;
    }

    this->Remove();
}

float NISCopCarDoorOpenAmount[4];
bMatrix4 NISCopCarDoorOpenMarkers[4];
bMatrix4 NISCopCarDoorClosedMarkers[4];

// UNSOLVED, to preserve my sanity
CarRenderInfo::CarRenderInfo(RideInfo *ride_info)
    : mDamageBehaviour(nullptr), //
      mWCollider(nullptr), //
      mWorldPos(0.025f), //
      mEmitterPositionsInitialized(false), //
      mOnLights(0), //
      mBrokenLights(0), //
      mRadius(lbl_8040AA60), //
      mAttributes(0xeec2271a, 0, nullptr), //
      mFlashing(false), //
      mFlashInterval(0.0f)
{
    ProfileNode profile_node;
    CarTypeInfo *info = &CarTypeInfoArray[ride_info->Type];
    char *car_base_name = info->BaseModelName;
    bVector3 tire_positions[4];
    float wheel_radius[4];

    bMemSet(&this->TheCarPartCuller, 0, sizeof(this->TheCarPartCuller));
    this->mAttributes.ChangeWithDefault(Attrib::StringToLowerCaseKey(car_base_name));
    *reinterpret_cast<unsigned int *>(&this->mMirrorLeftWheels) =
        static_cast<unsigned char>(this->mAttributes.WheelSpokeCount()) >> 7;
    bMemSet(&this->mDamageInfoCache, 0, 0x14);

    this->AnimTime = 0.0f;
    for (unsigned int wheel = 0; wheel < 4; wheel++) {
        UMath::Vector4 tire_offset;

        this->GetAttributes().TireOffsets(tire_offset, wheel);
        tire_positions[wheel] = bVector3(tire_offset.x, tire_offset.y, tire_offset.z);
        wheel_radius[wheel] = tire_offset.w;
    }

    this->WheelWidths[0] = WheelStandardWidth;
    this->WheelWidths[1] = WheelStandardWidth;
    this->WheelRadius[0] = WheelStandardRadius;
    this->WheelRadius[1] = WheelStandardRadius;

    for (int i = 0; i < 4; i++) {
        this->WheelWidthScales[i] = 1.0f;
        this->WheelRadiusScales[i] = 1.0f;
    }

    this->mVelocity.x = 0.0f;
    this->mVelocity.y = 0.0f;
    this->mVelocity.z = 0.0f;
    this->mAngularVelocity.x = 0.0f;
    this->mAngularVelocity.y = 0.0f;
    this->mAngularVelocity.z = 0.0f;
    this->mAcceleration.x = 0.0f;
    this->mAcceleration.y = 0.0f;
    this->mAcceleration.z = 0.0f;

    if (TheGameFlowManager.IsInFrontend()) {
        this->mMinLodLevel = reinterpret_cast<CarRenderRideInfoLayout *>(ride_info)->mMinFELodLevel;
        this->mMaxLodLevel = reinterpret_cast<CarRenderRideInfoLayout *>(ride_info)->mMaxFELodLevel;
    } else {
        this->mMinLodLevel = reinterpret_cast<CarRenderRideInfoLayout *>(ride_info)->mMinLodLevel;
        this->mMaxLodLevel = reinterpret_cast<CarRenderRideInfoLayout *>(ride_info)->mMaxLodLevel;
    }

    CARPART_LOD min_reflection_lod = reinterpret_cast<CarRenderRideInfoLayout *>(ride_info)->mMinReflectionLodLevel;

    this->pRideInfo = ride_info;
    this->mMinReflectionLodLevel = min_reflection_lod;
    this->LastCarPartChanged = -1;
    this->CarTimebaseStart = bRandom(1.0f);
    this->mDeltaTime = 0.0f;
    CarTypeInfo *car_type_info = &CarTypeInfoArray[this->pRideInfo->Type];
    int is_traffic_car = car_type_info->GetCarUsageType() == CAR_USAGE_TYPE_TRAFFIC;

    bMemSet(this->mCarPartModels, 0, sizeof(this->mCarPartModels));
    this->pCarTypeInfo = car_type_info;
    this->CarbonHood = 0;
    GetUsedCarTextureInfo(&this->mUsedTextureInfos, this->pRideInfo, 0);
    {
        UsedCarTextureInfo *used_texture_info = &this->mUsedTextureInfos;
        unsigned int mapped_skin_hash = used_texture_info->MappedSkinHash;
        unsigned int mapped_skin_b_hash = used_texture_info->MappedSkinBHash;
        unsigned int mapped_global_hash = used_texture_info->MappedGlobalHash;
        unsigned int mapped_badging_hash = used_texture_info->MappedBadging;
        unsigned int mapped_wheel_hash = used_texture_info->MappedWheelHash;
        unsigned int mapped_spinner_hash = used_texture_info->MappedSpinnerHash;
        unsigned int mapped_spoiler_hash = used_texture_info->MappedSpoilerHash;
        unsigned int mapped_roof_scoop_hash = used_texture_info->MappedRoofScoopHash;
        unsigned int mapped_dash_skin_hash = used_texture_info->MappedDashSkinHash;

        this->MasterReplacementTextureTable[REPLACETEX_CARSKIN].SetOldNameHash(mapped_skin_hash);
        this->MasterReplacementTextureTable[REPLACETEX_CARSKINB].SetOldNameHash(mapped_skin_b_hash);
        this->MasterReplacementTextureTable[REPLACETEX_GLOBALSKIN].SetOldNameHash(mapped_global_hash);
        this->MasterReplacementTextureTable[REPLACETEX_CARBONSKIN].SetOldNameHash(0xA7366AE6);
        this->MasterReplacementTextureTable[REPLACETEX_GLOBALCARBONSKIN].SetOldNameHash(0x3C84D757);
        this->MasterReplacementTextureTable[REPLACETEX_BADGING].SetOldNameHash(mapped_badging_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WHEEL].SetOldNameHash(mapped_wheel_hash);
        this->MasterReplacementTextureTable[REPLACETEX_SPINNER].SetOldNameHash(mapped_spinner_hash);
        this->MasterReplacementTextureTable[REPLACETEX_SPOILER].SetOldNameHash(mapped_spoiler_hash);
        this->MasterReplacementTextureTable[REPLACETEX_ROOF_SCOOP].SetOldNameHash(mapped_roof_scoop_hash);
        this->MasterReplacementTextureTable[REPLACETEX_DASHSKIN].SetOldNameHash(mapped_dash_skin_hash);
        this->MasterReplacementTextureTable[REPLACETEX_DRIVER].SetOldNameHash(0x5799E60B);
        unsigned int mapped_tire_hash = used_texture_info->MappedTireHash;
        this->MasterReplacementTextureTable[REPLACETEX_TIRE].SetOldNameHash(mapped_tire_hash);
        if (is_traffic_car != 0) {
            this->MasterReplacementTextureTable[REPLACETEX_TIRE].SetNewNameHash(bStringHash("_N"));
        }
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_FRONT].SetOldNameHash(bStringHash("WINDOW_FRONT"));
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_REAR].SetOldNameHash(bStringHash("WINDOW_REAR"));
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_LEFT_FRONT].SetOldNameHash(bStringHash("WINDOW_LEFT_FRONT"));
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_LEFT_REAR].SetOldNameHash(bStringHash("WINDOW_LEFT_REAR"));
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_RIGHT_FRONT].SetOldNameHash(bStringHash("WINDOW_RIGHT_FRONT"));
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_RIGHT_REAR].SetOldNameHash(bStringHash("WINDOW_RIGHT_REAR"));
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_REAR_DEFOST].SetOldNameHash(bStringHash("REAR_DEFROSTER"));
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_FRONT].SetOldNameHash(bStringHash("WINDOW_FRONT"));
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_REAR].SetOldNameHash(bStringHash("WINDOW_REAR"));
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_LEFT_FRONT].SetOldNameHash(bStringHash("WINDOW_LEFT_FRONT"));
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_LEFT_REAR].SetOldNameHash(bStringHash("WINDOW_LEFT_REAR"));
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_RIGHT_FRONT].SetOldNameHash(bStringHash("WINDOW_RIGHT_FRONT"));
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_RIGHT_REAR].SetOldNameHash(bStringHash("WINDOW_RIGHT_REAR"));
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_REAR_DEFOST].SetOldNameHash(bStringHash("REAR_DEFROSTER"));
        this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_LEFT].SetOldNameHash(0xA7E6EA53);
        this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_RIGHT].SetOldNameHash(0xA532FC46);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_LEFT].SetOldNameHash(this->mUsedTextureInfos.MappedLightHash[0]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_RIGHT].SetOldNameHash(this->mUsedTextureInfos.MappedLightHash[1]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_LEFT].SetOldNameHash(this->mUsedTextureInfos.MappedLightHash[2]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_RIGHT].SetOldNameHash(this->mUsedTextureInfos.MappedLightHash[3]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_CENTRE].SetOldNameHash(this->mUsedTextureInfos.MappedLightHash[4]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_LEFT].SetOldNameHash(this->mUsedTextureInfos.MappedLightHash[5]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_RIGHT].SetOldNameHash(this->mUsedTextureInfos.MappedLightHash[6]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_GLASS_LEFT].SetOldNameHash(this->mUsedTextureInfos.MappedLightHash[7]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_GLASS_RIGHT].SetOldNameHash(this->mUsedTextureInfos.MappedLightHash[8]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_GLASS_CENTRE].SetOldNameHash(this->mUsedTextureInfos.MappedLightHash[9]);
        this->BrakeLeftReplacementTextureTable[0].SetOldNameHash(0x17F9F794);
        this->BrakeLeftReplacementTextureTable[0].SetNewNameHash(0x85E9C79E);
        this->BrakeLeftReplacementTextureTable[1].SetOldNameHash(this->mUsedTextureInfos.MappedGlobalHash);
        this->BrakeRightReplacementTextureTable[0].SetOldNameHash(0x17F9F794);
        this->BrakeRightReplacementTextureTable[0].SetNewNameHash(0x17F9F794);
        this->BrakeRightReplacementTextureTable[1].SetOldNameHash(this->mUsedTextureInfos.MappedGlobalHash);
    }
    this->SwitchSkin(this->pRideInfo);
    {
        UsedCarTextureInfo *used_texture_info = &this->mUsedTextureInfos;
        unsigned int badging_hash = used_texture_info->MappedBadging;
        const char *europe_suffix = BuildRegion::IsEurope() ? "_EU" : nullptr;

        if (europe_suffix != nullptr) {
            unsigned int europe_badging_hash = bStringHash(europe_suffix, badging_hash);

            if (GetTextureInfo(europe_badging_hash, 0, 0) != nullptr) {
                badging_hash = europe_badging_hash;
            }
        }
        TextureInfo *badging_texture = GetTextureInfo(badging_hash, 0, 0);

        if (badging_texture != nullptr) {
            badging_texture->ApplyAlphaSorting = 0;
        }

        this->MasterReplacementTextureTable[REPLACETEX_BADGING].SetNewNameHash(badging_hash);
    }
    this->UpdateCarReplacementTextures();
    this->UpdateDecalTextures(ride_info);
    this->MasterReplacementTextureTable[REPLACETEX_DRIVER].SetNewNameHash(0xA244D489);
    this->UpdateCarParts();
    this->ShadowTexture = GetTextureInfo(bStringHash("CARSHADOW"), 1, 0);
    this->ShadowRampTexture = GetTextureInfo(0xBADB4475, 0, 0);

    if (this->ShadowTexture != nullptr) {
        this->ShadowTexture->ApplyAlphaSorting = 0;
        this->ShadowTexture->RenderingOrder = 2;
    }

    if (this->ShadowRampTexture != nullptr) {
        this->ShadowRampTexture->ApplyAlphaSorting = 0;
        this->ShadowRampTexture->RenderingOrder = 3;
    }

    {
        eModel *base_model = this->mCarPartModels[CARSLOTID_BASE][0][this->mMinLodLevel].GetModel();

        if (base_model != nullptr) {
            bVector4 *pivot_position = base_model->GetPivotPosition();
            float pivot_x;

            if (pivot_position != nullptr) {
                pivot_x = pivot_position->x;
            } else {
                pivot_x = 0.0f;
            }
            this->PivotPosition.x = pivot_x;

            float pivot_y;

            if (pivot_position != nullptr) {
                pivot_y = pivot_position->y;
            } else {
                pivot_y = 0.0f;
            }
            this->PivotPosition.y = pivot_y;

            float pivot_z;

            if (pivot_position != nullptr) {
                pivot_z = pivot_position->z;
            } else {
                pivot_z = 0.0f;
            }
            this->PivotPosition.z = pivot_z;
        }
    }

    this->CreateCarLightFlares();

    {
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_TIRE_FL, &tire_positions[0]);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_TIRE_FR, &tire_positions[1]);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_TIRE_RR, &tire_positions[2]);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_TIRE_RL, &tire_positions[3]);

        bVector3 v_left = tire_positions[0] + tire_positions[3];
        v_left /= 2.0f;
        bVector3 v_right = tire_positions[1] + tire_positions[2];
        v_right /= 2.0f;

        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_BRAKE_FL, &tire_positions[0]);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_BRAKE_FR, &tire_positions[1]);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_BRAKE_RR, &tire_positions[2]);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_BRAKE_RL, &tire_positions[3]);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_SIDE_FRONT, &tire_positions[0]);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_SIDE_REAR, &tire_positions[2]);

        bVector3 v_underneath = v_left + v_right;
        v_underneath /= 2.0f;
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_UNDERNEATH, &v_underneath);

        bVector3 v_front_diff = tire_positions[0] - tire_positions[1];
        bVector3 v_side_diff = tire_positions[1] - tire_positions[2];
        bVector3 v_normal;
        bCross(&v_normal, &v_front_diff, &v_side_diff);
        bNormalize(&v_normal, &v_normal);

        float tire_radius = wheel_radius[0];
        if (tire_radius < wheel_radius[1]) {
            tire_radius = wheel_radius[1];
        }
        if (tire_radius < wheel_radius[2]) {
            tire_radius = wheel_radius[2];
        }
        if (tire_radius < wheel_radius[3]) {
            tire_radius = wheel_radius[3];
        }

        bScale(&v_normal, &v_normal, tire_radius);
        bAdd(&v_underneath, &v_underneath, &v_normal);
    }

    {
        for (int lod = this->mMinLodLevel; lod < this->mMaxLodLevel + 1; lod++) {
            eModel *smooth_normal_models[0x4C];

            bMemSet(smooth_normal_models, 0, sizeof(smooth_normal_models));
            for (int slot = 0; slot < 0x4C; slot++) {
                eModel *model = this->mCarPartModels[slot][0][lod].GetModel();
                eModel *smooth_model = nullptr;

                if (model != nullptr) {
                    eModel *previous_model = nullptr;

                    if (slot == CARSLOTID_RIGHT_SIDE_MIRROR) {
                        goto use_smooth_normal_model;
                    } else {
                        if (slot > CARSLOTID_RIGHT_SIDE_MIRROR) {
                            if (slot != CARSLOTID_SPOILER &&
                                (slot < CARSLOTID_SPOILER || slot > CARSLOTID_BRAKELIGHT || slot < CARSLOTID_ROOF)) {
                                goto skip_smooth_normal_model;
                            }
                            goto use_smooth_normal_model;
                        }
                        if (slot == CARSLOTID_BODY) {
                            goto use_smooth_normal_model;
                        }
                        if (slot < CARSLOTID_BODY + 1) {
                            if (slot == CARSLOTID_BASE) {
                                goto use_smooth_normal_model;
                            }
                        } else if (slot == CARSLOTID_LEFT_SIDE_MIRROR) {
                            goto use_smooth_normal_model;
                        }

                        goto skip_smooth_normal_model;
                    }

use_smooth_normal_model:
                    if (lod > this->mMinLodLevel) {
                        previous_model = this->mCarPartModels[slot][0][lod - 1].GetModel();
                    }

                    if (lod <= this->mMinLodLevel || previous_model == nullptr ||
                        previous_model->GetNameHash() != model->GetNameHash()) {
                        smooth_model = model;
                    }

skip_smooth_normal_model: {}
                }

                smooth_normal_models[slot] = smooth_model;
            }

            eSmoothNormals(smooth_normal_models, 0x4C);
        }
    }

    {
        unsigned int light_material_hash = 0;
        CarPart *base_paint_part = ride_info->GetPart(CARSLOTID_BASE_PAINT);

        if (base_paint_part != nullptr) {
            light_material_hash = CarPart_GetAppliedAttributeUParam(base_paint_part, 0x6BA02C05, 0);
        }

        this->LightMaterial_CarSkin = elGetLightMaterial(light_material_hash);
        this->LightMaterial_Carbon = elGetLightMaterial(bStringHash("CARBONFIBRE"));

        {
            CarPart *window_tint_part = ride_info->GetPart(CARSLOTID_WINDOW_TINT);
            unsigned int window_tint_material_hash = 0x471A1DCA;

            if (window_tint_part != nullptr) {
                window_tint_material_hash = CarPart_GetAppliedAttributeUParam(window_tint_part, 0x6BA02C05, 0);
            }

            this->LightMaterial_WindowTint = elGetLightMaterial(window_tint_material_hash);
        }

        {
            CarPart *paint_rim_part = ride_info->GetPart(CARSLOTID_PAINT_RIM);
            CarPart *front_wheel_part = ride_info->GetPart(CARSLOTID_FRONT_WHEEL);
            CarPart *spoiler_part = ride_info->GetPart(CARSLOTID_SPOILER);
            CarPart *roof_part = ride_info->GetPart(CARSLOTID_ROOF);
            unsigned int wheel_rim_material_hash = 0;
            unsigned int caliper_material_hash = 0;
            unsigned int spoiler_material_hash = 0;
            unsigned int roof_material_hash = 0;
            eLightMaterial *wheel_rim_material;
            eLightMaterial *caliper_material;
            eLightMaterial *spoiler_material;
            eLightMaterial *roof_material;

            this->LightMaterial_Spinner = nullptr;

            if (paint_rim_part == nullptr || (reinterpret_cast<unsigned char *>(paint_rim_part)[5] >> 5) == 0) {
                paint_rim_part = nullptr;
            }

            if (front_wheel_part == nullptr || (reinterpret_cast<unsigned char *>(front_wheel_part)[5] >> 5) == 0) {
                front_wheel_part = nullptr;
            }

            if (spoiler_part == nullptr || (reinterpret_cast<unsigned char *>(spoiler_part)[5] >> 5) == 0) {
                spoiler_part = nullptr;
            }

            if (roof_part == nullptr || (reinterpret_cast<unsigned char *>(roof_part)[5] >> 5) == 0) {
                roof_part = nullptr;
            }

            if (paint_rim_part != nullptr) {
                wheel_rim_material_hash = CarPart_GetAppliedAttributeUParam(paint_rim_part, 0x6BA02C05, 0);
            }

            if (front_wheel_part != nullptr) {
                caliper_material_hash = CarPart_GetAppliedAttributeUParam(front_wheel_part, 0x6BA02C05, 0);
            }

            if (spoiler_part != nullptr) {
                spoiler_material_hash = CarPart_GetAppliedAttributeUParam(spoiler_part, 0x6BA02C05, 0);
            }

            if (roof_part != nullptr) {
                roof_material_hash = CarPart_GetAppliedAttributeUParam(roof_part, 0x6BA02C05, 0);
            }

            if (wheel_rim_material_hash != 0) {
                wheel_rim_material = elGetLightMaterial(wheel_rim_material_hash);
            } else {
                wheel_rim_material = nullptr;
            }

            if (caliper_material_hash != 0) {
                caliper_material = elGetLightMaterial(caliper_material_hash);
            } else {
                caliper_material = nullptr;
            }

            if (spoiler_material_hash != 0) {
                spoiler_material = elGetLightMaterial(spoiler_material_hash);
            } else {
                spoiler_material = nullptr;
            }

            if (roof_material_hash != 0) {
                roof_material = elGetLightMaterial(roof_material_hash);
            } else {
                roof_material = nullptr;
            }

            this->LightMaterial_Caliper = caliper_material;
            this->LightMaterial_WheelRim = wheel_rim_material;
            this->LightMaterial_Spoiler = spoiler_material;
            this->LightMaterial_Roof = roof_material;
        }
    }

    this->UpdateWheelYRenderOffset();
    
    { //?
        eModel *front_wheel_model = this->mCarPartModels[CARSLOTID_FRONT_WHEEL][0][this->mMinLodLevel].GetModel();
        eModel *rear_wheel_model  = this->mCarPartModels[CARSLOTID_REAR_WHEEL][0][this->mMinLodLevel].GetModel();

        ePositionMarker *front_position_marker;
        if (front_wheel_model != nullptr) {
            front_position_marker = front_wheel_model->GetPostionMarker(0xa4ccd4ac);
        } else {
            front_position_marker = nullptr;
        }

        ePositionMarker *rear_position_marker;
        if (rear_wheel_model != nullptr) {
            rear_position_marker = rear_wheel_model->GetPostionMarker(0xb2e13a0d);
        } else {
            rear_position_marker = nullptr;
        }

        if (front_position_marker != nullptr && rear_position_marker == nullptr) {
            rear_position_marker = front_position_marker;
        }

        if (front_position_marker != nullptr) {
            this->WheelBrakeMarkerY[0] = front_position_marker->Matrix.v3.y;
        } else {
            this->WheelBrakeMarkerY[0] = 0.0f;
        }
        if (rear_position_marker != nullptr) {
            this->WheelBrakeMarkerY[1] = rear_position_marker->Matrix.v3.y;
        } else {
            this->WheelBrakeMarkerY[1] = 0.0f;
        }
    }

    if (IsNISCopCar(this->pRideInfo->Type)) {
        for (int i = 0; i < 4; i++) {
            NISCopCarDoorOpenAmount[i] = 0;
            eIdentity(&NISCopCarDoorOpenMarkers[i]);
            eIdentity(&NISCopCarDoorClosedMarkers[i]);
        }
        
        eModel *base_model = this->mCarPartModels[CARSLOTID_BODY][0][this->mMinLodLevel].GetModel();
        if (base_model != nullptr) {
            unsigned int open_string_hashes[4] = {0xF91BCA96, 0x8DE14C29, 0x60989ECA, 0xD0F2CD17};
            unsigned int closed_string_hashes[4] = {0x58A2A425, 0x8FE91DD8, 0x05C907D9, 0x7B3CD206};

            for (int i = 0; i < 4; i++) {
                ePositionMarker *open_marker = base_model->GetPostionMarker(open_string_hashes[i]);
                ePositionMarker *closed_marker = base_model->GetPostionMarker(closed_string_hashes[i]);

                if (open_marker == nullptr || closed_marker == nullptr)
                    continue;

                NISCopCarDoorOpenMarkers[i] = open_marker->Matrix;
                NISCopCarDoorClosedMarkers[i] = closed_marker->Matrix;
            }
        }
    }

    if (this->pRideInfo->Type == CARTYPE_COPMIDSIZE || this->pRideInfo->Type == CARTYPE_COPSUV || this->pRideInfo->Type == CARTYPE_COPSPORT ||
        this->pRideInfo->Type == CARTYPE_COPGHOST || this->pRideInfo->Type == CARTYPE_COPGTO || this->pRideInfo->Type == CARTYPE_COPSUVL ||
        this->pRideInfo->Type == CARTYPE_COPGTOGHOST || this->pRideInfo->Type == CARTYPE_COPSPORTHENCH ||
        this->pRideInfo->Type == CARTYPE_COPSPORTGHOST) {
        this->mDamageBehaviour = ::new (__FILE__, __LINE__) VehiclePartDamageBehaviour(this);
    }

    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_FRONT, bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_REAR, bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_LEFT_FRONT, bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_LEFT_REAR, bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_RIGHT_FRONT, bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_RIGHT_REAR, bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_REAR_DEFOST, bStringHash("REAR_DEFROSTER"), 0xa155545);
}

// UNSOLVED
CarRenderInfo::~CarRenderInfo() {
    for (int model_index = 0; model_index < 0x4C; model_index++) {
        for (int model_number = 0; model_number < 1; model_number++) {
            for (int model_lod = this->mMinLodLevel; model_lod <= this->mMaxLodLevel; model_lod++) {
                eModel *model = this->mCarPartModels[model_index][model_number][model_lod].GetModel();
                if (model == nullptr)
                    break;
                if (model->GetNameHash() == 0)
                    break;

                model->UnInit();
                CarPartModelPool->Free(model);
                this->mCarPartModels[model_index][model_number][model_lod].SetModel(nullptr);
                this->mCarPartModels[model_index][model_number][model_lod].Clear();
            }
        }
    }

    if (mDamageBehaviour != nullptr) {
        delete mDamageBehaviour;
        mDamageBehaviour = nullptr;
    }
}

void CarRenderInfo::Init() {
    if (this->mDamageBehaviour != nullptr) {
        this->mDamageBehaviour->Init();
    }

    this->mDamageInfoCache.Clear();
    this->mDamageInfoCache.Value = 0;
}

void CarRenderInfo::Refresh() {
    this->SpoilerPositionMarker = reinterpret_cast<ePositionMarker *>(gDefragFixer.Fix(this->SpoilerPositionMarker));
    this->SpoilerPositionMarker2 = reinterpret_cast<ePositionMarker *>(gDefragFixer.Fix(this->SpoilerPositionMarker2));
    this->RoofScoopPositionMarker = reinterpret_cast<ePositionMarker *>(gDefragFixer.Fix(this->RoofScoopPositionMarker));

    for (int fxpos = 0; fxpos < 0x1C; fxpos++) {
        for (CarEmitterPosition *empos = this->EmitterPositionList[fxpos].GetHead(); empos != this->EmitterPositionList[fxpos].EndOfList();
             empos = empos->GetNext()) {
            if (empos->PositionMarker != nullptr) {
                empos->PositionMarker = reinterpret_cast<ePositionMarker *>(gDefragFixer.Fix(empos->PositionMarker));
            }
        };
    }
}

void CarRenderInfo::SetPlayerDamage(const DamageZone::Info &damageInfo) {
    if (this->mDamageInfoCache.Value == damageInfo.Value)
        return;

    const unsigned int kDamageThresh = 0;
    const unsigned int kDamageWindowThresh = 1;

    this->mDamageInfoCache.Value = damageInfo.Value;

    this->SetCarDamageState(damageInfo.Get(DamageZone::DZ_FRONT) > kDamageThresh, 0x2E, 0x2E);
    this->SetCarDamageState(damageInfo.Get(DamageZone::DZ_REAR) > kDamageThresh, 0x31, 0x31);
    this->SetCarDamageState(damageInfo.Get(DamageZone::DZ_LFRONT) > kDamageThresh, 0x2F, 0x2F);
    this->SetCarDamageState(damageInfo.Get(DamageZone::DZ_RFRONT) > kDamageThresh, 0x30, 0x30);
    this->SetCarDamageState(damageInfo.Get(DamageZone::DZ_LREAR) > kDamageThresh, 0x32, 0x32);
    this->SetCarDamageState(damageInfo.Get(DamageZone::DZ_RREAR) > kDamageThresh, 0x33, 0x33);

    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_FRONT) > kDamageWindowThresh, REPLACETEX_WINDOW_FRONT, bStringHash("WINDOW_FRONT"),
                                 0xa155545);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_REAR) > kDamageWindowThresh, REPLACETEX_WINDOW_REAR, bStringHash("WINDOW_FRONT"),
                                 0xa155545);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_LFRONT) > kDamageWindowThresh, REPLACETEX_WINDOW_LEFT_FRONT,
                                 bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_LREAR) > kDamageWindowThresh, REPLACETEX_WINDOW_LEFT_REAR, bStringHash("WINDOW_FRONT"),
                                 0xa155545);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_RFRONT) > kDamageWindowThresh, REPLACETEX_WINDOW_RIGHT_FRONT,
                                 bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_RREAR) > kDamageWindowThresh, REPLACETEX_WINDOW_RIGHT_REAR,
                                 bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_REAR) > kDamageWindowThresh, REPLACETEX_WINDOW_REAR_DEFOST,
                                 bStringHash("REAR_DEFROSTER"), 0xa155545);
}

void CarRenderInfo::SetCarDamageState(bool on, unsigned int startID, unsigned int endID) {
    bool hidden = !on;

    for (int model_number = 0; model_number < 1; model_number++) {
        for (int model_lod = this->mMinLodLevel; model_lod <= this->mMaxLodLevel; model_lod++) {
            for (unsigned int i = startID; i <= endID; i++) {
                CarPartModel *model = &this->mCarPartModels[i][model_number][model_lod];

                if (!model)
                    continue;
                if (!model->GetModel())
                    continue;

                model->Hide(hidden);
            }
        }
    }
}

void CarRenderInfo::SetCarGlassDamageState(bool on, CarReplacementTexID replacementId, unsigned int undamageHash, unsigned int damageHash) {
    if (on) {
        this->MasterReplacementTextureTable[replacementId].SetNewNameHash(damageHash);
    } else {
        this->MasterReplacementTextureTable[replacementId].SetNewNameHash(undamageHash);
    }
}

void CarRenderInfo::SetDamageInfo(const DamageZone::Info &damageInfo) {
    if (damageInfo.Value != this->mDamageZoneInfo.Value) {
        this->mDamageZoneInfo.Value = damageInfo.Value;
        if (this->mDamageBehaviour != nullptr) {
            if (this->mDamageZoneInfo.Value == 0) {
                this->mDamageBehaviour->Reset();
            } else {
                this->mDamageBehaviour->DamageVehicle(this->mDamageZoneInfo);
            }
        }
    }
    if (this->mDamageBehaviour == nullptr) {
        this->SetPlayerDamage(damageInfo);
    }
}

unsigned int CarRenderInfo::FindCarPart(int slotId) {
    unsigned int model_namehash = 0;

    if (slotId <= 0x4C) {
        eModel *model = this->mCarPartModels[slotId][0][this->mMinLodLevel].GetModel();
        if (model) {
            model_namehash = model->GetNameHash();
        }
    }

    return model_namehash;
}

unsigned int CarRenderInfo::HideCarPart(int slotId, bool hide) {
    unsigned int model_namehash = 0;

    if (slotId <= 0x4C) {
        for (int lodId = this->mMinLodLevel; lodId <= this->mMaxLodLevel; lodId++) {
            CarPartModel *carPartModel = &this->mCarPartModels[slotId][0][lodId];
            eModel *model = carPartModel->GetModel();

            carPartModel->Hide(hide);

            if (lodId == this->mMinLodLevel && model) {
                model_namehash = model->GetNameHash();
            }
        }
    }

    return model_namehash;
}

struct CarPartMetaLayout {
    unsigned short PartNameHashBot;
    unsigned short PartNameHashTop;
    char PartID;
    unsigned char GroupNumber_UpgradeLevel;
    char BaseModelNameHashSelector;
    unsigned char CarTypeNameHashIndex;
    unsigned short NameOffset;
    unsigned short AttributeTableOffset;
    unsigned short ModelNameHashTableOffset;
};

struct CameraAnchorLayout {
    bVector3 mVelocity;
    float mVelMag;
    float mTopSpeed;
    bVector3 mGeomPos;
};

struct CarPartModelLayout {
    unsigned int mModel;
};

static inline eModel *GetPackedCarPartModel(CarPartModel *car_part_model) {
    return reinterpret_cast<eModel *>(reinterpret_cast<CarPartModelLayout *>(car_part_model)->mModel & ~0x3);
}

static inline void SetPackedCarPartModel(CarPartModel *car_part_model, eModel *model) {
    unsigned int &packed_model = reinterpret_cast<CarPartModelLayout *>(car_part_model)->mModel;

    packed_model = reinterpret_cast<unsigned int>(model) | (packed_model & 1);
}

static inline void ClearPackedCarPartModel(CarPartModel *car_part_model) {
    reinterpret_cast<CarPartModelLayout *>(car_part_model)->mModel &= 1;
}

static inline bool DotPassesTest(const bVector3 *point) {
    bVector3 vec = *point - hull_Origin;
    float dot = bDot(&vec, &hull_Normal);

    if (dot < lbl_8040ADC0) {
        dot = -dot;
    }

    return dot < lbl_8040ADEC;
}

static inline void *CarRenderFrameMalloc(unsigned int size) {
    unsigned char *address = CurrentBufferPos;

    if (CurrentBufferEnd <= CurrentBufferPos + size) {
        FrameMallocFailed = 1;
        FrameMallocFailAmount += size;
        address = 0;
    } else {
        CurrentBufferPos += size;
    }

    return address;
}

inline bool CarRenderInfo::IsLightBroken(VehicleFX::ID id) const {
    return (this->mBrokenLights & id) != 0;
}

inline bool CarRenderInfo::IsLightOn(VehicleFX::ID id) const {
    return (this->mOnLights & id) != 0;
}

void elResetLightContext(eDynamicLightContext *light_context);
int elSetupLights(eDynamicLightContext *light_context, eShaperLightRig *shaper_lights, bVector3 *local_pos, bMatrix4 *local_world, bMatrix4 *world_view,
                  eView *view);
int elCloneLightContext(eDynamicLightContext *light_context, bMatrix4 *local_world, bMatrix4 *world_view, bVector4 *camera_world_position, eView *view,
                        eDynamicLightContext *old_context);

void CarRenderInfo::UpdateCarParts() {
    ProfileNode profile_node("UpdateCarParts", 0);

    bInitializeBoundingBox(&this->AABBMin, &this->AABBMax);

    for (int slot_id = 0; slot_id < 0x4C; slot_id++) {
        for (int model_number = 0; model_number < 1; model_number++) {
            for (int lod = this->mMinLodLevel; lod <= this->mMaxLodLevel; lod++) {
                eModel *model = this->mCarPartModels[slot_id][model_number][lod].GetModel();

                if (model != 0 && model->GetNameHash() != 0) {
                    model->UnInit();
                    CarPartModelPool->Free(model);
                    this->mCarPartModels[slot_id][model_number][lod].SetModel(nullptr);
                }
            }
        }
    }

    RideInfo *ride_info = this->pRideInfo;
    for (int slot_id = 0; slot_id < 0x4C; slot_id++) {
        CarPart *car_part = ride_info->GetPart(slot_id);

        if (car_part == 0) {
            continue;
        }

        for (int model_number = 0; model_number < 1; model_number++) {
            for (int lod = this->mMinLodLevel; lod <= this->mMaxLodLevel; lod++) {
                CARPART_LOD special_minimum;
                CARPART_LOD special_maximum;
                CARPART_LOD model_lod = static_cast<CARPART_LOD>(lod);

                if (ride_info->GetSpecialLODRangeForCarSlot(
                        slot_id,
                        &special_minimum,
                        &special_maximum,
                        IsGameFlowInFrontEnd())) {
                    model_lod = static_cast<CARPART_LOD>(bClamp(model_lod, special_minimum, special_maximum));
                }

                unsigned int model_name_hash = CarPart_GetModelNameHash(car_part, model_number, model_lod);

                if (model_name_hash == 0) {
                    continue;
                }

                eModel *model = static_cast<eModel *>(CarPartModelPool->Malloc());

                this->mCarPartModels[slot_id][model_number][lod].SetModel(model);
                model->Init(model_name_hash);

                if (!model->HasSolid()) {
                    model->UnInit();
                    CarPartModelPool->Free(model);
                    this->mCarPartModels[slot_id][model_number][lod].SetModel(nullptr);
                    model = 0;
                }

                if (model != 0) {
                    if (slot_id < CARSLOTID_DECAL_FRONT_WINDOW || slot_id > CARSLOTID_DECAL_RIGHT_QUARTER) {
                        if (slot_id == CARSLOTID_HOOD) {
                            int carbon_hood = CarPart_GetAppliedAttributeIParam(ride_info->GetPart(CARSLOTID_HOOD), 0x721AFF7C, 0);

                            if (carbon_hood != 0) {
                                model->AttachReplacementTextureTable(this->CarbonReplacementTextureTable, REPLACETEX_NUM, 0);
                                this->CarbonHood = 1;
                            } else {
                                model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                                this->CarbonHood = carbon_hood;
                            }
                        } else {
                            model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                        }
                    } else {
                        model->AttachReplacementTextureTable(&this->DecalReplacementTextureTable[(slot_id - CARSLOTID_DECAL_FRONT_WINDOW) * 8], 8, 0);
                    }
                }
            }

            eModel *model;

            if (slot_id <= CARSLOTID_DAMAGE_REAR_BUMPER) {
                if (slot_id >= CARSLOTID_DAMAGE_TRUNK) {
                    goto expand_bbox;
                }

                if (slot_id > CARSLOTID_DAMAGE_COP_LIGHTS) {
                    if (slot_id <= CARSLOTID_DAMAGE_FRONT_BUMPER) {
                        if (slot_id >= CARSLOTID_DAMAGE_HOOD) {
                            goto expand_bbox;
                        }
                    }
                } else {
                    if (slot_id >= CARSLOTID_DAMAGE_BODY) {
                        goto expand_bbox;
                    }
                    if (slot_id == CARSLOTID_BASE) {
                        goto expand_bbox;
                    }
                }
            } else if (slot_id == CARSLOTID_RIGHT_SIDE_MIRROR) {
                goto expand_bbox;
            } else if (slot_id < CARSLOTID_RIGHT_SIDE_MIRROR) {
                if (slot_id == CARSLOTID_BODY) {
                    goto expand_bbox;
                }
                if (slot_id == CARSLOTID_LEFT_SIDE_MIRROR) {
                    goto expand_bbox;
                }
            } else {
                if (slot_id == CARSLOTID_SPOILER) {
                    goto expand_bbox;
                }
                if (slot_id >= CARSLOTID_ROOF) {
                    if (slot_id <= CARSLOTID_HOOD) {
                        goto expand_bbox;
                    }
                }
            }

            goto skip_expand_bbox;

        expand_bbox:
            model = this->mCarPartModels[slot_id][model_number][this->mMinLodLevel].GetModel();

            if (model != 0) {
                bVector3 bbox_min;
                bVector3 bbox_max;

                model->GetBoundingBox(&bbox_min, &bbox_max);
                bExpandBoundingBox(&this->AABBMin, &this->AABBMax, &bbox_min, &bbox_max);
            }

        skip_expand_bbox:
            ;
        }
    }

    for (int model_number = 0; model_number < 1; model_number++) {
        for (int lod = this->mMinLodLevel; lod <= this->mMaxLodLevel; lod++) {
            CarPartModel *front_wheel_part_model = &this->mCarPartModels[CARSLOTID_FRONT_WHEEL][model_number][lod];
            eModel *front_wheel_model = front_wheel_part_model->GetModel();
            CarPartModel *rear_wheel_part_model = &this->mCarPartModels[CARSLOTID_REAR_WHEEL][model_number][lod];
            eModel *rear_wheel_model = rear_wheel_part_model->GetModel();

            if (front_wheel_model != 0 && rear_wheel_model == 0) {
                rear_wheel_model = static_cast<eModel *>(CarPartModelPool->Malloc());
                rear_wheel_part_model->SetModel(rear_wheel_model);
                rear_wheel_model->Init(front_wheel_model->GetNameHash());

                if (!rear_wheel_model->HasSolid()) {
                    rear_wheel_model->UnInit();
                    CarPartModelPool->Free(rear_wheel_model);
                    rear_wheel_part_model->SetModel(nullptr);
                } else {
                    rear_wheel_model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                }
            }

            CarPartModel *front_brake_part_model = &this->mCarPartModels[CARSLOTID_FRONT_BRAKE][model_number][lod];
            CarPartModel *rear_brake_part_model = &this->mCarPartModels[CARSLOTID_REAR_BRAKE][model_number][lod];
            eModel *front_brake_model = front_brake_part_model->GetModel();
            eModel *rear_brake_model = rear_brake_part_model->GetModel();

            if (front_brake_model != 0 && rear_brake_model == 0) {
                rear_brake_model = static_cast<eModel *>(CarPartModelPool->Malloc());
                rear_brake_part_model->SetModel(rear_brake_model);
                rear_brake_model->Init(front_brake_model->GetNameHash());

                if (!rear_brake_model->HasSolid()) {
                    rear_brake_model->UnInit();
                    CarPartModelPool->Free(rear_brake_model);
                    rear_brake_part_model->SetModel(nullptr);
                } else {
                    rear_brake_model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                }
            }
        }
    }

    eModel *front_wheel_model = this->mCarPartModels[CARSLOTID_FRONT_WHEEL][0][this->mMinLodLevel].GetModel();
    eModel *rear_wheel_model = this->mCarPartModels[CARSLOTID_REAR_WHEEL][0][this->mMinLodLevel].GetModel();
    bVector3 bbox_min;
    bVector3 bbox_max;

    if (front_wheel_model != 0) {
        float wheel_width;
        float wheel_radius;

        front_wheel_model->GetBoundingBox(&bbox_min, &bbox_max);
        wheel_width = UMath::Abs(bbox_max.y - bbox_min.y);
        this->WheelWidths[0] = wheel_width;

        wheel_radius = UMath::Abs(bbox_max.x - bbox_min.x);
        this->WheelRadius[0] = wheel_radius * 0.5f;
    }

    if (rear_wheel_model != 0) {
        float wheel_width;
        float wheel_radius;

        rear_wheel_model->GetBoundingBox(&bbox_min, &bbox_max);
        wheel_width = UMath::Abs(bbox_max.y - bbox_min.y);
        this->WheelWidths[1] = wheel_width;

        wheel_radius = UMath::Abs(bbox_max.x - bbox_min.x);
        this->WheelRadius[1] = wheel_radius * 0.5f;
    }

    this->ModelOffset = (this->AABBMax + this->AABBMin) * 0.5f;

    CarPart *base_part = ride_info->GetPart(CARSLOTID_BASE);
    if (base_part == 0) {
        this->RoofScoopPositionMarker = 0;
        this->SpoilerPositionMarker = 0;
        this->SpoilerPositionMarker2 = 0;
    } else {
        eSolid *solid = eFindSolid(CarPart_GetModelNameHash(base_part, 0, this->mMinLodLevel));

        if (solid == 0) {
            this->RoofScoopPositionMarker = 0;
            this->SpoilerPositionMarker = 0;
            this->SpoilerPositionMarker2 = 0;
        } else {
            this->SpoilerPositionMarker = solid->GetPostionMarker(0xC93B73FD);
            this->SpoilerPositionMarker2 = solid->GetPostionMarker(0xF0A9F3CF);
            this->RoofScoopPositionMarker = solid->GetPostionMarker(0x90C81258);
        }
    }

    CarPart *spoiler_part = ride_info->GetPart(CARSLOTID_SPOILER);
    bool mirror_left_wheels = true;

    if (spoiler_part != 0) {
        mirror_left_wheels = (reinterpret_cast<CarPartMetaLayout *>(spoiler_part)->GroupNumber_UpgradeLevel >> 5) == 0;
    }

    for (int lod = this->mMinLodLevel; lod <= this->mMaxLodLevel; lod++) {
        this->mCarPartModels[CARSLOTID_UNIVERSAL_SPOILER_BASE][0][lod].Hide(mirror_left_wheels);
    }
    this->mMirrorLeftWheels = mirror_left_wheels;

    this->SetCarDamageState(false, 0x2E, 0x33);
}

void CarRenderInfo::UpdateWheelYRenderOffset() {
    CarPart *front_wheel = nullptr;
    CarPart *rear_wheel = nullptr;
    int front_upgrade_level = 0;
    int rear_upgrade_level = 0;
    UMath::Vector4 tire_offset;

    if (this->pCarTypeInfo == nullptr) {
        bMemSet(this->WheelYRenderOffset, 0, sizeof(this->WheelYRenderOffset));
        return;
    }

    if (this->pRideInfo != nullptr) {
        front_wheel = this->pRideInfo->GetPart(CARSLOTID_FRONT_WHEEL);
    }
    if (this->pRideInfo != nullptr) {
        rear_wheel = this->pRideInfo->GetPart(CARSLOTID_REAR_WHEEL);
    }

    if (front_wheel != nullptr) {
        front_upgrade_level = 0;
    }
    if (rear_wheel != nullptr) {
        rear_upgrade_level = 0;
    }

    for (int wheel = 0; wheel < 4; wheel++) {
        int wheel_end = (wheel > 1);
        int kit_number = wheel_end ? rear_upgrade_level : front_upgrade_level;
        CarPart *body_part = nullptr;
        int kit_wheel_offset;
        float kit_wheel_offset_float;
        float model_width;
        float model_radius;
        float desired_width;
        float desired_radius;

        const UMath::Vector4 &tire_ref = this->GetAttributes().TireOffsets(wheel);
        this->WheelYRenderOffset[wheel] = -tire_ref.y;

        body_part = this->pRideInfo->GetPart(CARSLOTID_BODY);

        if (body_part != nullptr) {
            kit_number = CarPart_GetAppliedAttributeIParam(body_part, 0x796C0CB0, 0);
        }

        if (wheel_end == 0) {
            if (TweakKitWheelOffsetFront == 0) {
                kit_wheel_offset = this->GetAttributes().KitWheelOffsetFront(kit_number);
            } else {
                kit_wheel_offset = TweakKitWheelOffsetFront;
            }
        } else if (TweakKitWheelOffsetRear == 0) {
            kit_wheel_offset = this->GetAttributes().KitWheelOffsetRear(kit_number);
        } else {
            kit_wheel_offset = TweakKitWheelOffsetRear;
        }

        kit_wheel_offset_float = static_cast<float>(kit_wheel_offset) * 0.001f;
        if (this->WheelYRenderOffset[wheel] <= 0.0f) {
            this->WheelYRenderOffset[wheel] -= kit_wheel_offset_float;
        } else {
            this->WheelYRenderOffset[wheel] += kit_wheel_offset_float;
        }

        model_width = this->WheelWidths[wheel_end];
        model_radius = this->WheelRadius[wheel_end];
        desired_width = this->GetAttributes().TireSkidWidth(wheel);

        if (wheel <= 1) {
            desired_width *= this->GetAttributes().TireSkidWidthKitScale(kit_number).x;
        } else {
            desired_width *= this->GetAttributes().TireSkidWidthKitScale(kit_number).y;
        }

        tire_offset = tire_ref;
        desired_radius = tire_offset.w;

        if (model_width > 0.0f && desired_width > 0.0f) {
            this->WheelWidthScales[wheel] = desired_width / model_width;
        } else {
            this->WheelWidthScales[wheel] = 1.0f;
        }

        if (model_radius > 0.0f && desired_radius > 0.0f) {
            this->WheelRadiusScales[wheel] = desired_radius / model_radius;
        } else {
            this->WheelRadiusScales[wheel] = 1.0f;
        }
    }
}

void CarRenderInfo::UpdateDecalTextures(RideInfo *ride_info) {
    unsigned int alpha_hash = bStringHash("DEFAULTALPHA");

    for (int i = REPLACETEX_DECAL_START; i <= REPLACETEX_DECAL_END; i++) {
        this->MasterReplacementTextureTable[i].SetOldNameHash(CarReplacementDecalHash[i - REPLACETEX_DECAL_START]);
        this->MasterReplacementTextureTable[i].SetNewNameHash(alpha_hash);
    }

    unsigned int decal_hashes[8] = {
        bStringHash("DUMMY_DECAL1"),
        bStringHash("DUMMY_DECAL2"),
        bStringHash("DUMMY_DECAL3"),
        bStringHash("DUMMY_DECAL4"),
        bStringHash("DUMMY_DECAL5"),
        bStringHash("DUMMY_DECAL6"),
        bStringHash("DUMMY_NUMBER_LEFT"),
        bStringHash("DUMMY_NUMBER_RIGHT"),
    };

    for (int i = 0; i < 48; i++) {
        this->DecalReplacementTextureTable[i].SetOldNameHash(decal_hashes[i % 8]);
        this->DecalReplacementTextureTable[i].SetNewNameHash(alpha_hash);
    }

    int hood_decals = 1;
    if (ride_info->GetPart(CARSLOTID_HOOD) == 0) {
        hood_decals = 0;
    }
    unsigned int size_hash = bStringHash("SIZE");
    unsigned int shape_hash = bStringHash("SHAPE");
    unsigned int size_hashes[3] = {
        bStringHash("SMALL"),
        bStringHash("MEDIUM"),
        bStringHash("LARGE"),
    };
    unsigned int shape_hashes[3] = {
        bStringHash("SQUARE"),
        bStringHash("RECT"),
        bStringHash("WIDE"),
    };

    for (int i = CARSLOTID_DECAL_FRONT_WINDOW; i < CARSLOTID_BASE_PAINT; i++) {
        CarPart *decal_model_part = ride_info->GetPart(i);
        int decal_index = i - CARSLOTID_DECAL_FRONT_WINDOW;

        if (decal_model_part != 0 && hood_decals != 0 && CarPart_HasAppliedAttribute(decal_model_part, size_hash) != 0 &&
            CarPart_HasAppliedAttribute(decal_model_part, shape_hash) != 0) {
            unsigned int decal_size = CarPart_GetAppliedAttributeUParam(decal_model_part, size_hash, 0);
            unsigned int decal_shape = CarPart_GetAppliedAttributeUParam(decal_model_part, shape_hash, 0);
            eReplacementTextureTable *replace_table = &this->DecalReplacementTextureTable[decal_index * 8];
            int first_tex_part = CARSLOTID_DECAL_FRONT_WINDOW_TEX0 + decal_index * 8;

            (void)decal_size;
            (void)size_hashes;

            for (int j = 0; j < 8; j++) {
                CarPart *decal_texture_part = ride_info->GetPart(j + first_tex_part);

                if (decal_texture_part != 0) {
                    char buf[128];
                    unsigned int base_hash = CarPart_GetAppliedAttributeUParam(decal_texture_part, bStringHash("NAME"), 0);
                    unsigned int decal_texture_hash;

                    if (decal_shape == shape_hashes[0]) {
                        bStrCpy(buf, "_SQUARE");
                    } else if (decal_shape == shape_hashes[1]) {
                        bStrCpy(buf, "_RECT");
                    } else if (decal_shape == shape_hashes[2]) {
                        bStrCpy(buf, "_WIDE");
                    }

                    decal_texture_hash = bStringHash(buf, base_hash);
                    replace_table[j].SetNewNameHash(decal_texture_hash);
                }
            }
        }
    }
}

bool CarRenderInfo::Render(eView *view, const bVector3 *world_position, const bMatrix4 *body_matrix, bMatrix4 *tire_matrices,
                           bMatrix4 *brake_matrices, bMatrix4 *, unsigned int extra_render_flags, int force_light_state,
                           int reflexion, float shadow_scale, enum CARPART_LOD tireLOD, enum CARPART_LOD carLOD) {
    ProfileNode profile_node;
    bVector3 position = *world_position;
    int car_pixel_size;

    if (this->pCarTypeInfo != 0 && this->pCarTypeInfo->UsageType == CAR_USAGE_TYPE_COP) {
        view->NumCopsTotal++;
    }
    car_pixel_size = view->GetPixelSize(&position, this->mRadius);
    if (eGetCurrentViewMode() == EVIEWMODE_TWOH && iRam8047ff04 != 3) {
        car_pixel_size = static_cast<int>(static_cast<float>(car_pixel_size) * 0.5f);
    }
    if (car_pixel_size < view->PixelMinSize) {
        if (static_cast<unsigned int>(view->GetID() - 13) > 1) {
            return true;
        }
    }
    if (DrawCars == 0) {
        return true;
    }

    Camera *camera = view->GetCamera();
    int is_traffic_car;
    if (this->pCarTypeInfo != 0) {
        is_traffic_car = this->pCarTypeInfo->UsageType == CAR_USAGE_TYPE_RACING;
    } else {
        is_traffic_car = 0;
    }

    eDynamicLightContext *light_context;
    if (CurrentBufferEnd <= CurrentBufferPos + 0x130) {
        FrameMallocFailed = 1;
        FrameMallocFailAmount += 0x130;
        light_context = 0;
    } else {
        light_context = reinterpret_cast<eDynamicLightContext *>(CurrentBufferPos);
        CurrentBufferPos += 0x130;
    }

    bMatrix4 *local_world;
    {
        unsigned char *addr = CurrentBufferPos;
        unsigned int sz = sizeof(bMatrix4);
        if (CurrentBufferEnd <= addr + sz) {
            FrameMallocFailed = 1;
            FrameMallocFailAmount += sz;
            local_world = 0;
        } else {
            CurrentBufferPos = addr + sz;
            local_world = reinterpret_cast<bMatrix4 *>(addr);
        }
    }

    bMatrix4 *cpy_local_world;
    {
        unsigned char *addr = CurrentBufferPos;
        unsigned int sz = sizeof(bMatrix4);
        if (CurrentBufferEnd <= addr + sz) {
            FrameMallocFailed = 1;
            FrameMallocFailAmount += sz;
            cpy_local_world = 0;
        } else {
            CurrentBufferPos = addr + sz;
            cpy_local_world = reinterpret_cast<bMatrix4 *>(addr);
        }
    }

    bMatrix4 *biased_identity;
    {
        unsigned char *addr = CurrentBufferPos;
        unsigned int sz = sizeof(bMatrix4);
        if (CurrentBufferEnd <= addr + sz) {
            FrameMallocFailed = 1;
            FrameMallocFailAmount += sz;
            biased_identity = 0;
        } else {
            CurrentBufferPos = addr + sz;
            biased_identity = reinterpret_cast<bMatrix4 *>(addr);
        }
    }

    bMatrix4 *biased_local_world;
    {
        unsigned char *addr = CurrentBufferPos;
        unsigned int sz = sizeof(bMatrix4);
        if (CurrentBufferEnd <= addr + sz) {
            FrameMallocFailed = 1;
            FrameMallocFailAmount += sz;
            biased_local_world = 0;
        } else {
            CurrentBufferPos = addr + sz;
            biased_local_world = reinterpret_cast<bMatrix4 *>(addr);
        }
    }

    if (light_context == 0 || local_world == 0 || biased_identity == 0 || biased_local_world == 0) {
        return true;
    }

    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(body_matrix), *reinterpret_cast<Mtx44 *>(local_world));
    eMulMatrix(local_world, &CarScaleMatrix, local_world);
    local_world->v3.x = position.x;
    local_world->v3.y = position.y;
    local_world->v3.z = position.z;
    local_world->v3.w = 1.0f;

    if (view->GetVisibleState(&this->AABBMin, &this->AABBMax, local_world) == EVISIBLESTATE_NOT) {
        return true;
    }

    bVector4 camera_world_position;
    if (camera != 0) {
        camera_world_position.x = camera->GetPosition()->x;
        camera_world_position.y = camera->GetPosition()->y;
        camera_world_position.z = camera->GetPosition()->z;
    } else {
        camera_world_position.x = position.x;
        camera_world_position.y = position.y;
        camera_world_position.z = position.z;
    }
    camera_world_position.w = 1.0f;

    Player *player1 = Player::GetPlayerByIndex(0);
    int in_front_end = IsGameFlowInFrontEnd();
    bool print_query_light_mat = PrintQueryLightMat != 0;
    if (print_query_light_mat) {
        PrintLightQuery = 1;
    }

    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(body_matrix), *reinterpret_cast<Mtx44 *>(cpy_local_world));
    cpy_local_world->v3.x = position.x;
    cpy_local_world->v3.y = position.y;
    cpy_local_world->v3.z = position.z;
    cpy_local_world->v3.w = 1.0f;

    eDynamicLightContext base_light_context;
    elResetLightContext(&base_light_context);

    eShaperLightRig *shaper_lights;
    FEManager::Get();
    switch (FEManager::Get()->GetGarageType()) {
    case GARAGETYPE_CAREER_SAFEHOUSE:
        shaper_lights = &ShaperLightsSafehouse;
        break;
    case GARAGETYPE_CUSTOMIZATION_SHOP:
        shaper_lights = &ShaperLightsCShop;
        break;
    case GARAGETYPE_CUSTOMIZATION_SHOP_BACKROOM:
        shaper_lights = &ShaperLightsBackRoom;
        break;
    case GARAGETYPE_CAR_LOT:
        shaper_lights = &ShaperLightsCarLot;
        break;
    default:
        shaper_lights = &ShaperLightsQRace;
        break;
    }
    if (iRam8047ff04 == 6) {
        shaper_lights = &ShaperLightsCarsInGame;
    }

    elSetupLights(&base_light_context, shaper_lights, &position, 0, &hack_man_matrix, view);
    elCloneLightContext(light_context, cpy_local_world, &hack_man_matrix, &camera_world_position, view, &base_light_context);
    this->CarFrame = eFrameCounter;
    if (print_query_light_mat) {
        PrintLightQuery = 0;
    }

    unsigned int disable_env_flag = 0;
    unsigned int disable_env_flag_tires = 0;
    this->UpdateLightStateTextures();
    if (EnableEnvMap == 0 || static_cast<float>(car_pixel_size) < lbl_8040AD70) {
        disable_env_flag = 1;
        disable_env_flag_tires = 1;
    }

    int bodyLodIx = 0;
    int tireLodIx = 0;
    int mMinLod = static_cast<int>(this->mMinLodLevel);
    int mMaxLod = static_cast<int>(this->mMaxLodLevel);
    float const *body_lod_swap_size;

    if (!is_traffic_car) {
        if (mMinLod == 2) {
            goto use_traffic_lod;
        }
        body_lod_swap_size = CarBodyLodSwapSize;
        while (static_cast<float>(car_pixel_size) < body_lod_swap_size[bodyLodIx]) {
            bodyLodIx++;
            tireLodIx++;
        }
    } else {
        extra_render_flags |= 0x80000;
    use_traffic_lod:
        body_lod_swap_size = TrafficCarBodyLodSwapSize;
        while (static_cast<float>(car_pixel_size) < body_lod_swap_size[bodyLodIx]) {
            bodyLodIx++;
            tireLodIx++;
        }
    }

    int car_body_lod = mMaxLod;
    if (bodyLodIx + mMinLod < mMaxLod) {
        car_body_lod = bodyLodIx + mMinLod;
    }
    int car_tire_lod = mMaxLod;
    if (tireLodIx + mMinLod < mMaxLod) {
        car_tire_lod = tireLodIx + mMinLod;
    }

    if (ForceCarLOD != -1) {
        car_body_lod = mMinLod;
        if (mMinLod < ForceCarLOD) {
            car_body_lod = ForceCarLOD;
        }
        if (mMaxLod < car_body_lod) {
            car_body_lod = mMaxLod;
        }
    }
    if (ForceTireLOD != -1) {
        car_tire_lod = mMinLod;
        if (mMinLod < ForceTireLOD) {
            car_tire_lod = ForceTireLOD;
        }
        if (mMaxLod < car_tire_lod) {
            car_tire_lod = mMaxLod;
        }
    }

    if (car_body_lod == -1 && car_tire_lod == -1) {
        return true;
    }

    if (this->pRideInfo != 0 && this->pCarTypeInfo != 0 && this->pCarTypeInfo->UsageType != CAR_USAGE_TYPE_RACING) {
        extra_render_flags |= 0x800;
        if (INIS::Get() != 0) {
            extra_render_flags |= 0x80000;
        }
    }

    bMatrix4 world_local;
    bInvertMatrix(&world_local, local_world);
    bVector3 camera_eye_in_car_space;
    bMulMatrix(&camera_eye_in_car_space, &world_local, camera->GetPosition());
    float fDistanceToCamera = bDistBetween(camera->GetPosition(), reinterpret_cast<bVector3 *>(&local_world->v3));

    if (car_body_lod == 0 && fDistanceToCamera < lbl_8040AD74 && view->GetID() == 1 && INIS::Get() == 0) {
        camera_eye_in_car_space.y += lbl_8040AD78;
        if (bBoundingBoxIsInside(&this->AABBMin, &this->AABBMax, &camera_eye_in_car_space, lbl_8040AD7C) != 0) {
            return true;
        }
        camera_eye_in_car_space.y -= lbl_8040AD78;
    }

    PSMTX44Copy(*reinterpret_cast<Mtx44 *>(local_world), *reinterpret_cast<Mtx44 *>(biased_identity));
    *biased_local_world = eMathIdentityMatrix;
    biased_local_world->v3.x = position.x;
    biased_local_world->v3.y = position.y;
    biased_local_world->v3.z = position.z;
    biased_local_world->v3.w = 1.0f;

    bool nisPlaying = false;
    INIS *nis = INIS::Get();
    if (nis != 0) {
        nisPlaying = nis->IsPlaying();
    }

    if (reflexion == 0) {
        float bias_amount = lbl_8040AD80;
        biased_identity->v3.y += bias_amount;
        float dist_bias = static_cast<float>((fDistanceToCamera - lbl_8040AD84) / fDistanceToCamera);
        if (in_front_end) {
            view->BiasMatrixForZSorting(biased_identity, dist_bias);
        }
        view->BiasMatrixForZSorting(biased_local_world, dist_bias);

        bMatrix4 neg_pos_matrix;
        eIdentity(&neg_pos_matrix);
        neg_pos_matrix.v3.x = -position.x;
        neg_pos_matrix.v3.y = -position.y;
        neg_pos_matrix.v3.z = -position.z;
        eMulMatrix(biased_local_world, &neg_pos_matrix, biased_local_world);
    }

    unsigned short tangR = this->mSteeringR;
    unsigned short tangL = this->mSteeringL;
    unsigned short steerAngle = tangR;
    if (tangR > 0x8000) {
        steerAngle = static_cast<unsigned short>(-tangR);
    }
    unsigned short absL = tangL;
    if (tangL > 0x8000) {
        absL = static_cast<unsigned short>(-tangL);
    }
    if (absL < steerAngle) {
        tangL = tangR;
    }
    this->TheCarPartCuller.CullParts(&camera_eye_in_car_space, tangL);

    unsigned int body_render_flags;
    if (DrawCarShadow != 0 && reflexion == 0) {
        this->DrawAmbientShadow(view, &position, shadow_scale, local_world, &world_local, biased_local_world);
        if (iRam8047ff04 != 3 && this->pCarTypeInfo->UsageType != CAR_USAGE_TYPE_COP && is_traffic_car) {
            this->DrawKeithProjShadow(view, &position, local_world, &world_local, biased_local_world, car_body_lod);
        }
    }

    body_render_flags = 0;
    if (iRam8047ff04 == 6 && view->GetID() != 3) {
        body_render_flags = 0x8000;
    }

    eLightMaterial *light_material_carskin = this->LightMaterial_CarSkin;
    eLightMaterial *light_material_spoiler = this->LightMaterial_Spoiler;
    if (light_material_spoiler == 0) {
        light_material_spoiler = light_material_carskin;
    }
    eLightMaterial *light_material_roof = this->LightMaterial_Roof;
    if (light_material_roof == 0) {
        light_material_roof = light_material_carskin;
    }
    eLightMaterial *light_material_tint = this->LightMaterial_WindowTint;

    bMatrix4 trans_pivot[4];
    bMatrix4 trans_axle[4];
    bMatrix4 brake_trans_pivot[4];
    float wheel_pivot_trans_amount;
    float front_wheel_brake_marker;
    float rear_wheel_brake_marker;
    unsigned short wheel_camber_angle_front;
    float wheel_camber_push_down_front;
    unsigned short wheel_camber_angle_rear;
    float wheel_camber_push_down_rear;
    unsigned short wheel_wobble_angle;
    int tire_visible0;
    int tire_visible1;
    int tire_visible2;
    int tire_visible3;
    eDynamicLightContext *tire_light_context;
    int tires_enabled;
    bMatrix4 *tirelight_world_view;

    for (int slot_id = 0; slot_id < 0x4C; slot_id++) {
        bMatrix4 *slot_local_world;
        {
            unsigned char *addr = CurrentBufferPos;
            unsigned int sz = sizeof(bMatrix4);
            if (CurrentBufferEnd <= addr + sz) {
                FrameMallocFailed = 1;
                FrameMallocFailAmount += sz;
                slot_local_world = 0;
            } else {
                CurrentBufferPos = addr + sz;
                slot_local_world = reinterpret_cast<bMatrix4 *>(addr);
            }
        }
        if (slot_local_world == 0) {
            continue;
        }

        PSMTX44Copy(*reinterpret_cast<Mtx44 *>(biased_identity), *reinterpret_cast<Mtx44 *>(slot_local_world));

        if (slot_id == CARSLOTID_FRONT_WHEEL || slot_id == CARSLOTID_REAR_WHEEL || slot_id == CARSLOTID_FRONT_BRAKE ||
            slot_id == CARSLOTID_REAR_BRAKE || slot_id == CARSLOTID_SPOILER || slot_id == CARSLOTID_ROOF) {
            continue;
        }

        CarPartModel *car_part_model = &this->mCarPartModels[slot_id][0][car_body_lod];
        eModel *model = car_part_model->GetModel();

        if (model != 0) {
            eLightMaterial *paint_material = light_material_carskin;

            if (slot_id == CARSLOTID_SPOILER || slot_id == CARSLOTID_UNIVERSAL_SPOILER_BASE) {
                paint_material = light_material_spoiler;
            } else if (slot_id == CARSLOTID_ROOF) {
                paint_material = light_material_roof;
            } else if (slot_id == CARSLOTID_HOOD && this->CarbonHood != 0 && this->LightMaterial_Carbon != 0) {
                paint_material = this->LightMaterial_Carbon;
            }

            if (paint_material != 0) {
                model->ReplaceLightMaterial(0xD6D6080A, paint_material);
            }
            if (light_material_tint != 0) {
                model->ReplaceLightMaterial(0x471A1DCA, light_material_tint);
            }
        }

        this->RenderPart(view, car_part_model, slot_local_world, light_context, extra_render_flags | disable_env_flag | body_render_flags);
    }

    // Spoiler section
    if (car_body_lod <= pRideInfo->GetMaxSpoilerLodLevel()) {
        bMatrix4 *spoiler_local_world;
        {
            unsigned char *address = CurrentBufferPos;
            if (address + 0x40 >= CurrentBufferEnd) {
                FrameMallocFailed = 1;
                FrameMallocFailAmount += 0x40;
                spoiler_local_world = nullptr;
            } else {
                CurrentBufferPos = address + 0x40;
                spoiler_local_world = reinterpret_cast<bMatrix4 *>(address);
            }
        }

        if (spoiler_local_world) {
            CarPart *part_spoiler = pRideInfo->GetPart(CARSLOTID_SPOILER);
            if (part_spoiler) {
                unsigned int hash = bStringHash("USEMARKER2");
                int use_marker = CarPart_GetAppliedAttributeIParam(part_spoiler, hash, 0);
                if (use_marker != 0 && this->SpoilerPositionMarker2 != nullptr) {
                    this->SpoilerPositionMarker = this->SpoilerPositionMarker2;
                }
            }

            if (this->SpoilerPositionMarker == nullptr || part_spoiler == nullptr ||
                (reinterpret_cast<unsigned char *>(part_spoiler)[5] >> 5) == 0) {
                for (int i = 0; i < 1; i++) {
                    eModel *spoiler_model = this->mCarPartModels[CARSLOTID_SPOILER][i][car_body_lod].GetModel();
                    if (spoiler_model) {
                        spoiler_model->ReplaceLightMaterial(0xD6D6080A, light_material_spoiler);
                        ::Render(view, spoiler_model, local_world, light_context, extra_render_flags | disable_env_flag, 0);
                    }
                }
            } else {
                for (int i = 0; i < 1; i++) {
                    eModel *spoiler_model = this->mCarPartModels[CARSLOTID_SPOILER][i][car_body_lod].GetModel();
                    if (spoiler_model) {
                        eMulMatrix(&spoiler_local_world[i],
                                   reinterpret_cast<bMatrix4 *>(reinterpret_cast<char *>(this->SpoilerPositionMarker) + 0x10),
                                   local_world);
                        spoiler_model->ReplaceLightMaterial(0xD6D6080A, light_material_spoiler);
                        ::Render(view, spoiler_model, &spoiler_local_world[i], light_context, extra_render_flags | disable_env_flag, 0);
                    }
                }
            }
        }
    }

    // Roof scoop section
    if (car_body_lod <= pRideInfo->GetMaxRoofScoopLodLevel()) {
        bMatrix4 *roof_local_world;
        {
            unsigned char *address = CurrentBufferPos;
            if (address + 0x40 >= CurrentBufferEnd) {
                FrameMallocFailed = 1;
                FrameMallocFailAmount += 0x40;
                roof_local_world = nullptr;
            } else {
                CurrentBufferPos = address + 0x40;
                roof_local_world = reinterpret_cast<bMatrix4 *>(address);
            }
        }

        if (roof_local_world) {
            if (this->RoofScoopPositionMarker == nullptr) {
                for (int i = 0; i < 1; i++) {
                    eModel *roof_scoop_model = this->mCarPartModels[CARSLOTID_ROOF][i][car_body_lod].GetModel();
                    if (roof_scoop_model) {
                        roof_scoop_model->ReplaceLightMaterial(0xD6D6080A, light_material_roof);
                        ::Render(view, roof_scoop_model, local_world, light_context,
                                 (extra_render_flags | disable_env_flag) | body_render_flags, 0);
                    }
                }
            } else {
                eMulMatrix(roof_local_world,
                           reinterpret_cast<bMatrix4 *>(reinterpret_cast<char *>(this->RoofScoopPositionMarker) + 0x10),
                           local_world);
                for (int i = 0; i < 1; i++) {
                    eModel *roof_scoop_model = this->mCarPartModels[CARSLOTID_ROOF][i][car_body_lod].GetModel();
                    if (roof_scoop_model) {
                        roof_scoop_model->ReplaceLightMaterial(0xD6D6080A, light_material_roof);
                        ::Render(view, roof_scoop_model, roof_local_world, light_context,
                                 (extra_render_flags | disable_env_flag) | body_render_flags, 0);
                    }
                }
            }
        }
    }

    // Camber computation
    {
        float camber_amount_front = this->GetAttributes().CamberFront();
        float camber_amount_rear = this->GetAttributes().CamberRear();
        wheel_camber_angle_front = bDegToAng(camber_amount_front);
        wheel_camber_angle_rear = bDegToAng(camber_amount_rear);
        wheel_camber_push_down_rear = camber_amount_rear * 0.01f;
        wheel_camber_push_down_front = camber_amount_front * 0.01f;
    }

    wheel_wobble_angle = bDegToAng(0.0f);

    // Wheel scale loop - initialize transformation matrices
    {
        for (int wheel = 0; wheel < 4; wheel++) {
            int wheel_end = wheel < 2 ? 0 : 1;
            float wheel_width_scale = this->WheelWidthScales[wheel];
            float wheel_width = this->WheelWidths[wheel_end];
            float wheel_radius_scale = this->WheelRadiusScales[wheel];
            float yrender_offset = this->WheelYRenderOffset[wheel];
            float pivot_y = this->PivotPosition.y;
            float wheel_brake_marker_y = this->WheelBrakeMarkerY[wheel_end];

            eIdentity(&trans_pivot[wheel]);
            eIdentity(&trans_axle[wheel]);
            eIdentity(&brake_trans_pivot[wheel]);

            trans_pivot[wheel].v0.x = wheel_width_scale;
            trans_pivot[wheel].v1.y = wheel_radius_scale;
            trans_pivot[wheel].v2.z = wheel_radius_scale;
            trans_pivot[wheel].v3.y = yrender_offset - pivot_y;

            trans_axle[wheel].v0.x = wheel_width_scale;
            trans_axle[wheel].v1.y = wheel_radius_scale;
            trans_axle[wheel].v2.z = wheel_radius_scale;
            trans_axle[wheel].v3.y = yrender_offset - pivot_y;
            trans_axle[wheel].v3.x = wheel_width * (wheel_width_scale - 1.0f) * 0.5f;

            brake_trans_pivot[wheel].v3.y = yrender_offset - pivot_y;
            {
                float marker_y = wheel_brake_marker_y;
                brake_trans_pivot[wheel].v3.y += marker_y;
            }
        }
    }

    // Tire section
    if (tire_matrices != 0) {
        CARPART_LOD max_tire_lod = this->pRideInfo->GetMaxTireLodLevel();

        bMatrix4 *tire_local_world;
        float extra_rear_tire_offset;
        bMatrix4 *extra_tire_local_world;

        // Allocate tire_local_world from frame buffer
        {
            unsigned char *address = CurrentBufferPos;
            unsigned int size = sizeof(bMatrix4);
            if (address + size >= CurrentBufferEnd) {
                FrameMallocFailed = 1;
                FrameMallocFailAmount += size;
                tire_local_world = 0;
            } else {
                CurrentBufferPos = address + size;
                tire_local_world = reinterpret_cast<bMatrix4 *>(address);
            }
        }

        extra_rear_tire_offset = this->GetAttributes().ExtraRearTireOffset(0);

        // Allocate extra_tire_local_world from frame buffer
        {
            unsigned char *address = CurrentBufferPos;
            unsigned int size = sizeof(bMatrix4);
            if (address + size >= CurrentBufferEnd) {
                FrameMallocFailed = 1;
                FrameMallocFailAmount += size;
                extra_tire_local_world = 0;
            } else {
                CurrentBufferPos = address + size;
                extra_tire_local_world = reinterpret_cast<bMatrix4 *>(address);
            }
        }

        // Allocate tire_light_context from frame buffer
        {
            unsigned char *address = CurrentBufferPos;
            unsigned int size = sizeof(eDynamicLightContext);
            if (address + size >= CurrentBufferEnd) {
                FrameMallocFailed = 1;
                FrameMallocFailAmount += size;
                tire_light_context = 0;
            } else {
                CurrentBufferPos = address + size;
                tire_light_context = reinterpret_cast<eDynamicLightContext *>(address);
            }
        }

        {
            int tire_lod = car_tire_lod;
            CarPartModel *front_tire_models[1];
            CarPartModel *rear_tire_models[1];
            eLightMaterial *light_material_rim[2];
            bMatrix4 *left_tire_flip;
            unsigned int extra_mirror_flag;
            bMatrix4 wobbleMat;

            tire_visible0 = this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_TIRE_FL);
            tire_visible1 = this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_TIRE_FR);
            tire_visible2 = this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_TIRE_RR);
            tire_visible3 = this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_TIRE_RL);

            if (max_tire_lod > car_tire_lod) {
                tire_lod = max_tire_lod;
            }

            // Check tire models across LOD levels
            {
                for (int i = tire_lod; i >= 0; i--) {
                    front_tire_models[0] = &this->mCarPartModels[CARSLOTID_FRONT_WHEEL][0][i];
                    if (front_tire_models[0]->GetModel() != 0) {
                        break;
                    }
                    if (i == 0) {
                        front_tire_models[0]->GetModel();
                    }
                }
                for (int i = tire_lod; i >= 0; i--) {
                    rear_tire_models[0] = &this->mCarPartModels[CARSLOTID_REAR_WHEEL][0][i];
                    if (rear_tire_models[0]->GetModel() != 0) {
                        break;
                    }
                    if (i == 0) {
                        rear_tire_models[0]->GetModel();
                    }
                }
            }

            light_material_rim[0] = this->LightMaterial_WheelRim;
            light_material_rim[1] = this->LightMaterial_Spinner;

            if (this->mMirrorLeftWheels) {
                left_tire_flip = &LeftTireMirrorMatrix;
            } else {
                left_tire_flip = &LeftTireRotateZMatrix;
            }

            extra_mirror_flag = extra_render_flags | disable_env_flag_tires;

            wobbleMat = bMatrix4();
            bIdentity(&wobbleMat);

            // Tire 0 (front-left)
            {
                bMatrix4 *starting_tire_matrix = &tire_matrices[0];
                bMatrix4 tire_matrix_for_camber;
                bMatrix4 tire0;

                {
                    bVector3 wheel_offset;
                    bCopy(&tire_matrix_for_camber, starting_tire_matrix);
                    wheel_offset = bVector3();
                    bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&tire_matrix_for_camber.v3));
                    bFill(&tire_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                    bCopy(reinterpret_cast<bVector3 *>(&tire0.v3), &wheel_offset);
                }

                eRotateX(&tire_matrix_for_camber, &tire_matrix_for_camber, wheel_camber_angle_front);
                eMulMatrix(&tire0, &tire_matrix_for_camber, &trans_pivot[0]);
                tire0.v3.y += wheel_camber_push_down_front;
                eMulMatrix(&tire0, left_tire_flip, &tire0);
                eMulMatrix(&tire0, &wobbleMat, &tire0);
                PSMTX44Copy(*reinterpret_cast<Mtx44 *>(&tire0), *reinterpret_cast<Mtx44 *>(tire_local_world));
                eMulMatrix(tire_local_world, tire_local_world, local_world);

                if (tire_visible0) {
                    eModel *tire_model = front_tire_models[0]->GetModel();
                    if (tire_model != 0) {
                        eReplacementTextureTable *rep_table = this->BrakeLeftReplacementTextureTable;
                        tire_model->AttachReplacementTextureTable(rep_table, 2, 0);
                        if (light_material_rim[0] != 0) {
                            tire_model->ReplaceLightMaterial(0xD6640DFF, light_material_rim[0]);
                        }
                        if (light_material_rim[1] != 0) {
                            tire_model->ReplaceLightMaterial(0xA3186E2B, light_material_rim[1]);
                        }
                        float face = TireFace(tire_local_world, view);
                        ::Render(view, tire_model, tire_local_world, light_context, extra_mirror_flag, 0);
                    }
                }
            }

            // Tire 1 (front-right)
            {
                bMatrix4 *starting_tire_matrix = &tire_matrices[1];
                bMatrix4 tire_matrix_for_camber;
                bMatrix4 tire1;

                {
                    bVector3 wheel_offset;
                    bCopy(&tire_matrix_for_camber, starting_tire_matrix);
                    wheel_offset = bVector3();
                    bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&tire_matrix_for_camber.v3));
                    bFill(&tire_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                    bCopy(reinterpret_cast<bVector3 *>(&tire1.v3), &wheel_offset);
                }

                eRotateX(&tire_matrix_for_camber, &tire_matrix_for_camber, wheel_camber_angle_front);
                eMulMatrix(&tire1, &tire_matrix_for_camber, &trans_pivot[1]);
                tire1.v3.y += wheel_camber_push_down_front;
                tire1 = tire1;
                eMulMatrix(&tire1, &wobbleMat, &tire1);
                PSMTX44Copy(*reinterpret_cast<Mtx44 *>(&tire1), *reinterpret_cast<Mtx44 *>(tire_local_world));
                eMulMatrix(tire_local_world, tire_local_world, local_world);

                if (tire_visible1) {
                    eModel *tire_model = front_tire_models[0]->GetModel();
                    if (tire_model != 0) {
                        eReplacementTextureTable *rep_table = this->BrakeRightReplacementTextureTable;
                        tire_model->AttachReplacementTextureTable(rep_table, 2, 0);
                        if (light_material_rim[0] != 0) {
                            tire_model->ReplaceLightMaterial(0xD6640DFF, light_material_rim[0]);
                        }
                        if (light_material_rim[1] != 0) {
                            tire_model->ReplaceLightMaterial(0xA3186E2B, light_material_rim[1]);
                        }
                        float face = TireFace(tire_local_world, view);
                        ::Render(view, tire_model, tire_local_world, light_context, extra_mirror_flag, 0);
                    }
                }
            }

            // Tire 2 (rear-right)
            {
                bMatrix4 *starting_tire_matrix = &tire_matrices[2];
                bMatrix4 tire_matrix_for_camber;
                bMatrix4 tire2;

                {
                    bVector3 wheel_offset;
                    bCopy(&tire_matrix_for_camber, starting_tire_matrix);
                    wheel_offset = bVector3();
                    bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&tire_matrix_for_camber.v3));
                    bFill(&tire_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                    bCopy(reinterpret_cast<bVector3 *>(&tire2.v3), &wheel_offset);
                }

                eRotateX(&tire_matrix_for_camber, &tire_matrix_for_camber, wheel_camber_angle_rear);
                eMulMatrix(&tire2, &tire_matrix_for_camber, &trans_pivot[2]);
                tire2.v3.y += wheel_camber_push_down_rear;
                tire2 = tire2;
                eMulMatrix(&tire2, &wobbleMat, &tire2);
                PSMTX44Copy(*reinterpret_cast<Mtx44 *>(&tire2), *reinterpret_cast<Mtx44 *>(tire_local_world));
                eMulMatrix(tire_local_world, tire_local_world, local_world);

                if (tire_visible2) {
                    if (IsGameFlowInGame()) {
                        eModel *tire_model = rear_tire_models[0]->GetModel();
                        if (tire_model != 0) {
                            eReplacementTextureTable *rep_table = this->BrakeRightReplacementTextureTable;
                            tire_model->AttachReplacementTextureTable(rep_table, 2, 0);
                            if (light_material_rim[0] != 0) {
                                tire_model->ReplaceLightMaterial(0xD6640DFF, light_material_rim[0]);
                            }
                            if (light_material_rim[1] != 0) {
                                tire_model->ReplaceLightMaterial(0xA3186E2B, light_material_rim[1]);
                            }
                            float face = TireFace(tire_local_world, view);
                            ::Render(view, tire_model, tire_local_world, light_context, extra_mirror_flag, 0);
                        }
                    } else {
                        CarPartModel *tmpM = rear_tire_models[0];
                        this->RenderPart(view, tmpM, tire_local_world, light_context, extra_mirror_flag);
                    }
                }
            }

            // Extra rear tire offset intermediate
            {
                for (int i = 3; i >= 0; i--) {
                }
            }

            // Tire 3 (rear-left)
            {
                bMatrix4 *starting_tire_matrix = &tire_matrices[3];
                bMatrix4 tire_matrix_for_camber;
                bMatrix4 tire3;

                {
                    bVector3 wheel_offset;
                    bCopy(&tire_matrix_for_camber, starting_tire_matrix);
                    wheel_offset = bVector3();
                    bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&tire_matrix_for_camber.v3));
                    bFill(&tire_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                    bCopy(reinterpret_cast<bVector3 *>(&tire3.v3), &wheel_offset);
                }

                eRotateX(&tire_matrix_for_camber, &tire_matrix_for_camber, wheel_camber_angle_rear);
                eMulMatrix(&tire3, &tire_matrix_for_camber, &trans_pivot[3]);
                tire3.v3.y += wheel_camber_push_down_rear;
                eMulMatrix(&tire3, left_tire_flip, &tire3);
                eMulMatrix(&tire3, &wobbleMat, &tire3);
                PSMTX44Copy(*reinterpret_cast<Mtx44 *>(&tire3), *reinterpret_cast<Mtx44 *>(tire_local_world));
                eMulMatrix(tire_local_world, tire_local_world, local_world);

                if (tire_visible3) {
                    if (IsGameFlowInGame()) {
                        eModel *tire_model = rear_tire_models[0]->GetModel();
                        if (tire_model != 0) {
                            eReplacementTextureTable *rep_table = this->BrakeLeftReplacementTextureTable;
                            tire_model->AttachReplacementTextureTable(rep_table, 2, 0);
                            if (light_material_rim[0] != 0) {
                                tire_model->ReplaceLightMaterial(0xD6640DFF, light_material_rim[0]);
                            }
                            if (light_material_rim[1] != 0) {
                                tire_model->ReplaceLightMaterial(0xA3186E2B, light_material_rim[1]);
                            }
                            float face = TireFace(tire_local_world, view);
                            ::Render(view, tire_model, tire_local_world, light_context, extra_mirror_flag, 0);
                        }
                    } else {
                        CarPartModel *tmpM = rear_tire_models[0];
                        this->RenderPart(view, tmpM, tire_local_world, light_context, extra_mirror_flag);
                    }
                }
            }
        }
    }

    // Brake section
    if (brake_matrices != 0) {
        CARPART_LOD max_brake_lod = this->pRideInfo->GetMaxBrakeLodLevel();

        eModel *front_brake_check = this->mCarPartModels[CARSLOTID_FRONT_BRAKE][0][car_body_lod].GetModel();

        int brakes_visible_front_left = this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_BRAKE_FL);
        int brakes_visible_front_right = this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_BRAKE_FR);
        int brakes_visible_rear_right = this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_BRAKE_RR);
        int brakes_visible_rear_left = this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_BRAKE_RL);

        if (car_body_lod <= max_brake_lod && front_brake_check != 0) {
            bMatrix4 *brake_local_world;
            eDynamicLightContext *brake_light_context;

            {
                unsigned char *address = CurrentBufferPos;
                unsigned int size = sizeof(bMatrix4) * 4;
                if (address + size >= CurrentBufferEnd) {
                    FrameMallocFailed = 1;
                    FrameMallocFailAmount += size;
                    brake_local_world = 0;
                } else {
                    CurrentBufferPos = address + size;
                    brake_local_world = reinterpret_cast<bMatrix4 *>(address);
                }
            }

            {
                unsigned char *address = CurrentBufferPos;
                unsigned int size = sizeof(eDynamicLightContext) * 4;
                if (address + size >= CurrentBufferEnd) {
                    FrameMallocFailed = 1;
                    FrameMallocFailAmount += size;
                    brake_light_context = 0;
                } else {
                    CurrentBufferPos = address + size;
                    brake_light_context = reinterpret_cast<eDynamicLightContext *>(address);
                }
            }

            if (brake_local_world != 0 && brake_light_context != 0) {
                int brakes_lod = car_body_lod;
                eModel *front_brake_models[1];
                eModel *rear_brake_models[1];
                eLightMaterial *light_material_caliper = this->LightMaterial_Caliper;
                bMatrix4 mirror;

                {
                    for (int i = brakes_lod; i >= 0; i--) {
                        front_brake_models[0] = this->mCarPartModels[CARSLOTID_FRONT_BRAKE][0][i].GetModel();
                        rear_brake_models[0] = this->mCarPartModels[CARSLOTID_REAR_BRAKE][0][i].GetModel();
                        if (front_brake_models[0] != 0 || rear_brake_models[0] != 0) {
                            break;
                        }
                    }
                }

                if (light_material_caliper != 0) {
                    if (front_brake_models[0] != 0) {
                        front_brake_models[0]->ReplaceLightMaterial(0xD6640DFF, light_material_caliper);
                        front_brake_models[0]->ReplaceLightMaterial(0xA3186E2B, light_material_caliper);
                    }
                    if (rear_brake_models[0] != 0) {
                        rear_brake_models[0]->ReplaceLightMaterial(0xD6640DFF, light_material_caliper);
                        rear_brake_models[0]->ReplaceLightMaterial(0xA3186E2B, light_material_caliper);
                    }
                }

                mirror = bMatrix4();
                eIdentity(&mirror);
                mirror.v0.x = -1.0f;

                if (brakes_visible_front_left) {
                    bMatrix4 *starting_brake_matrix = &brake_matrices[0];
                    bMatrix4 brake_matrix_for_camber;
                    bMatrix4 bm0;

                    {
                        bVector3 wheel_offset;
                        bCopy(&brake_matrix_for_camber, starting_brake_matrix);
                        wheel_offset = bVector3();
                        bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&brake_matrix_for_camber.v3));
                        bFill(&brake_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                        bCopy(reinterpret_cast<bVector3 *>(&bm0.v3), &wheel_offset);
                    }

                    eRotateX(&brake_matrix_for_camber, &brake_matrix_for_camber, wheel_camber_angle_front);
                    eMulMatrix(&bm0, &brake_trans_pivot[0], &mirror);
                    eMulMatrix(&bm0, &bm0, &brake_matrix_for_camber);
                    eMulMatrix(&bm0, &bm0, &trans_axle[0]);
                    eMulMatrix(&brake_local_world[0], &bm0, local_world);

                    if (IsGameFlowInGame()) {
                        for (int i = 0; i < 1; i++) {
                            if (front_brake_models[i] != 0) {
                                front_brake_models[i]->AttachReplacementTextureTable(this->BrakeLeftReplacementTextureTable, 2, 0);
                                ::Render(view, front_brake_models[i], &brake_local_world[0], light_context,
                                         extra_render_flags | disable_env_flag_tires | 0x40000, 0);
                            }
                        }
                    } else {
                        for (int i = 0; i < 1; i++) {
                            if (front_brake_models[i] != 0) {
                                front_brake_models[i]->AttachReplacementTextureTable(this->BrakeLeftReplacementTextureTable, 2, 0);
                                elCloneLightContext(&brake_light_context[0], &brake_local_world[0], &hack_man_matrix, &camera_world_position, view,
                                                    &base_light_context);
                                ::Render(view, front_brake_models[i], &brake_local_world[0], &brake_light_context[0],
                                         extra_render_flags | disable_env_flag_tires | 0x40000, 0);
                            }
                        }
                    }
                }

                if (brakes_visible_front_right) {
                    bMatrix4 *starting_brake_matrix = &brake_matrices[1];
                    bMatrix4 brake_matrix_for_camber;
                    bMatrix4 bm1;

                    {
                        bVector3 wheel_offset;
                        bCopy(&brake_matrix_for_camber, starting_brake_matrix);
                        wheel_offset = bVector3();
                        bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&brake_matrix_for_camber.v3));
                        bFill(&brake_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                        bCopy(reinterpret_cast<bVector3 *>(&bm1.v3), &wheel_offset);
                    }

                    eRotateX(&brake_matrix_for_camber, &brake_matrix_for_camber, -wheel_camber_angle_front);
                    eMulMatrix(&bm1, &brake_trans_pivot[1], &brake_matrix_for_camber);
                    eMulMatrix(&bm1, &bm1, &trans_axle[1]);
                    eMulMatrix(&brake_local_world[1], &bm1, local_world);

                    if (IsGameFlowInGame()) {
                        for (int i = 0; i < 1; i++) {
                            if (front_brake_models[i] != 0) {
                                front_brake_models[i]->AttachReplacementTextureTable(this->BrakeRightReplacementTextureTable, 2, 0);
                                ::Render(view, front_brake_models[i], &brake_local_world[1], light_context, extra_render_flags | disable_env_flag_tires,
                                         0);
                            }
                        }
                    } else {
                        for (int i = 0; i < 1; i++) {
                            if (front_brake_models[i] != 0) {
                                front_brake_models[i]->AttachReplacementTextureTable(this->BrakeRightReplacementTextureTable, 2, 0);
                                elCloneLightContext(&brake_light_context[1], &brake_local_world[1], &hack_man_matrix, &camera_world_position, view,
                                                    &base_light_context);
                                ::Render(view, front_brake_models[i], &brake_local_world[1], &brake_light_context[1],
                                         extra_render_flags | disable_env_flag_tires, 0);
                            }
                        }
                    }
                }

                if (brakes_visible_rear_right) {
                    bMatrix4 *starting_brake_matrix = &brake_matrices[2];
                    bMatrix4 brake_matrix_for_camber;
                    bMatrix4 bm2;

                    {
                        bVector3 wheel_offset;
                        bCopy(&brake_matrix_for_camber, starting_brake_matrix);
                        wheel_offset = bVector3();
                        bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&brake_matrix_for_camber.v3));
                        bFill(&brake_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                        bCopy(reinterpret_cast<bVector3 *>(&bm2.v3), &wheel_offset);
                    }

                    eRotateX(&brake_matrix_for_camber, &brake_matrix_for_camber, -wheel_camber_angle_rear);
                    eMulMatrix(&bm2, &brake_trans_pivot[2], &brake_matrix_for_camber);
                    eMulMatrix(&bm2, &bm2, &trans_axle[2]);
                    eMulMatrix(&brake_local_world[2], &bm2, local_world);

                    if (IsGameFlowInGame()) {
                        for (int i = 0; i < 1; i++) {
                            if (rear_brake_models[i] != 0) {
                                rear_brake_models[i]->AttachReplacementTextureTable(this->BrakeRightReplacementTextureTable, 2, 0);
                                ::Render(view, rear_brake_models[i], &brake_local_world[2], light_context, extra_render_flags | disable_env_flag_tires,
                                         0);
                            }
                        }
                    } else {
                        for (int i = 0; i < 1; i++) {
                            if (rear_brake_models[i] != 0) {
                                rear_brake_models[i]->AttachReplacementTextureTable(this->BrakeRightReplacementTextureTable, 2, 0);
                                elCloneLightContext(&brake_light_context[2], &brake_local_world[2], &hack_man_matrix, &camera_world_position, view,
                                                    &base_light_context);
                                ::Render(view, rear_brake_models[i], &brake_local_world[2], &brake_light_context[2],
                                         extra_render_flags | disable_env_flag_tires, 0);
                            }
                        }
                    }
                }

                if (brakes_visible_rear_left) {
                    bMatrix4 *starting_brake_matrix = &brake_matrices[3];
                    bMatrix4 brake_matrix_for_camber;
                    bMatrix4 bm3;

                    {
                        bVector3 wheel_offset;
                        bCopy(&brake_matrix_for_camber, starting_brake_matrix);
                        wheel_offset = bVector3();
                        bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&brake_matrix_for_camber.v3));
                        bFill(&brake_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                        bCopy(reinterpret_cast<bVector3 *>(&bm3.v3), &wheel_offset);
                    }

                    eRotateX(&brake_matrix_for_camber, &brake_matrix_for_camber, wheel_camber_angle_rear);
                    eMulMatrix(&bm3, &brake_trans_pivot[3], &mirror);
                    eMulMatrix(&bm3, &bm3, &brake_matrix_for_camber);
                    eMulMatrix(&bm3, &bm3, &trans_axle[3]);
                    eMulMatrix(&brake_local_world[3], &bm3, local_world);

                    if (IsGameFlowInGame()) {
                        for (int i = 0; i < 1; i++) {
                            if (rear_brake_models[i] != 0) {
                                rear_brake_models[i]->AttachReplacementTextureTable(this->BrakeLeftReplacementTextureTable, 2, 0);
                                ::Render(view, rear_brake_models[i], &brake_local_world[3], light_context,
                                         extra_render_flags | disable_env_flag_tires | 0x40000, 0);
                            }
                        }
                    } else {
                        for (int i = 0; i < 1; i++) {
                            if (rear_brake_models[i] != 0) {
                                rear_brake_models[i]->AttachReplacementTextureTable(this->BrakeLeftReplacementTextureTable, 2, 0);
                                elCloneLightContext(&brake_light_context[3], &brake_local_world[3], &hack_man_matrix, &camera_world_position, view,
                                                    &base_light_context);
                                ::Render(view, rear_brake_models[i], &brake_local_world[3], &brake_light_context[3],
                                         extra_render_flags | disable_env_flag_tires | 0x40000, 0);
                            }
                        }
                    }
                }
            }
        }
    }

    if (tire_matrices != 0 && !this->mEmitterPositionsInitialized) {
        bVector4 tire_positions[4];

        for (int wheel = 0; wheel < 4; wheel++) {
            tire_positions[wheel] = tire_matrices[wheel].v3;
        }

        this->InitEmitterPositions(tire_positions);
    }

    this->RenderFlaresOnCar(view, &position, body_matrix, force_light_state, reflexion, static_cast<int>(extra_render_flags));

    return true;
}

void CarRenderInfo::RenderPart(eView *view, CarPartModel *carPart, bMatrix4 *local_to_world, eDynamicLightContext *light_context,
                               unsigned int flags) {
    if (carPart != nullptr) {
        if (carPart->IsLodMissing()) {
            view->Render(&StandardDebugModel, local_to_world, light_context, flags, nullptr);
        } else {
            view->Render(carPart->GetModel(), local_to_world, light_context, flags, nullptr);
        }
    }
}

int CarRenderInfo::GetEmitterPositions(bSList<CarEmitterPosition> &markers_out, const unsigned int *position_name_hashes,
                                       int num_pos_name_hashes) {
    int count;

    if (this->pCarTypeInfo == nullptr) {
        return 0;
    }

    count = 0;

    for (int slot_model_index = 0; slot_model_index < 0x4C; slot_model_index++) {
        eModel *model = this->mCarPartModels[slot_model_index][0][this->mMinLodLevel].GetModel();
        ePositionMarker *position_marker = nullptr;

        if (model == nullptr) {
            continue;
        }

        while ((position_marker = model->GetPostionMarker(position_marker)) != nullptr) {
            unsigned int position_marker_namehash;

            for (int i = 0; i < num_pos_name_hashes; i++) {
                if (position_marker->NameHash == position_name_hashes[i]) {
                    CarEmitterPosition *empos;

                    empos = new CarEmitterPosition(position_marker);
                    markers_out.AddTail(empos);
                    count++;
                }
            }
        }
    }

    return count;
}

void CarRenderInfo::InitEmitterPositions(bVector4 *tire_positions) {
    if (this->pCarTypeInfo != nullptr && !this->mEmitterPositionsInitialized) {
        int fxpos;

        for (fxpos = 0; fxpos < NUM_CARFXPOS; fxpos++) {
            int pos_count = 0;
            bool is_based_off_position_marker;

            is_based_off_position_marker = GetNumCarEffectMarkerHashes(static_cast<CarEffectPosition>(fxpos), pos_count);

            if (is_based_off_position_marker) {
                if (pos_count > 0) {
                    this->GetEmitterPositions(this->EmitterPositionList[fxpos],
                                              GetCarEffectMarkerHashes(static_cast<CarEffectPosition>(fxpos)),
                                              pos_count);
                }
                continue;
            }

            {
                CarEffectPosition efxpos = static_cast<CarEffectPosition>(fxpos);
                bSList<CarEmitterPosition> &pos_list = this->EmitterPositionList[fxpos];
                CarEmitterPosition *empos = nullptr;
                switch (efxpos) {
                    case CARFXPOS_NONE:
                        empos = new CarEmitterPosition(0.0f, 0.0f, 0.0f);
                        pos_list.AddTail(empos);
                        break;
                    case CARFXPOS_FRONT_TIRES:
                        {
                            bVector4 *fl = tire_positions;
                            bVector4 *fr = tire_positions + 1;
                            bVector4 avg = *fl + *fr;

                            avg *= 0.5f;
                            empos = new CarEmitterPosition(avg.x, avg.y, avg.z);
                            pos_list.AddTail(empos);
                        }
                        break;
                    case CARFXPOS_REAR_TIRES:
                        {
                            bVector4 *rr = tire_positions + 2;
                            bVector4 *rl = tire_positions + 3;
                            bVector4 avg = *rr + *rl;

                            avg *= 0.5f;
                            empos = new CarEmitterPosition(avg.x, avg.y, avg.z);
                            pos_list.AddTail(empos);
                        }
                        break;
                    case CARFXPOS_LEFT_TIRES:
                        {
                            bVector4 *fl = tire_positions;
                            bVector4 *rl = tire_positions + 3;
                            bVector4 avg = *fl + *rl;

                            avg *= 0.5f;
                            empos = new CarEmitterPosition(avg.x, avg.y, avg.z);
                            pos_list.AddTail(empos);
                        }
                        break;
                    case CARFXPOS_RIGHT_TIRES:
                        {
                            bVector4 *fr = tire_positions + 1;
                            bVector4 *rr = tire_positions + 2;
                            bVector4 avg = *fr + *rr;

                            avg *= 0.5f;
                            empos = new CarEmitterPosition(avg.x, avg.y, avg.z);
                            pos_list.AddTail(empos);
                        }
                        break;
                    case CARFXPOS_TIRE_FL:
                        empos = new CarEmitterPosition(tire_positions->x, tire_positions->y, tire_positions->z);
                        pos_list.AddTail(empos);
                        break;
                    case CARFXPOS_TIRE_FR:
                        empos = new CarEmitterPosition(tire_positions[1].x, tire_positions[1].y, tire_positions[1].z);
                        pos_list.AddTail(empos);
                        break;
                    case CARFXPOS_TIRE_RR:
                        empos = new CarEmitterPosition(tire_positions[2].x, tire_positions[2].y, tire_positions[2].z);
                        pos_list.AddTail(empos);
                        break;
                    case CARFXPOS_TIRE_RL:
                        empos = new CarEmitterPosition(tire_positions[3].x, tire_positions[3].y, tire_positions[3].z);
                        pos_list.AddTail(empos);
                        break;
                    case CARFXPOS_ENGINE:
                        {
                            bVector4 *fl = tire_positions;
                            bVector4 *fr = tire_positions + 1;
                            bVector4 avg = *fl + *fr;

                            avg *= 0.5f;
                            bVector4 diff;
                            bSub(&diff, fl, fr);
                            empos = new CarEmitterPosition(avg.x, avg.y, avg.z + diff.y * 0.2f);
                            pos_list.AddTail(empos);
                        }
                        break;
                }
            }
        }

        this->mEmitterPositionsInitialized = true;
    }
}

int cmpl(const void *a, const void *b) {
    const float *pa = *reinterpret_cast<const float *const *>(a);
    const float *pb = *reinterpret_cast<const float *const *>(b);
    float delta = pa[0] - pb[0];

    if (0.0f < delta) {
        return 1;
    }
    if (delta < 0.0f) {
        return -1;
    }

    delta = pb[1] - pa[1];
    if (0.0f < delta) {
        return 1;
    }
    if (delta < 0.0f) {
        return -1;
    }

    return 0;
}

int cmph(const void *a, const void *b) {
    return cmpl(b, a);
}

void bRotateVector(bVector3 *dest, const bMatrix4 *m, bVector3 *v) {
    dest->x = m->v0.x * v->x + m->v1.x * v->y + m->v2.x * v->z;
    dest->y = m->v0.y * v->x + m->v1.y * v->y + m->v2.y * v->z;
    dest->z = m->v0.z * v->x + m->v1.z * v->y + m->v2.z * v->z;
}

float coplightflicker(float time, int offset) {
    int counter = counter_31665 + 1;
    counter_31665 = counter % copModulo;

    return bCos((time + copt * static_cast<float>(offset)) * copm);
}

float coplightflicker2(float time, int whichColor, int flarecount) {
    int counter;
    float offset;
    float a;
    float t;

    counter = counter_31669 + 1;
    counter_31669 = counter % copModulo;

    switch (whichColor) {
    case 0:
        offset = copoffsetr;
        break;
    case 1:
        offset = copoffsetb;
        break;
    case 2:
        offset = copoffsetw;
        break;
    }

    t = bCos(time * 24.0f);
    t *= t;

    if (whichColor == 2) {
        a = t * coplightflicker(time, flarecount);
        return a;
    }

    float c = bCos(time * 8.0f + offset);
    if (c > 0.2f) {
        a = t;
        return a;
    }

    a = 0.0f;
    return a;
}

float TireFace(bMatrix4 *matrix, eView *view) {
    float face;

    if (TireFaceIt != 0) {
        bVector3 cDir(*view->GetCamera()->GetDirection());
        bMatrix4 matrix2(*matrix);
        bVector3 N(enX, enY, enZ);

        eMulVector(&N, &matrix2, &N);
        face = bDot(&N, &cDir);
    } else {
        face = 1.0f;
    }

    return face;
}

void CarRenderInfo::UpdateCarReplacementTextures() {
    bMemCpy(this->CarbonReplacementTextureTable, this->MasterReplacementTextureTable, sizeof(this->CarbonReplacementTextureTable));

    this->CarbonReplacementTextureTable[REPLACETEX_CARSKIN].SetNewNameHash(bStringHash("CARBONFIBRE"));
    this->CarbonReplacementTextureTable[REPLACETEX_CARSKINB].SetNewNameHash(
        *reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(this) + 0x15B0));
    this->CarbonReplacementTextureTable[REPLACETEX_GLOBALSKIN].SetNewNameHash(
        *reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(this) + 0x15B0));
    this->CarbonReplacementTextureTable[REPLACETEX_CARBONSKIN].SetNewNameHash(bStringHash("CARBONFIBRE"));
    this->CarbonReplacementTextureTable[REPLACETEX_GLOBALCARBONSKIN].SetNewNameHash(bStringHash("CARBONFIBRE"));
}

void CarRenderInfo::SwitchSkin(RideInfo *ride_info) {
    this->pRideInfo = ride_info;
    GetUsedCarTextureInfo(&this->mUsedTextureInfos, ride_info, 0);

    this->MasterReplacementTextureTable[REPLACETEX_CARSKIN].SetNewNameHash(this->mUsedTextureInfos.ReplaceSkinHash);
    this->MasterReplacementTextureTable[REPLACETEX_CARSKINB].SetNewNameHash(this->mUsedTextureInfos.ReplaceSkinBHash);
    this->MasterReplacementTextureTable[REPLACETEX_WHEEL].SetNewNameHash(this->mUsedTextureInfos.ReplaceWheelHash);
    this->MasterReplacementTextureTable[REPLACETEX_SPINNER].SetNewNameHash(this->mUsedTextureInfos.ReplaceSpinnerHash);
    this->MasterReplacementTextureTable[REPLACETEX_SPOILER].SetNewNameHash(this->mUsedTextureInfos.ReplaceSpoilerHash);
    this->MasterReplacementTextureTable[REPLACETEX_ROOF_SCOOP].SetNewNameHash(this->mUsedTextureInfos.ReplaceRoofScoopHash);
    this->MasterReplacementTextureTable[REPLACETEX_GLOBALSKIN].SetNewNameHash(this->mUsedTextureInfos.ReplaceGlobalHash);
    this->MasterReplacementTextureTable[REPLACETEX_CARBONSKIN].SetNewNameHash(this->mUsedTextureInfos.ReplaceGlobalHash);
    this->MasterReplacementTextureTable[REPLACETEX_GLOBALCARBONSKIN].SetNewNameHash(this->mUsedTextureInfos.ReplaceGlobalHash);

    bMemCpy(this->CarbonReplacementTextureTable, this->MasterReplacementTextureTable, sizeof(this->CarbonReplacementTextureTable));

    this->CarbonReplacementTextureTable[REPLACETEX_CARSKIN].SetNewNameHash(bStringHash("CARBONFIBRE"));
    this->CarbonReplacementTextureTable[REPLACETEX_CARSKINB].SetNewNameHash(this->mUsedTextureInfos.ReplaceGlobalHash);
    this->CarbonReplacementTextureTable[REPLACETEX_GLOBALSKIN].SetNewNameHash(this->mUsedTextureInfos.ReplaceGlobalHash);
    this->CarbonReplacementTextureTable[REPLACETEX_CARBONSKIN].SetNewNameHash(bStringHash("CARBONFIBRE"));
    this->CarbonReplacementTextureTable[REPLACETEX_GLOBALCARBONSKIN].SetNewNameHash(bStringHash("CARBONFIBRE"));

    this->BrakeLeftReplacementTextureTable[1].SetNewNameHash(this->mUsedTextureInfos.ReplaceGlobalHash);
    this->BrakeRightReplacementTextureTable[1].SetNewNameHash(this->mUsedTextureInfos.ReplaceGlobalHash);
}

void CarRenderInfo::CreateCarLightFlares() {
    if (this->pCarTypeInfo != 0) {
        int front_marker_slot = -1;
        int rear_marker_slot = -1;

        for (int slot_model_index = 0x4B; slot_model_index >= 0; slot_model_index--) {
            eModel *model = this->mCarPartModels[slot_model_index][0][this->mMinLodLevel].GetModel();
            ePositionMarker *position_marker = 0;

            if (model == 0) {
                continue;
            }

            while ((position_marker = model->GetPostionMarker(position_marker)) != 0) {
                unsigned int name_hash = position_marker->NameHash;
                int flare_type;

                switch (name_hash) {
                    case 0xD09091C6:
                    case 0x9DB90133:
                    case 0x7A5BCF69:
                        flare_type = 0;
                        break;
                    case 0x31A66786:
                    case 0xA2A2FC7C:
                    case 0xBF700A79:
                        flare_type = 1;
                        break;
                    case 0x1E4150B4:
                        flare_type = 5;
                        break;
                    case 0xE662C161:
                        flare_type = 6;
                        break;
                    case 0xB4348DBA:
                        flare_type = 7;
                        break;
                    case 0x41489594:
                        flare_type = 10;
                        break;
                    case 0x6A52A241:
                        flare_type = 11;
                        break;
                    case 0x28CD78F5:
                        flare_type = 12;
                        break;
                    case 0x7A5B2F25:
                        if (front_marker_slot != slot_model_index && front_marker_slot > 0) {
                            continue;
                        }
                        front_marker_slot = slot_model_index;
                        flare_type = 3;
                        break;
                    case 0x7ADF7EF8:
                        if (rear_marker_slot != slot_model_index && rear_marker_slot > 0) {
                            continue;
                        }
                        rear_marker_slot = slot_model_index;
                        flare_type = 3;
                        break;
                    default:
                        flare_type = -1;
                        break;
                }

                if (flare_type != -1) {
                    eLightFlare *light_flare = static_cast<eLightFlare *>(gFastMem.Alloc(sizeof(eLightFlare), 0));

                    light_flare->NameHash = name_hash;
                    light_flare->Type = static_cast<char>(flare_type);
                    if ((flare_type - 5U < 3) || flare_type == 10 || flare_type == 11 || flare_type == 12) {
                        light_flare->Flags = 2;
                    } else {
                        light_flare->Flags = 4;
                    }
                    light_flare->PositionX = position_marker->Matrix.v3.x;
                    light_flare->PositionY = position_marker->Matrix.v3.y;
                    light_flare->PositionZ = position_marker->Matrix.v3.z;
                    light_flare->ReflectPosZ = 0.0f;
                    light_flare->DirectionX = position_marker->Matrix.v2.x;
                    light_flare->DirectionY = position_marker->Matrix.v2.y;
                    light_flare->DirectionZ = position_marker->Matrix.v2.z;
                    this->LightFlareList.AddTail(light_flare);
                }
            }
        }
    }
}

void CarRenderInfo::RenderTextureHeadlights(eView *view, bMatrix4 *l_w, unsigned int) {
    bMatrix4 *matrix = reinterpret_cast<bMatrix4 *>(CurrentBufferPos);

    if (CurrentBufferEnd <= CurrentBufferPos + sizeof(bMatrix4)) {
        FrameMallocFailed = 1;
        FrameMallocFailAmount += sizeof(bMatrix4);
        matrix = 0;
    } else {
        CurrentBufferPos += sizeof(bMatrix4);
        *matrix = *l_w;
    }

    if (matrix != 0) {
        bVector3 headlight_direction(0.0f, 0.0f, 1.0f);

        if (bDot(reinterpret_cast<const bVector3 *>(&matrix->v2), &headlight_direction) < 0.707f) {
            return;
        }

        ePoly poly;
        __5ePoly(&poly);
        TextureInfo *texture_info = GetTextureInfo(bStringHash("2PLAYERHEADLIGHT1"), 1, 0);

        poly.Vertices[0].x = hOffX - hRad0x;
        poly.Vertices[0].y = hOffY - hRad0y;
        poly.Vertices[1].x = hRad1x + hOffX;
        poly.Vertices[1].y = hOffY - hRad1y;
        poly.Vertices[2].x = hRad2x + hOffX;
        poly.Vertices[2].y = hRad2y + hOffY;
        poly.Vertices[3].x = hOffX - hRad3x;
        poly.Vertices[3].y = hRad3y + hOffY;

        poly.UVs[0][0] = 0.0f;
        poly.UVs[1][0] = 0.0f;
        poly.UVs[0][1] = 1.0f;
        poly.UVs[1][1] = 0.0f;
        poly.UVs[0][2] = 1.0f;
        poly.UVs[1][2] = 1.0f;
        poly.UVs[0][3] = 0.0f;
        poly.UVs[1][3] = 1.0f;

        reinterpret_cast<unsigned int *>(poly.Colours)[0] = hcL;
        reinterpret_cast<unsigned int *>(poly.Colours)[1] = hcL;
        reinterpret_cast<unsigned int *>(poly.Colours)[2] = hcL;
        reinterpret_cast<unsigned int *>(poly.Colours)[3] = hcL;

        ::Render(view, &poly, texture_info, matrix, 0, 0.0f);
    }
}

void CarRenderInfo::RenderFlaresOnCar(eView *view, const bVector3 *position, const bMatrix4 *body_matrix, int force_light_state, int reflexion,
                                      int renderFlareFlags) {
    ProfileNode profile_node("RenderFlaresOnCar", 0);
    float Ftime = Sim::GetTime() + this->CarTimebaseStart;
    bMatrix4 *local_world = reinterpret_cast<bMatrix4 *>(CurrentBufferPos);

    if (CurrentBufferEnd <= CurrentBufferPos + sizeof(bMatrix4)) {
        FrameMallocFailed = 1;
        FrameMallocFailAmount += sizeof(bMatrix4);
        local_world = 0;
    } else {
        CurrentBufferPos += sizeof(bMatrix4);
    }

    if (local_world == 0) {
        return;
    }

    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(body_matrix), *reinterpret_cast<Mtx44 *>(local_world));
    local_world->v3.x = position->x;
    local_world->v3.y = position->y;
    local_world->v3.z = position->z;
    local_world->v3.w = 1.0f;

    if (!reflexion) {
        this->RenderTextureHeadlights(view, local_world, 0);
    }

    if (this->pCarTypeInfo != 0 && this->pCarTypeInfo->GetCarUsageType() == CAR_USAGE_TYPE_COP) {
        if (this->IsLightOn(VehicleFX::LIGHT_COPRED)) {
            view->NumCopsCherry++;
        }
    }

    int car_pixel_size = view->GetPixelSize(position, this->mRadius);
    if (eGetCurrentViewMode() == EVIEWMODE_TWOH) {
        car_pixel_size = static_cast<int>(static_cast<float>(car_pixel_size) * 0.5f);
    }

    if (car_pixel_size < view->PixelMinSize) {
        return;
    }

    unsigned int visibility_state = view->GetVisibleState(&this->AABBMin, &this->AABBMax, local_world);
    if (visibility_state == EVISIBLESTATE_NOT) {
        return;
    }

    CarTypeInfo *car_type_info = &CarTypeInfoArray[this->pRideInfo->Type];
    int is_traffic_car = car_type_info->GetCarUsageType() == CAR_USAGE_TYPE_TRAFFIC;
    float base_headlight_intensity;
    float base_brakelight_intensity;

    if (is_traffic_car != 0) {
        base_brakelight_intensity = 0.0f;
        base_headlight_intensity = 1.0f;
    } else {
        base_headlight_intensity = 1.0f;
        base_brakelight_intensity = 0.0f;
    }

    float headlight_left_intensity;
    if (UTL::Collections::Singleton<INIS>::Get() == 0) {
        headlight_left_intensity = base_headlight_intensity;
    } else {
        headlight_left_intensity = 0.5f;
    }

    float headlight_right_intensity;
    if (UTL::Collections::Singleton<INIS>::Get() == 0) {
        headlight_right_intensity = base_headlight_intensity;
    } else {
        headlight_right_intensity = 0.5f;
    }

    float brakelight_left_intensity = base_brakelight_intensity;
    float brakelight_centre_intensity = 0.0f;
    float brakelight_right_intensity = brakelight_left_intensity;
    float reverselight_left_intensity = 0.0f;
    float reverselight_right_intensity = 0.0f;
    float coplight_intensityR = 0.0f;
    float coplight_intensityB = 0.0f;
    float coplight_intensityW = 0.0f;
    unsigned int flashHeadlights = 0;

    if (ForceHeadlightsOn != 0) {
        force_light_state |= 1;
    }
    if (ForceBrakelightsOn != 0) {
        force_light_state |= 2;
    }
    if (ForceReverselightsOn != 0) {
        force_light_state |= 4;
    }

    if (force_light_state & 1) {
        headlight_left_intensity += 1.0f;
        headlight_right_intensity += 1.0f;
    } else if (force_light_state & 8) {
        headlight_left_intensity = 0.0f;
        headlight_right_intensity = 0.0f;
    }
    if (force_light_state & 2) {
        brakelight_left_intensity += 1.0f;
        brakelight_right_intensity += 1.0f;
        brakelight_centre_intensity += 1.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_LHEAD)) {
        headlight_left_intensity = 1.0f;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_LHEAD)) {
        headlight_left_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_RHEAD)) {
        headlight_right_intensity = 1.0f;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_RHEAD)) {
        headlight_right_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_LBRAKE)) {
        brakelight_left_intensity += 1.0f;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_LBRAKE)) {
        brakelight_left_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_RBRAKE)) {
        brakelight_right_intensity += 1.0f;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_RBRAKE)) {
        brakelight_right_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_CBRAKE)) {
        brakelight_centre_intensity += 1.0f;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_CBRAKE)) {
        brakelight_centre_intensity = 0.0f;
    }
    if (force_light_state & 4) {
        reverselight_left_intensity += 1.0f;
        reverselight_right_intensity += 1.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_LREVERSE)) {
        reverselight_left_intensity += 1.0f;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_LREVERSE)) {
        reverselight_left_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_RREVERSE)) {
        reverselight_right_intensity += 1.0f;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_RREVERSE)) {
        reverselight_right_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_COPRED)) {
        coplight_intensityR = cpr;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_COPRED)) {
        coplight_intensityR = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_COPWHITE)) {
        coplight_intensityW = cpw;
        flashHeadlights = 1;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_COPWHITE)) {
        coplight_intensityW = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_COPBLUE)) {
        coplight_intensityB = cpb;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_COPBLUE)) {
        coplight_intensityB = 0.0f;
    }

    CarPart *preview_part = this->pRideInfo->GetPreviewPart();
    CAR_PART_ID preview_part_id = CARPARTID_NUM;

    if (preview_part != 0) {
        preview_part_id = static_cast<CAR_PART_ID>(*reinterpret_cast<signed char *>(reinterpret_cast<unsigned char *>(preview_part) + 4));
    }
    float constFlicker = coplightflicker(Ftime, 0);
    int FlareCount = 0;

    for (eLightFlare *light_flare = this->LightFlareList.GetHead(); light_flare != this->LightFlareList.EndOfList();
         light_flare = light_flare->GetNext()) {
        unsigned int name_hash = light_flare->NameHash;
        int is_brakelight = preview_part_id == CARPARTID_BRAKELIGHT;
        int is_headlight = preview_part_id == CARPARTID_HEADLIGHT;
        float intensity = 0.0f;
        float sizescale = 1.0f;

        if (is_traffic_car != 0 && light_flare->Type == 1) {
            light_flare->Type = 2;
        }
        if ((renderFlareFlags & 2) != 0 && light_flare->Type != 1) {
            continue;
        }
        if ((renderFlareFlags & 1) != 0) {
            if (light_flare->Type < 5 || light_flare->Type > 12) {
                continue;
            }
            sizescale = 0.75f;
        }

        switch (name_hash) {
            case 0x9DB90133:
                intensity = headlight_left_intensity;
                if (flashHeadlights != 0) {
                    intensity *= constFlicker;
                }
                break;
            case 0xD09091C6:
                intensity = headlight_right_intensity;
                if (flashHeadlights != 0) {
                    intensity *= 1.0f - constFlicker;
                }
                break;
            case 0x31A66786:
                intensity = brakelight_left_intensity;
                break;
            case 0xBF700A79:
                intensity = brakelight_right_intensity;
                break;
            case 0xA2A2FC7C:
                intensity = brakelight_centre_intensity;
                break;
            case 0x7A5B2F25:
                intensity = reverselight_left_intensity;
                break;
            case 0x7ADF7EF8:
                intensity = reverselight_right_intensity;
                break;
            case 0x1E4150B4:
            case 0x41489594:
                intensity = coplight_intensityR * coplightflicker2(Ftime, 0, FlareCount);
                break;
            case 0x6A52A241:
            case 0xE662C161:
                intensity = coplight_intensityB * coplightflicker2(Ftime, 1, FlareCount);
                break;
            case 0xB4348DBA:
                intensity = bSin(coplight_intensityW * coplightflicker2(Ftime, 2, FlareCount) * copWhitemul);
                break;
            case 0x28CD78F5:
                if (is_brakelight || (is_headlight && renderFlareFlags != 0)) {
                    intensity = 1.0f;
                }
                break;
            default:
                intensity = 0.0f;
                break;
        }

        if (intensity > 1.0f) {
            intensity = 1.0f;
        }

        if (intensity > 0.0f) {
            if (!reflexion) {
                eRenderLightFlare(
                    view, light_flare, local_world, intensity, REF_NONE, (renderFlareFlags & 1) != 0 ? FLARE_ENV : FLARE_NORM, 0.0f, 0,
                    sizescale
                );
            } else {
                eRenderLightFlare(view, light_flare, local_world, intensity, REF_TOPO, FLARE_REFLECT, 0.0f, 0, 1.0f);
            }
        }

        FlareCount++;
    }

    if (view->GetID() == EVIEW_FIRST_PLAYER && !reflexion) {
        bVector3 NisLightPosition(position->x + gTWEAKER_NISLightPosX, position->y + gTWEAKER_NISLightPosY, position->z + gTWEAKER_NISLightPosZ);
        bVector3 *lightPosition = const_cast<bVector3 *>(position);
        float extraIntensity = 1.0f;

        if (gTWEAKER_NISLightEnabled != 0) {
            lightPosition = &NisLightPosition;
            extraIntensity = gTWEAKER_NISLightIntensity;
        }

        if (coplight_intensityR > 0.0f) {
            AddQuickDynamicLight(&ShaperLightsCharacters, Lightslot, 1.0f, 0.0f, 0.0f, coplight_intensityR * extraIntensity, lightPosition);
        } else if (coplight_intensityB > 0.0f) {
            AddQuickDynamicLight(&ShaperLightsCharacters, Lightslot, 0.0f, 0.0f, 1.0f, coplight_intensityB * extraIntensity, lightPosition);
        } else {
            RestoreShaperRig(&ShaperLightsCharacters, Lightslot, &ShaperLightsCharactersBackup);
        }
    }
}

void CarRenderInfo::UpdateLightStateTextures() {
    bool lights_always_on;

    {
        int left_light_state = 1;
        int right_light_state = left_light_state;
        int left_light_state_hash = this->mUsedTextureInfos.ReplaceHeadlightHash[left_light_state];
        int right_light_state_hash = this->mUsedTextureInfos.ReplaceHeadlightHash[right_light_state];
        int left_light_glass_state_hash = this->mUsedTextureInfos.ReplaceHeadlightGlassHash[left_light_state];
        int right_light_glass_state_hash = this->mUsedTextureInfos.ReplaceHeadlightGlassHash[right_light_state];

        this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_LEFT].SetNewNameHash(left_light_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_RIGHT].SetNewNameHash(right_light_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_GLASS_LEFT].SetNewNameHash(left_light_glass_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_GLASS_RIGHT].SetNewNameHash(right_light_glass_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_LEFT].SetNewNameHash(left_light_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_RIGHT].SetNewNameHash(right_light_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_LEFT].SetNewNameHash(left_light_glass_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_RIGHT].SetNewNameHash(right_light_glass_state_hash);
        this->CarbonReplacementTextureTable[REPLACETEX_HEADLIGHT_LEFT].SetNewNameHash(left_light_state_hash);
        this->CarbonReplacementTextureTable[REPLACETEX_HEADLIGHT_RIGHT].SetNewNameHash(right_light_state_hash);
        this->CarbonReplacementTextureTable[REPLACETEX_HEADLIGHT_GLASS_LEFT].SetNewNameHash(left_light_glass_state_hash);
        this->CarbonReplacementTextureTable[REPLACETEX_HEADLIGHT_GLASS_RIGHT].SetNewNameHash(right_light_glass_state_hash);
        this->CarbonReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_LEFT].SetNewNameHash(left_light_state_hash);
        this->CarbonReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_RIGHT].SetNewNameHash(right_light_state_hash);
        this->CarbonReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_LEFT].SetNewNameHash(left_light_glass_state_hash);
        this->CarbonReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_RIGHT].SetNewNameHash(right_light_glass_state_hash);

        unsigned int new_headlight_hash = this->MasterReplacementTextureTable[REPLACETEX_WINDOW_FRONT].GetNewNameHash();
        this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_GLASS_LEFT].SetNewNameHash(new_headlight_hash);
        this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_GLASS_RIGHT].SetNewNameHash(new_headlight_hash);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_LEFT].SetNewNameHash(new_headlight_hash);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_RIGHT].SetNewNameHash(new_headlight_hash);
    }

    {
        int left_light_state = 0;
        int right_light_state = 0;
        int centre_light_state = 0;

        if (!this->IsLightBroken(VehicleFX::LIGHT_LBRAKE) && this->IsLightOn(VehicleFX::LIGHT_LBRAKE)) {
            left_light_state = 1;
        }
        if (!this->IsLightBroken(VehicleFX::LIGHT_RBRAKE) && this->IsLightOn(VehicleFX::LIGHT_RBRAKE)) {
            right_light_state = 1;
        }
        if (!this->IsLightBroken(VehicleFX::LIGHT_CBRAKE) && this->IsLightOn(VehicleFX::LIGHT_CBRAKE)) {
            centre_light_state = 1;
        }
        if (ForceBrakelightsOn != 0) {
            left_light_state = 1;
            right_light_state = 1;
        }

        int left_light_state_hash = this->mUsedTextureInfos.ReplaceBrakelightHash[left_light_state];
        int right_light_state_hash = this->mUsedTextureInfos.ReplaceBrakelightHash[right_light_state];
        int centre_light_state_hash = this->mUsedTextureInfos.ReplaceBrakelightHash[centre_light_state];
        int left_light_glass_state_hash = this->mUsedTextureInfos.ReplaceBrakelightGlassHash[left_light_state];
        int right_light_glass_state_hash = this->mUsedTextureInfos.ReplaceBrakelightGlassHash[right_light_state];
        int centre_light_glass_state_hash = this->mUsedTextureInfos.ReplaceBrakelightGlassHash[centre_light_state];

        this->MasterReplacementTextureTable[REPLACETEX_BRAKELIGHT_LEFT].SetNewNameHash(left_light_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_BRAKELIGHT_RIGHT].SetNewNameHash(right_light_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_BRAKELIGHT_CENTRE].SetNewNameHash(centre_light_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_BRAKELIGHT_GLASS_LEFT].SetNewNameHash(left_light_glass_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_BRAKELIGHT_GLASS_RIGHT].SetNewNameHash(right_light_glass_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_BRAKELIGHT_GLASS_CENTRE].SetNewNameHash(centre_light_glass_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_LEFT].SetNewNameHash(left_light_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_RIGHT].SetNewNameHash(right_light_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_CENTRE].SetNewNameHash(centre_light_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_GLASS_LEFT].SetNewNameHash(left_light_glass_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_GLASS_RIGHT].SetNewNameHash(right_light_glass_state_hash);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_GLASS_CENTRE].SetNewNameHash(centre_light_glass_state_hash);
    }
}

void UpdateEnvironmentMapCameras() {
    bVector3 *car_world_position = nullptr;
    eView *view = eGetView(1, false);

    if (view->GetCameraMover() != nullptr) {
        CameraAnchor *anchor = view->GetCameraMover()->GetAnchor();

        if (anchor != nullptr) {
            car_world_position = &reinterpret_cast<CameraAnchorLayout *>(anchor)->mGeomPos;
        } else {
            static bVector3 sCarWorldPosition_31751;
            static int tmp_45_31752;

            if (tmp_45_31752 == 0) {
                tmp_45_31752 = 1;
            }

            IPlayer *first_player = IPlayer::First(PLAYER_LOCAL);
            if (first_player != nullptr) {
                ISimable *simable = first_player->GetSimable();
                IRigidBody *player_rigid_body = simable != nullptr ? simable->GetRigidBody() : nullptr;

                if (player_rigid_body != nullptr) {
                    eSwizzleWorldVector(player_rigid_body->GetPosition(), sCarWorldPosition_31751);
                    bSub(&sCarWorldPosition_31751, &sCarWorldPosition_31751, &EnvMapEyeOffset);
                    car_world_position = &sCarWorldPosition_31751;
                }
            }
        }
    }

    if (car_world_position == nullptr) {
        FrontEndRenderingCar *fecar = nullptr;

        if (FrontEndRenderingCarList.GetHead() != FrontEndRenderingCarList.EndOfList()) {
            fecar = FrontEndRenderingCarList.GetHead();
        }
        if (fecar == nullptr) {
            return;
        }

        car_world_position = &fecar->Position;
    }

    bVector3 camera_eye_position(*view->GetCamera()->GetPosition());
    bVector3 envmap_position;
    bAdd(&camera_eye_position, &camera_eye_position, &EnvMapCamOffset);
    bAdd(&envmap_position, car_world_position, &EnvMapEyeOffset);
    eGetEnvMap()->UpdateCameras(&camera_eye_position, &envmap_position);
}

void RefreshAllFrontEndCarRenderInfos(CarType type) {
    for (FrontEndRenderingCar *front_end_car = FrontEndRenderingCarList.GetHead(); front_end_car != FrontEndRenderingCarList.EndOfList();
         front_end_car = front_end_car->GetNext()) {
        RideInfo *ride_info = &reinterpret_cast<FrontEndRenderingCarLayout *>(front_end_car)->mRideInfo;

        if ((type == static_cast<CarType>(-1) || ride_info->Type == type) && front_end_car->RenderInfo != 0) {
            front_end_car->RenderInfo->Refresh();
        }
    }
}

void RefreshAllRenderInfo(CarType type) {
    UTL::Collections::Listable<VehicleRenderConn, 10>::List::const_iterator it = VehicleRenderConn::GetList().begin();

    while (it != VehicleRenderConn::GetList().end()) {
        VehicleRenderConn *conn = *it;

        if ((type == static_cast<CarType>(-1) || conn->GetCarType() == type) && conn->mState > 1) {
            conn->RefreshRenderInfo();
        }
        ++it;
    }

    RefreshAllFrontEndCarRenderInfos(type);
}

void RenderFEFlares(eView *, int) {}

void RenderFrontEndCars(eView *view, int reflection) {
    if (DrawCars != 0) {
        if (reflection) {
            FEManager *fe_manager = FEManager::Get();
            if (fe_manager->GetGarageType() == GARAGETYPE_CAR_LOT) {
                return;
            }
        }

        eGetCurrentViewMode();

        for (FrontEndRenderingCar *front_end_car = FrontEndRenderingCarList.GetHead(); front_end_car != FrontEndRenderingCarList.EndOfList();
             front_end_car = front_end_car->GetNext()) {
            CarRenderInfo *render_info = front_end_car->RenderInfo;

            if (render_info != 0 && front_end_car->Visible != 0) {
                CARPART_LOD lod = render_info->mMinLodLevel;
                bMatrix4 body_matrix(front_end_car->BodyMatrix);
                bVector3 position(front_end_car->Position.x, front_end_car->Position.y, front_end_car->Position.z);

                if (reflection) {
                    float offset_scale =
                        *reinterpret_cast<float *>(*reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(render_info) + 0x1764) + 0xF4);

                    body_matrix.v2.x = -body_matrix.v2.x;
                    body_matrix.v2.y = -body_matrix.v2.y;
                    body_matrix.v2.z = -body_matrix.v2.z;
                    position.x += feposoff.x + body_matrix.v2.x * offset_scale;
                    position.y += feposoff.y + body_matrix.v2.y * offset_scale;
                    position.z += feposoff.z + body_matrix.v2.z * offset_scale;
                }

                render_info->Render(view, &position, &body_matrix, front_end_car->TireMatrices, front_end_car->BrakeMatrices,
                                    front_end_car->TireMatrices, reflection, 0, reflection, 1.0f, lod, lod);
            }
        }
    }
}

void RenderVehicleFlares(eView *view, int reflection, int renderFlareFlags) {
    VehicleRenderConn::RenderFlares(view, reflection, renderFlareFlags);
}

void DrawTestCars(eView *view, int reflection) {
    VehicleRenderConn::RenderAll(view, reflection);
}

int smooth_shadow_corners(int nVerts) {
    bVector3 v[2];
    bVector3 vTemp;
    int i;
    int iNew;
    int nCurr;
    int nPrev;
    int nNext;

    iNew = 0;
    nPrev = nVerts - 1;
    v[0] = hullVertArray2[0] - hullVertArray2[nPrev];
    v[0] *= lbl_8040AD7C;

    for (i = 0; i < nVerts; i++) {
        nCurr = i & 1;
        nPrev = nCurr ^ 1;
        nNext = i + 1;

        if (nNext == nVerts) {
            nNext = 0;
        }

        v[nCurr] = hullVertArray2[nNext] - hullVertArray2[i];
        v[nCurr] *= lbl_8040AD7C;

        hullVertArray3[iNew] = hullVertArray2[i] - v[nPrev];
        hullVertArray3[iNew + 1] = hullVertArray2[i] + v[nCurr];
        vTemp = v[nCurr] - v[nPrev];
        bScaleAdd(&hullVertArray3[iNew + 2], &hullVertArray2[i], &vTemp, lbl_8040AD7C);
        iNew += 3;
    }

    return iNew;
}

void CarRenderInfo::DrawAmbientShadow(eView *view, const bVector3 *position, float shadow_scale, bMatrix4 *localWorld, bMatrix4 *worldLocal,
                                      bMatrix4 *biasedIdentity) {
    const int N = 16;
    int in_front_end;
    bVector3 usPoint;
    float scaleW;
    float scaleL;
    bVector3 min;
    bVector3 max;
    float scale;
    bVector3 SunCarVector;
    bVector3 light_pos;
    SunChunkInfo *sun_info;
    float SunScale;
    bVector3 sunpos_in_car_space;
    float sunAdjX;
    float sunAdjY;
    float sunDX;
    float sunDY;
    float sunStartX;
    float sunStartY;
    int bad_points[4];
    float py;
    float px;
    float dy;
    float dx;
    float ps;
    float pt;
    float ds;
    float dt;
    float shadow_alpha_scale;
    unsigned int shadow_colour;
    float shadow_alpha_min;
    float shadow_alpha_max;
    float shadow_alpha;
    int shadow_alphai;
    int shadow_alphai_raw;
    TextureInfo *texture_info;
    unsigned int colour;

    hull_Origin = *position;
    if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_RACING) {
        eUnSwizzleWorldVector(*position, usPoint);
        this->mWorldPos.FindClosestFace(this->mWCollider, reinterpret_cast<const UMath::Vector3 &>(usPoint), false);
        if (this->mWorldPos.OnValidFace()) {
            this->mCar_elevation = this->mWorldPos.HeightAtPoint(reinterpret_cast<const UMath::Vector3 &>(usPoint));
            car_elevation = position->z - this->mCar_elevation;
        }
    }

    car_elevation_scale = lbl_8040ADC0;
    if (car_elevation > lbl_8040ADC0 && car_elevation < lbl_8040ADC4) {
        car_elevation_scale = car_elevation * lbl_8040ADC8;
    } else if (car_elevation > lbl_8040ADC4) {
        car_elevation_scale = lbl_8040ADCC;
    }

    min = this->AABBMin;
    max = this->AABBMax;
    in_front_end = IsGameFlowInFrontEnd();

    scale = lbl_8040ADD0;
    if (this->pRideInfo->Type == static_cast<CarType>(4)) {
        scale *= heliScale;
    }

    min.x *= scale;
    min.y *= scale;
    min.z *= scale;
    max.x *= scale;
    max.y *= scale;
    max.z *= scale;
    sun_info = SunInfo;
    if (sun_info == 0) {
        light_pos.x = lbl_8040ADD4;
        light_pos.y = lbl_8040ADC0;
        light_pos.z = lbl_8040ADD8;
    } else {
        light_pos.x = sun_info->CarShadowPositionX;
        light_pos.y = sun_info->CarShadowPositionY;
        light_pos.z = sun_info->CarShadowPositionZ;
    }

    SunCarVector = light_pos - *position;
    bNormalize(&SunCarVector, &SunCarVector);
    SunScale = (lbl_8040ADCC - SunCarVector.z) * lbl_8040ADDC;
    bMulMatrix(&sunpos_in_car_space, worldLocal, &light_pos);
    bNormalize(&sunpos_in_car_space, &sunpos_in_car_space);

    sunAdjY = -sunpos_in_car_space.y * SunScale * lbl_8040ADE0;
    sunAdjX = -sunpos_in_car_space.x * SunScale * lbl_8040ADE0;
    sunDX = bAbs(sunAdjX);
    sunDY = bAbs(sunAdjY);
    sunStartX = sunAdjX;
    if (sunAdjX > lbl_8040ADC0) {
        sunStartX = lbl_8040ADC0;
    }
    sunStartY = sunAdjY;
    if (sunAdjY > lbl_8040ADC0) {
        sunStartY = lbl_8040ADC0;
    }

    bVector3 p[16];
    bVector3 *pp;
    bVector2 uv[16];
    bVector2 *puv;
    pp = p;
    puv = uv;
    py = min.y + sunStartY;
    scaleW = (max.y - min.y) * lbl_8040ADE0;
    scaleL = (max.x - min.x) * lbl_8040ADE0;
    dy = scaleW;
    dx = scaleL;
    ds = lbl_8040ADE0;
    dt = lbl_8040ADE0;
    pt = lbl_8040ADC0;
    float pz = lbl_8040ADC0;

    for (int y = 0; y < 4; y++) {
        px = min.x + sunStartX;
        ps = lbl_8040ADC0;
        for (int x = 0; x < 4; x++) {
            pp->x = px;
            puv->x = ps;
            pp->y = py;
            pp->z = pz;
            puv->y = pt;
            eMulVector(pp, localWorld, pp);
            px += sunDX;
            ps += ds;
            pp++;
            puv++;
            px += dx;
        }
        bad_points[y] = 0;
        py += sunDY;
        py += dy;
        pt += dt;
    }

    if (in_front_end != 0) {
        bVector3 *pz = p;

        for (int x = 0; x < N; x++) {
            pz->z = lbl_8040ADC0;
            pz++;
        }
    } else if (this->mWCollider != 0) {
        bVector3 usCenter;
        bVector3 sCenter;
        bVector3 ref;
        bool quitIfSameFace = true;

        sCenter = *position;
        eUnSwizzleWorldVector(sCenter, usCenter);
        this->mWorldPos.SetTolerance(lbl_8040ADE4);
        this->mWorldPos.FindClosestFace(this->mWCollider, reinterpret_cast<const UMath::Vector3 &>(usCenter), false);
        if (this->mWorldPos.OnValidFace()) {
            UMath::Vector4 worldNormal;

            memset(&worldNormal, 0, sizeof(worldNormal));
            worldNormal.y = lbl_8040ADCC;
            this->mWorldPos.UNormal(&worldNormal);
            UMath::Unitxyz(worldNormal, worldNormal);
            eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(UMath::Vector4To3(worldNormal)), hull_Normal);
        } else {
            hull_Normal.x = lbl_8040ADC0;
            hull_Normal.y = lbl_8040ADC0;
            hull_Normal.z = lbl_8040ADCC;
        }

        ref.x = lbl_8040ADC0;
        ref.y = lbl_8040ADC0;
        ref.z = lbl_8040ADC0;
        eMulVector(&ref, localWorld, &ref);
        this->mWorldPos.SetTolerance(lbl_8040ADE4);
        bVector3 *point = p;
        for (int x = 0; x < N; x++, point++) {
            bVector3 sPoint;
            bVector3 usPoint;
            bool validFace;

            sPoint = *point;
            eUnSwizzleWorldVector(sPoint, usPoint);
            this->mWorldPos.FindClosestFace(this->mWCollider, reinterpret_cast<const UMath::Vector3 &>(usPoint), quitIfSameFace);
            validFace = this->mWorldPos.OnValidFace();
            point->z = this->mWorldPos.HeightAtPoint(reinterpret_cast<const UMath::Vector3 &>(usPoint));
            if (validFace) {
                bVector3 vec = *point - hull_Origin;
                float dot = bDot(&vec, &hull_Normal);

                if (dot < lbl_8040ADC0) {
                    dot = -dot;
                }

                if (dot < lbl_8040ADEC) {
                    quitIfSameFace = true;
                    continue;
                }
            }

            quitIfSameFace = false;

            if (this->pRideInfo->Type == static_cast<CarType>(4)) {
                point->z = lbl_8040ADCC;
                bad_points[x / 4]++;
            } else {
                point->z = this->mCar_elevation;
                ref.z = this->mCar_elevation;
            }
        }
    }

    shadow_alpha_scale = bAbs(localWorld->v2.z) * (lbl_8040ADCC - car_elevation_scale);
    if (in_front_end != 0) {
        shadow_alpha_min = lbl_8040ADF4;
        shadow_alpha_max = lbl_8040ADF4;
    } else {
        shadow_alpha_min = lbl_8040ADC0;
        shadow_alpha_max = lbl_8040ADF8;
    }

    shadow_alpha = (shadow_alpha_max - shadow_alpha_min) * shadow_alpha_scale + shadow_alpha_min;
    shadow_alphai_raw = static_cast<int>(shadow_alpha);
    shadow_alphai = 0;
    if (shadow_alphai_raw > 0) {
        shadow_alphai = shadow_alphai_raw;
    }
    if (shadow_alphai > 0xFE) {
        shadow_alphai = 0xFE;
    }

    shadow_colour = static_cast<unsigned int>(shadow_alphai << 24) | 0x00808080;
    texture_info = this->ShadowTexture;
    if (texture_info == 0 || (shadow_colour & 0xFF000000) == 0) {
        return;
    }

    colour = shadow_colour;
    {
        bVector3 *p0 = p;
        bVector3 *p1 = p + 4;
        bVector2 *u0 = uv;
        bVector2 *u1 = uv + 4;

        for (int y = 0; y < 3; y++) {
            if (bad_points[y] + bad_points[y + 1] <= 3) {
                if (exBeginStrip(texture_info, 8, biasedIdentity)) {
                    for (int x = 0; x < 4; x++) {
                        exAddVertex(*p0);
                        p0++;
                        exAddColour(colour);
                        exAddUV(u0->x, u0->y);
                        u0++;
                        exAddVertex(*p1);
                        p1++;
                        exAddColour(colour);
                        exAddUV(u1->x, u1->y);
                        u1++;
                    }
                    exEndStrip(view);
                } else {
                    p0 += 4;
                    p1 += 4;
                    u0 += 4;
                    u1 += 4;
                }
            } else {
                p0 += 4;
                p1 += 4;
                u0 += 4;
                u1 += 4;
            }
        }
    }
}

void CarRenderInfo::convex_hull(bVector3 *p, const WCollider *wcoll, int &n, float Z, float zBias, int fast) {
    int i;
    int dec;
    bool bPointValid;
    bVector3 usPoint;

    for (i = 0; i < n; i++) {
        P[i] = &p[i];
    }

    n = ch2d(reinterpret_cast<float **>(P), n);
    if (wcoll != nullptr) {
        this->mWorldPos.SetTolerance(lbl_8040AD70);
        if (fast != 0) {
            bVector3 *vec;
            float fastZ = Z;

            vec = hullVertArray2;
            bFill(vec, P[0]->x, P[0]->y, Z);

            eUnSwizzleWorldVector(*vec, usPoint);
            this->mWorldPos.FindClosestFace(wcoll, reinterpret_cast<const UMath::Vector3 &>(usPoint), true);
            if (!this->mWorldPos.OnValidFace()) {
                fastZ = this->mWorldPos.HeightAtPoint(reinterpret_cast<const UMath::Vector3 &>(usPoint));
            }

            dec = 0;
            for (i = 0; i < n; i++) {
                bFill(vec, P[i]->x, P[i]->y, fastZ);

                bVector3 dotVec = *vec - hull_Origin;
                float dot = bDot(&dotVec, &hull_Normal);
                if (dot < lbl_8040AD74) {
                    dot = -dot;
                }

                if (lbl_8040AD78 <= dot) {
                    dec++;
                } else {
                    vec++;
                }
            }
        } else {
            bool quitIfSameFace = true;
            bVector3 *vec;

            vec = hullVertArray2;
            dec = 0;

            for (i = 0; i < n; i++) {
                bFill(vec, P[i]->x, P[i]->y, Z);

                eUnSwizzleWorldVector(*vec, usPoint);
                this->mWorldPos.FindClosestFace(wcoll, reinterpret_cast<const UMath::Vector3 &>(usPoint), quitIfSameFace);
                if (!this->mWorldPos.OnValidFace()) {
                    vec->z = this->mWorldPos.HeightAtPoint(reinterpret_cast<const UMath::Vector3 &>(usPoint));
                }

                bVector3 dotVec = *vec - hull_Origin;
                float dot = bDot(&dotVec, &hull_Normal);
                if (dot < lbl_8040AD74) {
                    dot = -dot;
                }

                if (lbl_8040AD78 <= dot) {
                    dec++;
                } else {
                    vec++;
                }
                quitIfSameFace = false;
            }
        }

        n -= dec;
    }
}

static inline bool ccw(float **P, int i, int j, int k) {
    float d = P[k][1] - P[j][1];
    float c = P[k][0] - P[j][0];
    float b = P[i][1] - P[j][1];
    float a = P[i][0] - P[j][0];
    return a * d - b * c > 0.0f;
}

int make_chain(float **V, int n, int (*cmp)(const void *, const void *)) {
    int s;
    qsort(V, n, 4, cmp);
    s = 1;
    for (int i = 2; i < n; i++) {
        int j;
        for (j = s; j >= 1 && !ccw(V, j - 1, j, i); j--) {
        }
        s = j + 1;
        float *t = V[s];
        V[s] = V[i];
        V[i] = t;
    }
    return s;
}

int ch2d(float **P, int n) {
    int u = make_chain(P, n, cmpl);
    P[n] = P[0];
    return u + make_chain(P + u, n - u + 1, cmph);
}

void CarRenderInfo::DrawKeithProjShadow(eView *view, const bVector3 *position, bMatrix4 *localWorld, bMatrix4 *worldLocal, bMatrix4 *biasedIdentity,
                                        int body_lod) {
    if (body_lod < 3) {
        int n = 16;
        bVector3 *shadowVertices = hullVertArray1;
        float shadowZ;
        bVector3 lightV;
        bVector3 scale;
        int i;

        sh_Setup(const_cast<bVector3 *>(position));
        shadowZ = position->z;
        if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_RACING) {
            bVector3 worldPosition;

            eUnSwizzleWorldVector(*position, worldPosition);
            this->mWorldPos.FindClosestFace(this->mWCollider, reinterpret_cast<const UMath::Vector3 &>(worldPosition), false);
            if (this->mWorldPos.OnValidFace()) {
                shadowZ = this->mWorldPos.HeightAtPoint(reinterpret_cast<const UMath::Vector3 &>(worldPosition));
            }
        }

        lightV = cs_lightV;
        scale.x = lbl_8040AD98;
        scale.y = lbl_8040AD9C;
        scale.z = lbl_8040ADA0;
        float one_over_z = cs_OneOverZ;
        for (i = 0; i < n; i++) {
            bVector3 localPoint;
            bVector3 worldPoint;
            float scaleToGround;

            localPoint.x = PointCloud[i].x * scale.x;
            localPoint.y = PointCloud[i].y * scale.y;
            localPoint.z = PointCloud[i].z * scale.z;
            eMulVector(&worldPoint, localWorld, &localPoint);
            scaleToGround = (shadowZ - worldPoint.z) * one_over_z;
            shadowVertices[i].x = scaleToGround * lightV.x + worldPoint.x;
            shadowVertices[i].y = scaleToGround * lightV.y + worldPoint.y;
            shadowVertices[i].z = scaleToGround * lightV.z + worldPoint.z;
        }

        this->convex_hull(hullVertArray1, this->mWCollider, n, shadowZ, lbl_8040ADA4, body_lod != this->mMinLodLevel);
        if (body_lod == this->mMinLodLevel) {
            n = smooth_shadow_corners(n);
            shadowVertices = hullVertArray3;
        } else {
            shadowVertices = hullVertArray2;
        }

        if (n > 2) {
            bVector3 shadowCenter = shadowVertices[0] + shadowVertices[n / 2];
            int alpha = static_cast<int>((lbl_8040ADA0 - car_elevation_scale) * lbl_8040ADB0);
            int alpha_clamped = 0;
            unsigned int colour;

            shadowCenter *= lbl_8040ADA8;
            FancyCarShadowEdgeMult = car_elevation_scale * lbl_8040ADB4 + lbl_8040ADB8;
            if (alpha > 0) {
                alpha_clamped = alpha;
            }
            if (alpha_clamped > 0xFE) {
                alpha_clamped = 0xFE;
            }
            colour = static_cast<unsigned int>(alpha_clamped << 24) | 0x00808080;

            if (dshad != 0) {
                int start = 0;
                int stop = (n & ~1) - 1;

                while (start < stop) {
                    int next = start + 2;

                    if (eBeginStrip(this->ShadowRampTexture, 4, biasedIdentity)) {
                        eAddVertex(shadowVertices[start]);
                        eAddVertex(shadowCenter);
                        eAddVertex(shadowVertices[start + 1]);
                        eAddVertex(shadowVertices[next - (next / n) * n]);
                        eAddColour(colour);
                        eAddColour(colour);
                        eAddColour(colour);
                        eAddColour(colour);
                        eAddUV(lbl_8040ADBC, lbl_8040ADAC);
                        eAddUV(lbl_8040ADAC, lbl_8040ADAC);
                        eAddUV(lbl_8040ADBC, lbl_8040ADAC);
                        eAddUV(lbl_8040ADBC, lbl_8040ADAC);
                        eEndStrip(view);
                    }

                    start = next;
                }

                if ((n & 1) != 0 && eBeginStrip(this->ShadowRampTexture, 3, biasedIdentity)) {
                    eAddVertex(shadowVertices[n - 1]);
                    eAddVertex(shadowCenter);
                    eAddVertex(shadowVertices[0]);
                    eAddColour(colour);
                    eAddColour(colour);
                    eAddColour(colour);
                    eAddUV(lbl_8040ADBC, lbl_8040ADAC);
                    eAddUV(lbl_8040ADAC, lbl_8040ADAC);
                    eAddUV(lbl_8040ADBC, lbl_8040ADAC);
                    eEndStrip(view);
                }

                {
                    int nStart = n / 3;
                    int section = 0;
                    int startIndex = 0;

                    do {
                        int nSubVerts = nStart;
                        int nextStart = startIndex + nStart;
                        int nextSection = section + 1;

                        if (nextSection > 2) {
                            nSubVerts = n - startIndex;
                        }
                        section = nextSection;

                        if (exBeginStrip(this->ShadowRampTexture, (nSubVerts + 1) * 2, biasedIdentity)) {
                            int endIndex = startIndex + nSubVerts;
                            int loopIndex = startIndex;

                            for (; loopIndex < endIndex; loopIndex++) {
                                bVector3 *edge = &shadowVertices[loopIndex];
                                bVector3 edgeVertex(*edge);
                                bVector3 inner(edgeVertex);

                                inner.x = FancyCarShadowEdgeMult * (edgeVertex.x - shadowCenter.x) + shadowCenter.x;
                                inner.y = FancyCarShadowEdgeMult * (edgeVertex.y - shadowCenter.y) + shadowCenter.y;
                                exAddVertex(edgeVertex);
                                exAddVertex(inner);
                                exAddColour(colour);
                                exAddColour(colour);
                                exAddUV(lbl_8040ADBC, lbl_8040ADAC);
                                exAddUV(lbl_8040ADA0, lbl_8040ADAC);
                            }

                            if (loopIndex < n) {
                            } else {
                                loopIndex = 0;
                            }

                            {
                                bVector3 *edge = &shadowVertices[loopIndex];
                                bVector3 edgeVertex(*edge);
                                bVector3 inner(edgeVertex);

                                inner.x = FancyCarShadowEdgeMult * (edgeVertex.x - shadowCenter.x) + shadowCenter.x;
                                inner.y = FancyCarShadowEdgeMult * (edgeVertex.y - shadowCenter.y) + shadowCenter.y;
                                exAddVertex(edgeVertex);
                                exAddVertex(inner);
                                exAddColour(colour);
                                exAddColour(colour);
                                exAddUV(lbl_8040ADBC, lbl_8040ADAC);
                                exAddUV(lbl_8040ADA0, lbl_8040ADAC);
                                exEndStrip(view);
                            }
                        }

                        startIndex = nextStart;
                    } while (section < 3);
                }
            }
        }
    }
}

void CarRender_Service(float dT) {
    VehicleRenderConn::FetchData(dT);
    VehicleFragmentConn::FetchData(dT);
}
