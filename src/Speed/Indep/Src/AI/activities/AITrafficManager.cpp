#include "Speed/Indep/Src/AI/activities/AITrafficManager.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/trafficpattern.h"
#include "Speed/Indep/Src/Generated/Messages/MSetTrafficSpeed.h"
#include "Speed/Indep/Src/Input/ActionRef.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficCenter.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <algorithm>

// TODO move
extern BOOL SkipFETrafficDensity; // size: 0x4
extern BOOL SkipFEDisableTraffic; // size: 0x4
extern BOOL SkipFE;               // size: 0x4

using namespace Attrib::Gen;
using UTL::Collections::Listable;
using UTL::Collections::ListableSet;
using UTL::Collections::Singleton;

// AITrafficManager::AITrafficManager(Sim::Param params) {}

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
        return VCR_DONTCARE;
    }

    if (ComparePtr(whosasking, Singleton<INIS>::Get())) {
        return removethis->IsActive() ? VCR_WANT : VCR_DONTCARE;
    }

    if (ComparePtr(whosasking, Singleton<ICopMgr>::Get())) {
        return removethis->IsActive() ? VCR_WANT : VCR_DONTCARE;
    }

    if (GRaceStatus::Exists() && whosasking == &GRaceStatus::Get()) {
        return VCR_DONTCARE;
    }

    if (GManager::Exists() && whosasking == &GManager::Get()) {
        return VCR_DONTCARE;
    }

    return VCR_WANT;
}

void AITrafficManager::OnRemovedVehicleCache(IVehicle *ivehicle) {}

void AITrafficManager::OnAttached(IAttachable *pOther) {
    IVehicle *ivehicle;
    if (pOther->QueryInterface(&ivehicle)) {
        this->mVehicles.push_back(ivehicle);
    }
    Sim::Activity::OnAttached(pOther);
}

void AITrafficManager::OnDetached(IAttachable *pOther) {
    IVehicle *ivehicle;
    if (pOther->QueryInterface(&ivehicle)) {
        std::list<IVehicle *>::iterator iter = std::find(this->mVehicles.begin(), this->mVehicles.end(), ivehicle);
        if (iter != this->mVehicles.end()) {
            this->mVehicles.erase(iter);
        }
    }
    Sim::Activity::OnDetached(pOther);
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
    unsigned int num_types = mPattern.Num_Vehicles();
    if (num_types == 0) {
        return 0;
    }
    unsigned int max_types = UMath::Min(num_types, 10);
    Attrib::Key key = 0;
    for (unsigned int i = 0; i < max_types && key == 0; i++, mSpawnIdx++) {
        mSpawnIdx %= max_types;
        const TrafficPatternRecord &record = mPattern.Vehicles(mSpawnIdx);
        if (mPatternTimer[mSpawnIdx] > record.Rate && record.Rate > 0.0f) {
            TypeCounter t = std::for_each(mVehicles.begin(), mVehicles.end(), TypeCounter(record.Vehicle.GetCollectionKey(), true));
            if (t.Count < record.MaxInstances || record.MaxInstances == 0) {
                unsigned int max_traffic = mVehicles.size() + 10 - IVehicle::Count(VEHICLE_ALL);
                if (record.Percent == 0 || t.Count < UMath::Max(1, max_traffic * record.Percent / 100)) {
                    key = record.Vehicle.GetCollectionKey();
                }
            }
        }
    }
    return key;
}

