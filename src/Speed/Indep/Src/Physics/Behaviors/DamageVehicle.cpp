#include "DamageVehicle.h"
#include "Speed/Indep/Src/Generated/Events/EVehicleDestroyed.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngineDamage.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/Damagezones.h"

static const bool Tweak_KillCops = false; // TODO use
BIND_BEHAVIOR_FACTORY(DamageVehicle);

Behavior *DamageVehicle::Construct(const BehaviorParams &params) {
    const DamageParams dp(params.fparams.Fetch<DamageParams>(UCrc32(UCRC32_BASE)));
    return new DamageVehicle(params, dp);
}

DamageVehicle::DamageVehicle(const BehaviorParams &bp, const DamageParams &sp)
    : VehicleBehavior(bp, 2),                  //
      IDamageable(bp.fowner),                  //
      Sim::Collision::IListener(),             //
      IDamageableVehicle(bp.fowner),           //
      EventSequencer::IContext(this),          //
      mShockTimer(0.0f),                       //
      mSpecs(this, 0),                         //
      mDamageTotal(0.0f),                      //
      mZoneDamage(),                           //
      mLastImpactSpeed(UMath::Vector3::kZero), //
      mBrokenParts(),                          //
      mLightDamage(0) {
    this->GetOwner()->QueryInterface(&mIRBComplex);
    this->GetOwner()->QueryInterface(&mRB);
    this->GetOwner()->QueryInterface(&mRenderable);
    Sim::Collision::AddListener(this, this->GetOwner(), "DamageVehicle");
}

void DamageVehicle::ResetParts() {
    IRenderable *irenderable;
    if (this->GetOwner()->QueryInterface(&irenderable)) {
        IModel *instance = irenderable->GetModel();
        if (instance != nullptr) {
            instance->ReleaseChildModels();
        }
    }
}

void DamageVehicle::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mIRBComplex);
        this->GetOwner()->QueryInterface(&this->mRB);
    }
    if (mechanic == BEHAVIOR_MECHANIC_DRAW) {
        this->GetOwner()->QueryInterface(&this->mRenderable);
    }
}

DamageVehicle::~DamageVehicle() {
    Sim::Collision::RemoveListener(this);
    ResetParts();
}

static const bool Tweak_EnableDamage = true; // TODO use

void DamageVehicle::OnCollision(const COLLISION_INFO &cinfo) {
    float speed = UMath::Length(cinfo.closingVel);
    if (speed < 1.0f) {
        return;
    }

    IRigidBody *rigidbody = this->GetOwner()->GetRigidBody();
    float mass = rigidbody->GetMass();
    float impact_force = cinfo.impulseA * mass;

    if (cinfo.type == COLLISION_INFO::OBJECT) {
        this->SetShockForce(cinfo.force);
        if (cinfo.objB == this->GetOwner()->GetInstanceHandle()) {
            impact_force = cinfo.impulseB * mass;
        }
        impact_force += impact_force;
    }

    float old_speed;
    if (cinfo.objA == this->GetOwner()->GetInstanceHandle()) {
        UMath::Vector3 normal = cinfo.normal;
        this->mLastImpactSpeed = cinfo.objAVel;
        old_speed = UMath::Length(cinfo.objAVel);

        this->OnImpact(cinfo.armA, normal, impact_force, speed, SimSurface(cinfo.objAsurface), ISimable::FindInstance(cinfo.objB));
    } else if (cinfo.objB == this->GetOwner()->GetInstanceHandle()) {
        UMath::Vector3 normal;
        UMath::Scale(cinfo.normal, -1.0f, normal);
        this->mLastImpactSpeed = cinfo.objBVel;
        old_speed = UMath::Length(cinfo.objBVel);

        this->OnImpact(cinfo.armB, normal, impact_force, speed, SimSurface(cinfo.objBsurface), ISimable::FindInstance(cinfo.objA));
    }
}

bool DamageVehicle::SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) {
    data->fVelocity = UMath::Vector4Make(this->mLastImpactSpeed, 1.0f);
    return true;
}

