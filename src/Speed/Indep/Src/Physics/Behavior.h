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

    ISimable *GetOwner() const {
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
    // total size: 0x4C
    bool mPaused;                 // offset 0x30, size 0x1
    struct PhysicsObject *mOwner; // offset 0x34, size 0x4
    ISimable *mIOwner;            // offset 0x38, size 0x4
    const UCrc32 mMechanic;       // offset 0x3C, size 0x4
    const UCrc32 mSignature;      // offset 0x40, size 0x4
    int mPriority;                // offset 0x44, size 0x4
    HSIMPROFILE mProfile;         // offset 0x48, size 0x4
};

template <typename T> class BehaviorSpecsPtr : public AttributeStructPtr<T> {

  public:
    BehaviorSpecsPtr(Behavior *behavior, int index) : AttributeStructPtr<T>(LookupKey(behavior->GetOwner(), index)) {}

    BehaviorSpecsPtr(ISimable *owner, int index) : AttributeStructPtr<T>(0) {
        // TODO
    }

    ~BehaviorSpecsPtr() {}

    Attrib::Key LookupKey(const ISimable *owner, int index) {
        const Attrib::Instance &owneratr = owner->GetAttributes();
        Attrib::Key classkey;
        if (!owneratr.IsValid()) {
            // TODO what's 0xeec2271a?
            classkey = 0xeec2271a;
            return classkey;
        }
        Attrib::Attribute attrib = owneratr.Get(AttributeStructPtr<T>::GetClassKey());
        Attrib::RefSpec refspec;
        if (attrib.Get(index, refspec)) {
            classkey = refspec.GetCollectionKey();
            return classkey;
        } else {
            classkey = owneratr.GetCollection();
            return classkey;
        }
    }
};

#endif
