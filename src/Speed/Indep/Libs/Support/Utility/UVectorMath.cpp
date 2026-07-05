#include "UVectorMath.h"
#include "UMath.h"

#ifndef EA_PLATFORM_GAMECUBE
// Decl: Carbon: UVectorMath.cpp: 1137, GC MW: UVectorMathGC.hpp: 352, PS2 MW: UVectorMath.hpp: TODO
void VU0_ExtractXAxis3FromQuat(const RQUAT &quat, UMath::Vector3 &result) {
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

// Decl: Carbon: UVectorMath.cpp: 1169, GC MW: UVectorMathGC.hpp: 386, PS2 MW: UVectorMath.hpp: TODO
void VU0_ExtractYAxis3FromQuat(const RQUAT &quat, UMath::Vector3 &result) {
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

// Decl: Carbon: UVectorMath.cpp: 1202, GC MW: UVectorMathGC.hpp: 423, PS2 MW: UVectorMath.hpp: TODO
void VU0_ExtractZAxis3FromQuat(const RQUAT &quat, UMath::Vector3 &result) {
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

// Decl: Carbon: UVectorMath.cpp: 1235, GC MW: UVectorMathGC.hpp: 472, PS2 MW: UVectorMath.hpp: TODO
void VU0_quattom4(const RQUAT &quat, UMath::Matrix4 &result) {
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

// Decl: Carbon: UVectorMath.cpp: 1313, GC MW: UVectorMathGC.hpp: 538, PS2 MW: UVectorMath.hpp: 1891
void VU0_MATRIX4_mult(const UMath::Matrix4 &m1, const UMath::Matrix4 &m2, UMath::Matrix4 &result) {
    UMath::Matrix4 temp;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp[i][j] = m1[i][0] * m2[0][j] + m1[i][1] * m2[1][j] + m1[i][2] * m2[2][j] + m1[i][3] * m2[3][j];
        }
    }
    result = temp;
}
#endif

// Decl: Carbon: UVectorMath.cpp: 4934, GC MW: UVectorMath.cpp: 4935, PS2 MW: UVectorMath.cpp: 4936
float VU0_Atan2(const float opposite, const float adjacent) {
    float x = UMath::Epsilon < VU0_fabs(adjacent) ? adjacent : 0.0f;
    float y = UMath::Epsilon < VU0_fabs(opposite) ? opposite : 0.0f;

    short bangle = bATan(x, y);
    return bangle / 65536.0f;
}
