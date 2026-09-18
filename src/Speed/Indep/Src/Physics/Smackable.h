#ifndef PHYSICS_SMACKABLE_H
#define PHYSICS_SMACKABLE_H

#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDisposable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplodeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/rigidbodyspecs.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"

// total size: 0x5C
struct SmackableParams : public Sim::Param {
    // TODO macro
    static UCrc32 TypeName() {
        static UCrc32 value = "SmackableParams";
        return value;
    }

    SmackableParams(const UMath::Matrix4 &mat, bool virginspawn, IModel *scenery, bool simple_physics)
        : Sim::Param(UCrc32(0xa6b47fac), this), fMatrix(mat), fVirginSpawn(virginspawn), fScenery(scenery), fSimplePhysics(simple_physics) {}

    struct UMath::Matrix4 fMatrix; // offset 0x10, size 0x40
    bool fVirginSpawn;             // offset 0x50, size 0x1
    IModel *fScenery;              // offset 0x54, size 0x4
    bool fSimplePhysics;           // offset 0x58, size 0x1
};

// total size: 0x158
class Smackable : public PhysicsObject,
                  public IDisposable,
                  public IRenderable,
                  public Sim::Collision::IListener,
                  public UTL::Collections::Listable<Smackable, 160>,
                  public IExplodeable,
                  public EventSequencer::IContext {
  public:
    typedef UTL::Collections::Listable<Smackable, 160> SmackList;

    USE_FASTALLOC(Smackable);

    // total size: 0x54
    class Manager : public Sim::Activity, public UTL::Collections::Singleton<Smackable::Manager> {
      public:
        USE_FASTALLOC(Manager);

        Manager(float rate);
        ~Manager() override;

        bool OnTask(HSIMTASK htask, float dT) override;

      private:
        HSIMTASK mSortTask; // offset 0x50, size 0x4
    };

    static bool SimplifySort(const Smackable *lhs, const Smackable *rhs) {
        if (lhs->mSimplifyWeight > rhs->mSimplifyWeight)
            return true;
        else if (lhs->mSimplifyWeight < rhs->mSimplifyWeight)
            return false;
        return lhs->GetInstanceHandle() < rhs->GetInstanceHandle();
    }

    // Overrides: IDisposable
    bool IsRequired() const override {
        return false;
    }

    virtual void HidePart(const UCrc32 &name) {}
    virtual void ShowPart(const UCrc32 &name) {}

    virtual bool IsPartVisible(const UCrc32 &name) const {
        return true;
    }

    // Overrides: IRenderable
    HMODEL GetModelHandle() const override {
        return mModel ? mModel->GetInstanceHandle() : NULL;
    }

    // Overrides: ISimable
    const IModel *GetModel() const override {
        return mModel;
    }

    // Overrides: ISimable
    IModel *GetModel() override {
        return mModel;
    }

    // Overrides: ISimable
    EventSequencer::IEngine *GetEventSequencer() override {
        if (mModel) {
            return mModel->GetEventSequencer();
        }
        return NULL;
    }

    static void PrintWeight(Smackable *smack) {}

    bool Simplify();
    static bool TrySimplify();
    static ISimable *Construct(Sim::Param params);

    Smackable(const UMath::Matrix4 &matrix, const Attrib::Gen::smackable &attributes, const CollisionGeometry::Bounds *geoms, bool virginspawn,
              IModel *scenery, bool simple_physics, bool is_persistant);

    // Overrides: IContext
    bool SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) override;

    // Overrides: IExplodeable
    bool OnExplosion(const UMath::Vector3 &normal, const UMath::Vector3 &position, float dT, IExplosion *explosion) override;

    // Overrides: PhysicsObject
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // Overrides: IUnknown
    ~Smackable() override;

    void DoImpactStimulus(unsigned int systemid, float intensity);
    void OnImpact(float acceleration, float speed, Sim::Collision::Info::CollisionType type, ISimable *iother);

    // Overrides: IListener
    void OnCollision(const Sim::Collision::Info &cinfo) override;

    // Overrides: IRenderable
    bool InView() const override;
    bool IsRenderable() const override;
    float DistanceToView() const override;

    // Overrides: ISimable
    void Kill() override;

    bool Dropout();
    bool ValidateWorld();
    bool ShouldDie();
    bool CanRetrigger() const;
    void ProcessDeath(float dT);
    bool ProcessDropout(float dT);
    void ProcessOffWorld(float dT);

    // Overrides: ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    // Overrides: IAttachable
    void OnDetached(IAttachable *pOther) override;

    void CalcSimplificationWeight();
    void Manage(float dT);

    // Overrides: PhysicsObject
    void OnTaskSimulate(float dT) override;

    static const Attrib::StringKey CYLINDER;
    static const Attrib::StringKey TUBE;
    static const Attrib::StringKey CONE;
    static const Attrib::StringKey SPHERE;

  private:
    Attrib::Gen::smackable mAttributes;                    // offset 0xD8, size 0x14
    float mSimplifyWeight;                                 // offset 0xEC, size 0x4
    float mAge;                                            // offset 0xF0, size 0x4
    float mLife;                                           // offset 0xF4, size 0x4
    float mDropTimer;                                      // offset 0xF8, size 0x4
    const float mDropOutTimerMax;                          // offset 0xFC, size 0x4
    float mOffWorldTimer;                                  // offset 0x100, size 0x4
    const float mAutoSimplify;                             // offset 0x104, size 0x4
    const bool mVirgin;                                    // offset 0x108, size 0x1
    IModel *mModel;                                        // offset 0x10C, size 0x4
    const CollisionGeometry::Bounds *mGeometry;            // offset 0x110, size 0x4
    HSIMTASK mManageTask;                                  // offset 0x114, size 0x4
    bool mDroppingOut;                                     // offset 0x118, size 0x1
    bool mPersistant;                                      // offset 0x11C, size 0x1
    ICollisionBody *mCollisionBody;                        // offset 0x120, size 0x4
    ISimpleBody *mSimpleBody;                              // offset 0x124, size 0x4
    UMath::Vector3 mLastImpactSpeed;                       // offset 0x128, size 0xC
    BehaviorSpecsPtr<Attrib::Gen::rigidbodyspecs> mRBSpecs; // offset 0x134, size 0x14
    UMath::Vector4 mLastCollisionPosition;                 // offset 0x148, size 0x10
};

#endif
