#include "Speed/Indep/bWare/Inc/bMath.hpp"

float bDistBetween(const bVector3 *v1, const bVector3 *v2) {
    float x = v1->x - v2->x;
    float y = v1->y - v2->y;
    float z = v1->z - v2->z;

    return bSqrt(x * x + y * y + z * z);
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

bVector2 *bScaleAdd(bVector2 *dest, const bVector2 *v1, const bVector2 *v2, float scale) {
    float x = v2->x * scale + v1->x;
    float y = v2->y * scale + v1->y;
    dest->x = x;
    dest->y = y;
    return dest;
}

bVector3 *bScaleAdd(bVector3 *dest, const bVector3 *v1, const bVector3 *v2, float scale) {
    float x = v2->x * scale + v1->x;
    float y = v2->y * scale + v1->y;
    float z = v2->z * scale + v1->z;
    dest->x = x;
    dest->y = y;
    dest->z = z;
    return dest;
}

bVector4 *bScaleAdd(bVector4 *dest, const bVector4 *v1, const bVector4 *v2, float scale) {
    float x = v2->x * scale + v1->x;
    float y = v2->y * scale + v1->y;
    float z = v2->z * scale + v1->z;
    float w = v2->w * scale + v1->w;
    dest->x = x;
    dest->y = y;
    dest->z = z;
    dest->w = w;
    return dest;
}

int bEqual(const bVector2 *v1, const bVector2 *v2, float epsilon) {
    if (bAbs(v1->x - v2->x) > epsilon) {
        return 0;
    }
    return bAbs(v1->y - v2->y) <= epsilon;
}

bVector3 *bCross(bVector3 *dest, const bVector3 *v1, const bVector3 *v2) {
    float x = v1->y * v2->z - v1->z * v2->y;
    float y = v1->z * v2->x - v1->x * v2->z;
    float z = v1->x * v2->y - v1->y * v2->x;
    dest->x = x;
    dest->y = y;
    dest->z = z;
    return dest;
}

void bInitializeBoundingBox(struct bVector2 *bbox_min, struct bVector2 *bbox_max) {
    bbox_min->x = 3.4028235e+38f;
    bbox_min->y = 3.4028235e+38f;

    bbox_max->x = -3.4028235e+38f;
    bbox_max->y = -3.4028235e+38f;
}

void bExpandBoundingBox(bVector2 *bbox_min, bVector2 *bbox_max, const bVector2 *point) {
    float min_x = bMin(bbox_min->x, point->x);
    float min_y = bMin(bbox_min->y, point->y);

    float max_x = bMax(bbox_max->x, point->x);
    float max_y = bMax(bbox_max->y, point->y);

    bFill(bbox_min, min_x, min_y);
    bFill(bbox_max, max_x, max_y);
}

BOOL bBoundingBoxIsInside(const bVector2 *bbox_min, const bVector2 *bbox_max, const bVector2 *point, float extra_width) {
    if ((point->x < bbox_min->x - extra_width) || (point->x > bbox_max->x + extra_width) || (point->y < bbox_min->y - extra_width) ||
        (point->y > bbox_max->y + extra_width)) {
        return false;
    }
    return true;
}

BOOL bBoundingBoxOverlapping(const bVector2 *bbox_min, const bVector2 *bbox_max, const bVector2 *bbox2_min, const bVector2 *bbox2_max) {
    if ((bbox2_min->x < bbox_max->x) && (bbox2_max->x > bbox_min->x) && (bbox2_min->y < bbox_max->y) && (bbox2_max->y > bbox_min->y)) {
        return true;
    }
    return false;
}

void bInitializeBoundingBox(bVector3 *bbox_min, bVector3 *bbox_max) {
    bbox_min->x = 3.4028235e+38f;
    bbox_min->y = 3.4028235e+38f;
    bbox_min->z = 3.4028235e+38f;

    bbox_max->x = -3.4028235e+38f;
    bbox_max->y = -3.4028235e+38f;
    bbox_max->z = -3.4028235e+38f;
}

void bInitializeBoundingBox(bVector3 *bbox_min, bVector3 *bbox_max, const bVector3 *point) {
    float x = point->x;
    float y = point->y;
    float z = point->z;

    bbox_min->x = x;
    bbox_min->y = y;
    bbox_min->z = z;

    bbox_max->x = x;
    bbox_max->y = y;
    bbox_max->z = z;
}

void bExpandBoundingBox(bVector3 *bbox_min, bVector3 *bbox_max, const bVector3 *point, float extra_width) {
    float x_min = point->x - extra_width;
    float y_min = point->y - extra_width;
    float z_min = point->z - extra_width;

    float x_max = point->x + extra_width;
    float y_max = point->y + extra_width;
    float z_max = point->z + extra_width;

    if (x_min < bbox_min->x) {
        bbox_min->x = x_min;
    }
    if (y_min < bbox_min->y) {
        bbox_min->y = y_min;
    }
    if (z_min < bbox_min->z) {
        bbox_min->z = z_min;
    }

    if (x_max > bbox_max->x) {
        bbox_max->x = x_max;
    }
    if (y_max > bbox_max->y) {
        bbox_max->y = y_max;
    }
    if (z_max > bbox_max->z) {
        bbox_max->z = z_max;
    }
}

void bExpandBoundingBox(bVector3 *bbox_min, bVector3 *bbox_max, const bVector3 *bbox2_min, const bVector3 *bbox2_max) {
    float x_min = bbox2_min->x;
    float y_min = bbox2_min->y;
    float z_min = bbox2_min->z;

    float x_max = bbox2_max->x;
    float y_max = bbox2_max->y;
    float z_max = bbox2_max->z;

    if (x_min < bbox_min->x) {
        bbox_min->x = bbox2_min->x;
    }
    if (y_min < bbox_min->y) {
        bbox_min->y = y_min;
    }
    if (z_min < bbox_min->z) {
        bbox_min->z = z_min;
    }

    if (x_max > bbox_max->x) {
        bbox_max->x = x_max;
    }
    if (y_max > bbox_max->y) {
        bbox_max->y = y_max;
    }
    if (z_max > bbox_max->z) {
        bbox_max->z = z_max;
    }
}

int bBoundingBoxIsInside(const bVector3 *bbox_min, const bVector3 *bbox_max, const bVector3 *point, float extra_width) {
    if ((point->x < bbox_min->x - extra_width) || (point->x > bbox_max->x + extra_width) || (point->y < bbox_min->y - extra_width) ||
        (point->y > bbox_max->y + extra_width || (point->z < bbox_min->z - extra_width) || (point->z > bbox_max->z + extra_width))) {
        return false;
    }
    return true;
}

bool bIsPointInPoly(const bVector2 *point, const bVector2 *points, int num_points) {
    float x = point->x;
    float y = point->y;
    bool inside = false;
    int j = num_points - 1;

    for (int i = 0; i < num_points; i++) {
        if ((((points[i].y <= y) && (y < points[j].y)) || (points[j].y <= y && (y < points[i].y))) &&
            (x < ((points[j].x - points[i].x) * (y - points[i].y)) / (points[j].y - points[i].y) + points[i].x)) {
            inside = !inside;
        }
        j = i;
    }
    return inside;
}

bool bIsPointInPoly(const bVector2 *point, const bVector3 *points, int num_points) {
    float x = point->x;
    float y = point->y;
    bool inside = false;
    int j = num_points - 1;

    for (int i = 0; i < num_points; i++) {
        if ((((points[i].y <= y) && (y < points[j].y)) || (points[j].y <= y && (y < points[i].y))) &&
            (x < ((points[j].x - points[i].x) * (y - points[i].y)) / (points[j].y - points[i].y) + points[i].x)) {
            inside = !inside;
        }
        j = i;
    }
    return inside;
}
