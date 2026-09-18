#include "eagl4runtimetransform.h"

#include <cmath>

namespace EAGL4 {

static inline void EAGL4m3toquat(const UMath::Matrix3 *mat, UMath::Vector4 *result) {
    unsigned long i;
    float s;

    s = mat->v0.x + mat->v1.y + mat->v2.z;

    if (s > 0.0f) {

        s = sqrtf(s + 1.0f);
        result->w = s * 0.5f;
        s = 0.5f / s;
        result->x = (mat->v1.z - mat->v2.y) * s;
        result->y = (mat->v2.x - mat->v0.z) * s;
        result->z = (mat->v0.y - mat->v1.x) * s;
    } else {

        // i solo se prueba por verdad; el original guarda el bit de CR en su sitio (valor 4)
        i = 0;
        if (mat->v1.y > mat->v0.x) {
            i = 4;
        }

        if (mat->v2.z > (i == 0 ? mat->v0.x : mat->v1.y)) {

            s = sqrtf(mat->v2.z - (mat->v0.x + mat->v1.y) + 1.0f);
            result->z = s * 0.5f;
            if (s != 0.0f) {
                s = 0.5f / s;
            }
            result->w = (mat->v0.y - mat->v1.x) * s;
            result->x = (mat->v2.x + mat->v0.z) * s;
            result->y = (mat->v2.y + mat->v1.z) * s;
        } else if (i) {

            s = sqrtf(mat->v1.y - (mat->v2.z + mat->v0.x) + 1.0f);
            result->y = s * 0.5f;
            if (s != 0.0f) {
                s = 0.5f / s;
            }
            result->w = (mat->v2.x - mat->v0.z) * s;
            result->z = (mat->v1.z + mat->v2.y) * s;
            result->x = (mat->v1.x + mat->v0.y) * s;
        } else {

            s = sqrtf(mat->v0.x - (mat->v1.y + mat->v2.z) + 1.0f);
            result->x = s * 0.5f;
            if (s != 0.0f) {
                s = 0.5f / s;
            }
            result->w = (mat->v1.z - mat->v2.y) * s;
            result->y = (mat->v0.y + mat->v1.x) * s;
            result->z = (mat->v0.z + mat->v2.x) * s;
        }
    }
}

void Transform::PostMult(const Transform &second) {
    Transform t;

    MultMatrix(&m, &second.m, &t.m);
    *this = t;
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

void Transform::BuildSQT(float sx, float sy, float sz, float qx, float qy, float qz, float qw, float tx, float ty,
                         float tz) {
    float s = 2.0f;
    float xs = qx * s, ys = qy * s, zs = qz * s;
    float wx = qw * xs, wy = qw * ys, wz = qw * zs;
    float xx = qx * xs, xy = qx * ys, xz = qx * zs;
    float yy = qy * ys, yz = qy * zs, zz = qz * zs;

    m.v0.x = sx * (1.0f - (yy + zz));
    m.v1.x = sy * (xy - wz);
    m.v2.x = sz * (xz + wy);
    m.v0.y = sx * (xy + wz);
    m.v1.y = sy * (1.0f - (xx + zz));
    m.v2.y = sz * (yz - wx);
    m.v0.z = sx * (xz - wy);
    m.v1.z = sy * (yz + wx);
    m.v2.z = sz * (1.0f - (xx + yy));
    m.v0.w = 0.0f;
    m.v1.w = 0.0f;
    m.v2.w = 0.0f;
    m.v3.x = tx;
    m.v3.y = ty;
    m.v3.z = tz;
    m.v3.w = 1.0f;
}

void MultMatrix(const UMath::Matrix4 *a, const UMath::Matrix4 *b, UMath::Matrix4 *result) {
    result->v0.x = a->v0.x * b->v0.x + a->v0.y * b->v1.x + a->v0.z * b->v2.x + a->v0.w * b->v3.x;
    result->v0.y = a->v0.x * b->v0.y + a->v0.y * b->v1.y + a->v0.z * b->v2.y + a->v0.w * b->v3.y;
    result->v0.z = a->v0.x * b->v0.z + a->v0.y * b->v1.z + a->v0.z * b->v2.z + a->v0.w * b->v3.z;
    result->v0.w = a->v0.x * b->v0.w + a->v0.y * b->v1.w + a->v0.z * b->v2.w + a->v0.w * b->v3.w;

    result->v1.x = a->v1.x * b->v0.x + a->v1.y * b->v1.x + a->v1.z * b->v2.x + a->v1.w * b->v3.x;
    result->v1.y = a->v1.x * b->v0.y + a->v1.y * b->v1.y + a->v1.z * b->v2.y + a->v1.w * b->v3.y;
    result->v1.z = a->v1.x * b->v0.z + a->v1.y * b->v1.z + a->v1.z * b->v2.z + a->v1.w * b->v3.z;
    result->v1.w = a->v1.x * b->v0.w + a->v1.y * b->v1.w + a->v1.z * b->v2.w + a->v1.w * b->v3.w;

    result->v2.x = a->v2.x * b->v0.x + a->v2.y * b->v1.x + a->v2.z * b->v2.x + a->v2.w * b->v3.x;
    result->v2.y = a->v2.x * b->v0.y + a->v2.y * b->v1.y + a->v2.z * b->v2.y + a->v2.w * b->v3.y;
    result->v2.z = a->v2.x * b->v0.z + a->v2.y * b->v1.z + a->v2.z * b->v2.z + a->v2.w * b->v3.z;
    result->v2.w = a->v2.x * b->v0.w + a->v2.y * b->v1.w + a->v2.z * b->v2.w + a->v2.w * b->v3.w;

    result->v3.x = a->v3.x * b->v0.x + a->v3.y * b->v1.x + a->v3.z * b->v2.x + a->v3.w * b->v3.x;
    result->v3.y = a->v3.x * b->v0.y + a->v3.y * b->v1.y + a->v3.z * b->v2.y + a->v3.w * b->v3.y;
    result->v3.z = a->v3.x * b->v0.z + a->v3.y * b->v1.z + a->v3.z * b->v2.z + a->v3.w * b->v3.z;
    result->v3.w = a->v3.x * b->v0.w + a->v3.y * b->v1.w + a->v3.z * b->v2.w + a->v3.w * b->v3.w;
}

}; // namespace EAGL4
