#ifndef PHYSICS_BEHAVIORS_DAMAGEVEHICLE_H
#define PHYSICS_BEHAVIORS_DAMAGEVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/damagespecs.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/Collision.h"

struct DamageParams : public Sim::Param {
    DamageParams(const DamageParams &_ctor_arg) : Sim::Param(_ctor_arg) {}

    DamageParams() : Sim::Param(TypeName(), static_cast<DamageParams *>(nullptr)) {}

    static UCrc32 TypeName() {
        static UCrc32 value = UCrc32("DamageParams");
        return value;
    }
};

// total size: 0xB8
class DamageVehicle : public VehicleBehavior,
                      public IDamageable,
                      public Sim::Collision::IListener,
                      public IDamageableVehicle,
                      protected EventSequencer::IContext {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    DamageVehicle(const BehaviorParams &bp, const DamageParams &dp);
    void ResetParts();
    const DamageScaleRecord &GetDamageRecord(DamageZone::ID zone) const;

    // Overrides
    // IUnknown
    override virtual ~DamageVehicle();

    // Behavior
    override virtual void OnBehaviorChange(const UCrc32 &mechanic);
    override virtual void OnTaskSimulate(float dT);
    override virtual void Reset();

    // IListener
    override virtual void OnCollision(const COLLISION_INFO &cinfo);

    // IContext
    override virtual bool SetDynamicData(const EventSequencer::System *system, EventDynamicData *data);

    // IDamageable
    override virtual void Destroy();
    override virtual void SetShockForce(float f);
    override virtual void SetInShock(float scale);
    override virtual void ResetDamage();

    // Virtual methods
    virtual void OnImpact(const UMath::Vector3 &arm, const UMath::Vector3 &normal, float force, float speed, const SimSurface &mysurface,
                          ISimable *iother);

    virtual bool CanDamageVisuals() const {
        return true;
    }

  private:
    float mShockTimer;                                 // offset 0x6C, size 0x4
    int fTempInvincibilityTimer;                       // offset 0x70, size 0x4
    BehaviorSpecsPtr<Attrib::Gen::damagespecs> mSpecs; // offset 0x74, size 0x14
    float mOffScreenTimer;                             // offset 0x88, size 0x4
    float mDamageTotal;                                // offset 0x8C, size 0x4
    ICollisionBody *mIRBComplex;                       // offset 0x90, size 0x4
    IRigidBody *mRB;                                   // offset 0x94, size 0x4
    IRenderable *mRenderable;                          // offset 0x98, size 0x4
    DamageZone::Info mZoneDamage;                      // offset 0x9C, size 0x4
    UMath::Vector3 mLastImpactSpeed;                   // offset 0xA0, size 0xC
    UTL::Std::list<UCrc32, _type_list> mBrokenParts;   // offset 0xAC, size 0x8
    unsigned int mLightDamage;                         // offset 0xB4, size 0x4
};

#endif
