#include "./CarRender.hpp"
#include "Interfaces/IVehicleDamageBehaviour.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/DefragFixer.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eEnvMap.hpp"
#include "Speed/Indep/Src/Ecstasy/eStrip.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
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
#include "Speed/Indep/Src/World/Car.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/World/CarPartID.h"
#include "Speed/Indep/Src/World/Player.hpp"
#include "Speed/Indep/Src/World/Sun.hpp"
#include "Speed/Indep/Src/World/VehicleFragmentConn.h"
#include "Speed/Indep/Src/World/VehiclePartDamage.h"
#include "Speed/Indep/Src/World/VehicleRenderConn.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "VehicleFX.h"
#include <cmath>

// int32 DrawLightFlares; // why is this here in the dwarfs? shouldn't it be in eLight?

static const int RenderShaperRigLightIndicators = 0;
static const float ShaperLightsIndicatorDistanceFromCar = 1.5f;
static const float ShaperLightsIndicatorSize = 0.075f;

static const CAR_SLOT_ID BreakOnModelInitCarSlotId = CARSLOTID_BASE;
static const CAR_SLOT_ID BreakOnSetModelCarSlotId = CARSLOTID_BASE;
static const CAR_SLOT_ID BreakOnRenderCarSlotId = CARSLOTID_BASE;

// STRIPPED
const char *GetCarRenderUsageString(CarRenderUsage usage) {}

float culldiv = 12000.0f;

enum eCullableCarParts {
    CULLABLE_CAR_PART_TIRE_FL = 0,
    CULLABLE_CAR_PART_TIRE_FR = 1,
    CULLABLE_CAR_PART_TIRE_RR = 2,
    CULLABLE_CAR_PART_TIRE_RL = 3,
    CULLABLE_CAR_PART_BRAKE_FL = 4,
    CULLABLE_CAR_PART_BRAKE_FR = 5,
    CULLABLE_CAR_PART_BRAKE_RR = 6,
    CULLABLE_CAR_PART_BRAKE_RL = 7,
    CULLABLE_CAR_PART_SIDE_FRONT = 8,
    CULLABLE_CAR_PART_SIDE_REAR = 9,
    CULLABLE_CAR_PART_UNDERNEATH = 10,
    NUM_CULLABLE_CAR_PARTS = 11,
};

enum eCullingPolarity {
    CULLING_POLARITY_ANY_VISIBLE = 0,
    CULLING_POLARITY_ALL_MUST_BE_VISIBLE = 1,
};

enum CarEffectPosition {
    CARFXPOS_NONE = 0,
    CARFXPOS_FRONT_TIRES = 1,
    CARFXPOS_REAR_TIRES = 2,
    CARFXPOS_LEFT_TIRES = 3,
    CARFXPOS_RIGHT_TIRES = 4,
    CARFXPOS_TIRE_FL = 5,
    CARFXPOS_TIRE_FR = 6,
    CARFXPOS_TIRE_RR = 7,
    CARFXPOS_TIRE_RL = 8,
    CARFXPOS_ENGINE = 9,
    CARFXPOS_EXHAUST = 10,
    CARFXPOS_RADIATOR = 11,
    CARFXPOS_WINDSHIELD = 12,
    CARFXPOS_WINDOW_REAR = 13,
    CARFXPOS_WINDOW_LEFT_FRONT = 14,
    CARFXPOS_WINDOW_LEFT_REAR = 15,
    CARFXPOS_WINDOW_RIGHT_FRONT = 16,
    CARFXPOS_WINDOW_RIGHT_REAR = 17,
    CARFXPOS_HEADLIGHT = 18,
    CARFXPOS_HEADLIGHT_LEFT = 19,
    CARFXPOS_HEADLIGHT_RIGHT = 20,
    CARFXPOS_BRAKELIGHT = 21,
    CARFXPOS_BRAKELIGHT_LEFT = 22,
    CARFXPOS_BRAKELIGHT_RIGHT = 23,
    CARFXPOS_SIDE_MIRROR_LEFT = 24,
    CARFXPOS_SIDE_MIRROR_RIGHT = 25,
    CARFXPOS_LICENSE_PLATE_FRONT = 26,
    CARFXPOS_LICENSE_PLATE_REAR = 27,
    NUM_CARFXPOS = 28,
};

static const int MAX_NUM_MARKERS_PER_CARFXPOS = 0; // TODO value

struct CarFXPosInfo {
    unsigned int position_marker_hashes[4]; // offset 0x0, size 0x10
    unsigned char marker_count;             // offset 0x10, size 0x1
};

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
    {{STRINGHASH_EXHAUST, STRINGHASH_LEFT_EXHAUST, STRINGHASH_RIGHT_EXHAUST, 0}, 3},
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

bool GetNumCarEffectMarkerHashes(CarEffectPosition fx_pos, int &count_out) {
    bool position_marker_based_fxpos = false;
    count_out = 0;
    if (FXMarkerNameHashMappings[fx_pos].marker_count != 255) {
        count_out = FXMarkerNameHashMappings[fx_pos].marker_count;
        position_marker_based_fxpos = true;
    }

    return position_marker_based_fxpos;
}

const unsigned int *GetCarEffectMarkerHashes(CarEffectPosition fx_pos) {
    return reinterpret_cast<const unsigned int *>(&FXMarkerNameHashMappings[fx_pos].marker_count);
}

struct CarPartCullingPlaneInfo {
  public:
    CarPartCullingPlaneInfo(eCullableCarParts type, char *name, eCullingPolarity polarity, int numPlanes, const bVector3 &normal0,
                            const bVector3 &normal1, const bVector3 &normal2, float nd0, float nd1, float nd2) {
        this->Type = type;
        this->Name = name;
        this->Polarity = polarity;
        this->NumPlanes = numPlanes;

        this->Normal[0] = normal0;
        this->Normal[1] = normal1;
        this->Normal[2] = normal2;

        this->NormalDistance[0] = nd0;
        this->NormalDistance[1] = nd1;
        this->NormalDistance[2] = nd2;
    }

    eCullableCarParts Type;    // offset 0x0, size 0x4
    char *Name;                // offset 0x4, size 0x4
    eCullingPolarity Polarity; // offset 0x8, size 0x4
    int NumPlanes;             // offset 0xC, size 0x4
    bVector3 Normal[3];        // offset 0x10, size 0x30
    float NormalDistance[3];   // offset 0x40, size 0xC
};

struct CarPartInfo {
    bVector3 Position;       // offset 0x0, size 0x10
    int NumCulledNotVisible; // offset 0x10, size 0x4
    int NumCulledVisible;    // offset 0x14, size 0x4
    bool IsVisible;          // offset 0x18, size 0x1
};

CarPartCullingPlaneInfo CarPartCullingPlaneInfoTable[11] = {
    CarPartCullingPlaneInfo(CULLABLE_CAR_PART_TIRE_FL, "CULLABLE_CAR_PART_TIRE_FL", CULLING_POLARITY_ANY_VISIBLE, 3, bVector3(-0.01f, -1.0f, 0.0f),
                            bVector3(0.0f, 0.0f, 1.0f), bVector3(-0.707f, 0.0f, 0.707f), 0.1f, 0.42, -0.298f),
    CarPartCullingPlaneInfo(CULLABLE_CAR_PART_TIRE_FR, "CULLABLE_CAR_PART_TIRE_FR", CULLING_POLARITY_ANY_VISIBLE, 3, bVector3(-0.01f, 1.0f, 0.0f),
                            bVector3(0.0f, 0.0f, 1.0f), bVector3(-0.707f, 0.0f, 0.707f), 0.1f, 0.42f, -0.298f),
    CarPartCullingPlaneInfo(CULLABLE_CAR_PART_TIRE_RR, "CULLABLE_CAR_PART_TIRE_RR", CULLING_POLARITY_ANY_VISIBLE, 3, bVector3(0.01f, 1.0f, 0.0f),
                            bVector3(0.0f, 0.0f, 1.0f), bVector3(0.707f, 0.0f, 0.707f), 0.1f, 0.42f, -0.298f),
    CarPartCullingPlaneInfo(CULLABLE_CAR_PART_TIRE_RL, "CULLABLE_CAR_PART_TIRE_RL", CULLING_POLARITY_ANY_VISIBLE, 3, bVector3(0.01f, -1.0f, 0.0f),
                            bVector3(0.0f, 0.0f, 1.0f), bVector3(0.707f, 0.0f, 0.707f), 0.1f, 0.42f, -0.298f),
    CarPartCullingPlaneInfo(CULLABLE_CAR_PART_BRAKE_FL, "CULLABLE_CAR_PART_BRAKE_FL", CULLING_POLARITY_ALL_MUST_BE_VISIBLE, 2,
                            bVector3(0.22f, -1.0f, 0.0f), bVector3(-0.22f, -1.0f, 0.0f), bVector3(-0.22f, -1.0f, 0.0f), 0.0f, 0.05f, 0.0f),
    CarPartCullingPlaneInfo(CULLABLE_CAR_PART_BRAKE_FR, "CULLABLE_CAR_PART_BRAKE_FR", CULLING_POLARITY_ALL_MUST_BE_VISIBLE, 2,
                            bVector3(0.22f, 1.0f, 0.0f), bVector3(-0.22f, 1.0f, 0.0f), bVector3(-0.22f, -1.0f, 0.0f), 0.0f, 0.05f, 0.0f),
    CarPartCullingPlaneInfo(CULLABLE_CAR_PART_BRAKE_RR, "CULLABLE_CAR_PART_BRAKE_RR", CULLING_POLARITY_ALL_MUST_BE_VISIBLE, 2,
                            bVector3(0.22f, 1.0f, 0.0f), bVector3(-0.22f, 1.0f, 0.0f), bVector3(-0.22f, -1.0f, 0.0f), 0.0f, 0.05f, 0.0f),
    CarPartCullingPlaneInfo(CULLABLE_CAR_PART_BRAKE_RL, "CULLABLE_CAR_PART_BRAKE_RL", CULLING_POLARITY_ALL_MUST_BE_VISIBLE, 2,
                            bVector3(0.22f, -1.0f, 0.0f), bVector3(-0.22f, -1.0f, 0.0f), bVector3(-0.22f, -1.0f, 0.0f), 0.0f, 0.05f, 0.0f),
    CarPartCullingPlaneInfo(CULLABLE_CAR_PART_SIDE_FRONT, "CULLABLE_CAR_PART_SIDE_FRONT", CULLING_POLARITY_ANY_VISIBLE, 1,
                            bVector3(-1.0f, 0.0f, 0.0f), bVector3(-1.0f, 0.0f, 0.0f), bVector3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f),
    CarPartCullingPlaneInfo(CULLABLE_CAR_PART_SIDE_REAR, "CULLABLE_CAR_PART_SIDE_REAR", CULLING_POLARITY_ANY_VISIBLE, 1, bVector3(1.0f, 0.0f, 0.0f),
                            bVector3(-1.0f, 0.0f, 0.0f), bVector3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f),
    CarPartCullingPlaneInfo(CULLABLE_CAR_PART_UNDERNEATH, "CULLABLE_CAR_PART_UNDERNEATH", CULLING_POLARITY_ALL_MUST_BE_VISIBLE, 2,
                            bVector3(-0.07f, 0.0f, 1.0f), bVector3(0.07f, 0.0f, 1.0f), bVector3(0.07f, 0.0f, 1.0f), 0.05f, 0.05f, 0.0f),
};

class CarPartCuller {
  public:
    CarPartCuller() {
        bMemSet(this, 0, sizeof(*this));
    }

    void InitPart(eCullableCarParts type, const bVector3 *position);
    void CullParts(bVector3 *camera_eye, bAngle stang);
    void RenderPart(eCullableCarParts type, eView *view, const bMatrix4 *local_world, bAngle stang);

    bool IsPartVisible(eCullableCarParts type) {
        return CarPartInfoTable[type].IsVisible;
    }

  private:
    CarPartInfo CarPartInfoTable[11];
};

static const eCullableCarParts CurrentCarPartCullingType = CULLABLE_CAR_PART_TIRE_FL; // TODO fix value

const CarPartCullingPlaneInfo *pCurrentPartCullingPlaneInfo = nullptr;

static const int DisableCarPartCulling = 0;

int my_xyz = 1;
int xyzhmm = 0;
int neghmm = 0;

void CarPartCuller::InitPart(eCullableCarParts type, const bVector3 *position) {
    CarPartCullingPlaneInfo *plane_info = &CarPartCullingPlaneInfoTable[type];
    CarPartInfo *part_info = &this->CarPartInfoTable[type];

    for (int n = 0; n < plane_info->NumPlanes; n++) {
        if (bAbs(1.0f - bLength(&plane_info->Normal[n])) > 0.01f) {
            bNormalize(&plane_info->Normal[n], &plane_info->Normal[n]);
        }
    }

    part_info->Position = *position;
}

// UNSOLVED
void CarPartCuller::RenderPart(eCullableCarParts type, eView *view, const bMatrix4 *local_world, unsigned short stang) {}

