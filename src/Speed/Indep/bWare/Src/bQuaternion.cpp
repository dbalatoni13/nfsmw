#include "Speed/Indep/bWare/Inc/bMath.hpp"

bQuaternion bIdentityQuaternion(0.0f, 0.0f, 0.0f, 1.0f);

bQuaternion &bQuaternion::Slerp(bQuaternion &r, const bQuaternion &target, float t) const {
    float cos_theta = bDot(reinterpret_cast<const bVector4 *>(this), reinterpret_cast<const bVector4 *>(&target));
    float scale1;
    float scale2;

    if ((1.0f - bAbs(cos_theta)) > 0.05f) {
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

bQuaternion *bConjugate(bQuaternion *qdest, const bQuaternion *q) {
    float x = q->x;
    float y = q->y;
    float z = q->z;
    float w = q->w;
    qdest->x = -x;
    qdest->y = -y;
    qdest->z = -z;
    qdest->w = w;
    return qdest;
}

float bLength(bQuaternion *q) {
    return bSqrt(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);
}

bQuaternion *bNormalize(bQuaternion *qdest, const bQuaternion *q) {
    float len = bLength(const_cast<bQuaternion *>(q));
    if (len != 0.0f) {
        float inv_len = 1.0f / len;
        qdest->x = q->x * inv_len;
        qdest->y = q->y * inv_len;
        qdest->z = q->z * inv_len;
        qdest->w = q->w * inv_len;
    } else {
        qdest->x = 0.0f;
        qdest->y = 0.0f;
        qdest->z = 0.0f;
        qdest->w = 1.0f;
    }
    return qdest;
}

bQuaternion *bMult(bQuaternion *qdest, const bQuaternion *q1, const bQuaternion *q2) {
    float w2;
    float w1;
    w1 = q1->w;
    w2 = q2->w;
    float w = w1 * w2 - (q1->x * q2->x + q1->y * q2->y + q1->z * q2->z);
    bVector3 cross;
    cross.x = q1->y * q2->z - q1->z * q2->y;
    cross.y = q1->z * q2->x - q1->x * q2->z;
    cross.z = q1->x * q2->y - q1->y * q2->x;
    cross.x += w1 * q2->x;
    cross.y += w1 * q2->y;
    cross.z += w1 * q2->z;
    cross.y += w2 * q1->y;
    cross.z += w2 * q1->z;
    cross.x += w2 * q1->x;
    qdest->x = cross.x;
    qdest->y = cross.y;
    qdest->z = cross.z;
    qdest->w = w;
    return qdest;
}
