#ifndef CAMERA_CAMERAMOVER_H
#define CAMERA_CAMERAMOVER_H

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "./Camera.hpp"
#include "./CameraAI.hpp"
#include "CameraInfo.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/camerainfo.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/ecar.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/World/WCollider.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/mtx.h>
#endif
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/bWare/Inc/Espresso.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Misc/Spline.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"

class eView;
struct tCubic1D;
struct tCubic3D;

enum CameraMoverTypes {
    CM_NONE_TYPE = 0,
    CM_DRIVE_CUBIC = 1,
    CM_DEBUG_WORLD = 2,
    CM_ROAD_EDITOR = 3,
    CM_ORBIT_CAR = 4,
    CM_REAR_VIEW_MIRROR = 5,
    CM_TRACK_CAR = 6,
    CM_TRACK_COP = 7,
    CM_MAX = 8,
    CM_STILL = 10,
    CM_SELECT_CAR = 9,
    CM_ZONE_FREEZE = 11,
    CM_ZONE_PREVIEW = 12,
    CM_AUTO_PILOT = 13,
    CM_ICE = 14,
    CM_ANIMATION_CONTROLLER = 15,
    CM_COP_VIEW = 16,
    CM_ANIMATION_ENTITY = 17,
    CM_SHOWCASE = 18,
};

extern const bVector4 CameraNoiseHandheldAmplitude;   // size: 0x10, address: 0x8045AB48
extern const bVector4 CameraNoiseHandheldFrequency;   // size: 0x10, address: 0x8045AB50
extern const bVector4 CameraNoiseChopperFrequency;    // size: 0x10, address: 0x8045AB58
extern const bVector4 CameraNoiseChopperAmplitude;    // size: 0x10, address: 0x8045AB68
extern const bVector4 CameraNoiseSpeedFrequency;      // size: 0x10, address: 0x8045AB78
extern const bVector4 CameraNoiseSpeedAmplitude;      // size: 0x10, address: 0x8045AB88
extern const bVector4 CameraNoiseTerrainFrequency;    // size: 0x10, address: 0x8045AB98
extern const bVector4 CameraNoiseTerrainAmplitude;    // size: 0x10, address: 0x8045ABA8

// total size: 0x124
class CameraAnchor {
  public:
    float GetVelocityMagnitude() {
        return mVelMag;
    }

    bVector3 *GetGeometryPosition() {
        return &mGeomPos;
    }

    bMatrix4 *GetGeometryOrientation() {
        return &mGeomRot;
    }

    unsigned int GetWorldID() const {
        return mWorldID;
    }

    void SetWorldID(unsigned int id) {
        mWorldID = id;
    }

    void SetTopSpeed(float speed) {
        mTopSpeed = speed;
    }

    void SetDimension(const bVector3 &dim) {
        mDimension = dim;
    }

    void SetZoom(float zoom) {
        mZoom = zoom;
    }

    void SetSurface(const SimSurface &surface) {
        mSurface = surface;
    }

    void SetCollisionDamping(float v) {
        mCollisionDamping = v;
    }

    void SetDrift(float v) {
        mDrift = v;
    }

    void SetGroundCollision(float v) {
        mGroundCollision = v;
    }

    void SetObjectCollision(float v) {
        mObjectCollision = v;
    }

    void SetNosEngaged(bool v) {
        mIsNosEngaged = v;
    }

    void SetBrakeEngaged(bool v) {
        mIsBrakeEngaged = v;
    }

    void SetDragRace(bool v) {
        mIsDragRace = v;
    }

    void SetOverRev(bool v) {
        mIsOverRev = v;
    }

    void SetTouchingGround(bool v) {
        mIsTouchingGround = v;
    }

    void SetVehicleDestroyed(bool v) {
        mIsVehicleDestroyed = v;
    }

    void SetGearChanging(bool v) {
        mIsGearChanging = v;
    }

    void SetCloseToRoadBlock(bool v) {
        mIsCloseToRoadBlock = v;
    }

