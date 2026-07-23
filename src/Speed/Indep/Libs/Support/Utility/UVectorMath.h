//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

#ifndef __VU0MATH_H
#define __VU0MATH_H 1

#include "UTypes.h"
#include <cmath>

#define ABS(x) ((x) < 0 ? -(x) : (x))

//

typedef UMath::Vector2 COORD2; // Decl: Carbon: 51
typedef UMath::Vector3 COORD3;
typedef UMath::Vector4 COORD4;
typedef UMath::Matrix4 MATRIX4;
typedef UMath::Matrix3 MATRIX3;
typedef UMath::Vector4 RQUAT; // Decl: Carbon: 56
// TODO
// typedef Quaternion RQUAT; // Decl: Carbon: 56

//
// static const float INVTWOPI; // Decl: Carbon: 61

// Decl: 109
inline int REAL2INT(float f) {}
inline int FLOAT2INT(float f) {
    return static_cast<int>(f);
}

inline int FloatAsInt(const float &f) {}

#define XBOX_USE_CPU 0     // Decl: 113
#define GAMECUBE_USE_CPU 0 // Decl: 114

#define UMATH_VECTOR_ASSERT bFAssert // Decl: 144

#define rABS(x) ((x) < 0 ? -(x) : (x)) // Decl: 150

// Decl: Carbon: 164
inline float IntAsFloat(const int &i) {
    return *reinterpret_cast<const float *>(&i);
}

inline int FloatSignBit(const float &f) {}

inline float FloatAbs(const float &f) {}

static const int kFloatOneAsInt = 0x3F800000;              // Decl: Carbon: 166
static const float kFloatScaleUp = IntAsFloat(0x00800000); // Decl: Carbon: 167
static const float kFloatScaleDown = 1.0f / kFloatScaleUp; // Decl: Carbon: 168

// Decl: Carbon: 191
inline const UMath::Vector3 &Coord4To3(const UMath::Vector4 &c4) {
    return reinterpret_cast<const UMath::Vector3 &>(c4);
}

inline UMath::Vector3 &Coord4To3(UMath::Vector4 &c4) {
    return reinterpret_cast<UMath::Vector3 &>(c4);
}

inline const UMath::Vector3 *Coord4To3(const UMath::Vector4 *c4) {
    return reinterpret_cast<const UMath::Vector3 *>(c4);
}
// Decl: Carbon: 197
inline UMath::Vector3 *Coord4To3(UMath::Vector4 *c4) {
    return reinterpret_cast<UMath::Vector3 *>(c4);
}

inline UMath::Vector2 Coord2(float x, float y) {
    UMath::Vector2 c;
}

inline UMath::Vector3 Coord3(float x, float y, float z) {
    UMath::Vector3 c;
}

inline struct UMath::Vector4 Coord4(float x, float y, float z, float w) {
    UMath::Vector4 c;
}

inline UMath::Vector4 Coord4(const UMath::Vector3 &c, float w) {
    UMath::Vector4 res;
}

inline UMath::Vector4 Coord4Position(const UMath::Vector3 &c) {
    UMath::Vector4 res;
}

inline struct UMath::Vector4 Coord4Vector(const UMath::Vector3 &c) {
    UMath::Vector4 res;
}

inline struct UMath::Vector4 RQuat(float x, float y, float z, float w) {
    UMath::Vector4 q;
}

inline void Coord3AsString(const UMath::Vector3 &c, char *str) {}

inline void Coord4AsString(const UMath::Vector4 &c, char *str) {}

inline float V3LengthSquared(const UMath::Vector4 &p) {}

// Decl: Carbon: 269
inline float PTDir(const UMath::Vector4 &vert, const UMath::Vector4 &p0, const UMath::Vector4 &p1) {}

inline float PTOrd(const UMath::Vector4 &vert, const UMath::Vector4 &p0, const UMath::Vector4 &p1) {}

