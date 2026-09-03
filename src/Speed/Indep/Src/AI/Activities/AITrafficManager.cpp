#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/trafficpattern.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/controller_hash.h"
#include "Speed/Indep/Src/Generated/Messages/MSetTrafficSpeed.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Input/ActionRef.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficCenter.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

static const unsigned int MAX_PATTERN_TYPES = 10; // Decl: 81

// Decl: 169
static const float Tweak_TrafficOffScreenDistance[11] = {
    130.0f, 120.0f, 110.0f, 100.0f, 90.0f, 80.0f, 70.0f, 60.0f, 55.0f, 45.0f, 40.0f,
};

// Decl: 176
static const float Tweak_TrafficOffScreenTime[11] = {
    12.0f, 10.0f, 9.0f, 8.0f, 7.0f, 6.5f, 6.0f, 5.5f, 5.0f, 4.5f, 4.0f,
};

static Table TrafficOffScreenDistance(Tweak_TrafficOffScreenDistance, 11, 0.0f, 1.0f); // Decl: 249

static Table TrafficOffScreenTime(Tweak_TrafficOffScreenTime, 11, 0.0f, 1.0f); // Decl: 256

static const float Tweak_TrafficDensitySpawnRates[11] = {0.0f, 0.05f, 0.1f, 0.125f, 0.2f, 0.4f, 0.6f, 1.0f, 3.0f, 5.0f, 8.0f};
static Table TrafficDensitySpawnRates(Tweak_TrafficDensitySpawnRates, 11, 0.0f, 1.0f);

// total size: 0x8
// Decl: 428
struct PartChecker : public IModel::Enumerator {
    bool Valid; // offset 0x4, size 0x1, Decl: 430

    PartChecker() : Valid(false) {} // Decl: 433

    virtual ~PartChecker() {} // Decl: 436

    // Overrides: Enumerator
    // Decl: 439
    bool OnModel(IModel *model) override {
        if (model->InView()) {
            Valid = true;
            return false;
        }
        return true;
    }
};

DECLARE_CONTAINER_TYPE(TrafficList);
DECLARE_CONTAINER_TYPE(AITrafficManager_PatternMap);

// total size: 0x3C4
// Decl: 453
class AITrafficManager : public Sim::Activity, public ITrafficMgr, public IVehicleCache, public Debugable {
  public:
    struct PatternKey {
        int BHash;
        Attrib::Key CollectionKey;

        bool operator<(const PatternKey &rhs) const {
            return this->BHash < rhs.BHash;
        }
    };

    struct PatternMap : public UTL::Std::vector<PatternKey, _type_AITrafficManager_PatternMap> {
        Attrib::Key Find(int bhash) const {
            PatternKey key;
            key.BHash = bhash;
            key.CollectionKey = 0;
            const_iterator iter = std::lower_bound(this->begin(), this->end(), key);
            if (iter != this->end() && iter->BHash == bhash) {
                return iter->CollectionKey;
            }
            return 0;
        }
    };

    typedef UTL::Std::list<IVehicle *, _type_TrafficList> TrafficList;

    AITrafficManager(Sim::Param params);
    ~AITrafficManager() override;

    static Sim::IActivity *Construct(Sim::Param params);

    // ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    // IVehicleCache
    eVehicleCacheResult OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const override;
    void OnRemovedVehicleCache(IVehicle *ivehicle) override;

    // Decl: 472
    const char *GetCacheName() const override {
        return "AITrafficManager";
    }

    // ITrafficMgr
    void FlushAllTraffic(bool release) override;

  protected:
    // IAttachable
    void OnAttached(IAttachable *pOther) override;
    void OnDetached(IAttachable *pOther) override;

    virtual void OnDebugDraw();

  private:
    bool FindSpawnPoint(WRoadNav &nav) const;
    bool FindCollisions(const UMath::Vector3 &spawnpoint) const;
    bool ChoosePattern();
    void SetTrafficPattern(Attrib::Key pattern_key);
    void Update(float dT);
    Attrib::Key NextSpawn();
    IVehicle *GetAvailableTrafficVehicle(Attrib::Key key, bool makenew);
    bool CheckRace(const WRoadNav &nav) const;
    bool ValidateVehicle(IVehicle *ivehicle, float density) const;
    bool SpawnTraffic();
    bool NeedsTraffic() const;
    float ComputeDensity() const;
    void UpdateDebug();

