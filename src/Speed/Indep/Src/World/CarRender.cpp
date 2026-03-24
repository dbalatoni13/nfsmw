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
extern unsigned int CarReplacementDecalHash[CarRenderInfo::REPLACETEX_DECAL_NUM];
extern unsigned int hcL;
extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;
extern int DrawCars;
extern int DrawCarShadow;
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
extern int ch2d(bVector3 **P, int n) asm("ch2d__FPPfi");
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

namespace {
float const CarBodyLodSwapSize[] = {120.0f, 25.0f, 20.0f, 10.0f, 0.0f};
float const TrafficCarBodyLodSwapSize[] = {20.0f, 10.0f, 4.0f, 0.0f, 0.0f};

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

CarEmitterPosition *AddTailEmitterPosition(bSList<CarEmitterPosition> &list, CarEmitterPosition *node) {
    bSListLayout<CarEmitterPosition> &layout = reinterpret_cast<bSListLayout<CarEmitterPosition> &>(list);
    bSNodeLayout<CarEmitterPosition> &node_layout = reinterpret_cast<bSNodeLayout<CarEmitterPosition> &>(*node);
    CarEmitterPosition *end = list.EndOfList();
    CarEmitterPosition *tail = layout.Tail;

    node_layout.Next = end;
    reinterpret_cast<bSNodeLayout<CarEmitterPosition> &>(*tail).Next = node;
    layout.Tail = node;

    return node;
}

} // namespace

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
      mAttributes(0xeec2271a, 0, nullptr)
{
    CarRenderRideInfoLayout *ride_layout = reinterpret_cast<CarRenderRideInfoLayout *>(ride_info);

    this->mOnLights = 0;
    this->mBrokenLights = 0;
    bMemSet(&this->TheCarPartCuller, 0, sizeof(this->TheCarPartCuller));
    this->mRadius = lbl_8040AA60;
    this->mAttributes.Change(Attrib::FindCollectionWithDefault(
        Attrib::Gen::ecar::ClassKey(), Attrib::StringToLowerCaseKey(CarTypeInfoArray[ride_info->Type].BaseModelName)));
    this->mMirrorLeftWheels = static_cast<unsigned char>(
        reinterpret_cast<Attrib::Gen::ecar::_LayoutStruct *>(this->mAttributes.GetLayoutPointer())->WheelSpokeCount) >> 7;
    this->mFlashing = false;
    this->mFlashInterval = 0.0f;
    bMemSet(&this->mDamageInfoCache, 0, 0x14);

    this->AnimTime = 0.0f;
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

    if (iRam8047ff04 == 3) {
        this->mMinLodLevel = ride_layout->mMinFELodLevel;
        this->mMaxLodLevel = ride_layout->mMaxFELodLevel;
    } else {
        this->mMinLodLevel = ride_layout->mMinLodLevel;
        this->mMaxLodLevel = ride_layout->mMaxLodLevel;
    }

    this->pRideInfo = ride_info;
    this->mMinReflectionLodLevel = ride_layout->mMinReflectionLodLevel;
    this->LastCarPartChanged = -1;
    this->CarTimebaseStart = bRandom(1.0f);
    this->mDeltaTime = 0.0f;
    this->pCarTypeInfo = &CarTypeInfoArray[ride_info->Type];
    this->CarbonHood = 0;
    this->mEmitterPositionsInitialized = false;
    bMemSet(this->mCarPartModels, 0, sizeof(this->mCarPartModels));
    GetUsedCarTextureInfo(&this->mUsedTextureInfos, this->pRideInfo, 0);
    {
        CarRenderUsedCarTextureInfoLayout *used_texture_info =
            reinterpret_cast<CarRenderUsedCarTextureInfoLayout *>(&this->mUsedTextureInfos);
        unsigned int window_front_hash = bStringHash("WINDOW_FRONT");
        unsigned int window_rear_hash = bStringHash("WINDOW_REAR");
        unsigned int window_left_front_hash = bStringHash("WINDOW_LEFT_FRONT");
        unsigned int window_left_rear_hash = bStringHash("WINDOW_LEFT_REAR");
        unsigned int window_right_front_hash = bStringHash("WINDOW_RIGHT_FRONT");
        unsigned int window_right_rear_hash = bStringHash("WINDOW_RIGHT_REAR");
        unsigned int rear_defroster_hash = bStringHash("REAR_DEFROSTER");

        this->MasterReplacementTextureTable[REPLACETEX_CARSKIN].SetOldNameHash(used_texture_info->MappedSkinHash);
        this->MasterReplacementTextureTable[REPLACETEX_CARSKINB].SetOldNameHash(used_texture_info->MappedSkinBHash);
        this->MasterReplacementTextureTable[REPLACETEX_GLOBALSKIN].SetOldNameHash(used_texture_info->MappedGlobalHash);
        this->MasterReplacementTextureTable[REPLACETEX_CARBONSKIN].SetOldNameHash(0xA7366AE6);
        this->MasterReplacementTextureTable[REPLACETEX_GLOBALCARBONSKIN].SetOldNameHash(0x3C84D757);
        this->MasterReplacementTextureTable[REPLACETEX_BADGING].SetOldNameHash(used_texture_info->MappedBadging);
        this->MasterReplacementTextureTable[REPLACETEX_WHEEL].SetOldNameHash(used_texture_info->MappedWheelHash);
        this->MasterReplacementTextureTable[REPLACETEX_SPINNER].SetOldNameHash(used_texture_info->MappedSpinnerHash);
        this->MasterReplacementTextureTable[REPLACETEX_SPOILER].SetOldNameHash(used_texture_info->MappedSpoilerHash);
        this->MasterReplacementTextureTable[REPLACETEX_ROOF_SCOOP].SetOldNameHash(used_texture_info->MappedRoofScoopHash);
        this->MasterReplacementTextureTable[REPLACETEX_DASHSKIN].SetOldNameHash(used_texture_info->MappedDashSkinHash);
        this->MasterReplacementTextureTable[REPLACETEX_DRIVER].SetOldNameHash(0x5799E60B);
        this->MasterReplacementTextureTable[REPLACETEX_TIRE].SetOldNameHash(used_texture_info->MappedTireHash);
    if (this->pCarTypeInfo->UsageType == CAR_USAGE_TYPE_TRAFFIC) {
        this->MasterReplacementTextureTable[REPLACETEX_TIRE].SetNewNameHash(bStringHash("_N", used_texture_info->MappedTireHash));
        }
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_FRONT].SetOldNameHash(window_front_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_REAR].SetOldNameHash(window_rear_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_LEFT_FRONT].SetOldNameHash(window_left_front_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_LEFT_REAR].SetOldNameHash(window_left_rear_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_RIGHT_FRONT].SetOldNameHash(window_right_front_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_RIGHT_REAR].SetOldNameHash(window_right_rear_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW_REAR_DEFOST].SetOldNameHash(rear_defroster_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_FRONT].SetOldNameHash(window_front_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_REAR].SetOldNameHash(window_rear_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_LEFT_FRONT].SetOldNameHash(window_left_front_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_LEFT_REAR].SetOldNameHash(window_left_rear_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_RIGHT_FRONT].SetOldNameHash(window_right_front_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_RIGHT_REAR].SetOldNameHash(window_right_rear_hash);
        this->MasterReplacementTextureTable[REPLACETEX_WINDOW2_REAR_DEFOST].SetOldNameHash(rear_defroster_hash);
        this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_LEFT].SetOldNameHash(0xA7E6EA53);
        this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_RIGHT].SetOldNameHash(0xA532FC46);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_LEFT].SetOldNameHash(used_texture_info->MappedLightHash[0]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_RIGHT].SetOldNameHash(used_texture_info->MappedLightHash[1]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_LEFT].SetOldNameHash(used_texture_info->MappedLightHash[2]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_RIGHT].SetOldNameHash(used_texture_info->MappedLightHash[3]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_CENTRE].SetOldNameHash(used_texture_info->MappedLightHash[4]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_LEFT].SetOldNameHash(used_texture_info->MappedLightHash[5]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_RIGHT].SetOldNameHash(used_texture_info->MappedLightHash[6]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_GLASS_LEFT].SetOldNameHash(used_texture_info->MappedLightHash[7]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_GLASS_RIGHT].SetOldNameHash(used_texture_info->MappedLightHash[8]);
        this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_GLASS_CENTRE].SetOldNameHash(used_texture_info->MappedLightHash[9]);
        this->BrakeLeftReplacementTextureTable[0].SetOldNameHash(0x17F9F794);
        this->BrakeLeftReplacementTextureTable[0].SetNewNameHash(0x85E9C79E);
        this->BrakeLeftReplacementTextureTable[1].SetOldNameHash(used_texture_info->MappedGlobalHash);
        this->BrakeRightReplacementTextureTable[0].SetOldNameHash(0x17F9F794);
        this->BrakeRightReplacementTextureTable[0].SetNewNameHash(0x17F9F794);
        this->BrakeRightReplacementTextureTable[1].SetOldNameHash(used_texture_info->MappedGlobalHash);
    }
    this->SwitchSkin(ride_info);
    {
        CarRenderUsedCarTextureInfoLayout *used_texture_info =
            reinterpret_cast<CarRenderUsedCarTextureInfoLayout *>(&this->mUsedTextureInfos);
        unsigned int badging_hash = used_texture_info->MappedBadging;
        if (BuildRegion::IsEurope()) {
            unsigned int europe_badging_hash = bStringHash("_EU", badging_hash);
            TextureInfo *europe_badging_texture = GetTextureInfo(europe_badging_hash, 0, 0);

            if (europe_badging_texture != nullptr) {
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
    this->ShadowRampTexture = GetTextureInfo(bStringHash("SHADOWRAMP"), 0, 0);

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
        bVector4 *pivot_position = nullptr;

        if (base_model != nullptr) {
            pivot_position = base_model->GetPivotPosition();
        }

        if (pivot_position != nullptr) {
            this->PivotPosition.x = pivot_position->x;
            this->PivotPosition.y = pivot_position->y;
            this->PivotPosition.z = pivot_position->z;
        } else {
            this->PivotPosition.x = 0.0f;
            this->PivotPosition.y = 0.0f;
            this->PivotPosition.z = 0.0f;
        }
    }

    this->CreateCarLightFlares();

    {
        UMath::Vector4 tire_offsets[4];
        bVector3 tire_positions[4];
        bVector3 left_side_sum;
        bVector3 right_side_sum;
        bVector3 left_side_position;
        bVector3 right_side_position;
        bVector3 underneath_position;
        float cull_position_scale = lbl_8040AA68 / lbl_8040AA6C;

        for (int wheel = 0; wheel < 4; wheel++) {
            this->GetAttributes().TireOffsets(tire_offsets[wheel], wheel);
            tire_positions[wheel].x = tire_offsets[wheel].x;
            tire_positions[wheel].y = tire_offsets[wheel].y;
            tire_positions[wheel].z = tire_offsets[wheel].z;
        }

        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_TIRE_FL, &tire_positions[0]);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_TIRE_FR, &tire_positions[1]);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_TIRE_RR, &tire_positions[2]);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_TIRE_RL, &tire_positions[3]);

        left_side_sum.x = tire_positions[0].x + tire_positions[3].x;
        left_side_sum.y = tire_positions[0].y + tire_positions[3].y;
        left_side_sum.z = tire_positions[0].z + tire_positions[3].z;
        left_side_position = left_side_sum;
        left_side_position.x *= cull_position_scale;
        left_side_position.y *= cull_position_scale;
        left_side_position.z *= cull_position_scale;

        right_side_sum.x = tire_positions[1].x + tire_positions[2].x;
        right_side_sum.y = tire_positions[1].y + tire_positions[2].y;
        right_side_sum.z = tire_positions[1].z + tire_positions[2].z;
        right_side_position = right_side_sum;
        right_side_position.x *= cull_position_scale;
        right_side_position.y *= cull_position_scale;
        right_side_position.z *= cull_position_scale;

        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_BRAKE_FL, &left_side_position);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_BRAKE_FR, &tire_positions[1]);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_BRAKE_RR, &right_side_position);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_BRAKE_RL, &tire_positions[3]);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_SIDE_FRONT, &left_side_position);
        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_SIDE_REAR, &right_side_position);

        underneath_position.x = left_side_position.x + right_side_position.x;
        underneath_position.y = left_side_position.y + right_side_position.y;
        underneath_position.z = left_side_position.z + right_side_position.z;
        underneath_position.x *= cull_position_scale;
        underneath_position.y *= cull_position_scale;
        underneath_position.z *= cull_position_scale;

        this->TheCarPartCuller.InitPart(CULLABLE_CAR_PART_UNDERNEATH, &underneath_position);
    }

    {
        for (int lod = this->mMinLodLevel; lod <= this->mMaxLodLevel; lod++) {
            eModel *smooth_normal_models[0x4C];

            bMemSet(smooth_normal_models, 0, sizeof(smooth_normal_models));
            for (int slot = 0; slot < 0x4C; slot++) {
                smooth_normal_models[slot] = this->mCarPartModels[slot][0][lod].GetModel();
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

        CarPart *window_tint_part = ride_info->GetPart(CARSLOTID_WINDOW_TINT);

        light_material_hash = 0x471A1DCA;
        if (window_tint_part != nullptr) {
            light_material_hash = CarPart_GetAppliedAttributeUParam(window_tint_part, 0x6BA02C05, 0);
        }

        this->LightMaterial_WindowTint = elGetLightMaterial(light_material_hash);

        CarPart *paint_rim_part = ride_info->GetPart(CARSLOTID_PAINT_RIM);
        CarPart *front_wheel_part = ride_info->GetPart(CARSLOTID_FRONT_WHEEL);

        light_material_hash = 0;
        if (paint_rim_part != nullptr && front_wheel_part != nullptr &&
            (reinterpret_cast<unsigned char *>(front_wheel_part)[5] >> 5) != 0) {
            light_material_hash = CarPart_GetAppliedAttributeUParam(paint_rim_part, 0x6BA02C05, 0);
        }

        this->LightMaterial_WheelRim = light_material_hash != 0 ? elGetLightMaterial(light_material_hash) : nullptr;

        this->LightMaterial_Caliper = nullptr;
        this->LightMaterial_Spoiler = nullptr;
        this->LightMaterial_Roof = nullptr;
        this->LightMaterial_Spinner = nullptr;
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
        for (int model_number = 0; model_index < 1; model_index++) {
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

static eModel *GetPackedCarPartModel(CarPartModel *car_part_model) {
    return reinterpret_cast<eModel *>(reinterpret_cast<CarPartModelLayout *>(car_part_model)->mModel & ~0x3);
}

static void SetPackedCarPartModel(CarPartModel *car_part_model, eModel *model) {
    unsigned int &packed_model = reinterpret_cast<CarPartModelLayout *>(car_part_model)->mModel;

    packed_model = reinterpret_cast<unsigned int>(model) | (packed_model & 1);
}

static void ClearPackedCarPartModel(CarPartModel *car_part_model) {
    reinterpret_cast<CarPartModelLayout *>(car_part_model)->mModel &= 1;
}

static bool DotPassesTest(const bVector3 *point) {
    bVector3 vec = *point - hull_Origin;
    float dot = bDot(&vec, &hull_Normal);

    if (dot < lbl_8040ADC0) {
        dot = -dot;
    }

    return dot < lbl_8040ADEC;
}

static void *CarRenderFrameMalloc(unsigned int size) {
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
    bVector3 bbox_min;
    bVector3 bbox_max;

    bInitializeBoundingBox(&this->AABBMin, &this->AABBMax);

    for (int slot_id = 0; slot_id < 0x4C; slot_id++) {
        for (int model_number = 0; model_number < 1; model_number++) {
            for (int lod = this->mMinLodLevel; lod <= this->mMaxLodLevel; lod++) {
                eModel *model = this->mCarPartModels[slot_id][model_number][lod].GetModel();

                if (model != 0 && model->GetNameHash() != 0) {
                    model->UnInit();
                    CarPartModelPool->Free(model);
                    reinterpret_cast<CarPartModelLayout *>(&this->mCarPartModels[slot_id][model_number][lod])->mModel &= 1;
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
                        TheGameFlowManager.GetState() == GAMEFLOW_STATE_IN_FRONTEND)) {
                    CARPART_LOD clamped_lod = special_minimum;

                    if (special_minimum < model_lod) {
                        clamped_lod = model_lod;
                    }
                    if (special_maximum < clamped_lod) {
                        clamped_lod = special_maximum;
                    }
                    model_lod = clamped_lod;
                }

                unsigned int model_name_hash = CarPart_GetModelNameHash(car_part, model_number, model_lod);

                if (model_name_hash == 0) {
                    continue;
                }

                CarPartModel *car_part_model = &this->mCarPartModels[slot_id][model_number][lod];
                eModel *model = static_cast<eModel *>(CarPartModelPool->Malloc());
                unsigned int &packed_model = reinterpret_cast<CarPartModelLayout *>(car_part_model)->mModel;

                packed_model = reinterpret_cast<unsigned int>(model) | (packed_model & 1);
                model->Init(model_name_hash);

                bool is_solid = model->Solid != 0;
                if (!is_solid) {
                    model->UnInit();
                    CarPartModelPool->Free(model);
                    packed_model &= 1;
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
            unsigned int &rear_wheel_packed_model = reinterpret_cast<CarPartModelLayout *>(rear_wheel_part_model)->mModel;
            eModel *rear_wheel_model = reinterpret_cast<eModel *>(rear_wheel_packed_model & ~0x3);

            if (front_wheel_model != 0 && rear_wheel_model == 0) {
                rear_wheel_model = static_cast<eModel *>(CarPartModelPool->Malloc());
                rear_wheel_packed_model = reinterpret_cast<unsigned int>(rear_wheel_model) | (rear_wheel_packed_model & 1);
                rear_wheel_model->Init(front_wheel_model->GetNameHash());

                bool is_solid = rear_wheel_model->Solid != 0;
                if (!is_solid) {
                    rear_wheel_model->UnInit();
                    CarPartModelPool->Free(rear_wheel_model);
                    rear_wheel_packed_model &= 1;
                } else {
                    rear_wheel_model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                }
            }

            CarPartModel *front_brake_part_model = &this->mCarPartModels[CARSLOTID_FRONT_BRAKE][model_number][lod];
            CarPartModel *rear_brake_part_model = &this->mCarPartModels[CARSLOTID_REAR_BRAKE][model_number][lod];
            unsigned int &rear_brake_packed_model = reinterpret_cast<CarPartModelLayout *>(rear_brake_part_model)->mModel;
            eModel *front_brake_model = front_brake_part_model->GetModel();
            eModel *rear_brake_model = reinterpret_cast<eModel *>(rear_brake_packed_model & ~0x3);

            if (front_brake_model != 0 && rear_brake_model == 0) {
                rear_brake_model = static_cast<eModel *>(CarPartModelPool->Malloc());
                rear_brake_packed_model = reinterpret_cast<unsigned int>(rear_brake_model) | (rear_brake_packed_model & 1);
                rear_brake_model->Init(front_brake_model->GetNameHash());

                bool is_solid = rear_brake_model->Solid != 0;
                if (!is_solid) {
                    rear_brake_model->UnInit();
                    CarPartModelPool->Free(rear_brake_model);
                    rear_brake_packed_model &= 1;
                } else {
                    rear_brake_model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                }
            }
        }
    }

    eModel *front_wheel_model = this->mCarPartModels[CARSLOTID_FRONT_WHEEL][0][this->mMinLodLevel].GetModel();
    eModel *rear_wheel_model = this->mCarPartModels[CARSLOTID_REAR_WHEEL][0][this->mMinLodLevel].GetModel();

    if (front_wheel_model != 0) {
        float wheel_width;
        float wheel_radius;

        front_wheel_model->GetBoundingBox(&bbox_min, &bbox_max);
        wheel_width = bbox_max.y - bbox_min.y;
        if (wheel_width < 0.0f) {
            wheel_width = -wheel_width;
        }
        this->WheelWidths[0] = wheel_width;

        wheel_radius = bbox_max.x - bbox_min.x;
        if (wheel_radius < 0.0f) {
            wheel_radius = -wheel_radius;
        }
        this->WheelRadius[0] = wheel_radius * 0.5f;
    }

    if (rear_wheel_model != 0) {
        float wheel_width;
        float wheel_radius;

        rear_wheel_model->GetBoundingBox(&bbox_min, &bbox_max);
        wheel_width = bbox_max.y - bbox_min.y;
        if (wheel_width < 0.0f) {
            wheel_width = -wheel_width;
        }
        this->WheelWidths[1] = wheel_width;

        wheel_radius = bbox_max.x - bbox_min.x;
        if (wheel_radius < 0.0f) {
            wheel_radius = -wheel_radius;
        }
        this->WheelRadius[1] = wheel_radius * 0.5f;
    }

    this->ModelOffset.x = (this->AABBMax.x + this->AABBMin.x) * 0.5f;
    this->ModelOffset.y = (this->AABBMax.y + this->AABBMin.y) * 0.5f;
    this->ModelOffset.z = (this->AABBMax.z + this->AABBMin.z) * 0.5f;

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

        this->GetAttributes().TireOffsets(tire_offset, wheel);
        this->WheelYRenderOffset[wheel] = -tire_offset.y;

        if (this->pRideInfo != nullptr) {
            body_part = this->pRideInfo->GetPart(CARSLOTID_BODY);
        }

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
        if (0.0f < this->WheelYRenderOffset[wheel]) {
            this->WheelYRenderOffset[wheel] += kit_wheel_offset_float;
        } else {
            this->WheelYRenderOffset[wheel] -= kit_wheel_offset_float;
        }

        model_radius = this->WheelRadius[wheel_end];
        model_width = this->WheelWidths[wheel_end];
        desired_width = this->GetAttributes().TireSkidWidth(wheel);

        if (wheel > 1) {
            desired_width *= this->GetAttributes().TireSkidWidthKitScale(kit_number).y;
        } else {
            desired_width *= this->GetAttributes().TireSkidWidthKitScale(kit_number).x;
        }

        desired_radius = tire_offset.w;

        if (model_width <= 0.0f || desired_width <= 0.0f) {
            this->WheelWidthScales[wheel] = 1.0f;
        } else {
            this->WheelWidthScales[wheel] = desired_width / model_width;
        }

        if (model_radius <= 0.0f || desired_radius <= 0.0f) {
            this->WheelRadiusScales[wheel] = 1.0f;
        } else {
            this->WheelRadiusScales[wheel] = desired_radius / model_radius;
        }
    }
}

void CarRenderInfo::UpdateDecalTextures(RideInfo *ride_info) {
    unsigned int alpha_hash;
    unsigned int decal_hashes[8];
    CarPart *hood_decals;
    unsigned int size_hash;
    unsigned int shape_hash;
    unsigned int size_hashes[3];
    unsigned int shape_hashes[3];

    alpha_hash = bStringHash("DEFAULTALPHA");

    for (int i = REPLACETEX_DECAL_START; i <= REPLACETEX_DECAL_END; i++) {
        this->MasterReplacementTextureTable[i].SetOldNameHash(CarReplacementDecalHash[i - REPLACETEX_DECAL_START]);
        if (alpha_hash != this->MasterReplacementTextureTable[i].GetNewNameHash()) {
            this->MasterReplacementTextureTable[i].SetNewNameHash(alpha_hash);
        }
    }

    decal_hashes[0] = bStringHash("DUMMY_DECAL1");
    decal_hashes[1] = bStringHash("DUMMY_DECAL2");
    decal_hashes[2] = bStringHash("DUMMY_DECAL3");
    decal_hashes[3] = bStringHash("DUMMY_DECAL4");
    decal_hashes[4] = bStringHash("DUMMY_DECAL5");
    decal_hashes[5] = bStringHash("DUMMY_DECAL6");
    decal_hashes[6] = bStringHash("DUMMY_NUMBER_LEFT");
    decal_hashes[7] = bStringHash("DUMMY_NUMBER_RIGHT");

    for (int i = 0; i < 48; i++) {
        this->DecalReplacementTextureTable[i].SetOldNameHash(decal_hashes[i % 8]);
        if (alpha_hash != this->DecalReplacementTextureTable[i].GetNewNameHash()) {
            this->DecalReplacementTextureTable[i].SetNewNameHash(alpha_hash);
        }
    }

    hood_decals = ride_info->GetPart(CARSLOTID_HOOD);
    size_hash = bStringHash("SIZE");
    shape_hash = bStringHash("SHAPE");
    size_hashes[0] = bStringHash("SMALL");
    size_hashes[1] = bStringHash("MEDIUM");
    size_hashes[2] = bStringHash("LARGE");
    shape_hashes[0] = bStringHash("SQUARE");
    shape_hashes[1] = bStringHash("RECT");
    shape_hashes[2] = bStringHash("WIDE");

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
                CarPart *decal_texture_part = ride_info->GetPart(first_tex_part + j);

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
                    if (decal_texture_hash != replace_table[j].GetNewNameHash()) {
                        replace_table[j].SetNewNameHash(decal_texture_hash);
                    }
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

    eDynamicLightContext light_context_storage;
    eDynamicLightContext *light_context;
    bMatrix4 *local_world;
    bMatrix4 *world_local;
    bVector4 *camera_world_position;
    eShaperLightRig *shaper_lights = &ShaperLightsCarsInGame;
    float pixel_size = static_cast<float>(car_pixel_size);
    float const *body_lod_swap_size = CarBodyLodSwapSize;
    Camera *camera = view->GetCamera();
    bool print_query_light_mat;
    unsigned short steering = this->mSteeringR;
    unsigned short left_steering = this->mSteeringL;
    int body_lod = static_cast<int>(this->mMinLodLevel);
    int tire_lod = static_cast<int>(this->mMinLodLevel);

    if (CurrentBufferEnd <= CurrentBufferPos + 0x130) {
        FrameMallocFailed = 1;
        FrameMallocFailAmount += 0x130;
        light_context = 0;
    } else {
        light_context = reinterpret_cast<eDynamicLightContext *>(CurrentBufferPos);
        CurrentBufferPos += 0x130;
    }
    if (CurrentBufferEnd <= CurrentBufferPos + sizeof(bMatrix4)) {
        FrameMallocFailed = 1;
        FrameMallocFailAmount += sizeof(bMatrix4);
        local_world = 0;
    } else {
        local_world = reinterpret_cast<bMatrix4 *>(CurrentBufferPos);
        CurrentBufferPos += sizeof(bMatrix4);
    }
    if (CurrentBufferEnd <= CurrentBufferPos + sizeof(bMatrix4)) {
        FrameMallocFailed = 1;
        FrameMallocFailAmount += sizeof(bMatrix4);
        world_local = 0;
    } else {
        world_local = reinterpret_cast<bMatrix4 *>(CurrentBufferPos);
        CurrentBufferPos += sizeof(bMatrix4);
    }
    if (CurrentBufferEnd <= CurrentBufferPos + sizeof(bVector4)) {
        FrameMallocFailed = 1;
        FrameMallocFailAmount += sizeof(bVector4);
        camera_world_position = 0;
    } else {
        camera_world_position = reinterpret_cast<bVector4 *>(CurrentBufferPos);
        CurrentBufferPos += sizeof(bVector4);
    }

    if (left_steering > 0x8000) {
        left_steering = static_cast<unsigned short>(-left_steering);
    }
    if (steering > 0x8000) {
        steering = static_cast<unsigned short>(-steering);
    }
    if (steering < left_steering) {
        steering = this->mSteeringL;
    }
    if (reflexion != 0 || this->mMinLodLevel == 2) {
        body_lod_swap_size = TrafficCarBodyLodSwapSize;
    }

    if (pixel_size > body_lod_swap_size[0]) {
        int lod_offset = 0;

        do {
            lod_offset++;
            if (pixel_size < body_lod_swap_size[lod_offset]) {
                continue;
            }
            break;
        } while (lod_offset < 4);

        body_lod += lod_offset;
        tire_lod += lod_offset;
    }

    if (body_lod > static_cast<int>(this->mMaxLodLevel)) {
        body_lod = static_cast<int>(this->mMaxLodLevel);
    }
    if (tire_lod > static_cast<int>(this->mMaxLodLevel)) {
        tire_lod = static_cast<int>(this->mMaxLodLevel);
    }
    if (ForceCarLOD != -1) {
        body_lod = ForceCarLOD;
        if (body_lod < static_cast<int>(this->mMinLodLevel)) {
            body_lod = static_cast<int>(this->mMinLodLevel);
        }
        if (body_lod > static_cast<int>(this->mMaxLodLevel)) {
            body_lod = static_cast<int>(this->mMaxLodLevel);
        }
    }
    if (ForceTireLOD != -1) {
        tire_lod = ForceTireLOD;
        if (tire_lod < static_cast<int>(this->mMinLodLevel)) {
            tire_lod = static_cast<int>(this->mMinLodLevel);
        }
        if (tire_lod > static_cast<int>(this->mMaxLodLevel)) {
            tire_lod = static_cast<int>(this->mMaxLodLevel);
        }
    }
    if (this->pRideInfo != 0 && this->pCarTypeInfo != 0 && this->pCarTypeInfo->UsageType != CAR_USAGE_TYPE_RACING) {
        extra_render_flags |= 0x800;
        if (INIS::Get() != 0) {
            extra_render_flags |= 0x80000;
        }
    }
    if (light_context == 0 || local_world == 0 || world_local == 0 || camera_world_position == 0) {
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

    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(body_matrix), *reinterpret_cast<Mtx44 *>(world_local));
    world_local->v3.x = position.x;
    world_local->v3.y = position.y;
    world_local->v3.z = position.z;
    world_local->v3.w = 1.0f;
    if (camera != 0) {
        camera_world_position->x = camera->GetPosition()->x;
        camera_world_position->y = camera->GetPosition()->y;
        camera_world_position->z = camera->GetPosition()->z;
    } else {
        camera_world_position->x = position.x;
        camera_world_position->y = position.y;
        camera_world_position->z = position.z;
    }
    camera_world_position->w = 1.0f;
    print_query_light_mat = PrintQueryLightMat != 0;
    if (print_query_light_mat) {
        PrintLightQuery = 1;
    }
    if (iRam8047ff04 != 6) {
        FEManager *fe_manager = FEManager::Get();

        if (fe_manager != 0) {
            switch (fe_manager->GetGarageType()) {
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
            case GARAGETYPE_MAIN_FE:
            default:
                shaper_lights = &ShaperLightsQRace;
                break;
            }
        }
    }
    elResetLightContext(&light_context_storage);
    elSetupLights(&light_context_storage, shaper_lights, &position, 0, &hack_man_matrix, view);
    elCloneLightContext(light_context, world_local, &hack_man_matrix, camera_world_position, view, &light_context_storage);
    this->CarFrame = eFrameCounter;
    if (print_query_light_mat) {
        PrintLightQuery = 0;
    }
    this->UpdateLightStateTextures();

    if (!IsGameFlowInFrontEnd()) {
        if (camera != 0) {
            this->TheCarPartCuller.CullParts(camera->GetPosition(), steering);
        }
    }

    for (int slot_id = 0; slot_id < 0x4C; slot_id++) {
        if (slot_id == CARSLOTID_FRONT_WHEEL || slot_id == CARSLOTID_REAR_WHEEL || slot_id == CARSLOTID_FRONT_BRAKE ||
            slot_id == CARSLOTID_REAR_BRAKE || slot_id == CARSLOTID_SPOILER || slot_id == CARSLOTID_ROOF) {
            continue;
        }

        CarPartModel *car_part_model = &this->mCarPartModels[slot_id][0][body_lod];
        eModel *model = car_part_model->GetModel();

        if (model != 0) {
            eLightMaterial *paint_material = this->LightMaterial_CarSkin;

            if (slot_id == CARSLOTID_SPOILER || slot_id == CARSLOTID_UNIVERSAL_SPOILER_BASE) {
                if (this->LightMaterial_Spoiler != 0) {
                    paint_material = this->LightMaterial_Spoiler;
                }
            } else if (slot_id == CARSLOTID_ROOF) {
                if (this->LightMaterial_Roof != 0) {
                    paint_material = this->LightMaterial_Roof;
                }
            } else if (slot_id == CARSLOTID_HOOD && this->CarbonHood != 0 && this->LightMaterial_Carbon != 0) {
                paint_material = this->LightMaterial_Carbon;
            }

            if (paint_material != 0) {
                model->ReplaceLightMaterial(0xD6D6080A, paint_material);
            }
            if (this->LightMaterial_WindowTint != 0) {
                model->ReplaceLightMaterial(0x471A1DCA, this->LightMaterial_WindowTint);
            }
        }

        this->RenderPart(view, car_part_model, local_world, light_context, extra_render_flags);
    }

    {
        CarPartModel *spoiler_part_model = &this->mCarPartModels[CARSLOTID_SPOILER][0][body_lod];
        eModel *spoiler_model = spoiler_part_model->GetModel();

        if (spoiler_model != 0) {
            eLightMaterial *spoiler_material = this->LightMaterial_Spoiler;

            if (spoiler_material == 0) {
                spoiler_material = this->LightMaterial_CarSkin;
            }
            if (spoiler_material != 0) {
                spoiler_model->ReplaceLightMaterial(0xD6D6080A, spoiler_material);
            }

            ::Render(view, spoiler_model, local_world, light_context, extra_render_flags, 0);
        }
    }

    {
        CarPartModel *roof_part_model = &this->mCarPartModels[CARSLOTID_ROOF][0][body_lod];
        eModel *roof_model = roof_part_model->GetModel();

        if (roof_model != 0) {
            eLightMaterial *roof_material = this->LightMaterial_Roof;

            if (roof_material == 0) {
                roof_material = this->LightMaterial_CarSkin;
            }
            if (roof_material != 0) {
                roof_model->ReplaceLightMaterial(0xD6D6080A, roof_material);
            }

            ::Render(view, roof_model, local_world, light_context, extra_render_flags, 0);
        }
    }

    if (tire_matrices != 0) {
        CarPartModel *front_tire = &this->mCarPartModels[CARSLOTID_FRONT_WHEEL][0][tire_lod];
        CarPartModel *rear_tire = &this->mCarPartModels[CARSLOTID_REAR_WHEEL][0][tire_lod];

        for (int wheel = 0; wheel < 4; wheel++) {
            bMatrix4 tire_local_world = tire_matrices[wheel];

            if ((wheel == 0 || wheel == 3) && this->mMirrorLeftWheels) {
                eMulMatrix(&tire_local_world, &tire_local_world, &LeftTireMirrorMatrix);
            }

            eMulMatrix(&tire_local_world, &tire_local_world, local_world);
            CarPartModel *wheel_part_model = wheel < 2 ? front_tire : rear_tire;
            eModel *wheel_model = wheel_part_model->GetModel();

            if (wheel_model != 0) {
                eReplacementTextureTable *brake_replacement_table =
                    (wheel == 0 || wheel == 3) ? this->BrakeLeftReplacementTextureTable : this->BrakeRightReplacementTextureTable;

                wheel_model->AttachReplacementTextureTable(brake_replacement_table, 2, 0);
                if (this->LightMaterial_WheelRim != 0) {
                    wheel_model->ReplaceLightMaterial(0xD6640DFF, this->LightMaterial_WheelRim);
                }
                if (this->LightMaterial_Spinner != 0) {
                    wheel_model->ReplaceLightMaterial(0xA3186E2B, this->LightMaterial_Spinner);
                }
            }

            if (wheel_model != 0) {
                ::Render(view, wheel_model, &tire_local_world, light_context, extra_render_flags, 0);
            }
        }
    }

    if (brake_matrices != 0) {
        CarPartModel *front_brake = &this->mCarPartModels[CARSLOTID_FRONT_BRAKE][0][body_lod];
        CarPartModel *rear_brake = &this->mCarPartModels[CARSLOTID_REAR_BRAKE][0][body_lod];

        for (int wheel = 0; wheel < 4; wheel++) {
            bMatrix4 brake_local_world = brake_matrices[wheel];

            eMulMatrix(&brake_local_world, &brake_local_world, local_world);
            CarPartModel *brake_part_model = wheel < 2 ? front_brake : rear_brake;
            eModel *brake_model = brake_part_model->GetModel();

            if (brake_model != 0) {
                eReplacementTextureTable *brake_replacement_table =
                    (wheel == 0 || wheel == 3) ? this->BrakeLeftReplacementTextureTable : this->BrakeRightReplacementTextureTable;

                brake_model->AttachReplacementTextureTable(brake_replacement_table, 2, 0);
                if (this->LightMaterial_Caliper != 0) {
                    brake_model->ReplaceLightMaterial(0xD6640DFF, this->LightMaterial_Caliper);
                    brake_model->ReplaceLightMaterial(0xA3186E2B, this->LightMaterial_Caliper);
                }
            }

            if (brake_model != 0) {
                ::Render(view, brake_model, &brake_local_world, light_context, extra_render_flags, 0);
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

    if (!reflexion && DrawCarShadow != 0 && shadow_scale > 0.0f) {
        bMatrix4 biased_identity = *local_world;

        view->BiasMatrixForZSorting(&biased_identity, 0.0f);
        this->DrawAmbientShadow(view, &position, shadow_scale, local_world, world_local, &biased_identity);
    }

    return true;
}

void CarRenderInfo::RenderPart(eView *view, CarPartModel *carPart, bMatrix4 *local_to_world, eDynamicLightContext *light_context,
                               unsigned int flags) {
    if (carPart != nullptr) {
        eModel *model = carPart->GetModel();

        if (model == nullptr) {
            model = &StandardDebugModel;
        }

        ::Render(view, model, local_to_world, light_context, flags, 0);
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
        float zero = lbl_8040ADFC;
        float tire_mid = lbl_8040AE00;
        float engine_offset = lbl_8040AE04;

        for (int fxpos = 0; fxpos < NUM_CARFXPOS; fxpos++) {
            int pos_count = 0;
            bool is_based_off_position_marker =
                GetNumCarEffectMarkerHashes(static_cast<CarEffectPosition>(fxpos), pos_count);

            if (is_based_off_position_marker) {
                if (pos_count > 0) {
                    bSList<CarEmitterPosition> &pos_list = this->EmitterPositionList[fxpos];

                    this->GetEmitterPositions(pos_list, GetCarEffectMarkerHashes(static_cast<CarEffectPosition>(fxpos)),
                                              pos_count);
                }
                continue;
            }

            CarEffectPosition efxpos = static_cast<CarEffectPosition>(fxpos);
            bSList<CarEmitterPosition> &pos_list = this->EmitterPositionList[fxpos];
            CarEmitterPosition *empos = nullptr;
            switch (efxpos) {
                case CARFXPOS_NONE:
                    empos = new CarEmitterPosition(zero, zero, zero);
                    break;
                case CARFXPOS_FRONT_TIRES:
                    {
                        bVector4 *fl = tire_positions;
                        bVector4 *fr = tire_positions + 1;
                        bVector4 avg = *fl + *fr;

                        avg *= tire_mid;
                        empos = new CarEmitterPosition(avg.x, avg.y, avg.z);
                    }
                    break;
                case CARFXPOS_REAR_TIRES:
                    {
                        bVector4 *rr = tire_positions + 2;
                        bVector4 *rl = tire_positions + 3;
                        bVector4 avg = *rr + *rl;

                        avg *= tire_mid;
                        empos = new CarEmitterPosition(avg.x, avg.y, avg.z);
                    }
                    break;
                case CARFXPOS_LEFT_TIRES:
                    {
                        bVector4 *fl = tire_positions;
                        bVector4 *rl = tire_positions + 3;
                        bVector4 avg = *fl + *rl;

                        avg *= tire_mid;
                        empos = new CarEmitterPosition(avg.x, avg.y, avg.z);
                    }
                    break;
                case CARFXPOS_RIGHT_TIRES:
                    {
                        bVector4 *fr = tire_positions + 1;
                        bVector4 *rr = tire_positions + 2;
                        bVector4 avg = *fr + *rr;

                        avg *= tire_mid;
                        empos = new CarEmitterPosition(avg.x, avg.y, avg.z);
                    }
                    break;
                case CARFXPOS_TIRE_FL:
                    empos = new CarEmitterPosition(tire_positions->x, tire_positions->y, tire_positions->z);
                    break;
                case CARFXPOS_TIRE_FR:
                    empos = new CarEmitterPosition(tire_positions[1].x, tire_positions[1].y, tire_positions[1].z);
                    break;
                case CARFXPOS_TIRE_RR:
                    empos = new CarEmitterPosition(tire_positions[2].x, tire_positions[2].y, tire_positions[2].z);
                    break;
                case CARFXPOS_TIRE_RL:
                    empos = new CarEmitterPosition(tire_positions[3].x, tire_positions[3].y, tire_positions[3].z);
                    break;
                case CARFXPOS_ENGINE:
                    {
                        bVector4 *fl = tire_positions;
                        bVector4 *fr = tire_positions + 1;
                        bVector4 avg = *fl + *fr;
                        bVector4 diff;

                        avg *= tire_mid;
                        bSub(&diff, fl, fr);
                        empos = new CarEmitterPosition(avg.x, avg.y, avg.z + diff.y * engine_offset);
                    }
                    break;
            }

            if (empos != nullptr) {
                pos_list.AddTail(empos);
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
    counter_31665 = (counter_31665 + 1) % copModulo;

    return bSin((time + copt * static_cast<float>(offset)) * copm + 1.5707964f);
}

float coplightflicker2(float time, int whichColor, int flarecount) {
    float offset = 0.0f;
    float flicker;

    counter_31669 = (counter_31669 + 1) % copModulo;

    if (whichColor == 1) {
        offset = copoffsetb;
    } else if (whichColor == 0) {
        offset = copoffsetr;
    } else if (whichColor == 2) {
        offset = copoffsetw;
    }

    flicker = bSin(time * 24.0f + 1.5707964f);
    flicker *= flicker;

    if (whichColor == 2) {
        return flicker * coplightflicker(time, flarecount);
    }

    if (bSin(time * 8.0f + offset + 1.5707964f) > 0.2f) {
        return flicker;
    }

    return 0.0f;
}

float TireFace(bMatrix4 *matrix, eView *view) {
    float face = 1.0f;

    if (TireFaceIt != 0) {
        CameraPositionAccess *camera = reinterpret_cast<CameraPositionAccess *>(view->pCamera);
        bMatrix4 local_matrix = *matrix;
        bVector3 normal;

        normal.x = enX;
        normal.y = enY;
        normal.z = enZ;
        eMulVector(&normal, &local_matrix, &normal);
        face = normal.x * camera->x + normal.y * camera->y + normal.z * camera->z;
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
    CarRenderUsedCarTextureInfoLayout *used_texture_info =
        reinterpret_cast<CarRenderUsedCarTextureInfoLayout *>(&this->mUsedTextureInfos);

    this->pRideInfo = ride_info;
    GetUsedCarTextureInfo(&this->mUsedTextureInfos, ride_info, 0);

    this->MasterReplacementTextureTable[REPLACETEX_CARSKIN].SetNewNameHash(used_texture_info->ReplaceSkinHash);
    this->MasterReplacementTextureTable[REPLACETEX_CARSKINB].SetNewNameHash(used_texture_info->ReplaceSkinBHash);
    this->MasterReplacementTextureTable[REPLACETEX_WHEEL].SetNewNameHash(used_texture_info->ReplaceWheelHash);
    this->MasterReplacementTextureTable[REPLACETEX_SPINNER].SetNewNameHash(used_texture_info->ReplaceSpinnerHash);
    this->MasterReplacementTextureTable[REPLACETEX_SPOILER].SetNewNameHash(used_texture_info->ReplaceSpoilerHash);
    this->MasterReplacementTextureTable[REPLACETEX_ROOF_SCOOP].SetNewNameHash(used_texture_info->ReplaceRoofScoopHash);
    this->MasterReplacementTextureTable[REPLACETEX_GLOBALSKIN].SetNewNameHash(used_texture_info->ReplaceGlobalHash);
    this->MasterReplacementTextureTable[REPLACETEX_CARBONSKIN].SetNewNameHash(used_texture_info->ReplaceGlobalHash);
    this->MasterReplacementTextureTable[REPLACETEX_GLOBALCARBONSKIN].SetNewNameHash(used_texture_info->ReplaceGlobalHash);

    this->UpdateCarReplacementTextures();
    this->BrakeLeftReplacementTextureTable[1].SetNewNameHash(used_texture_info->ReplaceGlobalHash);
    this->BrakeRightReplacementTextureTable[1].SetNewNameHash(used_texture_info->ReplaceGlobalHash);
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
    CarRenderUsedCarTextureInfoLayout *used_texture_info =
        reinterpret_cast<CarRenderUsedCarTextureInfoLayout *>(&this->mUsedTextureInfos);
    unsigned int headlights_on = used_texture_info->ReplaceHeadlightHash[1];
    unsigned int headlight_glass_on = used_texture_info->ReplaceHeadlightGlassHash[1];
    unsigned int window_front = this->MasterReplacementTextureTable[REPLACETEX_WINDOW_FRONT].GetNewNameHash();
    int left_brakelight_on = 0;
    int right_brakelight_on = 0;
    int center_brakelight_on = 0;

    this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_LEFT].SetNewNameHash(headlights_on);
    this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_RIGHT].SetNewNameHash(headlights_on);
    this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_GLASS_LEFT].SetNewNameHash(headlight_glass_on);
    this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_GLASS_RIGHT].SetNewNameHash(headlight_glass_on);
    this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_LEFT].SetNewNameHash(headlights_on);
    this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_RIGHT].SetNewNameHash(headlights_on);
    this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_LEFT].SetNewNameHash(headlight_glass_on);
    this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_RIGHT].SetNewNameHash(headlight_glass_on);
    this->CarbonReplacementTextureTable[REPLACETEX_HEADLIGHT_LEFT].SetNewNameHash(headlights_on);
    this->CarbonReplacementTextureTable[REPLACETEX_HEADLIGHT_RIGHT].SetNewNameHash(headlights_on);
    this->CarbonReplacementTextureTable[REPLACETEX_HEADLIGHT_GLASS_LEFT].SetNewNameHash(headlight_glass_on);
    this->CarbonReplacementTextureTable[REPLACETEX_HEADLIGHT_GLASS_RIGHT].SetNewNameHash(headlight_glass_on);
    this->CarbonReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_LEFT].SetNewNameHash(headlights_on);
    this->CarbonReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_RIGHT].SetNewNameHash(headlights_on);
    this->CarbonReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_LEFT].SetNewNameHash(headlight_glass_on);
    this->CarbonReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_RIGHT].SetNewNameHash(headlight_glass_on);
    this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_GLASS_LEFT].SetNewNameHash(window_front);
    this->MasterReplacementTextureTable[REPLACETEX_HEADLIGHT_GLASS_RIGHT].SetNewNameHash(window_front);
    this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_LEFT].SetNewNameHash(window_front);
    this->MasterReplacementTextureTable[REPLACETEX_OLD_HEADLIGHT_GLASS_RIGHT].SetNewNameHash(window_front);

    if (((this->mBrokenLights & 8) == 0) && ((this->mOnLights & 8) != 0)) {
        left_brakelight_on = 1;
    }
    if (((this->mBrokenLights & 0x10) == 0) && ((this->mOnLights & 0x10) != 0)) {
        right_brakelight_on = 1;
    }
    if (((this->mBrokenLights & 0x20) == 0) && ((this->mOnLights & 0x20) != 0)) {
        center_brakelight_on = 1;
    }
    if (ForceBrakelightsOn != 0) {
        left_brakelight_on = 1;
        right_brakelight_on = 1;
    }

    this->MasterReplacementTextureTable[REPLACETEX_BRAKELIGHT_LEFT].SetNewNameHash(used_texture_info->ReplaceBrakelightHash[left_brakelight_on]);
    this->MasterReplacementTextureTable[REPLACETEX_BRAKELIGHT_RIGHT].SetNewNameHash(used_texture_info->ReplaceBrakelightHash[right_brakelight_on]);
    this->MasterReplacementTextureTable[REPLACETEX_BRAKELIGHT_CENTRE].SetNewNameHash(used_texture_info->ReplaceBrakelightHash[center_brakelight_on]);
    this->MasterReplacementTextureTable[REPLACETEX_BRAKELIGHT_GLASS_LEFT].SetNewNameHash(used_texture_info->ReplaceBrakelightGlassHash[left_brakelight_on]);
    this->MasterReplacementTextureTable[REPLACETEX_BRAKELIGHT_GLASS_RIGHT].SetNewNameHash(used_texture_info->ReplaceBrakelightGlassHash[right_brakelight_on]);
    this->MasterReplacementTextureTable[REPLACETEX_BRAKELIGHT_GLASS_CENTRE].SetNewNameHash(used_texture_info->ReplaceBrakelightGlassHash[center_brakelight_on]);
    this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_LEFT].SetNewNameHash(used_texture_info->ReplaceBrakelightHash[left_brakelight_on]);
    this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_RIGHT].SetNewNameHash(used_texture_info->ReplaceBrakelightHash[right_brakelight_on]);
    this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_CENTRE].SetNewNameHash(used_texture_info->ReplaceBrakelightHash[center_brakelight_on]);
    this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_GLASS_LEFT]
        .SetNewNameHash(used_texture_info->ReplaceBrakelightGlassHash[left_brakelight_on]);
    this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_GLASS_RIGHT]
        .SetNewNameHash(used_texture_info->ReplaceBrakelightGlassHash[right_brakelight_on]);
    this->MasterReplacementTextureTable[REPLACETEX_OLD_BRAKELIGHT_GLASS_CENTRE]
        .SetNewNameHash(used_texture_info->ReplaceBrakelightGlassHash[center_brakelight_on]);
}

