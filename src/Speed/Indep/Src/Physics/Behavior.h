#ifndef PHYSICS_H
#define PHYSICS_H

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Sim/SimObject.h"
#include "Speed/Indep/Src/Sim/SimProfile.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// total size: 0x10
struct BehaviorParams {
    BehaviorParams(const Sim::Param &params, struct PhysicsObject *owner, const UCrc32 &mechanic, const UCrc32 &signature)
        : fparams(params), //
          fowner(owner),   //
          fSig(signature), //
          fMechanic(mechanic) {}

    const Sim::Param &fparams;    // offset 0x0, size 0x4
    struct PhysicsObject *fowner; // offset 0x4, size 0x4
    const UCrc32 &fSig;           // offset 0x8, size 0x4
    const UCrc32 &fMechanic;      // offset 0xC, size 0x4
};

// total size: 0x4C
class Behavior : public Sim::Object, public UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32> {
  public:
    USE_FASTALLOC(Behavior);

    const UCrc32 &GetMechanic() {
        return this->mMechanic;
    }

    const UCrc32 &GetSignature() {
        return this->mSignature;
    }

    bool IsPaused() const {
        return this->mPaused;
    }

    ISimable *GetOwner() const {
        return this->mIOwner;
    }

    void DoSimulate(float dT) {
        Sim::Profile::Scope profile(this->mProfile);
        this->OnTaskSimulate(dT);
    }

    void BehaviorChanged(const UCrc32 &mechanic) {
        this->OnBehaviorChange(mechanic);
    }

    void EnableProfile(const char *name) {
        Sim::Profile::Release(this->mProfile);
        this->mProfile = Sim::Profile::Create();
    }

    void Pause(bool pause);

    static void Destroy(Behavior *b) {
        delete b;
    }

    virtual void Reset() = 0;

    virtual const int GetPriority() const {
        return this->mPriority;
    }

    virtual void OnOwnerAttached(IAttachable *pOther) {}

    virtual void OnOwnerDetached(IAttachable *pOther) {
        // TODO right place?
    }

  protected:
    virtual void OnTaskSimulate(float dT);

    virtual void OnBehaviorChange(const UCrc32 &mechanic) {}

    virtual void OnPause();
    virtual void OnUnPause();

    Behavior(const BehaviorParams &params, unsigned int num_interfaces);

    ~Behavior() override {
        // TODO
        Sim::Profile::Release(nullptr);
    }

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

#endif