    HSIMTASK mTask;          // offset 0x68, size 0x4
    unsigned int mSpawnIdx;  // offset 0x6C, size 0x4
    float mPatternTimer[10]; // offset 0x70, size 0x28
    float mNewInstanceTimer; // offset 0x98, size 0x4
    TrafficList mVehicles;   // offset 0x9C, size 0x8

    static float mTrafficMinSpawnDist;
    static float mTrafficMaxSpawnDist;

    ActionQueue *mActionQ;                // offset 0xA4, size 0x4
    eTrafficDensity mDensity;             // offset 0xA8, size 0x4
    PatternMap mPatternMap;               // offset 0xAC, size 0x10
    WRoadNav mNav;                        // offset 0xBC, size 0x2F0
    float mOncommingChance;               // offset 0x3AC, size 0x4
    Attrib::Gen::trafficpattern mPattern; // offset 0x3B0, size 0x14
};

float AITrafficManager::mTrafficMinSpawnDist = 225.0f;
float AITrafficManager::mTrafficMaxSpawnDist = 300.0f;

// Decl: 554
BIND_ACTIVITY_FACTORY(AITrafficManager);

// Decl: 563
AITrafficManager::AITrafficManager(Sim::Param params)
    : Sim::Activity(2),       //
      ITrafficMgr(this),      //
      IVehicleCache(this),    //
      mSpawnIdx(0),           //
      mOncommingChance(0.5f), //
      mVehicles(),            //
      mPatternMap(),          //
      mNewInstanceTimer(0),   //
      mNav(),                 //
      mPattern(static_cast<Attrib::Collection *>(nullptr), 0, nullptr) {
    this->MakeDebugable(DBG_AI);
    bMemSet(this->mPatternTimer, 0, sizeof(this->mPatternTimer));
    this->mNewInstanceTimer = 0;
    this->SetTrafficPattern(Attrib::key_default);
    this->mVehicles.clear();
    this->mTask = this->AddTask(UCrc32("AITrafficManager"), 0.5f, 0.5f, Sim::TASK_FRAME_VARIABLE);
    Sim::ProfileTask(this->mTask, "AITrafficManager");
    this->mActionQ = new ActionQueue(0, Attrib::Hash::controller::key_debug, "AITrafficManager", false);
    const Attrib::Class *patternclass = Attrib::Database::Get().GetClass(Attrib::ClassName::trafficpattern);
    if (patternclass != nullptr) {
        this->mPatternMap.clear();
        this->mPatternMap.reserve(patternclass->GetNumCollections());
        Attrib::Key cKey = patternclass->GetFirstCollection();

        while (cKey != 0) {
            Attrib::Gen::trafficpattern pattern(cKey, 0, nullptr);
            const char *name = pattern.CollectionName();

            PatternKey key;
            key.BHash = bStringHash(name);
            key.CollectionKey = cKey;
            this->mPatternMap.insert(std::upper_bound(this->mPatternMap.begin(), this->mPatternMap.end(), key), key);

            cKey = patternclass->GetNextCollection(cKey);
        }
    }
}

AITrafficManager::~AITrafficManager() {
    this->RemoveTask(this->mTask);
    if (this->mActionQ != nullptr) {
        delete this->mActionQ;
        this->mActionQ = nullptr;
    }
}

Sim::IActivity *AITrafficManager::Construct(Sim::Param params) {
    if (SkipFE && SkipFEDisableTraffic) {
        return nullptr;
    }
    return new AITrafficManager(Sim::Param(params));
}

eVehicleCacheResult AITrafficManager::OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const {
    if (!this->IsAttached(removethis)) {
        return VCR_DONTCARE;
    }

    if (whosasking == this) {
        if (removethis->IsActive()) {
            return VCR_WANT;
        }
        if (removethis->IsLoading()) {
            return VCR_WANT;
        }
    } else if (ComparePtr(whosasking, INIS::Get())) {
        return VCR_DONTCARE;
    } else if (ComparePtr(whosasking, ICopMgr::Get())) {
        if (removethis->IsActive()) {
            return VCR_WANT;
        }
    } else if (GRaceStatus::Exists() && whosasking == &GRaceStatus::Get()) {
        return VCR_DONTCARE;
    } else if (GManager::Exists() && whosasking == &GManager::Get()) {
        return VCR_DONTCARE;
    }

    return VCR_DONTCARE;
}

