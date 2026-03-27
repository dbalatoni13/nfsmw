#include "Speed/Indep/Src/EAGL4Anim/eagl4runtimetransform.h"

#include "AnimUtil.h"

namespace EAGL4 {

static inline void EAGL4m3toquat(const UMath::Matrix3 *mat, UMath::Vector4 *result) {
    float s;
    float trace = (*mat)[0][0] + (*mat)[1][1] + (*mat)[2][2];

    if (trace > 0.0f) {
        s = EAGL4Anim::FastSqrt(trace + 1.0f);
        result->w = s * 0.5f;
        s = 0.5f / s;
        result->x = ((*mat)[1][2] - (*mat)[2][1]) * s;
        result->y = ((*mat)[2][0] - (*mat)[0][2]) * s;
        result->z = ((*mat)[0][1] - (*mat)[1][0]) * s;
    } else {
        unsigned long i = 0;

        if ((*mat)[1][1] > (*mat)[0][0]) {
            i = 1;
        }
        if ((*mat)[2][2] > (*mat)[i][i]) {
            i = 2;
        }

        if (i == 0) {
            s = EAGL4Anim::FastSqrt(((*mat)[0][0] - ((*mat)[1][1] + (*mat)[2][2])) + 1.0f);
            result->x = s * 0.5f;
            if (s != 0.0f) {
                s = 0.5f / s;
            }
            result->w = ((*mat)[1][2] - (*mat)[2][1]) * s;
            result->y = ((*mat)[0][1] + (*mat)[1][0]) * s;
            result->z = ((*mat)[0][2] + (*mat)[2][0]) * s;
        } else if (i == 1) {
            s = EAGL4Anim::FastSqrt(((*mat)[1][1] - ((*mat)[2][2] + (*mat)[0][0])) + 1.0f);
            result->y = s * 0.5f;
            if (s != 0.0f) {
                s = 0.5f / s;
            }
            result->w = ((*mat)[2][0] - (*mat)[0][2]) * s;
            result->x = ((*mat)[1][0] + (*mat)[0][1]) * s;
            result->z = ((*mat)[1][2] + (*mat)[2][1]) * s;
        } else {
            s = EAGL4Anim::FastSqrt(((*mat)[2][2] - ((*mat)[0][0] + (*mat)[1][1])) + 1.0f);
            result->z = s * 0.5f;
            if (s != 0.0f) {
                s = 0.5f / s;
            }
            result->w = ((*mat)[0][1] - (*mat)[1][0]) * s;
            result->x = ((*mat)[2][0] + (*mat)[0][2]) * s;
            result->y = ((*mat)[2][1] + (*mat)[1][2]) * s;
        }
    }
}

void MultMatrix(const UMath::Matrix4 *pm1, const UMath::Matrix4 *pm2, UMath::Matrix4 *presult) {
    const float *m1 = pm1->GetElements();
    const float *m2 = pm2->GetElements();
    float *result = presult->GetElements();

    result[0] = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12];
    result[1] = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13];
    result[2] = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14];
    result[3] = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15];
    result[4] = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12];
    result[5] = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13];
    result[6] = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14];
    result[7] = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15];
    result[8] = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12];
    result[9] = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13];
    result[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14];
    result[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15];
    result[12] = m1[12] * m2[0] + m1[13] * m2[4] + m1[14] * m2[8] + m1[15] * m2[12];
    result[13] = m1[12] * m2[1] + m1[13] * m2[5] + m1[14] * m2[9] + m1[15] * m2[13];
    result[14] = m1[12] * m2[2] + m1[13] * m2[6] + m1[14] * m2[10] + m1[15] * m2[14];
    result[15] = m1[12] * m2[3] + m1[13] * m2[7] + m1[14] * m2[11] + m1[15] * m2[15];
}

void Transform::PostMult(const Transform &second, Transform *pOutput) const {
    MultMatrix(&m, &second.m, &pOutput->m);
}

void Transform::PostMult(const Transform &second) {
    Transform result;

    MultMatrix(&m, &second.m, &result.m);
    m = result.m;
}

void Transform::ExtractQuatTrans(UMath::Vector4 *retQuat, UMath::Vector4 *retTrans) const {
    UMath::Matrix3 m3;

    m3.v0.x = m.v0.x;
    m3.v0.y = m.v0.y;
    m3.v0.z = m.v0.z;
    m3.v1.x = m.v1.x;
    m3.v1.y = m.v1.y;
    m3.v1.z = m.v1.z;
    m3.v2.x = m.v2.x;
    m3.v2.y = m.v2.y;
    m3.v2.z = m.v2.z;

    EAGL4m3toquat(&m3, retQuat);

    retTrans->x = m.v3.x;
    retTrans->y = m.v3.y;
    retTrans->z = m.v3.z;
    retTrans->w = m.v3.w;
}

void Transform::BuildSQT(float sx, float sy, float sz, float qx, float qy, float qz, float qw, float tx, float ty, float tz) {
    float s = 1.0f;
    float xs = qx + qx;
    float ys = qy + qy;
    float zs = qz + qz;
    float wx = qw * xs;
    float wy = qw * ys;
    float wz = qw * zs;
    float xx = qx * xs;
    float xy = qx * ys;
    float xz = qx * zs;
    float yy = qy * ys;
    float yz = qy * zs;
    float zz = qz * zs;

    m.v3.x = tx;
    m.v3.y = ty;
    m.v3.w = s;
    m.v2.w = 0.0f;
    m.v0.w = 0.0f;
    m.v1.w = 0.0f;
    m.v3.z = tz;
    m.v0.z = sx * (xz - wy);
    m.v1.z = sy * (yz + wx);
    m.v2.z = sz * (s - (xx + yy));
    m.v0.x = sx * (s - (yy + zz));
    m.v1.x = sy * (xy - wz);
    m.v2.x = sz * (xz + wy);
    m.v0.y = sx * (xy + wz);
    m.v1.y = sy * (s - (xx + zz));
    m.v2.y = sz * (yz - wx);
}

}; // namespace EAGL4
