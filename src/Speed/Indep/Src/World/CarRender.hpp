#ifndef WORLD_CARRENDER_H
#define WORLD_CARRENDER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "./CarInfo.hpp"
#include "Interfaces/IVehicleDamageBehaviour.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/Ecstasy/eModel.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/ecar.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

/////// NOT IN THIS FILE ///////
class ePointSprite3D {
  public:
    // Functions
    void *operator new(size_t size) {}

    ePointSprite3D() {}
    ~ePointSprite3D() {}

    // Members
    float X;             // offset 0x0, size 0x4
    float Y;             // offset 0x4, size 0x4
    float Z;             // offset 0x8, size 0x4
    float Radius;        // offset 0xC, size 0x4
    float S1;            // offset 0x10, size 0x4
    float T1;            // offset 0x14, size 0x4
    float S0;            // offset 0x18, size 0x4
    float T0;            // offset 0x1C, size 0x4
    unsigned int Colour; // offset 0x20, size 0x4
    unsigned int pad0;   // offset 0x24, size 0x4
    float Cos;           // offset 0x28, size 0x4
    float Sin;           // offset 0x2C, size 0x4
};
////////////////////////////////

enum CarEffectPosition {
    CARFXPOS_NONE,
    CARFXPOS_FRONT_TIRES,
    CARFXPOS_REAR_TIRES,
    CARFXPOS_LEFT_TIRES,
    CARFXPOS_RIGHT_TIRES,
    CARFXPOS_TIRE_FL,
    CARFXPOS_TIRE_FR,
    CARFXPOS_TIRE_RR,
    CARFXPOS_TIRE_RL,
    CARFXPOS_ENGINE,
    CARFXPOS_EXHAUST,
    CARFXPOS_RADIATOR,
    CARFXPOS_WINDSHIELD,
    CARFXPOS_WINDOW_REAR,
    CARFXPOS_WINDOW_LEFT_FRONT,
    CARFXPOS_WINDOW_LEFT_REAR,
    CARFXPOS_WINDOW_RIGHT_FRONT,
    CARFXPOS_WINDOW_RIGHT_REAR,
    CARFXPOS_HEADLIGHT,
    CARFXPOS_HEADLIGHT_LEFT,
    CARFXPOS_HEADLIGHT_RIGHT,
    CARFXPOS_BRAKELIGHT,
    CARFXPOS_BRAKELIGHT_LEFT,
    CARFXPOS_BRAKELIGHT_RIGHT,
    CARFXPOS_SIDE_MIRROR_LEFT,
    CARFXPOS_SIDE_MIRROR_RIGHT,
    CARFXPOS_LICENSE_PLATE_FRONT,
    CARFXPOS_LICENSE_PLATE_REAR,
    NUM_CARFXPOS,
};

enum eCullableCarParts {
    CULLABLE_CAR_PART_TIRE_FL,
    CULLABLE_CAR_PART_TIRE_FR,
    CULLABLE_CAR_PART_TIRE_RR,
    CULLABLE_CAR_PART_TIRE_RL,
    CULLABLE_CAR_PART_BRAKE_FL,
    CULLABLE_CAR_PART_BRAKE_FR,
    CULLABLE_CAR_PART_BRAKE_RR,
    CULLABLE_CAR_PART_BRAKE_RL,
    CULLABLE_CAR_PART_SIDE_FRONT,
    CULLABLE_CAR_PART_SIDE_REAR,
    CULLABLE_CAR_PART_UNDERNEATH,
    NUM_CULLABLE_CAR_PARTS,
};

enum eCullingPolarity { CULLING_POLARITY_ANY_VISIBLE, CULLING_POLARITY_ALL_MUST_BE_VISIBLE };

struct CarFXPosInfo {
    // Members
    unsigned int position_marker_hashes[4]; // offset 0x0, size 0x10
    unsigned char marker_count;             // offset 0x10, size 0x1
};

class CarPartCullingPlaneInfo {
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
    // not real?
    CarPartCullingPlaneInfo();

    // Members
    eCullableCarParts Type;    // offset 0x0, size 0x4
    char *Name;                // offset 0x4, size 0x4
    eCullingPolarity Polarity; // offset 0x8, size 0x4
    int NumPlanes;             // offset 0xC, size 0x4
    bVector3 Normal[3];        // offset 0x10, size 0x30
    float NormalDistance[3];   // offset 0x40, size 0xC
};

