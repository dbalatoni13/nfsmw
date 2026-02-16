#include "Speed/Indep/Src/AI/AISteer.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/AI/AIAvoidable.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

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
                       const UMath::Vector3 &t2) {
    UMath::Vector3 u = t1 - h1;
    UMath::Vector3 v = t2 - h2;
    UMath::Vector3 w = h1 - h2;

    float a = UMath::Dot(u, u);
    float b = UMath::Dot(u, v);
    float c = UMath::Dot(v, v);
    float d = UMath::Dot(u, w);
    float e = UMath::Dot(v, w);

    float D = a * c - b * b;
    const float small_number = 1e-20f;

    float sN;
    float sD = D;
    float tN;
    float tD = D;

    if (D < small_number) {
        sN = 0.0f;
        tN = e;
        sD = 1.0f;
        tD = c;
    } else {
        sN = b * e - c * d;
        tN = a * e - b * d;
        if (sN < 0.0f) {
            sN = 0.0f;
            tN = e;
            tD = c;
        } else if (sN > D) {
            sN = D;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0f) {
        tN = 0.0f;
        if (-d < 0.0f) {
            sN = 0.0f;
        } else if (-d > a) {
            sN = sD;
        } else {
            sN = -d;
            sD = a;
        }
    } else if (tN > tD) {
        tN = tD;
        if (b - d < 0.0f) {
            sN = 0.0f;
        } else if (b - d > a) {
            sN = sD;
        } else {
            sN = b - d;
            sD = a;
        }
    }

    float sc = UMath::Abs(sN) >= small_number ? sN / sD : 0.0f;
    float tc = UMath::Abs(tN) >= small_number ? tN / tD : 0.0f;

    result = (w + sc * u) - (tc * v);
}

namespace AISteer {

void Separation(UMath::Vector3 &result, const UMath::Vector3 &myHead, const UMath::Vector3 &myTail, float myRadius, const UMath::Vector3 &otherHead,
                const UMath::Vector3 &otherTail, float otherRadius) {
    UMath::Vector3 seperation;
    LineSegSeperation(seperation, myHead, myTail, otherHead, otherTail);
    float d2 = UMath::Dot(seperation, seperation);
    float r2 = (myRadius + otherRadius) * (myRadius + otherRadius);

    UMath::ScaleAdd(seperation, r2 / UMath::Max(d2 * bSqrt(d2), 0.1f), result, result);
}

// STRIPPED
void Separation(UMath::Vector3 &result, const UMath::Vector3 &myPos, float myRadius, const UMath::Vector3 &otherPos) {}

void SetupCapsule(const IBody *body, UMath::Vector3 &head, UMath::Vector3 &tail, float &radius) {
    UMath::Vector3 axis;
    UMath::Vector3 position;
    UMath::Vector3 dimension;
    UMath::Matrix4 matrix;

    body->GetDimension(dimension);
    body->GetTransform(matrix);
    position = UMath::Vector4To3(matrix.v3);

    float width = dimension.x;
    float length = dimension.z;
    if (length > width) {
        axis = UMath::Vector4To3(matrix.v2);
    } else {
        axis = UMath::Vector4To3(matrix.v0);
    }

    float a = UMath::Min(length, width);
    float b = UMath::Max(length, width);
    float c = a * a / b;
    float d = b - c;

    UMath::ScaleAdd(axis, d, position, head);
    UMath::ScaleAdd(axis, -d, position, tail);
    radius = UMath::Sqrt(c * c + a * a);
}

void Seperation(UMath::Vector3 &separation, IBody *my_body, IBody *target_body, float absolute, float relative) {
    float my_radius;
    UMath::Vector3 my_head;
    UMath::Vector3 my_tail;
    SetupCapsule(my_body, my_head, my_tail, my_radius);

    float target_radius;
    UMath::Vector3 target_head;
    UMath::Vector3 target_tail;
    SetupCapsule(target_body, target_head, target_tail, target_radius);

    Separation(separation, my_head, my_tail, my_radius, target_head, target_tail, target_radius);

    UMath::Vector3 target_velocity;
    target_body->GetLinearVelocity(target_velocity);

    UMath::Vector3 my_velocity;
    my_body->GetLinearVelocity(my_velocity);

    UMath::Vector3 my_position;
    UMath::Scale(my_head + UVector3(my_tail), 0.5f, my_position);

    UMath::Vector3 target_position;
    UMath::Scale(target_head + UVector3(target_tail), 0.5f, target_position);

    float scale = absolute;
    float denom = UMath::LengthSquare(my_velocity - target_velocity);
    if (denom > 1e-5f || denom < -1e5f) {
        float closest_time = -UMath::Dot(my_position - target_position, my_velocity - target_velocity) / denom;
        if (closest_time > 0.05f) {
            UMath::Vector3 my_closest;
            UMath::ScaleAdd(my_velocity, closest_time, my_position, my_closest);
            UMath::Vector3 target_closest;
            UMath::ScaleAdd(target_velocity, closest_time, target_position, target_closest);

            float closest_distance = UMath::LengthSquare(my_closest - target_closest);
            float denom = closest_distance * 0.5f + closest_time;
            float cscale = denom * denom;

            scale += relative / cscale;
        }
    }

    float seperation_length = UMath::Length(separation);
    if (seperation_length > 1e-5f) {
        UMath::Scale(separation, UMath::Min(scale, KPH2MPS(160.0f) / seperation_length), separation);
    }
}

void VehicleSeperation(UMath::Vector3 &separation, IVehicle *myvehicle, const AvoidableList &avoidList, float absolute, float relative) {
    IBody *my_body;
    if (myvehicle->QueryInterface(&my_body)) {
        for (AvoidableList::const_iterator iter = avoidList.begin(); iter != avoidList.end(); ++iter) {
            AIAvoidable *av = *iter;
            IBody *target_body;
            if (av->QueryInterface(&target_body)) {
                IVehicle *iv;
                if (av->QueryInterface(&iv)) {
                    UMath::Vector3 vseparation = UMath::Vector3::kZero;
                    Seperation(vseparation, my_body, target_body, absolute, relative);
                    UMath::Add(separation, vseparation, separation);
                }
            }
        }
    }
}

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

// Functionally matching
float GetDesiredSpeedToTarget(float distToTarget, float targetSpeed) {
    float desiredSpeed;
    if (distToTarget < 0.0f) {
        desiredSpeed = UMath::Max(distToTarget * 0.5f + targetSpeed, MPH2MPS(10.0f));
    } else {
        desiredSpeed = UMath::Min(distToTarget * 0.5f + targetSpeed, UMath::Max(MPH2MPS(25.0f), targetSpeed * 1.2f));
    }
    return desiredSpeed;
}

}; // namespace AISteer
