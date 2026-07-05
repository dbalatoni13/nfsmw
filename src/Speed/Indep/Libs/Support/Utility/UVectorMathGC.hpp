#include "UVectorMath.h"

// Decl: Carbon: UVectorMathCPU.hpp: 94, GC MW: UVectorMathGC.hpp: 26, PS2 MW: UVectorMath.hpp: 354
inline void VU0_v3unitcrossprod(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &dest) {
    VU0_v3crossprod(a, b, dest);
    VU0_v3unit(dest, dest);
}

// Decl: Carbon: UVectorMathCPU.hpp: 293, GC MW: UVectorMathGC.hpp: 62, PS2 MW: UVectorMath.hpp: 1223
inline void VU0_v3negate(UMath::Vector3 &result) {
    result.x = -result.x;
    result.y = -result.y;
    result.z = -result.z;
}

// Decl: Carbon: UVectorMathCPU.hpp: 300, GC MW: UVectorMathGC.hpp: 69, PS2 MW: UVectorMath.hpp: 1243
inline void VU0_v4negatexyz(UMath::Vector4 &result) {
    result.x = -result.x;
    result.y = -result.y;
    result.z = -result.z;
}

// Decl: Carbon: UVectorMathCPU.hpp: 506, GC MW: UVectorMathGC.hpp: 92, PS2 MW: UVectorMath.hpp: 2061
inline void VU0_MATRIX4_transpose(const UMath::Matrix4 &m, UMath::Matrix4 &result) {
    if (&m == &result) {
        UMath::Matrix4 temp;
        int i;
        int j;
        for (i = 0; i < 4 * 4; ++i) {
            temp.GetElements()[i] = m.GetElements()[i];
        }
        for (i = 0; i < 4; ++i) {
            for (j = 0; j < 4; ++j) {
                result[i][j] = temp[j][i];
            }
        }
    } else {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result[i][j] = m[j][i];
            }
        }
    }
}

// Decl: Carbon: UVectorMathCPU.hpp: 530, GC MW: UVectorMathGC.hpp: 115, PS2 MW: UVectorMath.hpp: 2086
inline void VU0_qtranspose(const RQUAT &a, RQUAT &result) {
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = a.w;
}

// Decl: Carbon: UVectorMathCPU.hpp: 538, GC MW: UVectorMathGC.hpp: 125, PS2 MW: UVectorMath.hpp: 1136
inline void VU0_qmul(const RQUAT &b, const RQUAT &a, RQUAT &dest) {
    UMath::Vector4 result;
    result.x = a.y * b.z - a.z * b.y + a.w * b.x + a.x * b.w;
    result.y = a.z * b.x - a.x * b.z + a.w * b.y + a.y * b.w;
    result.z = a.x * b.y - a.y * b.x + a.w * b.z + a.z * b.w;
    result.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;

    dest = result;
}

// Decl: Carbon: UVectorMathCPU.hpp: 584, GC MW: UVectorMathGC.hpp: 169, PS2 MW: UVectorMath.hpp: 2188
inline void VU0_MATRIX4Init(UMath::Matrix4 &dest, const float xx, const float yy, const float zz) {
    dest[0][0] = xx;
    dest[1][1] = yy;
    dest[2][2] = zz;
    dest[3][3] = 1.0f;

    dest[3][2] = 0.0f;
    dest[3][1] = 0.0f;
    dest[3][0] = 0.0f;

    dest[2][3] = 0.0f;
    dest[2][1] = 0.0f;
    dest[2][0] = 0.0f;

    dest[1][3] = 0.0f;
    dest[1][2] = 0.0f;
    dest[1][0] = 0.0f;

    dest[0][3] = 0.0f;
    dest[0][2] = 0.0f;
    dest[0][1] = 0.0f;
}

// Decl: Carbon: UVectorMathCPU.hpp: 607, GC MW: UVectorMathGC.hpp: 193, PS2 MW: UVectorMath.hpp: 2234
inline void VU0_v4Copy(const UMath::Vector4 &a, UMath::Vector4 &b) {
    b = a;
}

// Decl: Carbon: UVectorMathCPU.hpp: 612, GC MW: UVectorMathGC.hpp: 198, PS2 MW: UVectorMath.hpp: 2268
inline void VU0_MATRIX4Copy(const UMath::Matrix4 &a, UMath::Matrix4 &b) {
    b = a;
}