class CarPartInfo {
  public:
    CarPartInfo() {}

    bVector3 Position;       // offset 0x0, size 0x10
    int NumCulledNotVisible; // offset 0x10, size 0x4
    int NumCulledVisible;    // offset 0x14, size 0x4
    bool IsVisible;          // offset 0x18, size 0x1
};

class CarPartCuller {
  public:
    void InitPart(eCullableCarParts type, const bVector3 *position);
    void CullParts(bVector3 *camera_eye, unsigned short stang);

    CarPartCuller() {}
    bool IsPartVisible(eCullableCarParts type) {
        return CarPartInfoTable[type].IsVisible;
    }

    CarPartInfo CarPartInfoTable[11];
};

typedef struct tagCarEffectParam {
    const char *Name;
    unsigned int NameHash;
} CarEffectParam;

class CarEmitterPosition : public bSNode<CarEmitterPosition> {
  public:
    // Functions
    static void *operator new(size_t size) {}

    static void operator delete(void *ptr) {}

    CarEmitterPosition(ePositionMarker *position_marker) {}

    CarEmitterPosition(float x, float y, float z) {}

    // Members
    float X;                         // offset 0x4, size 0x4
    float Y;                         // offset 0x8, size 0x4
    float Z;                         // offset 0xC, size 0x4
    ePositionMarker *PositionMarker; // offset 0x10, size 0x4
};

class UsedCarTextureInfo {
    // Members
    unsigned int TexturesToLoadPerm[87];        // offset 0x0, size 0x15C
    unsigned int TexturesToLoadTemp[87];        // offset 0x15C, size 0x15C
    int NumTexturesToLoadPerm;                  // offset 0x2B8, size 0x4
    int NumTexturesToLoadTemp;                  // offset 0x2BC, size 0x4
    unsigned int MappedSkinHash;                // offset 0x2C0, size 0x4
    unsigned int MappedSkinBHash;               // offset 0x2C4, size 0x4
    unsigned int MappedGlobalHash;              // offset 0x2C8, size 0x4
    unsigned int MappedWheelHash;               // offset 0x2CC, size 0x4
    unsigned int MappedSpinnerHash;             // offset 0x2D0, size 0x4
    unsigned int MappedBadging;                 // offset 0x2D4, size 0x4
    unsigned int MappedSpoilerHash;             // offset 0x2D8, size 0x4
    unsigned int MappedRoofScoopHash;           // offset 0x2DC, size 0x4
    unsigned int MappedDashSkinHash;            // offset 0x2E0, size 0x4
    unsigned int MappedLightHash[11];           // offset 0x2E4, size 0x2C
    unsigned int MappedTireHash;                // offset 0x310, size 0x4
    unsigned int MappedRimHash;                 // offset 0x314, size 0x4
    unsigned int MappedRimBlurHash;             // offset 0x318, size 0x4
    unsigned int MappedLicensePlateHash;        // offset 0x31C, size 0x4
    unsigned int ReplaceSkinHash;               // offset 0x320, size 0x4
    unsigned int ReplaceSkinBHash;              // offset 0x324, size 0x4
    unsigned int ReplaceGlobalHash;             // offset 0x328, size 0x4
    unsigned int ReplaceWheelHash;              // offset 0x32C, size 0x4
    unsigned int ReplaceSpinnerHash;            // offset 0x330, size 0x4
    unsigned int ReplaceSpoilerHash;            // offset 0x334, size 0x4
    unsigned int ReplaceRoofScoopHash;          // offset 0x338, size 0x4
    unsigned int ReplaceDashSkinHash;           // offset 0x33C, size 0x4
    unsigned int ReplaceHeadlightHash[3];       // offset 0x340, size 0xC
    unsigned int ReplaceHeadlightGlassHash[3];  // offset 0x34C, size 0xC
    unsigned int ReplaceBrakelightHash[3];      // offset 0x358, size 0xC
    unsigned int ReplaceBrakelightGlassHash[3]; // offset 0x364, size 0xC
    unsigned int ReplaceReverselightHash[3];    // offset 0x370, size 0xC
    unsigned int ShadowHash;                    // offset 0x37C, size 0x4
};

inline int IsNISCopCar(int type) {
    return type == CARTYPE_COPMIDSIZEINT;
}

class CarPartModel {
  public:
    // Functions
    CarPartModel() {}

    ~CarPartModel() {}

    void Clear() {}

