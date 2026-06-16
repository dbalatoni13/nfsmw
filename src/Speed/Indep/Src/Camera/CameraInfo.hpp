#ifndef CAMERAINFO_HPP_
#define CAMERAINFO_HPP_

#include "Speed/Indep/bWare/Inc/bMath.hpp"

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

#endif
