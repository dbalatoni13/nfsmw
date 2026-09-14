#ifndef __PHYSICSOBJECT_H__
#define __PHYSICSOBJECT_H__

#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"
#include "Speed/Indep/Src/Sim/SimObject.h"

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
        using _Base::const_iterator;
        using _Base::end;
        using _Base::iterator;

        // total size: 0x8
        void Add(Behavior *beh);
        void Remove(Behavior *beh);
        void Reset();

        void OnAttach(IAttachable *iother) {
            for (const_iterator iter = this->begin(); iter != this->end(); ++iter) {
                (*iter)->OnOwnerAttached(iother);
            }
        }

        void OnDetach(IAttachable *iother) {
            for (const_iterator iter = this->begin(); iter != this->end(); ++iter) {
                (*iter)->OnOwnerDetached(iother);
            }
        }

        void Simulate(float dT) {
            for (const_iterator iter = this->begin(); iter != this->end(); ++iter) {
                if (!(*iter)->IsPaused()) {
                    (*iter)->DoSimulate(dT);
                }
            }
        }

        void Changed(const UCrc32 &mechanic) {
            for (const_iterator iter = this->begin(); iter != this->end(); ++iter) {
                (*iter)->BehaviorChanged(mechanic);
            }
        }
    };

    PhysicsObject(const Attrib::Instance &attribs, SimableType objType, WUID wuid, unsigned int num_interfaces);
    PhysicsObject(const char *attributeClass, const char *attribName, SimableType objType, HSIMABLE owner, WUID wuid);

    // Overrides
    ~PhysicsObject() override;

    // ISimable
    SimableType GetSimableType() const override {
        return this->mObjType;
    }

    void Kill() override;
    bool Attach(UTL::COM::IUnknown *object) override;
    bool Detach(UTL::COM::IUnknown *object) override;

    const IAttachable::List *GetAttachments() const override {
        if (this->mAttachments == nullptr) {
            return nullptr;
        }
        return &this->mAttachments->GetList();
    }

    void AttachEntity(Sim::IEntity *e) override;
    void DetachEntity() override;

    struct IPlayer *GetPlayer() const override {
        return this->mPlayer;
    }

    bool IsPlayer() const override {
        return this->mPlayer != nullptr;
    }

    bool IsOwnedByPlayer() const override;

    Sim::IEntity *GetEntity() const override {
        return this->mEntity;
    }

    void DebugObject() override;

    HSIMABLE GetOwnerHandle() const override {
        return this->mOwner;
    }

    ISimable *GetOwner() const override {
        return ISimable::FindInstance(this->mOwner);
    }

    bool IsOwnedBy(ISimable *queriedOwner) const override;
    void SetOwnerObject(ISimable *pOwner) override;

    const Attrib::Instance &GetAttributes() const override {
        return this->mAttributes;
    }

    WWorldPos &GetWPos() override;
    const WWorldPos &GetWPos() const override;
    class IRigidBody *GetRigidBody() override;

    const class IRigidBody *GetRigidBody() const override {
        return this->mRigidBody;
    }

    bool IsRigidBodySimple() const override {
        if (this->mRigidBody != nullptr) {
            return this->mRigidBody->IsSimple();
        }
        return false;
    }

    bool IsRigidBodyComplex() const override {
        return !this->IsRigidBodySimple();
    }

    const UMath::Vector3 &GetPosition() const override {
        return this->mRigidBody != nullptr ? this->mRigidBody->GetPosition() : UMath::Vector3::kZero;
    }

    void GetTransform(UMath::Matrix4 &matrix) const override;
    void GetLinearVelocity(UMath::Vector3 &velocity) const override;
    void GetAngularVelocity(UMath::Vector3 &velocity) const override;

    unsigned int GetWorldID() const override {
        return this->mWorldID;
    }

    EventSequencer::IEngine *GetEventSequencer() override {
        return nullptr;
    }

    void ProcessStimulus(unsigned int stimulus) override;
    IModel *GetModel() override;
    const IModel *GetModel() const override;
    void SetCausality(HCAUSE from, float time) override;
    HCAUSE GetCausality() const override;
    float GetCausalityTime() const override;

    // IAttachable
    void OnAttached(IAttachable *pOther) override;
    void OnDetached(IAttachable *pOther) override;

    bool IsAttached(const UTL::COM::IUnknown *pOther) const override {
        if (this->mAttachments != nullptr) {
            return this->mAttachments->IsAttached(pOther);
        }
        return false;
    }

    // IBody
    void GetDimension(UMath::Vector3 &dim) const override;

    // Sim::Object
    bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt) override;
    bool OnTask(HSIMTASK htask, float dT) override;

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
