#ifndef DAMAGE_DRAGSTER_H
#define DAMAGE_DRAGSTER_H

#include "DamageRacer.h"
#include "Speed/Indep/Src/Sim/Collision.h"

// total size: 0xDC
class DamageDragster : public DamageRacer {
  public:
    typedef DamageRacer Base;

    // IUnknown
    ~DamageDragster() override {}

    static Behavior *Construct(const BehaviorParams &params);

    // IListener
    void OnCollision(const COLLISION_INFO &cinfo) override;

  protected:
    DamageDragster(const BehaviorParams &bp, const DamageParams &dp);

  private:
    void CheckTotaling(const COLLISION_INFO &cinfo);
};

#endif
