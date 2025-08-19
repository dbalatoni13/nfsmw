#include "Speed/Indep/Src/AI/activities/AITrafficManager.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/trafficpattern.h"
#include "Speed/Indep/Src/Input/ActionRef.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficCenter.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <algorithm>

// TODO move
extern BOOL SkipFETrafficDensity; // size: 0x4
extern BOOL SkipFEDisableTraffic; // size: 0x4
extern BOOL SkipFE;               // size: 0x4

using namespace Attrib::Gen;
using UTL::Collections::Listable;
using UTL::Collections::ListableSet;

typedef ListableSet<IVehicle, 10, eVehicleList, VEHICLE_MAX> VehicleSet;

// AITrafficManager::AITrafficManager(Sim::Param params) {}

Sim::IActivity *AITrafficManager::Construct(Sim::Param params) {
    if (SkipFE && SkipFEDisableTraffic) {
        return nullptr;
    }
    return new AITrafficManager(Sim::Param(params));
}

eVehicleCacheResult AITrafficManager::OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const {}

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

    for (Listable<ITrafficCenter, 8>::const_iterator iter = ITrafficCenter::GetList().begin(); iter != ITrafficCenter::GetList().end(); iter++) {
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
    const VehicleSet::List &vehicles = IVehicle::GetList(VEHICLE_ALL);
    for (VehicleSet::const_iterator iter = vehicles.begin(); iter != vehicles.end(); iter++) {
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

bool AITrafficManager::OnTask(HSIMTASK htask, float dT) {
    ProfileNode profile_node("AITrafficManager::OnTask", 0);
    if (htask == this->mTask) {
        this->Update(dT);
        return true;
    }
    return false;
}

void AITrafficManager::OnDebugDraw() {}
