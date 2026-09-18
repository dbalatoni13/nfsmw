#include "Speed/Indep/Src/Physics/PVehicle.h"

#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Libs/Support/Utility/UTLVector.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Generated/Events/EPerfectLaunch.hpp"
#include "Speed/Indep/Src/Generated/Events/EPlayerAirborne.hpp"
#include "Speed/Indep/Src/Generated/Messages/MJumpCut.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IOnlinePlayer.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/IAttributeable.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAudible.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEffects.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICause.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplodeable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Sim/Util.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/VehicleFX.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

PresetCar *FindFEPresetCar(unsigned int key);

extern bool Tweak_UseTweakerTunings;
extern float Tweak_TuningAero;

DECLARE_CONTAINER_TYPE(ID_PVehicleChangeReq);

// total size: 0x1C0
class PVehicle : public PhysicsObject,
                 public bTNode<PVehicle>,
                 public IVehicle,
                 public Debugable,
                 public EventSequencer::IContext,
                 public IExplodeable,
                 public IAttributeable {
  public:
    // total size: 0x8
    struct LaunchState {
        // El orden de declaracion decide el inlining en GCC 2.9: con Clear()
        // declarado DESPUES, el ctor emite un `bl Clear__...`; el original lo
        // expande en linea (dos `stfs`), asi que Clear() va primero.
        void Clear() {
            Time = 0.0f;
            Amount = 0.0f;
        }

        LaunchState() {
            Clear();
        }

        bool IsSet() const {
            return Time > 0.0f;
        }

        void Set(float time) {
            Time = time;
        }

        void Tick(float dT) {
            Time -= dT;

            if (Time <= 0.0f) {

                Amount = 0.0f;
                Time = 0.0f;
            }
        }

        float Time;   // offset 0x0, size 0x4
        float Amount; // offset 0x4, size 0x4
    };

    // total size: 0xC
    struct Resource {
        enum eFlags {
            VALID = 1,
            SPOOL = 2,
            NEEDS_COMPOSITING = 4,
        };

        Resource() : Flags(0) {}

        bool NeedsCompositing() const {
            return (Flags & NEEDS_COMPOSITING) != 0;
        }

        bool IsValid() const {
            return (Flags & VALID) != 0;
        }

        bool IsSpooled() const {
            return (Flags & SPOOL) != 0;
        }

        void Invalidate() {
            Flags &= ~VALID;
        }

        Resource(const Attrib::Gen::pvehicle &pvehicle, bool spool, bool is_player);

        CarType Type;       // offset 0x0, size 0x4
        unsigned int Cost;  // offset 0x4, size 0x4
        unsigned int Flags; // offset 0x8, size 0x4
    };

    // total size: 0x18
    struct ManageNode {
        ManageNode() : result(VCR_DONTCARE), instancecount(0) {}

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
                if (lhs.result == VCR_WANT && rhs.result == VCR_DONTCARE) {
                    return true;
                }

                return false;
            }

            return lhs.resource.Type < rhs.resource.Type;
        }

        static bool is_kept(const ManageNode &h) {
            return h.result == VCR_WANT;
        }

        PVehicle *vehicle;              // offset 0x0, size 0x4
        Resource resource;              // offset 0x4, size 0xC
        eVehicleCacheResult result;     // offset 0x10, size 0x4
        unsigned int instancecount;     // offset 0x14, size 0x4
    };

    // total size: 0x100
    struct ManagementList : public UTL::FixedVector<ManageNode, 10, 16> {
        ManagementList() {
            reserve(10);
        }
        ~ManagementList() {}
    };

    typedef UTL::Std::list<Resource, _type_list> ResourceList;
    typedef UTL::Std::map<UCrc32, UCrc32, _type_ID_PVehicleChangeReq> ChangeRequest;

    USE_FASTALLOC(PVehicle);

    const UMath::Vector3 &GetPosition() const {
        return PhysicsObject::GetPosition();
    }
    const ISimable *GetSimable() const {
        return this;
    }
    ISimable *GetSimable() {
        return this;
    }
    Attrib::Gen::pvehicle &GetVehicleAttributes() const {
        return const_cast<Attrib::Gen::pvehicle &>(mAttributes);
    }
    const UCrc32 &GetVehicleClass() const {
        return mClass;
    }
    const char *GetVehicleName() const {
        return mAttributes.CollectionName();
    }
    unsigned int GetVehicleKey() const {
        return mAttributes.GetCollection();
    }

    DriverClass GetDriverClass() const {
        return mDriverClass;
    }

    DriverStyle GetDriverStyle() const {
        return mDriverStyle;
    }

    char GetForceStop() {
        return mForceStop;
    }

    bool IsOffWorld() const {
        return mOffWorld;
    }
    CarType GetModelType() const {
        return mResources.Type;
    }

    bool IsStaging() const {
        return mStaging;
    }

    float GetOffscreenTime() const {
        return mOffScreenTime;
    }
    float GetOnScreenTime() const {
        return mOnScreenTime;
    }

    bool InShock() const {
        return mDamage ? mDamage->InShock() > 0.0f : false;
    }
    bool IsDestroyed() const {
        return mDamage ? mDamage->IsDestroyed() : false;
    }
    float GetAbsoluteSpeed() const {
        return mAbsSpeed;
    }
    float GetSpeedometer() const {
        return mSpeedometer;
    }
    float GetSpeed() const {
        return mSpeed;
    }

    bool IsGlareOn(VehicleFX::ID glare) {
        return (mGlareState & glare) != 0;
    }

    bool IsCollidingWithSoftBarrier() {
        return false;
    }

    bool IsAnimating() const {
        return mAnimating;
    }

    bool IsActive() const {
        return mPhysicsMode != PHYSICS_MODE_INACTIVE;
    }

    PhysicsMode GetPhysicsMode() const {
        return mPhysicsMode;
    }
    const char *GetCacheName() const {
        return mCacheName;
    }

    IVehicleAI *GetAIVehiclePtr() const {
        return mAI;
    }
    float GetSlipAngle() const {
        return mSlipAngle;
    }
    UMath::Vector3 &GetLocalVelocity() const {
        return const_cast<UMath::Vector3 &>(mLocalVel);
    }
    const FECustomizationRecord *GetCustomizations() const {
        return mCustomization;
    }

    bool GetPerformance(Physics::Info::Performance &performance) const {
        performance = mPerformance;
        return mPerformanceValid;
    }

    EventSequencer::IEngine *GetEventSequencer() {
        return mSequencer;
    }

    IModel *GetModel() {
        return mRenderable ? mRenderable->GetModel() : NULL;
    }
    const IModel *GetModel() const {
        return mRenderable ? mRenderable->GetModel() : NULL;
    }

    void UpdateListing();

    void Launch();
    float GetPerfectLaunch() const;
    void SetStaging(bool staging);
    void SetDriverStyle(DriverStyle style);
    void SetDriverClass(DriverClass cclass);
    bool OnTask(HSIMTASK htask, float dT);

    static void CleanResources();
    static unsigned int CountResources();
    static bool MakeRoom(IVehicleCache *whosasking, const ResourceList &resources);
    static ISimable *Construct(Sim::Param params);

    PVehicle(DriverClass carClass, const Attrib::Gen::pvehicle &attributes, const UMath::Vector3 &initialVec, const UMath::Vector3 &initialPos,
             const CollisionGeometry::Bounds *bounds, const FECustomizationRecord *customization, const Resource &resource,
             const Physics::Info::Performance *performance, const char *cache_name);

    void Kill();
    void OnBehaviorChange(const UCrc32 &mechanic);
    UCrc32 LookupBehaviorSignature(const Attrib::StringKey &mechanic) const;
    void LoadBehaviors(const UMath::Vector3 &initialPos, const UMath::Matrix4 &initMat);
    virtual ~PVehicle();
    void SetBehaviorOverride(UCrc32 mechanic, UCrc32 behavior);
    void RemoveBehaviorOverride(UCrc32 mechanic);
    void Reset();
    bool SetVehicleOnGround(const UMath::Vector3 &resetPos, const UMath::Vector3 &initialVec);
    void OnAttributeChange(const Attrib::Collection *collection, Attrib::Key attribkey);
    void OnEnableModeling();
    void OnDisableModeling();
    void SetSpeed(float speed);
    void CommitBehaviorOverrides();
    bool IsLoading() const;
    void DoDebug(float dT);
    void UpdateLocalVelocities();
    void CheckOffWorld();
    void DoStaging(float dT);
    void OnTaskSimulate(float dT);
    bool OnExplosion(const UMath::Vector3 &normal, const UMath::Vector3 &position, float dT, IExplosion *explosion);
    void OnTaskFX(float dT);
    bool SetDynamicData(const EventSequencer::System *system, EventDynamicData *data);
    void GlareOn(VehicleFX::ID glare);
    void GlareOff(VehicleFX::ID glare);
    void DebugObject();
    void ReloadBehaviors();
    void SetAnimating(bool animate);
    void Activate();
    void Deactivate();
    void SetPhysicsMode(PhysicsMode mode);
    void OnBeginMode(const PhysicsMode mode);
    void OnEndMode(const PhysicsMode mode);
    const Physics::Tunings *GetTunings() const;
    void SetTunings(const Physics::Tunings &tunings);
    virtual void OnDebugDraw();
    void ComputeHeading(UMath::Vector3 *v);
    void ForceStopOn(char forceStopBits);
    void ForceStopOff(char forceStopBits);

    bool IsSpooled() const {
        return mResources.IsSpooled();
    }

    static const bool mRunDyno;
    static bTList<PVehicle> mInstances;

    Attrib::Gen::pvehicle mAttributes;    // offset 0xD8, size 0x14
    FECustomizationRecord *mCustomization; // offset 0xEC, size 0x4
    IInput *mInput;                        // offset 0xF0, size 0x4
    ICollisionBody *mCollisionBody;        // offset 0xF4, size 0x4
    ISuspension *mSuspension;              // offset 0xF8, size 0x4
    IEngine *mEngine;                      // offset 0xFC, size 0x4
    IDamageable *mDamage;                  // offset 0x100, size 0x4
    ITransmission *mTranny;                // offset 0x104, size 0x4
    IVehicleAI *mAI;                       // offset 0x108, size 0x4
    IArticulatedVehicle *mArticulation;    // offset 0x10C, size 0x4
    IRenderable *mRenderable;              // offset 0x110, size 0x4
    IAudible *mAudible;                    // offset 0x114, size 0x4
    EventSequencer::IEngine *mSequencer;   // offset 0x118, size 0x4
    HSIMTASK mTaskFX;                      // offset 0x11C, size 0x4
    UCrc32 mClass;                         // offset 0x120, size 0x4
    float mSpeed;                          // offset 0x124, size 0x4
    float mAbsSpeed;                       // offset 0x128, size 0x4
    float mSpeedometer;                    // offset 0x12C, size 0x4
    float mTimeInAir;                      // offset 0x130, size 0x4
    float mSlipAngle;                      // offset 0x134, size 0x4
    unsigned int mWheelsOnGround;          // offset 0x138, size 0x4
    UMath::Vector3 mLocalVel;              // offset 0x13C, size 0xC
    DriverClass mDriverClass;              // offset 0x148, size 0x4
    DriverStyle mDriverStyle;              // offset 0x14C, size 0x4
    unsigned int mGlareState;              // offset 0x150, size 0x4
    float mStartingNOS;                    // offset 0x154, size 0x4
    float mBrakeTime;                      // offset 0x158, size 0x4
    char mForceStop;                       // offset 0x15C, size 0x1
    PhysicsMode mPhysicsMode;              // offset 0x160, size 0x4
    bool mAnimating;                       // offset 0x164, size 0x1
    bool mStaging;                         // offset 0x168, size 0x1
    LaunchState mPerfectLaunch;            // offset 0x16C, size 0x8
    ChangeRequest mBehaviorOverrides;      // offset 0x174, size 0x10
    bool mOverrideDirty;                   // offset 0x184, size 0x1
    const CollisionGeometry::Bounds *mBounds; // offset 0x188, size 0x4
    bool mIsModeling;                      // offset 0x18C, size 0x1
    float mOffScreenTime;                  // offset 0x190, size 0x4
    float mOnScreenTime;                   // offset 0x194, size 0x4
    bool mOffWorld;                        // offset 0x198, size 0x1
    bool mHasDyno;                         // offset 0x19C, size 0x1
    Resource mResources;                   // offset 0x1A0, size 0xC
    Physics::Info::Performance mPerformance; // offset 0x1AC, size 0xC
    bool mPerformanceValid;                // offset 0x1B8, size 0x1
    const char *mCacheName;                // offset 0x1BC, size 0x4
};

