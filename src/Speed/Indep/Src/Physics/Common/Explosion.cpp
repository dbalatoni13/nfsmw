#include "Speed/Indep/Src/Physics/Explosion.h"

#include "Speed/Indep/Src/Interfaces/Simables/IExplodeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Physics/Behaviors/SimpleRigidBody.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"

namespace Sim {
bool CanSpawnSimpleRigidBody(const UMath::Vector3 &position, bool highPriority);
}

static inline ISimpleBody *FindSimpleBody(ISimable *owner) {
    return reinterpret_cast<ISimpleBody *>(
        (*reinterpret_cast<UTL::COM::Object **>(owner))->_mInterfaces.Find((HINTERFACE)ISimpleBody::_IHandle)
    );
}

UTL::COM::Factory<Sim::Param, ISimable, UCrc32>::Prototype _Explosion("Explosion", Explosion::Construct);

ISimable *Explosion::Construct(Sim::Param params) {
    const ExplosionParams ep(params.Fetch<ExplosionParams>(UCrc32(0xa6b47fac)));
    if (Sim::CanSpawnSimpleRigidBody(ep.fPosition, true)) {
        return new Explosion(ep, params);
    }
    return nullptr;
}

Explosion::Explosion(const ExplosionParams &params, Sim::Param sp)
    : PhysicsObject("explosion", "default", SIMABLE_EXPLOSION, nullptr, 0) //
    , IExplosion(this) //
    , mExpansionSpeed(params.fExpansionSpeed) //
    , mExpansionRadius(params.fRadius) //
    , mSource(params.fSource) //
    , mDamages(params.fDamage) //
    , mTargets(params.fTargets)
{
    mIRBSimple = nullptr;
    mCausality = nullptr;
    mCauseTime = 0.0f;
    mEffectSource = params.fEffectSource;

    float start_radius = UMath::Max(0.0f, params.fStartRadius);
    LoadBehavior(
        UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY), //
        UCrc32("SimpleRigidBody"), //
        RBSimpleParams(
            params.fPosition, //
            UMath::Vector3::kZero, //
            UMath::Vector3::kZero, //
            UMath::Matrix4::kIdentity, //
            start_radius, //
            1.0f
        )
    );

    IModel *model = IModel::FindInstance(mSource);
    if (model) {
        mCausality = model->GetCausality();
        mCauseTime = model->GetCausalityTime();
    }
}

Explosion::~Explosion() {}

void Explosion::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == UCrc32(BEHAVIOR_MECHANIC_RIGIDBODY)) {
        bool hasbody;

        mIRBSimple = FindSimpleBody(static_cast<ISimable *>(this));
        hasbody = mIRBSimple != nullptr;

        if (hasbody) {
            mIRBSimple->ModifyFlags(0, 0xC100);
        }
    }
    PhysicsObject::OnBehaviorChange(mechanic);
}

void Explosion::OnCollide(IRigidBody *other, float dT, float radius,
                          const Dynamics::Collision::Geometry &explosion_sphere) {
    IExplodeable *iexplodeable;
    if (!other->QueryInterface(&iexplodeable)) {
        return;
    }

    float total_radius = radius + other->GetRadius();
    if (UMath::DistanceSquare(other->GetPosition(), explosion_sphere.GetPosition()) <
        total_radius * total_radius) {
        float targetspeed;
        UMath::Vector3 dim;
        UMath::Matrix4 matrix;
        other->GetDimension(dim);
        other->GetMatrix4(matrix);
        Dynamics::Collision::Geometry box(matrix, other->GetPosition(), dim,
                                          Dynamics::Collision::Geometry::BOX, UMath::Vector3::kZero);
        if (Dynamics::Collision::Geometry::FindIntersection(&box, &explosion_sphere, &box)) {
            iexplodeable->OnExplosion(box.GetCollisionNormal(), box.GetCollisionPoint(), dT,
                                      static_cast<IExplosion *>(this));
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
    if (!mIRBSimple) {
        return;
    }
    {
        SimCollisionMap *cmap = mIRBSimple->GetCollisionMap();
        if (!cmap) {
            return;
        }
        if (!cmap->CollisionWithAny()) {
            return;
        }

        {
            IRigidBody *mybody = GetRigidBody();
            float radius = mybody->GetRadius();
            UVector3 mydim(radius, radius, radius);
            Dynamics::Collision::Geometry explosion_sphere(UMath::Matrix4::kIdentity, mybody->GetPosition(),
                                                           mydim, Dynamics::Collision::Geometry::SPHERE,
                                                           UMath::Vector3::kZero);

            for (unsigned int i = 0; i < 0xA0; i++) {
                if (!cmap->CollisionWithOrderedBody(i)) {
                    continue;
                }
                IRigidBody *irb = cmap->GetOrderedBody(i);
                if (!irb) {
                    continue;
                }
                if (!mEffectSource) {
                    IRenderable *irender;
                    if (irb->QueryInterface(&irender) && irender->GetModelHandle() == mSource) {
                        continue;
                    }
                }
                OnCollide(irb, dT, radius, explosion_sphere);
            }
        }
    }
}

void Explosion::OnTaskSimulate(float dT) {
    IRigidBody *irb = GetRigidBody();
    TestCollisions(dT);
    float radius = irb->GetRadius();
    if (mExpansionSpeed * dT + radius > mExpansionRadius) {
        Kill();
    } else {
        irb->SetRadius(mExpansionSpeed * dT + radius);
    }
}
