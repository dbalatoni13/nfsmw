

#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

void bInvertMatrix(bMatrix4 *dest, const bMatrix4 *src) {
    eInvertMatrix(dest, const_cast<bMatrix4 *>(src));
}

// UNSOLVED
float fDeterminant(bMatrix4 *m) {
    float value =
        m->v0.x * m->v1.y * m->v2.z * m->v3.w +
        (((m->v0.z * m->v1.x * m->v2.y * m->v3.w + m->v0.y * m->v1.z * m->v2.x * m->v3.w +
           (((m->v0.y * m->v1.x * m->v2.w * m->v3.z + m->v0.x * m->v1.w * m->v2.y * m->v3.z +
              (((m->v0.w * m->v1.y * m->v2.x * m->v3.z + m->v0.x * m->v1.z * m->v2.w * m->v3.y +
                 (((m->v0.w * m->v1.x * m->v2.z * m->v3.y + m->v0.z * m->v1.w * m->v2.x * m->v3.y +
                    (((m->v0.z * m->v1.y * m->v2.w * m->v3.x + m->v0.y * m->v1.w * m->v2.z * m->v3.x +
                       ((m->v0.w * m->v1.z * m->v2.y * m->v3.x - m->v0.z * m->v1.w * m->v2.y * m->v3.x) - m->v0.w * m->v1.y * m->v2.z * m->v3.x)) -
                      m->v0.y * m->v1.z * m->v2.w * m->v3.x) -
                     m->v0.w * m->v1.z * m->v2.x * m->v3.y)) -
                   m->v0.x * m->v1.w * m->v2.z * m->v3.y) -
                  m->v0.z * m->v1.x * m->v2.w * m->v3.y)) -
                m->v0.y * m->v1.w * m->v2.x * m->v3.z) -
               m->v0.w * m->v1.x * m->v2.y * m->v3.z)) -
             m->v0.x * m->v1.y * m->v2.w * m->v3.z) -
            m->v0.z * m->v1.y * m->v2.x * m->v3.w)) -
          m->v0.x * m->v1.z * m->v2.y * m->v3.w) -
         m->v0.y * m->v1.x * m->v2.z * m->v3.w);

    return value;
}