void PVehicle::UpdateListing() {

    for (unsigned int i = 1; i < VEHICLE_MAX; i++) {
        UnList((eVehicleList)i);
    }

    switch (mDriverClass) {
    case DRIVER_HUMAN:
        AddToList(VEHICLE_PLAYERS);
        AddToList(VEHICLE_RACERS);

        break;

    case DRIVER_REMOTE:
        AddToList(VEHICLE_PLAYERS);
        AddToList(VEHICLE_RACERS);
        AddToList(VEHICLE_REMOTE);

        break;

    case DRIVER_COP:
        AddToList(VEHICLE_AI);
        AddToList(VEHICLE_AICOPS);

        break;

    case DRIVER_RACER:
        AddToList(VEHICLE_AI);
        AddToList(VEHICLE_AIRACERS);
        AddToList(VEHICLE_RACERS);

        break;

    case DRIVER_TRAFFIC:
        AddToList(VEHICLE_AI);
        AddToList(VEHICLE_AITRAFFIC);

        break;

    default:
        break;
    }

    if (!IsActive()) {
        AddToList(VEHICLE_INACTIVE);
    }

    if (mClass == VehicleClass::TRAILER) {
        AddToList(VEHICLE_TRAILERS);
    }
}

void PVehicle::Launch() {
    if (mEngine) {
        if (!mPerfectLaunch.IsSet()) {
            if (mDriverClass == DRIVER_HUMAN) {
                if (mPerfectLaunch.Amount > 0.0f) {
                    mPerfectLaunch.Set(4.5f);

                    new EPerfectLaunch(GetInstanceHandle(), mPerfectLaunch.Amount);
                }
            } else {
                mPerfectLaunch.Clear();
            }
        }
    }
}

float PVehicle::GetPerfectLaunch() const {
    if (!IsStaging() && mPerfectLaunch.IsSet()) {
        return mPerfectLaunch.Amount;
    }
    return 0.0f;
}

void PVehicle::SetStaging(bool staging) {
    if (staging != mStaging) {
        mStaging = staging;

        if (staging) {
            SetSpeed(0.0f);
        }
    }
}

void PVehicle::SetDriverStyle(DriverStyle style) {
    if (mDriverStyle != style) {
        mDriverStyle = style;

        ReloadBehaviors();

        if (mDriverStyle == STYLE_RACING && mEngine) {
            mEngine->MatchSpeed(mLocalVel.z);
        }
    }
}

void PVehicle::SetDriverClass(DriverClass cclass) {
    if (mDriverClass != cclass) {
        mDriverClass = cclass;
        UpdateListing();

        ReloadBehaviors();
    }
}

bool PVehicle::OnTask(HSIMTASK htask, float dT) {
    if (mTaskFX == htask) {
        OnTaskFX(dT);
        return true;
    }

    return PhysicsObject::OnTask(htask, dT);
}

PVehicle::Resource::Resource(const Attrib::Gen::pvehicle &pvehicle, bool spool, bool is_player) : Flags(0) {
    Type = CarPartDB.GetCarType(bStringHash(pvehicle.MODEL().GetString()));

    if (Type == CARTYPE_NONE || Type >= NUM_CARTYPES) {
        return;
    }

    if (CarInfo_IsSkinned(Type)) {
        Flags |= NEEDS_COMPOSITING;
    }

    bool split_screen = Sim::IsSplitScreen();
    Cost = CarInfo_GetResourceCost(Type, is_player, split_screen);

    if (spool) {
        Flags |= SPOOL;
    }

    if (Cost != 0) {
        Flags |= VALID;
    }
}