void AITrafficManager::OnRemovedVehicleCache(IVehicle *ivehicle) {}

void AITrafficManager::OnAttached(IAttachable *pOther) {
    IVehicle *ivehicle;
    if (pOther->QueryInterface(&ivehicle)) {
        this->mVehicles.push_back(ivehicle);
    }
    this->Sim::Activity::OnAttached(pOther);
}

void AITrafficManager::OnDetached(IAttachable *pOther) {
    IVehicle *ivehicle;
    if (pOther->QueryInterface(&ivehicle)) {
        TrafficList::iterator iter = std::find(this->mVehicles.begin(), this->mVehicles.end(), ivehicle);
        if (iter != this->mVehicles.end()) {
            this->mVehicles.erase(iter);
        }
    }
    this->Sim::Activity::OnDetached(pOther);
}

struct TypeCounter {
    Attrib::Key Key;
    bool ActiveOnly;
    unsigned int Count;

  public:
    TypeCounter(Attrib::Key key, bool active_only) : Key(key), ActiveOnly(active_only), Count(0) {}

    void operator()(IVehicle *vehicle) {
        if (!this->ActiveOnly || vehicle->IsActive()) {
            if (vehicle->GetVehicleAttributes().GetCollection() == this->Key) {
                this->Count++;
            }
        }
    }
};

Attrib::Key AITrafficManager::NextSpawn() {
    unsigned int num_types = this->mPattern.Num_Vehicles();
    if (num_types == 0) {
        return 0;
    }
    unsigned int max_types = UMath::Min(num_types, 10u);
    Attrib::Key key = 0;
    for (unsigned int i = 0; i < max_types && key == 0; this->mSpawnIdx++, i++) {
        this->mSpawnIdx %= max_types;
        const TrafficPatternRecord &record = this->mPattern.Vehicles(this->mSpawnIdx);
        if (this->mPatternTimer[this->mSpawnIdx] > record.Rate && record.Rate > 0.0f) {
            TypeCounter t = std::for_each(this->mVehicles.begin(), this->mVehicles.end(), TypeCounter(record.Vehicle.GetCollectionKey(), true));
            if (t.Count < record.MaxInstances || record.MaxInstances == 0) {
                unsigned int max_traffic = this->mVehicles.size() + 10 - IVehicle::Count(VEHICLE_ALL);
                if (record.Percent == 0 || t.Count < UMath::Max(1U, max_traffic * record.Percent / 100)) {
                    key = record.Vehicle.GetCollectionKey();
                }
            }
        }
    }
    return key;
}

IVehicle *AITrafficManager::GetAvailableTrafficVehicle(Attrib::Key key, bool makenew) {
    if (key == 0) {
        return nullptr;
    }

    for (TrafficList::const_iterator iter = this->mVehicles.begin(); iter != this->mVehicles.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if ((!ivehicle->IsActive() || ivehicle->IsLoading()) && ivehicle->GetVehicleKey() == key) {
            return ivehicle;
        }
    }
    if (!makenew) {
        return nullptr;
    }
    UMath::Vector3 initialVec = {0.0f, 0.0f, 1.0f};
    UMath::Vector3 initialPos = {0.0f, 0.0f, 0.0f};
    VehicleParams params(this, DRIVER_TRAFFIC, key, initialVec, initialPos, 0, nullptr, nullptr);
    ISimable *isimable = ISimable::CreateInstance(UCrc32("PVehicle"), params);
    if (isimable != nullptr) {
        static_cast<IActivity *>(this)->Attach(isimable);
        IVehicle *ivehicle;
        if (isimable->QueryInterface(&ivehicle)) {
            ivehicle->GetAIVehiclePtr()->UnSpawn();
            this->mNewInstanceTimer = 0.0f;
            return ivehicle;
        }
    }
    return nullptr;
}

