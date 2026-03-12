#ifndef PHYSICS_PVEHICLE_H
#define PHYSICS_PVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Interfaces/IAttributeable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplodeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"

DECLARE_CONTAINER_TYPE(ID_PVehicleChangeReq);

struct FECustomizationRecord;
class IInput;
class ICollisionBody;
class ISuspension;
class IEngine;
class IDamageable;
class ITransmission;
class IVehicleAI;
class IArticulatedVehicle;
class IRenderable;
class IAudible;

struct VehicleParams : public Sim::Param {
    DriverClass carClass;
    unsigned int carType;
    const UMath::Vector3 &initialVec;
    const UMath::Vector3 &initialPos;
    const FECustomizationRecord *customization;
    IVehicleCache *VehicleCache;
    const Physics::Info::Performance *matched;
    unsigned int Flags;

    VehicleParams(IVehicleCache *cache, DriverClass driver, unsigned int type, const UMath::Vector3 &dir, const UMath::Vector3 &pos,
                  unsigned int flags, const FECustomizationRecord *upgrades, const Physics::Info::Performance *match)
        : Sim::Param(UCrc32(0xa6b47fac), this), //
          carClass(driver),                     //
          carType(type),                        //
          initialVec(dir),                      //
          initialPos(pos),                      //
          customization(upgrades),              //
          VehicleCache(cache),                  //
          matched(match),                       //
          Flags(flags) {}

    static UCrc32 TypeName() {
        static UCrc32 value = "VehicleParams";
        return value;
    }
};

