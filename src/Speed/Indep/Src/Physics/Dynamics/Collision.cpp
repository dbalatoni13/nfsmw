// d:/mw/speed/indep/src/Physics/dynamics/Collision.cpp

#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/Src/Physics/Dynamics/Articulation.h"

// Cola de las cadenas de bWare/STL. Va AQUI, no en la SourceList:
// entre las dos mitades cae el "bad_alloc" que cc1plus emite por su
// cuenta, que en el original es el tercer literal del bloque.



namespace Dynamics {
namespace Collision {

// 0.0f pooled by this TU in the shipped binary; a volatile deref keeps the
// compiler loading from the exact shipped pool symbol.

// d:/mw/speed/indep/src/Physics/dynamics/Collision.cpp:15
Friction::State Friction::GetForce(const UVector3 &p, const float impulse, const UVector3 &n, UVector3 &Ff) const {
    float fy = -impulse;
    Ff.x = Ff.y = Ff.z = 0.0f;
    if (!(fy > 0.0f)) {
        return None;
    }
    Ff = p + n * fy;
    float fx = Ff.Magnitude();
    if (fx <= 0.0f) {
        return Static;
    }
    if (fx > fy * mUs) {
        Ff *= -(fy * mUk) / fx;
        return Dynamic;
    }
    Ff.Negate();
    return Static;
}

// d:/mw/speed/indep/src/Physics/dynamics/Collision.cpp:32
Moment::Moment(const UMath::Matrix4 &orientation, float mass, const UMath::Vector3 &inertia, const UMath::Vector3 &cg, const UMath::Vector3 &linearVel,
               const UMath::Vector3 &angularVel, const UMath::Vector3 &position)
    : mInertiaP(inertia), mCG(cg), mLinearVelocity(linearVel), mAngularVelocity(angularVel), mPosition(position), mInertialScale(1.0f, 1.0f, 1.0f),
      mImmobile(false), mBrakingForce(0.0f), mMass(mass), mClosingVelocity(), mSlidingVelocity(), mForce(), mFriction() {
    mFrictionState = Friction::None;
    mElasticity = 0.0f;
    mFixedCG = false;
    UMath::Copy(orientation, mOrientation);
    UMath::Transpose(mOrientation, mOrientationInv);
    mMassInv = 1.0f / mMass;
}

// d:/mw/speed/indep/src/Physics/dynamics/Collision.cpp:64
Moment::Moment(const IEntity *rb)
    : mInertiaP(rb->GetPrincipalInertia()), mCG(rb->GetCenterOfGravity()), mLinearVelocity(rb->GetLinearVelocity()), mAngularVelocity(rb->GetAngularVelocity()),
      mPosition(rb->GetPosition()), mInertialScale(1.0f, 1.0f, 1.0f), mImmobile(rb->IsImmobile()), mBrakingForce(0.0f), mMass(rb->GetMass()),
      mClosingVelocity(), mSlidingVelocity(), mForce(), mFriction() {
    mFrictionState = Friction::None;
    mElasticity = 0.0f;
    mFixedCG = false;
    UMath::Copy(rb->GetRotation(), mOrientation);
    UMath::Transpose(mOrientation, mOrientationInv);
    mMassInv = 1.0f / mMass;
}

void Moment::SetInertia(const UMath::Vector3 &inertiaP) {
    mInertiaP = inertiaP;
}

void Moment::SetMass(float mass) {
    mMass = mass;
    mMassInv = 1.0f / mass;
}

void Moment::SetCG(const UMath::Vector3 &cg) {
    mCG = cg;
}

bool Moment::React(const Plane &plane, int nSteps) {
    int i;
    UVector3 R;
    UVector3 dVf;
    UVector3 dWf;
    UMath::Matrix4 Ic;
    UVector3 normal(plane.normal);
    float nlen = UMath::Sqrt(UMath::LengthSquare(normal));
    if (nlen != 0.0f) {
        UMath::Scale(normal, 1.0f / nlen, normal);
    }

    float mass = mMass;
    float Ft = 0.0f;

    GetMomentumArm(plane.point, R);
    GetPrincipalTensor(Ic);

    UVector3 w(mAngularVelocity);
    UVector3 v(mLinearVelocity);

    for (i = 0; i < nSteps; i++) {
        UVector3 dV_impact;
        UVector3 dV_friction;
        UVector3 dW_impact;
        UVector3 dW_friction;
        UVector3 Vv = v + (w.Cross(R));
        UVector3 P = Vv * mass;
        float impulse;
        impulse = normal.Dot(P);
        if (impulse >= 0.0f) {
            break;
        }
        UVector3 F = normal * -impulse;
        UVector3 Ff;
        Friction::State fstate = plane.friction.GetForce(P, impulse, normal, Ff);
        if (i == 0) {
            mClosingVelocity = normal * UMath::Dot(Vv, normal);
            mSlidingVelocity = Vv - mClosingVelocity;
            mFrictionState = fstate;
            mFriction = Ff;
        } else {
            float t = static_cast<float>(i) / static_cast<float>(nSteps);
            F *= t;
            Ff *= t;
            Ft -= impulse * t;
            ResolveForce(F, R, Ic, dV_impact, dW_impact);
            ResolveForce(Ff, R, Ic, dV_friction, dW_friction);
            UVector3 dW = dW_impact + dW_friction;
            UVector3 dV = dV_impact + dV_friction;
            v += dV;
            w += dW;
            dVf += dV + dV_impact * mElasticity;
            dWf += dW + dW_impact * mElasticity;
        }
    }

    if (i == 0) {
        return false;
    }
    mForce = normal * Ft;
    mLinearVelocity += dVf;
    mAngularVelocity += dWf;
    return true;
}

bool Moment::React(Moment &other, const Plane &plane, int nSteps) {

    Moment *o0 = this;
    Moment *o1 = &other;
    UVector3 R0;
    UVector3 R1;
    UVector3 w0;
    UVector3 w1;
    UVector3 v0;
    UVector3 v1;
    UMath::Matrix4 I0c;
    UMath::Matrix4 I1c;
    int i;
    UVector3 normal(plane.normal);
    normal.Normalize();

    float mass;
    bool o0_immobile;
    bool o1_immobile;
    bool a0 = o0->IsImmobile();
    if (!o0->IsImmobile() || !o1->IsImmobile()) { o0_immobile = 1; if (!a0) o0_immobile = 0; } else o0_immobile = 0;
    if (!o0->IsImmobile() || !o1->IsImmobile()) { o1_immobile = 1; if (!o1->IsImmobile()) o1_immobile = 0; } else o1_immobile = 0;

    if (o0_immobile && o1_immobile) {
        o0_immobile = 0;
        o1_immobile = 0;
    }

    GetMomentumArm(plane.point, R0);
    GetPrincipalTensor(I0c);

    UVector3 Rn0(R0);
    Rn0.Normalize();

    o1->GetMomentumArm(plane.point, R1);
    o1->GetPrincipalTensor(I1c);

    UVector3 Rn1(R1);
    Rn1.Normalize();

    w0 = o0->mAngularVelocity;
    v0 = o0->mLinearVelocity;
    w1 = o1->mAngularVelocity;
    v1 = o1->mLinearVelocity;

    if (o0_immobile) {
        mass = o1->mMass;
    } else if (o1_immobile) {
        mass = o0->mMass;
    } else {
        mass = (o0->mMass * o1->mMass) / (o0->mMass + o1->mMass);
    }

    float Ft = 0.0f;
    UVector3 dVf0;
    UVector3 dWf0;
    UVector3 dVf1;
    UVector3 dWf1;

    for (i = 0; i < nSteps; i++) {
        
        UVector3 Vv0 = v0 + (w0.Cross(R0));
        UVector3 Vv1 = v1 + (w1.Cross(R1));
        UVector3 P = (Vv0 - Vv1) * mass;
        float impulse = P.Dot(normal);
        if (impulse >= 0.0f) {
            break;
        }
        UVector3 F = normal * -impulse;
        UVector3 Ff;
        Friction::State fstate = plane.friction.GetForce(P, impulse, normal, Ff);
        if (i == 0) {
            mClosingVelocity = normal * UMath::Dot(Vv0 - Vv1, normal);
            mSlidingVelocity = Vv0 - Vv1 - mClosingVelocity;
            mFrictionState = fstate;
            mFriction = Ff;
            if (mBrakingForce > 0.0f) {
                if (-impulse > mBrakingForce) {
                    mImmobile = false;
                    o0_immobile = false;
                    mass = (mMass * other.mMass) / (mMass + other.mMass);
                }
            }
            other.mClosingVelocity = normal * UMath::Dot(Vv1 - Vv0, normal);
            other.mSlidingVelocity = Vv1 - Vv0 - other.mClosingVelocity;
            other.mFrictionState = fstate;
            other.mFriction = -1.0f * Ff;
            if (other.mBrakingForce > 0.0f) {
                if (-impulse > other.mBrakingForce) {
                    o1_immobile = false;
                    other.mImmobile = false;
                    mass = (mMass * other.mMass) / (mMass + other.mMass);
                }
            }
            continue;
        }
        float t = static_cast<float>(i) / static_cast<float>(nSteps);
        F *= t;
        Ff *= t;
        Ft -= impulse * t;
        if (!o0_immobile) {
            UVector3 dV_impact;
            UVector3 dV_friction;
            UVector3 dW_impact;
            UVector3 dW_friction;
            ResolveForce(F, R0, I0c, dV_impact, dW_impact);
            ResolveForce(Ff, R0, I0c, dV_friction, dW_friction);
            UVector3 dV0 = dV_impact + dV_friction;
            UVector3 dW0 = dW_impact + dW_friction;
            if (!mFixedCG) {
                v0 += dV0;
                dVf0 += dV0 + dV_impact * mElasticity;
            }
            w0 += dW0;
            dWf0 += dW0 + dW_impact * mElasticity;
        }
        if (!o1_immobile) {
            UVector3 dV_impact;
            UVector3 dV_friction;
            UVector3 dW_impact;
            UVector3 dW_friction;
            F.Negate();
            Ff.Negate();
            o1->ResolveForce(F, R1, I1c, dV_impact, dW_impact);
            o1->ResolveForce(Ff, R1, I1c, dV_friction, dW_friction);
            UVector3 dV1 = dV_impact + dV_friction;
            UVector3 dW1 = dW_impact + dW_friction;
            if (!o1->mFixedCG) {
                v1 += dV1;
                dVf1 += dV1 + dV_impact * o1->mElasticity;
            }
            w1 += dW1;
            dWf1 += dW1 + dW_impact * o1->mElasticity;
        }
    }

    if (i == 0) {
        return false;
    }
    mForce = normal * Ft;
    other.mForce = normal * -Ft;
    mLinearVelocity += dVf0;
    mAngularVelocity += dWf0;
    other.mLinearVelocity += dVf1;
    other.mAngularVelocity += dWf1;
    return true;
}

bool Moment::React(Moment &other, const Joint &joint, int nSteps) {

    Moment *o0 = this;
    Moment *o1 = &other;
    UVector3 R0;
    UVector3 R1;
    UVector3 w0;
    UVector3 w1;
    UVector3 v0;
    UVector3 v1;
    UMath::Matrix4 I0;
    UMath::Matrix4 I1;
    int i;
    float mass;
    bool o0_immobile;
    bool o1_immobile;

    bool a0 = o0->IsImmobile();
    if (!o0->IsImmobile() || !o1->IsImmobile()) { o0_immobile = 1; if (!a0) o0_immobile = 0; } else o0_immobile = 0;
    if (!o0->IsImmobile() || !o1->IsImmobile()) { o1_immobile = 1; if (!o1->IsImmobile()) o1_immobile = 0; } else o1_immobile = 0;

    if (o0_immobile && o1_immobile) {
        o0_immobile = 0;
        o1_immobile = 0;
    }

    GetMomentumArm(joint.p, R0);
    GetParallelTensor(R0, I0);

    other.GetMomentumArm(joint.p, R1);
    other.GetParallelTensor(R1, I1);

    w0 = mAngularVelocity;
    v0 = mLinearVelocity;
    w1 = other.mAngularVelocity;
    v1 = other.mLinearVelocity;

    if (o0_immobile) {
        mass = other.mMass;
    } else if (o1_immobile) {
        mass = mMass;
    } else {
        mass = (mMass * other.mMass) / (mMass + other.mMass);
    }

    UVector3 dVf0;
    UVector3 dVf1;
    UVector3 dWf0;
    UVector3 dWf1;

    for (i = 0; i < nSteps; i++) {
        UVector3 Vv0 = v0 + (w0.Cross(R0));
        UVector3 Vv1 = v1 + (w1.Cross(R1));
        UVector3 P = (Vv0 - Vv1) * mass;
        float impulse = P.Magnitude();
        if (impulse == 0.0f) {
            break;
        }
        UVector3 F = P * -1.0f;
        if (i == 0) {
            mForce = F;
            mClosingVelocity = Vv0 - Vv1;
            other.mForce = F * -1.0f;
            other.mClosingVelocity = (Vv0 - Vv1) * -1.0f;
        } else {
            float t = static_cast<float>(i) / static_cast<float>(nSteps);
            F *= t;
            if (!o0_immobile) {
                UVector3 dV_impact;
                UVector3 dW_impact;
                UMath::Scale(F, mMassInv, dV_impact);
                UMath::Cross(R0, F, dW_impact);
                UMath::Rotate(dW_impact, I0, dW_impact);
                v0 += dV_impact;
                w0 += dW_impact;
                dVf0 += dV_impact;
                dWf0 += dW_impact;
            }
            if (!o1_immobile) {
                UVector3 dV_impact;
                UVector3 dW_impact;
                F.Negate();
                UMath::Scale(F, other.mMassInv, dV_impact);
                UMath::Cross(R1, F, dW_impact);
                UMath::Rotate(dW_impact, I1, dW_impact);
                v1 += dV_impact;
                w1 += dW_impact;
                dVf1 += dV_impact;
                dWf1 += dW_impact;
            }
        }
    }

    if (i == 0) {
        return false;
    }
    mLinearVelocity += dVf0;
    mAngularVelocity += dWf0;
    other.mLinearVelocity += dVf1;
    other.mAngularVelocity += dWf1;
    return true;
}

bool Moment::React(const Joint &joint, int nSteps) {
    Moment *o0 = this;
    UVector3 R0;
    UVector3 w0;
    UVector3 v0;
    UMath::Matrix4 I0;
    int i;
    float mass;

    GetMomentumArm(joint.p, R0);
    GetParallelTensor(R0, I0);

    w0 = mAngularVelocity;
    v0 = mLinearVelocity;
    mass = mMass;

    UVector3 dVf0;
    UVector3 dWf0;

    for (i = 0; i < nSteps; i++) {
        UVector3 Vv0 = v0 + (w0.Cross(R0));
        UVector3 P = Vv0 * mass;
        float impulse = P.Magnitude();
        if (impulse == 0.0f) {
            break;
        }
        UVector3 F = P * -1.0f;
        if (i == 0) {
            mForce = F;
            mClosingVelocity = Vv0 * -1.0f;
        } else {
            float t = static_cast<float>(i) / static_cast<float>(nSteps);
            F *= t;
            UVector3 dV_impact;
            UVector3 dW_impact;
            ResolveForce(F, R0, I0, dV_impact, dW_impact);
            v0 += dV_impact;
            w0 += dW_impact;
            dVf0 += dV_impact;
            dWf0 += dW_impact;
        }
    }

    if (i == 0) {
        return false;
    }
    mLinearVelocity += dVf0;
    mAngularVelocity += dWf0;
    return true;
}

}; // namespace Collision
}; // namespace Dynamics