bool AITrafficManager::SpawnTraffic() {
    if (!this->mPattern.IsValid()) {
        return false;
    }

    if (!this->NeedsTraffic()) {
        return false;
    }

    if (!this->FindSpawnPoint(this->mNav)) {
        return false;
    }

    Attrib::Key key = this->NextSpawn();
    if (key == 0) {
        return false;
    }

    IVehicle *availableVehicle = this->GetAvailableTrafficVehicle(key, this->mNewInstanceTimer > this->mPattern.SpawnTime());
    if (availableVehicle == nullptr) {
        return false;
    }

    if (availableVehicle->IsLoading()) {
        return false;
    }

    IVehicleAI *ivehicleAI = availableVehicle->GetAIVehiclePtr();
    if (!ivehicleAI->ResetVehicleToRoadNav(&this->mNav)) {
        return false;
    }

    ivehicleAI->SetSpawned();
    availableVehicle->Activate();

    ITrafficAI *itv;
    if (availableVehicle->QueryInterface(&itv)) {
        float start_speed = UMath::Min(this->mPattern.SpeedStreet(), this->mPattern.SpeedHighway());
        itv->StartDriving(MPH2MPS(start_speed) * 0.75f);
    }

    MSetTrafficSpeed ai_msg(this->mPattern.SpeedStreet(), this->mPattern.SpeedHighway(), 0);
    ai_msg.SetID(availableVehicle->GetSimable()->GetWorldID());
    ai_msg.Post(UCrc32("AIAction"));

    Attrib::Key vehicle_key = availableVehicle->GetVehicleKey();
    unsigned int num_types = this->mPattern.Num_Vehicles();
    for (unsigned int i = 0; i < num_types && i < 10; i++) {
        const TrafficPatternRecord &record = this->mPattern.Vehicles(i);
        if (vehicle_key == record.Vehicle.GetCollectionKey()) {
            this->mPatternTimer[i] = 0.0f;
        }
    }

    return true;
}

bool AITrafficManager::NeedsTraffic() const {
    int inactive_count = 0;
    for (TrafficList::const_iterator iter = this->mVehicles.begin(); iter != this->mVehicles.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (!ivehicle->IsActive() && !ivehicle->IsLoading()) {
            inactive_count++;
        }
    }
    int active_count = IVehicle::Count(VEHICLE_ALL) - inactive_count;
    return static_cast<unsigned int>(active_count) < 10;
}

void AITrafficManager::UpdateDebug() {
    while (!this->mActionQ->IsEmpty()) {
        ActionRef aRef = this->mActionQ->GetAction();
        aRef.ID();
        this->mActionQ->PopAction();
    }
}

static bool RandomSortTCDir = false;

static bool RandomSortTC(ITrafficCenter *c0, ITrafficCenter *c1) {
    if (RandomSortTCDir) {
        return c0 < c1;
    }
    return c1 < c0;
}

void AITrafficManager::SetTrafficPattern(Attrib::Key pattern_key) {
    if (pattern_key == this->mPattern.GetCollection()) {
        return;
    }
    this->mPattern = Attrib::Gen::trafficpattern(pattern_key, 0, nullptr);
    bMemSet(this->mPatternTimer, 0, sizeof(this->mPatternTimer));

    unsigned int num_types = this->mPattern.Num_Vehicles();
    for (unsigned int i = 0; i < num_types && i < 10; i++) {
        const TrafficPatternRecord &record = this->mPattern.Vehicles(i);
        this->mPatternTimer[i] = record.Rate * bRandom(1.0f);
    }
}

bool AITrafficManager::FindCollisions(const UMath::Vector3 &spawnpoint) const {
    float worldHeight;
    if (!WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(spawnpoint, worldHeight, nullptr)) {
        return true;
    }

    for (ITrafficCenter::List::const_iterator iter = ITrafficCenter::GetList().begin(); iter != ITrafficCenter::GetList().end(); iter++) {
        UMath::Matrix4 basis;
        UMath::Vector3 velocity;
        ITrafficCenter *center = *iter;
        if (!center->GetTrafficBasis(basis, velocity)) {
            continue;
        }

        float distsq = UMath::DistanceSquarexz(UMath::Vector4To3(basis.v3), spawnpoint);
        if (distsq < 22500.0f) {
            return true;
        }
    }
    const IVehicle::List &vehicles = IVehicle::GetList(VEHICLE_ALL);
    for (IVehicle::List::const_iterator iter = vehicles.begin(); iter != vehicles.end(); iter++) {
        IVehicle *vehicle = *iter;
        if (!vehicle->IsActive()) {
            continue;
        }

        float distsq = UMath::DistanceSquarexz(vehicle->GetPosition(), spawnpoint);
        if (distsq < 400.0f) {
            return true;
        }
    }
    return false;
}