class PVehicle : public PhysicsObject,
                 public bTNode<PVehicle>,
                 public IVehicle,
                 public Debugable,
                 public EventSequencer::IContext,
                 public IExplodeable,
                 public IAttributeable {
  public:
    struct LaunchState {
        LaunchState() : Time(0.0f), Amount(0.0f) {}
        void Clear() { Time = 0.0f; Amount = 0.0f; }
        bool IsSet() const { return Time > 0.0f; }
        void Set(float time) { Time = time; Amount = 0.0f; }
        void Tick(float dT) {
            Time -= dT;
        }

        float Time;   // offset 0x0, size 0x4
        float Amount; // offset 0x4, size 0x4
    };

    struct Resource {
        enum eFlags {
            VALID = 1,
            SPOOL = 2,
            NEEDS_COMPOSITING = 4,
        };

        Resource() : Type(CARTYPE_NONE), Cost(0), Flags(0) {}
        Resource(const Attrib::Gen::pvehicle &pvehicle, bool spool, bool is_player);
        bool NeedsCompositing() const { return (Flags & NEEDS_COMPOSITING) != 0; }
        bool IsValid() const { return (Flags & VALID) != 0; }
        bool IsSpooled() const { return (Flags & SPOOL) != 0; }
        void Invalidate() { Flags &= ~VALID; }

        CarType Type;        // offset 0x0, size 0x4
        unsigned int Cost;   // offset 0x4, size 0x4
        unsigned int Flags;  // offset 0x8, size 0x4
    };

    struct ManageNode {
        ManageNode() {}
        static void print(const ManageNode &n) {}
        static bool sort_remove_resources(const ManageNode &lhs, const ManageNode &rhs) {
            if (rhs.resource.Type != lhs.resource.Type) {
                if (lhs.instancecount < rhs.instancecount) {
                    return true;
                }
                if (lhs.resource.Cost > rhs.resource.Cost) {
                    return true;
                }
            }
            return lhs.resource.Type < rhs.resource.Type;
        }
        static bool sort_remove_instances(const ManageNode &lhs, const ManageNode &rhs) {
            if (rhs.resource.Type != lhs.resource.Type) {
                if (lhs.instancecount > rhs.instancecount) {
                    return true;
                }
                if (lhs.resource.Cost > rhs.resource.Cost) {
                    return true;
                }
            }
            return lhs.resource.Type < rhs.resource.Type;
        }
        static bool sort_by_keep(const ManageNode &lhs, const ManageNode &rhs) {
            if (rhs.resource.Type == lhs.resource.Type) {
                if (lhs.result == VCR_WANT) {
                    if (rhs.result == VCR_DONTCARE) {
                        return true;
                    }
                }
                return false;
            }
            return lhs.resource.Type < rhs.resource.Type;
        }
        static bool is_kept(const ManageNode &h) {
            return h.result == VCR_WANT;
        }

        PVehicle *vehicle;            // offset 0x0, size 0x4
        Resource resource;            // offset 0x4, size 0xC
        eVehicleCacheResult result;   // offset 0x10, size 0x4
        unsigned int instancecount;   // offset 0x14, size 0x4
    };

    struct ManagementList : public UTL::FixedVector<ManageNode, 10, 16> {
        void print() const {}
    };

    typedef UTL::Std::list<Resource, _type_list> ResourceList;
    typedef UTL::Std::map<UCrc32, UCrc32, _type_ID_PVehicleChangeReq> ChangeRequest;


    PVehicle(DriverClass dc, const Attrib::Gen::pvehicle &attribs, const UMath::Vector3 &initialPos,
             const UMath::Vector3 &initialVec, const CollisionGeometry::Bounds *bounds,
             const FECustomizationRecord *customization, const Resource &resource,
             const Physics::Info::Performance *matched, const char *cacheName);
    virtual ~PVehicle();

    static ISimable *Construct(Sim::Param params);
    static bool MakeRoom(IVehicleCache *cache, const UTL::Std::list<Resource, _type_list> &resources);
    static void CleanResources();
    static unsigned int CountResources();

    virtual void Kill() override;
    virtual void Reset() override;
    virtual void DebugObject() override;
    virtual bool OnTask(HSIMTASK htask, float dT) override;
    virtual void OnTaskSimulate(float dT) override;
    virtual void OnBehaviorChange(const UCrc32 &mechanic) override;
    virtual EventSequencer::IEngine *GetEventSequencer() override;
    virtual IModel *GetModel() override;
    virtual const IModel *GetModel() const override;
    virtual const UMath::Vector3 &GetPosition() const override;
    virtual void OnDebugDraw();
    virtual const ISimable *GetSimable() const override;
    virtual ISimable *GetSimable() override;
    virtual Attrib::Gen::pvehicle &GetVehicleAttributes() const override;
    virtual const UCrc32 &GetVehicleClass() const override;
    virtual const char *GetVehicleName() const override;
    virtual unsigned int GetVehicleKey() const override;
    virtual void SetDriverClass(DriverClass dc) override;
    virtual DriverClass GetDriverClass() const override;
    virtual void SetDriverStyle(DriverStyle style) override;
    virtual DriverStyle GetDriverStyle() const override;
    virtual void SetBehaviorOverride(UCrc32 mechanic, UCrc32 behavior) override;
    virtual void RemoveBehaviorOverride(UCrc32 mechanic) override;
    virtual void CommitBehaviorOverrides() override;
    virtual bool SetVehicleOnGround(const UMath::Vector3 &resetPos, const UMath::Vector3 &initialVec) override;
    virtual char GetForceStop() override;
    virtual void ForceStopOn(char forceStopBits) override;
    virtual void ForceStopOff(char forceStopBits) override;
    virtual bool IsOffWorld() const override;
    virtual CarType GetModelType() const override;
    virtual bool IsSpooled() const override;
    virtual void SetStaging(bool staging) override;
    virtual bool IsStaging() const override;
    virtual void Launch() override;
    virtual float GetPerfectLaunch() const override;
    virtual bool IsLoading() const override;
    virtual float GetOffscreenTime() const override;
    virtual float GetOnScreenTime() const override;
    virtual bool InShock() const override;
    virtual bool IsDestroyed() const override;
    virtual float GetAbsoluteSpeed() const override;
    virtual float GetSpeedometer() const override;
    virtual float GetSpeed() const override;
    virtual void SetSpeed(float speed) override;
    virtual void GlareOn(VehicleFX::ID glare) override;
    virtual void GlareOff(VehicleFX::ID glare) override;
    virtual bool IsGlareOn(VehicleFX::ID glare) override;
    virtual bool IsCollidingWithSoftBarrier() override;
    virtual bool IsAnimating() const override;
    virtual void SetAnimating(bool animate) override;
    virtual void Activate() override;
    virtual void Deactivate() override;
    virtual bool IsActive() const override;
    virtual void SetPhysicsMode(PhysicsMode mode) override;
    virtual PhysicsMode GetPhysicsMode() const override;
    virtual IVehicleAI *GetAIVehiclePtr() const override;
    virtual float GetSlipAngle() const override;
    virtual const UMath::Vector3 &GetLocalVelocity() const override;
    virtual const FECustomizationRecord *GetCustomizations() const override;
    virtual const Physics::Tunings *GetTunings() const override;
    virtual void SetTunings(const Physics::Tunings &tunings) override;
    virtual void ComputeHeading(UMath::Vector3 *v) override;
    virtual bool GetPerformance(Physics::Info::Performance &performance) const override;
    virtual const char *GetCacheName() const override;
    virtual bool OnExplosion(const UMath::Vector3 &normal, const UMath::Vector3 &position, float dT, IExplosion *explosion) override;
    virtual bool SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) override;
    virtual void OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey) override;

    static bTList<PVehicle> mInstances;

  private:
    void OnBeginMode(PhysicsMode mode);
    void OnEndMode(PhysicsMode mode);
    void DoDebug(float dT);
    void DoStaging(float dT);
    void CheckOffWorld();
    void LoadBehaviors(const UMath::Vector3 &pos, const UMath::Matrix4 &matrix);
    UCrc32 LookupBehaviorSignature(const Attrib::StringKey &mechanic) const;
    void ReloadBehaviors();
    void UpdateLocalVelocities();
    void OnDisableModeling();
    void OnEnableModeling();
    void UpdateListing();
    void OnTaskFX(float dT);

    Attrib::Gen::pvehicle mAttributes;
    FECustomizationRecord *mCustomization;
    IInput *mInput;
    ICollisionBody *mCollisionBody;
    ISuspension *mSuspension;
    IEngine *mEngine;
    IDamageable *mDamage;
    ITransmission *mTranny;
    IVehicleAI *mAI;
    IArticulatedVehicle *mArticulation;
    IRenderable *mRenderable;
    IAudible *mAudible;
    EventSequencer::IEngine *mSequencer;
    HSIMTASK mTaskFX;
    UCrc32 mClass;
    float mSpeed;
    float mAbsSpeed;
    float mSpeedometer;
    float mTimeInAir;
    float mSlipAngle;
    unsigned int mWheelsOnGround;
    UMath::Vector3 mLocalVel;
    DriverClass mDriverClass;
    DriverStyle mDriverStyle;
    unsigned int mGlareState;
    float mStartingNOS;
    float mBrakeTime;
    char mForceStop;
    PhysicsMode mPhysicsMode;
    bool mAnimating;
    bool mStaging;
    LaunchState mPerfectLaunch;
    UTL::Std::map<UCrc32, UCrc32, _type_ID_PVehicleChangeReq> mBehaviorOverrides;
    bool mOverrideDirty;
    const CollisionGeometry::Bounds *mBounds;
    bool mIsModeling;
    float mOffScreenTime;
    float mOnScreenTime;
    bool mOffWorld;
    static bool mRunDyno;
    bool mHasDyno;
    Resource mResources;
    Physics::Info::Performance mPerformance;
    bool mPerformanceValid;
    const char *mCacheName;
};

#endif
