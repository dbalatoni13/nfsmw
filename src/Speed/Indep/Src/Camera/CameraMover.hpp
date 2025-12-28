#ifndef CAMERA_CAMERAMOVER_H
#define CAMERA_CAMERAMOVER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "./Camera.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/camerainfo.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/ecar.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

class eView;

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
    unsigned int GetWorldID() const {
        return mWorldID;
    }

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

    CameraMoverTypes GetType() {
        return Type;
    }

    bVector3 *GetPosition() {
        return pCamera->GetPosition();
    }

    // Virtual methods
    virtual ~CameraMover();

    virtual void Update(float dT);
    virtual void Render(eView *view);

    virtual CameraAnchor *GetAnchor() {}

    virtual void SetLookBack(bool b) {}

    virtual void SetLookbackSpeed(float speed) {}

    virtual void SetDisableLag(bool disable) {}

    virtual void SetPovType(int pov_type) {}

    virtual bool OutsidePOV() {}

    virtual bool RenderCarPOV() {}

    virtual float MinDistToWall() {}

    virtual unsigned short GetLookbackAngle() {}

    virtual void ResetState() {}

    // ICollisionHandler
    bool OnWCollide(const WCollisionMgr::WorldCollisionInfo &cInfo, const UMath::Vector3 &cPoint, void *userdata) override;

    virtual void Enable();
    virtual void Disable();

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

#endif