const DamageScaleRecord &DamageVehicle::GetDamageRecord(DamageZone::ID zone) const {
    static DamageScaleRecord null_record;
    switch (zone) {
        case DamageZone::DZ_FRONT:
            return this->mSpecs.DZ_FRONT();
        case DamageZone::DZ_REAR:
            return this->mSpecs.DZ_REAR();
        case DamageZone::DZ_LEFT:
            return this->mSpecs.DZ_LEFT();
        case DamageZone::DZ_RIGHT:
            return this->mSpecs.DZ_RIGHT();
        case DamageZone::DZ_LFRONT:
            return this->mSpecs.DZ_LFRONT();
        case DamageZone::DZ_RFRONT:
            return this->mSpecs.DZ_RFRONT();
        case DamageZone::DZ_LREAR:
            return this->mSpecs.DZ_LREAR();
        case DamageZone::DZ_RREAR:
            return this->mSpecs.DZ_RREAR();
        case DamageZone::DZ_TOP:
            return this->mSpecs.DZ_TOP();
        case DamageZone::DZ_BOTTOM:
            return this->mSpecs.DZ_BOTTOM();
        default:
            return null_record;
    }
}

void DamageVehicle::OnImpact(const UMath::Vector3 &arm, const UMath::Vector3 &normal, float force, float speed, const SimSurface &mysurface,
                             ISimable *iother) {
    float suppress_distance = this->mSpecs.SUPPRESS_DIST();
    if (0.0f < suppress_distance) {
        float viewdist = Sim::DistanceToCamera(this->mRB->GetPosition());
        if (viewdist > suppress_distance) {
            return;
        }
    }

    if ((iother != nullptr) && iother->GetSimableType() == SIMABLE_SMACKABLE) {
        Attrib::Gen::smackable smackable(iother->GetAttributes());
        if (smackable.NO_CAR_EFFECT()) {
            return;
        }
    }

    float material_strength = this->mSpecs.FORCE() * 1000.0f;
    if (0.0f >= material_strength) {
    } else {
        UMath::Vector3 dim = this->mRB->GetDimension();

        UMath::Vector3 relative_normal = normal;
        this->mRB->ConvertWorldToLocal(relative_normal, false);

        DamageZone::ID zone;
        const float kDirectionThreshold = 0.8f;
        const float kZoneThreshold = 0.5f;

        if (arm.z > dim.z * kZoneThreshold) {
            if (arm.x > dim.x * kZoneThreshold && -kDirectionThreshold > relative_normal.x) {
                zone = DamageZone::DZ_RFRONT;
            } else {
                if (arm.x < -dim.x * kZoneThreshold && kDirectionThreshold < relative_normal.x) {
                    zone = DamageZone::DZ_LFRONT;
                } else {
                    zone = DamageZone::DZ_FRONT;
                }
            }
        } else if (arm.z < -dim.z * kZoneThreshold) {
            if (arm.x > dim.x * kZoneThreshold && -kDirectionThreshold > relative_normal.x) {
                zone = DamageZone::DZ_RREAR;
            } else {
                if (arm.x < -dim.x * kZoneThreshold && kDirectionThreshold < relative_normal.x) {
                    zone = DamageZone::DZ_LREAR;
                } else {
                    zone = DamageZone::DZ_REAR;
                }
            }
        } else if (arm.y > dim.y * kZoneThreshold && relative_normal.y < -kDirectionThreshold) {
            zone = DamageZone::DZ_TOP;
        } else {
            if (arm.y < -dim.y * kZoneThreshold && kDirectionThreshold < relative_normal.y) {
                zone = DamageZone::DZ_BOTTOM;
            } else {
                if (0.0f < arm.x) {
                    zone = DamageZone::DZ_RIGHT;
                } else {
                    zone = DamageZone::DZ_LEFT;
                }
            }
        }

        if (zone == DamageZone::DZ_MAX) {
            return;
        }

        const DamageScaleRecord &record = this->GetDamageRecord(zone);
        float visual_scale = record.VisualScale;
        float hitpoint_scale = record.HitPointScale;
        float ratio = force / material_strength;
        int newimpact_level = UMath::Min(6, static_cast<int>(ratio * visual_scale));
        int newdamage_level = UMath::Min(2, static_cast<int>(ratio * visual_scale));

        float total_hit_points = this->mSpecs.HIT_POINTS();
        if (0.0f < total_hit_points && this->mDamageTotal < 1.0f) {
            float damage_points = (ratio * hitpoint_scale) / total_hit_points;
            float threshold = this->mSpecs.HP_THRESHOLD() / total_hit_points;
            if (damage_points > threshold) {
                this->mDamageTotal += damage_points;
                if (1.0f < this->mDamageTotal) {
                    new EVehicleDestroyed(this->GetOwner()->GetInstanceHandle());
                }
            }
        }

        float time = Sim::GetTime();
        EventSequencer::IEngine *sequencer = this->GetOwner()->GetEventSequencer();
        EventSequencer::System *system = nullptr;
        if (sequencer != nullptr) {
            system = sequencer->FindSystem(DamageZone::GetSystemName(zone).GetHash32());
        }

        if (system != nullptr) {
            system->ProcessStimulus(UCRC32_COLLISION, time, this, EventSequencer::QUEUE_ALLOW);

            IVehicle *iv;
            if ((iother != nullptr) && iother->QueryInterface(&iv)) {
                system->ProcessStimulus(UCRC32_COLLISION_VEHICLE, time, this, EventSequencer::QUEUE_ALLOW);
                if (iv->GetDriverClass() == DRIVER_COP) {
                    system->ProcessStimulus(UCRC32_COLLISION_COP, time, this, EventSequencer::QUEUE_ALLOW);
                }
                if (iother->IsPlayer()) {
                    system->ProcessStimulus(UCRC32_COLLISION_PLAYER, time, this, EventSequencer::QUEUE_ALLOW);
                }
            }

            for (int j = 0; j < newimpact_level; ++j) {
                system->ProcessStimulus(DamageZone::GetImpactStimulus(j).GetValue(), time, this, EventSequencer::QUEUE_ALLOW);
            }
        }

        if (0 < newdamage_level && this->CanDamageVisuals()) {
            int damage_level = this->mZoneDamage.Get(zone);
            if ((unsigned int)damage_level <= 5U && newdamage_level > damage_level) {
                if (system != nullptr) {
                    for (int j = damage_level; j < newdamage_level; ++j) {
                        system->ProcessStimulus(DamageZone::GetDamageStimulus(j).GetValue(), time, this, EventSequencer::QUEUE_ALLOW);
                    }
                }
                this->mZoneDamage.Set(zone, newdamage_level);
            }
        }
    }
}