void fInvertMatrix(bMatrix4 *d, bMatrix4 *s) {
    float scale = 1.0f / fDeterminant(s);

    d->v0.x = scale * (((((s->v1.z * s->v2.w * s->v3.y - s->v1.w * s->v2.z * s->v3.y) + s->v1.w * s->v2.y * s->v3.z) - s->v1.y * s->v2.w * s->v3.z) -
                        s->v1.z * s->v2.y * s->v3.w) +
                       s->v1.y * s->v2.z * s->v3.w);
    d->v0.y = scale * ((((s->v0.w * s->v2.z * s->v3.y - s->v0.z * s->v2.w * s->v3.y) - s->v0.w * s->v2.y * s->v3.z) + s->v0.y * s->v2.w * s->v3.z +
                        s->v0.z * s->v2.y * s->v3.w) -
                       s->v0.y * s->v2.z * s->v3.w);
    d->v0.z = scale * (((((s->v0.z * s->v1.w * s->v3.y - s->v0.w * s->v1.z * s->v3.y) + s->v0.w * s->v1.y * s->v3.z) - s->v0.y * s->v1.w * s->v3.z) -
                        s->v0.z * s->v1.y * s->v3.w) +
                       s->v0.y * s->v1.z * s->v3.w);
    d->v0.w = scale * ((((s->v0.w * s->v1.z * s->v2.y - s->v0.z * s->v1.w * s->v2.y) - s->v0.w * s->v1.y * s->v2.z) + s->v0.y * s->v1.w * s->v2.z +
                        s->v0.z * s->v1.y * s->v2.w) -
                       s->v0.y * s->v1.z * s->v2.w);
    d->v1.x = scale * ((((s->v1.w * s->v2.z * s->v3.x - s->v1.z * s->v2.w * s->v3.x) - s->v1.w * s->v2.x * s->v3.z) + s->v1.x * s->v2.w * s->v3.z +
                        s->v1.z * s->v2.x * s->v3.w) -
                       s->v1.x * s->v2.z * s->v3.w);
    d->v1.y = scale * (((((s->v0.z * s->v2.w * s->v3.x - s->v0.w * s->v2.z * s->v3.x) + s->v0.w * s->v2.x * s->v3.z) - s->v0.x * s->v2.w * s->v3.z) -
                        s->v0.z * s->v2.x * s->v3.w) +
                       s->v0.x * s->v2.z * s->v3.w);
    d->v1.z = scale * ((((s->v0.w * s->v1.z * s->v3.x - s->v0.z * s->v1.w * s->v3.x) - s->v0.w * s->v1.x * s->v3.z) + s->v0.x * s->v1.w * s->v3.z +
                        s->v0.z * s->v1.x * s->v3.w) -
                       s->v0.x * s->v1.z * s->v3.w);
    d->v1.w = scale * (((((s->v0.z * s->v1.w * s->v2.x - s->v0.w * s->v1.z * s->v2.x) + s->v0.w * s->v1.x * s->v2.z) - s->v0.x * s->v1.w * s->v2.z) -
                        s->v0.z * s->v1.x * s->v2.w) +
                       s->v0.x * s->v1.z * s->v2.w);
    d->v2.x = scale * (((((s->v1.y * s->v2.w * s->v3.x - s->v1.w * s->v2.y * s->v3.x) + s->v1.w * s->v2.x * s->v3.y) - s->v1.x * s->v2.w * s->v3.y) -
                        s->v1.y * s->v2.x * s->v3.w) +
                       s->v1.x * s->v2.y * s->v3.w);
    d->v2.y = scale * ((((s->v0.w * s->v2.y * s->v3.x - s->v0.y * s->v2.w * s->v3.x) - s->v0.w * s->v2.x * s->v3.y) + s->v0.x * s->v2.w * s->v3.y +
                        s->v0.y * s->v2.x * s->v3.w) -
                       s->v0.x * s->v2.y * s->v3.w);
    d->v2.z = scale * (((((s->v0.y * s->v1.w * s->v3.x - s->v0.w * s->v1.y * s->v3.x) + s->v0.w * s->v1.x * s->v3.y) - s->v0.x * s->v1.w * s->v3.y) -
                        s->v0.y * s->v1.x * s->v3.w) +
                       s->v0.x * s->v1.y * s->v3.w);
    d->v2.w = scale * ((((s->v0.w * s->v1.y * s->v2.x - s->v0.y * s->v1.w * s->v2.x) - s->v0.w * s->v1.x * s->v2.y) + s->v0.x * s->v1.w * s->v2.y +
                        s->v0.y * s->v1.x * s->v2.w) -
                       s->v0.x * s->v1.y * s->v2.w);
    d->v3.x = scale * ((((s->v1.z * s->v2.y * s->v3.x - s->v1.y * s->v2.z * s->v3.x) - s->v1.z * s->v2.x * s->v3.y) + s->v1.x * s->v2.z * s->v3.y +
                        s->v1.y * s->v2.x * s->v3.z) -
                       s->v1.x * s->v2.y * s->v3.z);
    d->v3.y = scale * (((((s->v0.y * s->v2.z * s->v3.x - s->v0.z * s->v2.y * s->v3.x) + s->v0.z * s->v2.x * s->v3.y) - s->v0.x * s->v2.z * s->v3.y) -
                        s->v0.y * s->v2.x * s->v3.z) +
                       s->v0.x * s->v2.y * s->v3.z);
    d->v3.z = scale * ((((s->v0.z * s->v1.y * s->v3.x - s->v0.y * s->v1.z * s->v3.x) - s->v0.z * s->v1.x * s->v3.y) + s->v0.x * s->v1.z * s->v3.y +
                        s->v0.y * s->v1.x * s->v3.z) -
                       s->v0.x * s->v1.y * s->v3.z);
    d->v3.w = scale * (((((s->v0.y * s->v1.z * s->v2.x - s->v0.z * s->v1.y * s->v2.x) + s->v0.z * s->v1.x * s->v2.y) - s->v0.x * s->v1.z * s->v2.y) -
                        s->v0.y * s->v1.x * s->v2.z) +
                       s->v0.x * s->v1.y * s->v2.z);
}

