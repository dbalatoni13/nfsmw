#ifndef PHYSICS_BEHAVIORS_SPIKESTRIP_H
#define PHYSICS_BEHAVIORS_SPIKESTRIP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"

// total size: 0x50
struct SpikeStrip : public Behavior {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    SpikeStrip(const BehaviorParams &params);
    ~SpikeStrip() override;

    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void Reset() override;
    void OnTaskSimulate(float dT) override;

    void SetupBody();
    void OnCollide(const Dynamics::Collision::Geometry &mygeometry, IRigidBody *irb, float dT);

  private:
    ISimpleBody *mBody; // offset 0x4C, size 0x4
};

#endif
