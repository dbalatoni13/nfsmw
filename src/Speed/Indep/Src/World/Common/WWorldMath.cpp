#include "Speed/Indep/Src/World/WWorldMath.h"

extern "C" float rsqrt(const float x);

bool WWorldMath::SegmentIntersect(const UMath::Vector4 *line1, const UMath::Vector4 *line2, UMath::Vector4 *intersectPt) {
    const float l1x = line1[0].x;
    const float l1z = line1[0].z;
    const float x11 = line1[1].x - l1x;
    const float z11 = line1[1].z - l1z;
    const float l2z = line2[0].z;
    const float l2x = line2[0].x;
    const float x22 = line2[1].x - l2x;
    const float z22 = line2[1].z - l2z;
    const float ua_d = z22 * x11 - x22 * z11;

    if (ua_d == 0.0f) {
        return false;
    } else {
        const float z12 = l1z - l2z;
        const float x12 = l1x - l2x;
        const float ua_n = x22 * z12 - z22 * x12;
        if ((ua_n >= -0.0001f && ua_n <= ua_d) || (ua_n <= 0.0001f && ua_n >= ua_d)) {
            const float ub_n = x11 * z12 - z11 * x12;
            if ((ub_n >= -0.0001f && ub_n <= ua_d) || (ub_n <= 0.0001f && ub_n >= ua_d)) {
                if (intersectPt != nullptr) {
                    float t = ua_n / ua_d;
                    intersectPt->x = t * x11 + l1x;
                    intersectPt->y = t * (line1[1].y - line1[0].y) + line1[0].y;
                    intersectPt->z = t * z11 + l1z;
                    intersectPt->w = 1.0f;
                }
                return true;
            }
        }
    }
    return false;
}

// UNSOLVED, instr order
bool WWorldMath::IntersectCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r, float &u1, float &u2) {
    if (InCircle(x1, y1, cx, cy, r) && InCircle(x2, y2, cx, cy, r)) {
        u2 = 0.0f;
        u1 = 0.0f;
        return true;
    }

    y1 -= cy;
    y2 -= cy;
    y2 -= y1;

    x1 -= cx;
    x2 -= cx;
    x2 -= x1;
    float a = pow2(x2) + pow2(y2);

    if (a == 0.0f) {
        u2 = 0.0f;
        u1 = 0.0f;
        return true;
    }

    float b = ((x2 * x1) + (y2 * y1)) + ((x2 * x1) + (y2 * y1));
    float c = ((x1 * x1) + (y1 * y1) - (r * r)) * 4.0f;
    float t = b * b - a * c;

    if (t < 0.0f) {
        return false;
    }

    float root = rsqrt(t);
    float inv2timesA = 1.0f / (a + a);
    u1 = (-b - root) * inv2timesA;
    u2 = (-b + root) * inv2timesA;
    if (u1 >= 0.0f && u1 <= 1.0f) {
        return true;
    }
    if (u2 >= 0.0f && u2 <= 1.0f) {
        return true;
    }
    return false;
}

void WWorldMath::NearestPointLine2D(const UMath::Vector4 &pt, const UMath::Vector4 *line, UMath::Vector4 &nearPt) {
    const float &x1 = line[0].x;
    const float &z1 = line[0].z;
    const float &x2 = line[1].x;
    const float &z2 = line[1].z;
    const float &px = pt.x;
    const float &pz = pt.z;

    float u = (px - x1) * (x2 - x1) + (pz - z1) * (z2 - z1);
    float x_fake = x2 - x1;
    float z_fake = z2 - z1;
    float div = pow2(x_fake) + pow2(z_fake);

    if (1e-6f < div) {
        u = u / div;
    } else {
        u = 0.0f;
    }

    float x = u * (x2 - x1) + x1;
    float z = u * (z2 - z1) + z1;
    nearPt.z = z;
    nearPt.y = 0.0f;
    nearPt.x = x;
}