// UNSOLVED have to use TypeName and fix the virtual stuff
IVehicle *AITrafficManager::GetAvailableTrafficVehicle(Attrib::Key key, bool makenew) {
    if (key == 0) {
        return nullptr;
    }

    for (std::list<IVehicle *>::const_iterator iter = mVehicles.begin(); iter != mVehicles.end(); ++iter) {
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
    VehicleParams params(this, DRIVER_TRAFFIC, key, initialVec, initialPos, 0, nullptr, 0);
    ISimable *isimable = UTL::COM::Factory<Sim::Param, ISimable, UCrc32>::CreateInstance(UCrc32("PVehicle"), Sim::Param(params));
    if (isimable) {
        // TODO hmm?
        this->Detach(isimable);
        IVehicle *ivehicle;
        if (isimable->QueryInterface(&ivehicle)) {
            ivehicle->GetAIVehiclePtr()->UnSpawn();
            mNewInstanceTimer = 0.0f;
            return ivehicle;
        }
    }
    return nullptr;
}

// TODO unsolved
bool AITrafficManager::SpawnTraffic() {
    if (!mPattern.IsValid()) {
        return false;
    }

    if (!this->NeedsTraffic()) {
        return false;
    }

    if (!this->FindSpawnPoint(mNav)) {
        return false;
    }

    Attrib::Key key = NextSpawn();
    if (key == 0) {
        return false;
    }

    IVehicle *availableVehicle = GetAvailableTrafficVehicle(key, mNewInstanceTimer > mPattern.SpawnTime(0));
    if (availableVehicle == nullptr) {
        return false;
    }

    if (availableVehicle->IsLoading()) {
        return false;
    }

    IVehicleAI *ivehicleAI = availableVehicle->GetAIVehiclePtr();
    // TODO
    if (!ivehicleAI->ResetVehicleToRoadNav(&mNav)) {
        return false;
    }

    // TODO
    ivehicleAI->SetSpawned();
    availableVehicle->Activate();

    ITrafficAI *itv;
    if (availableVehicle->QueryInterface(&itv)) {
        float start_speed = UMath::Min(mPattern.SpeedStreet(0), mPattern.SpeedHighway(0));
        itv->StartDriving(MPH2MPS(start_speed) * 0.75f);
    }

    MSetTrafficSpeed ai_msg(mPattern.SpeedStreet(0), mPattern.SpeedHighway(0), false);
    ai_msg.SetID(availableVehicle->GetSimable()->GetWorldID());
    ai_msg.Post(UCrc32("AIAction"));

    Attrib::Key vehicle_key = availableVehicle->GetVehicleKey();
    unsigned int num_types = mPattern.Num_Vehicles();
    for (unsigned int i = 0; i < num_types && i < 10; i++) {
        const TrafficPatternRecord &record = mPattern.Vehicles(i);
        if (vehicle_key == record.Vehicle.GetCollectionKey()) {
            mPatternTimer[i] = 0.0f;
        }
    }

    return true;
}

bool AITrafficManager::NeedsTraffic() const {
    int inactive_count = 0;
    for (std::list<IVehicle *>::const_iterator iter = mVehicles.begin(); iter != mVehicles.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (!ivehicle->IsActive() && !ivehicle->IsLoading()) {
            inactive_count++;
        }
    }
    int active_count = IVehicle::Count(VEHICLE_ALL);
    return (unsigned int)(active_count - inactive_count) < 10;
}

void AITrafficManager::UpdateDebug() {
    while (!this->mActionQ->IsEmpty()) {
        ActionRef aRef = this->mActionQ->GetAction();
        aRef.ID();
        this->mActionQ->PopAction();
    }
}

static bool RandomSortTCDir;

static bool RandomSortTC(ITrafficCenter *c0, ITrafficCenter *c1) {
    if (RandomSortTCDir) {
        return c0 < c1;
    }
    return c1 < c0;
}

void AITrafficManager::SetTrafficPattern(unsigned int pattern_key) {
    if (pattern_key == this->mPattern.GetCollection()) {
        return;
    }
    this->mPattern = trafficpattern(Attrib::FindCollection(trafficpattern::ClassKey(), pattern_key), 0, nullptr);
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
        if (center->GetTrafficBasis(basis, velocity)) {
            float distsq = DistanceSquarexz(Vector4To3(basis.v3), spawnpoint);
            if (distsq < 22500.0f) {
                return true;
            }
        }
    }
    const IVehicle::List &vehicles = IVehicle::GetList(VEHICLE_ALL);
    for (IVehicle::List::const_iterator iter = vehicles.begin(); iter != vehicles.end(); iter++) {
        IVehicle *vehicle = *iter;
        if (vehicle->IsActive()) {
            float distsq = DistanceSquarexz(vehicle->GetPosition(), spawnpoint);
            if (distsq < 400.0f) {
                return true;
            }
        }
    }
    return false;
}

