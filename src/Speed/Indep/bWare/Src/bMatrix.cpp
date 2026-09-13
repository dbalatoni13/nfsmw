

#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#ifdef EA_PLATFORM_WIN32
extern "C" __declspec(dllimport) bMatrix4 *__stdcall D3DXMatrixTranspose(bMatrix4 *dest, const bMatrix4 *src);
#endif

void bInvertMatrix(bMatrix4 *dest, const bMatrix4 *src) {
    float a = src->v0.x;
    float b = src->v0.y;
    float c = src->v0.z;
    float g = src->v1.x;
    float f = src->v1.y;
    float d = src->v1.z;
    float i = src->v2.x;
    float h = src->v2.y;
    float e = src->v2.z;
    float j = src->v3.x;
    float k = src->v3.y;
    float l = src->v3.z;

    float scale = 1.0f / (a * (f * e - d * h) - b * (g * e - d * i) + c * (g * h - f * i));

    dest->v0.x = scale * (f * e - d * h);
    dest->v0.y = scale * (c * h - b * e);
    dest->v0.z = scale * (b * d - c * f);
    dest->v0.w = 0.0f;
    dest->v1.x = scale * (d * i - g * e);
    dest->v1.y = scale * (a * e - c * i);
    dest->v1.z = scale * (c * g - a * d);
    dest->v1.w = 0.0f;
    dest->v2.x = scale * (g * h - f * i);
    dest->v2.y = scale * (b * i - a * h);
    dest->v2.z = scale * (a * f - b * g);
    dest->v2.w = 0.0f;
    dest->v3.x = -(dest->v0.x * j + dest->v1.x * k + dest->v2.x * l);
    dest->v3.y = -(dest->v0.y * j + dest->v1.y * k + dest->v2.y * l);
    dest->v3.z = -(dest->v0.z * j + dest->v1.z * k + dest->v2.z * l);
    dest->v3.w = 1.0f;
}

// Semantic reconstruction of the scalar four-by-four determinant. The
// target uses a different term grouping, so this remains compiler-near rather
// than byte-identical.
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
    __declspec(align(16)) bMatrix4 U(bMatrix4::NO_INITIALIZATION);
    bMatrix4 iU(bMatrix4::NO_INITIALIZATION);
    bMatrix4 Mf(bMatrix4::NO_INITIALIZATION);
    bMatrix4 iMf(bMatrix4::NO_INITIALIZATION);
    bMatrix4 Nf(bMatrix4::NO_INITIALIZATION);

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
    eMulMatrix(&U, &iMf, &iU);
    eMulMatrix(&Nf, &Mf, &U);
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
#elif defined(EA_PLATFORM_WIN32)
    D3DXMatrixTranspose(dest, m);
#else
    float transposed[4][4];
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            transposed[row][column] = (*m)[column][row];
        }
    }
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            (*dest)[row][column] = transposed[row][column];
        }
    }
#endif
    return dest;
}
