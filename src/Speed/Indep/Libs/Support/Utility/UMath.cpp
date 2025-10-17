#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <cmath>

namespace UMath {



float Ceil(const float x) {
    return bCeil(x);
}

// STRIPPED
float Floor(const float x) {
    return bFloor(x);
}

float Mod(const float x, const float e) {
    return bFMod(x, e);
}

bool IsNaN(const float f) {
    return isnanf(f) ? true : false;
}

// STRIPPED
bool IsNaN(const UMath::Vector3 &v) {}
bool IsNaN(const UMath::Vector4 &v) {}
bool IsNaN(const UMath::Matrix3 &v) {}
bool IsNaN(const UMath::Matrix4 &v) {}
//...

}

// UNSOLVED
void BuildRotate(UMath::Matrix4 &m, float r, float x, float y, float z) {
    r *= 0.0027777778f;
    if (r == 0.0f) {
        VU0_MATRIX4Init(m, 1.0f, 1.0f, 1.0f);
    } else {
        float angle = r * 6.2831855f; // 2pi
        float fSin = VU0_Sin(angle);
        float fCos = VU0_Cos(angle);

        float invllen = VU0_rsqrt(x * x + y * y + z * z);
        x *= invllen;
        y *= invllen;
        z *= invllen;

        float fT = 1.0f;
        float fTX = fT * x; // f9
        float fTY = fT * y; // f0
        float fTZ = fT * z; // f12
        float fSX = fSin * x; // f11
        float fSY = fSin * y; // f30
        float fSZ = fSin * z; // f8
        
        m.v0.x = fTX * x + fCos; // 0
        m.v0.y = fTX * y + fSZ;  // 1
        m.v0.z = fTX * z - fSY;  // 2
        m.v1.x = fTY * x - fSZ;  // 3
        m.v1.y = fTY * y + fCos; // 4
        m.v1.z = fTY * z + fSX;  // 5
        m.v2.x = fTZ * x + fSin; // 6
        m.v2.y = fTZ * y - fSX;  // 7
        m.v2.z = fTZ * z + fCos; // 8

        m.v3.x = 0.0f;
        m.v3.y = 0.0f;
        m.v3.z = 0.0f;

        m.v0.w = 0.0f;
        m.v1.w = 0.0f;
        m.v2.w = 0.0f;
        m.v3.w = fT;
        
    }
}
