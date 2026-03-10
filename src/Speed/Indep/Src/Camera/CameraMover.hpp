#ifndef CAMERA_CAMERAMOVER_H
#define CAMERA_CAMERAMOVER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "./Camera.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/camerainfo.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/ecar.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"

class eView;
class tCubic3D;
template <class T> class tAverage;

struct tCubic1D {
    float Val;         // offset 0x0, size 0x4
    float dVal;        // offset 0x4, size 0x4
    float ValDesired;  // offset 0x8, size 0x4
    float dValDesired; // offset 0xC, size 0x4
    float Coeff[4];    // offset 0x10, size 0x10
    float time;        // offset 0x20, size 0x4
    float duration;    // offset 0x24, size 0x4
    short state;       // offset 0x28, size 0x2
    short flags;       // offset 0x2A, size 0x2

    tCubic1D(short type, float dur)
        : Val(0.0f), //
          dVal(0.0f), //
          ValDesired(0.0f), //
          dValDesired(0.0f), //
          time(0.0f), //
          duration(dur), //
          state(0), //
          flags(type) {
        Coeff[0] = 0.0f;
        Coeff[1] = 0.0f;
        Coeff[2] = 0.0f;
        Coeff[3] = 0.0f;
    }

    void SetVal(const float v) {
        Val = v;
        if (v != ValDesired) {
            state = 2;
        }
    }

    void SetdVal(float v) {
        dVal = v;
        if (v != dValDesired) {
            state = 2;
        }
    }

    void SetValDesired(float v) {
        ValDesired = v;
        if (v != Val) {
            state = 2;
        }
    }

    void SetDuration(const float t) {
        duration = t;
    }

    void SetFlags(short f) {
        flags = f;
    }

    void Snap() {
        Val = ValDesired;
        dVal = dValDesired;
        state = 0;
    }
};

struct tCubic3D {
    tCubic1D x; // offset 0x0, size 0x2C
    tCubic1D y; // offset 0x2C, size 0x2C
    tCubic1D z; // offset 0x58, size 0x2C

    tCubic3D(short type, float dur)
        : x(type, dur), //
          y(type, dur), //
          z(type, dur) {}

    tCubic3D(short type, bVector3 *pDuration)
        : x(type, pDuration->x), //
          y(type, pDuration->y), //
          z(type, pDuration->z) {}

    void SetVal(const bVector3 *pV);
    void SetdVal(bVector3 *pV);
    void SetValDesired(const bVector3 *pV) {
        x.SetValDesired(pV->x);
        y.SetValDesired(pV->y);
        z.SetValDesired(pV->z);
    }

    void GetVal(bVector3 *pV) {
        pV->x = x.Val;
        pV->y = y.Val;
        pV->z = z.Val;
    }

    void GetValDesired(bVector3 *pV) {
        pV->x = x.ValDesired;
        pV->y = y.ValDesired;
        pV->z = z.ValDesired;
    }

    void Snap() {
        x.Snap();
        y.Snap();
        z.Snap();
    }
};

struct Bezier {
    bMatrix4 *pControlPoints; // offset 0x0, size 0x4
    bMatrix4 mBasis;          // offset 0x4, size 0x40

    Bezier();
    void GetPoint(bVector3 *pPoint, float parameter);

    void SetControlPoints(bMatrix4 *pPoints) {
        pControlPoints = pPoints;
    }
};

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

// total size: 0x124
class CameraAnchor {
  public:
    CameraAnchor(int model);
    ~CameraAnchor();

    bVector3 *GetGeomPos() {
        return &mGeomPos;
    }

    const bVector3 *GetVelocity() const {
        return &mVelocity;
    }

    const bVector3 *GetAccel() const {
        return &mAccel;
    }

    bMatrix4 *GetMatrix() {
        return &mGeomRot;
    }

    float GetVelMag() const {
        return mVelMag;
    }

    float GetTopSpeed() const {
        return mTopSpeed;
    }

