#ifndef __PHYSICSOBJECT_H__
#define __PHYSICSOBJECT_H__

#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"
#include "Speed/Indep/Src/Sim/SimObject.h"

class Behavior;

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
        // total size: 0x8
        void Add(Behavior *beh);
        void Remove(Behavior *beh);
        void Reset();
        void OnTaskSimulate(float dT);
        void OnBehaviorChange(const UCrc32 &mechanic);
        void OnOwnerAttached(IAttachable *iother);
        void OnOwnerDetached(IAttachable *iother);
    };

    PhysicsObject(const Attrib::Instance &attribs, SimableType objType, WUID wuid, unsigned int num_interfaces);
    PhysicsObject(const char *attributeClass, const char *attribName, SimableType objType, HSIMABLE owner, WUID wuid);

    // Overrides
    virtual ~PhysicsObject();

    // new virtuals (per DWARF)
    virtual void Reset();
    virtual void OnTaskSimulate(float dT) = 0;
    virtual void OnBehaviorChange(const UCrc32 &mechanic);

    Attrib::Instance &GetModifiableAttributes();

    // Overrides: Sim::Object
    virtual bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt);
    virtual bool OnTask(HSIMTASK htask, float dT);

    // Behaviors
    Behavior *LoadBehavior(const UCrc32 &mechanic, const UCrc32 &behavior, Sim::Param params);
    Behavior *FindBehavior(const UCrc32 &mechanic);
    bool IsBehaviorActive(const UCrc32 &mechanic) const;
    void PauseBehavior(const UCrc32 &mechanic, bool pause);
    bool ResetBehavior(const UCrc32 &mechanic);
    void ReleaseBehaviors();
    void ReleaseBehavior(const UCrc32 &mechanic);
    bool HasBehavior(const UCrc32 &mechanic);
    void DetachAll();

    // Overrides: ISimable
    virtual EventSequencer::IEngine *GetEventSequencer();
    virtual Sim::IEntity *GetEntity() const;
    virtual bool IsPlayer() const;
    virtual IPlayer *GetPlayer() const;
    virtual SimableType GetSimableType() const;
    virtual const Attrib::Instance &GetAttributes() const;
    virtual bool IsRigidBodySimple() const;
    virtual bool IsRigidBodyComplex() const;
    virtual WWorldPos &GetWPos();
    virtual const WWorldPos &GetWPos() const;
    virtual HSIMABLE GetOwnerHandle() const;
    virtual ISimable *GetOwner() const;
    virtual IRigidBody *GetRigidBody();
    virtual const IRigidBody *GetRigidBody() const;
    virtual const UMath::Vector3 &GetPosition() const;
    virtual unsigned int GetWorldID() const;
    virtual void Kill();
    virtual bool Attach(UTL::COM::IUnknown *object);
    virtual bool Detach(UTL::COM::IUnknown *object);
    virtual const UTL::Std::list<IAttachable *, _type_IAttachableList> *GetAttachments() const;
    virtual void AttachEntity(Sim::IEntity *e);
    virtual void DetachEntity();
    virtual bool IsOwnedByPlayer() const;
    virtual bool IsOwnedBy(ISimable *queriedOwner) const;
    virtual void SetOwnerObject(ISimable *pOwner);
    virtual void DebugObject();
    virtual void GetTransform(UMath::Matrix4 &matrix) const;
    virtual void GetLinearVelocity(UMath::Vector3 &velocity) const;
    virtual void GetAngularVelocity(UMath::Vector3 &velocity) const;
    virtual void ProcessStimulus(unsigned int stimulus);
    virtual IModel *GetModel() = 0;
    virtual const IModel *GetModel() const = 0;
    virtual void SetCausality(HCAUSE from, float time);
    virtual HCAUSE GetCausality() const;
    virtual float GetCausalityTime() const;

    // Overrides: IBody
    virtual void GetDimension(UMath::Vector3 &dim) const;

    // Overrides: IAttachable
    virtual bool IsAttached(const UTL::COM::IUnknown *pOther) const;
    virtual void OnAttached(IAttachable *pOther);
    virtual void OnDetached(IAttachable *pOther);

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

inline void PhysicsObject::Reset() {
    mBehaviors.Reset();
}

inline EventSequencer::IEngine *PhysicsObject::GetEventSequencer() {
    return NULL;
}

inline Sim::IEntity *PhysicsObject::GetEntity() const {
    return mEntity;
}

inline bool PhysicsObject::IsPlayer() const {
    return mPlayer != NULL;
}

inline IPlayer *PhysicsObject::GetPlayer() const {
    return mPlayer;
}

inline SimableType PhysicsObject::GetSimableType() const {
    return mObjType;
}

inline const Attrib::Instance &PhysicsObject::GetAttributes() const {
    return mAttributes;
}

inline bool PhysicsObject::IsRigidBodySimple() const {
    if (mRigidBody) {
        return mRigidBody->IsSimple();
    }
    return false;
}

inline bool PhysicsObject::IsRigidBodyComplex() const {
    return !IsRigidBodySimple();
}

inline WWorldPos &PhysicsObject::GetWPos() {
    return *mWPos;
}

inline const WWorldPos &PhysicsObject::GetWPos() const {
    return *mWPos;
}

inline HSIMABLE PhysicsObject::GetOwnerHandle() const {
    return mOwner;
}

inline ISimable *PhysicsObject::GetOwner() const {
    return ISimable::FindInstance(mOwner);
}

inline IRigidBody *PhysicsObject::GetRigidBody() {
    return mRigidBody;
}

inline const IRigidBody *PhysicsObject::GetRigidBody() const {
    return mRigidBody;
}

inline const UMath::Vector3 &PhysicsObject::GetPosition() const {
    return mRigidBody ? mRigidBody->GetPosition() : UMath::Vector3::kZero;
}

inline unsigned int PhysicsObject::GetWorldID() const {
    return mWorldID;
}

inline bool PhysicsObject::IsAttached(const UTL::COM::IUnknown *pOther) const {
    return mAttachments ? mAttachments->IsAttached(pOther) : false;
}

inline const UTL::Std::list<IAttachable *, _type_IAttachableList> *PhysicsObject::GetAttachments() const {
    return mAttachments ? &mAttachments->GetList() : NULL;
}

#endif
