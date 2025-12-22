#include "./CarRender.hpp"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Ecstasy/eModel.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "dolphin/types.h"

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
SlotPool *CarPartModelPool;

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
        
        position->z += this->RenderInfo->GetAttributes().FECompressions(index);
        position->w = 1.0f;

        return true;
    } else {
        return false;
    }
}
