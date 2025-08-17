#include "Speed/Indep/Src/AI/activities/AITrafficManager.hpp"

// TODO move
extern BOOL SkipFETrafficDensity; // size: 0x4
extern BOOL SkipFEDisableTraffic; // size: 0x4
extern BOOL SkipFE;               // size: 0x4

// AITrafficManager::AITrafficManager(Sim::Param params) {}

Sim::IActivity *AITrafficManager::Construct(Sim::Param params) {
    if (SkipFE && SkipFEDisableTraffic) {
        return nullptr;
    }
    return new AITrafficManager(Sim::Param(params));
}

eVehicleCacheResult AITrafficManager::OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const {}

void AITrafficManager::OnRemovedVehicleCache(IVehicle *ivehicle) {}

void AITrafficManager::OnAttached(IAttachable *pOther) {}