#define kZeroCoord3 UMath::Vector3::kZero          // Decl: 292
#define kZeroCoord4 UMath::Vector4::kIdentity      // Decl: 293
#define kAllZeroCoord4 UMath::Vector4::kZero       // Decl: 294
#define kIdentityMatrix4 UMath::Matrix4::kIdentity // Decl: 295
#define kIdentityMatrix3 UMath::Matrix3::kIdentity // Decl: 296

// Decl: Carbon: 304, GC MW: 304
// total size: 0x40
class UTransform {
  public:
    UTransform() {}

    UTransform(const UMath::Matrix4 &m) : fTransform(m) {}

    // UTransform(const UMath::Vector3 &t) {}

    // UTransform(const RQUAT &q) {}

    // UTransform(const UMath::Matrix3 &m, const UMath::Vector3 &t) {}

    // UTransform(const RQUAT &q, const UMath::Vector4 &t) {}

    ~UTransform() {}

    // RQUAT Quaternion() const {}

    // void Orientation(UMath::Matrix3 &result) const {}

    // const UTransform &operator=(const UTransform &src) {}

    // UMath::Vector4 Apply(const UMath::Vector4 &input) const {}

    // void Apply(const UMath::Vector4 &input, UMath::Vector4 &output) const {}

    // void Apply(unsigned int count, const UMath::Vector4 *input, UMath::Vector4 *output) const {}

    // static const UTransform &Identity() {}

    UMath::Matrix4 fTransform; // offset 0x0, size 0x40

  private:
    static const UTransform fgIdentityTransform; // size: 0x40, address: 0x80473E24
};

// Decl: Carbon: 346, GC MW: 426
inline float V3DistanceSquared(const UMath::Vector3 &a, const UMath::Vector3 &b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;

    return dx * dx + dy * dy + dz * dz;
}

// Decl: Carbon: 357, GC MW: 440
inline float PTDir(const UMath::Vector3 &vert, const UMath::Vector3 &p0, const UMath::Vector3 &p1) {
    float x0 = vert.x - p0.x;
    float z0 = vert.z - p0.z;
    float x1 = p1.x - p0.x;
    float z1 = p1.z - p0.z;
    return x1 * z0 - x0 * z1;
}

// Decl: Carbon: 391
inline bool PtInTri(const UMath::Vector4 &pt, const UMath::Vector4 &p0, const UMath::Vector4 &p1, const UMath::Vector4 &p2) {}

// Decl: Carbon: 404
inline bool PtInQuad(const UMath::Vector4 &pt, const UMath::Vector4 &p0, const UMath::Vector4 &p1, const UMath::Vector4 &p2,
                     const UMath::Vector4 &p3) {}

inline bool PtInPenta(const UMath::Vector4 &pt, const UMath::Vector4 &p0, const UMath::Vector4 &p1, const UMath::Vector4 &p2,
                      const UMath::Vector4 &p3, const UMath::Vector4 &p4) {}

static const float MATH_PI = 3.141592653589793f; // Decl: Carbon: 596

#define VU0_BUSY_CHECK() (void)0 // Decl: 589

#define PS2_FLOAT_ONE_AS_INT 0x3f800000       // Decl: 598
#define PS2_FLOAT_MINUS_ONE_AS_INT 0xbf800000 // Decl: 599
#define QUAD_ADDRESS_MASK 0xf                 // Decl: 600
#define LONGWORD_ADDRESS_MASK 0x7             // Decl: 601
#define WORD_ADDRESS_MASK 0x3                 // Decl: 602
#define SQR(_a) ((_a) * (_a))                 // Decl: 603

// Decl: 609
#define CLEAR_COORD3(_a)                                                                                                                             \
    {                                                                                                                                                \
        *((int *)&_a.x) = 0;                                                                                                                         \
        *((int *)&_a.y) = 0;                                                                                                                         \
        *((int *)&_a.z) = 0;                                                                                                                         \
    };
