#include "DamageVehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Generated/Events/EVehicleDestroyed.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/GenericAccessor.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngineDamage.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/Damagezones.h"

namespace DamageZone {
UCrc32 GetSystemName(DamageZone::ID zone);
UCrc32 GetDamageStimulus(unsigned int level);
UCrc32 GetImpactStimulus(unsigned int level);
} // namespace DamageZone

HINTERFACE IModel::_IHandle() {
    return (HINTERFACE)_IHandle;
}

IModel::~IModel() {}

IDamageable::~IDamageable() {}

IDamageableVehicle::~IDamageableVehicle() {}

IArticulatedVehicle::~IArticulatedVehicle() {}

template BehaviorSpecsPtr<Attrib::Gen::damagespecs>::~BehaviorSpecsPtr();

Behavior *DamageVehicle::Construct(const BehaviorParams &params) {
    const DamageParams dp(params.fparams.Fetch<DamageParams>(UCrc32(0xa6b47fac)));
    return new DamageVehicle(params, dp);
}

DamageVehicle::DamageVehicle(const BehaviorParams &bp, const DamageParams &)
    : VehicleBehavior(bp, 2), //
      IDamageable(bp.fowner), //
      Sim::Collision::IListener(), //
      IDamageableVehicle(bp.fowner), //
      EventSequencer::IContext(this), //
      mShockTimer(0.0f), //
      mSpecs(this, 0), //
      mDamageTotal(0.0f), //
      mZoneDamage(), //
      mLastImpactSpeed(UMath::Vector3::kZero), //
      mBrokenParts(), //
      mLightDamage(0) {
    GetOwner()->QueryInterface(&mIRBComplex);
    GetOwner()->QueryInterface(&mRB);
    GetOwner()->QueryInterface(&mRenderable);
    Sim::Collision::AddListener(this, GetOwner(), "DamageVehicle");
}

DamageVehicle::~DamageVehicle() {
    Sim::Collision::RemoveListener(this);
    ResetParts();
}

void DamageVehicle::ResetParts() {
    IRenderable *irenderable;
    if (GetOwner()->QueryInterface(&irenderable)) {
        IModel *instance = irenderable->GetModel();
        if (instance) {
            instance->ReleaseChildModels();
        }
    }
}

void DamageVehicle::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mIRBComplex);
        GetOwner()->QueryInterface(&mRB);
    }
    if (mechanic == BEHAVIOR_MECHANIC_DRAW) {
        GetOwner()->QueryInterface(&mRenderable);
    }
}

void DamageVehicle::OnCollision(const COLLISION_INFO &cinfo) {
    float closing_speed = UMath::Length(cinfo.closingVel);
    if (closing_speed < 1.0f) {
        return;
    }

    IRigidBody *body = GetOwner()->GetRigidBody();
    float force = cinfo.impulseA * body->GetMass();

    if (cinfo.Type() == COLLISION_INFO::OBJECT) {
        SetShockForce(cinfo.force);
        if (cinfo.objB == GetOwner()->GetInstanceHandle()) {
            force = cinfo.impulseB * body->GetMass();
        }
        force += force;
    }

    HSIMABLE my_handle = GetOwner()->GetInstanceHandle();
    if (cinfo.objA == my_handle) {
        UMath::Vector3 normal = cinfo.normal;
        mLastImpactSpeed = cinfo.objAVel;
        SimSurface surface(cinfo.objAsurface);
        ISimable *other = cinfo.objB ? ISimable::FindInstance(cinfo.objB) : nullptr;
        OnImpact(cinfo.armA, normal, force, UMath::Length(cinfo.objAVel), surface, other);
    } else if (cinfo.objB == my_handle) {
        UMath::Vector3 normal;
        UMath::Scale(cinfo.normal, -1.0f, normal);
        mLastImpactSpeed = cinfo.objBVel;
        SimSurface surface(cinfo.objBsurface);
        ISimable *other = cinfo.objA ? ISimable::FindInstance(cinfo.objA) : nullptr;
        OnImpact(cinfo.armB, normal, force, UMath::Length(cinfo.objBVel), surface, other);
    }
}

bool DamageVehicle::SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) {
    data->fVelocity = UMath::Vector4Make(mLastImpactSpeed, 1.0f);
    return true;
}

