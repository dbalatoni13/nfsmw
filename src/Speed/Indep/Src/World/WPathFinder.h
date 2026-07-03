#ifndef _WPATHFINDER
#define _WPATHFINDER

#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/World/WPathFinder.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern float ASTAR_METRIC_SCALE;
extern SlotPool *AStarSearchSlotPool;
extern SlotPool *AStarNodeSlotPool;

class AStarSearch;

class PathFinder : public Sim::Activity {
  public:
    PathFinder();
    ~PathFinder() override;
    static Sim::IActivity *Construct(Sim::Param params);
    bool OnTask(HSIMTASK htask, float elapsed_seconds) override;

    static PathFinder *Get() {
        return pInstance;
    }

    static void Set(PathFinder *instance) {
        pInstance = instance;
    }

    void Cancel(WRoadNav *road_nav);
    void Service(float time_limit_ms);
    void ServiceAll();
    AStarSearch *Pending(WRoadNav *road_nav);
    AStarSearch *Submit(WRoadNav *road_nav, const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction, const char *shortcut_allowed);

  private:
    HSIMTASK mSimTask; // offset 0x4C, size 0x4
    static PathFinder *pInstance;
    bTList<AStarSearch> lSearches; // offset 0x50, size 0x8
};

#endif
