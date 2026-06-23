#ifndef DAMAGE_HELI_H
#define DAMAGE_HELI_H

#include "DamageVehicle.h"

// total size: 0xC0
class DamageHeli : public DamageVehicle {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    // Behavior
    void OnTaskSimulate(float dT) override;
    void Reset() override;

  protected:
    DamageHeli(const BehaviorParams &bp, const DamageParams &dp);
    // IUnknown
    ~DamageHeli() override;

    void DoAutoDestruct();
    void StartAutoDestruct();

  private:
    int mAutoDestruct; // offset 0xB8, size 0x4
    int mDestroying;   // offset 0xBC, size 0x4
};

#endif
