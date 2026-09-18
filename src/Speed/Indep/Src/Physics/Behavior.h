#ifndef PHYSICS_H
#define PHYSICS_H

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Sim/SimObject.h"
#include "Speed/Indep/Src/Sim/SimProfile.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// total size: 0x10
struct BehaviorParams {
    BehaviorParams(const Sim::Param &params, PhysicsObject *owner, const UCrc32 &sig, const UCrc32 &mechanic)
        : fparams(params), fowner(owner), fSig(sig), fMechanic(mechanic) {}

    const Sim::Param &fparams; // offset 0x0, size 0x4
    PhysicsObject *fowner;     // offset 0x4, size 0x4
    const UCrc32 &fSig;        // offset 0x8, size 0x4
    const UCrc32 &fMechanic;   // offset 0xC, size 0x4
};

// total size: 0x4C
class Behavior : public Sim::Object, public UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32> {
  public:
    USE_FASTALLOC(Behavior);

    Behavior(const BehaviorParams &params, unsigned int num_interfaces);

    void Pause(bool pause);

    void SimulateTask(float dT) {
        Sim::Profile::Scope profile(mProfile);

        if (!mPaused) {
            OnTaskSimulate(dT);
        }
    }

    const UCrc32 &GetMechanic() {
        return mMechanic;
    }

    const UCrc32 &GetSignature() {
        return mSignature;
    }

    bool IsPaused() const {
        return mPaused;
    }

    ISimable *GetOwner() const {
        return mIOwner;
    }

    void EnableProfile(const char *name) {
        Sim::Profile::Release(mProfile);
        mProfile = Sim::Profile::Create();
    }

    virtual void Reset() = 0;

    virtual const int GetPriority() const {
        return mPriority;
    }

    virtual void OnOwnerAttached(IAttachable *pOther) {}

    virtual void OnOwnerDetached(IAttachable *pOther) {
        // TODO right place?
    }

    friend struct PhysicsObject::Behaviors;

  protected:
    virtual void OnTaskSimulate(float dT) {}

    virtual void OnBehaviorChange(const UCrc32 &mechanic) {}

    virtual void OnPause() {}

    virtual void OnUnPause() {}

  public:
    // deleted through UTL::Collections::Container<Behavior, _type_UContainer>
    virtual ~Behavior() {
        // TODO
        Sim::Profile::Release(nullptr);
    }

  private:

  private:
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

    BehaviorSpecsPtr(ISimable *owner, int index) : AttributeStructPtr<T>(LookupKey(owner, index)) {}

    Attrib::Key LookupKey(const ISimable *owner, int index) {
        const Attrib::Instance &owneratr = owner->GetAttributes();
        if (!owneratr.IsValid()) {
            return Attrib::key_default;
        }

        Attrib::Key classkey = AttributeStructPtr<T>::GetClassKey();
        Attrib::Attribute attrib = owneratr.Get(classkey);
        Attrib::RefSpec refspec;
        if (attrib.Get(index, refspec)) {
            return refspec.GetCollectionKey();
        } else {
            return owneratr.GetCollection();
        }
    }
};

#define BIND_BEHAVIOR_FACTORY(_TYPE_) Behavior::Prototype __##_TYPE_(UCrc32(#_TYPE_), _TYPE_::Construct);

#define REDIRECT_BEHAVIOR_FACTORY(_TYPE_, _FACTORYNAME_) Behavior::Prototype __##_TYPE_##_FACTORYNAME_(UCrc32(#_FACTORYNAME_), _TYPE_::Construct);

#define BIND_BEHAVIOR_SPECS BIND_ATTRIBUTE_STRUCT

inline void PhysicsObject::Behaviors::OnTaskSimulate(float dT) {
    for (const_iterator iter = begin(); iter != end(); ++iter) {
        (*iter)->SimulateTask(dT);
    }
}

inline void PhysicsObject::Behaviors::OnBehaviorChange(const UCrc32 &mechanic) {
    for (const_iterator iter = begin(); iter != end(); ++iter) {
        (*iter)->OnBehaviorChange(mechanic);
    }
}

inline void PhysicsObject::Behaviors::Reset() {
    for (const_iterator iter = begin(); iter != end(); ++iter) {
        (*iter)->Reset();
    }
}

inline void PhysicsObject::Behaviors::OnOwnerAttached(IAttachable *iother) {
    for (const_iterator iter = begin(); iter != end(); ++iter) {
        (*iter)->OnOwnerAttached(iother);
    }
}

inline void PhysicsObject::Behaviors::OnOwnerDetached(IAttachable *iother) {
    for (const_iterator iter = begin(); iter != end(); ++iter) {
        (*iter)->OnOwnerDetached(iother);
    }
}

#endif
