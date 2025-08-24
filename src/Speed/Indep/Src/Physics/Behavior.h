#ifndef PHYSICS_BEHAVIOR_H
#define PHYSICS_BEHAVIOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Sim/SimProfile.h"

struct BehaviorParams {
    // total size: 0x10
    const Sim::Param &fparams;    // offset 0x0, size 0x4
    struct PhysicsObject *fowner; // offset 0x4, size 0x4
    const UCrc32 &fSig;           // offset 0x8, size 0x4
    const UCrc32 &fMechanic;      // offset 0xC, size 0x4
};

class Behavior : public Sim::Object, public UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32> {
  public:
    Behavior(const BehaviorParams &params, unsigned int num_interfaces) : Sim::Object(num_interfaces) {}

    const UCrc32 &GetMechanic() {
        return mMechanic;
    }

    const UCrc32 &GetSignature() {
        return mSignature;
    }

    bool IsPaused() {
        return mPaused;
    }

    ISimable *GetOwner() {
        return mIOwner;
    }

    const int GetPriority() {
        return mPriority;
    }

    virtual void Reset();
    virtual const int GetPriority() const;

  protected:
    virtual void OnTaskSimulate(float dT);
    virtual void OnBehaviorChange(const UCrc32 &mechanic);
    virtual void OnPause();
    virtual void OnUnPause();
    virtual ~Behavior();

  private:
    bool mPaused;
    struct PhysicsObject *mOwner;
    ISimable *mIOwner;
    const UCrc32 mMechanic;
    const UCrc32 mSignature;
    int mPriority;
    HSIMPROFILE mProfile;
};

#endif
