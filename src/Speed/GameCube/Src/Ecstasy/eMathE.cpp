#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "dolphin/mtx.h"

#include <math.h>

bMatrix4 eMathIdentityMatrix;
bMatrix4 eMathZeroMatrix;

void eMathInit(void) {
    bMatrix4 *identity = eGetIdentityMatrix();
    bMatrix4 *zero = eGetZeroMatrix();
    MTX44Identity(*reinterpret_cast<Mtx44 *>(identity));
    bMemSet(zero, 0, sizeof(*zero));
}

void eCopyMatrix(bMatrix4 *dest, bMatrix4 *src) {
    bCopy(dest, src);
}

void eMulMatrix(bMatrix4 *ab, bMatrix4 *a, bMatrix4 *b) {
    MTX44Concat(*reinterpret_cast<Mtx44 *>(a), *reinterpret_cast<Mtx44 *>(b), *reinterpret_cast<Mtx44 *>(ab));
}

float eSin(float a) {
    const float twopi = 6.2831855f;
    float flip_sign;
    const float pi = 3.1415927f;
    const float piover2 = 1.5707964f;
    float result;

    while (a > twopi) {
        a -= twopi;
    }

    while (a < 0.0f) {
        a += twopi;
    }

    flip_sign = 1.0f;

    if (a >= pi) {
        a -= pi;
        flip_sign = -1.0f;
    }

    if (a >= piover2) {
        a = pi - a;
    }

    float a2 = a * a;
    float a3 = a * a2;
    result = a + a3 * -0.16666657f;
    float a5 = a3 * a2;
    result += a5 * 0.0083330255f;
    float a7 = a5 * a2;
    result += a7 * -0.00019807414f;
    float a9 = a7 * a2;
    result += a9 * 2.601887e-06f;

    return result * flip_sign;
}

void eCreateRotationZ(bMatrix4 *dest, bAngle angle) {
    float cn = eSin(bAngToRad(angle) + bDegToRad(90.0f));
    float sn = eSin(bAngToRad(angle));

    dest->v0.x = cn;
    dest->v0.y = sn;
    dest->v0.z = 0.0f;
    dest->v0.w = 0.0f;
    dest->v1.x = -sn;
    dest->v1.y = cn;
    dest->v1.z = 0.0f;
    dest->v1.w = 0.0f;
    dest->v2.x = 0.0f;
    dest->v2.y = 0.0f;
    dest->v2.z = 1.0f;
    dest->v2.w = 0.0f;
    dest->v3.x = 0.0f;
    dest->v3.y = 0.0f;
    dest->v3.z = 0.0f;
    dest->v3.w = 1.0f;
}

void eRotateX(bMatrix4 *dest, bMatrix4 *a, bAngle angle) {
    bMatrix4 r;
    float cn = eSin(bAngToRad(angle) + bDegToRad(90.0f));
    float sn = eSin(bAngToRad(angle));

    r.v0.x = 1.0f;
    r.v0.y = 0.0f;
    r.v0.z = 0.0f;
    r.v0.w = 0.0f;
    r.v1.x = 0.0f;
    r.v1.y = cn;
    r.v1.z = sn;
    r.v1.w = 0.0f;
    r.v2.x = 0.0f;
    r.v2.y = -sn;
    r.v2.z = cn;
    r.v2.w = 0.0f;
    r.v3.x = 0.0f;
    r.v3.y = 0.0f;
    r.v3.z = 0.0f;
    r.v3.w = 1.0f;
    eMulMatrix(dest, a, &r);
}

void eRotateY(bMatrix4 *dest, bMatrix4 *a, bAngle angle) {
    bMatrix4 r;
    float cn = eSin(bAngToRad(angle) + bDegToRad(90.0f));
    float sn = eSin(bAngToRad(angle));

    r.v0.x = cn;
    r.v0.y = 0.0f;
    r.v0.z = -sn;
    r.v0.w = 0.0f;
    r.v1.x = 0.0f;
    r.v1.y = 1.0f;
    r.v1.z = 0.0f;
    r.v1.w = 0.0f;
    r.v2.x = sn;
    r.v2.y = 0.0f;
    r.v2.z = cn;
    r.v2.w = 0.0f;
    r.v3.x = 0.0f;
    r.v3.y = 0.0f;
    r.v3.z = 0.0f;
    r.v3.w = 1.0f;
    eMulMatrix(dest, a, &r);
}

