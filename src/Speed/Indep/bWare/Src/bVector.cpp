#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "math.h"

float bDistBetween(const bVector3 *v1, const bVector3 *v2) {
    float x = v1->x - v2->x;
    float y = v1->y - v2->y;
    float z = v1->z - v2->z;

    return sqrtf(x * x + y * y + z * z);
}

bVector2 *bNormalize(bVector2 *dest, const bVector2 *v) {
    float len = bLength(v);

    if (len != 0.0f) {
        float x = v->x;
        float y = v->y;
        dest->x = x / len;
        dest->y = y / len;
    } else {
        dest->y = 0.0f;
        dest->x = 1.0f;
    }
    return dest;
}

bVector2 *bNormalize(bVector2 *dest, const bVector2 *v, float length) {
    float len = bLength(v) / length;

    if (len != 0.0f) {
        float x = v->x;
        float y = v->y;
        dest->x = x / len;
        dest->y = y / len;
    } else {
        dest->y = 0.0f;
        dest->x = 1.0f;
    }
    return dest;
}

bVector3 *bNormalize(bVector3 *dest, const bVector3 *v) {
    float len = bLength(v);

    if (len != 0.0f) {
        float inv_len = 1.0f / len;
        float x = v->x;
        float y = v->y;
        float z = v->z;
        dest->x = x * inv_len;
        dest->y = y * inv_len;
        dest->z = z * inv_len;
    } else {
        dest->x = 1.0f;
        dest->y = 0.0f;
        dest->z = 0.0f;
    }
    return dest;
}

bVector3 *bNormalize(bVector3 *dest, const bVector3 *v, float length) {
    float len = bLength(v) / length;

    if (len != 0.0f) {
        float inv_len = 1.0f / len;
        float x = v->x;
        float y = v->y;
        float z = v->z;
        dest->x = x * inv_len;
        dest->y = y * inv_len;
        dest->z = z * inv_len;
    } else {
        dest->x = length;
        dest->y = 0.0f;
        dest->z = 0.0f;
    }
    return dest;
}

bVector4 *bNormalize(bVector4 *dest, const bVector4 *v) {
    float len = bLength(v);
    if (len != 0.0f) {
        float inv_len = 1.0f / len;
        float x = v->x;
        float y = v->y;
        float z = v->z;
        dest->x = x * inv_len;
        dest->y = y * inv_len;
        dest->z = z * inv_len;
    } else {
        dest->x = 1.0f;
        dest->y = 0.0f;
        dest->z = 0.0f;
    }
    return dest;
}
