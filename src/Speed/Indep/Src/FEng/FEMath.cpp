#include "Speed/Indep/Src/FEng/FEMath.h"

void FEQuaternion::GetMatrix(FEMatrix4 *rotmat) {
    float *rm = reinterpret_cast<float *>(rotmat);

    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;

    float xx = x * x2;
    float xy = x * y2;
    float xz = x * z2;
    float yy = y * y2;
    float yz = y * z2;
    float zz = z * z2;
    float wx = w * x2;
    float wy = w * y2;
    float wz = w * z2;

    rotmat->m11 = 1.0f - (yy + zz);
    rotmat->m12 = xy + wz;
    rotmat->m13 = xz - wy;
    rotmat->m14 = 0.0f;
    rotmat->m21 = xy - wz;
    rotmat->m22 = 1.0f - (xx + zz);
    rotmat->m23 = yz + wx;
    rotmat->m24 = 0.0f;
    rotmat->m31 = xz + wy;
    rotmat->m32 = yz - wx;
    rotmat->m33 = 1.0f - (xx + yy);
    rotmat->m34 = 0.0f;
    rotmat->m41 = 0.0f;
    rotmat->m42 = 0.0f;
    rotmat->m43 = 0.0f;
    rotmat->m44 = 1.0f;
}

void FEMatrix4::Identify() {
    m11 = 1.0f;
    m12 = 0.0f;
    m13 = 0.0f;
    m14 = 0.0f;
    m21 = 0.0f;
    m22 = 1.0f;
    m23 = 0.0f;
    m24 = 0.0f;
    m31 = 0.0f;
    m32 = 0.0f;
    m33 = 1.0f;
    m34 = 0.0f;
    m41 = 0.0f;
    m42 = 0.0f;
    m43 = 0.0f;
    m44 = 1.0f;
}

void FEMultMatrix(FEMatrix4 *dest, const FEMatrix4 *a, const FEMatrix4 *b) {
    FEMatrix4 t;
    t.m11 = b->m11 * a->m11 + b->m12 * a->m21 + b->m13 * a->m31 + b->m14 * a->m41;
    t.m12 = b->m11 * a->m12 + b->m12 * a->m22 + b->m13 * a->m32 + b->m14 * a->m42;
    t.m13 = b->m11 * a->m13 + b->m12 * a->m23 + b->m13 * a->m33 + b->m14 * a->m43;
    t.m14 = b->m11 * a->m14 + b->m12 * a->m24 + b->m13 * a->m34 + b->m14 * a->m44;
    t.m21 = b->m21 * a->m11 + b->m22 * a->m21 + b->m23 * a->m31 + b->m24 * a->m41;
    t.m22 = b->m21 * a->m12 + b->m22 * a->m22 + b->m23 * a->m32 + b->m24 * a->m42;
    t.m23 = b->m21 * a->m13 + b->m22 * a->m23 + b->m23 * a->m33 + b->m24 * a->m43;
    t.m24 = b->m21 * a->m14 + b->m22 * a->m24 + b->m23 * a->m34 + b->m24 * a->m44;
    t.m31 = b->m31 * a->m11 + b->m32 * a->m21 + b->m33 * a->m31 + b->m34 * a->m41;
    t.m32 = b->m31 * a->m12 + b->m32 * a->m22 + b->m33 * a->m32 + b->m34 * a->m42;
    t.m33 = b->m31 * a->m13 + b->m32 * a->m23 + b->m33 * a->m33 + b->m34 * a->m43;
    t.m34 = b->m31 * a->m14 + b->m32 * a->m24 + b->m33 * a->m34 + b->m34 * a->m44;
    t.m41 = b->m41 * a->m11 + b->m42 * a->m21 + b->m43 * a->m31 + b->m44 * a->m41;
    t.m42 = b->m41 * a->m12 + b->m42 * a->m22 + b->m43 * a->m32 + b->m44 * a->m42;
    t.m43 = b->m41 * a->m13 + b->m42 * a->m23 + b->m43 * a->m33 + b->m44 * a->m43;
    t.m44 = b->m41 * a->m14 + b->m42 * a->m24 + b->m43 * a->m34 + b->m44 * a->m44;
    *dest = t;
}

void FEMultMatrix(FEVector3 *dest, const FEMatrix4 *m, const FEVector3 *v) {
    float x = m->m11 * v->x + m->m21 * v->y + m->m31 * v->z + m->m41;
    float y = m->m12 * v->x + m->m22 * v->y + m->m32 * v->z + m->m42;
    float z = m->m13 * v->x + m->m23 * v->y + m->m33 * v->z + m->m43;
    dest->z = z;
    dest->x = x;
    dest->y = y;
}