const DamageScaleRecord &DamageVehicle::GetDamageRecord(DamageZone::ID zone) const {
    static DamageScaleRecord null_record;
    switch (zone) {
        case DamageZone::DZ_FRONT:
            return mSpecs.DZ_FRONT();
        case DamageZone::DZ_REAR:
            return mSpecs.DZ_REAR();
        case DamageZone::DZ_LEFT:
            return mSpecs.DZ_LEFT();
        case DamageZone::DZ_RIGHT:
            return mSpecs.DZ_RIGHT();
        case DamageZone::DZ_LFRONT:
            return mSpecs.DZ_LFRONT();
        case DamageZone::DZ_RFRONT:
            return mSpecs.DZ_RFRONT();
        case DamageZone::DZ_LREAR:
            return mSpecs.DZ_LREAR();
        case DamageZone::DZ_RREAR:
            return mSpecs.DZ_RREAR();
        case DamageZone::DZ_TOP:
            return mSpecs.DZ_TOP();
        case DamageZone::DZ_BOTTOM:
            return mSpecs.DZ_BOTTOM();
        default:
            return null_record;
    }
}

void DamageVehicle::OnTaskSimulate(float dT) {
    if ((mShockTimer > 0.0f) && (mSpecs.SHOCK_TIME() > 0.0f)) {
        float recover = dT / mSpecs.SHOCK_TIME();
        if (mShockTimer > recover) {
            mShockTimer -= recover;
        } else {
            mShockTimer = 0.0f;
        }
    } else {
        mShockTimer = 0.0f;
    }
}

void DamageVehicle::Reset() {
    mShockTimer = 0.0f;
}

void DamageVehicle::Destroy() {
    if (mDamageTotal < 1.0f) {
        if (CanDamageVisuals()) {
            for (unsigned int i = 0; i < DamageZone::DZ_MAX; i++) {
                mZoneDamage.Set(static_cast<DamageZone::ID>(i), 6);
            }
        }
        mDamageTotal = 1.0f;
        new EVehicleDestroyed(GetOwner()->GetInstanceHandle());
    }
}

void DamageVehicle::ResetDamage() {
    ResetParts();
    mLightDamage = 0;
    mShockTimer = 0.0f;
    mZoneDamage.Clear();
    mDamageTotal = 0.0f;

    EventSequencer::IEngine *es = GetOwner()->GetEventSequencer();
    if (es) {
        // RESET_DAMAGE
        es->ProcessStimulus(0x98c52567, Sim::GetTime(), this, EventSequencer::QUEUE_ALLOW);
    }
    IEngineDamage *ienginedamage;
    if (GetOwner()->QueryInterface(&ienginedamage)) {
        ienginedamage->Repair();
    }
    IArticulatedVehicle *iarticulation;
    if (GetOwner()->QueryInterface(&iarticulation)) {
        IVehicle *itrailer = iarticulation->GetTrailer();
        IDamageable *idamage;
        if (itrailer && itrailer->QueryInterface(&idamage)) {
            idamage->ResetDamage();
        }
    }
}

void DamageVehicle::SetShockForce(float f) {
    if (f > 0.0f && mSpecs.SHOCK_FORCE() > 0.0f) {
        float mass = GetOwner()->GetRigidBody()->GetMass();
        float impulse = f / mass;
        SetInShock(1.0f / mSpecs->SHOCK_FORCE() * impulse);
    }
}

void DamageVehicle::SetInShock(float scale) {
    if (mSpecs.SHOCK_TIME() <= 0.0f || scale <= 0.2f) {
        return;
    }
    mShockTimer = UMath::Min(UMath::Max(mShockTimer, scale), 1.0f);
}