// Decl: 610
#define CLEAR_COORD4(_a)                                                                                                                             \
    {                                                                                                                                                \
        *((int *)&_a.x) = 0;                                                                                                                         \
        *((int *)&_a.y) = 0;                                                                                                                         \
        *((int *)&_a.z) = 0;                                                                                                                         \
        *((int *)&_a.w) = 0;                                                                                                                         \
    };
// Decl: 613
#define SET_MATRIX4_TRANSLATION(_a, _b)                                                                                                              \
    {                                                                                                                                                \
        _a.m44[3][0] = _b.x;                                                                                                                         \
        _a.m44[3][1] = _b.y;                                                                                                                         \
        _a.m44[3][2] = _b.z;                                                                                                                         \
        *((int *)&_a.m44[3][3]) = PS2_FLOAT_ONE_AS_INT;                                                                                              \
    };
// Decl: 617
#define SET_MATRIX4PTR_TRANSLATION(_a, _b)                                                                                                           \
    {                                                                                                                                                \
        _a->m44[3][0] = _b.x;                                                                                                                        \
        _a->m44[3][1] = _b.y;                                                                                                                        \
        _a->m44[3][2] = _b.z;                                                                                                                        \
        *((int *)&_a->m44[3][3]) = PS2_FLOAT_ONE_AS_INT;                                                                                             \
    };
// Decl: 621
#define GET_MATRIX4_TRANSLATION(_a) reinterpret_cast<const UMath::Vector4 &>(_a.m44[3])

// TODO sort these

float VU0_Sin(float x);
float VU0_Cos(float x);

float VU0_sqrt(const float a);
float VU0_Atan2(const float opposite, const float adjacent);
float VU0_rsqrt(const float a);

void VU0_v3add(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result);
void VU0_v3scale(const UMath::Vector3 &a, const float scaleby, UMath::Vector3 &result);
void VU0_v3scale(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result);
void VU0_v3scaleadd(const UMath::Vector3 &a, const float scaleby, const UMath::Vector3 &b, UMath::Vector3 &result);
void VU0_v3addscale(const UMath::Vector3 &a, const UMath::Vector3 &b, const float scaleby, UMath::Vector3 &result);
void VU0_v3sub(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result);
float VU0_v3dotprod(const UMath::Vector3 &a, const UMath::Vector3 &b);
float VU0_v3distancesquare(const UMath::Vector3 &p1, const UMath::Vector3 &p2);
float VU0_v3distancesquarexz(const UMath::Vector3 &p1, const UMath::Vector3 &p2);
void VU0_v3crossprod(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &dest);
float VU0_v3lengthsquare(const UMath::Vector3 &a);
void VU0_v3unit(const UMath::Vector3 &a, UMath::Vector3 &result);

void VU0_v4scaleadd(const UMath::Vector4 &a, const float scaleby, const UMath::Vector4 &b, UMath::Vector4 &result);
void VU0_v4scaleaddxyz(const UMath::Vector4 &a, const float scaleby, const UMath::Vector4 &b, UMath::Vector4 &result);
float VU0_v4lengthsquare(const UMath::Vector4 &a);
float VU0_v4lengthsquarexyz(const UMath::Vector4 &a);
void VU0_v4subxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result);
float VU0_v4dotprodxyz(const UMath::Vector4 &a, const UMath::Vector4 &b);
void VU0_v4scale(const UMath::Vector4 &a, const float scaleby, UMath::Vector4 &result);
void VU0_v4scalexyz(const UMath::Vector4 &a, const float scaleby, UMath::Vector4 &result);
void VU0_v4scalexyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result);
void VU0_v4add(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result);
float VU0_v4distancesquarexyz(const UMath::Vector4 &p1, const UMath::Vector4 &p2);
void VU0_v4addxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result);
void VU0_v4crossprodxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &dest);
void VU0_MATRIX3x4_vect3mult(const UMath::Vector3 &v, const UMath::Matrix4 &m, UMath::Vector3 &result);
void VU0_MATRIX3x4_vect4mult(const UMath::Vector4 &v, const UMath::Matrix4 &m, UMath::Vector4 &result);
void VU0_qmul(const UMath::Vector4 &b, const UMath::Vector4 &a, UMath::Vector4 &dest);

