#ifndef PHYSICS_SMACKABLE_H
#define PHYSICS_SMACKABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AIAvoidable.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/rigidbodyspecs.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDisposable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplodeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Sim/Collision.h"

class HeirarchyModel;

struct SmackableParams : public Sim::Param {
    SmackableParams(const SmackableParams &_ctor_arg)
        : Sim::Param(_ctor_arg) //
        , fMatrix(_ctor_arg.fMatrix) //
        , fVirginSpawn(_ctor_arg.fVirginSpawn) //
        , fScenery(_ctor_arg.fScenery) //
        , fSimplePhysics(_ctor_arg.fSimplePhysics) //
    {}

    static UCrc32 TypeName() {
        static UCrc32 value = "SmackableParams";
        return value;
    }

    UMath::Matrix4 fMatrix;
    bool fVirginSpawn;
    IModel *fScenery;
    bool fSimplePhysics;
};

class Smackable : public PhysicsObject,
                  public IDisposable,
                  public IRenderable,
                  public Sim::Collision::IListener,
                  public UTL::Collections::Listable<Smackable, 160>,
                  public IExplodeable,
                  public EventSequencer::IContext {
  public:
    class Manager : public Sim::Object, public UTL::Collections::Singleton<Manager> {
      public:
        void *operator new(std::size_t size) { return gFastMem.Alloc(size, nullptr); }
        void operator delete(void *mem, std::size_t size) {
            if (mem) { gFastMem.Free(mem, size, nullptr); }
        }
        Manager(float rate);
        virtual ~Manager();
        bool OnTask(HSIMTASK htask, float dT) override;
    };

    void *operator new(std::size_t size) { return gFastMem.Alloc(size, nullptr); }
    void operator delete(void *mem, std::size_t size) {
        if (mem) { gFastMem.Free(mem, size, nullptr); }
    }

    static ISimable *Construct(Sim::Param params);
    static bool SimplifySort(const Smackable *lhs, const Smackable *rhs);
    static bool TrySimplify();

    Smackable(const UMath::Matrix4 &matrix, const Attrib::Gen::smackable &attributes,
              const CollisionGeometry::Bounds *geoms, bool virginspawn, IModel *scenery,
              bool simple_physics, bool is_persistant);
    virtual ~Smackable();

    bool IsRequired() const override;
    bool InView() const override;
    bool IsRenderable() const override;
    HMODEL GetModelHandle() const override;
    const IModel *GetModel() const override;
    IModel *GetModel() override;
    float DistanceToView() const override;
    void OnCollision(const Sim::Collision::Info &cinfo) override;
    bool OnExplosion(const UMath::Vector3 &normal, const UMath::Vector3 &position, float dT, IExplosion *explosion) override;
    bool SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) override;
    void Kill() override;
    bool OnTask(HSIMTASK htask, float dT) override;
    void OnDetached(IAttachable *pOther) override;
    EventSequencer::IEngine *GetEventSequencer() override;

    virtual void HidePart(const UCrc32 &name);
    virtual void ShowPart(const UCrc32 &name);
    virtual bool IsPartVisible(const UCrc32 &name) const;

  protected:
    bool Simplify();
    void OnTaskSimulate(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    bool Dropout();
    bool ProcessDropout(float dT);
    void ProcessDeath(float dT);
    void ProcessOffWorld(float dT);
    void Manage(float dT);
    bool ValidateWorld();
    bool ShouldDie();
    bool CanRetrigger() const;
    void OnImpact(float acceleration, float speed, Sim::Collision::Info::CollisionType type, ISimable *iother);
    void DoImpactStimulus(unsigned int systemid, float intensity);
    void CalcSimplificationWeight();

    Attrib::Gen::smackable mAttributes;
    float mSimplifyWeight;
    float mAge;
    float mLife;
    float mDropTimer;
    float mDropOutTimerMax;
    float mOffWorldTimer;
    float mAutoSimplify;
    bool mVirgin;
    IModel *mModel;
    const CollisionGeometry::Bounds *mGeometry;
    HSIMTASK mManageTask;
    bool mDroppingOut;
    bool mPersistant;
    ICollisionBody *mCollisionBody;
    ISimpleBody *mSimpleBody;
    UMath::Vector3 mLastImpactSpeed;
    BehaviorSpecsPtr<Attrib::Gen::rigidbodyspecs> mRBSpecs;
    UMath::Vector4 mLastCollisionPosition;
};

class RBSmackable : public RigidBody {
  public:
    static Behavior *Construct(const BehaviorParams &parms);
    RBSmackable(const BehaviorParams &parms, const RBComplexParams &rp);
    virtual ~RBSmackable();
    bool ShouldSleep() const override;
    void OnTaskSimulate(float dT) override;
    bool CanCollideWith(const RigidBody &other) const override;
    bool CanCollideWithGround() const override;
    bool CanCollideWithWorld() const override;

  private:
    BehaviorSpecsPtr<Attrib::Gen::rigidbodyspecs> mSpecs;
    unsigned int mFrame;
};

class SmackableAvoidable : public AIAvoidable {
  public:
    void *operator new(std::size_t size) { return gFastMem.Alloc(size, nullptr); }
    void operator delete(void *mem, std::size_t size) {
        if (mem) { gFastMem.Free(mem, size, nullptr); }
    }
    SmackableAvoidable(HeirarchyModel *model);
    virtual ~SmackableAvoidable() {}
    bool OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) override;

  private:
    HeirarchyModel *mModel;
};

extern Attrib::StringKey BEHAVIOR_MECHANIC_EFFECTS;
extern int Smackable_RigidCount;

#endif
