#ifndef PHYSICS_PHYSICSOBJECT_H
#define PHYSICS_PHYSICSOBJECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"

DECLARE_CONTAINER_TYPE(ID_POMechanics);
DECLARE_CONTAINER_TYPE(ID_POBehaviors);

// total size: 0xAC
class PhysicsObject : public Sim::Object,
                      public ISimable,
                      public IBody,
                      public IAttachable,
                      public UTL::Collections::GarbageNode<PhysicsObject, 160>,
                      protected UTL::Collections::Container<Behavior, _type_UContainer> {
  public:
    typedef UTL::Std::map<unsigned int, Behavior *, _type_ID_POMechanics> Mechanics;

    struct Behaviors : protected UTL::Std::list<Behavior *, _type_ID_POBehaviors> {
        typedef UTL::Std::list<Behavior *, _type_ID_POBehaviors> _Base;
        using _Base::begin;
        using _Base::end;
        using _Base::const_iterator;
        using _Base::iterator;

        // total size: 0x8
        void Add(Behavior *beh);
        void Remove(Behavior *beh);
        void Reset();

        void OnAttach(IAttachable *iother) {
            for (const_iterator iter = begin(); iter != end(); ++iter) {
                (*iter)->OnOwnerAttached(iother);
            }
        }

        void OnDetach(IAttachable *iother) {
            for (const_iterator iter = begin(); iter != end(); ++iter) {
                (*iter)->OnOwnerDetached(iother);
            }
        }

        void Simulate(float dT) {
            for (const_iterator iter = begin(); iter != end(); ++iter) {
                if (!(*iter)->IsPaused()) {
                    (*iter)->DoSimulate(dT);
                }
            }
        }

        void Changed(const UCrc32 &mechanic) {
            for (const_iterator iter = begin(); iter != end(); ++iter) {
                (*iter)->BehaviorChanged(mechanic);
            }
        }
    };

    PhysicsObject(const Attrib::Instance &attribs, SimableType objType, WUID wuid, unsigned int num_interfaces);
    PhysicsObject(const char *attributeClass, const char *attribName, SimableType objType, HSIMABLE owner, WUID wuid);

    // Overrides
    virtual ~PhysicsObject();

    // ISimable overrides
    virtual SimableType GetSimableType() const override {
        return mObjType;
    }
    virtual void Kill() override;
    virtual bool Attach(UTL::COM::IUnknown *object) override;
    virtual bool Detach(UTL::COM::IUnknown *object) override;
    virtual const UTL::Std::list<IAttachable *, _type_IAttachableList> *GetAttachments() const override {
        if (mAttachments == nullptr) {
            return nullptr;
        }
        return &mAttachments->GetList();
    }
    virtual void AttachEntity(Sim::IEntity *e) override;
    virtual void DetachEntity() override;
    virtual struct IPlayer *GetPlayer() const override {
        return mPlayer;
    }
    virtual bool IsPlayer() const override {
        return mPlayer != nullptr;
    }
    virtual bool IsOwnedByPlayer() const override;
    virtual Sim::IEntity *GetEntity() const override {
        return mEntity;
    }
    virtual void DebugObject() override;
    virtual HSIMABLE GetOwnerHandle() const override {
        return mOwner;
    }
    virtual ISimable *GetOwner() const override {
        return ISimable::FindInstance(mOwner);
    }
    virtual bool IsOwnedBy(ISimable *queriedOwner) const override;
    virtual void SetOwnerObject(ISimable *pOwner) override;
    virtual const Attrib::Instance &GetAttributes() const override {
        return mAttributes;
    }
    virtual WWorldPos &GetWPos() override;
    virtual const WWorldPos &GetWPos() const override;
    virtual class IRigidBody *GetRigidBody() override;
    virtual const class IRigidBody *GetRigidBody() const override { return mRigidBody; }
    virtual bool IsRigidBodySimple() const override {
        if (mRigidBody) {
            return mRigidBody->IsSimple();
        }
        return false;
    }
    virtual bool IsRigidBodyComplex() const override {
        return !IsRigidBodySimple();
    }
    virtual const UMath::Vector3 &GetPosition() const override {
        return mRigidBody != nullptr ? mRigidBody->GetPosition() : UMath::Vector3::kZero;
    }
    virtual void GetTransform(UMath::Matrix4 &matrix) const override;
    virtual void GetLinearVelocity(UMath::Vector3 &velocity) const override;
    virtual void GetAngularVelocity(UMath::Vector3 &velocity) const override;
    virtual unsigned int GetWorldID() const override {
        return mWorldID;
    }
    virtual EventSequencer::IEngine *GetEventSequencer() override {
        return nullptr;
    }
    virtual void ProcessStimulus(unsigned int stimulus) override;
    virtual IModel *GetModel() override;
    virtual const IModel *GetModel() const override;
    virtual void SetCausality(HCAUSE from, float time) override;
    virtual HCAUSE GetCausality() const override;
    virtual float GetCausalityTime() const override;

    // IAttachable overrides
    virtual void OnAttached(IAttachable *pOther) override;
    virtual void OnDetached(IAttachable *pOther) override;
    virtual bool IsAttached(const UTL::COM::IUnknown *pOther) const override {
        if (mAttachments) {
            return mAttachments->IsAttached(pOther);
        }
        return false;
    }

    // IBody overrides
    virtual void GetDimension(UMath::Vector3 &dim) const override;

    // Sim::Object overrides
    virtual bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt) override;
    virtual bool OnTask(HSIMTASK htask, float dT) override;

    // Other public methods
    bool IsBehaviorActive(const UCrc32 &mechanic) const;
    void PauseBehavior(const UCrc32 &mechanic, bool pause);
    bool ResetBehavior(const UCrc32 &mechanic);
    Behavior *FindBehavior(const UCrc32 &mechanic);
    void DetachAll();
    void ReleaseBehaviors();
    virtual void Reset();

  protected:
    Behavior *LoadBehavior(const UCrc32 &mechanic, const UCrc32 &behavior, Sim::Param params);
    void ReleaseBehavior(const UCrc32 &mechanic);
    virtual void OnTaskSimulate(float dT);
    virtual void OnBehaviorChange(const UCrc32 &mechanic);

  protected:
    WWorldPos *mWPos;               // offset 0x58, size 0x4
    SimableType mObjType;           // offset 0x5C, size 0x4
    HSIMABLE mOwner;                // offset 0x60, size 0x4
    Attrib::Instance mAttributes;   // offset 0x64, size 0x14
    IRigidBody *mRigidBody;         // offset 0x78, size 0x4
    HSIMTASK mSimulateTask;         // offset 0x7C, size 0x4
    Sim::IEntity *mEntity;          // offset 0x80, size 0x4
    IPlayer *mPlayer;               // offset 0x84, size 0x4
    HSIMSERVICE mBodyService;       // offset 0x88, size 0x4
    WUID mWorldID;                  // offset 0x8C, size 0x4
    Mechanics mMechanics;           // offset 0x90, size 0x10
    Behaviors mBehaviors;           // offset 0xA0, size 0x8
    Sim::Attachments *mAttachments; // offset 0xA8, size 0x4
};

#endif