void PVehicle::CleanResources() {
    for (PVehicle *pv = mInstances.GetHead(); pv != mInstances.EndOfList();) {
        PVehicle *next = pv->GetNext();

        if (pv && pv->IsDirty()) {
            delete pv;
        }

        pv = next;
    }
}

bool CanInstancesShareResourceCost(CarType type) {
    CarUsageType usage = GetCarTypeInfo(type)->GetCarUsageType();

    if (usage == CAR_USAGE_TYPE_COP) {
        return true;
    }

    return usage == CAR_USAGE_TYPE_TRAFFIC;
}

unsigned int PVehicle::CountResources() {
    unsigned int total = 0;

    ResourceList counted;

    for (PVehicle *pv = mInstances.GetHead(); pv != mInstances.EndOfList(); pv = pv->GetNext()) {
        bool found = false;

        for (ResourceList::const_iterator i = counted.begin(); i != counted.end(); ++i) {
            if (i->Type == pv->mResources.Type) {
                found = true;
                break;
            }
        }

        unsigned int cost = 0;

        if (!found || !CanInstancesShareResourceCost(pv->mResources.Type)) {
            counted.push_back(pv->mResources);
            cost = pv->mResources.Cost;
        }

        total += cost;
    }

    return total;
}

bool PVehicle::MakeRoom(IVehicleCache *whosasking, const ResourceList &resources) {

    CleanResources();

    unsigned int numrequested = resources.size();

    unsigned int totalcost = 0;
    bool needscompositing = false;
    for (ResourceList::const_iterator ir = resources.begin(); ir != resources.end(); ++ir) {

        const Resource &res = *ir;

        if (res.NeedsCompositing()) {
            needscompositing = true;
        }

        unsigned int cost = res.Cost;

        for (PVehicle *pv = mInstances.GetHead(); pv != mInstances.EndOfList(); pv = pv->GetNext()) {

            if (pv->mResources.Type == res.Type) {

                if (CanInstancesShareResourceCost(pv->mResources.Type)) {
                    cost = 0;
                }
                break;
            }
        }

        totalcost += cost;
    }

    unsigned int pool = CarInfo_GetResourcePool(needscompositing);

    unsigned int used = CountResources();
    unsigned int listed = IVehicle::Count(VEHICLE_ALL);
    unsigned int overcost = 0;
    unsigned int overcount = 0;

    if (used + totalcost > pool) {
        overcost = used + totalcost - pool;
    }

    if (listed + numrequested > 10) {
        overcount = listed + numrequested - 10;
    }

    if (overcost == 0 && overcount == 0) {

        return true;
    }

    if (!whosasking) {
        return false;
    }

    ManagementList nodes;

    for (PVehicle *pv = mInstances.GetHead(); pv != mInstances.EndOfList(); pv = pv->GetNext()) {

        ManageNode node;
        node.vehicle = pv;
        node.resource = pv->mResources;
        node.result = VCR_DONTCARE;
        nodes.push_back(node);
    }

    for (ManagementList::iterator iq = nodes.begin(); iq != nodes.end(); ++iq) {

        if (iq->result != VCR_WANT) {

            iq->result = whosasking->OnQueryVehicleCache(iq->vehicle, whosasking);
        }
    }

    for (ManagementList::iterator ic = nodes.begin(); ic != nodes.end(); ++ic) {

        if (ic->result != VCR_WANT) {

            for (IVehicleCache::List::const_iterator ca = IVehicleCache::GetList().begin(); ca != IVehicleCache::GetList().end(); ++ca) {

                IVehicleCache *cache = *ca;

                if (!UTL::COM::ComparePtr(cache, whosasking)) {

                    eVehicleCacheResult result = cache->OnQueryVehicleCache(ic->vehicle, whosasking);
                    if (result == VCR_WANT) {

                        ic->result = result;
                        break;
                    }
                }
            }
        }
    }

    std::sort(nodes.begin(), nodes.end(), ManageNode::sort_by_keep);

    for (ResourceList::const_iterator iw = resources.begin(); iw != resources.end(); ++iw) {

        const Resource &want = *iw;
        for (ManagementList::iterator jw = nodes.begin(); jw != nodes.end(); ++jw) {

            if (want.Type == jw->resource.Type) {

                jw->result = VCR_WANT;
                break;
            }
        }

    }

    if (overcost != 0) {

        CarType lasttype = (CarType)-1;
        eVehicleCacheResult keep = VCR_DONTCARE;
        for (ManagementList::iterator ik = nodes.begin(); ik != nodes.end(); ++ik) {

            if (ik->resource.Type != lasttype) {

                keep = ik->result;
                lasttype = ik->resource.Type;
            }

            if (keep == VCR_WANT) {

                ik->result = keep;
            }
        }
    }

    nodes.erase(std::remove_if(nodes.begin(), nodes.end(), ManageNode::is_kept), nodes.end());

    if (nodes.size() == 0) {

        return false;
    }

    UTL::Std::map<CarType, unsigned int, _type_map> instancecounts;
    for (ManagementList::iterator im = nodes.begin(); im != nodes.end(); ++im) {

        instancecounts[im->resource.Type]++;
    }
    for (ManagementList::iterator in = nodes.begin(); in != nodes.end(); ++in) {

        in->instancecount = instancecounts[in->resource.Type];
    }

    unsigned int removedcount = 0;
    ManagementList::iterator i = nodes.begin();
    if (overcost != 0) {

        std::sort(nodes.begin(), nodes.end(), ManageNode::sort_remove_resources);

        unsigned int removedcost = 0;
        CarType lasttype = (CarType)-1;
        unsigned int cost = 0;
        while (i != nodes.end()) {

            if (i->resource.Type != lasttype) {

                removedcost += cost;
                lasttype = i->resource.Type;
            }

            if (removedcost >= overcost) {

                cost = 0;

                break;
            }

            cost = i->resource.Cost;

            removedcount++;
            ++i;
        }

        removedcost += cost;

        if (removedcost < overcost) {

            return false;
        }
    }

    if (removedcount < overcount) {

        std::sort(i, nodes.end(), ManageNode::sort_remove_instances);

        while (i != nodes.end() && removedcount < overcount) {

            removedcount++;
            ++i;
        }

        if (removedcount < overcount) {

            return false;
        }
    }

    nodes.erase(i, nodes.end());

    for (ManagementList::iterator id = nodes.begin(); id != nodes.end(); ++id) {

        PVehicle *pv = id->vehicle;

        for (IVehicleCache::List::const_iterator cd = IVehicleCache::GetList().begin(); cd != IVehicleCache::GetList().end(); ++cd) {

            (*cd)->OnRemovedVehicleCache(pv);
        }

        delete pv;

    }

    return true;
}

const UCrc32 VehicleClass::CAR = UCrc32("CAR");

const UCrc32 VehicleClass::SUBMARINE = UCrc32("SUBMARINE");

const UCrc32 VehicleClass::CHOPPER = UCrc32("CHOPPER");

const UCrc32 VehicleClass::BIKE = UCrc32("BIKE");

const UCrc32 VehicleClass::BOAT = UCrc32("BOAT");

const UCrc32 VehicleClass::SNOWMOBILE = UCrc32("SNOWMOBILE");

const UCrc32 VehicleClass::HOVER = UCrc32("HOVER");

const UCrc32 VehicleClass::PLANE = UCrc32("PLANE");

const UCrc32 VehicleClass::TANK = UCrc32("TANK");

const UCrc32 VehicleClass::TRAILER = UCrc32("TRAILER");

const UCrc32 VehicleClass::TRAIN = UCrc32("TRAIN");

const UCrc32 VehicleClass::TRANSPORT = UCrc32("TRANSPORT");

const UCrc32 VehicleClass::RC = UCrc32("RC");

const UCrc32 VehicleClass::TRACTOR = UCrc32("TRACTOR");

