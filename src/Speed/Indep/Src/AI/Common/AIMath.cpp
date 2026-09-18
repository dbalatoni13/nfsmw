#include "Speed/Indep/Src/AI/AIMath.h"
#include "Speed/Indep/Src/World/WWorldMath.h"

namespace AI {

namespace Math {

//
// Returns the angle between the normal n0 at p0 and the direction from p0 to p1.
//

float AngleTo(const UMath::Vector3 &p0, const UMath::Vector3 &n0, const UMath::Vector3 &p1) {
    UMath::Vector3 dir;

    UMath::Sub(p1, p0, dir);
    VU0_v3unit(dir, dir);

    return VU0_ACos(UMath::Clamp(UMath::Dot(dir, n0), -1.0f, 1.0f));
}

//
// Time until pos0 closes the gap to pos1, given both velocities. Zero if never.
//

float TimeToIntercept(const UMath::Vector3 &pos0, const UMath::Vector3 &vel0, const UMath::Vector3 &pos1, const UMath::Vector3 &vel1) {
    UMath::Vector3 delta;

    UMath::Sub(pos1, pos0, delta);

    float distance = UMath::Normalize(delta);

    if (distance <= 0.0f) {
        return 0.0f;
    }

    UMath::Vector3 relativevelocity;

    UMath::Sub(vel0, vel1, relativevelocity);

    float closingspeed = UMath::Dot(relativevelocity, delta);

    if (closingspeed <= 0.0f) {
        return 0.0f;
    }

    return distance / closingspeed;
}

//
// Intersects the segment p0..p1 against the sphere (cen, radius).
//

inline bool SegmentSphereIntersect(const UMath::Vector3 &p0, const UMath::Vector3 &p1, const UMath::Vector3 &cen, const float radius,
                                   UMath::Vector3 &IntersectPoint) {
    UMath::Vector3 dir;

    UMath::Sub(p1, p0, dir);

    float a = UMath::LengthSquare(dir);

    if (a < 0.005f) {
        return false;
    }

    float b = 2.0f * (dir.x * (p0.x - cen.x) + dir.y * (p0.y - cen.y) + dir.z * (p0.z - cen.z));
    float c = UMath::LengthSquare(cen) + UMath::LengthSquare(p0) - 2.0f * UMath::Dot(cen, p0) - radius * radius;

    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f) {
        return false;
    }

    float t = (-b - UMath::Sqrt(discriminant)) / (2.0f * a);

    if (t < 0.0f || t > 1.0f) {
        return false;
    }

    IntersectPoint.x = p0.x + t * dir.x;
    IntersectPoint.y = p0.y + t * dir.y;
    IntersectPoint.z = p0.z + t * dir.z;

    return true;
}

//
// Time until the two XZ discs touch, ignoring height. Zero if they never do.
//

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

    UMath::Vector3 relativevelocity;
    UMath::Vector4 segment[2];

    segment[0] = UMath::Vector4Make(p0, 1.0f);

    UMath::Sub(v0, v1, relativevelocity);
    UMath::ScaleAdd(relativevelocity, timetoimpact, p0, UMath::Vector4To3(segment[1]));

    const UMath::Vector4 &target = UMath::Vector4Make(p1, 1.0f);
    UMath::Vector4 nearestpoint;

    WWorldMath::NearestPointLine2D(target, segment, nearestpoint);

    float distance = UMath::Distancexz(UMath::Vector4To3(nearestpoint), UMath::Vector4To3(target));

    if (distance > rad0 + rad1) {
        return 0.0f;
    }

    return timetoimpact;
}

//
// Integrates the position forward, turning at a constant rate.
//

void PredictPosition(float predictTime, const UMath::Vector3 &position, const UMath::Vector3 &vfwd, float yaw, const UMath::Vector3 &linearVelocity,
                     const float angularVelocity, UMath::Vector3 &result) {
    float dT = UMath::Max(predictTime * 0.1f, 0.1f);

    result = position;

    float speed = UMath::Dot(linearVelocity, vfwd);
    float currentYaw = yaw;

    for (float t = dT; t < predictTime; t += dT) {
        currentYaw += dT * angularVelocity;

        float vx = speed * UMath::Sinr(currentYaw);
        float vz = speed * UMath::Cosr(currentYaw);

        result.x += vx * dT;
        result.z += vz * dT;

        result.y += linearVelocity.y * dT;
    }
}

}; // namespace Math

}; // namespace AI
