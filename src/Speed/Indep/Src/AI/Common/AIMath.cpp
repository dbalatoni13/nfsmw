#include "Speed/Indep/Src/AI/AIMath.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

#include <cmath>

namespace AI {

namespace Math {

float AngleTo(const UMath::Vector3 &p0, const UMath::Vector3 &n0, const UMath::Vector3 &p1) {
    UMath::Vector3 dir;
    UMath::Sub(p1, p0, dir);
    UMath::Unit(dir, dir);
    float dot = UMath::Dot(dir, n0);
    dot = UMath::Clamp(dot, -1.0f, 1.0f);
    return RAD2DEG(acosf(dot));
}

float TimeToIntercept(const UMath::Vector3 &p0, const UMath::Vector3 &v0, const UMath::Vector3 &p1, const UMath::Vector3 &v1) {
    UMath::Vector3 dir;
    UMath::Sub(p1, p0, dir);
    float distance = UMath::Normalize(dir);
    float timetointercept = 0.0f;
    if (distance != 0.0f) {
        if (distance > 0.0f) {
            UMath::Vector3 relSpeed;
            UMath::Sub(v0, v1, relSpeed);
            float closingspeed = UMath::Dot(relSpeed, dir);
            if (closingspeed > 0.0f) {
                return distance / closingspeed;
            }
        }
    }
    return timetointercept;
}

void PredictPosition(float predictTime, const UMath::Vector3 &position, const UMath::Vector3 &vfwd, float yaw, const UMath::Vector3 &linearVelocity,
                     const float angularVelocity, UMath::Vector3 &result) {
    float dT = UMath::Max(predictTime, 0.05f);
    result = position;
    float speed = UMath::Dot(linearVelocity, vfwd);
    if (dT < predictTime) {
        float step = 0.05f;
        float theta = yaw;
        do {
            theta = theta + dT * angularVelocity;
            float velX = speed * UMath::Sinr(theta);
            float velZ = speed * UMath::Cosr(theta);
            result.x += velX * step;
            result.z += velZ * step;
            dT += step;
        } while (dT < predictTime);
    }
}

bool SegmentSphereIntersect(const UMath::Vector3 &p0, const UMath::Vector3 &p1, const UMath::Vector3 &cen, float radius,
                           UMath::Vector3 &IntersectPoint) {
    UMath::Vector3 d;
    UMath::Sub(p1, p0, d);
    float a = UMath::LengthSquare(d);
    if (a < 0.0001f) {
        return false;
    }

    float halfb = d.z * (p0.z - cen.z) + d.x * (p0.x - cen.x) + d.y * (p0.y - cen.y);
    float b = halfb + halfb;

    float lenSqCen = UMath::LengthSquare(cen);
    float lenSqP0 = UMath::LengthSquare(p0);
    float dotCenP0 = UMath::Dot(cen, p0);
    float c = ((lenSqCen + lenSqP0) - (dotCenP0 + dotCenP0) - radius * radius) * 4.0f;

    float D = b * b - a * c;
    if (D < 0.0f) {
        return false;
    }

    float sqrtD = UMath::Sqrt(D);
    float t = (-b - sqrtD) / (a + a);

    if (t >= 0.0f && t <= 1.0f) {
        IntersectPoint.x = t * d.x + p0.x;
        IntersectPoint.y = t * d.y + p0.y;
        IntersectPoint.z = t * d.z + p0.z;
        return true;
    }

    return false;
}

namespace WWorldMath {
void NearestPointLine2D(const UMath::Vector4 &pt, const UMath::Vector4 *line, UMath::Vector4 &nearPt);
}; // namespace WWorldMath

float TimeToImpactXZ(const UMath::Vector3 &pos0, const UMath::Vector3 &vel0, float rad0,
                     const UMath::Vector3 &pos1, const UMath::Vector3 &vel1, float rad1) {
    UMath::Vector3 p0;
    memset(&p0, 0, sizeof(p0));
    p0.x = pos0.x;
    p0.z = pos0.z;

    UMath::Vector3 p1;
    memset(&p1, 0, sizeof(p1));
    p1.x = pos1.x;
    p1.z = pos1.z;

    UMath::Vector3 v0;
    memset(&v0, 0, sizeof(v0));
    v0.x = vel0.x;
    v0.z = vel0.z;

    UMath::Vector3 v1;
    memset(&v1, 0, sizeof(v1));
    v1.x = vel1.x;
    v1.z = vel1.z;

    float timetoimpact = TimeToIntercept(p0, v0, p1, v1);

    if (timetoimpact < 999.0f) {
        UMath::Vector3 relSpeed;
        UMath::Sub(v0, v1, relSpeed);

        UMath::Vector4 line[2];
        line[0] = UMath::Vector4Make(p0, 0.0f);

        UMath::ScaleAdd(relSpeed, timetoimpact, p0, UMath::Vector4To3(line[1]));
        line[1].w = 0.0f;

        UMath::Vector4 test = UMath::Vector4Make(p1, 0.0f);
        UMath::Vector4 nearpt = UMath::Vector4Make(p1, 0.0f);

        WWorldMath::NearestPointLine2D(test, line, nearpt);

        float lateraldistance = UMath::Distancexz(UMath::Vector4To3(nearpt), UMath::Vector4To3(test));
        if (lateraldistance <= rad0 + rad1) {
            return timetoimpact;
        }
    }

    return 999.0f;
}

}; // namespace Math

}; // namespace AI
