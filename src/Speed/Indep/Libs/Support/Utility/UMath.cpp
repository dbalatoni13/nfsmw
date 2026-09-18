#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <cmath>
#if defined(EA_PLATFORM_XENON) || defined(EA_PLATFORM_WIN32)
#include <float.h> // _isnan
#endif

// El objetivo guarda 0x7FF00000 en .rodata bajo el nombre `_Infinity_` (local,
// sin manglar: esta FUERA del namespace), lo carga con `lfs` y lo almacena en
// UMath::Infinity. El front-end no puede plegar el type-pun, y por eso esta
// inicializacion es DINAMICA y aparece en el static-init.
static const unsigned int _Infinity_ = 0x7FF00000;

namespace UMath {

const float Infinity = *reinterpret_cast<const float *>(&_Infinity_);

float Ceil(const float x) {
    return bCeil(x);
}

// STRIPPED

float Mod(const float x, const float e) {
    return bFMod(x, e);
}

bool IsNaN(const float f) {
#if defined(EA_PLATFORM_XENON) || defined(EA_PLATFORM_WIN32)
    // La CRT de MSVC (Xenon y PC) no tiene isnanf.
    return _isnan(f) ? true : false;
#else
    return isnanf(f) ? true : false;
#endif
}

// STRIPPED
bool IsNaN(const UMath::Vector3 &v) {}
bool IsNaN(const UMath::Vector4 &v) {}
bool IsNaN(const UMath::Matrix3 &v) {}
bool IsNaN(const UMath::Matrix4 &v) {}
//...

// Estaticos de clase que el objetivo emite en el .rodata de zFoundation
// (zFoundation.s: 0x803EB374..0x803EB3F0, 116 B). Sin definicion quedaban
// UNDEF en nuestro .o y el enlazador les daba basura.
const Vector2 Vector2::kZero = {0.0f, 0.0f};
const Vector3 Vector3::kZero = {0.0f, 0.0f, 0.0f};
const Vector4 Vector4::kZero = {0.0f, 0.0f, 0.0f, 0.0f};
const Vector4 Vector4::kIdentity = {0.0f, 0.0f, 0.0f, 1.0f};

const Matrix4 Matrix4::kIdentity = {{1.0f, 0.0f, 0.0f, 0.0f},
                                    {0.0f, 1.0f, 0.0f, 0.0f},
                                    {0.0f, 0.0f, 1.0f, 0.0f},
                                    {0.0f, 0.0f, 0.0f, 1.0f}};

} // namespace UMath

void BuildRotate(UMath::Matrix4 &m, float r, float x, float y, float z) {
    float angle;
    float fSin;
    float fCos;
    float invllen;
    float fT;
    float fTX;
    float fTY;
    float fTZ;
    float fSX;
    float fSY;
    float fSZ;
    float *p;

    r *= 0.0027777778f;
    if (r == 0.0f) {
        VU0_MATRIX4Init(m);
        return;
    }

    angle = r * 6.2831855f;
    fSin = VU0_Sin(angle);
    fCos = VU0_Cos(angle);

    invllen = VU0_rsqrt(x * x + y * y + z * z);
    x *= invllen;
    y *= invllen;
    z *= invllen;

    fT = 1.0f - fCos;
    fTX = fT * x;
    fTY = fT * y;
    fTZ = fT * z;
    fSX = fSin * x;
    fSY = fSin * y;
    fSZ = fSin * z;

    p = &m[0][0];

    p[0] = fTX * x + fCos;
    p[1] = fTX * y + fSZ;
    p[2] = fTX * z - fSY;
    p[3] = 0.0f;
    p[4] = fTY * x - fSZ;
    p[5] = fTY * y + fCos;
    p[6] = fTY * z + fSX;
    p[7] = 0.0f;
    p[8] = fTZ * x + fSY;
    p[9] = fTZ * y - fSX;
    p[10] = fTZ * z + fCos;
    p[11] = 0.0f;
    p[12] = 0.0f;
    p[13] = 0.0f;
    p[14] = 0.0f;
    p[15] = 1.0f;
}

void OrthoInverse(UMath::Matrix4 &m) {
    float temp;
    temp = m[0][1];
    m[0][1] = m[1][0];
    m[1][0] = temp;
    temp = m[0][2];
    m[0][2] = m[2][0];
    m[2][0] = temp;
    temp = m[1][2];
    m[1][2] = m[2][1];
    m[2][1] = temp;
    float ntx = -m[3][0];
    float ty = m[3][1];
    float tz = m[3][2];
    m[3][0] = (ntx * m[0][0] - ty * m[1][0]) - tz * m[2][0];
    m[3][1] = (ntx * m[0][1] - ty * m[1][1]) - tz * m[2][1];
    m[3][2] = (ntx * m[0][2] - ty * m[1][2]) - tz * m[2][2];
}
