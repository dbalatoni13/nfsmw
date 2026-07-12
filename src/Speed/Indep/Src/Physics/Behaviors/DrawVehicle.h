#ifndef DRAW_VEHICLE_H
#define DRAW_VEHICLE_H

#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ITriggerableModel.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Sim/SimEffect.h"
#include "Speed/Indep/Src/Sim/SimModel.h"
#include "Speed/Indep/Src/Physics/SmackableTrigger.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"

// total size: 0x88
class DrawVehicle : public VehicleBehavior, public IRenderable, public IModel, IAttachable {
  public:
    DrawVehicle(const BehaviorParams &bp);
    ~DrawVehicle() override;

    // Overrides: IRenderable
    HMODEL GetModelHandle() const override {
        return this->GetInstanceHandle();
    }
    const IModel *GetModel() const override {
        return this;
    }
    IModel *GetModel() override {
        return this;
    }

    // Overrides: IModel
    bool IsHidden() const override;
    void OnProcessFrame(float dT) override {}
    void GetLinearVelocity(UMath::Vector3 &velocity) const override {
        this->GetOwner()->GetLinearVelocity(velocity);
    }
    void GetAngularVelocity(UMath::Vector3 &velocity) const override {
        this->GetOwner()->GetAngularVelocity(velocity);
    }
    void GetTransform(UMath::Matrix4 &transform) const override {
        return this->GetOwner()->GetTransform(transform);
    }
    void ReleaseChildModels() override;

    virtual UCrc32 GetName() const;

    // Overrides: IModel
    UCrc32 GetPartName() const override {
        return UCrc32::kNull;
    }
    WUID GetWorldID() const override {
        return this->GetOwner()->GetWorldID();
    }
    const CollisionGeometry::Bounds *GetCollisionGeometry() const override {
        return this->mGeometry;
    }
    const Attrib::Instance &GetAttributes() const override {
        return this->GetOwner()->GetAttributes();
    }
    void ReleaseModel() override;
    ISimable *GetSimable() const override {
        return this->GetOwner();
    }
    IModel *GetChildModel(UCrc32 name) const override;
    IModel *GetRootModel() const override {
        return const_cast<DrawVehicle *>(this);
    }
    IModel *GetParentModel() const override {
        return nullptr;
    }
    IModel *SpawnModel(UCrc32 name, UCrc32 collisionnode, UCrc32 attributes) override;
    bool IsRootModel() const override {
        return true;
    }
    void HideModel() override;
    EventSequencer::IEngine *GetEventSequencer() override {
        return this->GetOwner()->GetEventSequencer();
    }
    void PlayEffect(UCrc32 identifire, const Attrib::Collection *effect, const UMath::Vector3 &position, const UMath::Vector3 &magnitude,
                    bool tracking) override;
    void StopEffect(UCrc32 identifire) override;
    void StopEffects() override;
    void SetCausality(HCAUSE from, float time) override;
    HCAUSE GetCausality() const override;
    float GetCausalityTime() const override {
        return this->mCausalityTime;
    }
    Enumerator *EnumerateChildren(Enumerator *enumerator) const override;

    // Overrides: IAttachable
    bool Attach(UTL::COM::IUnknown *pOther) override {
        return this->mAttachments->Attach(pOther);
    }
    bool Detach(UTL::COM::IUnknown *pOther) override {
        return this->mAttachments->Detach(pOther);
    }
    bool IsAttached(const UTL::COM::IUnknown *pOther) const override {
        return this->mAttachments->IsAttached(pOther);
    }
    void OnAttached(IAttachable *pOther) override {}
    void OnDetached(IAttachable *pOther) override {}
    const IAttachable::List *GetAttachments() const override {
        return &this->mAttachments->GetList();
    }

  private:
    CollisionGeometry::Bounds *mGeometry; // offset 0x70, size 0x4
    Sim::Attachments *mAttachments;       // offset 0x74, size 0x4

    class Part : public Sim::Model, public ITriggerableModel {
      public:
        Part(IModel *parent, unsigned int vehicleID, const CollisionGeometry::Bounds *geoms, const Attrib::Collection *spec, UCrc32 partname);
        ~Part() override;

        // Overrides: IModel
        void GetTransform(UMath::Matrix4 &transform) const override;

        // Overrides: ITriggerableModel
        void PlaceTrigger(const UMath::Matrix4 &matrix, bool enable) override;

        // Overrides: IModel
        const Attrib::Instance &GetAttributes() const override {
            return this->mAttributes;
        }
        void OnProcessFrame(float dT) override;
        void OnBeginSimulation() override;
        void OnBeginDraw() override;
        bool OnDraw(Sim::Packet *pkt) override;
        void OnEndDraw() override;

      private:
        enum State {
            S_NONE = 0,
            S_DRAW = 1,
            S_HIDE = 2,
            S_TRIGGER = 3,
        };

        void SetState(State);
        void CreateTrigger(const UMath::Matrix4 &matrix);
        void RemoveTrigger();

      private:
        State mState;                       // offset 0xA4, size 0x4
        SmackableTrigger *mTrigger;         // offset 0xA8, size 0x4
        Attrib::Gen::smackable mAttributes; // offset 0xAC, size 0x14
        bool mOffScreenTask;                // offset 0xC0, size 0x1
        float mOffScreenTime;               // offset 0xC4, size 0x4
        WUID mVehicleID;                    // offset 0xC8, size 0x4
    };

    struct Effect : public Sim::Effect {
        UCrc32 Identifire; // offset 0x5C, size 0x4
        Effect(UCrc32 id, WUID owner, const Attrib::Collection *parent)
            : Sim::Effect(owner, parent), //
              Identifire(id) {}

        ~Effect() override {}
    };

    typedef UTL::Std::list<DrawVehicle::Effect *, _type_list> EffectList;

    EffectList mEffects;  // offset 0x78, size 0x8
    HCAUSE mCausality;    // offset 0x80, size 0x4
    float mCausalityTime; // offset 0x84, size 0x4
};

#endif
