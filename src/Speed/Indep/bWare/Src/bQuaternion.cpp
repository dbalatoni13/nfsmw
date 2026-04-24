#include "Speed/Indep/bWare/Inc/bMath.hpp"

bQuaternion bIdentityQuaternion(0.0f, 0.0f, 0.0f, 1.0f);

bQuaternion &bQuaternion::Slerp(bQuaternion &r, const bQuaternion &target, float t) const {
    float cos_theta = bDot(reinterpret_cast<const bVector4 *>(this), reinterpret_cast<const bVector4 *>(&target));
    float scale1;
    float scale2;

    if ((1.0f - bAbs(cos_theta)) > 0.0001f) {
        unsigned short theta = bACos(bAbs(cos_theta));
        float sin_theta = bSin(theta);
        unsigned short a1 = static_cast<unsigned short>(static_cast<int>((1.0f - t) * static_cast<float>(theta)) & 0xffff);
        unsigned short a2 = static_cast<unsigned short>(static_cast<int>(t * static_cast<float>(theta)) & 0xffff);

        scale1 = bSin(a1) / sin_theta;
        scale2 = bSin(a2) / sin_theta;
    } else {
        scale1 = 1.0f - t;
        scale2 = t;
    }

    if (cos_theta < 0.0f) {
        scale1 = -scale1;
    }

    bQuaternion qtemp;
    bScale(reinterpret_cast<bVector4 *>(&qtemp), reinterpret_cast<const bVector4 *>(this), scale1);
    bScaleAdd(reinterpret_cast<bVector4 *>(&qtemp), reinterpret_cast<const bVector4 *>(&qtemp), reinterpret_cast<const bVector4 *>(&target), scale2);
    r = qtemp;
    return r;
}

void bMatrixToQuaternion(bQuaternion &quat, const bMatrix4 &m) {
    float tr = m[0][0] + m[1][1] + m[2][2];

    if (tr > 0.0f) {
        float t = bSqrt(tr + 1.0f);
        quat.w = t * 0.5f;
        t = 0.5f / t;
        quat.x = (m[1][2] - m[2][1]) * t;
        quat.y = (m[2][0] - m[0][2]) * t;
        quat.z = (m[0][1] - m[1][0]) * t;
    } else {
        int i = 0;
        if (m[1][1] > m[0][0]) {
            i = 1;
        }
        if (m[2][2] > m[i][i]) {
            i = 2;
        }
        int j = (i + 1) % 3;
        int k = (j + 1) % 3;
        float t = bSqrt((m[i][i] - m[j][j] - m[k][k]) + 1.0f);
        float q[4];
        q[i] = t * 0.5f;
        if (t != 0.0f) {
            t = 0.5f / t;
        }
        q[3] = (m[j][k] - m[k][j]) * t;
        q[j] = (m[i][j] + m[j][i]) * t;
        q[k] = (m[i][k] + m[k][i]) * t;
        quat.x = q[0];
        quat.y = q[1];
        quat.z = q[2];
        quat.w = q[3];
    }
}

// STRIPPED
bQuaternion *bConjugate(bQuaternion *qdest, const bQuaternion *q) {}

// STRIPPED
float bLength(bQuaternion *q) {}

// STRIPPED
bQuaternion *bNormalize(bQuaternion *qdest, const bQuaternion *q) {}

// STRIPPED
bQuaternion *bMult(bQuaternion *qdest, const bQuaternion *q1, const bQuaternion *q2) {}
