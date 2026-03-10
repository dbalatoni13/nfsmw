#ifndef _WPATHFINDER
#define _WPATHFINDER

#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct HSIMTASK__;
class WRoadNav;
struct WRoadNode;

struct AStarNode : public bTNode<AStarNode> {
    static void *operator new(unsigned int size);
    static void operator delete(void *ptr);

    short nParentSlot;
    short nSegmentIndex;
    short nRoadNode;
    unsigned short fActualCost;
    unsigned short fEstimatedCost;
};

enum AStarSearchState {};

extern SlotPool *AStarSearchSlotPool;
extern SlotPool *AStarNodeSlotPool;

struct AStarSearch : public bTNode<AStarSearch> {
    static void *operator new(unsigned int size) { return bMalloc(AStarSearchSlotPool); }
    static void operator delete(void *ptr) { bFree(AStarSearchSlotPool, ptr); }

    AStarSearch(WRoadNav *road_nav, const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction, const char *shortcut_allowed);
    virtual ~AStarSearch();
    float Service(float time);
    bool IsFinished() { return nState > 0; }
    WRoadNav *GetRoadNav() { return pRoadNav; }

    AStarSearchState nState;
    AStarNode *pSolution;
    int nServices;
    int nSteps;
    float fSearchTime;
    unsigned int nShortcutCached;
    unsigned int nShortcutAllowed;
    const char *pShortcutAllowed;
    WRoadNav *pRoadNav;
    const WRoadNode *pGoalNode;
    int nGoalSegment;
    UMath::Vector3 vGoalPosition;
    bTList<AStarNode> lOpen;
    bTList<AStarNode> lClosed;
};

class PathFinder : public Sim::Activity {
  public:
    PathFinder();
    ~PathFinder() override;

    static PathFinder *Get() {
        return pInstance;
    }

    static void Set(PathFinder *instance) {
        pInstance = instance;
    }

    static Sim::IActivity *Construct(Sim::Param params);
    void Service(float time_limit_ms);
    void ServiceAll();
    bool OnTask(HSIMTASK__ *htask, float elapsed_seconds) override;
    void Cancel(WRoadNav *road_nav);
    AStarSearch *Pending(WRoadNav *road_nav);
    AStarSearch *Submit(WRoadNav *road_nav, const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction, const char *shortcut_allowed);

  private:
    static PathFinder *pInstance;

    HSIMTASK__ *mSimTask;
    bTList<AStarSearch> lSearches;
};

#endif
