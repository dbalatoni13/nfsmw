#ifndef PHYSICS_BEHAVIORS_DRAWVEHICLE_H
#define PHYSICS_BEHAVIORS_DRAWVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/effects.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ITriggerableModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngineDamage.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Physics/SmackableTrigger.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/SimModel.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"

namespace RenderConn {
class Pkt_Car_Service;
class Pkt_Heli_Service;
}

// total size: 0x88
class DrawVehicle : public VehicleBehavior, public IRenderable, public IModel, public IAttachable {
  public:
    // total size: 0x60
    class Effect : public Sim::Effect {
      public:
        Effect(UCrc32 id, unsigned int owner, const Attrib::Collection *parent)
            : Sim::Effect(owner, parent), //
              Identifire(id) {}

        ~Effect() override;

        const UCrc32 Identifire; // offset 0x5C, size 0x4
    };

    // total size: 0xCC
    class Part : public Sim::Model, public ITriggerableModel {
      public:
        enum State {
            S_NONE = 0,
            S_DRAW = 1,
            S_HIDE = 2,
            S_TRIGGER = 3
        };

        Part(IModel *parent, WUID vehicleID, const CollisionGeometry::Bounds *geoms, const Attrib::Collection *spec, UCrc32 partname);
        ~Part() override;
        void GetTransform(UMath::Matrix4 &transform) const override;
        void OnProcessFrame(float dT) override;
        void PlaceTrigger(const UMath::Matrix4 &mat, bool retrigger) override;
        const Attrib::Instance &GetAttributes() const override;

      protected:
        void OnBeginSimulation() override;
        bool OnDraw(Sim::Packet *service) override;
        void OnBeginDraw() override;
        void OnEndDraw() override;

      private:
        void CreateTrigger(const UMath::Matrix4 &matrix);
        void RemoveTrigger();

        State mState;                       // offset 0xA4, size 0x4
        SmackableTrigger *mTrigger;         // offset 0xA8, size 0x4
        Attrib::Gen::smackable mAttributes; // offset 0xAC, size 0x14
        bool mOffScreenTask;                // offset 0xC0, size 0x1
        float mOffScreenTime;               // offset 0xC4, size 0x4
        WUID mVehicleID;                    // offset 0xC8, size 0x4
    };

    typedef UTL::Std::list<DrawVehicle::Effect *, _type_list> EffectList;

    HMODEL GetModelHandle() const override;
    const IModel *GetModel() const override;
    IModel *GetModel() override;
    void OnProcessFrame(float dT) override;
    void GetLinearVelocity(UMath::Vector3 &velocity) const override;
    void GetAngularVelocity(UMath::Vector3 &velocity) const override;
    void GetTransform(UMath::Matrix4 &transform) const override;
    UCrc32 GetPartName() const override;
    WUID GetWorldID() const override;
    const CollisionGeometry::Bounds *GetCollisionGeometry() const override;
    const Attrib::Instance &GetAttributes() const override;
    ISimable *GetSimable() const override;
    IModel *GetRootModel() const override;
    IModel *GetParentModel() const override;
    bool IsRootModel() const override;
    EventSequencer::IEngine *GetEventSequencer() override;
    float GetCausalityTime() const override;
    bool Attach(UTL::COM::IUnknown *pOther) override;
    bool Detach(UTL::COM::IUnknown *pOther) override;
    bool IsAttached(const UTL::COM::IUnknown *pOther) const override;
    void OnAttached(IAttachable *pOther) override;
    void OnDetached(IAttachable *pOther) override;
    const IAttachable::List *GetAttachments() const override;

    DrawVehicle(const BehaviorParams &bp);
    ~DrawVehicle() override;
    void SetCausality(HCAUSE from, float time) override;
    HCAUSE GetCausality() const override;
    Enumerator *EnumerateChildren(Enumerator *enumerator) const override;
    IModel *GetChildModel(UCrc32 name) const override;
    IModel *SpawnModel(UCrc32 name, UCrc32 collisionnode, UCrc32 attributes) override;
    void ReleaseModel() override;
    void ReleaseChildModels() override;
    virtual UCrc32 GetName() const;
    bool IsHidden() const override;
    void HideModel() override;
    void StopEffects() override;
    void PlayEffect(UCrc32 identifire, const Attrib::Collection *effect, const UMath::Vector3 &position, const UMath::Vector3 &magnitude,
                    bool tracking) override;
    void StopEffect(UCrc32 identifire) override;

