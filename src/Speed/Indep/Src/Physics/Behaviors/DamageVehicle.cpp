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

DamageVehicle::DamageVehicle(const BehaviorParams &bp, const DamageParams &sp)
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

void DamageVehicle::OnImpact(const UMath::Vector3 &arm, const UMath::Vector3 &normal, float force, float speed, const SimSurface &mysurface,
                             ISimable *iother) {
    float suppress_distance = mSpecs.SUPPRESS_DIST();
    if (0.0f < suppress_distance) {
        float viewdist = Sim::DistanceToCamera(mRB->GetPosition());
        if (viewdist > suppress_distance) {
            return;
        }
    }

    if (iother && iother->GetSimableType() == SIMABLE_SMACKABLE) {
        Attrib::Gen::smackable smackable(iother->GetAttributes());
        if (smackable.NO_CAR_EFFECT()) {
            return;
        }
    }

    float material_strength = mSpecs.FORCE() * 1000.0f;
    if (!(0.0f < material_strength)) {
        return;
    }

    {
        UMath::Vector3 dim = mRB->GetDimension();

        UMath::Vector3 relative_normal = normal;
        mRB->ConvertWorldToLocal(relative_normal, false);

        DamageZone::ID zone;
        const float kDirectionThreshold = 0.8f;
        const float kZoneThreshold = 0.5f;
        if (arm.z > dim.z * kZoneThreshold) {
            zone = DamageZone::DZ_RFRONT;
            if (arm.x <= dim.x * kZoneThreshold || -kDirectionThreshold <= relative_normal.x) {
                zone = DamageZone::DZ_FRONT;
                if (arm.x < -dim.x * kZoneThreshold && kDirectionThreshold < relative_normal.x) {
                    zone = DamageZone::DZ_LFRONT;
                }
            }
        } else if (arm.z < -dim.z * kZoneThreshold) {
            zone = DamageZone::DZ_RREAR;
            if (arm.x <= dim.x * kZoneThreshold || -kDirectionThreshold <= relative_normal.x) {
                zone = DamageZone::DZ_REAR;
                if (arm.x < -dim.x * kZoneThreshold && kDirectionThreshold < relative_normal.x) {
                    zone = DamageZone::DZ_LREAR;
                }
            }
        } else if (arm.y > dim.y * kZoneThreshold && relative_normal.y < -kDirectionThreshold) {
            zone = DamageZone::DZ_TOP;
        } else {
            if (arm.y < -dim.y * kZoneThreshold && kDirectionThreshold < relative_normal.y) {
                zone = DamageZone::DZ_BOTTOM;
            } else {
                zone = DamageZone::DZ_LEFT;
                if (0.0f < arm.x) {
                    zone = DamageZone::DZ_RIGHT;
                }
            }
        }

        const DamageScaleRecord &record = GetDamageRecord(zone);
        float visual_scale = record.VisualScale;
        float hitpoint_scale = record.HitPointScale;
        float ratio = force / material_strength;
        int newimpact_level = UMath::Min(static_cast<int>(ratio * visual_scale), 6);
        int newdamage_level = UMath::Min(static_cast<int>(ratio * visual_scale), 2);

        float total_hit_points = mSpecs.HIT_POINTS();
        if (0.0f < total_hit_points && mDamageTotal < 1.0f) {
            float damage_points = (ratio * hitpoint_scale) / total_hit_points;
            float threshold = mSpecs.HP_THRESHOLD() / total_hit_points;
            if (threshold < damage_points) {
                mDamageTotal += damage_points;
                if (1.0f < mDamageTotal) {
                    mDamageTotal = 1.0f;
                    new EVehicleDestroyed(GetOwner()->GetInstanceHandle());
                }
            }
        }

        EventSequencer::IEngine *sequencer = GetOwner()->GetEventSequencer();
        EventSequencer::System *system = nullptr;
        if (sequencer) {
            system = sequencer->FindSystem(DamageZone::GetSystemName(zone).GetValue());
        }

        if (system) {
            const float time = Sim::GetTime();
            system->ProcessStimulus(0x1ea131b8, time, this, EventSequencer::QUEUE_ALLOW);

            IVehicle *iv = nullptr;
            if (iother && iother->QueryInterface(&iv)) {
                system->ProcessStimulus(0x1e3a3751, time, this, EventSequencer::QUEUE_ALLOW);
                if (iv->GetDriverClass() == DRIVER_COP) {
                    system->ProcessStimulus(0xe8d20c6b, time, this, EventSequencer::QUEUE_ALLOW);
                }
                if (iother->IsPlayer()) {
                    system->ProcessStimulus(0xb13e6c7e, time, this, EventSequencer::QUEUE_ALLOW);
                }
            }

            for (int j = 0; j < newimpact_level; ++j) {
                system->ProcessStimulus(DamageZone::GetImpactStimulus(j).GetValue(), time, this, EventSequencer::QUEUE_ALLOW);
            }

            if (0 < newdamage_level && CanDamageVisuals()) {
                int damage_level = mZoneDamage.Get(zone);
                if (damage_level < 6 && damage_level < newdamage_level) {
                    while (damage_level < newdamage_level) {
                        system->ProcessStimulus(DamageZone::GetDamageStimulus(damage_level).GetValue(), time, this,
                                                EventSequencer::QUEUE_ALLOW);
                        ++damage_level;
                    }
                    mZoneDamage.Set(zone, newdamage_level);
                }
            }
        } else if (0 < newdamage_level && CanDamageVisuals()) {
            int damage_level = mZoneDamage.Get(zone);
            if (damage_level < newdamage_level) {
                mZoneDamage.Set(zone, newdamage_level);
            }
        }
    }
}