void hermite_basis(bMatrix4 *b, bMatrix4 *p, float u1, float u2, float u3, float u4) {
    bMatrix4 U;
    bMatrix4 iU;
    bMatrix4 Mf;
    bMatrix4 iMf;
    bMatrix4 K;
    bMatrix4 Nf;

    Mf.v0.x = 2.0f;
    Mf.v0.y = -2.0f;
    Mf.v0.z = 1.0f;
    Mf.v0.w = 1.0f;
    Mf.v1.x = -3.0f;
    Mf.v1.y = 3.0f;
    Mf.v1.z = -2.0f;
    Mf.v1.w = -1.0f;
    Mf.v2.x = 0.0f;
    Mf.v2.y = 0.0f;
    Mf.v2.z = 1.0f;
    Mf.v2.w = 0.0f;
    Mf.v3.x = 1.0f;
    Mf.v3.y = 0.0f;
    Mf.v3.z = 0.0f;
    Mf.v3.w = 0.0f;

    iMf.v0.x = 0.0f;
    iMf.v0.y = 0.0f;
    iMf.v0.z = 0.0f;
    iMf.v0.w = 1.0f;
    iMf.v1.x = 1.0f;
    iMf.v1.y = 1.0f;
    iMf.v1.z = 1.0f;
    iMf.v1.w = 1.0f;
    iMf.v2.x = 0.0f;
    iMf.v2.y = 0.0f;
    iMf.v2.z = 1.0f;
    iMf.v2.w = 0.0f;
    iMf.v3.x = 3.0f;
    iMf.v3.y = 2.0f;
    iMf.v3.z = 1.0f;
    iMf.v3.w = 0.0f;

    U.v0.x = u1 * u1 * u1;
    U.v0.y = u1 * u1;
    U.v0.z = u1;
    U.v0.w = 1.0f;
    U.v1.x = u2 * u2 * u2;
    U.v1.y = u2 * u2;
    U.v1.z = u2;
    U.v1.w = 1.0f;
    U.v2.x = u3 * u3 * u3;
    U.v2.y = u3 * u3;
    U.v2.z = u3;
    U.v2.w = 1.0f;
    U.v3.x = u4 * u4 * u4;
    U.v3.y = u4 * u4;
    U.v3.z = u4;
    U.v3.w = 1.0f;

    fInvertMatrix(&iU, &U);
    eMulMatrix(&K, &iMf, &iU);
    eMulMatrix(&Nf, &Mf, &K);
    eMulMatrix(b, &Nf, p);
}

void hermite_parameter(bVector4 *dest, const bMatrix4 *b, float t) {
    bVector4 u;

    u.x = t * t * t;
    u.y = t * t;
    u.z = t;
    u.w = 1.0f;
    eMulVector(dest, b, &u);
}

void bMulMatrix(bMatrix4 *dest, const bMatrix4 *a, const bMatrix4 *b) {
    eMulMatrix(dest, const_cast<bMatrix4 *>(b), const_cast<bMatrix4 *>(a));
}

void bMulMatrix(bVector4 *dest, const bMatrix4 *m, const bVector4 *v) {
    eMulVector(dest, m, v);
}

void bMulMatrix(bVector3 *dest, const bMatrix4 *m, const bVector3 *v) {
    eMulVector(dest, m, v);
}

bMatrix4 *bTransposeMatrix(bMatrix4 *dest, const bMatrix4 *m) {
#ifdef EA_PLATFORM_GAMECUBE
    MTX44Transpose(*reinterpret_cast<const Mtx44 *>(m), *reinterpret_cast<Mtx44 *>(dest));
#else
// TODO
#endif
    return dest;
}