ISimable *PVehicle::Construct(Sim::Param params) {

    const VehicleParams vp = params.Fetch<VehicleParams>(UCrc32(0xa6b47fac));

    Attrib::Gen::pvehicle attributes(vp.carType, 0, NULL);

    if (!attributes.IsValid()) {

        return NULL;
    }

    const char *vehicle_name = attributes.CollectionName();

    const FECustomizationRecord *customizations = vp.customization;

    if (customizations == NULL) {
        if (attributes.DefaultPresetRide()) {

            PresetCar *preset = FindFEPresetCar(bStringHashUpper(attributes.DefaultPresetRide()));
            if (preset) {
                static FECustomizationRecord temp_record;

                temp_record.Default();
                temp_record.BecomePreset(preset);
                customizations = &temp_record;
            }
        }
    }

    if (customizations) {

        if (!customizations->WriteRecordIntoPhysics(attributes)) {

            return NULL;
        }
    }

    if (vp.matched) {

        if (!Physics::Upgrades::MatchPerformance(attributes, *vp.matched)) {

            return NULL;
        }
    }

    if (vp.Flags & 4) {

        Physics::Upgrades::RemoveJunkman(attributes, Physics::Upgrades::PUT_NOS);
        Physics::Upgrades::RemovePart(attributes, Physics::Upgrades::PUT_NOS);
    }

    if (GetMikeMannBuild()) {

        int new_nos = Physics::Upgrades::GetMaxLevel(attributes, Physics::Upgrades::PUT_NOS);
        Physics::Upgrades::SetLevel(attributes, Physics::Upgrades::PUT_NOS, new_nos);
    }

    if (vp.Flags & 0x10) {

        if (Physics::Upgrades::GetLevel(attributes, Physics::Upgrades::PUT_NOS) == 0) {

            if (Physics::Upgrades::GetMaxLevel(attributes, Physics::Upgrades::PUT_NOS) > 0) {

                Physics::Upgrades::SetLevel(attributes, Physics::Upgrades::PUT_NOS, 1);
            }
        }
    }

    const CollisionGeometry::Collection *geoms = CollisionGeometry::Lookup(UCrc32(attributes.MODEL()));

    if (geoms == NULL || geoms->GetRoot() == NULL) {

        return NULL;
    }

    bool spooling_resources = (vp.Flags & 1) != 0;
    bool is_player = vp.carClass == DRIVER_HUMAN;
    Resource resource(attributes, spooling_resources, is_player);

    if (!resource.IsValid()) {

        return NULL;
    }

    ResourceList resources;
    resources.push_back(resource);

    Attrib::RefSpec trailer_ref = attributes.Trailer();

    if (trailer_ref.GetCollectionKey()) {

        bool is_player = false;

        resources.push_back(Resource(Attrib::Gen::pvehicle(trailer_ref, 0, NULL), spooling_resources, is_player));
    }

    if (!MakeRoom(vp.VehicleCache, resources)) {

        return NULL;
    }

    resources.clear();

    Physics::Info::Performance perf;

    const Physics::Info::Performance *performance = NULL;

    if (vp.matched) {

        performance = vp.matched;

    } else if (vp.Flags & 8) {

        if (Physics::Info::ComputePerformance(attributes, perf)) {
            performance = &perf;
        }
    }

    if (Sim::CanSpawnRigidBody(vp.initialPos, true)) {

#ifndef EA_BUILD_A124
        const char *cache_name = vp.VehicleCache ? vp.VehicleCache->GetCacheName() : NULL;
#else
        // La alpha 124 no tiene IVehicleCache::GetCacheName (IVehicleCache.h) ni
        // pasa nombre de cache al constructor (DWARF de PS2).
        const char *cache_name = NULL;
#endif

        PVehicle *vehicle = new PVehicle(vp.carClass, attributes, vp.initialVec, vp.initialPos, geoms->GetRoot(), customizations, resource,
                                         performance, cache_name);
        if (vp.Flags & 2) {

            vehicle->SetVehicleOnGround(vp.initialPos, vp.initialVec);
        }

        return vehicle;
    }

    return NULL;
}

PVehicle::PVehicle(DriverClass carClass, const Attrib::Gen::pvehicle &attributes, const UMath::Vector3 &initialVec, const UMath::Vector3 &initialPos,
                   const CollisionGeometry::Bounds *bounds, const FECustomizationRecord *customization, const Resource &resource,
                   const Physics::Info::Performance *performance, const char *cache_name)
    : PhysicsObject(attributes.GetBase(), SIMABLE_VEHICLE, 0, 24), //
      IVehicle(this),                                              //
      EventSequencer::IContext(this),                              //
      IExplodeable(this),                                          //
      mAttributes(attributes),                                     //
      mCustomization(NULL),                                        //
      mInput(NULL),                                                //
      mCollisionBody(NULL),                                        //
      mSuspension(NULL),                                           //
      mEngine(NULL),                                               //
      mDamage(NULL),                                               //
      mTranny(NULL),                                               //
      mAI(NULL),                                                   //
      mArticulation(NULL),                                         //
      mRenderable(NULL),                                           //
      mAudible(NULL),                                              //
      mSequencer(NULL),                                            //
      mTaskFX(NULL),                                               //
      mSpeed(0.0f),                                                //
      mAbsSpeed(0.0f),                                             //
      mSpeedometer(0.0f),                                          //
      mTimeInAir(0.0f),                                            //
      mSlipAngle(0.0f),                                            //
      mWheelsOnGround(0),                                          //
      mLocalVel(UMath::Vector3::kZero),                            //
      mDriverClass(carClass),                                      //
      mDriverStyle(STYLE_RACING),                                  //
      mGlareState(0),                                              //
      mStartingNOS(1.0f),                                          //
      mBrakeTime(0.0f),                                            //
      mForceStop(0),                                               //
      mPhysicsMode(PHYSICS_MODE_SIMULATED),                        //
      mAnimating(false),                                           //
      mStaging(false),                                             //
      mOverrideDirty(false),                                       //
      mBounds(bounds),                                             //
      mIsModeling(true),                                           //
      mOffScreenTime(0.0f),                                        //
      mOnScreenTime(0.0f),                                         //
      mOffWorld(false),                                            //
      mHasDyno(false),                                             //
      mResources(resource),                                        //
      mPerformanceValid(false),                                    //
      mCacheName(cache_name) {

    if (performance) {
        mPerformance = *performance;

        mPerformanceValid = true;
    }

    mInstances.AddTail(this);
    IAttributeable::Register(this, Attrib::Gen::pvehicle::ClassKey());

    AITarget::Register(this);

    if (customization) {

        mCustomization = new FECustomizationRecord(*customization);
    }

    MakeDebugable(DBG_RIGIDBODY);

    mClass = UCrc32(mAttributes.CLASS());

    AddToList(VEHICLE_ALL);

    UpdateListing();

    switch (mDriverClass) {

    case DRIVER_HUMAN:
        mTaskFX = AddTask(UCrc32("FX"), 1.0f, 0.0f, (Sim::TaskMode) 0);
        break;

    case DRIVER_TRAFFIC:
        mTaskFX = AddTask(UCrc32("FX"), 0.25f, 0.0f, (Sim::TaskMode) 0);
        break;

    default:
        mTaskFX = AddTask(UCrc32("FX"), 0.5f, 0.0f, (Sim::TaskMode) 0);
        break;
    }

    Reset();

    if (mDamage) {
        mDamage->ResetDamage();
    }

    mGlareState = 0;

    UMath::Matrix4 initMat;

    initMat = Util_GenerateMatrix(initialVec, NULL);

    LoadBehaviors(initialPos, initMat);

    SetOwnerObject(this);

    const Attrib::StringKey seq = mAttributes.EventSequencer();

    if (seq.IsNotEmpty()) {

        mSequencer = EventSequencer::Create(this, this, UCrc32(seq.GetString()), Sim::GetTime(), 1.0f);
    }

    OnBeginMode(PHYSICS_MODE_SIMULATED);
}

BIND_PHYSICS_FACTORY(PVehicle);

bTList<PVehicle> PVehicle::mInstances;

