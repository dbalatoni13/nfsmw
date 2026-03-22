#include "./CarRender.hpp"
#include "Interfaces/IVehicleDamageBehaviour.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/DefragFixer.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Ecstasy/eModel.hpp"
#include "Speed/Indep/Src/Ecstasy/eSolid.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/ecar.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
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

float culldiv = 12000.0f;
static const CarFXPosInfo FXMarkerNameHashMappings[28] = {
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0, 0, 0, 0 }, 255 },
    { { 0xA5B28001, 0xBCF8A18B, 0xBD7CF15E, 0 }, 3 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
    { { 0, 0, 0, 0 }, 0 },
};

SlotPool *CarEmitterPositionSlotPool = nullptr;
const int MAX_CAR_PART_MODELS = 250;
SlotPool *CarPartModelPool = nullptr;

struct eEnvMap {
    void UpdateCameras(bVector3 *viewer_world_position, bVector3 *envmap_world_position);
};

void GetUsedCarTextureInfo(UsedCarTextureInfo *used_texture_info, RideInfo *ride_info, int front_end_only);
extern float copm;
extern float copt;
extern int copModulo;
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
extern int TweakKitWheelOffsetFront;
extern int TweakKitWheelOffsetRear;
extern int ForceBrakelightsOn;
extern int iRam8047ff04;
extern bVector3 EnvMapEyeOffset;
extern bVector3 EnvMapCamOffset;
extern float lbl_8040AD70;
extern float lbl_8040AD74;
extern float lbl_8040AD78;
extern float lbl_8040AD7C;
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
extern bVector3 hull_Origin asm("hull_Origin");
extern bVector3 hull_Normal asm("hull_Normal");
extern bVector3 hullVertArray1[16] asm("hullVertArray1");
extern bVector3 hullVertArray2[16] asm("hullVertArray2");
extern bVector3 hullVertArray3[48] asm("hullVertArray3");
extern bVector4 PointCloud[16] asm("PointCloud");
extern bVector3 *P[17] asm("P");
extern int ch2d(bVector3 **P, int n) asm("ch2d__FPPfi");
extern float FancyCarShadowEdgeMult;
extern float car_elevation_scale;
extern int dshad;
extern bVector3 cs_lightV asm("cs_lightV");
extern float cs_OneOverZ asm("cs_OneOverZ");
extern int counter_31665 asm("counter.31665");
extern int counter_31669 asm("counter.31669");
extern void sh_Setup(bVector3 *car_pos) asm("sh_Setup__FP8bVector3");

namespace {

void Render(eViewPlatInterface *view, eModel *model, bMatrix4 *local_to_world, eLightContext *light_context, unsigned int flags,
            unsigned int exc_flag);
void Render(eViewPlatInterface *view, ePoly *poly, TextureInfo *texture_info, bMatrix4 *matrix, int accurate, float z_bias);
eEnvMap *eGetEnvMap();
void bExpandBoundingBox(bVector3 *bbox_min, bVector3 *bbox_max, const bVector3 *bbox2_min, const bVector3 *bbox2_max);
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

    node_layout.Next = end;
    if (layout.Head == end) {
        layout.Head = node;
    } else {
        reinterpret_cast<bSNodeLayout<CarEmitterPosition> &>(*layout.Tail).Next = node;
    }
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

        if (IsGameFlowInFrontEnd()) continue;

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