bool AITrafficManager::CheckRace(const WRoadNav &nav) const {}

// UNSOLVED
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
        float angle = DEG2ANGLE(bRandom(2.0f) - 1.0f);
        UMath::Matrix4 rotation;
        UMath::SetYRot(rotation, angle);
        UMath::Rotate(direction, rotation, direction);

        float offset = bRandom(2.0f);
        float camera_speed = UMath::Max(0.0f, UMath::Dot(velocity, direction));
        UMath::Vector3 spawnpoint;
        // TODO
        UMath::ScaleAdd(position, (offset - 1.0f) * 50.0f + 200.0f + camera_speed, velocity, spawnpoint);

        float t = UMath::Ramp(camera_speed, 0.0f, 100.0f);
        float oncomming_chance = bRandom(1.0f);
        // TODO
        camera_speed = UMath::Lerp(1.0f, 1.5f, t);
        if (oncomming_chance <= camera_speed) {
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

// UNSOLVED
bool AITrafficManager::ChoosePattern() {
    mOncommingChance = 0.5f;

    if (GRaceStatus::Exists()) {
        GRaceStatus &race = GRaceStatus::Get();
        if (race.GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
            unsigned int race_pattern = race.GetTrafficPattern();
            if (race_pattern != 0) {
                SetTrafficPattern(race_pattern);
                return mPattern.IsValid();
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
        if (zone) {
            // TODO
            SetTrafficPattern(mPatternMap.Find(zone->Data[0]));
        }
    }

    return mPattern.IsValid();
}

bool AITrafficManager::ValidateVehicle(IVehicle *ivehicle, float density) const {}

// TODO move
static Table TrafficDensitySpawnRates;

// UNSOLVED virtual stuff
void AITrafficManager::Update(float dT) {
    UpdateDebug();
    float density = ComputeDensity();

    if (density > 0.0f && ChoosePattern()) {
        float spawn_time = TrafficDensitySpawnRates.GetValue(density);
        for (int i = 0; i < 10u; i++) {
            mPatternTimer[i] += dT * spawn_time;
        }
        mNewInstanceTimer += dT;
        SpawnTraffic();
    }

    for (std::list<IVehicle *>::const_iterator iter = mVehicles.begin(); iter != mVehicles.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->IsActive() && !ValidateVehicle(ivehicle, density)) {
            ivehicle->GetAIVehiclePtr()->UnSpawn();
        }
    }
}

// UNSOLVED virtual stuff
void AITrafficManager::FlushAllTraffic(bool release) {
    for (std::list<IVehicle *>::const_iterator iter = mVehicles.begin(); iter != mVehicles.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (release) {
            ISimable *isimable;
            if (ivehicle->QueryInterface(&isimable)) {
                isimable->Kill();
            }
        } else {
            if (ivehicle->IsActive()) {
                ivehicle->GetAIVehiclePtr()->UnSpawn();
            }
        }
    }

    if (release) {
        mVehicles.clear();
    }

    if (mPattern.IsValid()) {
        unsigned int num_types = mPattern.Num_Vehicles();
        for (unsigned int i = 0; i < num_types && i < 10; i++) {
            const TrafficPatternRecord &record = mPattern.Vehicles(i);
            mPatternTimer[i] = record.Rate * bRandom(1.0f);
        }
    }
}

bool AITrafficManager::OnTask(HSIMTASK htask, float dT) {
    ProfileNode profile_node("AITrafficManager::OnTask", 0);
    if (htask == this->mTask) {
        this->Update(dT);
        return true;
    }
    return false;
}

void AITrafficManager::OnDebugDraw() {}