void PVehicle::Kill() {
    PhysicsObject::Kill();

    ReleaseBehavior(BEHAVIOR_MECHANIC_DRAW);
    ReleaseBehavior(BEHAVIOR_MECHANIC_AUDIO);
    mResources.Invalidate();
}

void PVehicle::OnBehaviorChange(const UCrc32 &mechanic) {
    PhysicsObject::OnBehaviorChange(mechanic);

    if (mechanic == BEHAVIOR_MECHANIC_AI) {
        ISimable::QueryInterface(&mAI);
    } else if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        ISimable::QueryInterface(&mInput);
    } else if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        if (ISimable::QueryInterface(&mCollisionBody)) {
            mCollisionBody->SetAnimating(mAnimating);
        }

        ISimable::QueryInterface(&mArticulation);
    } else if (mechanic == BEHAVIOR_MECHANIC_DRAW) {
        ISimable::QueryInterface(&mRenderable);
    } else if (mechanic == BEHAVIOR_MECHANIC_AUDIO) {
        ISimable::QueryInterface(&mAudible);
    } else if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        if (ISimable::QueryInterface(&mSuspension)) {
            if (mCollisionBody) {
                float speed = UMath::Dot(mCollisionBody->GetForwardVector(), GetRigidBody()->GetLinearVelocity());
                mSuspension->MatchSpeed(speed);
            }
        }
    } else if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        ISimable::QueryInterface(&mTranny);

        if (ISimable::QueryInterface(&mEngine)) {
            mEngine->ChargeNOS(mStartingNOS - mEngine->GetNOSCapacity());

            if (mCollisionBody) {
                float speed = UMath::Dot(mCollisionBody->GetForwardVector(), GetRigidBody()->GetLinearVelocity());
                mEngine->MatchSpeed(speed);
            }
        }
    } else if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        ISimable::QueryInterface(&mDamage);
    }
}

struct AIBehavior {
    DriverClass driver;      // offset 0x0, size 0x4
    UCrc32 vehicleclass;     // offset 0x4, size 0x4
    UCrc32 behavior;         // offset 0x8, size 0x4
};

static AIBehavior ai_behaviors[] = {
    {DRIVER_NONE, UCrc32::kNull, UCrc32("AIVehicleEmpty")},
    {DRIVER_NIS, UCrc32::kNull, UCrc32("AIVehicleEmpty")},
    {DRIVER_RACER, UCrc32::kNull, UCrc32("AIVehicleRacecar")},
    {DRIVER_TRAFFIC, UCrc32::kNull, UCrc32("AIVehicleTraffic")},
    {DRIVER_COP, UCrc32("CHOPPER"), UCrc32("AIVehicleHelicopter")},
    {DRIVER_COP, UCrc32::kNull, UCrc32("AIVehicleCopCar")},
    {DRIVER_HUMAN, UCrc32::kNull, UCrc32("AIVehicleHuman")},
    {DRIVER_REMOTE, UCrc32::kNull, UCrc32("AIVehicleHuman")},
    {DRIVER_NONE, UCrc32::kNull, UCrc32::kNull},
};

UCrc32 PVehicle::LookupBehaviorSignature(const Attrib::StringKey &mechanic) const {
    if (mechanic == BEHAVIOR_MECHANIC_AUDIO && !IsSoundEnabled) {
        return UCrc32::kNull;
    }

    ChangeRequest::const_iterator i = mBehaviorOverrides.find(mechanic);

    if (i != mBehaviorOverrides.end()) {
        return i->second;
    }

    if (mAnimating) {
        if (mClass != VehicleClass::CHOPPER) {
            if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
                return UCrc32("SuspensionSpline");
            }

            if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
                return UCrc32("EngineSpline");
            }
        }

        if (mechanic == BEHAVIOR_MECHANIC_RESET) {
            return UCrc32::kNull;
        }

        if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
            return UCrc32("InputNIS");
        }
    }

    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        if (mDriverClass == DRIVER_REMOTE) {
            return UCrc32("RBRemote");
        }
    }

    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        if (mDriverClass == DRIVER_HUMAN) {
            if (mDriverStyle == STYLE_DRAG) {
                return UCrc32("InputPlayerDrag");
            }

            return UCrc32("InputPlayer");
        }
    }

    if (mechanic == BEHAVIOR_MECHANIC_DAMAGE && mDriverStyle == STYLE_DRAG) {
        return UCrc32("DamageDragster");
    }

    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        if (mClass == VehicleClass::CAR && mDriverStyle == STYLE_DRAG) {
            return UCrc32("EngineDragster");
        }
    }

    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        if (mClass == VehicleClass::CAR) {
            switch (mDriverClass) {
            case DRIVER_RACER:
            case DRIVER_NONE:
            case DRIVER_REMOTE:
                return UCrc32("SuspensionSimple");
            }
        }
    }

    if (mechanic == BEHAVIOR_MECHANIC_AI) {
        const AIBehavior *ab = ai_behaviors;

        UCrc32 signature = ab->behavior;
        while (ab->behavior != UCrc32::kNull) {

            if (mDriverClass == ab->driver) {

                if (mClass == ab->vehicleclass || ab->vehicleclass == UCrc32::kNull) {
                    signature = ab->behavior;
                    break;
                }
            }

            ++ab;
        }

        return signature;
    }

    if (mechanic == BEHAVIOR_MECHANIC_EFFECTS) {
        if (mDriverClass == DRIVER_HUMAN || IsPlayer()) {
            return UCrc32("EffectsPlayer");
        }
    }

    Attrib::Instance behaviors(NULL, 0, NULL);
    Attrib::StringKey behavior;
    Attrib::Attribute attrib = mAttributes.Get(mechanic);

    if (!attrib.Get(0, behavior)) {
        return UCrc32::kNull;
    }

    return behavior;
}

void PVehicle::LoadBehaviors(const UMath::Vector3 &initialPos, const UMath::Matrix4 &initMat) {
    if (IsDirty()) {
        return;
    }

    if (mEngine) {
        mStartingNOS = mEngine->GetNOSCapacity();
    }

    UMath::Vector3 zero = {0.0f, 0.0f, 0.0f};
    UMath::Vector3 dimension;
    mBounds->GetHalfDimensions(dimension);

    unsigned int collisionmask = 0;

    switch (mDriverClass) {
    case DRIVER_HUMAN:
    case DRIVER_RACER:
    case DRIVER_REMOTE:
        break;
    case DRIVER_COP:
        collisionmask |= 0x80;
        break;
    default:
        collisionmask |= 0x40;
        break;
    }

    float mass = mAttributes.MASS();
    UMath::Vector3 tensor = Util_GenerateCarTensor(mass, dimension.x, dimension.y, dimension.z, UMath::Vector4To3(mAttributes.TENSOR_SCALE()));

    UCrc32 sig_rigidbody = LookupBehaviorSignature(BEHAVIOR_MECHANIC_RIGIDBODY);
    LoadBehavior(BEHAVIOR_MECHANIC_RIGIDBODY, sig_rigidbody, RBComplexParams(initialPos, zero, UMath::Vector3::kZero, initMat, mass, tensor, dimension, mBounds, true, collisionmask));

    UCrc32 sig_input = LookupBehaviorSignature(BEHAVIOR_MECHANIC_INPUT);
    LoadBehavior(BEHAVIOR_MECHANIC_INPUT, sig_input, Sim::Param());

    UCrc32 sig_engine = LookupBehaviorSignature(BEHAVIOR_MECHANIC_ENGINE);
    LoadBehavior(BEHAVIOR_MECHANIC_ENGINE, sig_engine, EngineParams());

    UCrc32 sig_suspension = LookupBehaviorSignature(BEHAVIOR_MECHANIC_SUSPENSION);
    LoadBehavior(BEHAVIOR_MECHANIC_SUSPENSION, sig_suspension, SuspensionParams());

    UCrc32 sig_damage = LookupBehaviorSignature(BEHAVIOR_MECHANIC_DAMAGE);
    LoadBehavior(BEHAVIOR_MECHANIC_DAMAGE, sig_damage, DamageParams());

    UCrc32 sig_draw = LookupBehaviorSignature(BEHAVIOR_MECHANIC_DRAW);
    LoadBehavior(BEHAVIOR_MECHANIC_DRAW, sig_draw, Sim::Param());

    UCrc32 sig_audio = LookupBehaviorSignature(BEHAVIOR_MECHANIC_AUDIO);
    LoadBehavior(BEHAVIOR_MECHANIC_AUDIO, sig_audio, Sim::Param());

    UCrc32 sig_ai = LookupBehaviorSignature(BEHAVIOR_MECHANIC_AI);
    LoadBehavior(BEHAVIOR_MECHANIC_AI, sig_ai, AIParams());

    UCrc32 sig_effects = LookupBehaviorSignature(BEHAVIOR_MECHANIC_EFFECTS);
    LoadBehavior(BEHAVIOR_MECHANIC_EFFECTS, sig_effects, Sim::Param());

    UCrc32 sig_reset = LookupBehaviorSignature(BEHAVIOR_MECHANIC_RESET);
    LoadBehavior(BEHAVIOR_MECHANIC_RESET, sig_reset, Sim::Param());

    ResetBehavior(BEHAVIOR_MECHANIC_RIGIDBODY);
    ResetBehavior(BEHAVIOR_MECHANIC_INPUT);
    ResetBehavior(BEHAVIOR_MECHANIC_SUSPENSION);
    ResetBehavior(BEHAVIOR_MECHANIC_ENGINE);
    ResetBehavior(BEHAVIOR_MECHANIC_DAMAGE);
    ResetBehavior(BEHAVIOR_MECHANIC_EFFECTS);
}