// UNSOLVED, weird debug_print stuff
void CarPartCuller::CullParts(bVector3 *camera_eye, bAngle stang) {
    ProfileNode profile_node("TODO", 0);
    bVector3 Modcamera_eye = *camera_eye;
    bVector3 *unModcamera_eyeP = camera_eye;
    bMatrix4 turnMat;

    if (stang != 0) {
        eIdentity(&turnMat);
        eRotateZ(&turnMat, &turnMat, -stang);
        eMulVector(&Modcamera_eye, &turnMat, camera_eye);
    }

    for (eCullableCarParts type = CULLABLE_CAR_PART_TIRE_FL; type < NUM_CULLABLE_CAR_PARTS;
         type = static_cast<eCullableCarParts>(static_cast<int>(type) + 1)) {
        bool visible = true;
        CarPartCullingPlaneInfo *plane_info = &CarPartCullingPlaneInfoTable[type];
        CarPartInfo *part_info = &this->CarPartInfoTable[type];
        float omodifier = plane_info->NormalDistance[0];
        float modifier;

        if (!IsGameFlowInFrontEnd()) {
            // TODO
            camera_eye = (static_cast<unsigned int>(type) - 4 > 1) ? unModcamera_eyeP : &Modcamera_eye;

            if (type <= CULLABLE_CAR_PART_TIRE_FR) {
                if (stang > 0x8000) {
                    stang = -stang;
                }

                modifier = static_cast<float>(stang) / culldiv;
                plane_info->NormalDistance[0] = omodifier + modifier;
            } else {
                plane_info->NormalDistance[0] = omodifier;
            }

            if (plane_info->NumPlanes > 0) {
                bVector3 v = *camera_eye - part_info->Position;
                int debug_print = 0;

                if (plane_info->Polarity == CULLING_POLARITY_ANY_VISIBLE) {
                    debug_print = plane_info->NumPlanes;
                    int n = 0;

                    for (; n < plane_info->NumPlanes; n++) {
                        bVector3 normal = plane_info->Normal[n];

                        if (pCurrentPartCullingPlaneInfo != nullptr) {
                            bNormalize(&normal, &normal);
                        }

                        float distance = bDot(&v, &normal) - plane_info->NormalDistance[n];
                        debug_print = plane_info->NumPlanes;
                        if (distance < 0.0f) {
                            break;
                        }
                    }

                    if (n == debug_print) {
                        visible = false;
                    }
                } else if (plane_info->Polarity == CULLING_POLARITY_ALL_MUST_BE_VISIBLE) {
                    for (int n = 0; n < plane_info->NumPlanes; n++) {
                        bVector3 normal = plane_info->Normal[n];

                        if (pCurrentPartCullingPlaneInfo != nullptr) {
                            bNormalize(&normal, &normal);
                        }

                        float distance = bDot(&v, &normal) - plane_info->NormalDistance[n];
                        if (distance >= 0.0f) {
                            visible = false;
                            break;
                        }
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

extern SlotPool *CarEmitterPositionSlotPool;

const int MAX_CAR_PART_MODELS = 250;
SlotPool *CarPartModelPool = nullptr;

class CarEmitterPosition : public bSNode<CarEmitterPosition> {
  public:
    USE_SLOTALLOC(CarEmitterPositionSlotPool);

    CarEmitterPosition(ePositionMarker *position_marker) {
        PositionMarker = position_marker;
        X = position_marker->Matrix.v3.x;
        Y = position_marker->Matrix.v3.y;
        Z = position_marker->Matrix.v3.z;
    }

    CarEmitterPosition(float x, float y, float z) {
        PositionMarker = nullptr;
        X = x;
        Y = y;
        Z = z;
    }

    float X;                         // offset 0x4, size 0x4
    float Y;                         // offset 0x8, size 0x4
    float Z;                         // offset 0xC, size 0x4
    ePositionMarker *PositionMarker; // offset 0x10, size 0x4
};

uint32 CarLightsStateMap[5] = {1, 2, 4, 8, 16};
uint32 CarReplacementDecalHash[26] = {
    bStringHash("BOTTOM_DECAL"),
    bStringHash("FRONT_BUMPER_DECAL"),
    bStringHash("FRONT_DECAL"),
    bStringHash("GTWING_DECAL"),
    bStringHash("HOOD_DECAL"),
    bStringHash("LEFT_BRAKELIGHT_DECAL"),
    bStringHash("LEFT_DOOR_DECAL"),
    bStringHash("LEFT_FENDER_DECAL"),
    bStringHash("LEFT_QUARTER_DECAL"),
    bStringHash("LEFT_SIDE_MIRROR_DECAL"),
    bStringHash("LEFT_SKIRT_DECAL"),
    bStringHash("REAR_BUMPER_DECAL"),
    bStringHash("REAR_DECAL"),
    bStringHash("RIGHT_BRAKELIGHT_DECAL"),
    bStringHash("RIGHT_DOOR_DECAL"),
    bStringHash("RIGHT_FENDER_DECAL"),
    bStringHash("RIGHT_QUARTER_DECAL"),
    bStringHash("RIGHT_SIDE_MIRROR_DECAL"),
    bStringHash("RIGHT_SKIRT_DECAL"),
    bStringHash("TOP_DECAL"),
    bStringHash("FRONT_WINDOW_DECAL"),
    bStringHash("REAR_WINDOW_DECAL"),
    bStringHash("LEFT_FRONT_WINDOW_DECAL"),
    bStringHash("LEFT_REAR_WINDOW_DECAL"),
    bStringHash("RIGHT_FRONT_WINDOW_DECAL"),
    bStringHash("RIGHT_REAR_WINDOW_DECAL"),
};

static const int NUM_TRUNK_AUDIO_MARKERS = 0; // TODO fix the value

unsigned int gTrunkAudioMarkerHash[12] = {
    bStringHash("MARKER_AUDIO_COMP_0"), bStringHash("MARKER_AUDIO_COMP_1"), bStringHash("MARKER_AUDIO_COMP_2"),  bStringHash("MARKER_AUDIO_COMP_3"),
    bStringHash("MARKER_AUDIO_COMP_4"), bStringHash("MARKER_AUDIO_COMP_5"), bStringHash("MARKER_AUDIO_COMP_6"),  bStringHash("MARKER_AUDIO_COMP_7"),
    bStringHash("MARKER_AUDIO_COMP_8"), bStringHash("MARKER_AUDIO_COMP_9"), bStringHash("MARKER_AUDIO_COMP_10"), bStringHash("MARKER_AUDIO_COMP_11"),
};

float NISCopCarDoorOpenAmount[4];
bMatrix4 NISCopCarDoorOpenMarkers[4];
bMatrix4 NISCopCarDoorClosedMarkers[4];

void SetNISCopCarDoorOpenAmount(CarRenderDoorID door, float open_amount) {}

int NISCopCarDriverVisible = 0;

void SetNISCopCarDriverVisible(int visible) {}

int NISRaceDriverVisible = 1;

void SetNISRaceDriverVisible(int visible) {
    NISRaceDriverVisible = visible;
}

class CarPartModel {
  public:
    CarPartModel() {
        this->mModel = 0;
    }

    ~CarPartModel() {
        this->mModel = 0;
    }

    void Clear() {
        mModel = 0;
    }

    int IsHidden() {
        return this->mModel & 1;
    }

    void Hide(int bHide) {
        mModel = (mModel & ~3) | (bHide ? 1 : 0);
    }

    struct eModel *GetModel() {
        return reinterpret_cast<eModel *>(mModel & ~0x3);
    }

    void SetModel(struct eModel *model) {
        this->mModel = reinterpret_cast<unsigned int>(model) | this->IsHidden();
    }

    bool IsLodMissing() const {
        return (mModel & ~3u) == 0;
    }

  private:
    uint32 mModel; // offset 0x0, size 0x4
};

static const float Tweak_ResetFlashOff = 0.05f;
static const float Tweak_ResetFlashOn = 0.07f;

class CarRenderInfo {
  public:
    enum CarReplacementTexID {
        REPLACETEX_CARSKIN,
        REPLACETEX_CARSKINB,
        REPLACETEX_GLOBALSKIN,
        REPLACETEX_CARBONSKIN,
        REPLACETEX_GLOBALCARBONSKIN,
        REPLACETEX_BADGING,
        REPLACETEX_WHEEL,
        REPLACETEX_SPINNER,
        REPLACETEX_SPOILER,
        REPLACETEX_ROOF_SCOOP,
        REPLACETEX_DASHSKIN,
        REPLACETEX_DRIVER,
        REPLACETEX_TIRE,
        REPLACETEX_WINDOW_FRONT,
        REPLACETEX_WINDOW_REAR,
        REPLACETEX_WINDOW_LEFT_FRONT,
        REPLACETEX_WINDOW_LEFT_REAR,
        REPLACETEX_WINDOW_RIGHT_FRONT,
        REPLACETEX_WINDOW_RIGHT_REAR,
        REPLACETEX_WINDOW_REAR_DEFOST,
        REPLACETEX_WINDOW2_FRONT,
        REPLACETEX_WINDOW2_REAR,
        REPLACETEX_WINDOW2_LEFT_FRONT,
        REPLACETEX_WINDOW2_LEFT_REAR,
        REPLACETEX_WINDOW2_RIGHT_FRONT,
        REPLACETEX_WINDOW2_RIGHT_REAR,
        REPLACETEX_WINDOW2_REAR_DEFOST,
        REPLACETEX_HEADLIGHT_LEFT,
        REPLACETEX_HEADLIGHT_RIGHT,
        REPLACETEX_BRAKELIGHT_LEFT,
        REPLACETEX_BRAKELIGHT_RIGHT,
        REPLACETEX_BRAKELIGHT_CENTRE,
        REPLACETEX_HEADLIGHT_GLASS_LEFT,
        REPLACETEX_HEADLIGHT_GLASS_RIGHT,
        REPLACETEX_BRAKELIGHT_GLASS_LEFT,
        REPLACETEX_BRAKELIGHT_GLASS_RIGHT,
        REPLACETEX_BRAKELIGHT_GLASS_CENTRE,
        REPLACETEX_OLD_HEADLIGHT_LEFT,
        REPLACETEX_OLD_HEADLIGHT_RIGHT,
        REPLACETEX_OLD_BRAKELIGHT_LEFT,
        REPLACETEX_OLD_BRAKELIGHT_RIGHT,
        REPLACETEX_OLD_BRAKELIGHT_CENTRE,
        REPLACETEX_OLD_HEADLIGHT_GLASS_LEFT,
        REPLACETEX_OLD_HEADLIGHT_GLASS_RIGHT,
        REPLACETEX_OLD_BRAKELIGHT_GLASS_LEFT,
        REPLACETEX_OLD_BRAKELIGHT_GLASS_RIGHT,
        REPLACETEX_OLD_BRAKELIGHT_GLASS_CENTRE,
        REPLACETEX_BOTTOM_DECAL,
        REPLACETEX_FRONT_BUMPER_DECAL,
        REPLACETEX_FRONT_DECAL,
        REPLACETEX_GTWING_DECAL,
        REPLACETEX_HOOD_DECAL,
        REPLACETEX_LEFT_BRAKELIGHT_DECAL,
        REPLACETEX_LEFT_DOOR_DECAL,
        REPLACETEX_LEFT_FENDER_DECAL,
        REPLACETEX_LEFT_QUARTER_DECAL,
        REPLACETEX_LEFT_SIDE_MIRROR_DECAL,
        REPLACETEX_LEFT_SKIRT_DECAL,
        REPLACETEX_REAR_BUMPER_DECAL,
        REPLACETEX_REAR_DECAL,
        REPLACETEX_RIGHT_BRAKELIGHT_DECAL,
        REPLACETEX_RIGHT_DOOR_DECAL,
        REPLACETEX_RIGHT_FENDER_DECAL,
        REPLACETEX_RIGHT_QUARTER_DECAL,
        REPLACETEX_RIGHT_SIDE_MIRROR_DECAL,
        REPLACETEX_RIGHT_SKIRT_DECAL,
        REPLACETEX_TOP_DECAL,
        REPLACETEX_FRONT_WINDOW_DECAL,
        REPLACETEX_REAR_WINDOW_DECAL,
        REPLACETEX_LEFT_FRONT_WINDOW_DECAL,
        REPLACETEX_LEFT_REAR_WINDOW_DECAL,
        REPLACETEX_RIGHT_FRONT_WINDOW_DECAL,
        REPLACETEX_RIGHT_REAR_WINDOW_DECAL,
        REPLACETEX_DECAL_START = 47,
        REPLACETEX_DECAL_END = 72,
        REPLACETEX_DECAL_NUM = 26,
        REPLACETEX_NUM = 73,
    };

    CarRenderInfo(RideInfo *ride_info);

    ~CarRenderInfo();

    void Init();
    void Refresh();

    uint32 FindCarPart(int slotId);
    uint32 HideCarPart(int slotId, bool hide);

    void SetDeltaTime(float dt) {
        this->mDeltaTime = dt;
    }

    float GetDeltaTime() const {
        return this->mDeltaTime;
    }

    void SetCarDamageState(bool on, unsigned int startID, unsigned int endID);
    void SetCarGlassDamageState(bool on, CarReplacementTexID replacementId, uint32 undamageHash, uint32 damageHash);

    void UpdateCarReplacementTextures();
    void UpdateLightStateTextures();
    void CreateCarLightFlares();

    void SetRadius(float r) {
        this->mRadius = r;
    }

    float GetRadius() const {
        return this->mRadius;
    }

    void SetCollider(const WCollider *collider) {
        this->mWCollider = collider;
    }

    void SetAnimationTime(float animationTime) {
        this->AnimTime = animationTime;
    }

    void SetWheelWobble(unsigned int wheelInd, bool enable) {
        this->mWheelWobbleEnabled[wheelInd] = enable;
    }

    bool GetWheelWobble(unsigned int wheelInd) {
        return this->mWheelWobbleEnabled[wheelInd];
    }

    void SetFlashing(bool b) {
        this->mFlashing = b;
    }

    void UpdateFlashing() {
        if (this->mFlashing) {
            this->mFlashInterval += RealTimeElapsed;

            if (Tweak_ResetFlashOff < this->mFlashInterval) {
                this->mFlashInterval -= Tweak_ResetFlashOn + Tweak_ResetFlashOff;
            }
        }
    }

    bool Render(eView *view, const bVector3 *world_position, const bMatrix4 *body_matrix, bMatrix4 *tire_matrices, bMatrix4 *brake_matrices,
                bMatrix4 *spinner_matrices, uint32 extra_render_flags, int force_light_state, int reflexion, float shadow_scale, CARPART_LOD tireLOD,
                CARPART_LOD carLOD);

    void DrawAmbientShadow(eView *view, const bVector3 *position, float shadow_scale, bMatrix4 *localWorld, bMatrix4 *worldLocal,
                           bMatrix4 *biasedIdentity);

    void DrawKeithProjShadow(eView *view, const bVector3 *position, bMatrix4 *localWorld, bMatrix4 *worldLocal, bMatrix4 *biasedIdentity,
                             int body_lod);

    void SetCarRenderFlags(int draw_solid, int draw_alpha, int draw_shadows) {}

    void RenderFlaresOnCar(eView *view, const bVector3 *position, const bMatrix4 *body_matrix, int force_light_state, int reflexion,
                           int renderFlareFlags);
    void RenderTextureHeadlights(eView *view, bMatrix4 *l_w, uint32 Accurate);
    void SwitchSkin(RideInfo *ride_info);
    void UpdateDecalTextures(RideInfo *ride_info);
    void UpdateCarParts();

    void UpdateWheelYRenderOffset();

    float CalculateTireBlur(int nWheel);
    void RenderTireBlur(eView *view, TextureInfo *texture_info, bMatrix4 *local_world, float alpha, eDynamicLightContext *light_context);

    void SetSpecialFX(int fx) {
        this->SpecialFX = fx;
    }

    int GetSpecialFX() {
        return this->SpecialFX;
    }

    RideInfo *GetRideInfo() const {
        return this->pRideInfo;
    }

    void SetDamageInfo(const DamageZone::Info &damageInfo);
    void SetPlayerDamage(const DamageZone::Info &damageInfo);

    CARPART_LOD GetMinLodLevel() const {
        return this->mMinLodLevel;
    }

    CARPART_LOD GetMaxLodLevel() const {
        return this->mMaxLodLevel;
    }

    CARPART_LOD GetMinReflectionLodLevel() const {
        return this->mMinReflectionLodLevel;
    }

    void GetLightPositions(bSList<CarEmitterPosition> &pos, uint32 position);
    int GetEmitterPositions(bSList<CarEmitterPosition> &markers_out, const unsigned int *position_name_hashes, int num_pos_name_hashes);
    void InitEmitterPositions(bVector4 *tire_positions);

    void EnableAlphaWrites(bool onOff) {}

    const Attrib::Gen::ecar &GetAttributes() const {
        return this->mAttributes;
    }

    void SetBrokenLights(unsigned int vehiclefx_ids) {
        this->mBrokenLights = vehiclefx_ids;
    }

    void SetLights(unsigned int vehiclefx_ids) {
        this->mOnLights = vehiclefx_ids;
    }

    bool IsLightBroken(VehicleFX::ID id) const {
        return (this->mBrokenLights & id) != 0;
    }

    bool IsLightOn(VehicleFX::ID id) const {
        return (this->mOnLights & id) != 0;
    }

    void RenderPart(eView *view, CarPartModel *carPart, bMatrix4 *local_to_world, eDynamicLightContext *light_context, uint32 flags);
    void convex_hull(bVector3 *p, const WCollider *wcoll, int &n, float Z, float zBias, int fast);

    float AnimTime;                                                         // offset 0x0, size 0x4
    bVector3 mVelocity;                                                     // offset 0x4, size 0x10
    bVector3 mAngularVelocity;                                              // offset 0x14, size 0x10
    bVector3 mAcceleration;                                                 // offset 0x24, size 0x10
    IVehiclePartDamageBehaviour *mDamageBehaviour;                          // offset 0x34, size 0x4
    const WCollider *mWCollider;                                            // offset 0x38, size 0x4
    WWorldPos mWorldPos;                                                    // offset 0x3C, size 0x3C
    RideInfo *pRideInfo;                                                    // offset 0x78, size 0x4
    CarTypeInfo *pCarTypeInfo;                                              // offset 0x7C, size 0x4
    int16 LastCarPartChanged;                                               // offset 0x80, size 0x2
    bVector3 AABBMin;                                                       // offset 0x84, size 0x10
    bVector3 AABBMax;                                                       // offset 0x94, size 0x10
    bVector3 ModelOffset;                                                   // offset 0xA4, size 0x10
    bVector3 PivotPosition;                                                 // offset 0xB4, size 0x10
    TextureInfo *ShadowTexture;                                             // offset 0xC4, size 0x4
    TextureInfo *ShadowRampTexture;                                         // offset 0xC8, size 0x4
    eLightMaterial *LightMaterial_CarSkin;                                  // offset 0xCC, size 0x4
    eLightMaterial *LightMaterial_Carbon;                                   // offset 0xD0, size 0x4
    eLightMaterial *LightMaterial_WindowTint;                               // offset 0xD4, size 0x4
    eLightMaterial *LightMaterial_WheelRim;                                 // offset 0xD8, size 0x4
    eLightMaterial *LightMaterial_Caliper;                                  // offset 0xDC, size 0x4
    eLightMaterial *LightMaterial_Spoiler;                                  // offset 0xE0, size 0x4
    eLightMaterial *LightMaterial_Roof;                                     // offset 0xE4, size 0x4
    eLightMaterial *LightMaterial_Spinner;                                  // offset 0xE8, size 0x4
    int CarbonHood;                                                         // offset 0xEC, size 0x4
    eDynamicLightContext *Car_light_context;                                // offset 0xF0, size 0x4
    uint32 CarFrame;                                                        // offset 0xF4, size 0x4
    bTList<eLightFlare> LightFlareList;                                     // offset 0xF8, size 0x8
    float CarTimebaseStart;                                                 // offset 0x100, size 0x4
    float WheelYRenderOffset[4];                                            // offset 0x104, size 0x10
    float WheelWidths[2];                                                   // offset 0x114, size 0x8
    float WheelRadius[2];                                                   // offset 0x11C, size 0x8
    float WheelWidthScales[4];                                              // offset 0x124, size 0x10
    float WheelRadiusScales[4];                                             // offset 0x134, size 0x10
    float WheelBrakeMarkerY[2];                                             // offset 0x144, size 0x8
    bool mEmitterPositionsInitialized;                                      // offset 0x14C, size 0x1
    bSList<CarEmitterPosition> EmitterPositionList[28];                     // offset 0x150, size 0xE0
    eReplacementTextureTable MasterReplacementTextureTable[73];             // offset 0x230, size 0x36C
    eReplacementTextureTable CarbonReplacementTextureTable[73];             // offset 0x59C, size 0x36C
    eReplacementTextureTable DecalReplacementTextureTable[48];              // offset 0x908, size 0x240
    eReplacementTextureTable BrakeLeftReplacementTextureTable[2];           // offset 0xB48, size 0x18
    eReplacementTextureTable BrakeRightReplacementTextureTable[2];          // offset 0xB60, size 0x18
    CarPartModel mCarPartModels[CARSLOTID_MODEL_NUM][CARPART_MODEL_NUM][5]; // offset 0xB78, size 0x5F0
    int SpecialFX;                                                          // offset 0x1168, size 0x4
    float mCar_elevation;                                                   // offset 0x116C, size 0x4
    int NOSstate;                                                           // offset 0x1170, size 0x4
    bAngle mSteeringR;                                                      // offset 0x1174, size 0x2
    bAngle mSteeringL;                                                      // offset 0x1176, size 0x2
    bMatrix4 LastFewMatrices[3];                                            // offset 0x1178, size 0xC0
    bVector3 LastFewPositions[3];                                           // offset 0x1238, size 0x30
    int matrixIndex;                                                        // offset 0x1268, size 0x4
    ePositionMarker *SpoilerPositionMarker;                                 // offset 0x126C, size 0x4
    ePositionMarker *SpoilerPositionMarker2;                                // offset 0x1270, size 0x4
    ePositionMarker *RoofScoopPositionMarker;                               // offset 0x1274, size 0x4
    uint32 AAflags;                                                         // offset 0x1278, size 0x4
    int AAdraw_solid;                                                       // offset 0x127C, size 0x4
    int AAdraw_alpha;                                                       // offset 0x1280, size 0x4
    int AAdraw_shadows;                                                     // offset 0x1284, size 0x4

  private:
    UsedCarTextureInfo mUsedTextureInfos; // offset 0x1288, size 0x380
    unsigned int mOnLights;               // offset 0x1608, size 0x4
    unsigned int mBrokenLights;           // offset 0x160C, size 0x4
    CARPART_LOD mMinLodLevel;             // offset 0x1610, size 0x4
    CARPART_LOD mMaxLodLevel;             // offset 0x1614, size 0x4
    CARPART_LOD mMinReflectionLodLevel;   // offset 0x1618, size 0x4
    CarPartCuller TheCarPartCuller;       // offset 0x161C, size 0x134
    static bool mAlphaWritesEnabled;
    DamageZone::Info mDamageZoneInfo;  // offset 0x1750, size 0x4
    float mDeltaTime;                  // offset 0x1754, size 0x4
    float mRadius;                     // offset 0x1758, size 0x4
    Attrib::Gen::ecar mAttributes;     // offset 0x175C, size 0x14
    bool mFlashing;                    // offset 0x1770, size 0x1
    float mFlashInterval;              // offset 0x1774, size 0x4
    DamageZone::Info mDamageInfoCache; // offset 0x1778, size 0x4
    bool mWheelWobbleEnabled[4];       // offset 0x177C, size 0x4
    bool mMirrorLeftWheels;            // offset 0x178C, size 0x1
};

bool AlphaWritesEnabled = true;

static const int NUM_DASHES = 3;

enum DASH_INDEX {
    DASH_F50 = 0,
    DASH_BARCHETTA = 1,
    DASH_360 = 2,
};

// TODO use these, the values are from Undercover, so they might be different in MW

DASH_INDEX dash_index;

static const float dashPositionX[3] = {-0.25581399f, -0.25581399f, -0.25128201f};

static const float dashPositionY[3] = {-0.107692f, -0.15897401f, -0.179488f};

static const float dashPositionZ[3] = {-0.308f, -0.40697601f, -0.32307601f};

static const float dashScaleX[3] = {0.89999998f, 0.89999998f, 0.89999998f};

static const float dashScaleY[3] = {1.703488f, 2.0f, 1.703488f};

static const float dashScaleZ[3] = {1.697674f, 2.0f, 1.697674f};

float WheelPivotTranslationAmount = 0.5f;

float WheelStandardWidth = 0.225f;

float WheelStandardRadius = 0.32f;

static const float TweakWheelWidthScale = 1.0f;
static const float TweakWheelRadiusScale = 1.0f;
static const int MaxDrawnCars = 0; // TODO fix value
int DrawCars = 1;
int DrawCarsReflections = 1;
static const int DrawBody = 1;
static const int DrawCarReflection = 1;
static const int DrawCarHeadlight = 1;
static const int DrawCarBrakelight = 1;
static const int DrawCarEngine = 1;
static const int DrawLicensePlates = 1;
static const int DrawScuffs = 1;
static const int DrawSideMirrors = 1;
int DrawCarShadow = 1;
static const int DrawTires = 1;
static const int DrawTireBlurs = 1;
static const int DrawBrakes = 1;
static const int DrawWipers = 1;
static const int DrawDashboard = 1;
static const int DrawInCarSteeringWheel = 1;
static const int DrawDriverModel = 1;
static const int DrawDriverModelInFE = 1;
int DrawTrunkAudio = 0;
static const int DrawSpoiler = 1;
static const int DrawRoofScoop = 1;
static const int DrawDoors = 1;
static const int DrawHood = 1;
static const int DrawInterior = 1;
static const int ForceRenderCar = 1;
static const int DrawCarParts = 1;
static const int PrintCarInit = 0;
static const int PrintCarFlow = 0;
static const int PrintMissingCarModels = 0;
static const int EnablePreviousLODs = 1;
static const int PrintCarRenderModel = 0;
int ForceHeadlightsOn = 0;
int ForceBrakelightsOn = 0;
int ForceReverselightsOn = 0;
int ForceCopLightsOn = 0;
int ForceDriverModelOn = 0;
static const int ApplySpoilerPivotRotation = 0;
static const int UsePositionMarkerForStockSpoiler = 0;
static const float HeadlightBaseIntensity = 0.2f;
static const float BrakelightBaseIntensity = 0.0f;
static const float TrafficBrakelightBaseIntensity = 0.5f;
static const float TrafficHeadlightBaseIntensity = 0.2f;
static const float ExtraRearTireOffsetOverride = 0.0f;
static const float TweakBrakeMarkerY[2] = {0.0f, 0.0f};
static const float BrakeLightScaleFactor = 16.0f;
static const int CarPrintEffects = 0;
static const float CarShadowElevationIG = 0.01f;
static const float CarShadowScaleWidthIG = 1.12f;
static const float CarShadowScaleLengthIG = 1.14f;
static const int CarBaseShadowAlphaMinIG = 0;
static const int CarBaseShadowAlphaMaxIG = 100;
static const float CarShadowElevationFE = 0.0f; // TODO fix value
static const float CarShadowScaleWidthFE = 1.12f;
static const float CarShadowScaleLengthFE = 1.14f;
static const int CarBaseShadowAlphaMinFE = 255;
static const int CarBaseShadowAlphaMaxFE = 255;

static const float CarNeonElevation = 0.01f;
static const float CarNeonElevationFalloff = 0.0f; // TODO fix value?
static const float CarNeonElevationCutoff = 0.7f;  // TODO fix value
static const float CarNeonScaleMax = 0.75f;

static const bool TweakCarShadowSun = false;
static const float TweakCarShadowSunX = -16384.0f;
static const float TweakCarShadowSunY = 0.0f;
static const float TweakCarShadowSunZ = 7000.0f;
static const float TweakCarShadowSunScale = 0.5f;
static const float TweakCarShadowTopologyTolerance = 1.25f;

static const float HullShadowElevation = 0.06f;

static const float fCarFudgeElevation = 0.01f;
static const float fCarFudgeBias = 0.25f;
static const float BrakeMirrorX = 1.0f;
static const float BrakeMirrorY = -1.0f;
static const float BrakeMirrorZ = 1.0f;
static const int TweakerDriverHeader = 0;
static const int ReplaceUnusedDecals = 1;
unsigned char CopLightsRed[8] = {1, 1, 1, 1, 0, 0, 0, 0};
unsigned char CopLightsBlue[8] = {0, 0, 1, 1, 1, 1, 0, 0};
unsigned char CopLightsWhite[8] = {1, 1, 1, 1, 0, 0, 0, 0};
unsigned char CopLightsSlowRed[16] = {1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0};
unsigned char CopLightsSlowBlue[16] = {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1};
unsigned char CopLightsSlowWhite[16] = {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0};
unsigned char CopLightsHeadlights[16] = {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
int PrintQueryLightMat = 0;
int PrintLightMat = 0;
int PrintLightMatX = 0;
int PrintLightMatY = 0;
static const float DashVibrationAmplitude = 0.048f;
static const float CarBodyLodSwapSize[5] = {120.0f, 25.0f, 20.0f, 10.0f, 0.0f};
static const float TrafficCarBodyLodSwapSize[5] = {20.0, 10.0f, 4.0f, 0.0f, 0.0f};
static const float DisableEnvMapSize = 5.0f;
static const float CarTwoPlayerLODFactor = 0.7f;
static const CARPART_LOD CarBodyMaxLOD = CARPART_LOD_D;

CARPART_LOD ForceCarLOD = CARPART_LOD_OFF;
CARPART_LOD ForceTireLOD = CARPART_LOD_OFF;
static const int ShowCarLODScreenPrint = 0;
static const int CarLODScreenPrintX = 100;
static const int CarLODScreenPrintY = -228;
float CarLightFlareNormals[4][6] = {{1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f},
                                    {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
                                    {0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
                                    {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}};
bVector3 EnvMapEyeOffset(0.0f, 0.0f, 0.95f);
bVector3 EnvMapCamOffset(0.0f, 0.0f, 0.6f);
bMatrix4 CarScaleMatrix;

typedef struct tagCarEffectParam {
    const char *Name;
    uint32 NameHash;
} CarEffectParam;

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

SlotPool *CarEmitterPositionSlotPool = nullptr;

bAngle TestSpriteAngle = 0;
ePointSprite3D TestSprite; // TODO values, or is it just zero initialized?
int TestSpriteonoff = 0;
int numTestSprites = 1;
int numTestSpritesmul = 1;
float testSpriteXoff = 1.0f;

static const int MirrorLeftTires = 0;

bMatrix4 LeftTireRotateZMatrix;
bMatrix4 LeftTireMirrorMatrix;

TextureInfo *pTextureInfo2PlayerHeadlight1 = nullptr;

void InitCarRender() {
    CarPartModelPool = bNewSlotPool(0x18, 1024, "CarPartModelPool", 0);

    eIdentity(&LeftTireRotateZMatrix);
    eRotateZ(&LeftTireRotateZMatrix, &LeftTireRotateZMatrix, 32768);

    eIdentity(&LeftTireMirrorMatrix);
    LeftTireMirrorMatrix.v0.x = 1.0f;
    LeftTireMirrorMatrix.v1.y = -1.0f;
    LeftTireMirrorMatrix.v2.z = 1.0f;
}

void InitCarEffects() {
    CarEmitterPositionSlotPool = bNewSlotPool(0x14, MAX_CAR_PART_MODELS, "CarEmitterPositionSlotPool", GetVirtualMemoryAllocParams());
    for (int i = 0; i < NUM_ELEMENTS(CarEffectParameters); i++) {
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
    StandardCubeModel.Init(STRINGHASH_STD_CUBE);
    StandardDebugModel.Init(bStringHash("DEBUG_LOD_CUBE"));
}

void DrawCube(eView *view, bMatrix4 *local_world, float w, float h, float l) {}

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

static const CarEffectPosition CarFXForcePosition = CARFXPOS_NONE;

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

bool FrontEndRenderingCar::LookupWheelRadius(unsigned int index, float &radius) {
    if (this->RenderInfo == nullptr)
        return false;

    UMath::Vector4 pos;
    this->RenderInfo->GetAttributes().TireOffsets(pos, index);
    radius = pos.w;

    return true;
}

void FrontEndRenderingCar::ReInit(struct RideInfo *ride_info) {
    if (this->RenderInfo != nullptr) {
        CarRenderInfo *info;

        delete this->RenderInfo;
        this->RenderInfo = nullptr;
    }

    if (ride_info != nullptr) {
        this->mRideInfo = *ride_info;
        this->RenderInfo = new ("CarRenderInfo", 0) CarRenderInfo(&this->mRideInfo);
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

    FrontEndRenderingCarList.Remove(this);
}

inline int IsNISCopCar(int type) {
    return static_cast<int>(type == CARTYPE_COPMIDSIZEINT);
}

// STRIPPED
void FrontEndRenderingCar::OverRideAlpha(unsigned char nAlphaBits, bool bSemiTrans, bool bWriteZ) {}

// STRIPPED
void FrontEndRenderingCar::RestoreAlpha() {}

CarRenderInfo::CarRenderInfo(RideInfo *ride_info)
    : mDamageBehaviour(nullptr),           //
      mWCollider(nullptr),                 //
      mWorldPos(0.025f),                   //
      mEmitterPositionsInitialized(false), //
      mOnLights(0),                        //
      mBrokenLights(0),                    //
      mRadius(3.0f),                       //
      mAttributes(0xeec2271a, 0, nullptr), //
      mFlashing(false),                    //
      mFlashInterval(0.0f) {
    ProfileNode profile_node("TODO", 0);
    mWheelWobbleEnabled[0] = false;
    mWheelWobbleEnabled[1] = false;
    mWheelWobbleEnabled[2] = false;
    mWheelWobbleEnabled[3] = false;

    CarTypeInfo *info = GetCarTypeInfo(ride_info->Type);
    char *car_base_name = info->BaseModelName;
    this->mAttributes.ChangeWithDefault(Attrib::StringToLowerCaseKey(car_base_name));
    this->mMirrorLeftWheels = (this->mAttributes.WheelSpokeCount() & 0x80) != 0;
    bVector3 tire_positions[4];
    float wheel_radius[4];

    this->AnimTime = 0.0f;
    for (unsigned int i = 0; i < 4; i++) {
        UMath::Vector4 tire_offset;

        this->mAttributes.TireOffsets(tire_offset, i);
        // TODO how to assign bVector instead of bFill? why are there two bVector constructor calls here?
        // tire_positions[i] = bVector3(tire_offset.x, tire_offset.y, tire_offset.z);
        bFill(&tire_positions[i], tire_offset.x, tire_offset.y, tire_offset.z);
        wheel_radius[i] = tire_offset.w;
    }

    this->WheelWidths[0] = WheelStandardWidth;
    this->WheelWidths[1] = WheelStandardWidth;
    this->WheelRadius[0] = WheelStandardRadius;
    this->WheelRadius[1] = WheelStandardRadius;

    for (int i = 0; i < 4; i++) {
        this->WheelWidthScales[i] = TweakWheelWidthScale;
        this->WheelRadiusScales[i] = TweakWheelRadiusScale;
    }

    this->mVelocity = bVector3(0.0f, 0.0f, 0.0f);
    this->mAngularVelocity = bVector3(0.0f, 0.0f, 0.0f);
    this->mAcceleration = bVector3(0.0f, 0.0f, 0.0f);

    if (IsGameFlowInFrontEnd()) {
        this->mMinLodLevel = ride_info->GetMinFELodLevel();
        this->mMaxLodLevel = ride_info->GetMaxFELodLevel();
    } else {
        this->mMinLodLevel = ride_info->GetMinLodLevel();
        this->mMaxLodLevel = ride_info->GetMaxLodLevel();
    }

    this->mMinReflectionLodLevel = ride_info->GetMinReflectionLodLevel();
    this->pRideInfo = ride_info;
    this->LastCarPartChanged = -1;
    this->CarTimebaseStart = bRandom(1.0f);
    this->mDeltaTime = 0.0f;

    CarTypeInfo *car_type_info = GetCarTypeInfo(this->pRideInfo->Type);
    int is_traffic_car = static_cast<int>(car_type_info->GetCarUsageType() == CAR_USAGE_TYPE_TRAFFIC);

    bMemSet(this->mCarPartModels, 0, sizeof(this->mCarPartModels));
    this->pCarTypeInfo = car_type_info;
    this->CarbonHood = 0;
    GetUsedCarTextureInfo(&this->mUsedTextureInfos, this->pRideInfo, 0);

    {
        this->MasterReplacementTextureTable[REPLACETEX_CARSKIN].SetOldNameHash(this->mUsedTextureInfos.MappedSkinHash);
        this->MasterReplacementTextureTable[REPLACETEX_CARSKINB].SetOldNameHash(this->mUsedTextureInfos.MappedSkinBHash);
        this->MasterReplacementTextureTable[REPLACETEX_GLOBALSKIN].SetOldNameHash(this->mUsedTextureInfos.MappedGlobalHash);
        this->MasterReplacementTextureTable[REPLACETEX_CARBONSKIN].SetOldNameHash(0xA7366AE6);
        this->MasterReplacementTextureTable[REPLACETEX_GLOBALCARBONSKIN].SetOldNameHash(0x3C84D757);
        this->MasterReplacementTextureTable[REPLACETEX_BADGING].SetOldNameHash(this->mUsedTextureInfos.MappedBadging);
        this->MasterReplacementTextureTable[REPLACETEX_WHEEL].SetOldNameHash(this->mUsedTextureInfos.MappedWheelHash);
        this->MasterReplacementTextureTable[REPLACETEX_SPINNER].SetOldNameHash(this->mUsedTextureInfos.MappedSpinnerHash);
        this->MasterReplacementTextureTable[REPLACETEX_SPOILER].SetOldNameHash(this->mUsedTextureInfos.MappedSpoilerHash);
        this->MasterReplacementTextureTable[REPLACETEX_ROOF_SCOOP].SetOldNameHash(this->mUsedTextureInfos.MappedRoofScoopHash);
        this->MasterReplacementTextureTable[REPLACETEX_DASHSKIN].SetOldNameHash(this->mUsedTextureInfos.MappedDashSkinHash);
        this->MasterReplacementTextureTable[REPLACETEX_DRIVER].SetOldNameHash(0x5799E60B);
        this->MasterReplacementTextureTable[REPLACETEX_TIRE].SetOldNameHash(this->mUsedTextureInfos.MappedTireHash);

        if (is_traffic_car != 0) {
            this->MasterReplacementTextureTable[REPLACETEX_TIRE].SetNewNameHash(bStringHash("TRAFFIC_TIRE"));
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
        this->BrakeLeftReplacementTextureTable[0].SetOldNameHash(STRINGHASH_BRAKE_GLOBAL);
        this->BrakeLeftReplacementTextureTable[0].SetNewNameHash(STRINGHASH_BRAKE_GLOBAL_LEFT);
        this->BrakeLeftReplacementTextureTable[1].SetOldNameHash(this->mUsedTextureInfos.MappedGlobalHash);
        this->BrakeRightReplacementTextureTable[0].SetOldNameHash(STRINGHASH_BRAKE_GLOBAL);
        this->BrakeRightReplacementTextureTable[0].SetNewNameHash(STRINGHASH_BRAKE_GLOBAL);
        this->BrakeRightReplacementTextureTable[1].SetOldNameHash(this->mUsedTextureInfos.MappedGlobalHash);

        this->SwitchSkin(this->pRideInfo);

        uint32 badging_namehash = this->mUsedTextureInfos.MappedBadging;
        const char *car_badging_suffix = BuildRegion::GetCarBadgingSuffix();

        if (car_badging_suffix != nullptr) {
            uint32 badging_eu_namehash = bStringHash(car_badging_suffix, badging_namehash);

            if (GetTextureInfo(badging_eu_namehash, 0, 0) != nullptr) {
                badging_namehash = badging_eu_namehash;
            }
        }

        TextureInfo *badging_texture = GetTextureInfo(badging_namehash, 0, 0);

        if (badging_texture != nullptr) {
            badging_texture->ApplyAlphaSorting = 0;
        }

        this->MasterReplacementTextureTable[REPLACETEX_BADGING].SetNewNameHash(badging_namehash);
        this->UpdateCarReplacementTextures();
        this->UpdateDecalTextures(ride_info);
        unsigned int driver_namehash =
            TweakerDriverHeader > 1 ? (TweakerDriverHeader == 2 ? STRINGHASH_DRIVER_FEMALE : STRINGHASH_DRIVER_HELMET) : STRINGHASH_DRIVER_MALE;
        this->MasterReplacementTextureTable[REPLACETEX_DRIVER].SetNewNameHash(driver_namehash);
    }

    this->UpdateCarParts();

    int shadow_hash = bStringHash("CARSHADOW");
    this->ShadowTexture = GetTextureInfo(shadow_hash, 1, 0);
    this->ShadowRampTexture = GetTextureInfo(STRINGHASH_SHADOWRAMP, 0, 0);

    if (this->ShadowTexture != nullptr) {
        this->ShadowTexture->ApplyAlphaSorting = 0;
        this->ShadowTexture->RenderingOrder = 2;
    }

    if (this->ShadowRampTexture != nullptr) {
        this->ShadowRampTexture->ApplyAlphaSorting = 0;
        this->ShadowRampTexture->RenderingOrder = 3;
    }

    eModel *model = this->mCarPartModels[CARSLOTID_BASE][0][this->mMinLodLevel].GetModel();

    if (model != nullptr) {
        bVector4 *pivot_position = model->GetPivotPosition();

        this->PivotPosition.x = pivot_position != nullptr ? pivot_position->x : 0.0f;
        this->PivotPosition.y = pivot_position != nullptr ? pivot_position->y : 0.0f;
        this->PivotPosition.z = pivot_position != nullptr ? pivot_position->z : 0.0f;
    }

    this->CreateCarLightFlares();

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

    {
        ProfileNode profile_node("TODO", 0);
        eModel *model_table[CARSLOTID_MODEL_NUM][CARPART_MODEL_NUM];

        for (int lod_index = this->mMinLodLevel; lod_index < this->mMaxLodLevel + 1; lod_index++) {
            bMemSet(model_table, 0, sizeof(model_table));

            for (int model_number = 0; model_number < CARPART_MODEL_NUM; model_number++) {
                for (int model_index = 0; model_index < CARSLOTID_MODEL_NUM; model_index++) {
                    eModel *model = this->mCarPartModels[model_index][model_number][lod_index].GetModel();
                    eModel *prev_model = nullptr;
                    int car_slot_id = model_index;

                    if (lod_index > this->mMinLodLevel) {
                        prev_model = this->mCarPartModels[model_index][model_number][lod_index - 1].GetModel();
                    }

                    if (model != nullptr) {
                        switch (car_slot_id) {
                            case CARSLOTID_RIGHT_SIDE_MIRROR:
                            case CARSLOTID_BODY:
                            case CARSLOTID_BASE:
                            case CARSLOTID_LEFT_SIDE_MIRROR:
                            case CARSLOTID_SPOILER:
                            case CARSLOTID_ROOF:
                            case CARSLOTID_HOOD:
                            case CARSLOTID_HEADLIGHT:
                            case CARSLOTID_BRAKELIGHT:
                                if (lod_index == this->mMinLodLevel || prev_model == nullptr ||
                                    (lod_index > this->mMinLodLevel && prev_model->GetNameHash() != model->GetNameHash())) {
                                    model_table[model_index][model_number] = model;
                                }
                                break;

                            default:
                                model_table[model_index][model_number] = nullptr;
                                break;
                        }
                    } else {
                        model_table[model_index][model_number] = model;
                    }
                }
            }

            eSmoothNormals(model_table[0], CARSLOTID_MODEL_NUM);
        }
    }

    {
        CarPart *base_paint_layer = ride_info->GetPart(CARSLOTID_BASE_PAINT);
        unsigned int light_material_namehash = 0;

        if (base_paint_layer != nullptr) {
            light_material_namehash = base_paint_layer->GetLightMaterialNameHash();
        }

        this->LightMaterial_CarSkin = elGetLightMaterial(light_material_namehash);
        this->LightMaterial_Carbon = elGetLightMaterial(bStringHash("CARBONFIBER"));
    }

    {
        CarPart *window_tint_part = ride_info->GetPart(CARSLOTID_WINDOW_TINT);
        unsigned int light_material_namehash = STRINGHASH_WINDSHIELD;

        if (window_tint_part != nullptr) {
            light_material_namehash = window_tint_part->GetLightMaterialNameHash();
        }

        this->LightMaterial_WindowTint = elGetLightMaterial(light_material_namehash);
    }

    {
        CarPart *paint_part_rim = ride_info->GetPart(CARSLOTID_PAINT_RIM);
        CarPart *paint_part_caliper = nullptr;
        CarPart *paint_part_spoiler = nullptr;
        CarPart *paint_part_roof = nullptr;
        CarPart *part_rim = ride_info->GetPart(CARSLOTID_FRONT_WHEEL);
        CarPart *part_caliper = nullptr;
        CarPart *part_spoiler = nullptr;
        CarPart *part_roof = nullptr;

        if (part_rim == nullptr || part_rim->GetUpgradeLevel() == 0) {
            paint_part_rim = nullptr;
        }

        if (part_caliper == nullptr || part_caliper->GetUpgradeLevel() == 0) {
            paint_part_caliper = nullptr;
        }

        if (part_spoiler == nullptr || part_spoiler->GetUpgradeLevel() == 0) {
            paint_part_spoiler = nullptr;
        }

        if (part_roof == nullptr || part_roof->GetUpgradeLevel() == 0) {
            paint_part_roof = nullptr;
        }

        uint32 light_material_namehash_rim = 0;
        uint32 light_material_namehash_caliper = 0;
        uint32 light_material_namehash_spoiler = 0;
        uint32 light_material_namehash_roof = 0;

        if (paint_part_rim != nullptr) {
            light_material_namehash_rim = paint_part_rim->GetLightMaterialNameHash();
        }

        if (paint_part_caliper != nullptr) {
            light_material_namehash_caliper = paint_part_caliper->GetLightMaterialNameHash();
        }

        if (paint_part_spoiler != nullptr) {
            light_material_namehash_spoiler = paint_part_spoiler->GetLightMaterialNameHash();
        }

        if (paint_part_roof != nullptr) {
            light_material_namehash_roof = paint_part_roof->GetLightMaterialNameHash();
        }

        eLightMaterial *light_material_rim = light_material_namehash_rim ? elGetLightMaterial(light_material_namehash_rim) : nullptr;
        eLightMaterial *light_material_caliper = light_material_namehash_caliper ? elGetLightMaterial(light_material_namehash_caliper) : nullptr;
        eLightMaterial *light_material_spoiler = light_material_namehash_spoiler ? elGetLightMaterial(light_material_namehash_spoiler) : nullptr;
        eLightMaterial *light_material_roof = light_material_namehash_roof ? elGetLightMaterial(light_material_namehash_roof) : nullptr;

        this->LightMaterial_Caliper = light_material_caliper;
        this->LightMaterial_WheelRim = light_material_rim;
        this->LightMaterial_Spoiler = light_material_spoiler;
        this->LightMaterial_Roof = light_material_roof;
    }

    this->UpdateWheelYRenderOffset();

    {
        eModel *front_wheel_model = this->mCarPartModels[CARSLOTID_FRONT_WHEEL][0][this->mMinLodLevel].GetModel();
        eModel *rear_wheel_model = this->mCarPartModels[CARSLOTID_REAR_WHEEL][0][this->mMinLodLevel].GetModel();
        ePositionMarker *front_position_marker = front_wheel_model != nullptr ? front_wheel_model->GetPostionMarker(STRINGHASH_FRONT_BRAKE) : nullptr;
        ePositionMarker *rear_position_marker = rear_wheel_model != nullptr ? rear_wheel_model->GetPostionMarker(STRINGHASH_REAR_BRAKE) : nullptr;

        if (front_position_marker != nullptr && rear_position_marker == nullptr) {
            rear_position_marker = front_position_marker;
        }

        this->WheelBrakeMarkerY[0] = front_position_marker != nullptr ? front_position_marker->Matrix.v3.y : 0.0f;
        this->WheelBrakeMarkerY[1] = rear_position_marker != nullptr ? rear_position_marker->Matrix.v3.y : 0.0f;
    }

    if (IsNISCopCar(this->pRideInfo->Type)) {
        for (int i = 0; i < 4; i++) {
            NISCopCarDoorOpenAmount[i] = 0;
            bIdentity(&NISCopCarDoorOpenMarkers[i]);
            bIdentity(&NISCopCarDoorClosedMarkers[i]);
        }

        eModel *base_model = this->mCarPartModels[CARSLOTID_BODY][0][this->mMinLodLevel].GetModel();

        if (base_model != nullptr) {
            unsigned int open_string_hashes[4] = {STRINGHASH_FRONT_LEFT_DOOR_OPEN, STRINGHASH_FRONT_RIGHT_DOOR_OPEN, STRINGHASH_REAR_RIGHT_DOOR_OPEN,
                                                  STRINGHASH_REAR_LEFT_DOOR_OPEN};
            unsigned int closed_string_hashes[4] = {STRINGHASH_FRONT_LEFT_DOOR, STRINGHASH_FRONT_RIGHT_DOOR, STRINGHASH_REAR_RIGHT_DOOR,
                                                    STRINGHASH_REAR_LEFT_DOOR};

            for (int i = 0; i < 4; i++) {
                ePositionMarker *open_marker = base_model->GetPostionMarker(open_string_hashes[i]);
                ePositionMarker *closed_marker = base_model->GetPostionMarker(closed_string_hashes[i]);

                if (open_marker == nullptr || closed_marker == nullptr) {
                    continue;
                }

                NISCopCarDoorOpenMarkers[i] = open_marker->Matrix;
                NISCopCarDoorClosedMarkers[i] = closed_marker->Matrix;
            }
        }
    }

    if (this->pRideInfo->Type == CARTYPE_COPMIDSIZE || this->pRideInfo->Type == CARTYPE_COPSUV || this->pRideInfo->Type == CARTYPE_COPSPORT ||
        this->pRideInfo->Type == CARTYPE_COPGHOST || this->pRideInfo->Type == CARTYPE_COPGTO || this->pRideInfo->Type == CARTYPE_COPSUVL ||
        this->pRideInfo->Type == CARTYPE_COPGTOGHOST || this->pRideInfo->Type == CARTYPE_COPSPORTHENCH ||
        this->pRideInfo->Type == CARTYPE_COPSPORTGHOST) {
        this->mDamageBehaviour = new ("VehiclePartDamageBehaviour", 0) VehiclePartDamageBehaviour(this);
    }

    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_FRONT, bStringHash("WINDOW_FRONT"), STRINGHASH_WINDOW_DAMAGE0);
    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_REAR, bStringHash("WINDOW_FRONT"), STRINGHASH_WINDOW_DAMAGE0);
    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_LEFT_FRONT, bStringHash("WINDOW_FRONT"), STRINGHASH_WINDOW_DAMAGE0);
    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_LEFT_REAR, bStringHash("WINDOW_FRONT"), STRINGHASH_WINDOW_DAMAGE0);
    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_RIGHT_FRONT, bStringHash("WINDOW_FRONT"), STRINGHASH_WINDOW_DAMAGE0);
    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_RIGHT_REAR, bStringHash("WINDOW_FRONT"), STRINGHASH_WINDOW_DAMAGE0);
    this->SetCarGlassDamageState(false, REPLACETEX_WINDOW_REAR_DEFOST, bStringHash("REAR_DEFROSTER"), STRINGHASH_WINDOW_DAMAGE0);
}

CarRenderInfo::~CarRenderInfo() {
    for (int model_index = 0; model_index < CARSLOTID_MODEL_NUM; model_index++) {
        for (int model_number = 0; model_number < CARPART_MODEL_NUM; model_number++) {
            for (int model_lod = this->mMinLodLevel; model_lod <= this->mMaxLodLevel;) {
                eModel *model = this->mCarPartModels[model_index][model_number][model_lod].GetModel();

                if (model != nullptr && model->GetNameHash() != 0) {
                    model->UnInit();
                    CarPartModelPool->Free(model);
                    this->mCarPartModels[model_index][model_number][model_lod].SetModel(nullptr);
                }

                this->mCarPartModels[model_index][model_number][model_lod++].Clear();
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

    for (int fxpos = 0; fxpos < NUM_ELEMENTS(this->EmitterPositionList); fxpos++) {
        for (CarEmitterPosition *empos = this->EmitterPositionList[fxpos].GetHead(); empos != this->EmitterPositionList[fxpos].EndOfList();
             empos = empos->GetNext()) {
            if (empos->PositionMarker != nullptr) {
                empos->PositionMarker = reinterpret_cast<ePositionMarker *>(gDefragFixer.Fix(empos->PositionMarker));
            }
        };
    }
}

static const bool Tweak_EnableAllSkinDamage = true;
static const bool Tweak_EnableSkinDamageFRONT = true;
static const bool Tweak_EnableSkinDamageFRONTLEFT = true;
static const bool Tweak_EnableSkinDamageFRONTRIGHT = true;
static const bool Tweak_EnableSkinDamageREAR = true;
static const bool Tweak_EnableSkinDamageREARLEFT = true;
static const bool Tweak_EnableSkinDamageREARRIGHT = true;

void CarRenderInfo::SetPlayerDamage(const DamageZone::Info &damageInfo) {
    if (this->mDamageInfoCache.Value == damageInfo.Value)
        return;

    const unsigned int kDamageThresh = 0;
    const unsigned int kDamageWindowThresh = 1;

    this->mDamageInfoCache.Value = damageInfo.Value;

    this->SetCarDamageState((Tweak_EnableAllSkinDamage || Tweak_EnableSkinDamageFRONT) && damageInfo.Get(DamageZone::DZ_FRONT) > kDamageThresh,
                            CARSLOTID_DAMAGE0_FRONT, CARSLOTID_DAMAGE0_FRONT);
    this->SetCarDamageState((Tweak_EnableAllSkinDamage || Tweak_EnableSkinDamageFRONT) && damageInfo.Get(DamageZone::DZ_REAR) > kDamageThresh,
                            CARSLOTID_DAMAGE0_REAR, CARSLOTID_DAMAGE0_REAR);
    this->SetCarDamageState((Tweak_EnableAllSkinDamage || Tweak_EnableSkinDamageFRONT) && damageInfo.Get(DamageZone::DZ_LFRONT) > kDamageThresh,
                            CARSLOTID_DAMAGE0_FRONTLEFT, CARSLOTID_DAMAGE0_FRONTLEFT);
    this->SetCarDamageState((Tweak_EnableAllSkinDamage || Tweak_EnableSkinDamageFRONT) && damageInfo.Get(DamageZone::DZ_RFRONT) > kDamageThresh,
                            CARSLOTID_DAMAGE0_FRONTRIGHT, CARSLOTID_DAMAGE0_FRONTRIGHT);
    this->SetCarDamageState((Tweak_EnableAllSkinDamage || Tweak_EnableSkinDamageFRONT) && damageInfo.Get(DamageZone::DZ_LREAR) > kDamageThresh,
                            CARSLOTID_DAMAGE0_REARLEFT, CARSLOTID_DAMAGE0_REARLEFT);
    this->SetCarDamageState((Tweak_EnableAllSkinDamage || Tweak_EnableSkinDamageFRONT) && damageInfo.Get(DamageZone::DZ_RREAR) > kDamageThresh,
                            CARSLOTID_DAMAGE0_REARRIGHT, CARSLOTID_DAMAGE0_REARRIGHT);

    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_FRONT) > kDamageWindowThresh, REPLACETEX_WINDOW_FRONT, bStringHash("WINDOW_FRONT"),
                                 STRINGHASH_WINDOW_DAMAGE0);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_REAR) > kDamageWindowThresh, REPLACETEX_WINDOW_REAR, bStringHash("WINDOW_FRONT"),
                                 STRINGHASH_WINDOW_DAMAGE0);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_LFRONT) > kDamageWindowThresh, REPLACETEX_WINDOW_LEFT_FRONT,
                                 bStringHash("WINDOW_FRONT"), STRINGHASH_WINDOW_DAMAGE0);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_LREAR) > kDamageWindowThresh, REPLACETEX_WINDOW_LEFT_REAR, bStringHash("WINDOW_FRONT"),
                                 STRINGHASH_WINDOW_DAMAGE0);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_RFRONT) > kDamageWindowThresh, REPLACETEX_WINDOW_RIGHT_FRONT,
                                 bStringHash("WINDOW_FRONT"), STRINGHASH_WINDOW_DAMAGE0);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_RREAR) > kDamageWindowThresh, REPLACETEX_WINDOW_RIGHT_REAR,
                                 bStringHash("WINDOW_FRONT"), STRINGHASH_WINDOW_DAMAGE0);
    this->SetCarGlassDamageState(damageInfo.Get(DamageZone::DZ_REAR) > kDamageWindowThresh, REPLACETEX_WINDOW_REAR_DEFOST,
                                 bStringHash("REAR_DEFROSTER"), STRINGHASH_WINDOW_DAMAGE0);
}

