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
    if (distance != 0.0f && distance > 0.0f) {
        UMath::Vector3 relSpeed;
        UMath::Sub(v0, v1, relSpeed);
        float closingspeed = UMath::Dot(relSpeed, dir);
        if (closingspeed > 0.0f) {
            timetointercept = distance / closingspeed;
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

}; // namespace Math

}; // namespace AI