PVehicle::~PVehicle() {
    DetachAll();

    AITarget::UnRegister(this);

    if (mCustomization) {
        delete mCustomization;
        mCustomization = NULL;
    }

    ReleaseBehaviors();

    if (mTaskFX) {
        RemoveTask(mTaskFX);
        mTaskFX = NULL;
    }

    if (mSequencer) {
        mSequencer->Release();
        mSequencer = NULL;
    }

    Remove();

    CameraAI::RemoveAvoidable(this);
}

void PVehicle::SetBehaviorOverride(UCrc32 mechanic, UCrc32 behavior) {
    Behavior *beh = FindBehavior(mechanic);
    if (!beh || beh->GetSignature() != behavior) {
        mBehaviorOverrides[mechanic] = behavior;
        mOverrideDirty = true;
    }
}

void PVehicle::RemoveBehaviorOverride(UCrc32 mechanic) {
    ChangeRequest::iterator i = mBehaviorOverrides.find(mechanic);
    if (i != mBehaviorOverrides.end()) {
        mBehaviorOverrides.erase(i);
        mOverrideDirty = true;
    }
}

void PVehicle::Reset() {
    PhysicsObject::Reset();

    mTimeInAir = 0.0f;
    mWheelsOnGround = 0;
    mBrakeTime = 0.0f;
}

bool PVehicle::SetVehicleOnGround(const UMath::Vector3 &resetPos, const UMath::Vector3 &initialVec) {
    IRigidBody *rb = GetRigidBody();

    if (!rb || !mCollisionBody) {
        return false;
    }

    MJumpCut jumpcut(GetWorldID());

    jumpcut.Send("CameraMessagePort");

    UMath::Vector3 dims = rb->GetDimension();
    UMath::Vector3 position = resetPos;
    position.y += dims.y;

    UMath::Matrix4 mat = Util_GenerateMatrix(initialVec, NULL);

    rb->SetLinearVelocity(UMath::Vector3::kZero);
    rb->SetAngularVelocity(UMath::Vector3::kZero);

    float height;

    bool success = true;

    float downforce = mCollisionBody->GetGravity() * rb->GetMass();

    if (!WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(position, height, NULL)) {
        position = resetPos;
        success = false;
    } else if (!mSuspension) {
        position = resetPos;
        position.y = height + dims.y;
    } else {
        WWorldPos wpos;
        wpos.SetTolerance(dims.y);

        position.y = height;
        UMath::Vector4 corners[4];
        UMath::Vector4 center = UMath::Vector4::kZero;
        UMath::Vector4 vpos = UMath::Vector3To4(position, 1.0f);

        for (unsigned int i = 0; i < 4; ++i) {
            UMath::Vector4 &corner = corners[i];

            corner = UMath::Vector3To4(mSuspension->GetWheelLocalPos(i), 0.0f);
            corner.y = 0.0f;
            UMath::ScaleAdd(corner, 0.25f, center, center);
            UMath::Rotate(corner, mat, corner);
            UMath::Add(corner, vpos, corner);
            wpos.FindClosestFace(UMath::Vector4To3(corner), true);

            if (wpos.OnValidFace()) {
                float compression = mSuspension->GuessCompression(i, downforce);
                float delta = mSuspension->GetRideHeight(i) - compression;

                corner.y = dims.y + wpos.HeightAtPoint(UMath::Vector4To3(corner)) + delta;
            } else {
                success = false;
            }
        }

        if (!success) {
            position = resetPos;
        } else {
            UMath::Vector4 front;
            UMath::Vector4 rear;
            UMath::Vector4 right;
            UMath::Vector4 left;
            UMath::Vector4 rotcenter;
            UMath::Vector4 frontrear;
            UMath::Vector4 rightleft;

            UMath::AddScale(corners[0], corners[1], 0.5f, front);
            UMath::AddScale(corners[2], corners[3], 0.5f, rear);
            UMath::AddScale(corners[0], corners[2], 0.5f, left);
            UMath::AddScale(corners[1], corners[3], 0.5f, right);

            UMath::Subxyz(front, rear, mat.v2);
            UMath::Unitxyz(mat.v2, mat.v2);
            UMath::Subxyz(right, left, mat.v0);
            UMath::Unitxyz(mat.v0, mat.v0);

            UMath::UnitCross(reinterpret_cast<UMath::Vector3 &>(mat.v2), reinterpret_cast<UMath::Vector3 &>(mat.v0),
                             reinterpret_cast<UMath::Vector3 &>(mat.v1));
            UMath::UnitCross(reinterpret_cast<UMath::Vector3 &>(mat.v1), reinterpret_cast<UMath::Vector3 &>(mat.v2),
                             reinterpret_cast<UMath::Vector3 &>(mat.v0));

            UMath::Rotate(center, mat, rotcenter);

            UMath::AddScalexyz(front, rear, 0.5f, frontrear);
            UMath::AddScalexyz(right, left, 0.5f, rightleft);
            UMath::AddScalexyz(frontrear, rightleft, 0.5f, vpos);
            UMath::Subxyz(vpos, rotcenter, vpos);

            position = UMath::Vector4To3(vpos);
        }
    }

    rb->PlaceObject(mat, position);

    IPlayer *player = GetPlayer();
    IOnlinePlayer *onlineplayer;

    if (player) {

        if (player->QueryInterface(&onlineplayer)) {
            onlineplayer->Reposition();
        }
    }

    if (mArticulation && !mArticulation->Pose()) {
        success = false;
    }

    ResetBehavior(BEHAVIOR_MECHANIC_SUSPENSION);
    ResetBehavior(BEHAVIOR_MECHANIC_ENGINE);
    ResetBehavior(BEHAVIOR_MECHANIC_AI);
    ResetBehavior(BEHAVIOR_MECHANIC_RESET);

    SetSpeed(0.0f);

    mOffWorld = !success;

    AITarget::Track(this);

    return success;
}

void PVehicle::OnAttributeChange(const Attrib::Collection *collection, Attrib::Key attribkey) {}

void PVehicle::OnEnableModeling() {}

void PVehicle::OnDisableModeling() {
    IEffects *ieff;

    if (ISimable::QueryInterface(&ieff)) {
        ieff->Purge();
    }
}

