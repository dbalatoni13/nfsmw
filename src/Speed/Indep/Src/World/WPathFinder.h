#ifndef _WPATHFINDER
#define _WPATHFINDER

#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct HSIMTASK__;

extern float ASTAR_METRIC_SCALE;

struct AStarNode : public bTNode<AStarNode> {
    static void *operator new(unsigned int size);
    static void operator delete(void *ptr);

    const WRoadNode *GetRoadNode() { return WRoadNetwork::Get().GetNode(nRoadNode); }
    int GetSegmentIndex() { return nSegmentIndex; }
    float GetActualCost() { return static_cast<float>(fActualCost) * ASTAR_METRIC_SCALE; }
    float GetEstimatedCost() { return static_cast<float>(fEstimatedCost) * ASTAR_METRIC_SCALE; }
    float GetTotalCost() { return GetActualCost() + GetEstimatedCost(); }

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
    bool IsGoal(AStarNode *node);
    AStarNode *FindOpenNode(const WRoadNode *road_node, int segment_number);
    AStarNode *FindClosedNode(const WRoadNode *road_node, int segment_number);
    static int AStarCheckFlip(AStarNode *before, AStarNode *after);
    bool Admissible(const WRoadSegment *segment, bool forward, WRoadNav::EPathType path_type);
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
