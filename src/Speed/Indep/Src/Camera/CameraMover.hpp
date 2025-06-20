#pragma once

#include "./Camera.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

class eView;

enum CameraMoverTypes {
    CM_SHOWCASE = 18,
    CM_ANIMATION_ENTITY = 17,
    CM_COP_VIEW = 16,
    CM_ANIMATION_CONTROLLER = 15,
    CM_ICE = 14,
    CM_AUTO_PILOT = 13,
    CM_ZONE_PREVIEW = 12,
    CM_ZONE_FREEZE = 11,
    CM_STILL = 10,
    CM_SELECT_CAR = 9,
    CM_MAX = 8,
    CM_TRACK_COP = 7,
    CM_TRACK_CAR = 6,
    CM_REAR_VIEW_MIRROR = 5,
    CM_ORBIT_CAR = 4,
    CM_ROAD_EDITOR = 3,
    CM_DEBUG_WORLD = 2,
    CM_DRIVE_CUBIC = 1,
    CM_NONE_TYPE = 0,
};

struct CameraMover : public bTNode<CameraMover>, public WCollisionMgr::ICollisionHandler {
    // total size: 0x80
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

    CameraMover();
};