void PVehicle::SetSpeed(float speed) {
    mSpeed = speed;
    mPerfectLaunch.Clear();

    if (mCollisionBody) {
        UMath::Vector3 vel;
        UMath::Scale(mCollisionBody->GetForwardVector(), speed, vel);
        GetRigidBody()->SetLinearVelocity(vel);

        if (mSuspension) {
            mSuspension->MatchSpeed(speed);
        }

        if (mEngine) {
            mEngine->MatchSpeed(speed);
        }

        UpdateLocalVelocities();

        if (mArticulation && mArticulation->GetTrailer()) {
            mArticulation->GetTrailer()->SetSpeed(speed);
        }
    }
}

void PVehicle::CommitBehaviorOverrides() {
    if (mOverrideDirty) {
        mOverrideDirty = false;
        ReloadBehaviors();
    }
}

bool PVehicle::IsLoading() const {
    if ((mRenderable && !mRenderable->IsRenderable()) || //
        (mAudible && !mAudible->IsAudible())) {
        return true;
    }

    if (mArticulation) {
        IVehicle *trailer = mArticulation->GetTrailer();
        if (trailer) {
            return trailer->IsLoading();
        }
    }

    return false;
}

void PVehicle::DoDebug(float dT) {}

void PVehicle::UpdateLocalVelocities() {
    IRigidBody *rb = GetRigidBody();
    if (!rb || !mCollisionBody) {
        UMath::Clear(mLocalVel);
        mSlipAngle = 0.0f;
        mSpeed = 0.0f;
        mAbsSpeed = 0.0f;
    } else {
        mLocalVel = rb->GetLinearVelocity();
        rb->ConvertWorldToLocal(mLocalVel, false);
        mSlipAngle = UMath::Atan2a(mLocalVel.x, UMath::Abs(mLocalVel.z));

        mSpeed = rb->GetSpeed();
        if (UMath::Dot(mCollisionBody->GetForwardVector(), rb->GetLinearVelocity()) < 0.0f) {
            mSpeed = -mSpeed;
        }
        mAbsSpeed = UMath::Abs(mSpeed);
    }
}

void PVehicle::CheckOffWorld() {
    if (IsBehaviorActive(BEHAVIOR_MECHANIC_SUSPENSION)) {
        if (GetWPos().GetSurface() == SimSurface::kNull.GetConstCollection()) {
            if (mSuspension) {
                unsigned int numnull = 0;

                for (unsigned int i = 0; i < mSuspension->GetNumWheels(); ++i) {
                    if (mSuspension->GetWheelRoadSurface(i).GetConstCollection() == SimSurface::kNull.GetConstCollection()) {
                        ++numnull;
                    }
                }

                mOffWorld = numnull > 1;
            } else {
                mOffWorld = true;
            }
        } else {
            mOffWorld = false;
        }
    } else {
        float height = 0.0f;

        mOffWorld = !WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(GetPosition(), height, NULL);
    }
}

void PVehicle::DoStaging(float dT) {
    if (!mPerfectLaunch.IsSet()) {
        mPerfectLaunch.Amount = 0.0f;

        IRaceEngine *iracer;
        if (mEngine && mEngine->QueryInterface(&iracer)) {
            float range = 0.0f;
            float perfect = iracer->GetPerfectLaunchRange(range);
            if (range > 0.0f && perfect > 0.0f) {
                float rpm = mEngine->GetRPM();
                float delta = rpm - perfect;
                if (delta < range && delta > 0.0f) {
                    mPerfectLaunch.Amount = (delta / range) * 0.5f + 0.5f;
                }
            }
        }
    }
}

void PVehicle::OnTaskSimulate(float dT) {

    bool onscreen = IsBehaviorActive(BEHAVIOR_MECHANIC_DRAW) && mRenderable && mRenderable->IsRenderable();
    if (onscreen) {
        if (!mRenderable->InView()) {

            mOffScreenTime += dT;
            mOnScreenTime = 0.0f;
        } else {

            mOffScreenTime = 0.0f;
            mOnScreenTime += dT;
        }
    } else {

        mOffScreenTime = 0.0f;
        mOnScreenTime = 0.0f;
    }

    CommitBehaviorOverrides();

    DoDebug(dT);

    if (mCollisionBody) {

        if (mCollisionBody->IsModeling() != mIsModeling) {

            mIsModeling = mCollisionBody->IsModeling();
            if (mIsModeling) {

                OnEnableModeling();
            } else {

                OnDisableModeling();
            }
        } else {

            mIsModeling = mCollisionBody->IsModeling();
        }
    }

    if (mPhysicsMode != PHYSICS_MODE_INACTIVE) {

        UpdateLocalVelocities();
        CheckOffWorld();
    }

    if (mPhysicsMode == PHYSICS_MODE_SIMULATED) {

        PauseBehavior(BEHAVIOR_MECHANIC_SUSPENSION, mCollisionBody->IsSleeping() && IsDestroyed());

        if (mTranny) {

            if (!mTranny->IsGearChanging()) {
                mSpeedometer = mTranny->GetSpeedometer();
            }
        } else {
            mSpeedometer = mAbsSpeed;
        }

        if (mSuspension) {

            unsigned int wheelsonground = mSuspension->GetNumWheelsOnGround();
            if (!wheelsonground && mWheelsOnGround && mDriverClass == DRIVER_HUMAN) {

                new EPlayerAirborne(GetInstanceHandle());
            }

            mWheelsOnGround = wheelsonground;
        }

        if (!mSuspension || !mWheelsOnGround) {

            mTimeInAir += dT;
        } else {

            mTimeInAir = 0.0f;
        }

        if (IsStaging()) {

            DoStaging(dT);
        } else if (mPerfectLaunch.IsSet() && mTranny) {

            if (!mTranny->IsGearChanging()) {

                mPerfectLaunch.Tick(dT);
            } else {

                if (mDriverStyle == STYLE_DRAG) {

                    mPerfectLaunch.Clear();
                }
            }

            if (GetSpeed() > MPH2MPS(60.0f)) {

                mPerfectLaunch.Clear();
            }
        }
    } else if (mPhysicsMode == PHYSICS_MODE_EMULATED) {

        mTimeInAir = 0.0f;
        if (mSuspension) {
            mWheelsOnGround = mSuspension->GetNumWheels();
        } else {
            mWheelsOnGround = 0;
        }
        mSpeedometer = mAbsSpeed;
    } else {

        mTimeInAir = 0.0f;
        mWheelsOnGround = 0;
        mSpeedometer = 0.0f;
    }
}

bool PVehicle::OnExplosion(const UMath::Vector3 &normal, const UMath::Vector3 &position, float dT, IExplosion *explosion) {
    unsigned int targets = explosion->GetTargets();

    if ((targets & 2) == 0 || //
        (IsPlayer() && (targets & 4) == 0)) {
        return false;
    }

    if (GetCausality() == NULL && explosion->GetCausality()) {
        ICause *icause = ICause::FindInstance(explosion->GetCausality());

        if (icause) {
            icause->OnCausedExplosion(explosion, this);
        }
    }

    IRigidBody *irb = GetRigidBody();

    float scale = 800.0f / irb->GetMass();
    float force = explosion->GetExpansionSpeed() * scale;

    UMath::Vector3 vel;
    irb->GetPointVelocity(position, vel);

    float dot = UMath::Dot(vel, normal);
    if (dot < force) {

        UMath::Vector3 impulse;
        UMath::Scale(normal, force - dot, impulse);

        UMath::Vector3 forcevec;
        UMath::Scale(impulse, irb->GetMass() / dT, forcevec);

        irb->ResolveForce(forcevec, position);
    }

    if (mSequencer) {
        mSequencer->ProcessStimulus(UCRC32_EXPLOSION, Sim::GetTime(), NULL, EventSequencer::QUEUE_ALLOW);

        if (explosion->HasDamage()) {
            mSequencer->ProcessStimulus(UCRC32_EXPLOSION_DAMAGE, Sim::GetTime(), NULL, EventSequencer::QUEUE_ALLOW);
        }
    }

    return true;
}

