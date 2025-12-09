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
    class Effect : public Sim::Effect {
      public:
      private:
        const UCrc32 Identifire; // offset 0x5C, size 0x4
    };

    Model(IModel *parent, const CollisionGeometry::Bounds *geometry, UCrc32 nodename, std::size_t numinterfaces);

    // Virtual functions
    // IUnknown
    override virtual ~Model();

    // IContext
    override virtual bool SetDynamicData(const EventSequencer::System *system, EventDynamicData *data);

    // IAttachable
    override virtual bool Attach(IUnknown *pOther);
    override virtual bool Detach(IUnknown *pOther);
    override virtual bool IsAttached(const IUnknown *pOther) const;
    override virtual void OnAttached(IAttachable *pOther);
    override virtual void OnDetached(IAttachable *pOther);
    override virtual const UTL::Std::list<IAttachable *, _type_IAttachableList> *GetAttachments() const;

    // IModel
    override virtual void GetLinearVelocity(UMath::Vector3 &velocity) const;
    override virtual void GetAngularVelocity(UMath::Vector3 &velocity) const;

    override virtual IModel *SpawnModel(UCrc32 rendernode, UCrc32 collisionnode, UCrc32 attributes) {
        return nullptr;
    }

    // IEngine
    override virtual EventSequencer::IEngine *GetEventSequencer() {
        return mSequencer;
    }

    // IModel
    override virtual bool InView() const {
        return mInView;
    }

    override virtual float DistanceToView() const {
        return mDistanceToView;
    }

    override virtual UCrc32 GetPartName() const {
        return UCrc32(mNodeName);
    }

    override virtual bool IsHidden() const;
    override virtual IModel *GetChildModel(UCrc32 name) const;

    override virtual IModel *GetRootModel() const {
        if (mRoot) {
            return mRoot;
        } else {
            return const_cast<Model *>(this);
        }
    }

    override virtual IModel *GetParentModel() const {
        return mParent;
    }

    override virtual WUID GetWorldID() const;
    override virtual const CollisionGeometry::Bounds *GetCollisionGeometry() const;
    override virtual void ReleaseModel();

    override virtual ISimable *GetSimable() const {
        return mSimable;
    }

    override virtual void ReleaseChildModels();

    override virtual bool IsRootModel() const {
        return mIsRoot;
    }

    override virtual void HideModel();

    override virtual void HidePart(const UCrc32 &nodename) {}

    override virtual void ShowPart(const UCrc32 &nodename) {}

    override virtual bool IsPartVisible(const UCrc32 &nodename) const {
        return false;
    }

    override virtual void PlayEffect(UCrc32 identifire, const Attrib::Collection *effect, const UMath::Vector3 &position,
                                     const UMath::Vector3 &magnitude, bool tracking);
    override virtual void StopEffect(UCrc32 identifire);
    override virtual void StopEffects();

    override virtual void SetCausality(HCAUSE from, float time) {
        mCausality = from;
        mCauseTime = time;
    }

    override virtual HCAUSE GetCausality() const {
        return mCausality;
    }

    override virtual float GetCausalityTime() const {
        return mCauseTime;
    }

    override virtual Enumerator *EnumerateChildren(IModel::Enumerator *enumerator) const;

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

    virtual bool OnDraw() {}

    virtual void OnEndDraw() {}

    // IServiceable
    virtual bool OnService(HSIMSERVICE hCon, Packet *pkt);

  private:
    IModel *mParent;                                              // offset 0x50, size 0x4
    IModel *mRoot;                                                // offset 0x54, size 0x4
    UCrc32 mNodeName;                                             // offset 0x58, size 0x4
    const CollisionGeometry::Bounds *mGeometry;                   // offset 0x5C, size 0x4
    ISimable *mSimable;                                           // offset 0x60, size 0x4
    Attachments *mAttachments;                                    // offset 0x64, size 0x4
    float mDistanceToView;                                        // offset 0x68, size 0x4
    bool mInView;                                                 // offset 0x6C, size 0x1
    const bool mIsRoot;                                           // offset 0x70, size 0x1
    EventSequencer::IEngine *mSequencer;                          // offset 0x74, size 0x4
    UTL::Std::vector<IModel *, _type_SimModelChildren> mChildren; // offset 0x78, size 0x10
    HSIMSERVICE mService;                                         // offset 0x88, size 0x4
    HCAUSE mCausality;                                            // offset 0x8C, size 0x4
    float mCauseTime;                                             // offset 0x90, size 0x4
    bTList<Model::Effect> mEffects;                               // offset 0x94, size 0x8
};

}; // namespace Sim

#endif