void VU0_v3quatrotate(const UMath::Vector4 &q, const UMath::Vector3 &v, UMath::Vector3 &result);

void VU0_m4toquat(const UMath::Matrix4 &mat, UMath::Vector4 &result);
void VU0_MATRIX4_vect3mult(const UMath::Vector3 &v, const UMath::Matrix4 &m, UMath::Vector3 &result);
void VU0_MATRIX4_vect4mult(const UMath::Vector4 &v, const UMath::Matrix4 &m, UMath::Vector4 &result);
void VU0_MATRIX4_vect4mult(const UMath::Vector4 *v, const UMath::Matrix4 &m, UMath::Vector4 *result, int count);
void VU0_MATRIX4setyrot(UMath::Matrix4 &dest, const float yangle);
void VU0_Matrix4ToEuler(const UMath::Matrix4 &m, UMath::Vector3 &e);

void VU0_ExtractXAxis3FromQuat(const RQUAT &quat, UMath::Vector3 &result);
void VU0_ExtractYAxis3FromQuat(const RQUAT &quat, UMath::Vector3 &result);
void VU0_ExtractZAxis3FromQuat(const RQUAT &quat, UMath::Vector3 &result);
void VU0_quattom4(const RQUAT &quat, UMath::Matrix4 &result);

void VU0_MATRIX4_mult(const UMath::Matrix4 &m1, const UMath::Matrix4 &m2, UMath::Matrix4 &result);

// Decl: Carbon: 789, GC MW: 868
inline float VU0_ASin(float x) {
    return asinf(x) / (float)M_TWOPI;
}

// Decl: Carbon: 800, GC MW: 878
inline float VU0_Pow(float x, float e) {
    return powf(x, e);
}

// Decl: Carbon: 802, GC MW: 882
inline void VU0_MATRIX3x4dotprod(const UMath::Vector3 &a, const UMath::Matrix4 &b, UMath::Vector3 &r) {
    r.x = VU0_v3dotprod(a, UMath::Vector4To3(b.v0));
    r.y = VU0_v3dotprod(a, UMath::Vector4To3(b.v1));
    r.z = VU0_v3dotprod(a, UMath::Vector4To3(b.v2));
}

// Decl: Carbon: 811, GC MW: ~889
inline float VU0_Sin(float x) {
    return sinf(x);
}
// Decl: Carbon: 812, GC MW: ~889
inline float VU0_Cos(float x) {
    return cosf(x);
}

#define VU0_prefetch(_a) ; // Decl: 961

// Decl: Carbon: 963
inline int VU0_FloatToInt(const float floatvalue) {}

// these might not use inline asm
// Decl: Carbon: 968, GC MW: 1048, PS2 MW: 1003
inline float VU0_floatmin(const float a, const float b) {
#ifdef EA_PLATFORM_PLAYSTATION2
    float result;
    asm __volatile__("min.s %0, %1, %2" : "=f"(result) : "f"(a), "f"(b));
    return result;
#else
    if (a < b)
        return a;
    else
        return b;
#endif
}

// Decl: Carbon: 976, GC MW: 1056
inline float VU0_floatmax(const float a, const float b) {
#ifdef EA_PLATFORM_PLAYSTATION2
    float result;
    asm __volatile__("max.s %0, %1, %2" : "=f"(result) : "f"(a), "f"(b));
    return result;
#else
    if (a > b)
        return a;
    else
        return b;
#endif
}

// Decl: Carbon: 1019, GC MW: 1102, PS2 MW: 1014
inline float VU0_fabs(const float a) {
#ifdef EA_PLATFORM_PLAYSTATION2
    float result;
    asm __volatile__("abs.s %0, %1" : "=f"(result) : "f"(a));
    return result;
#else
    if (a < 0.0f) {
        return -a;
    }
    return a;
        // return a < 0.0f ? -a : a;
#endif
}

#define VU0MATH_INLINE inline // Decl: 1044

#endif