void WWorldMath::NearestPointLine2D3(const UMath::Vector3 &pt, const UMath::Vector3 &p0, const UMath::Vector3 &p1, UMath::Vector3 &nearPt) {
    const float &x1 = p0.x;
    const float &z1 = p0.z;
    const float &x2 = p1.x;
    const float &z2 = p1.z;
    const float &px = pt.x;
    const float &pz = pt.z;

    float u = (px - x1) * (x2 - x1) + (pz - z1) * (z2 - z1);
    float x_fake = x2 - x1;
    float z_fake = z2 - z1;
    float div = pow2(x_fake) + pow2(z_fake);

    if (1e-6f < div) {
        u = u / div;
    } else {
        u = 0.0f;
    }

    float x = u * (x2 - x1) + x1;
    float z = u * (z2 - z1) + z1;
    nearPt.z = z;
    nearPt.y = 0.0f;
    nearPt.x = x;
}

float WWorldMath::GetPlaneY(const UMath::Vector3 &normal, const UMath::Vector3 &pointOnPlane, const UMath::Vector3 &testPoint) {
    if (normal.y == 0.0f) {
        return pointOnPlane.y;
    }
    return pointOnPlane.y - (normal.x * (testPoint.x - pointOnPlane.x) + normal.z * (testPoint.z - pointOnPlane.z)) / normal.y;
}

bool WWorldMath::IntersectSegPlane(const UMath::Vector3 &P1, const UMath::Vector3 &P2, const UMath::Vector3 &PtOnPlane, const UMath::Vector3 &Normal,
                                   UMath::Vector3 &intersectionPt, float &t) {
    UMath::Vector3 PtOnPlanemP1;
    UMath::Vector3 P2mP1;
    UMath::Sub(PtOnPlane, P1, PtOnPlanemP1);
    UMath::Sub(P2, P1, P2mP1);

    float d = UMath::Dot(Normal, P2mP1);
    if (d == 0.0f) {
        return false;
    }

    float n = UMath::Dot(Normal, PtOnPlanemP1);
    t = n / d;

    UMath::Sub(P2, P1, intersectionPt);
    UMath::ScaleAdd(intersectionPt, t, P1, intersectionPt);

    // return t >= 0.0f && t <= 1.0f;
    // TODO get rid of this var
    bool result = false;
    if (t < 0.0f || (result = true, t > 1.0f)) {
        result = false;
    }
    return result;
}

bool WWorldMath::MakeSegSpaceMatrix(const UMath::Vector3 &startPt, const UMath::Vector3 &endPt, UMath::Matrix4 &mat) {
    if (PtsEqual(startPt, endPt, 0.0001f)) {
        return false;
    }

    UMath::Vector4 &right = mat.v0;
    UMath::Vector4 &up = mat.v1;
    UMath::Vector4 &forward = mat.v2;
    UMath::Vector4 &trans = mat.v3;

    UMath::Sub(startPt, endPt, UMath::Vector4To3(up));
    up.w = 0.0f;

    float rLen = InvSqrt(up.x * up.x + up.y * up.y + up.z * up.z);
    up.x = up.x * rLen;
    up.z = up.z * rLen;
    up.y = up.y * rLen;

    if (fabsf(up.y) > 0.707f) {
        right.y = 0.0f;
        right.z = 0.0f;
        right.x = 1.0f;
        right.w = 0.0f;
    } else {
        right.x = 0.0f;
        right.z = 0.0f;
        right.y = 1.0f;
        right.w = 0.0f;
    }

    Crossxyz(right, up, forward);

    forward.w = 0.0f;
    float lensq = forward.x * forward.x + forward.y * forward.y + forward.z * forward.z;
    if (lensq != 0.0f) {
        rLen = InvSqrt(lensq);
    } else {
        rLen = 0.0f;
    }
    forward.x = forward.x * rLen;
    forward.y = forward.y * rLen;
    forward.z = forward.z * rLen;

    Crossxyz(up, forward, right);

    trans = UMath::Vector4Make(startPt, 1.0f);

    return true;
}
