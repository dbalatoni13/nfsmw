#ifndef AI_ACTIVITIES_AI_TRAFFIC_MANAGER_H
#define AI_ACTIVITIES_AI_TRAFFIC_MANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/trafficpattern.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/World/WRoadNetwork.hpp"

#include "algorithm"

enum eTrafficDensity {
    NUM_TRAFFIC_DENSITIES = 4,
    eTRAFFICDENSITY_HIGH = 3,
    eTRAFFICDENSITY_MEDIUM = 2,
    eTRAFFICDENSITY_LOW = 1,
    eTRAFFICDENSITY_OFF = 0,
};

struct _type_TrafficList {
    const char *name() {
        return "TrafficList";
    }
};

struct _type_AITrafficManager_PatternMap {
    const char *name() {
        return "AITrafficManager::PatternMap";
    }
};

// total size: 0x3C4
class AITrafficManager : public Sim::Activity, public ITrafficMgr, public IVehicleCache, public Debugable {
  public:
    struct PatternKey {
        int BHash;
        Attrib::Key CollectionKey;

        bool operator<(const PatternKey &rhs) const {
            return BHash < rhs.BHash;
        }
    };

    struct PatternMap : public UTL::Std::vector<PatternKey, _type_AITrafficManager_PatternMap> {

        Attrib::Key Find(int bhash) const {
            PatternKey key = {bhash, 0};
            UTL::Std::vector<PatternKey, _type_AITrafficManager_PatternMap>::const_iterator iter = std::lower_bound(begin(), end(), key);
            if (iter != end() && iter->BHash == bhash) {
                return iter->CollectionKey;
            }
            return 0;
        }
    };

    static Sim::IActivity *Construct(Sim::Param params);

    AITrafficManager(Sim::Param params);
    virtual eVehicleCacheResult OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const;
    virtual void OnRemovedVehicleCache(IVehicle *ivehicle);
    virtual void OnAttached(IAttachable *pOther);
    virtual void OnDetached(IAttachable *pOther);
    unsigned int NextSpawn();
    IVehicle *GetAvailableTrafficVehicle(Attrib::Key key, bool makenew);
    bool SpawnTraffic();
    bool NeedsTraffic() const;
    void UpdateDebug();
    void SetTrafficPattern(unsigned int pattern_key);
    bool FindCollisions(const UMath::Vector3 &spawnpoint) const;
    bool CheckRace(const WRoadNav &nav) const;
    bool FindSpawnPoint(WRoadNav &nav) const;
    bool ChoosePattern();
    bool ValidateVehicle(IVehicle *ivehicle, float density) const;
    float ComputeDensity() const;
    void Update(float dT);

    // Overrides
    // ITrafficMgr
    virtual void FlushAllTraffic(bool release);

    // ITaskable
    virtual bool OnTask(HSIMTASK htask, float dT);

    // Virtual methods
    virtual void OnDebugDraw();

  private:
    HSIMTASK mTask;                                          // offset 0x68, size 0x4
    unsigned int mSpawnIdx;                                  // offset 0x6C, size 0x4
    float mPatternTimer[10];                                 // offset 0x70, size 0x28
    float mNewInstanceTimer;                                 // offset 0x98, size 0x4
    UTL::Std::list<IVehicle *, _type_TrafficList> mVehicles; // offset 0x9C, size 0x8
    ActionQueue *mActionQ;                                   // offset 0xA4, size 0x4
    eTrafficDensity mDensity;                                // offset 0xA8, size 0x4
    PatternMap mPatternMap;                                  // offset 0xAC, size 0x10
    WRoadNav mNav;                                           // offset 0xBC, size 0x2F0
    float mOncommingChance;                                  // offset 0x3AC, size 0x4
    Attrib::Gen::trafficpattern mPattern;                    // offset 0x3B0, size 0x14
};

#endif
