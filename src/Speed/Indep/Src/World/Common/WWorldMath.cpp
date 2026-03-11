#include "Speed/Indep/Src/World/WWorldMath.h"

float WWorldMath::GetPlaneY(const UMath::Vector3 &normal, const UMath::Vector3 &pointOnPlane, const UMath::Vector3 &testPoint) {
    if (normal.y == 0.0f) {
        return pointOnPlane.y;
    }
    return pointOnPlane.y - (normal.x * (testPoint.x - pointOnPlane.x) + normal.z * (testPoint.z - pointOnPlane.z)) / normal.y;
}

void WWorldMath::NearestPointLine2D3(const UMath::Vector3 &pt, const UMath::Vector3 &p0, const UMath::Vector3 &p1, UMath::Vector3 &nearPt) {
    const float &x1 = p0.x;
    const float &z1 = p0.z;
    const float &x2 = p1.x;
    const float &z2 = p1.z;
    const float &px = pt.x;
    const float &pz = pt.z;
    float z = z2 - z1;
    float x = x2 - x1;
    float div = pow2(x) + pow2(z);
    float u = (px - x1) * x + (pz - z1) * z;
    if (0.0f < div) {
        u = u / div;
    } else {
        u = 0.0f;
    }
    nearPt.y = 0.0f;
    nearPt.z = u * (z2 - z1) + z1;
    nearPt.x = u * (x2 - x1) + x1;
}

void WWorldMath::NearestPointLine2D(const UMath::Vector4 &pt, const UMath::Vector4 *line, UMath::Vector4 &nearPt) {
    const float &x1 = line[0].x;
    const float &z1 = line[0].z;
    const float &x2 = line[1].x;
    const float &z2 = line[1].z;
    const float &px = pt.x;
    const float &pz = pt.z;
    float z = z2 - z1;
    float x = x2 - x1;
    float div = pow2(x) + pow2(z);
    float u = (px - x1) * x + (pz - z1) * z;
    if (0.0f < div) {
        u = u / div;
    } else {
        u = 0.0f;
    }
    nearPt.y = 0.0f;
    nearPt.z = u * (z2 - z1) + z1;
    nearPt.x = u * (x2 - x1) + x1;
}

bool WWorldMath::IntersectSegPlane(const UMath::Vector3 &P1, const UMath::Vector3 &P2, const UMath::Vector3 &PtOnPlane, const UMath::Vector3 &Normal, UMath::Vector3 &intersectionPt, float &t) {
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
    bool result = true;
    if (t < 0.0f || t > 1.0f) {
        result = false;
    }
    return result;
}

bool WWorldMath::SegmentIntersect(const UMath::Vector4 *line1, const UMath::Vector4 *line2, UMath::Vector4 *intersectPt) {
    const float l1x = line1[1].x - line1[0].x;
    const float l1z = line1[1].z - line1[0].z;
    const float x11 = line1[0].x;
    const float z11 = line1[0].z;
    const float l2x = line2[1].x - line2[0].x;
    const float l2z = line2[1].z - line2[0].z;
    const float x22 = line2[0].x;
    const float z22 = line2[0].z;
    const float ua_d = l2z * l1x - l2x * l1z;
    if (ua_d == 0.0f) {
        return false;
    }
    const float z12 = z11 - z22;
    const float x12 = x11 - x22;
    const float ua_n = l2x * z12 - l2z * x12;
    if ((ua_n >= 0.0f && ua_n <= ua_d) || (ua_n <= 0.0f && ua_d <= ua_n)) {
        const float ub_n = l1x * z12 - l1z * x12;
        if ((ub_n >= 0.0f && ub_n <= ua_d) || (ub_n <= 0.0f && ua_d <= ub_n)) {
            if (intersectPt != nullptr) {
                float t = ua_n / ua_d;
                intersectPt->x = t * l1x + x11;
                intersectPt->w = 1.0f;
                intersectPt->z = t * l1z + z11;
                intersectPt->y = t * (line1[1].y - line1[0].y) + line1[0].y;
            }
            return true;
        }
    }
    return false;
}
