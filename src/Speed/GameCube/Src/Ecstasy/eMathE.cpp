#include "Speed/GameCube/Src/Ecstasy/eMatrixE.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
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

inline float eRecip(float x) {
    float val = x;
    float recip;

    asm("fres %0, %1" : "=f"(recip) : "f"(val));
    return recip;
}

void eCopyMatrix(bMatrix4 *dest, bMatrix4 *src) {
    bCopy(dest, src);
}

void eMulMatrix(bMatrix4 *ab, bMatrix4 *a, bMatrix4 *b) {
    MTX44Concat(*reinterpret_cast<Mtx44 *>(a), *reinterpret_cast<Mtx44 *>(b), *reinterpret_cast<Mtx44 *>(ab));
}

// TODO the variables might be unused in these two
void eMulVector(bVector4 *vm, const bMatrix4 *m, const bVector4 *v) {
    {
        double FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9, FP10, FP11;

        asm volatile("psq_l %0, 0(%6), 0, 0\n"
                     "psq_l %4, 0(%5), 0, 0\n"
                     "ps_muls0 %4, %4, %0\n"
                     "addi 9, %5, 0x10\n"
                     "psq_l %2, 0(9), 0, 0\n"
                     "ps_madds1 %2, %2, %0, %4\n"
                     "addi %6, %6, 8\n"
                     "psq_l %1, 0(%6), 0, 0\n"
                     "addi 9, %5, 0x20\n"
                     "psq_l %3, 0(9), 0, 0\n"
                     "ps_madds0 %3, %3, %1, %2\n"
                     "addi 9, %5, 0x30\n"
                     "psq_l %4, 0(9), 0, 0\n"
                     "ps_madds1 %4, %4, %1, %3\n"
                     "psq_st %4, 0(%7), 0, 0\n"
                     "addi 9, %5, 8\n"
                     "psq_l %4, 0(9), 0, 0\n"
                     "ps_muls0 %4, %4, %0\n"
                     "addi 9, %5, 0x18\n"
                     "psq_l %3, 0(9), 0, 0\n"
                     "ps_madds1 %3, %3, %0, %4\n"
                     "addi 9, %5, 0x28\n"
                     "psq_l %2, 0(9), 0, 0\n"
                     "ps_madds0 %2, %2, %1, %3\n"
                     "addi %5, %5, 0x38\n"
                     "psq_l %4, 0(%5), 0, 0\n"
                     "ps_madds1 %4, %4, %1, %2\n"
                     "psq_st %4, 8(%7), 0, 0"
                     : "=&f"(FP0), "=&f"(FP1), "=&f"(FP2), "=&f"(FP3), "=&f"(FP4)
                     : "b"(m), "b"(v), "b"(vm)
                     : "r9", "memory");
    }
}

void eMulVector(bVector3 *vm, const bMatrix4 *m, const bVector3 *v) {
    {
        double FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9, FP10, FP11;

        asm volatile("psq_l %0, 0(%6), 0, 0\n"
                     "psq_l %4, 0(%5), 0, 0\n"
                     "ps_muls0 %4, %4, %0\n"
                     "addi 9, %5, 0x10\n"
                     "psq_l %2, 0(9), 0, 0\n"
                     "ps_madds1 %2, %2, %0, %4\n"
                     "addi 5, 5, 8\n"
                     "psq_l %1, 0(5), 1, 0\n"
                     "addi 9, %5, 0x20\n"
                     "psq_l %3, 0(9), 0, 0\n"
                     "ps_madds0 %3, %3, %1, %2\n"
                     "addi 9, %5, 0x30\n"
                     "psq_l %4, 0(9), 0, 0\n"
                     "ps_madds1 %4, %4, %1, %3\n"
                     "psq_st %4, 0(%7), 0, 0\n"
                     "addi 9, %5, 8\n"
                     "psq_l %4, 0(9), 0, 0\n"
                     "ps_muls0 %4, %4, %0\n"
                     "addi 9, %5, 0x18\n"
                     "psq_l %3, 0(9), 0, 0\n"
                     "ps_madds1 %3, %3, %0, %4\n"
                     "addi 9, %5, 0x28\n"
                     "psq_l %2, 0(9), 0, 0\n"
                     "ps_madds0 %2, %2, %1, %3\n"
                     "addi 4, 4, 0x38\n"
                     "psq_l %4, 0(4), 0, 0\n"
                     "ps_madds1 %4, %4, %1, %2\n"
                     "psq_st %4, 8(%7), 1, 0"
                     : "=&f"(FP0), "=&f"(FP1), "=&f"(FP2), "=&f"(FP3), "=&f"(FP4)
                     : "r"(m), "r"(v), "r"(vm)
                     : "r9", "memory");
    }
}

