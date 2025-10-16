#include "./UTypes.h"

extern "C" {

float rasin(float a) {
    return asinf(a) * 0.15915494f;
}

float rsqrt(const float x) {
    return bSqrt(x);
}

float v3length(const UMath::Vector3 *v) {
    return rsqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

void rsincos(const float *ang, float *s, float *c) {
    float angle = *ang * 6.2831855f;
    *s = sinf(angle);
    *c = cosf(angle);
}

void v3crossprod(const UMath::Vector3 *p1, const UMath::Vector3 *p2, UMath::Vector3 *result) {
    float x = p1->y * p2->z - p1->z * p2->y;
    float y = p1->z * p2->x - p1->x * p2->z;
    float z = p1->x * p2->y - p1->y * p2->x;

    result->x = x;
    result->y = y;
    result->z = z;
}

void v3unit(const UMath::Vector3 *v, UMath::Vector3 *result) {
    float vlen = v3length(v);
    if (vlen > 0.0f) {
        float wol = 1.0f / vlen;

        result->x = v->x * wol;
        result->y = v->y * wol;
        result->z = v->z * wol;
    } else {
        *result = *v;
    }
}

void v3sub(int num, const UMath::Vector3 *src, const UMath::Vector3 *vtosub, UMath::Vector3 *results) {
    if (num <= 0) return;

    while (num != 0) {
        results->x = src->x - vtosub->x;
        results->y = src->y - vtosub->y;
        results->z = src->z - vtosub->z;
        src++;
        results++;
        num--;
    }
}

void v3add(int num, const UMath::Vector3 *src, const UMath::Vector3 *vtosub, UMath::Vector3 *results) {
    if (num <= 0) return;

    while (num != 0) {
        results->x = src->x + vtosub->x;
        results->y = src->y + vtosub->y;
        results->z = src->z + vtosub->z;
        src++;
        results++;
        num--;
    }
}

void MATRIX4_multxrot(const UMath::Matrix4 *m4, float xbangle, UMath::Matrix4 *resultm) {
    float xsin;
    float xcos;
    rsincos(&xbangle, &xsin, &xcos);

    float tmp = m4->v0.y;

    const float *m;
    float *result;

    resultm->v0.x = m4->v0.x;
    resultm->v0.y = tmp * xcos - m4->v0.z * xsin;
    resultm->v0.z = tmp * xsin + m4->v0.z * xcos;
    resultm->v0.w = m4->v0.w;

    tmp = m4->v1.y;

    resultm->v1.x = m4->v1.x;
    resultm->v1.y = tmp * xcos - m4->v1.z * xsin;
    resultm->v1.z = tmp * xsin + m4->v1.z * xcos;
    resultm->v1.w = m4->v1.w;

    tmp = m4->v2.y;

    resultm->v2.x = m4->v2.x;
    resultm->v2.y = tmp * xcos - m4->v2.z * xsin;
    resultm->v2.z = tmp * xsin + m4->v2.z * xcos;
    resultm->v2.w = m4->v2.w;

    tmp = m4->v3.y;

    resultm->v3.x = m4->v3.x;
    resultm->v3.y = tmp * xcos - m4->v3.z * xsin;
    resultm->v3.z = tmp * xsin + m4->v3.z * xcos;
    resultm->v3.w = m4->v3.w;
}

void MATRIX4_multyrot(const UMath::Matrix4 *m4, float ybangle, UMath::Matrix4 *resultm) {
    const float *m;
    float *result;

    float ysin;
    float ycos;
    rsincos(&ybangle, &ysin, &ycos);

    float tmp = m4->v0.x;

    resultm->v0.x = tmp * ycos + m4->v0.z * ysin;
    resultm->v0.y = m4->v0.y;
    resultm->v0.z = m4->v0.z * ycos - tmp * ysin;
    resultm->v0.w = m4->v0.w;

    tmp = m4->v1.x;

    resultm->v1.x = tmp * ycos + m4->v1.z * ysin;
    resultm->v1.y = m4->v1.y;
    resultm->v1.z = m4->v1.z * ycos - tmp * ysin;
    resultm->v1.w = m4->v1.w;

    tmp = m4->v2.x;

    resultm->v2.x = tmp * ycos + m4->v2.z * ysin;
    resultm->v2.y = m4->v2.y;
    resultm->v2.z = m4->v2.z * ycos - tmp * ysin;
    resultm->v2.w = m4->v2.w;

    tmp = m4->v3.x;

    resultm->v3.x = tmp * ycos + m4->v3.z * ysin;
    resultm->v3.y = m4->v3.y;
    resultm->v3.z = m4->v3.z * ycos - tmp * ysin;
    resultm->v3.w = m4->v3.w;
}

void MATRIX4_multzrot(const UMath::Matrix4 *m4, float zbangle, UMath::Matrix4 *resultm) {
    float zsin;
    float zcos;
    rsincos(&zbangle, &zsin, &zcos);

    float tmp = m4->v0.x;

    const float *m;
    float *result;

    resultm->v0.x = tmp * zcos - m4->v0.y * zsin;
    resultm->v0.y = tmp * zsin + m4->v0.y * zcos;
    resultm->v0.z = m4->v0.z;
    resultm->v0.w = m4->v0.w;

    tmp = m4->v1.x;

    resultm->v1.x = tmp * zcos - m4->v1.y * zsin;
    resultm->v1.y = tmp * zsin + m4->v1.y * zcos;
    resultm->v1.z = m4->v1.z;
    resultm->v1.w = m4->v1.w;

    tmp = m4->v2.x;

    resultm->v2.x = tmp * zcos - m4->v2.y * zsin;
    resultm->v2.y = tmp * zsin + m4->v2.y * zcos;
    resultm->v2.z = m4->v2.z;
    resultm->v2.w = m4->v2.w;

    tmp = m4->v3.x;

    resultm->v3.x = tmp * zcos - m4->v3.y * zsin;
    resultm->v3.y = tmp * zsin + m4->v3.y * zcos;
    resultm->v3.z = m4->v3.z;
    resultm->v3.w = m4->v3.w;
}

}
