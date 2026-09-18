#include "Speed/Indep/Src/Physics/Explosion.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplodeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

inline HMODEL Explosion::GetSource() const {
    return mSource;
}
inline float Explosion::GetExpansionSpeed() const {
    return mExpansionSpeed;
}
inline float Explosion::GetMaximumRadius() const {
    return mExpansionRadius;
}

inline const UMath::Vector3 &Explosion::GetOrigin() const {
    return mRigidBody ? mRigidBody->GetPosition() : UMath::Vector3::kZero;
}
inline bool Explosion::HasDamage() const {
    return mDamages;
}
inline unsigned int Explosion::GetTargets() const {
    return mTargets;
}

inline void Explosion::SetCausality(HCAUSE from, float time) {
    mCausality = from;
    mCauseTime = time;
}
inline HCAUSE Explosion::GetCausality() const {
    return mCausality;
}
inline float Explosion::GetCausalityTime() const {
    return mCauseTime;
}

inline IModel *Explosion::GetModel() {
    return NULL;
}
inline const IModel *Explosion::GetModel() const {
    return NULL;
}

ISimable *Explosion::Construct(Sim::Param params) {
    ExplosionParams ep = params.Fetch<ExplosionParams>(UCrc32(0xa6b47fac));

    if (Sim::CanSpawnSimpleRigidBody(ep.fPosition, true)) {
        return new Explosion(ep, params);
    }

    return NULL;
}

BIND_PHYSICS_FACTORY(Explosion);

Explosion::Explosion(const ExplosionParams &params, Sim::Param sp)
    : PhysicsObject("explosion", "default", SIMABLE_EXPLOSION, NULL, 0), //
      IExplosion(this),                                                  //
      mExpansionSpeed(params.fExpansionSpeed),                           //
      mExpansionRadius(params.fRadius),                                  //
      mSource(params.fSource),                                           //
      mIRBSimple(NULL),                                                  //
      mEffectSource(params.fEffectSource),                               //
      mCausality(NULL),                                                  //
      mCauseTime(0.0f),                                                  //
      mDamages(params.fDamage),                                          //
      mTargets(params.fTargets) {
    LoadBehavior(BEHAVIOR_MECHANIC_RIGIDBODY, UCrc32("SimpleRigidBody"),
                 RBSimpleParams(params.fPosition, UMath::Vector3::kZero, UMath::Vector3::kZero, UMath::Matrix4::kIdentity,
                                UMath::Max(params.fStartRadius, 0.01f), 1.0f));

    IModel *model = IModel::FindInstance(mSource);
    if (model) {
        mCausality = model->GetCausality();
        mCauseTime = model->GetCausalityTime();
    }
}

Explosion::~Explosion() {
}

void Explosion::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        if (this->ISimable::QueryInterface(&mIRBSimple)) {
            mIRBSimple->ModifyFlags(0, 0xC100);
        }
    }

    PhysicsObject::OnBehaviorChange(mechanic);
}

void Explosion::OnCollide(IRigidBody *other, float dT, float radius, const Dynamics::Collision::Geometry &explosion_sphere) {
    IExplodeable *explodeable;

    if (other->QueryInterface(&explodeable)) {
        float total_radius = radius + other->GetRadius();

        if (UMath::DistanceSquare(other->GetPosition(), explosion_sphere.GetPosition()) < total_radius * total_radius) {
            UMath::Vector3 dim;
            other->GetDimension(dim);
            UMath::Matrix4 matrix;
            other->GetMatrix4(matrix);
            Dynamics::Collision::Geometry box(matrix, other->GetPosition(), dim, Dynamics::Collision::Geometry::BOX, UMath::Vector3::kZero);

            if (Dynamics::Collision::Geometry::FindIntersection(&box, &explosion_sphere, &box)) {
                explodeable->OnExplosion(box.GetCollisionNormal(), box.GetCollisionPoint(), dT, this);
            }
        }
    }
}

float Explosion::GetRadius() const {
    const IRigidBody *irb = GetRigidBody();

    if (irb) {
        return irb->GetRadius();
    }
    return 0.0f;
}

void Explosion::TestCollisions(float dT) {
    if (mIRBSimple) {
        SimCollisionMap *cmap = mIRBSimple->GetCollisionMap();

        if (cmap && cmap->CollisionWithAny()) {
            IRigidBody *mybody = GetRigidBody();
            float radius = mybody->GetRadius();
            UVector3 mydim(radius, radius, radius);
            Dynamics::Collision::Geometry explosion_sphere(UMath::Matrix4::kIdentity, mybody->GetPosition(), mydim,
                                                           Dynamics::Collision::Geometry::SPHERE, UMath::Vector3::kZero);

            for (int i = 0; i < Sim::MaxAnyBodies; i++) {
                if (cmap->CollisionWithOrderedBody(i)) {
                    IRigidBody *irb = cmap->GetOrderedBody(i);

                    if (irb) {
                        if (!mEffectSource) {
                            IRenderable *irenderable;

                            if (irb->QueryInterface(&irenderable)) {
                                if (irenderable->GetModelHandle() == mSource) {
                                    continue;
                                }
                            }
                        }

                        OnCollide(irb, dT, radius, explosion_sphere);
                    }
                }
            }
        }
    }
}

void Explosion::OnTaskSimulate(float dT) {
    IRigidBody *irb = GetRigidBody();

    TestCollisions(dT);

    float radius = irb->GetRadius();

    radius += mExpansionSpeed * dT;

    if (radius > mExpansionRadius) {
        Kill();
    } else {
        irb->SetRadius(radius);
    }
}