    int IsHidden() {}

    void Hide(int bHide) {}

    struct eModel *GetModel() {
        return reinterpret_cast<eModel *>(mModel);
    }

    void SetModel(struct eModel *model) {
        this->mModel = reinterpret_cast<unsigned int>(model);
    }

    bool IsLodMissing() const {}

  private:
    unsigned int mModel; // offset 0x0, size 0x4
};

class CarRenderInfo {
  public:
    // Inner declarations
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

    // Functions
    void SetDeltaTime(float dt) {}

    float GetDeltaTime() const {}

    void SetRadius(float r) {}

    float GetRadius() const {}

    void SetCollider(const WCollider *collider) {}

    void SetAnimationTime(float animationTime) {}

    void SetWheelWobble(unsigned int wheelInd, bool enable) {}

    bool GetWheelWobble(unsigned int wheelInd) {}

    void SetFlashing(bool b) {}

    void UpdateFlashing() {}

    void SetCarRenderFlags(int draw_solid, int draw_alpha, int draw_shadows) {}

    void SetSpecialFX(int fx) {}

    int GetSpecialFX() {}

    RideInfo *GetRideInfo() const {}

    enum CARPART_LOD GetMinLodLevel() const {}

    enum CARPART_LOD GetMaxLodLevel() const {}

    enum CARPART_LOD GetMinReflectionLodLevel() const {}

    void EnableAlphaWrites(bool onOff) {}

    const Attrib::Gen::ecar &GetAttributes() const {
        return this->mAttributes;
    }

    void SetBrokenLights(unsigned int vehiclefx_ids) {}

    void SetLights(unsigned int vehiclefx_ids) {}

    bool IsLightBroken(enum VehicleFX::ID id) const {}

    bool IsLightOn(enum VehicleFX::ID id) const {}

    CarRenderInfo(RideInfo *ride_info);

    ~CarRenderInfo();

    void Init();

    void Refresh();

    void SetPlayerDamage(const Sim::Collision::Info &damageInfo);

    void SetCarDamageState(bool on, unsigned int startID, unsigned int endID);

    void SetCarGlassDamageState(bool on, CarReplacementTexID replacementId, unsigned int undamageHash, unsigned int damageHash);

    void SetDamageInfo(const Sim::Collision::Info &damageInfo);

    unsigned int FindCarPart(int slotId);

    unsigned int HideCarPart(int slotId, bool hide);

    void UpdateCarParts();

    void UpdateWheelYRenderOffset();

    void SwitchSkin(RideInfo *ride_info);

    void UpdateDecalTextures(RideInfo *ride_info);

    void UpdateCarReplacementTextures();

    void UpdateLightStateTextures();

    void CreateCarLightFlares();

    float CalculateTireBlur(int nWheel);

    void RenderTireBlur(eView *view, TextureInfo *texture_info, bMatrix4 *local_world, float alpha, eDynamicLightContext *light_context);

    void RenderTextureHeadlights(eView *view, bMatrix4 *l_w, unsigned int Accurate);

    void RenderFlaresOnCar(eView *view, const bVector3 *position, const bMatrix4 *body_matrix, int force_light_state, int reflexion,
                           int renderFlareFlags);

    bool Render(eView *view, const bVector3 *world_position, const bMatrix4 *body_matrix, bMatrix4 *tire_matrices, bMatrix4 *brake_matrices,
                bMatrix4 *spinner_matrices, unsigned int extra_render_flags, int force_light_state, int reflexion, float shadow_scale,
                enum CARPART_LOD tireLOD, enum CARPART_LOD carLOD);

    void convex_hull(bVector3 *p, const WCollider *wcoll, int &n, float Z, float zBias, int fast);

    void DrawKeithProjShadow(eView *view, const bVector3 *position, bMatrix4 *localWorld, bMatrix4 *worldLocal, bMatrix4 *biasedIdentity,
                             int body_lod);

    void DrawAmbientShadow(eView *view, const bVector3 *position, float shadow_scale, bMatrix4 *localWorld, bMatrix4 *worldLocal,
                           bMatrix4 *biasedIdentity);

    void RenderPart(eView *view, CarPartModel *carPart, bMatrix4 *local_to_world, eDynamicLightContext *light_context, unsigned int flags);

    void InitEmitterPositions(bVector4 *tire_positions);

    int GetEmitterPositions(bSList<CarEmitterPosition> &markers_out, const unsigned int *position_name_hashes, int num_pos_name_hashes);