                    for (;n < debug_print; n++) {
                        bVector3 normal = plane_info->Normal[n];                        
                        if (pCurrentPartCullingPlaneInfo != nullptr) {
                            bNormalize(&normal, &normal);
                        }
                        debug_print = plane_info->NumPlanes;
                        
                        float distance = bDot(&v, &normal) - plane_info->Normal[n].x;
                        if (distance < 0.0f) break;
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
    { "CARFX_NONE", 0 },
    { "CARFX_DRIVE_OVER", 0 },
    { "CARFX_DRIVE_OVER2", 0 },
    { "CARFX_SKID_SMOKE", 0 },
    { "CARFX_TIRE_SPEW", 0 },
    { "CARFX_BOTTOM_OUT", 0 },
    { "CARFX_DAM_RADIATOR", 0 },
    { "CARFX_DAM_ENGINE", 0 },
    { "CARFX_BLOWN_TIRE", 0 },
    { "CARFX_DRIVE_ON_FLAT_TIRE", 0 },
    { "CARFX_NITRO", 0 },
    { "CARFX_EXHAUST_SMOKE", 0 },
    { "CARFX_EXHAUST_BLOWOFF", 0 },
    { "CARFX_NOS_BLOWOFF", 0 },
    { "CARFX_BREAK_SIDE_MIRROR_LEFT", 0 },
    { "CARFX_BREAK_SIDE_MIRROR_RIGHT", 0 },
    { "CARFX_BREAK_LICENSE_PLATE_FRONT", 0 },
    { "CARFX_BREAK_LICENSE_PLATE_RIGHT", 0 },
    { "CARFX_BREAK_HEADLIGHT_LEFT", 0 },
    { "CARFX_BREAK_HEADLIGHT_RIGHT", 0 },
    { "CARFX_BREAK_BRAKELIGHT_LEFT", 0 },
    { "CARFX_BREAK_BRAKELIGHT_RIGHT", 0 },
    { "CARFX_BREAK_BRAKELIGHT_CENTRE", 0 },
    { "CARFX_BREAK_WINDSHIELD", 0 },
    { "CARFX_BREAK_WINDOW_REAR", 0 },
    { "CARFX_BREAK_WINDOW_LEFT_FRONT", 0 },
    { "CARFX_BREAK_WINDOW_LEFT_REAR", 0 },
    { "CARFX_BREAK_WINDOW_RIGHT_FRONT", 0 },
    { "CARFX_BREAK_WINDOW_RIGHT_REAR", 0 },
};

ePointSprite3D TestSprite;

bMatrix4 LeftTireRotateZMatrix;
bMatrix4 LeftTireMirrorMatrix;

TextureInfo * pTextureInfo2PlayerHeadlight1;

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

bool FrontEndRenderingCar::LookupWheelRadius(unsigned int index /* r4 */, float & radius /* r5 */) {
    if (this->RenderInfo == nullptr) return false;
    
    UMath::Vector4 pos;
    this->RenderInfo->GetAttributes().TireOffsets(pos, index);
    radius = pos.w;

    return true;
}

void FrontEndRenderingCar::ReInit(struct RideInfo * ride_info /* r31 */) {
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
    : mDamageBehaviour(nullptr), mWorldPos(0.025f),
    mAttributes(Attrib::FindCollection(this->GetAttributes().ClassKey(), 0xeec2271a), 0, nullptr)
{
    // ...
    
    { //?
        eModel *front_wheel_model = this->mCarPartModels[this->mMinLodLevel][0][1].GetModel();
        eModel *rear_wheel_model  = this->mCarPartModels[this->mMinLodLevel][0][2].GetModel();

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
        
        eModel *base_model = this->mCarPartModels[this->mMinLodLevel][0][1].GetModel();
        if (base_model != nullptr) {
            unsigned int open_string_hashes[4] = { 0xF91BCA96, 0x8DE14C29, 0x60989ECA, 0xD0F2CD17 };
            unsigned int closed_string_hashes[4] = { 0x58A2A425, 0x8FE91DD8, 0x05C907D9, 0x7B3CD206 };

            for (int i = 0; i < 4; i++) {
                ePositionMarker *open_marker = base_model->GetPostionMarker(open_string_hashes[i]);
                ePositionMarker *closed_marker = base_model->GetPostionMarker(closed_string_hashes[i]);

                if (open_marker == nullptr || closed_marker == nullptr) continue;

                open_marker->Matrix = NISCopCarDoorOpenMarkers[i];
                closed_marker->Matrix = NISCopCarDoorClosedMarkers[i];
            }
        }
    }

    if (
        this->pRideInfo->Type == CARTYPE_COPMIDSIZE    ||
        this->pRideInfo->Type == CARTYPE_COPSUV        ||
        this->pRideInfo->Type == CARTYPE_COPSPORT      ||
        this->pRideInfo->Type == CARTYPE_COPGHOST      ||
        this->pRideInfo->Type == CARTYPE_COPGTO        ||
        this->pRideInfo->Type == CARTYPE_COPSUVL       ||
        this->pRideInfo->Type == CARTYPE_COPGTOGHOST   ||
        this->pRideInfo->Type == CARTYPE_COPSPORTHENCH ||
        this->pRideInfo->Type == CARTYPE_COPSPORTGHOST
    ) {
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
                if (model == nullptr) break;
                if (model->GetNameHash() == 0) break;

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
    this->SpoilerPositionMarker   = reinterpret_cast<ePositionMarker *>(gDefragFixer.Fix(this->SpoilerPositionMarker  ));
    this->SpoilerPositionMarker2  = reinterpret_cast<ePositionMarker *>(gDefragFixer.Fix(this->SpoilerPositionMarker2 ));
    this->RoofScoopPositionMarker = reinterpret_cast<ePositionMarker *>(gDefragFixer.Fix(this->RoofScoopPositionMarker));

    for (int fxpos = 0; fxpos < 0x1C; fxpos++) {
        for (CarEmitterPosition *empos = this->EmitterPositionList[fxpos].GetHead(); empos != this->EmitterPositionList[fxpos].EndOfList(); empos = empos->GetNext()) {
            if (empos->PositionMarker != nullptr) {
                empos->PositionMarker = reinterpret_cast<ePositionMarker *>(gDefragFixer.Fix(empos->PositionMarker));
            }
        };
    }
}

void CarRenderInfo::SetPlayerDamage(const DamageZone::Info &damageInfo) {
    if (this->mDamageInfoCache.Value == damageInfo.Value) return;
    
    const unsigned int kDamageThresh = 0;
    const unsigned int kDamageWindowThresh = 1;

    this->mDamageInfoCache.Value = damageInfo.Value;

    this->SetCarDamageState(damageInfo.Get(DamageZone::DZ_FRONT ) > kDamageThresh, 0x2E, 0x2E);
    this->SetCarDamageState(damageInfo.Get(DamageZone::DZ_REAR  ) > kDamageThresh, 0x31, 0x31);
    this->SetCarDamageState(damageInfo.Get(DamageZone::DZ_LFRONT) > kDamageThresh, 0x2F, 0x2F);
    this->SetCarDamageState(damageInfo.Get(DamageZone::DZ_RFRONT) > kDamageThresh, 0x30, 0x30);
    this->SetCarDamageState(damageInfo.Get(DamageZone::DZ_LREAR ) > kDamageThresh, 0x32, 0x32);
    this->SetCarDamageState(damageInfo.Get(DamageZone::DZ_RREAR ) > kDamageThresh, 0x33, 0x33);
    
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_FRONT ) > kDamageWindowThresh, REPLACETEX_WINDOW_FRONT, bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_REAR  ) > kDamageWindowThresh, REPLACETEX_WINDOW_REAR, bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_LFRONT) > kDamageWindowThresh, REPLACETEX_WINDOW_LEFT_FRONT, bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_LREAR ) > kDamageWindowThresh, REPLACETEX_WINDOW_LEFT_REAR, bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_RFRONT) > kDamageWindowThresh, REPLACETEX_WINDOW_RIGHT_FRONT, bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_RREAR ) > kDamageWindowThresh, REPLACETEX_WINDOW_RIGHT_REAR, bStringHash("WINDOW_FRONT"), 0xa155545);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_REAR  ) > kDamageWindowThresh, REPLACETEX_WINDOW_REAR_DEFOST, bStringHash("REAR_DEFROSTER"), 0xa155545);
}