    bVector3 *GetVelocity() {
        return &mVelocity;
    }

    bVector3 *GetAcceleration() {
        return &mAccel;
    }

    bVector3 *GetForwardVector() {
        return (bVector3 *)&mGeomRot.v0;
    }

    bVector3 *GetLeftVector() {
        return (bVector3 *)&mGeomRot.v1;
    }

    bVector3 *GetUpVector() {
        return (bVector3 *)&mGeomRot.v2;
    }

    const SimSurface &GetSurface() const {
        return mSurface;
    }

    float GetTopSpeed() const {
        return mTopSpeed;
    }

    bool IsNosEngaged() const {
        return mIsNosEngaged;
    }

    bool IsBrakeEngaged() const {
        return mIsBrakeEngaged;
    }

    bool IsDragRace() const {
        return mIsDragRace;
    }

    bool IsOverRev() const {
        return mIsOverRev;
    }

    bool IsTouchingGround() const {
        return mIsTouchingGround;
    }

    float GetCollisionDamping() const {
        return mCollisionDamping;
    }

    float GetDrift() const {
        return mDrift;
    }

    bool IsGearChanging() const {
        return mIsGearChanging;
    }

    bool IsCloseToRoadBlock() const {
        return mIsCloseToRoadBlock;
    }

    bool IsUpsideDown() {
        return GetUpVector()->z < 0.0f;
    }

    short GetPOVType() {
        return this->mPOV.Type;
    }

    CameraAnchor(int model);
    ~CameraAnchor();
    void SetModel(int model);
    POV *GetPov(int pov_type);
    void Update(float dT, const bMatrix4 &mat, const bVector3 &vel, const bVector3 &dim);

  private:
    bVector3 mVelocity;                            // offset 0x0, size 0x10
    float mVelMag;                                 // offset 0x10, size 0x4
    float mTopSpeed;                               // offset 0x14, size 0x4
    bVector3 mGeomPos;                             // offset 0x18, size 0x10
    bVector3 mDimension;                           // offset 0x28, size 0x10
    bVector3 mAccel;                               // offset 0x38, size 0x10
    bMatrix4 mGeomRot;                             // offset 0x48, size 0x40
    int mModel;                                    // offset 0x88, size 0x4
    unsigned int mWorldID;                         // offset 0x8C, size 0x4
    SimSurface mSurface;                           // offset 0x90, size 0x14
    float mCollisionDamping;                       // offset 0xA4, size 0x4
    float mDrift;                                  // offset 0xA8, size 0x4
    float mGroundCollision;                        // offset 0xAC, size 0x4
    float mObjectCollision;                        // offset 0xB0, size 0x4
    bool mIsNosEngaged;                            // offset 0xB4, size 0x1
    bool mIsBrakeEngaged;                          // offset 0xB8, size 0x1
    bool mIsDragRace;                              // offset 0xBC, size 0x1
    bool mIsOverRev;                               // offset 0xC0, size 0x1
    bool mIsTouchingGround;                        // offset 0xC4, size 0x1
    bool mIsVehicleDestroyed;                      // offset 0xC8, size 0x1
    bool mIsGearChanging;                          // offset 0xCC, size 0x1
    bool mIsCloseToRoadBlock;                      // offset 0xD0, size 0x1
    float mZoom;                                   // offset 0xD4, size 0x4
    POV mPOV;                                      // offset 0xD8, size 0x24
    Attrib::Gen::ecar mModelAttributes;            // offset 0xFC, size 0x14
    Attrib::Gen::camerainfo mCameraInfoAttributes; // offset 0x110, size 0x14
};

// total size: 0x80
class CameraMover : public bTNode<CameraMover>, public WCollisionMgr::ICollisionHandler {
  public:
    CameraMover(int view_id, CameraMoverTypes type);

    CameraMoverTypes GetType() {
        return Type;
    }

    static void ComputeBankedUpVector(bVector3 *up, bVector3 *eye, bVector3 *look, bAngle bank);