// Decl: Carbon: UVectorMathCPU.hpp: 129, GC MW: UVectorMathGC.hpp: 220, PS2 MW: UVectorMath.hpp: 621
inline void VU0_v3unit(const UMath::Vector3 &a, UMath::Vector3 &result) {
    float rlen = VU0_rsqrt(VU0_v3lengthsquare(a));
    VU0_v3scale(a, rlen, result);
}

// Decl: Carbon: UVectorMathCPU.hpp: 139, GC MW: UVectorMathGC.hpp: 228, PS2 MW: UVectorMath.hpp: 1203
inline void VU0_v4unit(const UMath::Vector4 &a, UMath::Vector4 &result) {
    float rlen = VU0_rsqrt(VU0_v4lengthsquare(a));
    VU0_v4scale(a, rlen, result);
}

// Decl: Carbon: UVectorMathCPU.hpp: 146, GC MW: UVectorMathGC.hpp: 236, PS2 MW: UVectorMath.hpp: 677
inline void VU0_v4unitxyz(const UMath::Vector4 &a, UMath::Vector4 &result) {
    float rlen = VU0_rsqrt(VU0_v4lengthsquarexyz(a));
    VU0_v4scalexyz(a, rlen, result);
}

// Decl: Carbon: UVectorMathCPU.hpp: 324, GC MW: UVectorMathGC.hpp: 243, PS2 MW: UVectorMath.hpp: 1323
inline float VU0_v3distance(const UMath::Vector3 &p1, const UMath::Vector3 &p2) {
    UMath::Vector3 temp;
    VU0_v3sub(p1, p2, temp);
    return VU0_sqrt(VU0_v3lengthsquare(temp));
}

// Decl: Carbon: UVectorMathCPU.hpp: 333, GC MW: UVectorMathGC.hpp: 255, PS2 MW: UVectorMath.hpp: 842
inline float VU0_v4distancexyz(const UMath::Vector4 &p1, const UMath::Vector4 &p2) {
    UMath::Vector4 temp;
    VU0_v4subxyz(p1, p2, temp);
    return VU0_sqrt(VU0_v4lengthsquarexyz(temp));
}

// Decl: Carbon: UVectorMathCPU.hpp: 342, GC MW: UVectorMathGC.hpp: 263, PS2 MW: UVectorMath.hpp: 1437
inline float VU0_v3distancexz(const UMath::Vector3 &p1, const UMath::Vector3 &p2) {
    return VU0_sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.z - p1.z) * (p2.z - p1.z));
}

// Decl: Carbon: UVectorMathCPU.hpp: 354, GC MW: UVectorMathGC.hpp: 274, PS2 MW: UVectorMath.hpp: 1014
inline float VU0_v3length(const struct UMath::Vector3 &a) {
    return VU0_sqrt(VU0_v3lengthsquare(a));
}

// Decl: Carbon: UVectorMathCPU.hpp: 359, GC MW: UVectorMathGC.hpp: 279, PS2 MW: UVectorMath.hpp: 1571
inline float VU0_v3lengthxz(const UMath::Vector3 &a) {
    return VU0_sqrt(a.x * a.x + a.z * a.z);
}

// Decl: Carbon: UVectorMathCPU.hpp: 364, GC MW: UVectorMathGC.hpp: 284, PS2 MW: UVectorMath.hpp: TODO
inline float VU0_v4length(const UMath::Vector4 &a) {
    return VU0_sqrt(VU0_v4lengthsquare(a));
}

// Decl: Carbon: UVectorMathCPU.hpp: 374, GC MW: UVectorMathGC.hpp: 294, PS2 MW: UVectorMath.hpp: 1653
inline float VU0_v4lengthxyz(const UMath::Vector4 &a) {
    return VU0_sqrt(VU0_v4lengthsquarexyz(a));
}

// Decl: Carbon: UVectorMathCPU.hpp: 644, GC MW: UVectorMathGC.hpp: 311, PS2 MW: UVectorMath.hpp: 2466
inline void VU0_v3lerp(const UMath::Vector3 &v1, const UMath::Vector3 &v2, const float t, UMath::Vector3 &target) {
    target.x = v1.x + (v2.x - v1.x) * t;
    target.y = v1.y + (v2.y - v1.y) * t;
    target.z = v1.z + (v2.z - v1.z) * t;
}