bool AITrafficManager::CheckRace(const WRoadNav &nav) const {
    if (!GRaceStatus::Exists()) {
        return true;
    }
    GRaceStatus &race = GRaceStatus::Get();
    if (race.GetPlayMode() != GRaceStatus::kPlayMode_Racing) {
        return true;
    }
    GRaceParameters *params = race.GetRaceParameters();
    if (params == nullptr || !params->HasFinishLine()) {
        return true;
    }
    const WRoadSegment *seg = nav.GetSegment();
    if (seg != nullptr && seg->IsInRace()) {
        return true;
    }
    return false;
}

bool AITrafficManager::FindSpawnPoint(WRoadNav &nav) const {
    RandomSortTCDir = !RandomSortTCDir;
    ITrafficCenter::Sort(RandomSortTC);
    nav.Reset();
    const ITrafficCenter::List &traffic_centers = ITrafficCenter::GetList();

    for (ITrafficCenter::List::const_iterator iter = traffic_centers.begin(); iter != traffic_centers.end(); ++iter) {
        UMath::Matrix4 basis;
        UMath::Vector3 velocity;
        ITrafficCenter *center = *iter;
        if (!center->GetTrafficBasis(basis, velocity)) {
            continue;
        }
        const UMath::Vector3 &position = UMath::Vector4To3(basis.v3);

        UMath::Vector3 direction = UMath::Vector4To3(basis.v2);
        // float angle = DEG2ANGLE((bRandom(2.0f) - 1.0f) * 45.0f);
        float angle = (bRandom(2.0f) - 1.0f) * 0.125f; // TODO
        UMath::Matrix4 rotation;
        UMath::SetYRot(rotation, angle);
        UMath::Rotate(direction, rotation, direction);

        float offset = (bRandom(2.0f) - 1.0f) * 50.0f;
        float camera_speed = UMath::Dot(velocity, direction);
        float time_offset = offset + 200.0f + UMath::Max(camera_speed, 0.0f);
        // float distance = time_offset; // TODO
        UMath::Vector3 spawnpoint;
        UMath::ScaleAdd(direction, time_offset, position, spawnpoint);

        float t = UMath::Ramp(camera_speed, 0.0f, 50.0f);
        float oncomming_chance = UMath::Lerp(1.0f, 0.5f, t);
        if (bRandom(1.0f) <= oncomming_chance) {
            UMath::Negate(direction);
        }

        nav.InitAtPoint(spawnpoint, direction, false, 1.0f);
        if (!nav.IsValid() || !nav.CanTrafficSpawn() || !this->CheckRace(nav)) {
            continue;
        }
        UMath::Vector3 nav_point = nav.GetPosition();
        if (!this->FindCollisions(nav_point)) {
            return true;
        }
    }
    return false;
}

bool AITrafficManager::ChoosePattern() {
    this->mOncommingChance = 0.5f;

    // huh
    if (this->mPatternMap.size() < 0) {
        int pattern_idx = this->mPatternMap.size() - 1;
        PatternKey &key = this->mPatternMap[pattern_idx];
        return this->mPattern.IsValid();
    }

    if (GRaceStatus::Exists()) {
        GRaceStatus &race = GRaceStatus::Get();
        if (race.GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
            unsigned int race_pattern = race.GetTrafficPattern();
            if (race_pattern != 0) {
                this->SetTrafficPattern(race_pattern);
                return this->mPattern.IsValid();
            }
        }
    }
    UMath::Vector3 pattern_center = UMath::Vector3::kZero;

    float count = 0.0f;

    for (ITrafficCenter::List::const_iterator iter = ITrafficCenter::GetList().begin(); iter != ITrafficCenter::GetList().end(); ++iter) {
        ITrafficCenter *center = *iter;
        UMath::Vector3 velocity;
        UMath::Matrix4 matrix;

        if (center->GetTrafficBasis(matrix, velocity)) {
            UMath::Add(pattern_center, Vector4To3(matrix.v3), pattern_center);
            count += 1.0f;
        }
    }

    if (count > 0.0f) {
        Scale(pattern_center, 1.0f / count, pattern_center);
        bVector2 point(pattern_center.z, -pattern_center.x);

        TrackPathZone *zone = TheTrackPathManager.FindZone(&point, TRACK_PATH_ZONE_TRAFFIC_PATTERN, nullptr);
        if (zone != nullptr) {
            this->SetTrafficPattern(this->mPatternMap.Find(zone->Data[0]));
        }
    }

    return this->mPattern.IsValid();
}