    WUID GetAnchorID();

    bVector3 *GetPosition() {
        return pCamera->GetPosition();
    }

    bVector3 *GetDirection() {
        return pCamera->GetDirection();
    }

    float GetDistanceTo(const bVector3 *to) {
        bVector3 rel;

        bSub(&rel, GetPosition(), to);
        return bLength(&rel);
    }

    // Virtual methods
    virtual ~CameraMover();

    virtual void Update(float dT);
    virtual void Render(eView *view);

    void ChopperNoise(bMatrix4 *world_to_camera, float f_scale, bool useWorldTimer);
    bool MinGapTopology(bMatrix4 *dest, bVector3 *pos);
    bool EnforceMinGapToWalls(WCollider *collider, bVector3 *pos, bVector3 *target, bVector4 *push);
    void HandheldNoise(bMatrix4 *world_to_camera, float f_scale, bool useWorldTimer);
    void TerrainVelocityNoise(bMatrix4 *world_to_camera /* r26 */, CameraAnchor *p_car /* r30 */, float f_speed_scale /* f31 */,
                              float f_terrain_scale /* f28 */);
    bool IsSomethingInBetween(const UMath::Vector4 &pos1, const UMath::Vector4 &pos2);
    bool IsSomethingInBetween(const bVector3 *pos1, const bVector3 *pos2);
    void IsoProjectionMatrix(bMatrix4 *dest, bVector3 *eye, bVector3 *look, bVector2 *offset);
    float AdjustHeightAroundCar(const bVector3 *pos, bVector3 *eye, bVector3 *look);
    bVector3 *DutchAroundCar(bVector3 *eye, bVector3 *look);
    bool MinGapCars(bMatrix4 *dest, bVector3 *pos, bVector3 *dim);

    void FovCubicInit(tCubic1D *cubic);
    void EyeCubicInit(tCubic3D *cubic, bMatrix4 *world_to_camera, bVector3 *offset);
    void LookCubicInit(tCubic3D *cubic, bMatrix4 *world_to_camera, bVector3 *offset);
    void SetEyeLook(tCubic3D *eye, tCubic3D *look, tCubic1D *fov, bMatrix4 *world_to_camera, bVector3 *offset);

    virtual CameraAnchor *GetAnchor() { return 0; }

    virtual void SetLookBack(bool b) {}

    virtual void SetLookbackSpeed(float speed) {}

    virtual void SetDisableLag(bool disable) {}

    virtual void SetPovType(int pov_type) {}

    virtual bool OutsidePOV() { return true; }

    // Original returns the raw 4-byte value (TrackCopCameraMover::RenderCarPOV = lwz+blr with no
    // bool normalization), so the original signature returned int.
    virtual int RenderCarPOV() { return 1; }

    virtual float MinDistToWall();

    virtual unsigned short GetLookbackAngle() { return 0; }

    virtual void ResetState() {}

    // ICollisionHandler
    bool OnWCollide(const WCollisionMgr::WorldCollisionInfo &cInfo, const UMath::Vector3 &cPoint, void *userdata) override;

    virtual void Enable();
    virtual void Disable();

    virtual bool IsHoodCamera() { return false; }

    bool IsDriveCamera() {
        return Type == CM_DRIVE_CUBIC;
    }

    Camera *GetCamera() {
        return pCamera;
    }

    virtual bVector3 *GetTarget() {
        return pCamera->GetTarget();
    }

  protected:
    CameraMoverTypes Type;       // offset 0xC, size 0x4
    int ViewID;                  // offset 0x10, size 0x4
    int Enabled;                 // offset 0x14, size 0x4
    eView *pView;                // offset 0x18, size 0x4
    Camera *pCamera;             // offset 0x1C, size 0x4
    int RenderDash;              // offset 0x20, size 0x4
    struct WCollider *mCollider; // offset 0x24, size 0x4
    WWorldPos mWPos;             // offset 0x28, size 0x3C
    float fAccumulatedClearance; // offset 0x64, size 0x4
    float fAccumulatedAdjust;    // offset 0x68, size 0x4
    float fSavedAdjust;          // offset 0x6C, size 0x4
    bVector3 vSavedForward;      // offset 0x70, size 0x10
};