void PVehicle::OnTaskFX(float dT) {
    IRigidBody &rigidBody = *GetRigidBody();

    if (!INIS::Get()) {
        GlareOn(VehicleFX::LIGHT_HEADLIGHTS);
    }

    bool do_brake = mInput && mInput->GetControls().fBrake > 0.0f;

    if (!IsPlayer()) {
        if (do_brake) {
            mBrakeTime += dT;
        } else {
            mBrakeTime = 0.0f;
        }

        do_brake = mBrakeTime > 0.5f;
    }

    if (do_brake) {
        GlareOn(VehicleFX::LIGHT_BRAKELIGHTS);
    } else {
        GlareOff(VehicleFX::LIGHT_BRAKELIGHTS);
    }

    if (mTranny && mTranny->IsReversing()) {
        GlareOn(VehicleFX::LIGHT_REVERSE);
    } else {
        GlareOff(VehicleFX::LIGHT_REVERSE);
    }
}

bool PVehicle::SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) {
    if (IsDirty()) {
        return false;
    }

    data->fhSimable = reinterpret_cast<uintptr_t>(ISimable::GetInstanceHandle());
    data->fWorldID = GetWorldID();

    IRigidBody *irb = GetRigidBody();
    if (irb) {
        UMath::Vector3 fwd;

        data->fPosition = UMath::Vector4Make(irb->GetPosition(), 0.0f);

        irb->GetForwardVector(fwd);
        data->fVector = UMath::Vector4Make(fwd, 0.0f);
        data->fVelocity = UMath::Vector4Make(irb->GetLinearVelocity(), 1.0f);
        data->fAngularVelocity = UMath::Vector4Make(irb->GetAngularVelocity(), 1.0f);
    }

    if (mRenderable) {
        data->fhModel = reinterpret_cast<uintptr_t>(mRenderable->GetModelHandle());
    }

    return true;
}

void PVehicle::GlareOn(VehicleFX::ID glare) {
    mGlareState |= glare;
}

void PVehicle::GlareOff(VehicleFX::ID glare) {
    mGlareState &= ~glare;
}

void PVehicle::DebugObject() {
    PhysicsObject::DebugObject();
}

void PVehicle::ReloadBehaviors() {
    UMath::Vector3 pos = GetRigidBody()->GetPosition();
    UMath::Matrix4 mat;
    GetRigidBody()->GetMatrix4(mat);

    LoadBehaviors(pos, mat);
}

void PVehicle::SetAnimating(bool animate) {
    if (animate != mAnimating) {
        mBehaviorOverrides.clear();

        mAnimating = animate;
        ReloadBehaviors();
        PauseBehavior(BEHAVIOR_MECHANIC_AI, animate);
        if (mCollisionBody) {
            mCollisionBody->SetAnimating(mAnimating);
        }
    }
}

void PVehicle::Activate() {
    if (mPhysicsMode == PHYSICS_MODE_INACTIVE) {
        SetPhysicsMode(PHYSICS_MODE_SIMULATED);
    }
}

void PVehicle::Deactivate() {
    SetPhysicsMode(PHYSICS_MODE_INACTIVE);
}

void PVehicle::SetPhysicsMode(PhysicsMode mode) {
    if (mode != mPhysicsMode) {
        OnEndMode(mPhysicsMode);
        mPhysicsMode = mode;
        OnBeginMode(mode);
    }
    if (mArticulation) {
        IVehicle *trailer = mArticulation->GetTrailer();
        if (trailer) {
            trailer->SetPhysicsMode(mode);
        }
    }
}

void PVehicle::OnBeginMode(const PhysicsMode mode) {
    if (mode == PHYSICS_MODE_INACTIVE) {
        AddToList(VEHICLE_INACTIVE);

        if (mCollisionBody) {
            mCollisionBody->DisableTriggering();
        }

        Reset();
    } else if (mode == PHYSICS_MODE_EMULATED) {
    } else if (mode == PHYSICS_MODE_SIMULATED) {
        if (mCollisionBody) {
            mCollisionBody->EnableModeling();
        }

        CameraAI::AddAvoidable(this);

        PauseBehavior(BEHAVIOR_MECHANIC_DRAW, false);
        PauseBehavior(BEHAVIOR_MECHANIC_SUSPENSION, false);
        PauseBehavior(BEHAVIOR_MECHANIC_ENGINE, false);
        PauseBehavior(BEHAVIOR_MECHANIC_RIGIDBODY, false);
        PauseBehavior(BEHAVIOR_MECHANIC_EFFECTS, false);
        PauseBehavior(BEHAVIOR_MECHANIC_RESET, false);
        PauseBehavior(BEHAVIOR_MECHANIC_AUDIO, false);
        PauseBehavior(BEHAVIOR_MECHANIC_DAMAGE, false);
    }
}

void PVehicle::OnEndMode(const PhysicsMode mode) {
    if (mode == PHYSICS_MODE_INACTIVE) {
        if (mCollisionBody) {
            mCollisionBody->EnableTriggering();
        }

        Reset();
        UnList(VEHICLE_INACTIVE);
    } else if (mode == PHYSICS_MODE_EMULATED) {
    } else if (mode == PHYSICS_MODE_SIMULATED) {
        if (mCollisionBody) {
            mCollisionBody->DisableModeling();
        }

        CameraAI::RemoveAvoidable(this);

        PauseBehavior(BEHAVIOR_MECHANIC_DRAW, true);
        PauseBehavior(BEHAVIOR_MECHANIC_SUSPENSION, true);
        PauseBehavior(BEHAVIOR_MECHANIC_ENGINE, true);
        PauseBehavior(BEHAVIOR_MECHANIC_RIGIDBODY, true);
        PauseBehavior(BEHAVIOR_MECHANIC_EFFECTS, true);
        PauseBehavior(BEHAVIOR_MECHANIC_RESET, true);
        PauseBehavior(BEHAVIOR_MECHANIC_AUDIO, true);
        PauseBehavior(BEHAVIOR_MECHANIC_DAMAGE, true);
    }
}

const Physics::Tunings *PVehicle::GetTunings() const {
    if (GetDriverClass() != DRIVER_HUMAN) {
        return NULL;
    }
    {
        if (Tweak_UseTweakerTunings) {
            struct DefaultTunings : Physics::Tunings {};
            static DefaultTunings tunings;
            tunings.Value[Physics::Tunings::HANDLING] = 0.0f;
            tunings.Value[Physics::Tunings::BRAKES] = 0.0f;
            tunings.Value[Physics::Tunings::RIDEHEIGHT] = 0.0f;
            tunings.Value[Physics::Tunings::STEERING] = 0.0f;
            tunings.Value[Physics::Tunings::AERODYNAMICS] = Tweak_TuningAero;
            tunings.Value[Physics::Tunings::NOS] = 0.0f;
            tunings.Value[Physics::Tunings::INDUCTION] = 0.0f;
            return &tunings;
        }

        return mCustomization != NULL ? mCustomization->GetTunings() : NULL;
    }
}

void PVehicle::SetTunings(const Physics::Tunings &tunings) {
    if (mCustomization) {
        for (unsigned int i = 0; i < Physics::Tunings::MAX_TUNINGS; ++i) {
            mCustomization->SetTuning((Physics::Tunings::Path)i, tunings.Value[i]);
        }
    }
}

void PVehicle::OnDebugDraw() {}

void PVehicle::ComputeHeading(UMath::Vector3 *v) {
    IRigidBody *rb = GetRigidBody();
    float speed = rb->GetSpeed();

    UMath::Vector3 fwd;
    rb->GetForwardVector(fwd);
    UMath::Vector3 vel = rb->GetLinearVelocity();
    if (speed > 0.01f) {
        VU0_v3unit(vel, vel);
    }

    float t = UMath::Clamp(speed, 0.0f, 1.0f);
    UMath::Scale(fwd, 1.0f - t, fwd);
    UMath::ScaleAdd(vel, t, fwd, fwd);
    VU0_v3unit(fwd, *v);
}

void PVehicle::ForceStopOn(char forceStopBits) {
    mForceStop |= forceStopBits;
    if (mInput) {
        mInput->ClearInput();
    }
}

void PVehicle::ForceStopOff(char forceStopBits) {
    mForceStop &= ~forceStopBits;
    if (mInput) {
        mInput->ClearInput();
    }
}