void CarRenderInfo::SetCarDamageState(bool on, unsigned int startID, unsigned int endID) {
    bool hidden = !on;

    for (int model_number = 0; model_number < CARPART_MODEL_NUM; model_number++) {
        for (int model_lod = this->mMinLodLevel; model_lod <= this->mMaxLodLevel; model_lod++) {
            CarPartModel *model;
            for (unsigned int i = startID; i <= endID; i++) {
                model = &this->mCarPartModels[i][model_number][model_lod];

                if (model != nullptr && model->GetModel() != nullptr) {
                    model->Hide(static_cast<int>(hidden));
                }
            }
        }
    }
}

void CarRenderInfo::SetCarGlassDamageState(bool on, CarReplacementTexID replacementId, uint32 undamageHash, uint32 damageHash) {
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

uint32 CarRenderInfo::FindCarPart(int slotId) {
    uint32 model_namehash = 0;

    if (slotId <= CARSLOTID_MODEL_NUM) {
        unsigned int lodId = this->mMinLodLevel;
        eModel *model = this->mCarPartModels[slotId][0][lodId].GetModel();
        if (model != nullptr) {
            model_namehash = model->GetNameHash();
        }
    }

    return model_namehash;
}

uint32 CarRenderInfo::HideCarPart(int slotId, bool hide) {
    uint32 model_namehash = 0;

    if (slotId <= CARSLOTID_MODEL_NUM) {
        for (int lodId = this->mMinLodLevel; lodId <= this->mMaxLodLevel; lodId++) {
            CarPartModel *carPartModel = &this->mCarPartModels[slotId][0][lodId];
            eModel *model = carPartModel->GetModel();

            carPartModel->Hide(static_cast<int>(hide));

            if (lodId == this->mMinLodLevel && (model != nullptr)) {
                model_namehash = model->GetNameHash();
            }
        }
    }

    return model_namehash;
}

void CarRenderInfo::UpdateCarParts() {
    ProfileNode profile_node("TODO", 0);

    bInitializeBoundingBox(&this->AABBMin, &this->AABBMax);

    for (int model_index = 0; model_index < CARSLOTID_MODEL_NUM; model_index++) {
        for (int model_number = 0; model_number < CARPART_MODEL_NUM; model_number++) {
            for (int model_lod = this->mMinLodLevel; model_lod <= this->mMaxLodLevel; model_lod++) {
                eModel *model = this->mCarPartModels[model_index][model_number][model_lod].GetModel();

                if (model != nullptr && model->GetNameHash() != 0) {
                    model->UnInit();
                    CarPartModelPool->Free(model);
                    this->mCarPartModels[model_index][model_number][model_lod].SetModel(nullptr);
                }
            }
        }
    }

    RideInfo *ride_info = this->pRideInfo;

    for (int model_index = 0; model_index < CARSLOTID_MODEL_NUM; model_index++) {
        int model_part_id = model_index;
        CarPart *car_part = ride_info->GetPart(model_part_id);

        if (car_part == nullptr) {
            continue;
        }

        for (int model_number = 0; model_number < CARPART_MODEL_NUM; model_number++) {
            for (int model_lod = this->mMinLodLevel; model_lod <= this->mMaxLodLevel; model_lod++) {
                int attempt_lod = model_lod;
                CARPART_LOD special_minimum_lod;
                CARPART_LOD special_maximum_lod;

                if (ride_info->GetSpecialLODRangeForCarSlot(model_part_id, &special_minimum_lod, &special_maximum_lod, IsGameFlowInFrontEnd())) {
                    attempt_lod = bClamp(attempt_lod, special_minimum_lod, special_maximum_lod);
                }

                int attempt_namehash = car_part->GetModelNameHash(model_number, attempt_lod);

                eModel *model = nullptr;
                if (attempt_namehash != 0) {
                    model = static_cast<eModel *>(CarPartModelPool->Malloc());

                    this->mCarPartModels[model_index][model_number][model_lod].SetModel(model);
                    model->Init(attempt_namehash);

                    if (!model->HasSolid()) {
                        model->UnInit();
                        CarPartModelPool->Free(model);
                        this->mCarPartModels[model_index][model_number][model_lod].SetModel(nullptr);
                        model = nullptr;
                    }
                }

                if (model != nullptr) {
                    if (model_index < CARSLOTID_DECAL_FRONT_WINDOW || model_index > CARSLOTID_DECAL_RIGHT_QUARTER) {
                        if (model_index == CARSLOTID_HOOD) {
                            CarPart *hood_part = ride_info->GetPart(CARSLOTID_HOOD);
                            int carbon = hood_part->GetAppliedAttributeIParam(STRINGHASH_CARBONFIBRE, 0);

                            if (carbon != 0) {
                                model->AttachReplacementTextureTable(this->CarbonReplacementTextureTable, REPLACETEX_NUM, 0);
                                this->CarbonHood = 1;
                            } else {
                                model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                                this->CarbonHood = carbon;
                            }
                        } else {
                            model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                        }
                    } else {
                        int decal_model_index = model_index - CARSLOTID_DECAL_FRONT_WINDOW;
                        model->AttachReplacementTextureTable(&this->DecalReplacementTextureTable[decal_model_index * 8], 8, 0);
                    }
                }
            }

            switch (model_index) {
                case CARSLOTID_BASE:
                case CARSLOTID_DAMAGE_BODY:
                case CARSLOTID_DAMAGE_COP_LIGHTS:
                case CARSLOTID_DAMAGE_HOOD:
                case CARSLOTID_DAMAGE_BUSHGUARD:
                case CARSLOTID_DAMAGE_FRONT_BUMPER:
                case CARSLOTID_DAMAGE_TRUNK:
                case CARSLOTID_DAMAGE_REAR_BUMPER:
                case CARSLOTID_BODY:
                case CARSLOTID_LEFT_SIDE_MIRROR:
                case CARSLOTID_RIGHT_SIDE_MIRROR:
                case CARSLOTID_SPOILER:
                case CARSLOTID_ROOF:
                case CARSLOTID_HOOD: {
                    bVector3 bbox_min;
                    bVector3 bbox_max;

                    eModel *model = this->mCarPartModels[model_index][model_number][this->mMinLodLevel].GetModel();
                    if (model != nullptr) {
                        model->GetBoundingBox(&bbox_min, &bbox_max);
                        bExpandBoundingBox(&this->AABBMin, &this->AABBMax, &bbox_min, &bbox_max);
                    }
                    break;
                }
            }
        }
    }

    for (int model_number = 0; model_number < CARPART_MODEL_NUM; model_number++) {
        for (int model_lod = this->mMinLodLevel; model_lod <= this->mMaxLodLevel; model_lod++) {
            {
                CarPartModel *front_wheel = &this->mCarPartModels[CARSLOTID_FRONT_WHEEL][model_number][model_lod];
                CarPartModel *rear_wheel = &this->mCarPartModels[CARSLOTID_REAR_WHEEL][model_number][model_lod];
                eModel *front_wheel_model = front_wheel->GetModel();
                eModel *rear_wheel_model = rear_wheel->GetModel();

                if (front_wheel_model != nullptr && rear_wheel_model == nullptr) {
                    eModel *model = static_cast<eModel *>(CarPartModelPool->Malloc());

                    rear_wheel->SetModel(model);
                    model->Init(front_wheel_model->GetNameHash());

                    if (!model->HasSolid()) {
                        model->UnInit();
                        CarPartModelPool->Free(model);
                        rear_wheel->SetModel(nullptr);
                    } else {
                        model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                    }
                }
            }

            {
                CarPartModel *front_brake = &this->mCarPartModels[CARSLOTID_FRONT_BRAKE][model_number][model_lod];
                CarPartModel *rear_brake = &this->mCarPartModels[CARSLOTID_REAR_BRAKE][model_number][model_lod];
                eModel *front_brake_model = front_brake->GetModel();
                eModel *rear_brake_model = rear_brake->GetModel();

                if (front_brake_model != nullptr && rear_brake_model == nullptr) {
                    eModel *model = static_cast<eModel *>(CarPartModelPool->Malloc());

                    rear_brake->SetModel(model);
                    model->Init(front_brake_model->GetNameHash());

                    if (!model->HasSolid()) {
                        model->UnInit();
                        CarPartModelPool->Free(model);
                        rear_brake->SetModel(nullptr);
                    } else {
                        model->AttachReplacementTextureTable(this->MasterReplacementTextureTable, REPLACETEX_NUM, 0);
                    }
                }
            }
        }
    }

    {
        eModel *front_wheel_model = this->mCarPartModels[CARSLOTID_FRONT_WHEEL][0][this->mMinLodLevel].GetModel();
        eModel *rear_wheel_model = this->mCarPartModels[CARSLOTID_REAR_WHEEL][0][this->mMinLodLevel].GetModel();
        bVector3 bbox_min;
        bVector3 bbox_max;

        if (front_wheel_model != nullptr) {
            front_wheel_model->GetBoundingBox(&bbox_min, &bbox_max);
            this->WheelWidths[0] = UMath::Abs(bbox_max.y - bbox_min.y);
            this->WheelRadius[0] = UMath::Abs(bbox_max.x - bbox_min.x) * 0.5f;
        }

        if (rear_wheel_model != nullptr) {
            rear_wheel_model->GetBoundingBox(&bbox_min, &bbox_max);
            this->WheelWidths[1] = this->WheelRadius[1] = UMath::Abs(bbox_max.y - bbox_min.y);
            this->WheelRadius[1] = UMath::Abs(bbox_max.x - bbox_min.x) * 0.5f;
        }
    }

    this->ModelOffset = (this->AABBMax + this->AABBMin) * 0.5f;

    CarPart *base_part = ride_info->GetPart(CARSLOTID_BASE);
    if (base_part != nullptr) {
        eSolid *base_solid = eFindSolid(base_part->GetModelNameHash(0, this->mMinLodLevel));

        if (base_solid != nullptr) {
            this->SpoilerPositionMarker = base_solid->GetPostionMarker(STRINGHASH_SPOILER);
            this->SpoilerPositionMarker2 = base_solid->GetPostionMarker(STRINGHASH_SPOILER2);
            this->RoofScoopPositionMarker = base_solid->GetPostionMarker(STRINGHASH_ROOF_SCOOP);
        } else {
            this->SpoilerPositionMarker = nullptr;
            this->SpoilerPositionMarker2 = nullptr;
            this->RoofScoopPositionMarker = nullptr;
        }
    } else {
        this->SpoilerPositionMarker = nullptr;
        this->SpoilerPositionMarker2 = nullptr;
        this->RoofScoopPositionMarker = nullptr;
    }

    {
        bool hidden = true;
        CarPart *base_part = ride_info->GetPart(CARSLOTID_SPOILER);

        if (base_part != nullptr) {
            hidden = base_part->GetUpgradeLevel() == 0;
        }

        for (int model_number = 0; model_number < CARPART_MODEL_NUM; model_number++) {
            for (int model_lod = this->mMinLodLevel; model_lod <= this->mMaxLodLevel; model_lod++) {
                CarPartModel *spoiler_base = &this->mCarPartModels[CARSLOTID_UNIVERSAL_SPOILER_BASE][model_number][model_lod];

                spoiler_base->Hide(static_cast<int>(hidden));
            }
        }
    }

    this->SetCarDamageState(false, CARSLOTID_DAMAGE0_FRONT, CARSLOTID_DAMAGE0_REARRIGHT);
}

int TweakKitWheelOffsetFront = 0;
int TweakKitWheelOffsetRear = 0;

// UNSOLVED
void CarRenderInfo::UpdateWheelYRenderOffset() {
    if (this->pCarTypeInfo == nullptr) {
        bMemSet(this->WheelYRenderOffset, 0, sizeof(this->WheelYRenderOffset));
        return;
    }

    CarPart *front_wheel;
    CarPart *rear_wheel;
    int front_upgrade_level;
    int rear_upgrade_level;
    if (this->pRideInfo != nullptr) {
        front_wheel = this->pRideInfo->GetPart(CARSLOTID_FRONT_WHEEL);

        if (front_wheel != nullptr) {
            front_upgrade_level = front_wheel->GetUpgradeLevel();
        }
    }
    if (this->pRideInfo != nullptr) {
        rear_wheel = this->pRideInfo->GetPart(CARSLOTID_REAR_WHEEL);

        if (rear_wheel != nullptr) {
            rear_upgrade_level = rear_wheel->GetUpgradeLevel();
        }
    }

    UMath::Vector4 tire_offset;
    for (int wheel = 0; wheel < 4; wheel++) {
        int wheel_end = static_cast<int>(wheel > 1);

        this->WheelYRenderOffset[wheel] = -this->mAttributes.TireOffsets(wheel).y;

        int kit_number = 0;
        CarPart *body_part = this->pRideInfo->GetPart(CARSLOTID_BODY);

        if (body_part != nullptr) {
            kit_number = body_part->GetAppliedAttributeIParam(STRINGHASH_KITNUMBER, 0);
        }

        int kit_wheel_offset;
        if (wheel_end == 0) {
            if (TweakKitWheelOffsetFront == 0) {
                kit_wheel_offset = this->mAttributes.KitWheelOffsetFront(kit_number);
            } else {
                kit_wheel_offset = TweakKitWheelOffsetFront;
            }
        } else if (TweakKitWheelOffsetRear == 0) {
            kit_wheel_offset = this->mAttributes.KitWheelOffsetRear(kit_number);
        } else {
            kit_wheel_offset = TweakKitWheelOffsetRear;
        }

        float kit_wheel_offset_float = static_cast<float>(kit_wheel_offset) * 0.001f;
        if (this->WheelYRenderOffset[wheel] <= 0.0f) {
            this->WheelYRenderOffset[wheel] -= kit_wheel_offset_float;
        } else {
            this->WheelYRenderOffset[wheel] += kit_wheel_offset_float;
        }

        float model_width = this->WheelWidths[wheel_end];
        float model_radius = this->WheelRadius[wheel_end];
        float desired_width = this->mAttributes.TireSkidWidth(wheel);

        if (wheel <= 1) {
            desired_width *= this->mAttributes.TireSkidWidthKitScale(kit_number).x;
        } else {
            desired_width *= this->mAttributes.TireSkidWidthKitScale(kit_number).y;
        }

        this->mAttributes.TireOffsets(tire_offset, wheel);
        float desired_radius = tire_offset.w;

        if (model_width > 0.0f && desired_width > 0.0f) {
            this->WheelWidthScales[wheel] = desired_width / model_width;
        } else {
            this->WheelWidthScales[wheel] = TweakWheelWidthScale;
        }

        if (model_radius > 0.0f && desired_radius > 0.0f) {
            this->WheelRadiusScales[wheel] = desired_radius / model_radius;
        } else {
            this->WheelRadiusScales[wheel] = TweakWheelRadiusScale;
        }
    }
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

void CarRenderInfo::UpdateDecalTextures(RideInfo *ride_info) {
    unsigned int alpha_hash = bStringHash("DEFAULTALPHA");

    for (int i = REPLACETEX_DECAL_START; i <= REPLACETEX_DECAL_END; i++) {
        this->MasterReplacementTextureTable[i].SetOldNameHash(CarReplacementDecalHash[i - REPLACETEX_DECAL_START]);
        this->MasterReplacementTextureTable[i].SetNewNameHash(alpha_hash);
    }

    unsigned int decal_hashes[8] = {
        bStringHash("DUMMY_DECAL1"), bStringHash("DUMMY_DECAL2"), bStringHash("DUMMY_DECAL3"),      bStringHash("DUMMY_DECAL4"),
        bStringHash("DUMMY_DECAL5"), bStringHash("DUMMY_DECAL6"), bStringHash("DUMMY_NUMBER_LEFT"), bStringHash("DUMMY_NUMBER_RIGHT"),
    };

    for (int i = 0; i < NUM_ELEMENTS(this->DecalReplacementTextureTable); i++) {
        this->DecalReplacementTextureTable[i].SetOldNameHash(decal_hashes[i % 8]);
        this->DecalReplacementTextureTable[i].SetNewNameHash(alpha_hash);
    }

    int hood_decals = 1;
    if (ride_info->GetPart(CARSLOTID_HOOD) == nullptr) {
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

        if (decal_model_part != nullptr && hood_decals != 0 && decal_model_part->HasAppliedAttribute(size_hash) != 0 &&
            decal_model_part->HasAppliedAttribute(shape_hash) != 0) {
            unsigned int decal_size = decal_model_part->GetAppliedAttributeUParam(size_hash, 0);
            unsigned int decal_shape = decal_model_part->GetAppliedAttributeUParam(shape_hash, 0);
            eReplacementTextureTable *replace_table = &this->DecalReplacementTextureTable[decal_index * 8];
            int first_tex_part = CARSLOTID_DECAL_FRONT_WINDOW_TEX0 + decal_index * 8;

            for (int j = 0; j < 8; j++) {
                CarPart *decal_texture_part = ride_info->GetPart(j + first_tex_part);

                if (decal_texture_part != nullptr) {
                    char buf[128];
                    unsigned int base_hash = decal_texture_part->GetAppliedAttributeUParam(bStringHash("NAME"), 0);
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

void CarRenderInfo::UpdateCarReplacementTextures() {
    bMemCpy(this->CarbonReplacementTextureTable, this->MasterReplacementTextureTable, sizeof(this->CarbonReplacementTextureTable));

    this->CarbonReplacementTextureTable[REPLACETEX_CARSKIN].SetNewNameHash(bStringHash("CARBONFIBRE"));
    this->CarbonReplacementTextureTable[REPLACETEX_CARSKINB].SetNewNameHash(this->mUsedTextureInfos.ReplaceGlobalHash);
    this->CarbonReplacementTextureTable[REPLACETEX_GLOBALSKIN].SetNewNameHash(this->mUsedTextureInfos.ReplaceGlobalHash);
    this->CarbonReplacementTextureTable[REPLACETEX_CARBONSKIN].SetNewNameHash(bStringHash("CARBONFIBRE"));
    this->CarbonReplacementTextureTable[REPLACETEX_GLOBALCARBONSKIN].SetNewNameHash(bStringHash("CARBONFIBRE"));
}

// UNSOLVED
void CarRenderInfo::UpdateLightStateTextures() {
    bool lights_always_on = true;

    {
        int left_light_state = 0;
        if (lights_always_on) {
            left_light_state = 1;
        }
        int right_light_state = 0;
        if (lights_always_on) {
            right_light_state = 1;
        }
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

// TODO regswap
void CarRenderInfo::CreateCarLightFlares() {
    int found_STRINGHASH_LEFT_REVERSE_slot;
    int found_STRINGHASH_RIGHT_REVERSE_slot;

    if (this->pCarTypeInfo != nullptr) {
        found_STRINGHASH_LEFT_REVERSE_slot = -1;
        found_STRINGHASH_RIGHT_REVERSE_slot = -1;

        for (int slotIndex = CARSLOTID_MODEL_NUM - 1; slotIndex >= 0; slotIndex--) {
            {
                eModel *model = this->mCarPartModels[slotIndex][0][this->mMinLodLevel].GetModel();
                ePositionMarker *position_marker = nullptr;

                if (model == nullptr) {
                    continue;
                }

                while ((position_marker = model->GetPostionMarker(position_marker)) != nullptr) {
                    unsigned int position_marker_namehash = position_marker->NameHash;
                    const char *type_name;
                    int flare_type;

                    switch (position_marker_namehash) {
                        case STRINGHASH_LEFT_HEADLIGHT:
                        case STRINGHASH_CENTRE_HEADLIGHT:
                        case STRINGHASH_RIGHT_HEADLIGHT:
                            flare_type = 0;
                            break;
                        case STRINGHASH_LEFT_BRAKELIGHT:
                        case STRINGHASH_CENTRE_BRAKELIGHT:
                        case STRINGHASH_RIGHT_BRAKELIGHT:
                            flare_type = 1;
                            break;
                        case STRINGHASH_COPLIGHTRED:
                            flare_type = 5;
                            break;
                        case STRINGHASH_COPLIGHTBLUE:
                            flare_type = 6;
                            break;
                        case STRINGHASH_COPLIGHTWHITE:
                            flare_type = 7;
                            break;
                        case STRINGHASH_COPLIGHTBRIGHTRED:
                            flare_type = 10;
                            break;
                        case STRINGHASH_COPLIGHTBRIGHTBLUE:
                            flare_type = 11;
                            break;
                        case STRINGHASH_COPLIGHTORANGE:
                            flare_type = 12;
                            break;
                        case STRINGHASH_LEFT_REVERSE:
                            if (found_STRINGHASH_LEFT_REVERSE_slot != slotIndex && found_STRINGHASH_LEFT_REVERSE_slot > 0) {
                                continue;
                            }
                            found_STRINGHASH_LEFT_REVERSE_slot = slotIndex;
                            flare_type = 3;
                            break;
                        case STRINGHASH_RIGHT_REVERSE:
                            if (found_STRINGHASH_RIGHT_REVERSE_slot != slotIndex && found_STRINGHASH_RIGHT_REVERSE_slot > 0) {
                                continue;
                            }
                            found_STRINGHASH_RIGHT_REVERSE_slot = slotIndex;
                            flare_type = 3;
                            break;
                        default:
                            flare_type = -1;
                            break;
                    }

                    if (flare_type != -1) {
                        eLightFlare *light_flare = new eLightFlare;

                        light_flare->NameHash = position_marker_namehash;
                        light_flare->Type = static_cast<char>(flare_type);
                        if ((flare_type >= ELF_CAR_COPLIGHTRED && flare_type <= ELF_CAR_COPLIGHTWHITE) || flare_type == ELF_CAR_COPLIGHTBRIGHTRED ||
                            flare_type == ELF_CAR_COPLIGHTBRIGHTBLUE || flare_type == ELF_CAR_COPLIGHTORANGE) {
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
}

static const float fTireBlurAngVelMin = 0.0f;
static const float fTireBlurAngVelMax = 50.0f;
static const float fTireBlurAlphaMax = 70.0f;

float blurOverRide = 0.0f;

// STRIPPED
float CarRenderInfo::CalculateTireBlur(int nWheel) {}

// STRIPPED
void CarRenderInfo::RenderTireBlur(eView *view, TextureInfo *texture_info, struct bMatrix4 *local_world, float alpha,
                                   eDynamicLightContext *light_context) {}

// STRIPPED
int IsCarBlinkingOrPoofed(Car *car, eView *view) {}

// STRIPPED
int ShouldCarBeRendered(Car *car, Camera *camera, eView *view) {}

float hOffX = 13.0f;
float hOffY = 0.0f;
float hOffZ = 0.05f;
float hOffZ00 = 0.1f;
float hOffZ11 = 0.1f;
float hOffZ22 = 0.1f;
float hOffZ33 = 0.1f;
float hRad1x = 10.0f;
float hRad2x = 10.0f;
float hRad1y = 4.9f;
float hRad2y = 4.9f;
float hRad0x = 10.0f;
float hRad3x = 10.0f;
float hRad0y = 4.9f;
float hRad3y = 4.9f;

bMatrix4 IdMat;

uint32 TexturedHeadLightsON = 1;
uint32 TexturedHeadLightsAccuracy = 0;

float bbmul = 10.0f;

unsigned int hcL = 0x80CCCCCC;

void CarRenderInfo::RenderTextureHeadlights(eView *view, bMatrix4 *l_w, unsigned int Accurate) {
    bMatrix4 *local_world = eFrameMallocMatrix(1);

    *local_world = *l_w;

    bVector3 Up(0.0f, 0.0f, 1.0f);
    bVector3 Basis(local_world->v0.z, local_world->v1.z, local_world->v2.z);
    float hOffZ0 = 0.0f;
    float hOffZ1 = 0.0f;
    float hOffZ2 = 0.0f;
    float hOffZ3 = 0.0f;
    float hOffMID12 = 1.0f;
    float hOffMID03 = 0.0f;
    float hOffMID13 = 1.0f;

    if (bDot(&Up, &Basis) < 0.707f) {
        return;
    }

    ePoly HeadLightPoly;
    TextureInfo *pTextureInfo2PlayerHeadlight1 = GetTextureInfo(bStringHash("2PLAYERHEADLIGHT1"), 1, 0);

    HeadLightPoly.Vertices[0] = bVector3(-(hRad0x - hOffX), -(hRad0y - hOffY), hOffZ0);
    HeadLightPoly.Vertices[1] = bVector3(hRad1x + hOffX, hOffY - hRad1y, hOffZ1);
    HeadLightPoly.Vertices[3] = bVector3(hOffX - hRad3x, hRad3y + hOffY, hOffZ3);
    HeadLightPoly.Vertices[2] = bVector3(hRad2x + hOffX, hRad2y + hOffY, hOffZ2);

    reinterpret_cast<uint32 *>(HeadLightPoly.Colours)[0] = hcL;
    reinterpret_cast<uint32 *>(HeadLightPoly.Colours)[1] = hcL;
    reinterpret_cast<uint32 *>(HeadLightPoly.Colours)[2] = hcL;
    reinterpret_cast<uint32 *>(HeadLightPoly.Colours)[3] = hcL;

    view->Render(&HeadLightPoly, pTextureInfo2PlayerHeadlight1, local_world, 0, 0.0f);
}

float cpr = 1.0f;
float cpb = 1.0f;
float cpw = 1.f;
float copm = 12.0f;
float copt = 1.5f;
int copModulo = 19;
float copWhitemul = 3.0f;
float copoffsetr = 0.0f;
float copoffsetb = M_PI;
float copoffsetw = 4.0f;

static const int CopLightsAlwaysON = 0;

float coplightflicker(float time, int offset) {
    static int counter = 0;
    counter = (counter + 1) % copModulo;

    return bCos((time + copt * static_cast<float>(offset)) * copm);
}

static const float FlarePeriodLong = static_cast<float>(M_PI_4);
static const float FlarePeriodShort = FlarePeriodLong / 3.0f;
static const bool FlareCopWhiteOn = false;

float coplightflicker2(float time, int whichColor, int flarecount) {
    static int counter = 0;

    counter = (counter + 1) % copModulo;

    float offset;
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

    // TODO 'a' regalloc
    float a;
    float t;
    t = bCos((static_cast<float>(M_TWOPI) / FlarePeriodShort) * time);
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

bool gTWEAKER_NISLightEnabled = false;
float gTWEAKER_NISLightIntensity = 1.0f;
float gTWEAKER_NISLightPosX = 0.0f;
float gTWEAKER_NISLightPosY = 0.0f;
float gTWEAKER_NISLightPosZ = 0.0f;
static const bool gTWEAKER_NISLightGrabPos = false;

float HackTime = 0.0f;

int Lightslot = 3;

// UNSOLVED, just scheduling, our variant doesn't use r1+0x20
void CarRenderInfo::RenderFlaresOnCar(eView *view, const bVector3 *position, const bMatrix4 *body_matrix, int force_light_state, int reflexion,
                                      int renderFlareFlags) {
    ProfileNode profile_node("TODO", 0);
    float Ftime = Sim::GetTime() + this->CarTimebaseStart;
    Camera *camera = view->GetCamera();
    int in_front_end = static_cast<int>(IsGameFlowInFrontEnd());
    bMatrix4 *local_world = eFrameMallocMatrix(1);
    bMatrix4 *world_view = camera->GetCameraMatrix();

    if (local_world == nullptr) {
        return;
    }

    bCopy(local_world, body_matrix);
    local_world->v3.x = position->x;
    local_world->v3.y = position->y;
    local_world->v3.z = position->z;
    local_world->v3.w = 1.0f;

    if (reflexion) {
        this->RenderTextureHeadlights(view, local_world, 0);
    }

    if (this->pCarTypeInfo != nullptr && this->pCarTypeInfo->GetCarUsageType() == CAR_USAGE_TYPE_COP) {
#ifndef EA_BUILD_A124
        if (this->IsLightOn(VehicleFX::LIGHT_COPRED)) {
            view->NumCopsCherry++;
        }
#endif
    }

    int car_pixel_size = view->GetPixelSize(position, this->mRadius);
    if (eGetCurrentViewMode() == EVIEWMODE_TWOH) {
        car_pixel_size = static_cast<int>(car_pixel_size * CarTwoPlayerLODFactor);
    }

    if (car_pixel_size < view->GetPixelMinSize()) {
        return;
    }

    unsigned int visibility_state = view->GetVisibleState(&this->AABBMin, &this->AABBMax, local_world);
    if (visibility_state == EVISIBLESTATE_NOT) {
        return;
    }

    float base_headlight_intensity;
    float base_brakelight_intensity;
    CarTypeInfo *car_type_info = GetCarTypeInfo(this->pRideInfo->Type);
    int is_traffic_car = static_cast<int>(car_type_info->GetCarUsageType() == CAR_USAGE_TYPE_TRAFFIC);

    if (is_traffic_car != 0) {
        base_brakelight_intensity = TrafficBrakelightBaseIntensity;
        base_headlight_intensity = 1.0f;
    } else {
        base_headlight_intensity = 1.0f;
        base_brakelight_intensity = 0.5f;
    }

    float headlight_left_intensity;
    if (UTL::Collections::Singleton<INIS>::Get() == nullptr) {
        headlight_left_intensity = base_headlight_intensity;
    } else {
        headlight_left_intensity = 0.0f;
    }

    float headlight_right_intensity;
    if (UTL::Collections::Singleton<INIS>::Get() == nullptr) {
        headlight_right_intensity = base_headlight_intensity;
    } else {
        headlight_right_intensity = 0.0f;
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
        brakelight_centre_intensity += 1.0f;
        brakelight_right_intensity += 1.0f;
    }

    float intensity = BrakeLightScaleFactor;

    if (this->IsLightOn(VehicleFX::LIGHT_LHEAD)) {
        headlight_left_intensity = base_headlight_intensity;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_LHEAD)) {
        headlight_left_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_RHEAD)) {
        headlight_right_intensity = base_headlight_intensity;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_RHEAD)) {
        headlight_right_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_LREVERSE)) {
        reverselight_left_intensity += 1.0f;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_LREVERSE)) {
        reverselight_left_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_LREVERSE)) {
        reverselight_right_intensity += 1.0f;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_LREVERSE)) {
        reverselight_right_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_LBRAKE)) {
        brakelight_left_intensity += intensity;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_LBRAKE)) {
        brakelight_left_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_RBRAKE)) {
        brakelight_right_intensity += intensity;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_RBRAKE)) {
        brakelight_right_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_CBRAKE)) {
        brakelight_centre_intensity = intensity + base_brakelight_intensity;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_CBRAKE)) {
        brakelight_centre_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_LREVERSE)) {
        reverselight_left_intensity += intensity;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_LREVERSE)) {
        reverselight_left_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_RREVERSE)) {
        reverselight_right_intensity += intensity;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_RREVERSE)) {
        reverselight_right_intensity = 0.0f;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_COPRED)) {
        coplight_intensityR = cpr;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_COPBLUE)) {
        coplight_intensityB = cpb;
    }
    if (this->IsLightOn(VehicleFX::LIGHT_COPWHITE)) {
        coplight_intensityW = cpw;
        flashHeadlights = 1;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_COPRED)) {
        coplight_intensityR = 0.0f;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_COPBLUE)) {
        coplight_intensityB = 0.0f;
    }
    if (this->IsLightBroken(VehicleFX::LIGHT_COPWHITE)) {
        coplight_intensityW = 0.0f;
    }

    CarPart *preview_part;
    if (this->pRideInfo != nullptr) {
        preview_part = this->pRideInfo->GetPreviewPart();
    } else {
        preview_part = nullptr;
    }

    CAR_PART_ID preview_part_id;
    if (preview_part != nullptr) {
        preview_part_id = static_cast<CAR_PART_ID>(*reinterpret_cast<signed char *>(reinterpret_cast<unsigned char *>(preview_part) + 4));
    } else {
        preview_part_id = CARPARTID_NUM;
    }

    int FlareCount = 0;
    float constFlicker = coplightflicker(Ftime, 0);

    for (eLightFlare *light_flare = this->LightFlareList.GetHead(); light_flare != this->LightFlareList.EndOfList();
         light_flare = light_flare->GetNext()) {
        unsigned int name_hash = light_flare->NameHash;
        int is_brakelight = 0;
        int is_headlight = 0;
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
            sizescale = 10.0f;
        }

        switch (name_hash) {
            case STRINGHASH_LEFT_HEADLIGHT:
                intensity = headlight_left_intensity;
                if (flashHeadlights != 0) {
                    intensity *= constFlicker;
                }
                break;
            case STRINGHASH_RIGHT_HEADLIGHT:
                intensity = headlight_right_intensity;
                if (flashHeadlights != 0) {
                    intensity *= 1.0f - constFlicker;
                }
                break;
            case STRINGHASH_LEFT_BRAKELIGHT:
                intensity = brakelight_left_intensity;
                break;
            case STRINGHASH_CENTRE_BRAKELIGHT:
                intensity = brakelight_centre_intensity;
                break;
            case STRINGHASH_RIGHT_BRAKELIGHT:
                intensity = brakelight_right_intensity;
                break;
            case STRINGHASH_LEFT_REVERSE:
                intensity = reverselight_left_intensity;
                break;
            case STRINGHASH_RIGHT_REVERSE:
                intensity = reverselight_right_intensity;
                break;
            case STRINGHASH_COPLIGHTRED:
                coplight_intensityR *= coplightflicker2(Ftime, 0, FlareCount);
                intensity = coplight_intensityR;
                break;
            case STRINGHASH_COPLIGHTBLUE:
                coplight_intensityB *= coplightflicker2(Ftime, 1, FlareCount);
                intensity = coplight_intensityB;
                break;
            case STRINGHASH_COPLIGHTBRIGHTRED:
                intensity = coplight_intensityR * coplightflicker2(Ftime, 0, FlareCount);
                break;
            case STRINGHASH_COPLIGHTBRIGHTBLUE:
                intensity = coplight_intensityB * coplightflicker2(Ftime, 1, FlareCount);
                break;
            case STRINGHASH_COPLIGHTWHITE:
                intensity = bSin(coplight_intensityW * coplightflicker2(Ftime, 2, FlareCount) * copWhitemul);
                break;
            case STRINGHASH_COPLIGHTORANGE:
                intensity = 1.0f;
                break;
            default:
                break;
        }

        if ((preview_part_id == CARPARTID_BRAKELIGHT && is_brakelight != 0) || (preview_part_id == CARPARTID_HEADLIGHT && is_headlight != 0)) {
            intensity = 0.0f;
        }

        if (intensity > 0.0f) {
            if (intensity > 1.0f) {
                intensity = 1.0f;
            }

            if (!reflexion) {
                if ((renderFlareFlags & 1) != 0) {
                    eRenderLightFlare(view, light_flare, local_world, intensity, REF_NONE, FLARE_ENV, 0.0f, 0, sizescale);
                } else {
                    eRenderLightFlare(view, light_flare, local_world, intensity, REF_NONE, FLARE_NORM, 0.0f, 0, sizescale);
                }
            } else {
                eRenderLightFlare(view, light_flare, local_world, intensity, REF_TOPO, FLARE_REFLECT, this->mCar_elevation, 0, 1.0f);
            }
        }

        FlareCount++;
    }

    if (view->GetID() == EVIEW_FIRST_PLAYER && !reflexion) {
        bVector3 *lightPosition = const_cast<bVector3 *>(position);
        float extraIntensity = 1.0f;
        bVector3 NisLightPosition(gTWEAKER_NISLightPosX + position->x, gTWEAKER_NISLightPosY + position->y, gTWEAKER_NISLightPosZ + position->z);

        if (gTWEAKER_NISLightEnabled) {
            lightPosition = &NisLightPosition;
            extraIntensity = gTWEAKER_NISLightIntensity;
        }

        if (coplight_intensityR > 0.0f || coplight_intensityB > 0.0f) {
            if (coplight_intensityR > 0.0f) {
                AddQuickDynamicLight(&ShaperLightsCharacters, Lightslot, 0.8f, 0.2f, 0.0f, coplight_intensityR * extraIntensity, lightPosition);
            } else if (coplight_intensityB > 0.0f) {
                AddQuickDynamicLight(&ShaperLightsCharacters, Lightslot, 0.2f, 0.2f, 0.8f, coplight_intensityB * extraIntensity, lightPosition);
            }
        } else {
            RestoreShaperRig(&ShaperLightsCharacters, Lightslot, &ShaperLightsCharactersBackup);
        }
    }
}