struct CubicPovData;

// tAverage lives in Misc/Table.hpp

bool OutsidePovType(int nType);
bool RenderCarPovType(int nType, bool bLookBack);

// total size: 0xE8
class CubicCameraMover : public CameraMover {
  public:
    void SetLookBack(bool b) override { bLookBack = b; }
    void SetDisableLag(bool off) override { bAccelLag = !off; }

    unsigned short GetLookbackAngle() override {
        if (!bLookBack) {
            return 0;
        }
        return 0x8000;
    }

    CameraAnchor *GetAnchor() override { return pCar; }

    bool IsHoodCamera() override;

    CubicCameraMover(int nView, CameraAnchor *p_car, int pov_type, bool smooth, bool disable_lag, bool look_back, bool perfect_focus);
    ~CubicCameraMover() override;

    float MinDistToWall() override;
    bool OutsidePOV() override;
    int RenderCarPOV() override;
    bool HighliteMode();
    void SetSnapNext();
    void SetPovType(int pov_type) override;
    void ResetState() override;
    bool IsUnderVehicle();
    void SetForward(POV *pov, bool bSnap);
    void MakeSpace(bMatrix4 *pMatrix);
    void CameraAccelCurve(bVector3 *pAccel);
    void CameraSpeedHug(bVector3 *pEyeOffset);
    void SetDesired(bMatrix4 *pCarToWorld, POV *pov, CubicPovData *pov_data, bool bSnapForward);
    void Update(float dT) override;

  protected:
    CameraAnchor *pCar;                // offset 0x80, size 0x4
    tCubic1D *pFov;                    // offset 0x84, size 0x4
    tCubic3D *pEye;                    // offset 0x88, size 0x4
    tCubic3D *pLook;                   // offset 0x8C, size 0x4
    tCubic3D *pForward;                // offset 0x90, size 0x4
    tCubic3D *pUp;                     // offset 0x94, size 0x4
    int nPovType;                      // offset 0x98, size 0x4
    int nPovTypeUsed;                  // offset 0x9C, size 0x4
    bool bAccelLag;                    // offset 0xA0, size 0x1
    bool bLookBack;                    // offset 0xA4, size 0x1
    bool bSnapNext;                    // offset 0xA8, size 0x1
    bool bPerfectFocus;                // offset 0xAC, size 0x1
    bool bFirstTime;                   // offset 0xB0, size 0x1
    Timer tLastGrounded;               // offset 0xB4, size 0x4
    Timer tLastUnderVehicle;           // offset 0xB8, size 0x4
    Timer tLastGearChange;             // offset 0xBC, size 0x4
    float fIgnoreSetSnapNextTimer;     // offset 0xC0, size 0x4
    bVector3 vSavedEye;                // offset 0xC4, size 0x10
    bVector2 vCameraImpcat;            // offset 0xD4, size 0x8
    bVector2 vCameraImpcatTimer;       // offset 0xDC, size 0x8
    tAverage<bVector3> *pAvgAccel;     // offset 0xE4, size 0x4
};

void CameraMoverRestartRace();
void UpdateCameraMovers(float dT);
void RenderCameraMovers(eView *view);

static inline void bMatrixCopy(bMatrix4 &dst, const bMatrix4 &src) {
#ifdef EA_PLATFORM_GAMECUBE
    PSMTX44Copy(reinterpret_cast<const float(*)[4]>(&src), reinterpret_cast<float(*)[4]>(&dst));
#else
    dst = src;
#endif
}

static inline void bSetPosition(bVector4 &dst, const UMath::Vector3 &src) {
    dst.x = src.z;
    dst.y = -src.x;
    dst.z = src.y;
}

#endif