    const SimSurface &GetSurface() const {
        return mSurface;
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

    unsigned int GetWorldID() const {
        return mWorldID;
    }

    void Update(float dT, const bMatrix4 &matrix, const bVector3 &velocity, const bVector3 &forward);
    void SetModel(int model);
    POV *GetPov(int pov_type);

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
    CameraMover();
    CameraMover(int view_id, CameraMoverTypes type);

    CameraMoverTypes GetType() {
        return Type;
    }

    bVector3 *GetPosition() {
        return pCamera->GetPosition();
    }

    Camera *GetCamera() {
        return pCamera;
    }

    // Virtual methods
    virtual ~CameraMover();

    virtual void Update(float dT);
    virtual void Render(eView *view);

    virtual CameraAnchor *GetAnchor();

    virtual void SetLookBack(bool b) {}

    virtual void SetLookbackSpeed(float speed) {}

    virtual void SetDisableLag(bool disable) {}

    virtual void SetPovType(int pov_type) {}

    virtual bool OutsidePOV();

    virtual bool RenderCarPOV();

    virtual float MinDistToWall();

    virtual unsigned short GetLookbackAngle();

    virtual void ResetState();

    // ICollisionHandler
    bool OnWCollide(const WCollisionMgr::WorldCollisionInfo &cInfo, const UMath::Vector3 &cPoint, void *userdata) override;

    virtual void Enable();
    virtual void Disable();

    virtual bVector3 *GetTarget();

    bool IsSomethingInBetween(const UMath::Vector4 &start, const UMath::Vector4 &end);
    bool IsSomethingInBetween(const bVector3 *start, const bVector3 *end);
    void HandheldNoise(bMatrix4 *world_to_camera, float f_scale, bool useWorldTimer);
    void ChopperNoise(bMatrix4 *world_to_camera, float f_scale, bool useWorldTimer);
    void TerrainVelocityNoise(bMatrix4 *world_to_camera, CameraAnchor *car, float speed_scale, float terrain_scale);
    static void ComputeBankedUpVector(bVector3 *up, bVector3 *eye, bVector3 *look, unsigned short bank);
    void IsoProjectionMatrix(bMatrix4 *pMatrix, bVector3 *pEye, bVector3 *pLook, bVector2 *pProjection);
    double AdjustHeightAroundCar(const bVector3 *car_pos, bVector3 *pEye, bVector3 *pForward);
    int MinGapCars(bMatrix4 *pMatrix, bVector3 *pLook, bVector3 *pForward);
    int MinGapTopology(bMatrix4 *pMatrix, bVector3 *pLook);
    void FovCubicInit(tCubic1D *cubic);
    void EyeCubicInit(tCubic3D *eye, bMatrix4 *matrix, bVector3 *target);
    void LookCubicInit(tCubic3D *look, bMatrix4 *matrix, bVector3 *target);
    void SetEyeLook(tCubic3D *eye, tCubic3D *look, tCubic1D *fov, bMatrix4 *matrix, bVector3 *target);

    unsigned int GetAnchorID();
    int IsHoodCamera();

  private:
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

void CameraMoverRestartRace();

class CubicCameraMover : public CameraMover {
  public:
    CubicCameraMover(int nView, CameraAnchor *pCar, int pov_type, bool disable_lag, bool look_back, bool perfect_focus, bool snap_next);
    virtual ~CubicCameraMover();

    virtual void Update(float dT);
    virtual CameraAnchor *GetAnchor();
    virtual void SetLookBack(bool b);
    virtual void SetDisableLag(bool disable);
    virtual void SetPovType(int pov_type);
    virtual bool OutsidePOV();
    virtual bool RenderCarPOV();
    virtual float MinDistToWall();
    virtual unsigned short GetLookbackAngle();
    virtual void ResetState();

    int IsHoodCamera();
    bool HighliteMode();
    void SetSnapNext();
    void SetForward(POV *pov, bool snap);
    void MakeSpace(bMatrix4 *pMatrix);
    void CameraAccelCurve(bVector3 *pAccel);
    void CameraSpeedHug(bVector3 *pForward);

  private:
    CameraAnchor *pCar;                // offset 0x80, size 0x4
    tCubic1D *pFov;                    // offset 0x84, size 0x4
    tCubic3D *pEye;                    // offset 0x88, size 0x4
    tCubic3D *pLook;                   // offset 0x8C, size 0x4
    tCubic3D *pForward;                // offset 0x90, size 0x4
    tCubic3D *pUp;                     // offset 0x94, size 0x4
    int nPovType;                      // offset 0x98, size 0x4
    int nPovTypeUsed;                  // offset 0x9C, size 0x4
    int bAccelLag;                     // offset 0xA0, size 0x4
    int bLookBack;                     // offset 0xA4, size 0x4
    int bSnapNext;                     // offset 0xA8, size 0x4
    int bPerfectFocus;                 // offset 0xAC, size 0x4
    int bFirstTime;                    // offset 0xB0, size 0x4
    Timer tLastGrounded;               // offset 0xB4, size 0x4
    Timer tLastUnderVehicle;           // offset 0xB8, size 0x4
    Timer tLastGearChange;             // offset 0xBC, size 0x4
    float fIgnoreSetSnapNextTimer;     // offset 0xC0, size 0x4
    bVector3 vSavedEye;                // offset 0xC4, size 0x10
    bVector2 vCameraImpcat;            // offset 0xD4, size 0x8
    bVector2 vCameraImpcatTimer;       // offset 0xDC, size 0x8
    tAverage<bVector3> *pAvgAccel;     // offset 0xE4, size 0x4
};

class RearViewMirrorCameraMover : public CameraMover {
  public:
    RearViewMirrorCameraMover(int view_id, CameraAnchor *car);
    virtual ~RearViewMirrorCameraMover();

    virtual void Update(float dT);
    virtual CameraAnchor *GetAnchor();

  private:
    CameraAnchor *pCar; // offset 0x80, size 0x4
};

class TrackCarCameraMover : public CameraMover {
  public:
    TrackCarCameraMover(int nView, CameraAnchor *pCar, bool focus_effects);
    virtual ~TrackCarCameraMover();

    virtual void Update(float dT);
    virtual CameraAnchor *GetAnchor();
    virtual bVector3 *GetTarget();

  private:
    bVector3 Eye;                // offset 0x80, size 0x10
    bVector3 Look;               // offset 0x90, size 0x10
    CameraAnchor *CarToFollow;   // offset 0xA0, size 0x4
    tCubic1D FocalDistCubic;     // offset 0xA4, size 0x2C
    int FocusEffects;            // offset 0xD0, size 0x4
    int CameraType;              // offset 0xD4, size 0x4

    void Init();
};

class TrackCopCameraMover : public CameraMover {
  public:
    TrackCopCameraMover(int nView, CameraAnchor *pCar, bool focus_effects);
    virtual ~TrackCopCameraMover();

    virtual void Update(float dT);
    virtual CameraAnchor *GetAnchor();
    virtual bool RenderCarPOV();
    virtual bVector3 *GetTarget();

  private:
    Bezier ZoomSpline;           // offset 0x80, size 0x44
    bMatrix4 ZoomVerts;          // offset 0xC4, size 0x40
    float ZoomSplineParam;       // offset 0x104, size 0x4
    Bezier EyeSpline;            // offset 0x108, size 0x44
    bMatrix4 EyeVerts;           // offset 0x14C, size 0x40
    float EyeSplineParam;        // offset 0x18C, size 0x4
    Bezier LookSpline;           // offset 0x190, size 0x44
    bMatrix4 LookVerts;          // offset 0x1D4, size 0x40
    float LookSplineParam;       // offset 0x214, size 0x4
    CameraAnchor *CarToFollow;   // offset 0x218, size 0x4
    tCubic1D FocalDistCubic;     // offset 0x21C, size 0x2C
    int FocusEffects;            // offset 0x248, size 0x4
    bool bRenderCarPOV;          // offset 0x24C, size 0x4

    bool FindPursuitVehiclePosition(bVector3 *pPosition);
    void Init();
};

#endif
