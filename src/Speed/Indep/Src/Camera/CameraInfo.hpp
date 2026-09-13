#ifndef CAMERA_CAMERAINFO_H
#define CAMERA_CAMERAINFO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

enum POVTypes {
    POV_BUMPER = 0,
    POV_HOOD = 1,
    POV_OUTSIDE_CLOSE = 2,
    POV_OUTSIDE_FAR = 3,
    POV_SUPER_FAR = 4,
    POV_DRIFT = 5,
    POV_PURSUIT = 6,
    NUM_POV_TYPES = 7,
};

struct POV {
    short Type;         // offset 0x0, size 0x2
    bAngle Angle;       // offset 0x2, size 0x2
    float Lag;          // offset 0x4, size 0x4
    float Height;       // offset 0x8, size 0x4
    float LatOffset;    // offset 0xC, size 0x4
    bAngle Fov;         // offset 0x10, size 0x2
    float Stiffness;    // offset 0x14, size 0x4
    short AllowTilting; // offset 0x18, size 0x2
    float pad4[2];      // offset 0x1C, size 0x8
};

class CameraInfo {
  public:
    int NumFullScreenPOVs;
    int NumSplitScreenPOVs;
    int Padding[2];
    POV FullScreenPOVArray[7];
    POV SplitScreenPOVArray[7];

    POV *GetPov(int nPovType);
};

#endif
