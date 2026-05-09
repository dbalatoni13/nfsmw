#ifndef CAMERA_CAMERAINFO_H
#define CAMERA_CAMERAINFO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

typedef enum {
    POV_BUMPER = 0,
    POV_HOOD = 1,
    POV_OUTSIDE_CLOSE = 2,
    POV_OUTSIDE_FAR = 3,
    POV_SUPER_FAR = 4,
    POV_DRIFT = 5,
    POV_PURSUIT = 6,
    NUM_POV_TYPES = 7
} POVTypes;

struct POV {
    short int Type;
    bAngle Angle;
    float Lag;
    float Height;
    float LatOffset;
    bAngle Fov;
    float Stiffness;
    short int AllowTilting;
    float pad4[2];
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
