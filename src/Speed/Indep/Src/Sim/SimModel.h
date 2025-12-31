#ifndef SIM_SIMMODEL_H
#define SIM_SIMMODEL_H

#include "Speed/Indep/Src/World/WorldTypes.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SimAttachable.h"
#include "SimObject.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Sim/SimEffect.h"

struct _type_SimModelChildren {
    const char *name() {
        return "SimModelChildren";
    }
};

namespace Sim {

// total size: 0x9C
class Model : public Sim::Object,
              public IAttachable,
              public IModel,
              public EventSequencer::IContext,
              public UTL::Collections::GarbageNode<Model, 434> {
  public:
    // total size: 0x60
    struct Effect : public Sim::Effect {
        Effect(UCrc32 id, WUID owner, const Attrib::Collection *parent) : Sim::Effect(owner, parent), Identifire(id) {}

        ~Effect() {}

        ALIGN_XENON(16) const UCrc32 Identifire; // offset 0x5C, size 0x4
    };

    Model(IModel *parent, const CollisionGeometry::Bounds *geometry, UCrc32 nodename, std::size_t numinterfaces);

    // Virtual functions
    // IUnknown
    ~Model() override;

    // IContext
    bool SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) override;

    // IAttachable
    bool Attach(IUnknown *pOther) override;
    bool Detach(IUnknown *pOther) override;
    bool IsAttached(const IUnknown *pOther) const override;
    void OnAttached(IAttachable *pOther) override;
    void OnDetached(IAttachable *pOther) override;
    const UTL::Std::list<IAttachable *, _type_IAttachableList> *GetAttachments() const override;

    // IModel
    void GetLinearVelocity(UMath::Vector3 &velocity) const override;
    void GetAngularVelocity(UMath::Vector3 &velocity) const override;

    IModel *SpawnModel(UCrc32 rendernode, UCrc32 collisionnode, UCrc32 attributes) override {
        return nullptr;
    }

    // IEngine
    EventSequencer::IEngine *GetEventSequencer() override {
        return mSequencer;
    }

    // IModel
    bool InView() const override {
        return mInView;
    }

    float DistanceToView() const override {
        return mDistanceToView;
    }

    UCrc32 GetPartName() const override {
        return UCrc32(mNodeName);
    }

    bool IsHidden() const override;
    IModel *GetChildModel(UCrc32 name) const override;

    IModel *GetRootModel() const override {
        if (mRoot) {
            return mRoot;
        } else {
            return const_cast<Model *>(this);
        }
    }

    IModel *GetParentModel() const override {
        return mParent;
    }

    WUID GetWorldID() const override;
    const CollisionGeometry::Bounds *GetCollisionGeometry() const override;
    void ReleaseModel() override;

    ISimable *GetSimable() const override {
        return mSimable;
    }

    void ReleaseChildModels() override;

    bool IsRootModel() const override {
        return mIsRoot;
    }

    void HideModel() override;

    void HidePart(const UCrc32 &nodename) override {}

    void ShowPart(const UCrc32 &nodename) override {}

    bool IsPartVisible(const UCrc32 &nodename) const override {
        return false;
    }

    void PlayEffect(UCrc32 identifire, const Attrib::Collection *effect, const UMath::Vector3 &position, const UMath::Vector3 &magnitude,
                    bool tracking) override;
    void StopEffect(UCrc32 identifire) override;
    void StopEffects() override;

    void SetCausality(HCAUSE from, float time) override {
        mCausality = from;
        mCauseTime = time;
    }

    HCAUSE GetCausality() const override {
        return mCausality;
    }

    float GetCausalityTime() const override {
        return mCauseTime;
    }

    Enumerator *EnumerateChildren(IModel::Enumerator *enumerator) const override;

    typedef UTL::Std::vector<IModel *, _type_SimModelChildren> Children;

  protected:
    void StartSequencer(UCrc32 name);
    void ReleaseSequencer();
    void BeginDraw(UCrc32 service, Packet *what);
    void EndDraw();
    void EndSimulation();

    void UpdateVisibility(bool visible) {}

    bool IsRendering() const {}

    bool IsSimulating() const {}

    virtual void OnBeginSimulation() {}

    virtual void OnEndSimulation() {}

    virtual void OnBeginDraw() {}

    virtual bool OnDraw(Packet *service);

    virtual void OnEndDraw() {}

    // IServiceable
    bool OnService(HSIMSERVICE hCon, Packet *pkt) override;

  private:
    IModel *mParent;                            // offset 0x50, size 0x4
    IModel *mRoot;                              // offset 0x54, size 0x4
    UCrc32 mNodeName;                           // offset 0x58, size 0x4
    const CollisionGeometry::Bounds *mGeometry; // offset 0x5C, size 0x4
    ISimable *mSimable;                         // offset 0x60, size 0x4
    Attachments *mAttachments;                  // offset 0x64, size 0x4
    float mDistanceToView;                      // offset 0x68, size 0x4
    bool mInView;                               // offset 0x6C, size 0x1
    const bool mIsRoot;                         // offset 0x70, size 0x1
    EventSequencer::IEngine *mSequencer;        // offset 0x74, size 0x4
    Children mChildren;                         // offset 0x78, size 0x10
    HSIMSERVICE mService;                       // offset 0x88, size 0x4
    HCAUSE mCausality;                          // offset 0x8C, size 0x4
    float mCauseTime;                           // offset 0x90, size 0x4
    bTList<Model::Effect> mEffects;             // offset 0x94, size 0x8
};

}; // namespace Sim

#endif