void eRotateZ(bMatrix4 *dest, bMatrix4 *a, bAngle angle) {
    bMatrix4 rot;

    eCreateRotationZ(&rot, angle);
    eMulMatrix(dest, a, &rot);
}

void eTranslate(bMatrix4 *dest, bMatrix4 *a, bVector3 *tran) {
    bMatrix4 t;

    t.v0.x = 1.0f;
    t.v0.y = 0.0f;
    t.v0.z = 0.0f;
    t.v0.w = 0.0f;
    t.v1.x = 0.0f;
    t.v1.y = 1.0f;
    t.v1.z = 0.0f;
    t.v1.w = 0.0f;
    t.v2.x = 0.0f;
    t.v2.y = 0.0f;
    t.v2.z = 1.0f;
    t.v2.w = 0.0f;
    t.v3.x = tran->x;
    t.v3.y = tran->y;
    t.v3.z = tran->z;
    t.v3.w = 1.0f;
    eMulMatrix(dest, a, &t);
}

void eCreateTranslationMatrix(bMatrix4 *dest, bVector3 &tran) {
    dest->v0.x = 1.0f;
    dest->v0.y = 0.0f;
    dest->v0.z = 0.0f;
    dest->v0.w = 0.0f;
    dest->v1.x = 0.0f;
    dest->v1.y = 1.0f;
    dest->v1.z = 0.0f;
    dest->v1.w = 0.0f;
    dest->v2.x = 0.0f;
    dest->v2.y = 0.0f;
    dest->v2.z = 1.0f;
    dest->v2.w = 0.0f;
    dest->v3.x = tran.x;
    dest->v3.y = tran.y;
    dest->v3.z = tran.z;
    dest->v3.w = 1.0f;
}

bMatrix4 *eInvertMatrix(bMatrix4 *dest, bMatrix4 *m) {
    MTX44Inverse(*reinterpret_cast<Mtx44 *>(m), *reinterpret_cast<Mtx44 *>(dest));
    return dest;
}

bMatrix4 *eInvertTransformationMatrix(bMatrix4 *dest, const bMatrix4 *tm) {
    {
        bMatrix4 m1;
        bMatrix4 m2;
        float x0 = tm->v0.x;
        float y0 = tm->v0.y;
        float z0 = tm->v0.z;
        float x1 = tm->v1.x;
        float y1 = tm->v1.y;
        float z1 = tm->v1.z;
        float x2 = tm->v2.x;
        float y2 = tm->v2.y;
        float z2 = tm->v2.z;
        float x3 = tm->v3.x;
        float y3 = tm->v3.y;
        float z3 = tm->v3.z;

        m1.v0.x = x0;
        m1.v0.y = x1;
        m1.v0.z = x2;
        m1.v0.w = 0.0f;
        m1.v1.x = y0;
        m1.v1.y = y1;
        m1.v1.z = y2;
        m1.v1.w = 0.0f;
        m1.v2.x = z0;
        m1.v2.y = z1;
        m1.v2.z = z2;
        m1.v2.w = 0.0f;
        m1.v3.x = 0.0f;
        m1.v3.y = 0.0f;
        m1.v3.z = 0.0f;
        m1.v3.w = 1.0f;

        m2.v0.x = 1.0f;
        m2.v0.y = 0.0f;
        m2.v0.z = 0.0f;
        m2.v0.w = 0.0f;
        m2.v1.x = 0.0f;
        m2.v1.y = 1.0f;
        m2.v1.z = 0.0f;
        m2.v1.w = 0.0f;
        m2.v2.x = 0.0f;
        m2.v2.y = 0.0f;
        m2.v2.z = 1.0f;
        m2.v2.w = 0.0f;
        m2.v3.x = -x3;
        m2.v3.y = -y3;
        m2.v3.z = -z3;
        m2.v3.w = 1.0f;

        eMulMatrix(dest, &m2, &m1);
    }
    return dest;
}

bMatrix4 *eInvertRotationMatrix(bMatrix4 *dest, bMatrix4 *src) {
    bTransposeMatrix(dest, src);
    dest->v0.w = 0.0f;
    dest->v1.w = 0.0f;
    dest->v2.w = 0.0f;
    dest->v3.w = 0.0f;
    dest->v3.x = 0.0f;
    dest->v3.y = 0.0f;
    dest->v3.z = 0.0f;
    return dest;
}

float ePowf(float x, float y) {
    return powf(x, y);
}