float enX = .0f;
float enY = 1.0f;
float enZ = 0.0f;

float FaceCos = 0.0f;
uint32 TireFaceIt = 1;

float TireFace(bMatrix4 *matrix, eView *view) {
    if (TireFaceIt == 0) {
        return 1.0f;
    }
    bVector3 cDir(*view->GetCamera()->GetDirection());
    bMatrix4 matrix2(*matrix);

    matrix2.v3.x = matrix2.v3.y = matrix2.v3.z = 0.0f;
    matrix2.v3.w = 1.0f;

    bVector3 N(enX, enY, enZ);

    eMulVector(&N, &matrix2, &N);
    return bDot(&N, &cDir);
}

static const float WheelCamberMaxAngle = 7.0f;
static const float WheelCamberMaxPushDown = 0.03f;
static const float WheelWobbleMaxAngle = 16.0f;

int PartDamage_Test = 0;
static const int PartDamage_Level = 0;
static const float PartDamage_X = 0.0f;
static const float PartDamage_Y = 0.0f;
static const float PartDamage_Z = 0.0f;
static const float PartDamage_Intens = 0.0f;
static const uint32 CullBFace = 1;
static const uint32 shadowBias = 1;

// UNSOLVED
bool CarRenderInfo::Render(eView *view, const bVector3 *world_position, const bMatrix4 *body_matrix, bMatrix4 *tire_matrices,
                           bMatrix4 *brake_matrices, bMatrix4 *spinner_matrices, unsigned int extra_render_flags, int force_light_state,
                           int reflexion, float shadow_scale, enum CARPART_LOD tireLOD, enum CARPART_LOD carLOD) {
    ProfileNode profile_node("TODO", 0);
    bVector3 position(*world_position);

#ifndef EA_BUILD_A124
    if (this->pCarTypeInfo != nullptr && this->pCarTypeInfo->GetCarUsageType() == CAR_USAGE_TYPE_COP) {
        view->NumCopsTotal++;
    }
#endif
    int car_pixel_size = view->GetPixelSize(&position, this->mRadius);
    if (eGetCurrentViewMode() == EVIEWMODE_TWOH && !IsGameFlowInFrontEnd()) {
        car_pixel_size = static_cast<int>(static_cast<float>(car_pixel_size) * CarTwoPlayerLODFactor);
    }
    if (car_pixel_size < view->GetPixelMinSize()) {
        if (static_cast<unsigned int>(view->ID - 13) > 1) {
            return false;
        }
    }
    if (DrawCars == 0) {
        return true;
    }

    Camera *camera = view->GetCamera();
    int is_traffic_car;
    if (this->pCarTypeInfo != nullptr) {
        is_traffic_car = static_cast<int>(this->pCarTypeInfo->GetCarUsageType() == CAR_USAGE_TYPE_TRAFFIC);
    } else {
        is_traffic_car = 0;
    }

    eDynamicLightContext *light_context = static_cast<eDynamicLightContext *>(eFrameMalloc(sizeof(eDynamicLightContext)));
    bMatrix4 *local_world = eFrameMallocMatrix(1);
    bMatrix4 *cpy_local_world = eFrameMallocMatrix(1);
    bMatrix4 *biased_local_world = eFrameMallocMatrix(1);
    bMatrix4 *biased_identity = eFrameMallocMatrix(1);

    if (light_context == nullptr || local_world == nullptr || biased_local_world == nullptr || biased_identity == nullptr) {
        return false;
    }

    bMatrix4 *world_view = camera->GetCameraMatrix();
    bCopy(local_world, body_matrix);
    eMulMatrix(local_world, &CarScaleMatrix, local_world);
    local_world->v3.x = position.x;
    local_world->v3.y = position.y;
    local_world->v3.z = position.z;
    local_world->v3.w = 1.0f;

    unsigned int visibility_state = view->GetVisibleState(&this->AABBMin, &this->AABBMax, local_world);
    if (visibility_state == EVISIBLESTATE_NOT) {
        return false;
    }

    bVector4 camera_world_position;
    bVector3 *eye = camera->GetPosition();
    camera_world_position.x = eye->x;
    camera_world_position.y = eye->y;
    camera_world_position.z = eye->z;
    camera_world_position.w = 1.0f;

    Player *player1 = Player::GetPlayerByIndex(0);
    int in_front_end = static_cast<int>(IsGameFlowInFrontEnd());
    if (PrintQueryLightMat != 0) {
        PrintLightQuery = 1;
    }

    bCopy(cpy_local_world, body_matrix);
    cpy_local_world->v3.x = position.x;
    cpy_local_world->v3.y = position.y;
    cpy_local_world->v3.z = position.z;
    cpy_local_world->v3.w = 1.0f;

    eDynamicLightContext base_light_context;
    elResetLightContext(&base_light_context);

    {
        eShaperLightRig *shaper_lights;
        switch (FEManager::Get()->GetGarageType()) {
            // case GARAGETYPE_MAIN_FE:
            //     shaper_lights = &ShaperLightsQRace;
            //     break;
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
        if (IsGameFlowInGame()) {
            shaper_lights = &ShaperLightsCarsInGame;
        }

        elSetupLights(&base_light_context, shaper_lights, &position, nullptr, &hack_man_matrix, view);
    }
    elCloneLightContext(light_context, cpy_local_world, &hack_man_matrix, &camera_world_position, view, &base_light_context);
    this->CarFrame = eFrameCounter;
    if (PrintQueryLightMat != 0) {
        PrintLightQuery = 0;
    }

    unsigned int disable_env_flag = 0;
    unsigned int disable_env_flag_tires = 0;
    this->UpdateLightStateTextures();
    if (EnableEnvMap == 0 || static_cast<float>(car_pixel_size) < DisableEnvMapSize) {
        disable_env_flag = 1;
        disable_env_flag_tires = 1;
    }

    int bodyLodIx = 0;
    int tireLodIx = 0;

    if (is_traffic_car || this->mMinLodLevel == CARPART_LOD_C) {
        if (is_traffic_car) {
            extra_render_flags = 0x80000;
        }
        while (static_cast<float>(car_pixel_size) < TrafficCarBodyLodSwapSize[bodyLodIx]) {
            bodyLodIx++;
            tireLodIx++;
        }
    } else {
        while (static_cast<float>(car_pixel_size) < CarBodyLodSwapSize[bodyLodIx]) {
            bodyLodIx++;
            tireLodIx++;
        }
    }

    int car_body_lod = bMin(this->mMaxLodLevel, bodyLodIx + this->mMinLodLevel);
    int car_tire_lod = bMin(this->mMaxLodLevel, tireLodIx + this->mMinLodLevel);
    int force_show_car_lod_screen_print;

    if (ForceCarLOD != CARPART_LOD_OFF) {
        car_body_lod = bClamp(ForceCarLOD, this->mMinLodLevel, this->mMaxLodLevel);
    }
    if (ForceTireLOD != CARPART_LOD_OFF) {
        car_tire_lod = bClamp(ForceTireLOD, this->mMinLodLevel, this->mMaxLodLevel);
    }

    if (car_body_lod == CARPART_LOD_OFF && car_tire_lod == CARPART_LOD_OFF) {
        return false;
    }

    if (this->pRideInfo != nullptr) {
        CarTypeInfo *car_type_info = this->pCarTypeInfo;
        if (GetCarTypeInfo(this->pRideInfo->Type)->GetCarUsageType() != CAR_USAGE_TYPE_RACING) {
            extra_render_flags |= 0x800;
            if (INIS::Exists()) {
                extra_render_flags |= 0x80000;
            }
        }
    }

    bMatrix4 world_local;
    bVector3 camera_eye_in_car_space;
    bInvertMatrix(&world_local, local_world);
    bMulMatrix(&camera_eye_in_car_space, &world_local, camera->GetPosition());
    float fDistanceToCamera = bDistBetween(camera->GetPosition(), reinterpret_cast<bVector3 *>(&local_world->v3));

    if (car_body_lod == CARPART_LOD_A) {
        if (fDistanceToCamera < 4.0f && view->GetID() == EVIEW_FIRST_PLAYER && !INIS::Exists()) {
            camera_eye_in_car_space.z += 0.5f;
            if (bBoundingBoxIsInside(&this->AABBMin, &this->AABBMax, &camera_eye_in_car_space, 0.0f) != 0) {
                return false;
            }
            camera_eye_in_car_space.z -= 0.5f;
        }
    }

    if (this->mFlashing == true && this->mFlashInterval > 0.0f) {
        return true;
    }

    bCopy(biased_local_world, local_world);
    bCopy(biased_identity, eGetIdentityMatrix(), &position);

    bool nisPlaying = false;
    INIS *nis = INIS::Get();
    if (nis != nullptr) {
        nisPlaying = nis->IsPlaying();
    }

    if (reflexion == 0) {
        biased_local_world->v3.z += fCarFudgeElevation;
        float fBias = (fDistanceToCamera - fCarFudgeBias) / fDistanceToCamera;
        if (in_front_end) {
            view->BiasMatrixForZSorting(biased_local_world, fBias);
        }
        view->BiasMatrixForZSorting(biased_identity, fBias);

        bMatrix4 pre_translate;
        bIdentity(&pre_translate);
        pre_translate.v3.x = -position.x;
        pre_translate.v3.y = -position.y;
        pre_translate.v3.z = -position.z;
        eMulMatrix(biased_identity, &pre_translate, biased_identity);
    }

    unsigned short steerAngleR = this->mSteeringR;
    unsigned short tangL = this->mSteeringL;
    unsigned short tangR = steerAngleR;
    unsigned short steerAngle;
    if (steerAngleR > 0x8000) {
        tangR = static_cast<unsigned short>(-steerAngleR);
    }
    if (tangL > 0x8000) {
        tangL = static_cast<unsigned short>(-tangL);
    }
    steerAngle = this->mSteeringL;
    int tangR_compare = tangR;
    int tangL_compare = tangL;
    if (tangR_compare > tangL_compare) {
        steerAngle = steerAngleR;
    }
    this->TheCarPartCuller.CullParts(&camera_eye_in_car_space, steerAngle);

    unsigned int body_render_flags;
    if (DrawCarShadow != 0 && reflexion == 0) {
        this->DrawAmbientShadow(view, &position, shadow_scale, local_world, &world_local, biased_identity);
        if (!IsGameFlowInFrontEnd() && this->pRideInfo->Type != CARTYPE_COPHELI && !is_traffic_car) {
            this->DrawKeithProjShadow(view, &position, local_world, &world_local, biased_identity, car_body_lod);
        }
    }

    body_render_flags = 0;
    if (IsGameFlowInGame() && view->GetID() != 3) {
        body_render_flags = 0x8000;
    }

    if ((static_cast<unsigned int>(view->GetID() - EVIEW_PLAYER1) <= 1) && WorldTimeElapsed > 0.0f) {
        if (this->mDamageBehaviour != nullptr) {
            this->mDamageBehaviour->Update(biased_local_world);
        }
    } else {
        if (this->mDamageBehaviour != nullptr) {
            this->mDamageBehaviour->Pose(biased_local_world);
        }
    }

    if (this->pCarTypeInfo != nullptr && this->pCarTypeInfo->GetCarUsageType() == CAR_USAGE_TYPE_COP) {
        view->NumCopsInView++;
    }

    eLightMaterial *light_material_carskin = this->LightMaterial_CarSkin;
    eLightMaterial *light_material_spoiler = this->LightMaterial_Spoiler;
    if (light_material_spoiler == nullptr) {
        light_material_spoiler = light_material_carskin;
    }
    eLightMaterial *light_material_roof = this->LightMaterial_Roof;
    if (light_material_roof == nullptr) {
        light_material_roof = light_material_carskin;
    }
    eLightMaterial *light_material_tint = this->LightMaterial_WindowTint;

    for (int model_index = 0; model_index < CARSLOTID_MODEL_NUM; model_index++) {
        bMatrix4 *finalmat = static_cast<bMatrix4 *>(eFrameMalloc(sizeof(bMatrix4)));
        if (finalmat == nullptr) {
            continue;
        }

        *finalmat = *biased_local_world;

        int current_model_lod = car_body_lod;
        for (int model_number = 0; model_number < CARPART_MODEL_NUM; model_number++) {
            CarPartModel *model_lod_base = &this->mCarPartModels[model_index][model_number][current_model_lod];
            int model_part_id = model_index;
            eLightMaterial *model_light_material = light_material_carskin;

            // TODO get rid of gotos, there are probably switch statements here
            if (model_lod_base->GetModel() != nullptr && !model_lod_base->IsHidden()) {
                if (model_part_id == CARSLOTID_RIGHT_SIDE_MIRROR) {
                    goto render_side_mirror;
                }

                {
                    if (model_part_id < CARSLOTID_DRIVER) {
                        if (model_part_id == CARSLOTID_LEFT_SIDE_MIRROR) {
                            goto render_side_mirror;
                        } else if (model_part_id < CARSLOTID_REAR_BRAKE) {
                            if (model_part_id == CARSLOTID_FRONT_BRAKE) {
                                continue;
                            }
                        } else if (model_part_id == CARSLOTID_REAR_BRAKE) {
                            continue;
                        }
                    } else if (model_part_id != CARSLOTID_ROOF) {
                        if (model_part_id < CARSLOTID_HOOD) {
                            if (model_part_id == CARSLOTID_SPOILER) {
                                continue;
                            }
                        } else if (model_part_id <= CARSLOTID_LICENSE_PLATE) {
                            if (model_part_id >= CARSLOTID_FRONT_WHEEL) {
                                continue;
                            }
                        }
                    } else {
                        continue;
                    }

                    int draw_part = this->pRideInfo->IsPartEnabled(model_part_id);

                    if (model_part_id == CARSLOTID_DAMAGE_REAR_LEFT_WINDOW || model_part_id == CARSLOTID_DAMAGE_FRONT_LEFT_WINDOW ||
                        model_part_id == CARSLOTID_DAMAGE_FRONT_RIGHT_WINDOW || model_part_id == CARSLOTID_DAMAGE_REAR_RIGHT_WINDOW ||
                        model_part_id == CARSLOTID_FRONT_LEFT_WINDOW || model_part_id == CARSLOTID_FRONT_RIGHT_WINDOW ||
                        model_part_id == CARSLOTID_FRONT_WINDOW || model_part_id == CARSLOTID_REAR_LEFT_WINDOW ||
                        model_part_id == CARSLOTID_REAR_RIGHT_WINDOW || model_part_id == CARSLOTID_REAR_WINDOW) {
                        if (!AlphaWritesEnabled) {
                            goto done_model_part;
                        }
                    }

                    if (model_part_id == CARSLOTID_LEFT_SIDE_MIRROR) {
                        draw_part = 1;
                    }
                    if (model_part_id == CARSLOTID_RIGHT_SIDE_MIRROR) {
                        draw_part = 1;
                    }

                    if (model_part_id == 0x15 || model_part_id == 0x0e || model_part_id == 0x0d || model_part_id == 0x16) {
                        if (IsNISCopCar(this->pRideInfo->Type)) {
                            int door_index = 0;
                            if (model_part_id == 0x0d) {
                                door_index = 1;
                            } else if (model_part_id == 0x0e) {
                                door_index = 2;
                            } else if (model_part_id == 0x16) {
                                door_index = 3;
                            }

                            if (NISCopCarDoorOpenAmount[door_index] == 0.0f) {
                                eMulMatrix(finalmat, &NISCopCarDoorClosedMarkers[door_index], biased_local_world);
                            } else if (NISCopCarDoorOpenAmount[door_index] == 1.0f) {
                                eMulMatrix(finalmat, &NISCopCarDoorOpenMarkers[door_index], biased_local_world);
                            } else {
                                bQuaternion open_quaternion(NISCopCarDoorOpenMarkers[door_index]);
                                bQuaternion closed_quaternion(NISCopCarDoorClosedMarkers[door_index]);
                                bQuaternion blend_quaternion;
                                closed_quaternion.Slerp(blend_quaternion, open_quaternion, NISCopCarDoorOpenAmount[door_index]);

                                bVector4 blend_translation(NISCopCarDoorOpenMarkers[door_index].v3);
                                bScale(&blend_translation, &blend_translation, NISCopCarDoorOpenAmount[door_index]);
                                bScaleAdd(&blend_translation, &blend_translation, &NISCopCarDoorClosedMarkers[door_index].v3,
                                          1.0f - NISCopCarDoorOpenAmount[door_index]);

                                bMatrix4 blend_matrix;
                                blend_quaternion.GetMatrix(&blend_matrix);
                                blend_matrix.v3 = blend_translation;
                                eMulMatrix(finalmat, &blend_matrix, biased_local_world);
                            }
                        }
                    } else if (model_part_id == 0x3f && this->CarbonHood != 0) {
                        model_light_material = this->LightMaterial_Carbon;
                    }

                    if (model_part_id == 0x2b) {
                        draw_part &= 1;
                        if (IsGameFlowInFrontEnd()) {
                            draw_part = 0;
                        }
                        if (IsNISCopCar(this->pRideInfo->Type)) {
                            draw_part &= NISCopCarDriverVisible;
                        } else {
                            draw_part &= NISRaceDriverVisible;
                        }
                    } else {
                        draw_part &= 1;
                    }

                    if (draw_part != 0) {
                        eLightMaterial *light_material_body = model_light_material;
                        eModel *model = model_lod_base->GetModel();

                        model->ReplaceLightMaterial(STRINGHASH_CARSKIN, light_material_body);
                        model = model_lod_base->GetModel();
                        model->ReplaceLightMaterial(STRINGHASH_WINDSHIELD, light_material_tint);

                        if (this->mDamageBehaviour == nullptr || model_part_id == 0x2b) {
                            this->RenderPart(view, model_lod_base, finalmat, light_context,
                                             disable_env_flag | extra_render_flags | body_render_flags);
                        } else if (model_part_id <= CARSLOTID_BODY) {
                            bMatrix4 *damage_matrix = this->mDamageBehaviour->GetPartMatrix(model_part_id);
                            bCopy(finalmat, damage_matrix);
                            if (!this->mDamageBehaviour->IsPartHidden(model_part_id)) {
                                this->RenderPart(view, model_lod_base, finalmat, light_context,
                                                 disable_env_flag | extra_render_flags | body_render_flags);
                            }
                        }
                    }
                }
                goto done_model_part;

            render_side_mirror:
                if (reflexion == 0) {
                    eModel *model = model_lod_base->GetModel();
                    model->ReplaceLightMaterial(STRINGHASH_CARSKIN, this->LightMaterial_CarSkin);
                    view->Render(model, biased_local_world, light_context, disable_env_flag | extra_render_flags | body_render_flags, 0);
                }

            done_model_part:;
            }
        }
    }

    if (car_body_lod <= this->pRideInfo->GetMaxLicenseLodLevel() || reflexion == 0) {
        bMatrix4 license_rotate;
        eIdentity(&license_rotate);
        eRotateY(&license_rotate, &license_rotate, 0x4000);

        for (CarEmitterPosition *emitter_position = this->EmitterPositionList[27].GetHead();
             emitter_position != this->EmitterPositionList[27].EndOfList(); emitter_position = emitter_position->GetNext()) {
            ePositionMarker *position_marker = emitter_position->PositionMarker;
            bMatrix4 *license_local_world = eFrameMallocMatrix(1);
            if (license_local_world != nullptr) {
                eMulMatrix(license_local_world, &license_rotate, &position_marker->Matrix);
                eMulMatrix(license_local_world, license_local_world, biased_local_world);

                for (int i = 0; i < 1; i++) {
                    eModel *license_model = this->mCarPartModels[i + CARSLOTID_LICENSE_PLATE][0][this->mMinLodLevel].GetModel();
                    if (license_model != nullptr) {
                        view->Render(license_model, license_local_world, light_context, disable_env_flag | extra_render_flags | body_render_flags,
                                     nullptr);
                    }
                }
            }
        }
    }

    if (car_body_lod <= this->pRideInfo->GetMaxSpoilerLodLevel()) {
        bMatrix4 *spoiler_local_world = eFrameMallocMatrix(1);

        if (spoiler_local_world != nullptr) {
            CarPart *part_spoiler = this->pRideInfo->GetPart(CARSLOTID_SPOILER);
            if (part_spoiler != nullptr) {
                if (part_spoiler->GetAppliedAttributeIParam(bStringHash("USEMARKER2"), 0) != 0 && this->SpoilerPositionMarker2 != nullptr) {
                    this->SpoilerPositionMarker = this->SpoilerPositionMarker2;
                }
            }

            if (this->SpoilerPositionMarker != nullptr && part_spoiler != nullptr && part_spoiler->GetUpgradeLevel() != 0) {
                for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                    eModel *spoiler_model = this->mCarPartModels[CARSLOTID_SPOILER][i][car_body_lod].GetModel();
                    if (spoiler_model != nullptr) {
                        eMulMatrix(&spoiler_local_world[i], &this->SpoilerPositionMarker->Matrix, biased_local_world);
                        spoiler_model->ReplaceLightMaterial(STRINGHASH_CARSKIN, light_material_spoiler);
                        view->Render(spoiler_model, &spoiler_local_world[i], light_context, disable_env_flag | extra_render_flags, nullptr);
                    }
                }
            } else {
                for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                    eModel *spoiler_model = this->mCarPartModels[CARSLOTID_SPOILER][i][car_body_lod].GetModel();
                    if (spoiler_model != nullptr) {
                        spoiler_model->ReplaceLightMaterial(STRINGHASH_CARSKIN, light_material_spoiler);
                        view->Render(spoiler_model, biased_local_world, light_context, disable_env_flag | extra_render_flags, nullptr);
                    }
                }
            }
        }
    }

    if (car_body_lod <= this->pRideInfo->GetMaxRoofScoopLodLevel()) {
        bMatrix4 *roof_local_world = eFrameMallocMatrix(1);

        if (roof_local_world != nullptr) {
            if (this->RoofScoopPositionMarker != nullptr) {
                eMulMatrix(roof_local_world, &this->RoofScoopPositionMarker->Matrix, biased_local_world);
                for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                    eModel *roof_scoop_model = this->mCarPartModels[CARSLOTID_ROOF][i][car_body_lod].GetModel();
                    if (roof_scoop_model != nullptr) {
                        roof_scoop_model->ReplaceLightMaterial(STRINGHASH_CARSKIN, light_material_roof);
                        view->Render(roof_scoop_model, roof_local_world, light_context, (disable_env_flag | extra_render_flags) | body_render_flags,
                                     nullptr);
                    }
                }
            } else {
                for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                    eModel *roof_scoop_model = this->mCarPartModels[CARSLOTID_ROOF][i][car_body_lod].GetModel();
                    if (roof_scoop_model != nullptr) {
                        roof_scoop_model->ReplaceLightMaterial(STRINGHASH_CARSKIN, light_material_roof);
                        view->Render(roof_scoop_model, biased_local_world, light_context, (disable_env_flag | extra_render_flags) | body_render_flags,
                                     nullptr);
                    }
                }
            }
        }
    }

    bMatrix4 trans_pivot[4];
    bMatrix4 trans_axle[4];
    bMatrix4 brake_trans_pivot[4];

    float wheel_pivot_trans_amount = WheelPivotTranslationAmount;

    float front_wheel_brake_marker = TweakBrakeMarkerY[0] != 0.0f ? TweakBrakeMarkerY[0] : this->WheelBrakeMarkerY[0];
    float rear_wheel_brake_marker = TweakBrakeMarkerY[1] != 0.0f ? TweakBrakeMarkerY[1] : this->WheelBrakeMarkerY[1];

    unsigned short wheel_camber_angle_front = 0;
    float wheel_camber_push_down_front = 0.0f;
    unsigned short wheel_camber_angle_rear = 0;
    float wheel_camber_push_down_rear = 0.0f;

    if (car_body_lod <= CARPART_LOD_B) {
        float camber_amount_front = this->mAttributes.CamberFront();
        float camber_amount_rear = this->mAttributes.CamberRear();
        wheel_camber_angle_front = bDegToAng(camber_amount_front * 7.0f);
        wheel_camber_angle_rear = bDegToAng(camber_amount_rear * 7.0f);
        wheel_camber_push_down_front = camber_amount_front * 0.03f;
        wheel_camber_push_down_rear = camber_amount_rear * 0.03f;
    }

    unsigned short wheel_wobble_angle = bDegToAng(0.0f);

    for (int wheel = 0; wheel < 4; wheel++) {
        int wheel_end = static_cast<int>(wheel > 1);
        float wheel_width_scale = this->WheelWidthScales[wheel];
        float wheel_width = this->WheelWidths[wheel_end] * wheel_width_scale;
        float wheel_radius_scale = this->WheelRadiusScales[wheel];
        float yrender_offset = this->WheelYRenderOffset[wheel];
        float pivot_y = wheel_width * wheel_pivot_trans_amount;
        float wheel_brake_marker_y = rear_wheel_brake_marker;

        if (wheel <= 1) {
            wheel_brake_marker_y = front_wheel_brake_marker;
        }

        eIdentity(&trans_pivot[wheel]);
        eIdentity(&trans_axle[wheel]);
        eIdentity(&brake_trans_pivot[wheel]);

        trans_pivot[wheel].v0.x = wheel_radius_scale;
        trans_pivot[wheel].v1.y = wheel_width_scale;
        trans_pivot[wheel].v2.z = wheel_radius_scale;
        trans_pivot[wheel].v3.y = -pivot_y;

        if (yrender_offset < 0.0f) {
            trans_axle[wheel].v3.y = -(yrender_offset + pivot_y);
        } else {
            trans_axle[wheel].v3.y = pivot_y - yrender_offset;
        }

        if (wheel_brake_marker_y != 0.0f) {
            float marker_y = -pivot_y;
            brake_trans_pivot[wheel].v3.y = wheel_brake_marker_y * wheel_width_scale + marker_y;
        }

        if (wheel <= 1) {
            trans_axle[wheel].v3.z -= wheel_camber_push_down_front;
        } else {
            trans_axle[wheel].v3.z -= wheel_camber_push_down_rear;
        }

        tire_matrices[wheel].v3.y = 0.0f;
        brake_matrices[wheel].v3.y = 0.0f;
    }

    int tire_visible0 = 0;
    int tire_visible1 = 0;
    int tire_visible2 = 0;
    int tire_visible3 = 0;
    eDynamicLightContext *tire_light_context = nullptr;

    int tires_enabled = this->pRideInfo->IsPartEnabled(CARSLOTID_FRONT_WHEEL);
    bMatrix4 *tirelight_world_view = &hack_man_matrix;

    if (this->pRideInfo->Type != CARTYPE_COPHELI && tires_enabled != 0 && car_body_lod <= this->pRideInfo->GetMaxTireLodLevel()) {
        bMatrix4 *tire_local_world = eFrameMallocMatrix(4);
        float extra_rear_tire_offset = this->mAttributes.ExtraRearTireOffset();
        bMatrix4 *extra_tire_local_world = nullptr;

        if (extra_rear_tire_offset != 0.0f) {
            extra_tire_local_world = eFrameMallocMatrix(2);
        }

        tire_light_context = static_cast<eDynamicLightContext *>(eFrameMalloc(0x490));

        if (tire_local_world != nullptr && tire_light_context != nullptr) {
            int tire_lod = car_tire_lod;
            CarPartModel *front_tire_models[CARPART_MODEL_NUM];
            CarPartModel *rear_tire_models[CARPART_MODEL_NUM];
            eLightMaterial *light_material_rim[2];
            bMatrix4 *left_tire_flip;
            unsigned int extra_mirror_flag;

            tire_visible0 = static_cast<int>(this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_TIRE_FL));
            tire_visible1 = static_cast<int>(this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_TIRE_FR));
            tire_visible2 = static_cast<int>(this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_TIRE_RR));
            tire_visible3 = static_cast<int>(this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_TIRE_RL));

            if (car_tire_lod > this->pRideInfo->GetMaxTireLodLevel()) {
                tire_lod = this->pRideInfo->GetMaxTireLodLevel();
            }

            light_material_rim[0] = this->LightMaterial_WheelRim;
            light_material_rim[1] = this->LightMaterial_Spinner;

            for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                front_tire_models[i] = &this->mCarPartModels[CARSLOTID_FRONT_WHEEL + i][0][tire_lod];
                rear_tire_models[i] = &this->mCarPartModels[CARSLOTID_REAR_WHEEL + i][0][tire_lod];

                if (front_tire_models[i] != nullptr && light_material_rim[i] != nullptr) {
                    front_tire_models[i]->GetModel()->ReplaceLightMaterial(STRINGHASH_MAGSILVER, light_material_rim[i]);
                    front_tire_models[i]->GetModel()->ReplaceLightMaterial(STRINGHASH_MAGCHROME, light_material_rim[i]);
                    front_tire_models[i]->GetModel()->ReplaceLightMaterial(STRINGHASH_MAGGUNMETAL, light_material_rim[i]);
                }
                if (rear_tire_models[i] != nullptr && light_material_rim[i] != nullptr) {
                    front_tire_models[i]->GetModel()->ReplaceLightMaterial(STRINGHASH_MAGSILVER, light_material_rim[i]);
                    front_tire_models[i]->GetModel()->ReplaceLightMaterial(STRINGHASH_MAGCHROME, light_material_rim[i]);
                    front_tire_models[i]->GetModel()->ReplaceLightMaterial(STRINGHASH_MAGGUNMETAL, light_material_rim[i]);
                }
            }

            left_tire_flip = &LeftTireRotateZMatrix;
            extra_mirror_flag = 0;
            if (MirrorLeftTires || this->mMirrorLeftWheels) {
                left_tire_flip = &LeftTireMirrorMatrix;
                extra_mirror_flag = 0x40000;
            }

            bMatrix4 wobbleMat;
            if (this->mWheelWobbleEnabled[0] || this->mWheelWobbleEnabled[1] || this->mWheelWobbleEnabled[2] || this->mWheelWobbleEnabled[3]) {
                bIdentity(&wobbleMat);
                eRotateX(&wobbleMat, &wobbleMat, 0x4fa);
            }

            if (tire_visible0) {
                bMatrix4 *starting_tire_matrix = &tire_matrices[0];
                bMatrix4 tire_matrix_for_camber;

                if (wheel_camber_angle_front != 0) {
                    bVector3 wheel_offset;
                    bCopy(&tire_matrix_for_camber, starting_tire_matrix);
                    bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&tire_matrix_for_camber.v3));
                    bFill(&tire_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                    eRotateX(&tire_matrix_for_camber, &tire_matrix_for_camber, wheel_camber_angle_front);
                    bCopy(reinterpret_cast<bVector3 *>(&tire_matrix_for_camber.v3), &wheel_offset);
                    starting_tire_matrix = &tire_matrix_for_camber;
                }

                bMatrix4 tire0;
                eMulMatrix(&tire0, &trans_pivot[0], left_tire_flip);
                if (this->mWheelWobbleEnabled[0]) {
                    eMulMatrix(&tire0, &tire0, &wobbleMat);
                }
                eMulMatrix(&tire0, &tire0, starting_tire_matrix);
                eMulMatrix(&tire0, &tire0, &trans_axle[0]);
                eMulMatrix(&tire_local_world[0], &tire0, biased_local_world);
                elCloneLightContext(&tire_light_context[0], &tire_local_world[0], tirelight_world_view, &camera_world_position, view,
                                    &base_light_context);

                this->RenderPart(view, front_tire_models[0], &tire_local_world[0], &tire_light_context[0],
                                 disable_env_flag_tires | extra_render_flags | body_render_flags | extra_mirror_flag);
            }

            if (tire_visible1) {
                bMatrix4 *starting_tire_matrix = &tire_matrices[1];
                bMatrix4 tire_matrix_for_camber;

                if (wheel_camber_angle_front != 0) {
                    bVector3 wheel_offset;
                    bCopy(&tire_matrix_for_camber, starting_tire_matrix);
                    bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&tire_matrix_for_camber.v3));
                    bFill(&tire_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                    eRotateX(&tire_matrix_for_camber, &tire_matrix_for_camber, -wheel_camber_angle_front);
                    bCopy(reinterpret_cast<bVector3 *>(&tire_matrix_for_camber.v3), &wheel_offset);
                    starting_tire_matrix = &tire_matrix_for_camber;
                }

                bMatrix4 tire1;
                if (this->mWheelWobbleEnabled[1]) {
                    eMulMatrix(&tire1, &trans_pivot[1], &wobbleMat);
                } else {
                    tire1 = trans_pivot[1];
                }
                eMulMatrix(&tire1, &tire1, starting_tire_matrix);
                eMulMatrix(&tire1, &tire1, &trans_axle[1]);
                eMulMatrix(&tire_local_world[1], &tire1, biased_local_world);
                elCloneLightContext(&tire_light_context[1], &tire_local_world[1], tirelight_world_view, &camera_world_position, view,
                                    &base_light_context);
                this->RenderPart(view, front_tire_models[0], &tire_local_world[1], &tire_light_context[1],
                                 disable_env_flag_tires | extra_render_flags | body_render_flags);
            }

            if (tire_visible2) {
                bMatrix4 *starting_tire_matrix = &tire_matrices[2];
                bMatrix4 tire_matrix_for_camber;

                if (wheel_camber_angle_rear != 0) {
                    bVector3 wheel_offset;
                    bCopy(&tire_matrix_for_camber, starting_tire_matrix);
                    bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&tire_matrix_for_camber.v3));
                    bFill(&tire_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                    eRotateX(&tire_matrix_for_camber, &tire_matrix_for_camber, -wheel_camber_angle_rear);
                    bCopy(reinterpret_cast<bVector3 *>(&tire_matrix_for_camber.v3), &wheel_offset);
                    starting_tire_matrix = &tire_matrix_for_camber;
                }

                bMatrix4 tire2;
                if (this->mWheelWobbleEnabled[2]) {
                    eMulMatrix(&tire2, &trans_pivot[2], &wobbleMat);
                } else {
                    tire2 = trans_pivot[2];
                }
                eMulMatrix(&tire2, &tire2, starting_tire_matrix);
                eMulMatrix(&tire2, &tire2, &trans_axle[2]);
                eMulMatrix(&tire_local_world[2], &tire2, biased_local_world);
                elCloneLightContext(&tire_light_context[2], &tire_local_world[2], tirelight_world_view, &camera_world_position, view,
                                    &base_light_context);

                if (!in_front_end && !INIS::Exists() && tire_lod == this->mMinLodLevel && TireFace(&tire_local_world[2], view) < FaceCos &&
                    this->mMinLodLevel + CARPART_LOD_B <= this->mMaxLodLevel) {
                    CarPartModel *tmpM = &this->mCarPartModels[CARSLOTID_REAR_WHEEL][0][this->mMinLodLevel + CARPART_LOD_B];
                    if (tmpM != nullptr) {
                        rear_tire_models[0] = tmpM;
                    }
                }

                this->RenderPart(view, rear_tire_models[0], &tire_local_world[2], &tire_light_context[2],
                                 disable_env_flag_tires | extra_render_flags | body_render_flags);
                if (extra_tire_local_world != nullptr) {
                    tire2.v3.x += extra_rear_tire_offset;
                    eMulMatrix(&extra_tire_local_world[0], &tire2, biased_local_world);
                    this->RenderPart(view, rear_tire_models[0], &extra_tire_local_world[0], &tire_light_context[2],
                                     disable_env_flag_tires | extra_render_flags | body_render_flags);
                }
            }

            for (int i = 0; i < 1; i++) {
                rear_tire_models[i] = &this->mCarPartModels[CARSLOTID_REAR_WHEEL + i][0][tire_lod];
            }

            if (tire_visible3) {
                bMatrix4 *starting_tire_matrix = &tire_matrices[3];
                bMatrix4 tire_matrix_for_camber;

                if (wheel_camber_angle_rear != 0) {
                    bVector3 wheel_offset;
                    bCopy(&tire_matrix_for_camber, starting_tire_matrix);
                    bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&tire_matrix_for_camber.v3));
                    bFill(&tire_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                    eRotateX(&tire_matrix_for_camber, &tire_matrix_for_camber, wheel_camber_angle_rear);
                    bCopy(reinterpret_cast<bVector3 *>(&tire_matrix_for_camber.v3), &wheel_offset);
                    starting_tire_matrix = &tire_matrix_for_camber;
                }

                bMatrix4 tire3;
                eMulMatrix(&tire3, &trans_pivot[3], left_tire_flip);
                if (this->mWheelWobbleEnabled[3]) {
                    eMulMatrix(&tire3, &tire3, &wobbleMat);
                }
                eMulMatrix(&tire3, &tire3, starting_tire_matrix);
                eMulMatrix(&tire3, &tire3, &trans_axle[3]);
                eMulMatrix(&tire_local_world[3], &tire3, biased_local_world);
                elCloneLightContext(&tire_light_context[3], &tire_local_world[3], tirelight_world_view, &camera_world_position, view,
                                    &base_light_context);

                if (!in_front_end && !INIS::Exists() && tire_lod == this->mMinLodLevel && TireFace(&tire_local_world[3], view) < FaceCos &&
                    this->mMinLodLevel + CARPART_LOD_B <= this->mMaxLodLevel) {
                    CarPartModel *tmpM = &this->mCarPartModels[CARSLOTID_REAR_WHEEL][0][this->mMinLodLevel + CARPART_LOD_B];
                    if (tmpM != nullptr) {
                        rear_tire_models[0] = tmpM;
                    }
                }

                this->RenderPart(view, rear_tire_models[0], &tire_local_world[3], &tire_light_context[3],
                                 disable_env_flag_tires | extra_render_flags | body_render_flags | extra_mirror_flag);
                if (extra_tire_local_world != nullptr) {
                    tire3.v3.x += extra_rear_tire_offset;
                    eMulMatrix(&extra_tire_local_world[1], &tire3, biased_local_world);
                    this->RenderPart(view, rear_tire_models[0], &extra_tire_local_world[1], &tire_light_context[3],
                                     disable_env_flag_tires | extra_render_flags | body_render_flags | extra_mirror_flag);
                }
            }
        }
    }

    if (car_body_lod <= this->pRideInfo->GetMaxBrakeLodLevel() &&
        this->mCarPartModels[CARSLOTID_FRONT_BRAKE][0][this->mMinLodLevel].GetModel() != nullptr) {
        int brakes_visible_front_left = static_cast<int>(this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_BRAKE_FL));
        int brakes_visible_front_right = static_cast<int>(this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_BRAKE_FR));
        int brakes_visible_rear_right = static_cast<int>(this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_BRAKE_RR));
        int brakes_visible_rear_left = static_cast<int>(this->TheCarPartCuller.IsPartVisible(CULLABLE_CAR_PART_BRAKE_RL));

        if ((brakes_visible_front_left | brakes_visible_front_right | brakes_visible_rear_right | brakes_visible_rear_left) != 0) {
            bMatrix4 *brake_local_world = eFrameMallocMatrix(4);
            eDynamicLightContext *brake_light_context = static_cast<eDynamicLightContext *>(eFrameMalloc(sizeof(eDynamicLightContext) * 4));

            if (brake_local_world != nullptr && brake_light_context != nullptr) {
                int brakes_lod = car_body_lod;
                eModel *front_brake_models[CARPART_MODEL_NUM];
                eModel *rear_brake_models[CARPART_MODEL_NUM];
                eLightMaterial *light_material_caliper = this->LightMaterial_Caliper;
                bMatrix4 mirror;

                for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                    if (this->mCarPartModels[CARSLOTID_FRONT_BRAKE + i][0][brakes_lod].GetModel() != nullptr) {
                        front_brake_models[i] = this->mCarPartModels[CARSLOTID_FRONT_BRAKE + i][0][brakes_lod].GetModel();
                        if (light_material_caliper != nullptr) {
                            front_brake_models[i]->ReplaceLightMaterial(STRINGHASH_CALIPER, light_material_caliper);
                            front_brake_models[i]->ReplaceLightMaterial(STRINGHASH_CALLIPER, light_material_caliper);
                        }
                    } else {
                        front_brake_models[i] = nullptr;
                    }

                    if (this->mCarPartModels[CARSLOTID_REAR_BRAKE + i][0][brakes_lod].GetModel() != nullptr) {
                        rear_brake_models[i] = this->mCarPartModels[CARSLOTID_REAR_BRAKE + i][0][brakes_lod].GetModel();
                        if (light_material_caliper != nullptr) {
                            rear_brake_models[i]->ReplaceLightMaterial(STRINGHASH_CALIPER, light_material_caliper);
                            rear_brake_models[i]->ReplaceLightMaterial(STRINGHASH_CALLIPER, light_material_caliper);
                        }
                    } else {
                        rear_brake_models[i] = nullptr;
                    }
                }

                eIdentity(&mirror);
                mirror.v0.x = BrakeMirrorX;
                mirror.v1.y = BrakeMirrorY;
                mirror.v2.z = BrakeMirrorZ;

                if (brakes_visible_front_left) {
                    bMatrix4 *starting_brake_matrix = &brake_matrices[0];
                    bMatrix4 brake_matrix_for_camber;

                    if (wheel_camber_angle_front != 0) {
                        bVector3 wheel_offset;
                        bCopy(&brake_matrix_for_camber, starting_brake_matrix);
                        bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&brake_matrix_for_camber.v3));
                        bFill(&brake_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                        eRotateX(&brake_matrix_for_camber, &brake_matrix_for_camber, wheel_camber_angle_front);
                        bCopy(reinterpret_cast<bVector3 *>(&brake_matrix_for_camber.v3), &wheel_offset);
                        starting_brake_matrix = &brake_matrix_for_camber;
                    }

                    bMatrix4 bm0;
                    eMulMatrix(&bm0, &brake_trans_pivot[0], &mirror);
                    eMulMatrix(&bm0, &bm0, starting_brake_matrix);
                    eMulMatrix(&bm0, &bm0, &trans_axle[0]);
                    eMulMatrix(&brake_local_world[0], &bm0, biased_local_world);

                    if (tire_visible0 && IsGameFlowInGame()) {
                        for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                            if (front_brake_models[i] != nullptr) {
                                front_brake_models[i]->AttachReplacementTextureTable(this->BrakeLeftReplacementTextureTable, 2, 0);
                                view->Render(front_brake_models[i], &brake_local_world[0], &tire_light_context[0],
                                             extra_render_flags | body_render_flags | 0x40000, nullptr);
                            }
                        }
                    } else {
                        for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                            if (front_brake_models[i] != nullptr) {
                                front_brake_models[i]->AttachReplacementTextureTable(this->BrakeLeftReplacementTextureTable, 2, 0);
                                elCloneLightContext(&brake_light_context[0], &brake_local_world[0], tirelight_world_view, &camera_world_position,
                                                    view, &base_light_context);
                                view->Render(front_brake_models[i], &brake_local_world[0], &brake_light_context[0],
                                             extra_render_flags | body_render_flags | 0x40000, nullptr);
                            }
                        }
                    }
                }

                if (brakes_visible_front_right) {
                    bMatrix4 *starting_brake_matrix = &brake_matrices[1];
                    bMatrix4 brake_matrix_for_camber;

                    if (wheel_camber_angle_front != 0) {
                        bVector3 wheel_offset;
                        bCopy(&brake_matrix_for_camber, starting_brake_matrix);
                        bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&brake_matrix_for_camber.v3));
                        bFill(&brake_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                        eRotateX(&brake_matrix_for_camber, &brake_matrix_for_camber, -wheel_camber_angle_front);
                        bCopy(reinterpret_cast<bVector3 *>(&brake_matrix_for_camber.v3), &wheel_offset);
                        starting_brake_matrix = &brake_matrix_for_camber;
                    }

                    bMatrix4 bm1;
                    eMulMatrix(&bm1, &brake_trans_pivot[1], starting_brake_matrix);
                    eMulMatrix(&bm1, &bm1, &trans_axle[1]);
                    eMulMatrix(&brake_local_world[1], &bm1, biased_local_world);

                    if (tire_visible1 && IsGameFlowInGame()) {
                        for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                            if (front_brake_models[i] != nullptr) {
                                front_brake_models[i]->AttachReplacementTextureTable(this->BrakeRightReplacementTextureTable, 2, 0);
                                view->Render(front_brake_models[i], &brake_local_world[1], &tire_light_context[1],
                                             extra_render_flags | body_render_flags, nullptr);
                            }
                        }
                    } else {
                        for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                            if (front_brake_models[i] != nullptr) {
                                front_brake_models[i]->AttachReplacementTextureTable(this->BrakeRightReplacementTextureTable, 2, 0);
                                elCloneLightContext(&brake_light_context[1], &brake_local_world[1], tirelight_world_view, &camera_world_position,
                                                    view, &base_light_context);
                                view->Render(front_brake_models[i], &brake_local_world[1], &brake_light_context[1],
                                             extra_render_flags | body_render_flags, nullptr);
                            }
                        }
                    }
                }

                if (brakes_visible_rear_right) {
                    bMatrix4 *starting_brake_matrix = &brake_matrices[2];
                    bMatrix4 brake_matrix_for_camber;

                    if (wheel_camber_angle_rear != 0) {
                        bCopy(&brake_matrix_for_camber, starting_brake_matrix);
                        bVector3 wheel_offset;
                        bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&brake_matrix_for_camber.v3));
                        bFill(&brake_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                        eRotateX(&brake_matrix_for_camber, &brake_matrix_for_camber, -wheel_camber_angle_rear);
                        bCopy(reinterpret_cast<bVector3 *>(&brake_matrix_for_camber.v3), &wheel_offset);
                        starting_brake_matrix = &brake_matrix_for_camber;
                    }

                    bMatrix4 bm2;
                    eMulMatrix(&bm2, &brake_trans_pivot[2], starting_brake_matrix);
                    eMulMatrix(&bm2, &bm2, &trans_axle[2]);
                    eMulMatrix(&brake_local_world[2], &bm2, biased_local_world);

                    if (tire_visible2 && IsGameFlowInGame()) {
                        for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                            if (rear_brake_models[i] != nullptr) {
                                rear_brake_models[i]->AttachReplacementTextureTable(this->BrakeRightReplacementTextureTable, 2, 0);
                                view->Render(rear_brake_models[i], &brake_local_world[2], &tire_light_context[2],
                                             extra_render_flags | body_render_flags, nullptr);
                            }
                        }
                    } else {
                        for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                            if (rear_brake_models[i] != nullptr) {
                                rear_brake_models[i]->AttachReplacementTextureTable(this->BrakeRightReplacementTextureTable, 2, 0);
                                elCloneLightContext(&brake_light_context[2], &brake_local_world[2], tirelight_world_view, &camera_world_position,
                                                    view, &base_light_context);
                                view->Render(rear_brake_models[i], &brake_local_world[2], &brake_light_context[2],
                                             extra_render_flags | body_render_flags, nullptr);
                            }
                        }
                    }
                }

                if (brakes_visible_rear_left) {
                    bMatrix4 *starting_brake_matrix = &brake_matrices[3];
                    bMatrix4 brake_matrix_for_camber;

                    if (wheel_camber_angle_rear != 0) {
                        bVector3 wheel_offset;
                        bCopy(&brake_matrix_for_camber, starting_brake_matrix);
                        bCopy(&wheel_offset, reinterpret_cast<bVector3 *>(&brake_matrix_for_camber.v3));
                        bFill(&brake_matrix_for_camber.v3, 0.0f, 0.0f, 0.0f, 1.0f);
                        eRotateX(&brake_matrix_for_camber, &brake_matrix_for_camber, wheel_camber_angle_rear);
                        bCopy(reinterpret_cast<bVector3 *>(&brake_matrix_for_camber.v3), &wheel_offset);
                        starting_brake_matrix = &brake_matrix_for_camber;
                    }

                    bMatrix4 bm3;
                    eMulMatrix(&bm3, &brake_trans_pivot[3], &mirror);
                    eMulMatrix(&bm3, &bm3, starting_brake_matrix);
                    eMulMatrix(&bm3, &bm3, &trans_axle[3]);
                    eMulMatrix(&brake_local_world[3], &bm3, biased_local_world);

                    if (tire_visible3 && IsGameFlowInGame()) {
                        for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                            if (rear_brake_models[i] != nullptr) {
                                rear_brake_models[i]->AttachReplacementTextureTable(this->BrakeLeftReplacementTextureTable, 2, 0);
                                view->Render(rear_brake_models[i], &brake_local_world[3], &tire_light_context[3],
                                             extra_render_flags | body_render_flags | 0x40000, nullptr);
                            }
                        }
                    } else {
                        for (int i = 0; i < CARPART_MODEL_NUM; i++) {
                            if (rear_brake_models[i] != nullptr) {
                                rear_brake_models[i]->AttachReplacementTextureTable(this->BrakeLeftReplacementTextureTable, 2, 0);
                                elCloneLightContext(&brake_light_context[3], &brake_local_world[3], tirelight_world_view, &camera_world_position,
                                                    view, &base_light_context);
                                view->Render(rear_brake_models[i], &brake_local_world[3], &brake_light_context[3],
                                             extra_render_flags | body_render_flags | 0x40000, nullptr);
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

int cmpl(const void *a, const void *b) {
    float v = (*reinterpret_cast<const float *const *>(a))[0] - (*reinterpret_cast<const float *const *>(b))[0];

    if (0.0f < v) {
        return 1;
    }
    if (v < 0.0f) {
        return -1;
    }

    v = (*reinterpret_cast<const float *const *>(b))[1] - (*reinterpret_cast<const float *const *>(a))[1];

    if (0.0f < v) {
        return 1;
    }
    if (v < 0.0f) {
        return -1;
    }

    return 0;
}

int cmph(const void *a, const void *b) {
    return cmpl(b, a);
}

static inline bool ccw(float **P, int i, int j, int k) {
    float a = P[i][0] - P[j][0];
    float b = P[i][1] - P[j][1];
    float c = P[k][0] - P[j][0];
    float d = P[k][1] - P[j][1];
    return a * d - b * c > 0.0f;
}

// UNSOLVED
int make_chain(float **V, int n, int (*cmp)(const void *, const void *)) {
    int i;
    int j;
    int s;
    float *t;

    qsort(V, n, 4, cmp);

    s = 1;
    for (i = 2; i < n; i++) {
        for (j = s; j >= 1 && !ccw(V, j - 1, j, i); j--) {
        }

        s = j + 1;
        t = V[s];
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

inline bool ZPassesTest(float z, float zref) {
    return true;
}

static bVector3 hull_Origin;
static bVector3 hull_Normal;

inline bool DotPassesTest(const bVector3 *point) {
    bVector3 vec = *point - hull_Origin;
    float dot = bDot(&vec, &hull_Normal);
    if (dot < 0.0f) {
        dot = -dot;
    }

    return dot < 20.0f;
}

float PointCloud[16][4] = {
    {-0.5f, 0.5f, 1.0f, 1.0f}, {0.5f, 0.5f, 1.0f, 1.0f}, {0.5f, -0.5f, 1.0f, 1.0f}, {-0.5f, -0.5f, 1.0f, 1.0f},
    {-0.5f, 0.5f, 0.5f, 1.0f}, {0.5f, 0.5f, 0.5f, 1.0f}, {0.5f, -0.5f, 0.5f, 1.0f}, {-0.5f, -0.5f, 0.5f, 1.0f},
    {-1.0f, 1.0f, 0.5f, 1.0f}, {1.0f, 1.0f, 0.5f, 1.0f}, {1.0f, -1.0f, 0.5f, 1.0f}, {-1.0f, -1.0f, 0.5f, 1.0f},
    {-1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, -1.0f, 0.0f, 1.0f}, {-1.0f, -1.0f, 0.0f, 1.0f},
};

// TODO figure out and use these values
// static const unsigned short CarShadowSunAngleMin;

// static const float CarShadowSunAdjust;

// static const float FancyCarShadowFanUV;

// static const float FancyCarShadowEdgeMultMin;

// static const float FancyCarShadowEdgeMultMax;

float FancyCarShadowEdgeMult = 1.1f;

// static const float FancyCarShadowZAdd;

// static const float CarShadowScaleHULLx;

// static const float CarShadowScaleHULLy;

// static const float CarShadowScaleHULLz;

// static const float CarHullShadowZeroSunlightAmount;

// static const float CarBaseShadowZeroSunlightAmount;

// static const int CarHullShadowAlphaMinIG;

// static const int CarHullShadowAlphaMaxIG;

float car_elevation = 0.0f;

float car_elevation_scale = 0.0f;

bVector3 hullVertArray1[16];

bVector3 hullVertArray2[16];

bVector3 hullVertArray3[48];

static float *P[17];

void CarRenderInfo::convex_hull(bVector3 *p, const WCollider *wcoll, int &n, float Z, float zBias, int fast) {
    int i;
    int dec;
    bool bPointValid;
    UMath::Vector3 usPoint;

    for (i = 0; i < n; i++) {
        P[i] = reinterpret_cast<float *>(&p[i]);
    }

    n = ch2d(reinterpret_cast<float **>(P), n);
    if (wcoll != nullptr) {
        this->mWorldPos.SetTolerance(TweakCarShadowTopologyTolerance);
        if (fast != 0) {
            bVector3 *vec = hullVertArray2;
            bFill(vec, P[0][0], P[0][1], Z);

            eUnSwizzleWorldVector(*vec, reinterpret_cast<bVector3 &>(usPoint));
            float fastZ = 0.0f;
            this->mWorldPos.FindClosestFace(wcoll, usPoint, true);
            if (this->mWorldPos.OnValidFace()) {
                fastZ = this->mWorldPos.HeightAtPoint(usPoint);
            }

            dec = 0;
            for (i = 0; i < n; i++) {
                bFill(vec, P[i][0], P[i][1], fastZ);

                if (ZPassesTest(vec->z, fastZ) && DotPassesTest(vec)) {
                    vec++;
                } else {
                    dec++;
                }
            }
        } else {
            bool quitIfSameFace = true;
            bVector3 *vec;

            vec = hullVertArray2;
            dec = 0;

            for (i = 0; i < n; i++) {
                bFill(vec, P[i][0], P[i][1], Z);

                eUnSwizzleWorldVector(*vec, reinterpret_cast<bVector3 &>(usPoint));
                this->mWorldPos.FindClosestFace(wcoll, usPoint, quitIfSameFace);
                if (this->mWorldPos.OnValidFace()) {
                    vec->z = this->mWorldPos.HeightAtPoint(usPoint);
                }

                if (ZPassesTest(vec->z, Z) && DotPassesTest(vec)) {
                    quitIfSameFace = true;
                    vec++;
                } else {
                    quitIfSameFace = false;
                    dec++;
                }
            }
        }

        n -= dec;
    }
}

int smooth_shadow_corners(int nVerts) {
    bVector3 v[2];
    int i;
    int iNew;
    int nCurr;
    int nPrev;
    int nNext;

    iNew = 0;
    v[1] = hullVertArray2[0] - hullVertArray2[nVerts - 1];
    v[1] *= 0.333f;

    for (i = 0; i < nVerts; i++) {
        nCurr = i & 1;
        nPrev = nCurr ^ 1;
        nNext = i + 1;

        if (nNext == nVerts) {
            nNext = 0;
        }

        v[nCurr] = hullVertArray2[nNext] - hullVertArray2[i];
        v[nCurr] *= 0.333f;

        hullVertArray3[iNew] = hullVertArray2[i] - v[nPrev];
        hullVertArray3[iNew + 2] = hullVertArray2[i] + v[nCurr];

        {
            bVector3 vTemp = v[nCurr] - v[nPrev];
            bScaleAdd(&hullVertArray3[iNew + 1], &hullVertArray2[i], &vTemp, 0.333f);
        }

        iNew += 3;
    }

    return iNew;
}

bVector3 cs_lightV;

float cs_OneOverZ;

void sh_Setup(bVector3 *car_pos) {
    bVector3 light_pos;

    if (SunInfo == nullptr) {
        light_pos.x = TweakCarShadowSunX;
        light_pos.y = TweakCarShadowSunY;
        light_pos.z = TweakCarShadowSunZ;
    } else {
        light_pos.x = SunInfo->CarShadowPositionX;
        light_pos.y = SunInfo->CarShadowPositionY;
        light_pos.z = SunInfo->CarShadowPositionZ;
    }

    cs_lightV = *car_pos - light_pos;

    float fBase = cs_lightV.x * cs_lightV.x + cs_lightV.y * cs_lightV.y;
    fBase = bSqrt(fBase);

    unsigned short aSunAngle = bATan(fBase, -cs_lightV.z);
    if (aSunAngle < 4000) {
        float fSin;
        float fCos;

        bSinCos(&fSin, &fCos, 4000);

        float fLength = bLength(&cs_lightV);
        float fTotal;
        float fX = cs_lightV.x;
        float fY = cs_lightV.y;
        fTotal = bAbs(fX) + bAbs(fY);

        cs_lightV.x = (fX / fTotal) * fLength * fCos;
        cs_lightV.z = -fLength * fSin;
        cs_lightV.y = (fY / fTotal) * fLength * fCos;
    }

    cs_OneOverZ = (cs_lightV.z != 0.0f) ? (1.0f / cs_lightV.z) : 0.0f;
}

inline bVector3 sh_Adjust(const bVector3 &p, const bVector3 &mid) {
    bVector3 v;

    v.x = FancyCarShadowEdgeMult * (p.x - mid.x) + mid.x;
    v.y = FancyCarShadowEdgeMult * (p.y - mid.y) + mid.y;
    v.z = p.z;
    return v;
}

int dshad = 1;

// UNSOLVED r29, r24
void CarRenderInfo::DrawKeithProjShadow(eView *view, const bVector3 *position, bMatrix4 *localWorld, bMatrix4 *worldLocal, bMatrix4 *biasedIdentity,
                                        int body_lod) {
    if (body_lod >= 3) {
        return;
    }

    int i;
    int nVert = 16;
    bVector3 mid;
    bool bColour2;
    bVector3 *p = hullVertArray1;

    sh_Setup(const_cast<bVector3 *>(position));
    UMath::Vector3 usPoint;
    float zplane = position->z;

    if (IsGameFlowInGame()) {
        eUnSwizzleWorldVector(*position, reinterpret_cast<bVector3 &>(usPoint));
        this->mWorldPos.FindClosestFace(this->mWCollider, usPoint, false);
        if (this->mWorldPos.OnValidFace()) {
            zplane = this->mWorldPos.HeightAtPoint(usPoint);
        }
    }

    {
        bVector3 *pVec = p;
        bVector3 lightV(cs_lightV);
        float oneOverZ = cs_OneOverZ;
        bVector3 scale(1.75f, 0.75f, 1.0f);

        for (i = 0; i < nVert; i++) {
            bVector3 vLocal(PointCloud[i][0] * scale.x, PointCloud[i][1] * scale.y, PointCloud[i][2] * scale.z);
            bVector3 vWorld;

            eMulVector(&vWorld, localWorld, &vLocal);
            float w = (zplane - vWorld.z) * oneOverZ;
            pVec->x = w * cs_lightV.x + vWorld.x;
            pVec->y = vWorld.y;
            pVec->y += w * cs_lightV.y;
            pVec++;
        }
    }

    int fastfancy = static_cast<int>(body_lod != this->mMinLodLevel);
    this->convex_hull(p, this->mWCollider, nVert, zplane, HullShadowElevation, fastfancy);

    if (body_lod == this->mMinLodLevel) {
        nVert = smooth_shadow_corners(nVert);
        p = hullVertArray3;
    } else {
        p = hullVertArray2;
    }

    if (nVert > 2) {
        bVector3 v = p[0] + p[nVert / 2];

        mid = v;
        mid *= 0.5f;

        float alpha_min;
        float alpha_max = 27.0f;
        float shadow_alpha = (1.0f - car_elevation_scale) * alpha_max;
        FancyCarShadowEdgeMult = car_elevation_scale * -0.29999995f + 1.4f;
        i = static_cast<int>(shadow_alpha);
        unsigned int colour = static_cast<unsigned int>(bClamp(i, 0, 0xFE) << 24) | 0x00808080;

        if (dshad != 0) {
            int nMax = (nVert & ~1) - 1; // TODO doesn't exist

            for (i = 0; i < nMax; i += 2) {
                if (eBeginStrip(this->ShadowRampTexture, 4, biasedIdentity)) {
                    eAddVertex(p[i]);
                    eAddVertex(mid);
                    eAddVertex(p[i + 1]);
                    eAddVertex(p[(i + 2) % nVert]);
                    eAddColour(colour);
                    eAddColour(colour);
                    eAddColour(colour);
                    eAddColour(colour);
                    eAddUV(0.155f, 0.0f);
                    eAddUV(0.0f, 0.0f);
                    eAddUV(0.155f, 0.0f);
                    eAddUV(0.155f, 0.0f);
                    eEndStrip(view);
                }
            }

            if ((nVert & 1) != 0 && eBeginStrip(this->ShadowRampTexture, 3, biasedIdentity)) {
                eAddVertex(p[nVert - 1]);
                eAddVertex(mid);
                eAddVertex(p[0]);
                eAddColour(colour);
                eAddColour(colour);
                eAddColour(colour);
                eAddUV(0.155f, 0.0f);
                eAddUV(0.0f, 0.0f);
                eAddUV(0.155f, 0.0f);
                eEndStrip(view);
            }

            int fake = nVert / 3;
            int nStart = 0;
            int nStep;

            for (int j = 0; j < 3; j++) {
                int nSubVerts = (j + 1 > 2) ? nVert - nStart : fake;

                if (exBeginStrip(this->ShadowRampTexture, (nSubVerts + 1) * 2, biasedIdentity)) {
                    int nMax = nStart + nSubVerts;

                    for (i = nStart; i < nMax; i++) {
                        v = sh_Adjust(p[i], mid);
                        exAddVertex(p[i]);
                        exAddVertex(v);
                        exAddColour(colour);
                        exAddColour(colour);
                        exAddUV(0.155f, 0.0f);
                        exAddUV(1.0f, 0.0f);
                    }

                    if (i >= nVert) {
                        i = 0;
                    }

                    v = sh_Adjust(p[i], mid);
                    exAddVertex(p[i]);
                    exAddVertex(v);
                    exAddColour(colour);
                    exAddColour(colour);
                    exAddUV(0.155f, 0.0f);
                    exAddUV(1.0f, 0.0f);
                    exEndStrip(view);
                }

                nStart += fake;
            }
        }
    }
}

float heliScale = 0.43f;

static const float hull_DotCutoff = 20.0f;

// UNSOLVED, maybe equivalent?
void CarRenderInfo::DrawAmbientShadow(eView *view, const bVector3 *position, float shadow_scale, bMatrix4 *localWorld, bMatrix4 *worldLocal,
                                      bMatrix4 *biasedIdentity) {
    const int N = 16;

    hull_Origin = *position;

    if (IsGameFlowInGame()) {
        UMath::Vector3 usPoint;

        eUnSwizzleWorldVector(*position, reinterpret_cast<bVector3 &>(usPoint));
        this->mWorldPos.FindClosestFace(this->mWCollider, usPoint, false);
        if (this->mWorldPos.OnValidFace()) {
            this->mCar_elevation = this->mWorldPos.HeightAtPoint(usPoint);
            car_elevation = position->z - this->mCar_elevation;
        }
    }

    car_elevation_scale = 0.0f;
    if (car_elevation > 0.0f && car_elevation < 8.0f) {
        car_elevation_scale = car_elevation * 0.125f;
    } else if (car_elevation > 8.0f) {
        car_elevation_scale = 1.0f;
    }

    int in_front_end = static_cast<int>(IsGameFlowInFrontEnd());
    // TODO
    float scaleW;
    float scaleL;
    bVector3 min(this->AABBMin);
    bVector3 max(this->AABBMax);

    // TODO
    float scale = 1.05f;
    if (this->pRideInfo->Type == CARTYPE_COPHELI) {
        scale *= heliScale;
    }

    // TODO
    min.x *= scale;
    min.y *= scale;
    max.x *= scale;
    max.y *= scale;

    bVector3 SunCarVector;
    bVector3 light_pos;
    SunChunkInfo *sun_info = SunInfo;

    if (sun_info == nullptr) {
        light_pos.x = TweakCarShadowSunX;
        light_pos.y = TweakCarShadowSunY;
        light_pos.z = TweakCarShadowSunZ;
    } else {
        light_pos.x = sun_info->CarShadowPositionX;
        light_pos.y = sun_info->CarShadowPositionY;
        light_pos.z = sun_info->CarShadowPositionZ;
    }

    SunCarVector = light_pos - *position;
    bNormalize(&SunCarVector, &SunCarVector);

    float SunScale = (1.0f - SunCarVector.z) * TweakCarShadowSunScale;
    bVector3 sunpos_in_car_space;

    bMulMatrix(&sunpos_in_car_space, worldLocal, &light_pos);
    bNormalize(&sunpos_in_car_space, &sunpos_in_car_space);

    float sunAdjX = -sunpos_in_car_space.x * SunScale;
    float sunAdjY = -sunpos_in_car_space.y * SunScale;
    float sunDX = bAbs(sunAdjX * 0.33333334f);
    float sunDY = bAbs(sunAdjY * 0.33333334f);

    float sunStartX = sunAdjX > 0.0f ? 0.0f : sunAdjX;
    float sunStartY = sunAdjY > 0.0f ? 0.0f : sunAdjY;

    bVector3 p[16];
    bVector3 *pp;
    bVector2 uv[16];
    bVector2 *puv;
    int bad_points[4];

    pp = p;
    puv = uv;

    float py = min.y + sunStartY;
    scaleW = (max.y - min.y) * 0.33333334f;
    scaleL = (max.x - min.x) * 0.33333334f;
    float px;
    float dy = scaleW;
    float dx = scaleL;
    float ps;
    float pt = 0.0f;
    float ds = 0.33333334f;
    float dt = 0.33333334f;

    for (int y = 0; y < 4; y++) {
        px = min.x + sunStartX;
        ps = 0.0f;

        for (int x = 0; x < 4; x++) {
            pp->x = px;
            pp->y = py;
            pp->z = 0.0f;
            puv->x = ps;
            puv->y = pt;
            eMulVector(pp, localWorld, pp);
            px += sunDX;
            px += dx;
            ps += ds;
            pp++;
            puv++;
        }

        bad_points[y] = 0;
        py += sunDY;
        py += dy;
        pt += dt;
    }

    if (in_front_end != 0) {
        pp = p;

        for (int x = 0; x < N; x++) {
            pp->z = 0.0f;
            pp++;
        }
    } else if (this->mWCollider != nullptr) {
        UMath::Vector3 usCenter;
        bVector3 sCenter(*position);

        eUnSwizzleWorldVector(sCenter, reinterpret_cast<bVector3 &>(usCenter));
        this->mWorldPos.SetTolerance(TweakCarShadowTopologyTolerance);
        this->mWorldPos.FindClosestFace(this->mWCollider, usCenter, false);

        if (this->mWorldPos.OnValidFace()) {
            UMath::Vector4 worldNormal = {};

            worldNormal.y = 1.0f;
            this->mWorldPos.UNormal(&UMath::Vector4To3(worldNormal));
            UMath::Unitxyz(worldNormal, worldNormal);
            worldNormal.w = 0.0f;
            eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(worldNormal), hull_Normal);
        } else {
            hull_Normal.x = 0.0f;
            hull_Normal.y = 0.0f;
            hull_Normal.z = 1.0f;
        }

        bVector3 ref(0.0f, 0.0f, 0.0f);

        eMulVector(&ref, localWorld, &ref);
        this->mWorldPos.SetTolerance(TweakCarShadowTopologyTolerance);

        pp = p;
        bool quitIfSameFace = true;

        for (int x = 0; x < N; x++, pp++) {
            UMath::Vector3 usPoint;
            bVector3 sPoint(*pp);

            eUnSwizzleWorldVector(sPoint, reinterpret_cast<bVector3 &>(usPoint));
            this->mWorldPos.FindClosestFace(this->mWCollider, usPoint, quitIfSameFace);
            bool validFace = this->mWorldPos.OnValidFace();
            pp->z = this->mWorldPos.HeightAtPoint(usPoint);

            if (validFace && ZPassesTest(pp->z, ref.z) && DotPassesTest(pp)) {
                quitIfSameFace = true;
                continue;
            }

            quitIfSameFace = false;

            if (this->pRideInfo->Type == CARTYPE_COPHELI) {
                pp->z = -100.0f;
                bad_points[x / 4]++;
            } else {
                pp->z = this->mCar_elevation;
                ref.z = this->mCar_elevation;
            }
        }
    }

    float shadow_alpha_scale = bAbs(localWorld->v2.z);
    unsigned int shadow_colour;

    shadow_alpha_scale *= 1.0f - car_elevation_scale;

    float shadow_alpha_min = in_front_end != 0 ? 110.0f : 0.0f;
    float shadow_alpha_max = in_front_end != 0 ? 110.0f : 25.0f;

    float shadow_alpha = (shadow_alpha_max - shadow_alpha_min) * shadow_alpha_scale + shadow_alpha_min;
    int shadow_alphai = bClamp(static_cast<int>(shadow_alpha), 0, 0xFE);

    shadow_colour = static_cast<unsigned int>(shadow_alphai << 24);
    TextureInfo *texture_info = this->ShadowTexture;
    shadow_colour |= 0x00808080;
    if (texture_info == nullptr || (shadow_colour & 0xFF000000) == 0) {
        return;
    }

    unsigned int colour = shadow_colour;

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

void CarRenderInfo::InitEmitterPositions(bVector4 *tire_positions) {
    if (this->pCarTypeInfo != nullptr && !this->mEmitterPositionsInitialized) {
        int fxpos;

        for (fxpos = 0; fxpos < NUM_CARFXPOS; fxpos++) {
            int pos_count = 0;
            bool is_based_off_position_marker;

            is_based_off_position_marker = GetNumCarEffectMarkerHashes(static_cast<CarEffectPosition>(fxpos), pos_count);

            if (is_based_off_position_marker) {
                if (pos_count > 0) {
                    this->GetEmitterPositions(this->EmitterPositionList[fxpos], GetCarEffectMarkerHashes(static_cast<CarEffectPosition>(fxpos)),
                                              pos_count);
                }
                continue;
            }

            {
                CarEffectPosition efxpos = static_cast<CarEffectPosition>(fxpos);
                CarEmitterPosition *empos = nullptr;
                bSList<CarEmitterPosition> &pos_list = this->EmitterPositionList[fxpos];

                switch (efxpos) {
                    case CARFXPOS_NONE:
                        empos = new CarEmitterPosition(0.0f, 0.0f, 0.0f);
                        pos_list.AddTail(empos);
                        break;

                    case CARFXPOS_FRONT_TIRES: {
                        bVector4 *fl = tire_positions;
                        bVector4 *fr = tire_positions + 1;
                        bVector4 avg = *fl + *fr;

                        avg *= 0.5f;
                        empos = new CarEmitterPosition(avg.x, avg.y, avg.z);
                        pos_list.AddTail(empos);
                    } break;

                    case CARFXPOS_REAR_TIRES: {
                        bVector4 *rr = tire_positions + 2;
                        bVector4 *rl = tire_positions + 3;
                        bVector4 avg = *rr + *rl;

                        avg *= 0.5f;
                        empos = new CarEmitterPosition(avg.x, avg.y, avg.z);
                        pos_list.AddTail(empos);
                    } break;

                    case CARFXPOS_LEFT_TIRES: {
                        bVector4 *fl = tire_positions;
                        bVector4 *rl = tire_positions + 3;
                        bVector4 avg = *fl + *rl;

                        avg *= 0.5f;
                        empos = new CarEmitterPosition(avg.x, avg.y, avg.z);
                        pos_list.AddTail(empos);
                    } break;

                    case CARFXPOS_RIGHT_TIRES: {
                        bVector4 *fr = tire_positions + 1;
                        bVector4 *rr = tire_positions + 2;
                        bVector4 avg = *fr + *rr;

                        avg *= 0.5f;
                        empos = new CarEmitterPosition(avg.x, avg.y, avg.z);
                        pos_list.AddTail(empos);
                    } break;

                    case CARFXPOS_TIRE_FL:
                        empos = new CarEmitterPosition(tire_positions[0].x, tire_positions[0].y, tire_positions[0].z);
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

                    case CARFXPOS_ENGINE: {
                        bVector4 *fl = tire_positions;
                        bVector4 *fr = tire_positions + 1;
                        bVector4 avg = *fl + *fr;

                        avg *= 0.5f;

                        bVector4 diff;
                        bSub(&diff, fl, fr);

                        empos = new CarEmitterPosition(avg.x, avg.y, avg.z + diff.y * 0.2f);
                        pos_list.AddTail(empos);
                    } break;
                }
            }
        }

        this->mEmitterPositionsInitialized = true;
    }
}

int CarRenderInfo::GetEmitterPositions(bSList<CarEmitterPosition> &markers_out, const unsigned int *position_name_hashes, int num_pos_name_hashes) {
    if (this->pCarTypeInfo == nullptr) {
        return 0;
    }

    int count = 0;

    for (int slot_model_index = 0; slot_model_index < CARSLOTID_MODEL_NUM; slot_model_index++) {
        eModel *model = this->mCarPartModels[slot_model_index][0][this->mMinLodLevel].GetModel();
        ePositionMarker *position_marker = nullptr;

        if (model != nullptr) {
            while ((position_marker = model->GetPostionMarker(position_marker)) != nullptr) {
                unsigned int position_marker_namehash;
                for (int i = 0; i < num_pos_name_hashes; i++) {
                    position_marker_namehash = position_marker->NameHash;
                    if (position_marker_namehash == position_name_hashes[i]) {
                        CarEmitterPosition *empos = new CarEmitterPosition(position_marker);

                        markers_out.AddTail(empos);
                        count++;
                    }
                }
            }
        }
    }

    return count;
}

// STRIPPED
void CarRenderInfo::GetLightPositions(bSList<CarEmitterPosition> &pos, unsigned int position) {}

static const bVector3 EPfe(1.0f, 1.0f, 1.0f);

void UpdateEnvironmentMapCameras() {
    bVector3 *car_world_position = nullptr;
    eView *view = eGetView(1, false);

    if (view != nullptr && view->GetCameraMover() != nullptr) {
        CameraAnchor *anchor = view->GetCameraMover()->GetAnchor();

        if (anchor != nullptr) {
            car_world_position = anchor->GetGeometryPosition();
        } else {
            static bVector3 sCarWorldPosition;
            IPlayer *firstPlayer;

            if (IPlayer::First(PLAYER_LOCAL) != nullptr) {
                firstPlayer = IPlayer::First(PLAYER_LOCAL);

                {
                    IRigidBody *player_rigid_body = firstPlayer->GetSimable()->GetRigidBody();

                    if (player_rigid_body != nullptr) {
                        eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(player_rigid_body->GetPosition()), sCarWorldPosition);
                        bSub(&sCarWorldPosition, &sCarWorldPosition, &EnvMapEyeOffset);
                        car_world_position = &sCarWorldPosition;
                    }
                }
            }
        }
    }

    if (car_world_position == nullptr) {
        if (!FrontEndRenderingCarList.IsEmpty()) {
            FrontEndRenderingCar *fecar = FrontEndRenderingCarList.GetHead();
            car_world_position = &fecar->Position;
        }

        if (car_world_position == nullptr) {
            return;
        }
    }

    {
        bVector3 camera_eye_position(*eGetView(1, false)->GetCamera()->GetPosition());
        bVector3 envmap_position;
        bAdd(&envmap_position, &EnvMapEyeOffset, car_world_position);
        bAdd(&camera_eye_position, &EnvMapCamOffset, &camera_eye_position);

        eEnvMap *envmap = eGetEnvMap();
        envmap->UpdateCameras(&camera_eye_position, &envmap_position);
    }
}

// STRIPPED
int IsAnyFrontEndCars() {}

// STRIPPED
void RenderFrontEndCarNeon(eView *view, int reflection) {}

// STRIPPED
void SetFeCarRenderFlagsGlob(int draw_solid, int draw_alpha, int draw_shadows) {}

void RefreshAllFrontEndCarRenderInfos(CarType type) {
    for (FrontEndRenderingCar *fecar = FrontEndRenderingCarList.GetHead(); fecar != FrontEndRenderingCarList.EndOfList(); fecar = fecar->GetNext()) {
        if ((type == CARTYPE_NONE || fecar->GetCarType() == type) && fecar->GetRenderInfo() != nullptr) {
            fecar->GetRenderInfo()->Refresh();
        }
    }
}

bVector3 feposoff(0.0f, 0.0f, -0.35f);

int taildrag = 4;

void RenderFrontEndCars(eView *view, int reflection) {
    if (DrawCars == 0) {
        return;
    }

    if (reflection && FEManager::Get()->GetGarageType() == GARAGETYPE_CAR_LOT) {
        return;
    }

    EVIEWMODE view_mode = eGetCurrentViewMode();

    for (FrontEndRenderingCar *fecar = FrontEndRenderingCarList.GetHead(); fecar != FrontEndRenderingCarList.EndOfList(); fecar = fecar->GetNext()) {
        CarRenderInfo *info = fecar->RenderInfo;

        if (info != nullptr && fecar->Visible != 0) {
            CARPART_LOD car_lod = info->GetMinLodLevel();
            CARPART_LOD tire_lod = info->GetMinLodLevel();
            bMatrix4 cbm(fecar->BodyMatrix);
            bVector3 cpos(fecar->Position);
            int render_flags = 0;
            int force_light_state = 0;

            if (reflection) {
                render_flags = 1;

                cbm.v2.x = -cbm.v2.x;
                cbm.v2.y = -cbm.v2.y;
                cbm.v2.z = -cbm.v2.z;

                cpos += reinterpret_cast<bVector3 &>(feposoff);

                float reflection_offset = info->GetAttributes().ReflectionOffset();
                bVector3 offset;

                offset.x = cbm.v2.x * reflection_offset;
                offset.y = cbm.v2.y * reflection_offset;
                offset.z = cbm.v2.z * reflection_offset;

                cpos += offset;
            }

            info->Render(view, &cpos, &cbm, fecar->TireMatrices, fecar->BrakeMatrices, fecar->TireMatrices, render_flags, force_light_state,
                         reflection, 1.0f, car_lod, tire_lod);
        }
    }
}

void RenderFEFlares(eView *view, int reflexion) {}

// STRIPEPD
void SetSpecialFX(Car *car, int fx) {}

// STRIPEPD
void UpdateCarStreakingFlares(eView *view) {}

float CarLightSize[2] = {90.0f, 10.0f};

float CarLightFarStart[2] = {0.0f, 0.0f};
float CarLightFalloff[2] = {10.0f, 10.0f};

float CarLightOffsets[2] = {0.0f, 1.0f};
uint32 CarLightColours[2] = {0x00B0B0B0, 0x00000090};
bAngle CarLightAngles[2] = {0x36B0, 0x4000};

unsigned int lastlight = 2;

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