void DamageVehicle::OnTaskSimulate(float dT) {
    if ((this->mShockTimer > 0.0f) && (this->mSpecs.SHOCK_TIME() > 0.0f)) {
        float recover = dT / this->mSpecs.SHOCK_TIME();
        if (this->mShockTimer > recover) {
            this->mShockTimer -= recover;
        } else {
            this->mShockTimer = 0.0f;
        }
    } else {
        this->mShockTimer = 0.0f;
    }
}

void DamageVehicle::Reset() {
    this->mShockTimer = 0.0f;
}

void DamageVehicle::Destroy() {
    if (this->mDamageTotal < 1.0f) {
        if (this->CanDamageVisuals()) {
            for (unsigned int i = 0; i < DamageZone::DZ_MAX; i++) {
                this->mZoneDamage.Set(static_cast<DamageZone::ID>(i), 6);
            }
        }
        this->mDamageTotal = 1.0f;
        new EVehicleDestroyed(GetOwner()->GetInstanceHandle());
    }
}

void DamageVehicle::ResetDamage() {
    this->ResetParts();
    this->mLightDamage = 0;
    this->mShockTimer = 0.0f;
    this->mZoneDamage.Clear();
    this->mDamageTotal = 0.0f;

    EventSequencer::IEngine *es = this->GetOwner()->GetEventSequencer();
    if (es != nullptr) {
        es->ProcessStimulus(UCRC32_RESET_DAMAGE, Sim::GetTime(), this, EventSequencer::QUEUE_ALLOW);
    }
    IEngineDamage *ienginedamage;
    if (this->GetOwner()->QueryInterface(&ienginedamage)) {
        ienginedamage->Repair();
    }
    IArticulatedVehicle *iarticulation;
    if (this->GetOwner()->QueryInterface(&iarticulation)) {
        IVehicle *itrailer = iarticulation->GetTrailer();
        IDamageable *idamage;
        if ((itrailer != nullptr) && itrailer->QueryInterface(&idamage)) {
            idamage->ResetDamage();
        }
    }
}

void DamageVehicle::SetShockForce(float f) {
    if (f > 0.0f && this->mSpecs.SHOCK_FORCE() > 0.0f) {
        float mass = this->GetOwner()->GetRigidBody()->GetMass();
        float impulse = f / mass;
        this->SetInShock(1.0f / this->mSpecs->SHOCK_FORCE() * impulse);
    }
}

void DamageVehicle::SetInShock(float scale) {
    if (this->mSpecs.SHOCK_TIME() <= 0.0f || scale <= 0.2f) {
        return;
    }
    this->mShockTimer = UMath::Min(UMath::Max(this->mShockTimer, scale), 1.0f);
}
