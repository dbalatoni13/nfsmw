#ifndef DAMAGE_RACER_H
#define DAMAGE_RACER_H

#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Physics/Behaviors/DamageVehicle.h"

// total size: 0xDC
class DamageRacer : public DamageVehicle, public ISpikeable {
  public:
    typedef DamageVehicle Base;

    static Behavior *Construct(const BehaviorParams &params);

    // Behavior
    void OnTaskSimulate(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // IDamageable
    void ResetDamage() override;

    // ISpikeable
    eTireDamage GetTireDamage(unsigned int wheelId) const override;
    void Puncture(unsigned int wheelId) override;
    unsigned int GetNumBlowouts() const override;

    // IDamageableVehicle
    bool IsLightDamaged(VehicleFX::ID idx) const override;

    // IDamageable
    DamageZone::Info GetZoneDamage() const override;

  protected:
    DamageRacer(const BehaviorParams &bp, const DamageParams &dp);

    // IUnknown
    ~DamageRacer() override;

    // DamageVehicle
    bool CanDamageVisuals() const override;

  private:
    ISuspension *mSuspension; // offset 0xC4, size 0x4
    float mBlowOutTimes[4];   // offset 0xC8, size 0x10
    uint8 mDamage[4];         // offset 0xD8, size 0x4
};

#endif