void UpdateEnvironmentMapCameras() {
    bVector3 *car_world_position = nullptr;
    eView *view = eGetView(1, false);

    if (view->GetCameraMover() != nullptr) {
        CameraAnchor *anchor = view->GetCameraMover()->GetAnchor();

        if (anchor == nullptr) {
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
        } else {
            car_world_position = &reinterpret_cast<CameraAnchorLayout *>(anchor)->mGeomPos;
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
    const UTL::Collections::Listable<VehicleRenderConn, 10>::List &loader_list = VehicleRenderConn::GetList();
    UTL::Collections::Listable<VehicleRenderConn, 10>::List::const_iterator it = loader_list.begin();
    UTL::Collections::Listable<VehicleRenderConn, 10>::List::const_iterator end = loader_list.end();

    for (; it != end; ++it) {
        VehicleRenderConn *vehicle_render_conn = *it;

        if ((type == static_cast<CarType>(-1) || vehicle_render_conn->mCarType == type) && vehicle_render_conn->mState > 1) {
            vehicle_render_conn->RefreshRenderInfo();
        }
    }

    RefreshAllFrontEndCarRenderInfos(type);
}

void RenderFEFlares(eView *, int) {}

void RenderFrontEndCars(eView *view, int reflection) {
    if (DrawCars != 0) {
        bool reflection_pass = reflection != 0;

        if (reflection_pass) {
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

                if (reflection_pass) {
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
                                    front_end_car->TireMatrices, reflection_pass, 0, reflection, 1.0f, lod, lod);
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

    n = ch2d(P, n);
    if (wcoll != nullptr) {
        bVector3 *vec;

        this->mWorldPos.SetTolerance(lbl_8040AD70);
        if (fast != 0) {
            vec = hullVertArray2;
            dec = 0;
            bPointValid = true;

            for (i = 0; i < n; i++) {
                bFill(vec, P[i]->x, P[i]->y, Z);

                eUnSwizzleWorldVector(*vec, usPoint);
                this->mWorldPos.FindClosestFace(wcoll, reinterpret_cast<const UMath::Vector3 &>(usPoint), bPointValid);
                if (!this->mWorldPos.OnValidFace()) {
                    vec->z = this->mWorldPos.HeightAtPoint(reinterpret_cast<const UMath::Vector3 &>(usPoint));
                }

                bVector3 dotVec = *vec - hull_Origin;
                float dot = bDot(&dotVec, &hull_Normal);
                if (dot < lbl_8040AD74) {
                    dot = -dot;
                }

                bPointValid = zBias > dot;
                if (!bPointValid) {
                    dec++;
                } else {
                    vec++;
                }
            }
        } else {
            float fastZ = lbl_8040AD74;

            vec = hullVertArray2;
            vec->x = P[0]->x;
            vec->y = P[0]->y;
            vec->z = Z;

            eUnSwizzleWorldVector(*vec, usPoint);
            this->mWorldPos.FindClosestFace(wcoll, reinterpret_cast<const UMath::Vector3 &>(usPoint), true);
            if (!this->mWorldPos.OnValidFace()) {
                fastZ = this->mWorldPos.HeightAtPoint(reinterpret_cast<const UMath::Vector3 &>(usPoint));
            }

            dec = 0;
            for (i = 0; i < n; i++) {
                vec->x = P[i]->x;
                vec->y = P[i]->y;
                vec->z = fastZ;

                bVector3 dotVec = *vec - hull_Origin;
                float dot = bDot(&dotVec, &hull_Normal);
                if (dot < lbl_8040AD74) {
                    dot = -dot;
                }

                if (zBias <= dot) {
                    dec++;
                } else {
                    vec++;
                }
            }
        }

        n -= dec;
    }
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
            int centerIndex = ((((n - (n >> 31)) * 8) & ~0xF) >> 4);
            bVector3 shadowCenter = shadowVertices[0] + shadowVertices[centerIndex];
            int alpha = static_cast<int>((lbl_8040ADA0 - car_elevation_scale) * lbl_8040ADB0);
            unsigned int colour;

            shadowCenter *= lbl_8040ADA8;
            FancyCarShadowEdgeMult = car_elevation_scale * lbl_8040ADB4 + lbl_8040ADB8;
            if (alpha < 0) {
                alpha = 0;
            }
            if (alpha > 0xFE) {
                alpha = 0xFE;
            }
            colour = static_cast<unsigned int>(alpha << 24) | 0x00808080;

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

                        section++;
                        if (section > 2) {
                            nSubVerts = n - startIndex;
                        }

                        if (exBeginStrip(this->ShadowRampTexture, (nSubVerts + 1) * 2, biasedIdentity)) {
                            int endIndex = startIndex + nSubVerts;
                            int loopIndex = startIndex;

                            for (; loopIndex < endIndex; loopIndex++) {
                                bVector3 *edge = &shadowVertices[loopIndex];
                                bVector3 inner(*edge);

                                inner.x = FancyCarShadowEdgeMult * (edge->x - shadowCenter.x) + shadowCenter.x;
                                inner.y = FancyCarShadowEdgeMult * (edge->y - shadowCenter.y) + shadowCenter.y;
                                exAddVertex(*edge);
                                exAddVertex(inner);
                                exAddColour(colour);
                                exAddColour(colour);
                                exAddUV(lbl_8040ADBC, lbl_8040ADAC);
                                exAddUV(lbl_8040ADA0, lbl_8040ADAC);
                            }

                            if (n <= loopIndex) {
                                loopIndex = 0;
                            }

                            {
                                bVector3 *edge = &shadowVertices[loopIndex];
                                bVector3 inner(*edge);

                                inner.x = FancyCarShadowEdgeMult * (edge->x - shadowCenter.x) + shadowCenter.x;
                                inner.y = FancyCarShadowEdgeMult * (edge->y - shadowCenter.y) + shadowCenter.y;
                                exAddVertex(*edge);
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
