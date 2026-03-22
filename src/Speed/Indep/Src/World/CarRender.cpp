#include "./CarRender.hpp"
#include "Interfaces/IVehicleDamageBehaviour.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/DefragFixer.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Ecstasy/eModel.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/ecar.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/World/VehicleRenderConn.h"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

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
extern unsigned int TireFaceIt;
extern int ForceBrakelightsOn;
extern int counter_31665 asm("counter.31665");
extern int counter_31669 asm("counter.31669");

class VehicleFragmentConn {
  public:
    static void FetchData(float dT);
};

namespace {

void Render(eViewPlatInterface *view, eModel *model, bMatrix4 *local_to_world, eLightContext *light_context, unsigned int flags,
            unsigned int exc_flag);

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

void CarRender_Service(float dT) {
    VehicleRenderConn::FetchData(dT);
    VehicleFragmentConn::FetchData(dT);
}