void eProject(float x, float y, float z, Mtx mtx, float *pm, float *vp, float *sx, float *sy, float *sz) {
    Vec local;
    Vec eye;

    local.x = x;
    local.y = y;
    local.z = z;
    MTXMultVec(mtx, &local, &eye);

    if (eye.z == 0.0f) {
        *sy = 0.0f;
        *sx = 0.0f;
        *sz = -2.0f;
        return;
    }

    float oneOverW = eRecip(-eye.z);
    float clipX = eye.x * pm[1] + eye.z * pm[2];
    float clipY = -(eye.y * pm[3] + eye.z * pm[4]);
    float clipZ = eye.z * pm[5] + pm[6];
    float halfVP2 = vp[2] * 0.5f;
    float halfVP3 = vp[3] * 0.5f;

    *sx = vp[0] + halfVP2 + halfVP2 * clipX * oneOverW;
    *sy = vp[1] + halfVP3 + halfVP3 * clipY * oneOverW;
    *sz = (vp[5] - vp[4]) * clipZ * oneOverW + vp[5];
}

void eRotTransPers(bVector3 *dest, const bVector3 *src, bMatrix4 *wv, bMatrix4 *vs, float xOrig, float yOrig, float width, float height, float zNear,
                   float zFar) {
    float eproj[7] = {0.0f, vs->v0.x, vs->v0.z, vs->v1.y, vs->v1.z, vs->v2.z, vs->v2.w};
    float eviewport[6] = {xOrig, yOrig, width, height, zNear, zFar};
    float mhW2V[3][4];

    eConvertToGX34(mhW2V, *wv);
    eProject(src->x, src->y, src->z, mhW2V, eproj, eviewport, &dest->x, &dest->y, &dest->z);
}

void eCreateAxisRotationMatrix(bMatrix4 *dest, bVector3 &axis, bAngle angle) {
    float c = bCos(angle);
    float s = bSin(angle);
    float t = 1.0f - c;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    float tx = t * x;
    float ty = t * y;

    dest->v0.x = tx * x + c;
    dest->v0.y = tx * y - s * z;
    dest->v0.z = tx * z + s * y;
    dest->v0.w = 0.0f;
    dest->v1.x = tx * y + s * z;
    dest->v1.y = ty * y + c;
    dest->v1.z = ty * z - x * s;
    dest->v1.w = 0.0f;
    dest->v2.x = tx * z - s * y;
    dest->v2.y = ty * z + x * s;
    dest->v2.z = t * z * z + c;
    dest->v2.w = 0.0f;
    dest->v3.x = 0.0f;
    dest->v3.y = 0.0f;
    dest->v3.z = 0.0f;
    dest->v3.w = 1.0f;
}

void eCreateLookAtMatrix(bMatrix4 *mat, bVector3 &eye, bVector3 &center, bVector3 &up) {
    bVector3 c = center - eye;
    bNormalize(&c, &c);
    bVector3 b = -up;
    bVector3 a;
    bMatrix4 tl;

    bCross(&a, &b, &c);
    bCross(&b, &c, &a);
    bNormalize(&a, &a);
    bNormalize(&b, &b);

    mat->v0.x = a.x;
    mat->v0.y = b.x;
    mat->v0.z = c.x;
    mat->v0.w = 0.0f;
    mat->v1.x = a.y;
    mat->v1.y = b.y;
    mat->v1.z = c.y;
    mat->v1.w = 0.0f;
    mat->v2.x = a.z;
    mat->v2.y = b.z;
    mat->v2.z = c.z;
    mat->v2.w = 0.0f;
    mat->v3.x = 0.0f;
    mat->v3.y = 0.0f;
    mat->v3.z = 0.0f;
    mat->v3.w = 1.0f;

    tl.v0.x = 1.0f;
    tl.v0.y = 0.0f;
    tl.v0.z = 0.0f;
    tl.v0.w = 0.0f;
    tl.v1.x = 0.0f;
    tl.v1.y = 1.0f;
    tl.v1.z = 0.0f;
    tl.v1.w = 0.0f;
    tl.v2.x = 0.0f;
    tl.v2.y = 0.0f;
    tl.v2.z = 1.0f;
    tl.v2.w = 0.0f;
    tl.v3.x = -eye.x;
    tl.v3.y = -eye.y;
    tl.v3.z = -eye.z;
    tl.v3.w = 1.0f;
    eMulMatrix(mat, &tl, mat);
}

float eSin(float a) {
    const float twopi = UMath::PI * 2;
    float flip_sign;
    const float pi = UMath::PI;
    const float piover2 = pi / 2.0f;
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