// Decl: Carbon: UVectorMathCPU.cpp: 1137, GC MW: UVectorMathGC.hpp: 352, PS2 MW: UVectorMath.hpp: unknown
inline void VU0_ExtractXAxis3FromQuat(const RQUAT &quat, UMath::Vector3 &result) {
    const float scale = 2.0f;
    float yy = scale * (quat.y * quat.y);
    float zz = scale * (quat.z * quat.z);
    float xy = scale * (quat.x * quat.y);
    float xz = scale * (quat.x * quat.z);
    float yw = scale * (quat.y * quat.w);
    float zw = scale * (quat.z * quat.w);

    result.x = 1.0f - (yy + zz);
    result.y = xy + zw;
    result.z = xz - yw;
}

// Decl: Carbon: UVectorMathCPU.cpp: 1169, GC MW: UVectorMathGC.hpp: 386, PS2 MW: UVectorMath.hpp: unknown
inline void VU0_ExtractYAxis3FromQuat(const RQUAT &quat, UMath::Vector3 &result) {
    const float scale = 2.0f;
    float xx = scale * (quat.x * quat.x);
    float zz = scale * (quat.z * quat.z);
    float xy = scale * (quat.x * quat.y);
    float xw = scale * (quat.x * quat.w);
    float yz = scale * (quat.y * quat.z);
    float zw = scale * (quat.z * quat.w);

    result.x = xy - zw;
    result.y = 1.0f - (xx + zz);
    result.z = yz + xw;
}

// Decl: Carbon: UVectorMathCPU.cpp: 1202, GC MW: UVectorMathGC.hpp: 423, PS2 MW: UVectorMath.hpp: unknown
inline void VU0_ExtractZAxis3FromQuat(const RQUAT &quat, UMath::Vector3 &result) {
    const float scale = 2.0f;
    float xx = scale * (quat.x * quat.x);
    float yy = scale * (quat.y * quat.y);
    float xz = scale * (quat.x * quat.z);
    float xw = scale * (quat.x * quat.w);
    float yz = scale * (quat.y * quat.z);
    float yw = scale * (quat.y * quat.w);

    result.x = xz + yw;
    result.y = yz - xw;
    result.z = 1.0f - (xx + yy);
}

// Decl: Carbon: UVectorMathCPU.cpp: 1235, GC MW: UVectorMathGC.hpp: 472, PS2 MW: UVectorMath.hpp: unknown
inline void VU0_quattom4(const RQUAT &quat, UMath::Matrix4 &result) {
    const float scale = 2.0f;
    float xx = quat.x * quat.x * scale;
    float yy = quat.y * quat.y * scale;
    float zz = quat.z * quat.z * scale;

    float xy = quat.x * quat.y * scale;
    float xz = quat.x * quat.z * scale;
    float xw = quat.x * quat.w * scale;

    float yz = quat.y * quat.z * scale;
    float yw = quat.y * quat.w * scale;
    float zw = quat.z * quat.w * scale;

    result[0][0] = 1.0f - (yy + zz);
    result[0][1] = (xy + zw);
    result[0][2] = (xz - yw);
    result[0][3] = 0.0f;

    result[1][0] = (xy - zw);
    result[1][1] = 1.0f - (xx + zz);
    result[1][2] = (yz + xw);
    result[1][3] = 0.0f;

    result[2][0] = (xz + yw);
    result[2][1] = (yz - xw);
    result[2][2] = 1.0f - (xx + yy);
    result[2][3] = 0.0f;

    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;
}

// Decl: Carbon: UVectorMathCPU.cpp: 1313, GC MW: UVectorMathGC.hpp: 538, PS2 MW: UVectorMath.hpp: 1891
inline void VU0_MATRIX4_mult(const UMath::Matrix4 &m1, const UMath::Matrix4 &m2, UMath::Matrix4 &result) {
    UMath::Matrix4 temp;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp[i][j] = m1[i][0] * m2[0][j] + m1[i][1] * m2[1][j] + m1[i][2] * m2[2][j] + m1[i][3] * m2[3][j];
        }
    }
    result = temp;
}