void DamageVehicle::OnImpact(const UMath::Vector3 &arm, const UMath::Vector3 &normal, float force, float speed, const SimSurface &,
                             ISimable *iother) {
    const float suppress_dist = mSpecs.SUPPRESS_DIST();
    if (0.0f < suppress_dist) {
        if (Sim::DistanceToCamera(mRB->GetPosition()) > suppress_dist) {
            return;
        }
    }

    if (iother && iother->GetSimableType() == SIMABLE_SMACKABLE) {
        Attrib::Gen::smackable smackable(iother->GetAttributes());
        if (smackable.NO_CAR_EFFECT()) {
            return;
        }
    }

    const float scaled_force = mSpecs.FORCE() * 1000.0f;
    if (!(0.0f < scaled_force)) {
        return;
    }

    UMath::Vector3 dimension = mRB->GetDimension();

    UMath::Vector3 local_normal = normal;
    mRB->ConvertWorldToLocal(local_normal, false);

    DamageZone::ID zone;
    if (arm.z <= dimension.z * 0.5f) {
        if (-dimension.z * 0.5f <= arm.z) {
            if (arm.y <= dimension.y * 0.5f || -0.8f <= local_normal.y) {
                if (-dimension.y * 0.5f <= arm.y || local_normal.y <= 0.8f) {
                    zone = DamageZone::DZ_LEFT;
                    if (0.0f < arm.x) {
                        zone = DamageZone::DZ_RIGHT;
                    }
                } else {
                    zone = DamageZone::DZ_BOTTOM;
                }
            } else {
                zone = DamageZone::DZ_TOP;
            }
        } else {
            zone = DamageZone::DZ_RREAR;
            if (arm.x <= dimension.x * 0.5f || -0.8f <= local_normal.x) {
                zone = DamageZone::DZ_REAR;
                if (arm.x < -dimension.x * 0.5f && 0.8f < local_normal.x) {
                    zone = DamageZone::DZ_LREAR;
                }
            }
        }
    } else {
        zone = DamageZone::DZ_RFRONT;
        if (arm.x <= dimension.x * 0.5f || -0.8f <= local_normal.x) {
            zone = DamageZone::DZ_FRONT;
            if (arm.x < -dimension.x * 0.5f && 0.8f < local_normal.x) {
                zone = DamageZone::DZ_LFRONT;
            }
        }
    }

    const DamageScaleRecord &record = GetDamageRecord(zone);
    const float intensity = force / scaled_force;
    unsigned int impact_level = static_cast<unsigned int>(intensity * record.VisualScale);
    if (6 < impact_level) {
        impact_level = 6;
    }

    unsigned int damage_level = static_cast<unsigned int>(intensity * record.VisualScale);
    if (2 < damage_level) {
        damage_level = 2;
    }

    const float hit_points = mSpecs.HIT_POINTS();
    if (0.0f < hit_points && mDamageTotal < 1.0f) {
        float damage = (intensity * record.HitPointScale) / hit_points;
        if (mSpecs.HP_THRESHOLD() / hit_points < damage) {
            mDamageTotal += damage;
            if (1.0f < mDamageTotal) {
                mDamageTotal = 1.0f;
                new EVehicleDestroyed(GetOwner()->GetInstanceHandle());
            }
        }
    }

    EventSequencer::System *system = nullptr;
    EventSequencer::IEngine *engine = GetOwner()->GetEventSequencer();
    if (engine) {
        system = engine->FindSystem(DamageZone::GetSystemName(zone).GetValue());
    }

    if (system) {
        const float time = Sim::GetTime();
        system->ProcessStimulus(0x1ea131b8, time, this, EventSequencer::QUEUE_ALLOW);

        IVehicle *ivehicle = nullptr;
        if (iother && iother->QueryInterface(&ivehicle)) {
            system->ProcessStimulus(0x1e3a3751, time, this, EventSequencer::QUEUE_ALLOW);
            if (ivehicle->GetDriverClass() == DRIVER_COP) {
                system->ProcessStimulus(0xe8d20c6b, time, this, EventSequencer::QUEUE_ALLOW);
            }
            if (iother->IsPlayer()) {
                system->ProcessStimulus(0xb13e6c7e, time, this, EventSequencer::QUEUE_ALLOW);
            }
        }

        for (unsigned int i = 0; i < impact_level; ++i) {
            system->ProcessStimulus(DamageZone::GetImpactStimulus(i).GetValue(), time, this, EventSequencer::QUEUE_ALLOW);
        }

        if (0 < damage_level && CanDamageVisuals()) {
            unsigned int current = mZoneDamage.Get(zone);
            if (current < 6 && current < damage_level) {
                while (current < damage_level) {
                    system->ProcessStimulus(DamageZone::GetDamageStimulus(current).GetValue(), time, this, EventSequencer::QUEUE_ALLOW);
                    ++current;
                }
                mZoneDamage.Set(zone, damage_level);
            }
        }
    } else if (0 < damage_level && CanDamageVisuals()) {
        unsigned int current = mZoneDamage.Get(zone);
        if (current < damage_level) {
            mZoneDamage.Set(zone, damage_level);
        }
    }
}