void CarRenderInfo::SetCarDamageState(bool on, unsigned int startID, unsigned int endID) {
    bool hidden = !on;

    for (int model_number = 0; model_number < 1; model_number++) {
        for (int model_lod = this->mMinLodLevel; model_lod <= this->mMaxLodLevel; model_lod++) {
            for (unsigned int i = startID; i <= endID; i++) {
                CarPartModel* model = &this->mCarPartModels[i][model_number][model_lod];
                
                if (!model) continue;
                if (!model->GetModel()) continue;
                
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

void CarRenderInfo::SetDamageInfo(const DamageZone::Info& damageInfo) {
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

void CarRenderInfo::UpdateCarParts() {
    RideInfo *ride_info = this->pRideInfo;
    bVector3 bbox_min;
    bVector3 bbox_max;

    bInitializeBoundingBox(&this->AABBMin, &this->AABBMax);

    for (int slot_id = 0; slot_id < 0x4C; slot_id++) {
        for (int model_number = 0; model_number < 1; model_number++) {
            for (int lod = this->mMinLodLevel; lod <= this->mMaxLodLevel; lod++) {
                CarPartModel *car_part_model = &this->mCarPartModels[slot_id][model_number][lod];
                eModel *model = car_part_model->GetModel();

                if (model != 0 && model->GetSolid() != 0) {
                    model->UnInit();
                    CarPartModelPool->Free(model);
                    car_part_model->SetModel(0);
                    car_part_model->Clear();
                }
            }
        }
    }

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

                if (ride_info->GetSpecialLODRangeForCarSlot(slot_id, &special_minimum, &special_maximum, iRam8047ff04 == 3)) {
                    if (model_lod < special_minimum) {
                        model_lod = special_minimum;
                    }
                    if (special_maximum < model_lod) {
                        model_lod = special_maximum;
                    }
                }

                unsigned int model_name_hash = CarPart_GetModelNameHash(car_part, model_number, model_lod);

                if (model_name_hash == 0) {
                    continue;
                }

                CarPartModel *car_part_model = &this->mCarPartModels[slot_id][model_number][lod];
                eModel *model = static_cast<eModel *>(CarPartModelPool->Malloc());

                car_part_model->SetModel(model);
                model->Init(model_name_hash);

                if (model->GetSolid() == 0) {
                    model->UnInit();
                    CarPartModelPool->Free(model);
                    car_part_model->SetModel(0);
                    continue;
                }

                if (slot_id >= CARSLOTID_DECAL_FRONT_WINDOW && slot_id <= CARSLOTID_DECAL_RIGHT_QUARTER) {
                    model->AttachReplacementTextureTable(&this->DecalReplacementTextureTable[(slot_id - CARSLOTID_DECAL_FRONT_WINDOW) * 8], 8, 0);
                } else if (slot_id == CARSLOTID_HOOD) {
                    if (CarPart_GetAppliedAttributeIParam(car_part, 0x721AFF7C, 0) == 0) {
                        model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                        this->CarbonHood = 0;
                    } else {
                        model->AttachReplacementTextureTable(this->CarbonReplacementTextureTable, REPLACETEX_NUM, 0);
                        this->CarbonHood = 1;
                    }
                } else {
                    model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                }
            }

            eModel *model = this->mCarPartModels[slot_id][model_number][this->mMinLodLevel].GetModel();

            if (model != 0) {
                model->GetBoundingBox(&bbox_min, &bbox_max);
                bExpandBoundingBox(&this->AABBMin, &this->AABBMax, &bbox_min, &bbox_max);
            }
        }
    }

    for (int model_number = 0; model_number < 1; model_number++) {
        for (int lod = this->mMinLodLevel; lod <= this->mMaxLodLevel; lod++) {
            eModel *front_wheel_model = this->mCarPartModels[CARSLOTID_FRONT_WHEEL][model_number][lod].GetModel();
            eModel *rear_wheel_model = this->mCarPartModels[CARSLOTID_REAR_WHEEL][model_number][lod].GetModel();

            if (front_wheel_model != 0 && rear_wheel_model == 0) {
                rear_wheel_model = static_cast<eModel *>(CarPartModelPool->Malloc());
                this->mCarPartModels[CARSLOTID_REAR_WHEEL][model_number][lod].SetModel(rear_wheel_model);
                rear_wheel_model->Init(front_wheel_model->GetNameHash());

                if (rear_wheel_model->GetSolid() == 0) {
                    rear_wheel_model->UnInit();
                    CarPartModelPool->Free(rear_wheel_model);
                    this->mCarPartModels[CARSLOTID_REAR_WHEEL][model_number][lod].SetModel(0);
                } else {
                    rear_wheel_model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                }
            }

            eModel *front_brake_model = this->mCarPartModels[CARSLOTID_FRONT_BRAKE][model_number][lod].GetModel();
            eModel *rear_brake_model = this->mCarPartModels[CARSLOTID_REAR_BRAKE][model_number][lod].GetModel();

            if (front_brake_model != 0 && rear_brake_model == 0) {
                rear_brake_model = static_cast<eModel *>(CarPartModelPool->Malloc());
                this->mCarPartModels[CARSLOTID_REAR_BRAKE][model_number][lod].SetModel(rear_brake_model);
                rear_brake_model->Init(front_brake_model->GetNameHash());

                if (rear_brake_model->GetSolid() == 0) {
                    rear_brake_model->UnInit();
                    CarPartModelPool->Free(rear_brake_model);
                    this->mCarPartModels[CARSLOTID_REAR_BRAKE][model_number][lod].SetModel(0);
                } else {
                    rear_brake_model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                }
            }
        }
    }

    eModel *front_wheel_model = this->mCarPartModels[CARSLOTID_FRONT_WHEEL][0][this->mMinLodLevel].GetModel();
    eModel *rear_wheel_model = this->mCarPartModels[CARSLOTID_REAR_WHEEL][0][this->mMinLodLevel].GetModel();

    if (front_wheel_model != 0) {
        front_wheel_model->GetBoundingBox(&bbox_min, &bbox_max);
        this->WheelWidths[0] = fabsf(bbox_max.y - bbox_min.y);
        this->WheelRadius[0] = fabsf(bbox_max.z - bbox_min.z) * 0.5f;
    }

    if (rear_wheel_model != 0) {
        rear_wheel_model->GetBoundingBox(&bbox_min, &bbox_max);
        this->WheelWidths[1] = fabsf(bbox_max.y - bbox_min.y);
        this->WheelRadius[1] = fabsf(bbox_max.z - bbox_min.z) * 0.5f;
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
        eSolid *solid = eFindSolid(CarPart_GetModelNameHash(base_part, 0, this->mMinLodLevel), 0);

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
    this->mMirrorLeftWheels = true;
    if (spoiler_part != 0) {
        this->mMirrorLeftWheels = (reinterpret_cast<CarPartMetaLayout *>(spoiler_part)->GroupNumber_UpgradeLevel >> 5) == 0;
    }

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
    int count = 0;

    InitSList(markers_out);

    if (this->pCarTypeInfo == nullptr) {
        return 0;
    }

    for (int slot_model_index = 0; slot_model_index < 0x4C; slot_model_index++) {
        eModel *model = this->mCarPartModels[slot_model_index][0][this->mMinLodLevel].GetModel();
        ePositionMarker *position_marker = nullptr;

        if (model == nullptr) {
            continue;
        }

        while ((position_marker = model->GetPostionMarker(position_marker)) != nullptr) {
            unsigned int position_marker_namehash = position_marker->NameHash;

            for (int i = 0; i < num_pos_name_hashes; i++) {
                if (position_marker_namehash == position_name_hashes[i]) {
                    CarEmitterPosition *empos = static_cast<CarEmitterPosition *>(bOMalloc(CarEmitterPositionSlotPool));

                    empos->X = position_marker->Matrix.v3.x;
                    empos->Y = position_marker->Matrix.v3.y;
                    empos->Z = position_marker->Matrix.v3.z;
                    empos->PositionMarker = position_marker;
                    AddTailEmitterPosition(markers_out, empos);
                    count++;
                }
            }
        }
    }

    return count;
}

void CarRenderInfo::InitEmitterPositions(bVector4 *tire_positions) {
    if (this->pCarTypeInfo != nullptr && !this->mEmitterPositionsInitialized) {
        for (int i = 0; i < NUM_CARFXPOS; i++) {
            int num_pos_name_hashes = 0;
            bSList<CarEmitterPosition> &markers = this->EmitterPositionList[i];

            InitSList(markers);

            if (GetNumCarEffectMarkerHashes(static_cast<CarEffectPosition>(i), num_pos_name_hashes)) {
                if (num_pos_name_hashes > 0) {
                    this->GetEmitterPositions(markers, GetCarEffectMarkerHashes(static_cast<CarEffectPosition>(i)), num_pos_name_hashes);
                }
                continue;
            }

            CarEmitterPosition *empos = nullptr;
            switch (i) {
                case CARFXPOS_NONE:
                    empos = static_cast<CarEmitterPosition *>(bOMalloc(CarEmitterPositionSlotPool));
                    empos->X = 0.0f;
                    empos->Y = 0.0f;
                    empos->Z = 0.0f;
                    break;
                case CARFXPOS_FRONT_TIRES:
                    empos = static_cast<CarEmitterPosition *>(bOMalloc(CarEmitterPositionSlotPool));
                    empos->X = (tire_positions[0].x + tire_positions[1].x) * 0.5f;
                    empos->Y = (tire_positions[0].y + tire_positions[1].y) * 0.5f;
                    empos->Z = (tire_positions[0].z + tire_positions[1].z) * 0.5f;
                    break;
                case CARFXPOS_REAR_TIRES:
                    empos = static_cast<CarEmitterPosition *>(bOMalloc(CarEmitterPositionSlotPool));
                    empos->X = (tire_positions[2].x + tire_positions[3].x) * 0.5f;
                    empos->Y = (tire_positions[2].y + tire_positions[3].y) * 0.5f;
                    empos->Z = (tire_positions[2].z + tire_positions[3].z) * 0.5f;
                    break;
                case CARFXPOS_LEFT_TIRES:
                    empos = static_cast<CarEmitterPosition *>(bOMalloc(CarEmitterPositionSlotPool));
                    empos->X = (tire_positions[0].x + tire_positions[3].x) * 0.5f;
                    empos->Y = (tire_positions[0].y + tire_positions[3].y) * 0.5f;
                    empos->Z = (tire_positions[0].z + tire_positions[3].z) * 0.5f;
                    break;
                case CARFXPOS_RIGHT_TIRES:
                    empos = static_cast<CarEmitterPosition *>(bOMalloc(CarEmitterPositionSlotPool));
                    empos->X = (tire_positions[1].x + tire_positions[2].x) * 0.5f;
                    empos->Y = (tire_positions[1].y + tire_positions[2].y) * 0.5f;
                    empos->Z = (tire_positions[1].z + tire_positions[2].z) * 0.5f;
                    break;
                case CARFXPOS_TIRE_FL:
                    empos = static_cast<CarEmitterPosition *>(bOMalloc(CarEmitterPositionSlotPool));
                    empos->X = tire_positions[0].x;
                    empos->Y = tire_positions[0].y;
                    empos->Z = tire_positions[0].z;
                    break;
                case CARFXPOS_TIRE_FR:
                    empos = static_cast<CarEmitterPosition *>(bOMalloc(CarEmitterPositionSlotPool));
                    empos->X = tire_positions[1].x;
                    empos->Y = tire_positions[1].y;
                    empos->Z = tire_positions[1].z;
                    break;
                case CARFXPOS_TIRE_RR:
                    empos = static_cast<CarEmitterPosition *>(bOMalloc(CarEmitterPositionSlotPool));
                    empos->X = tire_positions[2].x;
                    empos->Y = tire_positions[2].y;
                    empos->Z = tire_positions[2].z;
                    break;
                case CARFXPOS_TIRE_RL:
                    empos = static_cast<CarEmitterPosition *>(bOMalloc(CarEmitterPositionSlotPool));
                    empos->X = tire_positions[3].x;
                    empos->Y = tire_positions[3].y;
                    empos->Z = tire_positions[3].z;
                    break;
                case CARFXPOS_ENGINE:
                    empos = static_cast<CarEmitterPosition *>(bOMalloc(CarEmitterPositionSlotPool));
                    empos->X = (tire_positions[0].x + tire_positions[1].x) * 0.5f;
                    empos->Y = (tire_positions[0].y + tire_positions[1].y) * 0.5f;
                    empos->Z = (tire_positions[0].z + tire_positions[1].z) * 0.5f + (tire_positions[0].y - tire_positions[1].y) * 0.2f;
                    break;
            }

            if (empos != nullptr) {
                empos->PositionMarker = nullptr;
                AddTailEmitterPosition(markers, empos);
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

int GetCarLightFlareType(unsigned int name_hash, int slot_model_index, int &front_marker_slot, int &rear_marker_slot) {
    switch (name_hash) {
        case 0xD09091C6:
        case 0x9DB90133:
        case 0x7A5BCF69:
            return 0;
        case 0x31A66786:
        case 0xA2A2FC7C:
        case 0xBF700A79:
            return 1;
        case 0x1E4150B4:
            return 5;
        case 0xE662C161:
            return 6;
        case 0xB4348DBA:
            return 7;
        case 0x41489594:
            return 10;
        case 0x6A52A241:
            return 11;
        case 0x28CD78F5:
            return 12;
        case 0x7A5B2F25:
            if (front_marker_slot == slot_model_index || front_marker_slot < 1) {
                front_marker_slot = slot_model_index;
                return 3;
            }
            return -1;
        case 0x7ADF7EF8:
            if (rear_marker_slot != slot_model_index && rear_marker_slot > 0) {
                return -1;
            }
            rear_marker_slot = slot_model_index;
            return 3;
        default:
            return -1;
    }
}

void CarRenderInfo::UpdateCarReplacementTextures() {
    CarRenderUsedCarTextureInfoLayout *used_texture_info =
        reinterpret_cast<CarRenderUsedCarTextureInfoLayout *>(&this->mUsedTextureInfos);

    bMemCpy(this->CarbonReplacementTextureTable, this->MasterReplacementTextureTable, sizeof(this->CarbonReplacementTextureTable));

    this->CarbonReplacementTextureTable[REPLACETEX_CARSKIN].SetNewNameHash(bStringHash("CARBONFIBRE"));
    this->CarbonReplacementTextureTable[REPLACETEX_CARSKINB].SetNewNameHash(used_texture_info->ReplaceGlobalHash);
    this->CarbonReplacementTextureTable[REPLACETEX_GLOBALSKIN].SetNewNameHash(used_texture_info->ReplaceGlobalHash);
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
                int flare_type =
                    GetCarLightFlareType(position_marker->NameHash, slot_model_index, front_marker_slot, rear_marker_slot);

                if (flare_type != -1) {
                    eLightFlare *light_flare = static_cast<eLightFlare *>(gFastMem.Alloc(sizeof(eLightFlare), 0));

                    bMemSet(light_flare, 0, sizeof(eLightFlare));
                    light_flare->NameHash = position_marker->NameHash;
                    light_flare->ColourTint = 0;
                    light_flare->Type = static_cast<char>(flare_type);
                    light_flare->Flags =
                        static_cast<char>(((flare_type - 5U < 3) || flare_type == 10 || flare_type == 11 || flare_type == 12) ? 2 : 4);
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

    if (matrix != 0 && matrix->v2.z >= 0.707f) {
        ePoly poly;
        TextureInfo *texture_info = GetTextureInfo(bStringHash("2PLAYERHEADLIGHT1"), 1, 0);

        bMemSet(&poly, 0, sizeof(poly));

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
        float fastZ = lbl_8040AD74;

        this->mWorldPos.SetTolerance(lbl_8040AD70);
        if (fast == 0) {
            vec = hullVertArray2;
            dec = 0;
            bPointValid = true;

            for (i = 0; i < n; i++) {
                vec->x = P[i]->x;
                vec->y = P[i]->y;
                vec->z = Z;

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

                if (lbl_8040AD78 <= dot) {
                    dec++;
                } else {
                    vec++;
                }

                bPointValid = lbl_8040AD78 > dot;
            }
        } else {
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

                if (lbl_8040AD78 <= dot) {
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
        if (iRam8047ff04 == 6) {
            bVector3 worldPosition;

            worldPosition.x = position->x;
            worldPosition.y = -position->y;
            worldPosition.z = position->z;
            this->mWorldPos.FindClosestFace(this->mWCollider, reinterpret_cast<const UMath::Vector3 &>(worldPosition), false);
            if (!this->mWorldPos.OnValidFace()) {
                shadowZ = this->mWorldPos.HeightAtPoint(reinterpret_cast<const UMath::Vector3 &>(worldPosition));
            }
        }

        lightV = cs_lightV;
        scale.x = lbl_8040AD98;
        scale.y = lbl_8040AD9C;
        scale.z = lbl_8040ADA0;
        for (i = 0; i < n; i++) {
            bVector3 localPoint;
            bVector3 worldPoint;
            float scaleToGround;

            localPoint.x = PointCloud[i].x * scale.x;
            localPoint.y = PointCloud[i].y * scale.y;
            localPoint.z = PointCloud[i].z * scale.z;
            eMulVector(&worldPoint, localWorld, &localPoint);
            scaleToGround = (shadowZ - worldPoint.z) * cs_OneOverZ;
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