bool AITrafficManager::ValidateVehicle(IVehicle *ivehicle, float density) const {
    if (ivehicle == nullptr) {
        return false;
    }

    bool invalid = ivehicle->IsOffWorld();

    if (!invalid) {
        float offscreen_time = TrafficOffScreenTime.GetValue(density);
        float offscreen_dist = TrafficOffScreenDistance.GetValue(density);
        if (ivehicle->GetOffscreenTime() > offscreen_time) {
            invalid = Sim::DistanceToCamera(ivehicle->GetPosition()) > offscreen_dist;
        }
    }

    if (invalid) {
        IArticulatedVehicle *iarticulate;
        if (ivehicle->QueryInterface(&iarticulate)) {
            IVehicle *trailer = iarticulate->GetTrailer();
            if (trailer != nullptr) {
                if (ValidateVehicle(trailer, density)) {
                    return true;
                }
            }
        }
    }

    if (invalid) {
        ISimable *isimable = ivehicle->GetSimable();
        IModel *imodel = isimable->GetModel();
        if (imodel != nullptr) {
            PartChecker pc;
            if (static_cast<PartChecker *>(imodel->EnumerateChildren(&pc))->Valid) {
                return true;
            }
        }
    }

    return !invalid;
}

float AITrafficManager::ComputeDensity() const {
    float result;

    if (INIS::Exists() || (ICopMgr::Exists() && ICopMgr::Get()->IsCopRequestPending())) {
        return 0.0f;
    }

    if (SkipFE && !SkipFEDisableTraffic) {
        result = UMath::Clamp(static_cast<float>(SkipFETrafficDensity) * 0.01f, 0.0f, 1.0f);
    } else {
        result = 0.0f;

        if (GRaceStatus::Exists()) {
            GRaceStatus::PlayMode mode = GRaceStatus::Get().GetPlayMode();

            if (mode == GRaceStatus::kPlayMode_Racing) {
                result = UMath::Clamp(static_cast<float>(GRaceStatus::Get().GetTrafficDensity()) * 0.01f, 0.0f, 1.0f);
            } else if (mode == GRaceStatus::kPlayMode_Roaming) {
                result = 1.0f;
            }
        }

        if (IPursuit::Count() != 0) {
            result *= 0.75f;
        }
    }

    return result;
}

void AITrafficManager::Update(float dT) {
    this->UpdateDebug();
    float density = this->ComputeDensity();

    if (density > 0.0f && this->ChoosePattern()) {
        float spawn_time = dT * TrafficDensitySpawnRates.GetValue(density);
        for (int i = 0; i < 10u; i++) {
            this->mPatternTimer[i] += spawn_time;
        }
        this->mNewInstanceTimer += dT;
        this->SpawnTraffic();
    }

    for (TrafficList::const_iterator iter = this->mVehicles.begin(); iter != this->mVehicles.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->IsActive() && !this->ValidateVehicle(ivehicle, density)) {
            ivehicle->GetAIVehiclePtr()->UnSpawn();
        }
    }
}

void AITrafficManager::FlushAllTraffic(bool release) {
    for (TrafficList::const_iterator iter = this->mVehicles.begin(); iter != this->mVehicles.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (release) {
            ISimable *isimable;
            if (ivehicle->QueryInterface(&isimable)) {
                isimable->Kill();
            }
        } else if (ivehicle->IsActive()) {
            ivehicle->GetAIVehiclePtr()->UnSpawn();
        }
    }

    if (release) {
        this->mVehicles.clear();
    }

    if (this->mPattern.IsValid()) {
        unsigned int num_types = this->mPattern.Num_Vehicles();
        for (unsigned int i = 0; i < num_types && i < 10; i++) {
            const TrafficPatternRecord &record = this->mPattern.Vehicles(i);
            this->mPatternTimer[i] = record.Rate * bRandom(1.0f);
        }
    }
}

bool AITrafficManager::OnTask(HSIMTASK htask, float dT) {
    ProfileNode profile_node("AITrafficManager::OnTask", 0);
    Object::OnTask(htask, dT);
    if (htask == this->mTask) {
        this->Update(dT);
        return true;
    }
    return false;
}

void AITrafficManager::OnDebugDraw() {}