    void GetLightPositions(bSList<CarEmitterPosition> &pos, unsigned int position);

    // Static members
    static bool mAlphaWritesEnabled; // size: 0x1, address: 0xFFFFFFFF

    // Members
    float AnimTime;                                                // offset 0x0, size 0x4
    bVector3 mVelocity;                                            // offset 0x4, size 0x10
    bVector3 mAngularVelocity;                                     // offset 0x14, size 0x10
    bVector3 mAcceleration;                                        // offset 0x24, size 0x10
    VehiclePartDamageBehaviour *mDamageBehaviour;                  // offset 0x34, size 0x4 // TODO IVehiclePartDamageBehaviour
    const WCollider *mWCollider;                                   // offset 0x38, size 0x4
    WWorldPos mWorldPos;                                           // offset 0x3C, size 0x3C
    RideInfo *pRideInfo;                                           // offset 0x78, size 0x4
    CarTypeInfo *pCarTypeInfo;                                     // offset 0x7C, size 0x4
    short LastCarPartChanged;                                      // offset 0x80, size 0x2
    bVector3 AABBMin;                                              // offset 0x84, size 0x10
    bVector3 AABBMax;                                              // offset 0x94, size 0x10
    bVector3 ModelOffset;                                          // offset 0xA4, size 0x10
    bVector3 PivotPosition;                                        // offset 0xB4, size 0x10
    TextureInfo *ShadowTexture;                                    // offset 0xC4, size 0x4
    TextureInfo *ShadowRampTexture;                                // offset 0xC8, size 0x4
    eLightMaterial *LightMaterial_CarSkin;                         // offset 0xCC, size 0x4
    eLightMaterial *LightMaterial_Carbon;                          // offset 0xD0, size 0x4
    eLightMaterial *LightMaterial_WindowTint;                      // offset 0xD4, size 0x4
    eLightMaterial *LightMaterial_WheelRim;                        // offset 0xD8, size 0x4
    eLightMaterial *LightMaterial_Caliper;                         // offset 0xDC, size 0x4
    eLightMaterial *LightMaterial_Spoiler;                         // offset 0xE0, size 0x4
    eLightMaterial *LightMaterial_Roof;                            // offset 0xE4, size 0x4
    eLightMaterial *LightMaterial_Spinner;                         // offset 0xE8, size 0x4
    int CarbonHood;                                                // offset 0xEC, size 0x4
    eDynamicLightContext *Car_light_context;                       // offset 0xF0, size 0x4
    unsigned int CarFrame;                                         // offset 0xF4, size 0x4
    bTList<eLightFlare> LightFlareList;                            // offset 0xF8, size 0x8
    float CarTimebaseStart;                                        // offset 0x100, size 0x4
    float WheelYRenderOffset[4];                                   // offset 0x104, size 0x10
    float WheelWidths[2];                                          // offset 0x114, size 0x8
    float WheelRadius[2];                                          // offset 0x11C, size 0x8
    float WheelWidthScales[4];                                     // offset 0x124, size 0x10
    float WheelRadiusScales[4];                                    // offset 0x134, size 0x10
    float WheelBrakeMarkerY[2];                                    // offset 0x144, size 0x8
    bool mEmitterPositionsInitialized;                             // offset 0x14C, size 0x1
    bSList<CarEmitterPosition> EmitterPositionList[28];            // offset 0x150, size 0xE0
    eReplacementTextureTable MasterReplacementTextureTable[73];    // offset 0x230, size 0x36C
    eReplacementTextureTable CarbonReplacementTextureTable[73];    // offset 0x59C, size 0x36C
    eReplacementTextureTable DecalReplacementTextureTable[48];     // offset 0x908, size 0x240
    eReplacementTextureTable BrakeLeftReplacementTextureTable[2];  // offset 0xB48, size 0x18
    eReplacementTextureTable BrakeRightReplacementTextureTable[2]; // offset 0xB60, size 0x18
    CarPartModel mCarPartModels[5][1][76];                         // offset 0xB78, size 0x5F0
    int SpecialFX;                                                 // offset 0x1168, size 0x4
    float mCar_elevation;                                          // offset 0x116C, size 0x4
    int NOSstate;                                                  // offset 0x1170, size 0x4
    unsigned short mSteeringR;                                     // offset 0x1174, size 0x2
    unsigned short mSteeringL;                                     // offset 0x1176, size 0x2
    bMatrix4 LastFewMatrices[3];                                   // offset 0x1178, size 0xC0
    bVector3 LastFewPositions[3];                                  // offset 0x1238, size 0x30
    int matrixIndex;                                               // offset 0x1268, size 0x4
    ePositionMarker *SpoilerPositionMarker;                        // offset 0x126C, size 0x4
    ePositionMarker *SpoilerPositionMarker2;                       // offset 0x1270, size 0x4
    ePositionMarker *RoofScoopPositionMarker;                      // offset 0x1274, size 0x4
    unsigned int AAflags;                                          // offset 0x1278, size 0x4
    int AAdraw_solid;                                              // offset 0x127C, size 0x4
    int AAdraw_alpha;                                              // offset 0x1280, size 0x4
    int AAdraw_shadows;                                            // offset 0x1284, size 0x4
    UsedCarTextureInfo mUsedTextureInfos;                          // offset 0x1288, size 0x380
    unsigned int mOnLights;                                        // offset 0x1608, size 0x4
    unsigned int mBrokenLights;                                    // offset 0x160C, size 0x4
    CARPART_LOD mMinLodLevel;                                      // offset 0x1610, size 0x4
    CARPART_LOD mMaxLodLevel;                                      // offset 0x1614, size 0x4
    CARPART_LOD mMinReflectionLodLevel;                            // offset 0x1618, size 0x4
    CarPartCuller TheCarPartCuller;                                // offset 0x161C, size 0x134
    COLLISION_INFO *mDamageZoneInfo;                               // offset 0x1750, size 0x4
    float mDeltaTime;                                              // offset 0x1754, size 0x4
    float mRadius;                                                 // offset 0x1758, size 0x4
    Attrib::Gen::ecar mAttributes;                                 // offset 0x175C, size 0x14
    bool mFlashing;                                                // offset 0x1770, size 0x1
    float mFlashInterval;                                          // offset 0x1774, size 0x4
    COLLISION_INFO *mDamageInfoCache;                              // offset 0x1778, size 0x4
    bool mWheelWobbleEnabled[4];                                   // offset 0x177C, size 0x4
    bool mMirrorLeftWheels;                                        // offset 0x178C, size 0x1
};

