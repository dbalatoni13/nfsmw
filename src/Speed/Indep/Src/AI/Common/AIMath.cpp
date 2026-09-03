#include "Speed/Indep/Src/AI/AIMath.h"
#include "Speed/Indep/Src/World/WWorldMath.h"

namespace AI {

namespace Math {

float AngleTo(const UMath::Vector3 &p0, const UMath::Vector3 &n0, const UMath::Vector3 &p1) {
    UMath::Vector3 dir;
    UMath::Sub(p1, p0, dir);
    UMath::Unit(dir);
    float dot = UMath::Clamp(UMath::Dot(dir, n0), -1.0f, 1.0f);
    return UMath::ACosa(dot);
}

float TimeToIntercept(const UMath::Vector3 &p0, const UMath::Vector3 &v0, const UMath::Vector3 &p1, const UMath::Vector3 &v1) {
    UMath::Vector3 dir;
    UMath::Sub(p1, p0, dir);
    float distance = UMath::Normalize(dir);
    if (distance <= 0.0f) {
        return 0.0f;
    }

    UMath::Vector3 relSpeed;
    UMath::Sub(v0, v1, relSpeed);
    float closingspeed = UMath::Dot(relSpeed, dir);
    if (closingspeed <= 0.0f) {
        return 0.0f;
    }

    float timetointercept = distance / closingspeed;
    return timetointercept;
}

float TimeToImpactXZ(const UMath::Vector3 &pos0, const UMath::Vector3 &vel0, const float rad0, const UMath::Vector3 &pos1, const UMath::Vector3 &vel1,
                     const float rad1) {
    UMath::Vector3 p0 = {pos0.x, 0.0f, pos0.z};
    UMath::Vector3 p1 = {pos1.x, 0.0f, pos1.z};
    UMath::Vector3 v0 = {vel0.x, 0.0f, vel0.z};
    UMath::Vector3 v1 = {vel1.x, 0.0f, vel1.z};
    float timetoimpact = TimeToIntercept(p0, v0, p1, v1);

    if (timetoimpact <= 0.0f) {
        return 0.0f;
    }

    UMath::Vector4 line[2];
    line[0] = UMath::Vector4Make(p0, 1.0f);

    UMath::Vector3 relSpeed;
    UMath::Sub(v0, v1, relSpeed);
    UMath::ScaleAdd(relSpeed, timetoimpact, p0, UMath::Vector4To3(line[1]));

    UMath::Vector4 test = UMath::Vector4Make(p1, 1.0f);
    UMath::Vector4 nearpt;
    WWorldMath::NearestPointLine2D(test, line, nearpt);

    float lateraldistance = UMath::Distancexz(test, nearpt);
    if (lateraldistance > rad0 + rad1) {
        return 0.0f;
    }
    return timetoimpact;
}

// STRIPPED
float TimeToImpact(const UMath::Vector3 &p0, const UMath::Vector3 &v0, const float rad0, const UMath::Vector3 &p1, const UMath::Vector3 &v1,
                   const float rad1) {
    return 0.0f;
}

void PredictPosition(float predictTime, const UMath::Vector3 &position, const UMath::Vector3 &vfwd, float yaw, const UMath::Vector3 &linearVelocity,
                     const float angularVelocity, UMath::Vector3 &result) {
    float dT = UMath::Max(predictTime * 0.1f, 0.1f);
    result = position;

    float speed = UMath::Dot(linearVelocity, vfwd);

    float theta = yaw;
    for (float step = dT; step < predictTime; step += dT) {
        theta = theta + dT * angularVelocity;

        float velX = speed * UMath::Sinr(theta);
        float velZ = speed * UMath::Cosr(theta);

        result.x += velX * dT;
        result.z += velZ * dT;
        result.y += linearVelocity.y * dT;
    }
}

bool SegmentSphereIntersect(const UMath::Vector3 &p0, const UMath::Vector3 &p1, const UMath::Vector3 &cen, const float radius,
                            UMath::Vector3 &IntersectPoint) {
    UMath::Vector3 d;
    UMath::Sub(p1, p0, d);
    float a = UMath::LengthSquare(d);
    if (a < 0.005f) {
        return false;
    }

    float b = 2.0f * (d.x * (p0.x - cen.x) + d.y * (p0.y - cen.y) + d.z * (p0.z - cen.z));
    float c = UMath::LengthSquare(cen) + UMath::LengthSquare(p0) - 2.0f * UMath::Dot(cen, p0) - radius * radius;
    float D = b * b - 4.0f * a * c;
    if (D < 0.0f) {
        return false;
    }

    float t = (-b - UMath::Sqrt(D)) / (2.0f * a);
    if (t < 0.0f) {
        return false;
    }

    if (t <= 1.0f) {
        IntersectPoint.x = p0.x + t * d.x;
        IntersectPoint.y = p0.y + t * d.y;
        IntersectPoint.z = p0.z + t * d.z;
        return true;
    }

    return false;
}

}; // namespace Math

}; // namespace AI