  protected:
    const CollisionGeometry::Bounds *mGeometry; // offset 0x70, size 0x4
    Sim::Attachments *mAttachments;             // offset 0x74, size 0x4
    EffectList mEffects;                        // offset 0x78, size 0x8
    HCAUSE mCausality;                          // offset 0x80, size 0x4
    float mCausalityTime;                       // offset 0x84, size 0x4
};

// total size: 0x114
class DrawHeli : public DrawVehicle {
  public:
    typedef DrawVehicle Base;

    void HidePart(const UCrc32 &name) override;
    void ShowPart(const UCrc32 &name) override;
    bool IsPartVisible(const UCrc32 &name) const override;
    bool InView() const override;
    bool IsRenderable() const override;
    float DistanceToView() const override;
    void Reset() override;
    void OnTaskSimulate(float dT) override;

    static Behavior *Construct(const BehaviorParams &params);

    DrawHeli(const BehaviorParams &params);
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    ~DrawHeli() override;
    bool OnTask(HSIMTASK hTask, float dT) override;
    void OnService(RenderConn::Pkt_Heli_Service &pkt);
    bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt) override;

  private:
    HSIMSERVICE mRenderService;        // offset 0x88, size 0x4
    Sim::Effect mEffect;               // offset 0x8C, size 0x5C
    Attrib::Gen::effects mWash;        // offset 0xE8, size 0x14
    HSIMTASK mWashTask;                // offset 0xFC, size 0x4
    bool mInView;                      // offset 0x100, size 0x1
    float mDistanceToView;             // offset 0x104, size 0x4
    IAIHelicopter *mIAIHelicopter;     // offset 0x108, size 0x4
    IVehicle *mIVehicle;               // offset 0x10C, size 0x4
    ICollisionBody *mCollisionBody;    // offset 0x110, size 0x4
};

// total size: 0xC4
class DrawCar : public DrawVehicle {
  public:
    typedef DrawVehicle Base;
    typedef UTL::Std::map<UCrc32, int, _type_map> Parts;

    bool InView() const override;
    bool IsRenderable() const override;
    float DistanceToView() const override;
    void Reset() override;
    void OnTaskSimulate(float dT) override;

    DrawCar(const BehaviorParams &params, CarRenderUsage usage);
    bool IsHidden() const override;
    void HideModel() override;
    void ReleaseModel() override;
    void ReleaseChildModels() override;
    bool IsPartVisible(const UCrc32 &name) const override;
    void HidePart(const UCrc32 &name) override;
    void ShowPart(const UCrc32 &name) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    ~DrawCar() override;
    virtual void OnService(RenderConn::Pkt_Car_Service &pkt);
    bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt) override;

  private:
    HSIMSERVICE mRenderService;          // offset 0x88, size 0x4
    ISuspension *mSuspension;            // offset 0x8C, size 0x4
    IDamageable *mDamage;                // offset 0x90, size 0x4
    IVehicle *mVehicle;                  // offset 0x94, size 0x4
    IDamageableVehicle *mVehicleDamage;  // offset 0x98, size 0x4
    IRBVehicle *mRBVehicle;              // offset 0x9C, size 0x4
    INISCarControl *mNIS;                // offset 0xA0, size 0x4
    ISpikeable *mSpikeDamage;            // offset 0xA4, size 0x4
    Parts mParts;                        // offset 0xA8, size 0x10
    bool mInView;                        // offset 0xB8, size 0x1
    float mDistanceToView;               // offset 0xBC, size 0x4
    bool mHidden;                        // offset 0xC0, size 0x1
};

// total size: 0xC4
class DrawTraffic : public DrawCar {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    DrawTraffic(const BehaviorParams &params);
};

// total size: 0xCC
class DrawPerformanceCar : public DrawCar {
  public:
    DrawPerformanceCar(const BehaviorParams &params, CarRenderUsage usage);
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void OnService(RenderConn::Pkt_Car_Service &pkt) override;

  private:
    ITransmission *mTransmission; // offset 0xC4, size 0x4
    IEngine *mEngine;             // offset 0xC8, size 0x4
};

// total size: 0xCC
class DrawNISCar : public DrawPerformanceCar {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    DrawNISCar(const BehaviorParams &params);
};

// total size: 0xCC
class DrawCopCar : public DrawPerformanceCar {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    DrawCopCar(const BehaviorParams &params);
};

// total size: 0xD0
class DrawRaceCar : public DrawPerformanceCar {
  public:
    static Behavior *Construct(const BehaviorParams &params);

    DrawRaceCar(const BehaviorParams &params, CarRenderUsage usage);
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void OnService(RenderConn::Pkt_Car_Service &pkt) override;

  private:
    IEngineDamage *mEngineDamage; // offset 0xCC, size 0x4
};

#endif