class FrontEndRenderingCar : public bTNode<FrontEndRenderingCar> {
  public:
    // Functions
    void SetPosition(bVector3 *position) {}

    void SetBodyMatrix(bMatrix4 *body_matrix) {}

    void SetTireMatrices(bMatrix4 *tire_matrices) {}

    void SetBrakeMatrices(bMatrix4 *brake_matrices) {}

    void SetTireMatrix(int n, bMatrix4 *m) {}

    void SetBrakeMatrix(int n, bMatrix4 *m) {}

    void SetOverrideModel(eModel *override_model) {}

    RideInfo *GetRideInfo() {}

    CarRenderInfo *GetRenderInfo() {}

    CarType GetCarType() {}

    FrontEndRenderingCar(RideInfo *ride_info, int view_id);

    bool LookupWheelPosition(unsigned int index, bVector4 *position);

    bool LookupWheelRadius(unsigned int index, float &radius);

    void ReInit(RideInfo *ride_info);

    ~FrontEndRenderingCar();

    void OverRideAlpha(unsigned char nAlphaBits, bool bSemiTrans, bool bWriteZ);

    void RestoreAlpha();

  private:
    // Members
    RideInfo mRideInfo; // offset 0x8, size 0x310

  public:
    CarRenderInfo *RenderInfo; // offset 0x318, size 0x4
    int ViewID;                // offset 0x31C, size 0x4
    bVector3 Position;         // offset 0x320, size 0x10
    bMatrix4 BodyMatrix;       // offset 0x330, size 0x40
    bMatrix4 TireMatrices[4];  // offset 0x370, size 0x100
    bMatrix4 BrakeMatrices[4]; // offset 0x470, size 0x100
    eModel *OverrideModel;     // offset 0x570, size 0x4
    int Visible;               // offset 0x574, size 0x4
    int nPasses;               // offset 0x578, size 0x4
    int Reflection;            // offset 0x57C, size 0x4
    int LightsOn;              // offset 0x580, size 0x4
    int CopLightsOn;           // offset 0x584, size 0x4
};

void InitCarRender();
void InitCarEffects();
void CloseCarEffects();
void InitStandardModels();

#endif
