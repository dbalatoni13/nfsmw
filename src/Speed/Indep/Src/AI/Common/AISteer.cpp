#include "Speed/Indep/Src/AI/AISteer.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"

namespace AISteer {

void Seek(UMath::Vector3 &result, const UMath::Vector3 &myPos, const UMath::Vector3 &myVel, const UMath::Vector3 &targetPos) {
    UMath::Sub(targetPos, myPos, result);
}

// STRIPPED
void Flee(UMath::Vector3 &result, const UMath::Vector3 &myPos, const UMath::Vector3 &myVel, const UMath::Vector3 &targetPos) {}

void Pursuit(UMath::Vector3 &result, const UMath::Vector3 &myPos, const UMath::Vector3 &myVel, const UMath::Vector3 &targetPos,
             const UMath::Vector3 &targetVel) {
    float timeToIntercept;
    float directionMod;
    UMath::Vector3 targetFuturePos;
    UMath::ScaleAdd(targetVel, 1.0f, targetPos, targetFuturePos);
    Seek(result, myPos, myVel, targetFuturePos);
}

void Ram(UMath::Vector3 &result, const UMath::Vector3 &myPos, float mySpeed, const UMath::Vector3 &targetPos, const UMath::Vector3 &targetVelocity) {
    UMath::Vector3 para = UMath::Vector3Make(targetVelocity.x, 0.0f, targetVelocity.z);
    UMath::Vector3 perp = UMath::Vector3Make(targetVelocity.z, 0.0f, -targetVelocity.x);
    UMath::Vector3 d = targetPos - myPos;

    float t = 0.0f;
    float vlen = UMath::Length(para);
    if (vlen > 1e-5f) {
        UMath::Scale(para, 1.0f / vlen, para);
        UMath::Scale(perp, 1.0f / vlen, perp);

        float dpara = UMath::Dot(para, d);
        float dperp = UMath::Dot(perp, d);
        float a = vlen * vlen - mySpeed * mySpeed;
        float b = dpara * vlen;
        float c = dpara * dpara + dperp * dperp;

        if (UMath::Abs(a) > 1e-5f) {
            float radical = b * b - a * c;
            if (radical >= 0.0f) {
                float root = UMath::Sqrt(radical);
                float t0 = (-b - root) / a;
                float t1 = (-b + root) / a;
                if (t0 >= 0.0f) {
                    if (t1 >= 0.0f) {
                        t = UMath::Min(t0, t1);
                    } else {
                        t = t0;
                    }
                } else if (t1 >= 0.0f) {
                    t = t1;
                }
            }
        } else if (UMath::Abs(b) > 1e-5f) {
            float t0 = -c / (2.0f * b);
            if (t0 > 0.0f) {
                t = t0;
            }
        }
    }

    UMath::Vector3 intersect;
    UMath::ScaleAdd(targetVelocity, t, targetPos, intersect);
    UMath::Vector3 off = intersect - myPos;
    float offlen = UMath::Length(off);
    if (offlen > 1e-5f) {
        UMath::Scale(off, mySpeed / offlen, result);
    } else {
        result = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    }
}

// STRIPPED
void Avoid(UMath::Vector3 &result, const UMath::Vector3 &myPos, const UMath::Vector3 &myVel, float dT, float myRadius, const UMath::Vector3 &objPos,
           float objRadius) {}

// STRIPPED
void Avoid(UMath::Vector3 &result, const UMath::Vector3 &myPos, const UMath::Vector3 &myVel, float dT, const UMath::Vector3 &collNormal) {}

}; // namespace AISteer

void LineSegSeperation(UMath::Vector3 &result, const UMath::Vector3 &h1, const UMath::Vector3 &t1, const UMath::Vector3 &h2,
                       const UMath::Vector3 &t2) {}

namespace AISteer {

void Separation(UMath::Vector3 &result, const UMath::Vector3 &myHead, const UMath::Vector3 &myTail, float myRadius, const UMath::Vector3 &otherHead,
                const UMath::Vector3 &otherTail, float otherRadius) {}

// STRIPPED
void Separation(UMath::Vector3 &result, const UMath::Vector3 &myPos, float myRadius, const UMath::Vector3 &otherPos) {}

void SetupCapsule(const IBody *body, UMath::Vector3 &head, UMath::Vector3 &tail, float &radius) {}

void Seperation(UMath::Vector3 &separation, IBody *my_body, IBody *target_body, float absolute, float relative) {}

void VehicleSeperation(UMath::Vector3 &separation, IVehicle *myvehicle, const AvoidableList &avoidList, float absolute, float relative) {}

// STRIPPED
void Alignment(UMath::Vector3 &result, const UMath::Vector3 &otherForward) {}

// STRIPPED
void Cohesion(UMath::Vector3 &result, const UMath::Vector3 &myPos, const UMath::Vector3 otherPos) {}

// STRIPPED
void Separation(UMath::Vector3 &result, const UMath::Vector3 &myPos, const AvoidableList &irbList) {}

// STRIPPED
void Alignment(UMath::Vector3 &result, const UMath::Vector3 &myForward, const AvoidableList &irbList) {}

// STRIPPED
void Cohesion(UMath::Vector3 &result, const UMath::Vector3 &myPos, const AvoidableList &irbList) {}

float GetDesiredSpeedToTarget(float distToTarget, float targetSpeed) {}

}; // namespace AISteer
