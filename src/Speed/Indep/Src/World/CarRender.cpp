#include "./CarRender.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
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

const CarPartCullingPlaneInfo *pCurrentPartCullingPlaneInfo = nullptr;

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

int NISRaceDriverVisible = 1;

void SetNISRaceDriverVisible(int visible /* r3 */) {
    NISRaceDriverVisible = visible;
}
