#include "Speed/Indep/Src/World/WPathFinder.h"

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WRoadElem.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

bool bPathFinderPrints = false;
SlotPool *AStarNodeSlotPool = nullptr;
SlotPool *AStarSearchSlotPool = nullptr;
float ASTAR_METRIC_SCALE = 0.25f;

// total size: 0x14
class AStarNode : public bTNode<AStarNode> {
  public:
    USE_SLOTALLOC(AStarNodeSlotPool);

    AStarNode() {}

    AStarNode(AStarNode *parent, const WRoadNode *road_node, int segment, float actual_cost, float estimated_cost)
        : nParentSlot((parent != nullptr) ? AStarNodeSlotPool->GetSlotNumber(parent) : -1), //
          nSegmentIndex(static_cast<short>(segment)),                                       //
          nRoadNode(road_node->fIndex),                                                     //
          fActualCost(static_cast<unsigned short>(actual_cost / ASTAR_METRIC_SCALE + 0.5f)),
          fEstimatedCost(static_cast<unsigned short>(estimated_cost / ASTAR_METRIC_SCALE + 0.5f)) {}

    AStarNode *GetParent() {
        return nParentSlot >= 0 ? static_cast<AStarNode *>(AStarNodeSlotPool->GetSlot(nParentSlot)) : nullptr;
    }

    const WRoadNode *GetRoadNode() {
        return WRoadNetwork::Get().GetNode(nRoadNode);
    }

    int GetSegmentIndex() {
        return nSegmentIndex;
    }

    float GetActualCost() {
        return static_cast<float>(fActualCost) * ASTAR_METRIC_SCALE;
    }

    float GetEstimatedCost() {
        return static_cast<float>(fEstimatedCost) * ASTAR_METRIC_SCALE;
    }

    float GetTotalCost() {
        return GetActualCost() + GetEstimatedCost();
    }

  private:
    short nParentSlot;             // offset 0x8, size 0x2
    short nSegmentIndex;           // offset 0xA, size 0x2
    short nRoadNode;               // offset 0xC, size 0x2
    unsigned short fActualCost;    // offset 0xE, size 0x2
    unsigned short fEstimatedCost; // offset 0x10, size 0x2
};

enum AStarSearchState {
    ASTAR_SEARCHING = 0,
    ASTAR_FULL_WAY = 1,
    ASTAR_HALF_WAY = 2,
    ASTAR_NO_WAY = 3,
};

// total size: 0x54
class AStarSearch : public bTNode<AStarSearch> {
  public:
    AStarSearch(WRoadNav *road_nav, const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction, const char *shortcut_allowed);
    virtual ~AStarSearch();

    void *operator new(size_t size) {
        return bMalloc(AStarSearchSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(AStarSearchSlotPool, ptr);
    }

    float Service(float time);

    bool IsActive() {
        return nState == ASTAR_SEARCHING;
    }

    bool IsFinished() {
        return nState > ASTAR_SEARCHING;
    }

    WRoadNav *GetRoadNav() {
        return pRoadNav;
    }

  private:
    bool IsGoal(AStarNode *node);
    AStarNode *FindOpenNode(const WRoadNode *road_node, int segment_number);
    AStarNode *FindClosedNode(const WRoadNode *road_node, int segment_number);
    static int AStarCheckFlip(AStarNode *before, AStarNode *after);
    bool Admissible(const WRoadSegment *segment, bool forward, WRoadNav::EPathType path_type);

    AStarSearchState nState;               // offset 0x8, size 0x4
    AStarNode *pSolution;                  // offset 0xC, size 0x4
    int nServices;                         // offset 0x10, size 0x4
    int nSteps;                            // offset 0x14, size 0x4
    float fSearchTime;                     // offset 0x18, size 0x4
    uint32 nShortcutCached;                // offset 0x1C, size 0x4
    uint32 nShortcutAllowed;               // offset 0x20, size 0x4
    const char *pShortcutAllowed;          // offset 0x24, size 0x4
    WRoadNav *pRoadNav;                    // offset 0x28, size 0x4
    const WRoadNode *pGoalNode;            // offset 0x2C, size 0x4
    int nGoalSegment;                      // offset 0x30, size 0x4
    ALIGN_16 UMath::Vector3 vGoalPosition; // offset 0x34, size 0xC
    bTList<AStarNode> lOpen;               // offset 0x40, size 0x8
    bTList<AStarNode> lClosed;             // offset 0x48, size 0x8
};

AStarSearch::AStarSearch(WRoadNav *road_nav, const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction,
                         const char *shortcut_allowed) {
    pSolution = nullptr;
    pRoadNav = road_nav;
    nState = ASTAR_SEARCHING;
    nServices = 0;
    nSteps = 0;
    fSearchTime = 0.0f;
    vGoalPosition = *goal_position;

    WRoadNav goal_nav;
    goal_nav.SetNavType(WRoadNav::kTypeDirection);
    goal_nav.SetPathType(road_nav->GetPathType());
    goal_nav.SetRaceFilter(road_nav->GetRaceFilter());
    goal_nav.SetTrafficFilter(road_nav->GetTrafficFilter());
    goal_nav.SetCopFilter(road_nav->GetCopFilter());
    goal_nav.SetDecisionFilter(road_nav->GetDecisionFilter());

    nShortcutCached = 0;
    nShortcutAllowed = 0;
    pShortcutAllowed = shortcut_allowed;

    unsigned char shortcut_number = road_nav->GetShortcutNumber();
    if (shortcut_number != 0xff) {
        int mask = 1 << shortcut_number;
        nShortcutCached |= mask;
        nShortcutAllowed |= mask;
        if (road_nav->GetPathType() == WRoadNav::kPathRaceRoute && (shortcut_allowed != nullptr) && !shortcut_allowed[shortcut_number]) {
            nState = ASTAR_NO_WAY;
            return;
        }
    }

    UMath::Vector3 fake = {1.0f, 0.0f, 0.0f};
    if (goal_direction != nullptr) {
        goal_nav.InitAtPoint(*goal_position, *goal_direction, false, 0.0f);
    } else {
        goal_nav.InitAtPoint(*goal_position, fake, false, 0.0f);
    }

    bool race_route = (road_nav->GetPathType() == WRoadNav::kPathRaceRoute);

    if (!goal_nav.IsValid()) {
        bVector2 goal_position_2d(goal_position->z, -goal_position->x);
        bVector2 goal_direction_2d((goal_direction != nullptr) ? goal_direction->z : 0.0f, (goal_direction != nullptr) ? -goal_direction->x : 0.0f);
        nState = ASTAR_NO_WAY;
        return;
    }
    WRoadNetwork &road_network = WRoadNetwork::Get();

    nGoalSegment = goal_nav.GetSegmentInd();
    road_nav->SetPathGoal(static_cast<unsigned short>(nGoalSegment), goal_nav.GetSegmentTime());

    if (road_nav->IsSegmentInCookieTrail(nGoalSegment, false)) {
        nState = ASTAR_FULL_WAY;
        return;
    }

    const WRoadSegment *goal_segment = road_network.GetSegment(nGoalSegment);
    if (goal_direction != nullptr) {
        pGoalNode = road_network.GetNode(goal_segment->fNodeIndex[goal_nav.GetNodeInd()]);
    } else {
        pGoalNode = nullptr;
    }

    int current_segment_index = road_nav->GetSegmentInd();
    const WRoadSegment *current_segment = road_network.GetSegment(current_segment_index);
    const WRoadNode *current_road_node = road_network.GetNode(current_segment->fNodeIndex[road_nav->GetNodeInd()]);

    if (bPathFinderPrints) {
        bVector2 goal_position_2d(goal_position->z, -goal_position->x);
        bVector2 current_position_2d(road_nav->GetPosition().z, -road_nav->GetPosition().x);
        bVector2 goal_direction_2d((goal_direction != nullptr) ? goal_direction->z : 0.0f, (goal_direction != nullptr) ? -goal_direction->x : 0.0f);
    }

    float estimated_cost = UMath::Distance(current_road_node->fPosition, vGoalPosition);
    lOpen.AddTail(new AStarNode(nullptr, current_road_node, current_segment_index, 0, estimated_cost));

    if (!race_route) {
        const WRoadNode *opp_road_node = road_network.GetSegmentOppNode(current_segment_index, current_road_node);
        estimated_cost = UMath::Distance(opp_road_node->fPosition, vGoalPosition);
        AStarNode *other_way_node = new AStarNode(nullptr, opp_road_node, current_segment_index, 0, estimated_cost);
        lOpen.AddSorted(&AStarCheckFlip, other_way_node);
    }
}

AStarSearch::~AStarSearch() {
    delete pSolution;
}

bool AStarSearch::IsGoal(AStarNode *node) {
    return nGoalSegment == node->GetSegmentIndex() && ((pGoalNode == nullptr) || pGoalNode == node->GetRoadNode());
}

AStarNode *AStarSearch::FindOpenNode(const WRoadNode *road_node, int segment_number) {
    for (AStarNode *node = lOpen.GetHead(); node != lOpen.EndOfList(); node = node->GetNext()) {
        if (road_node == node->GetRoadNode() && segment_number == node->GetSegmentIndex()) {
            return node;
        }
    }
    return nullptr;
}

AStarNode *AStarSearch::FindClosedNode(const WRoadNode *road_node, int segment_number) {
    for (AStarNode *node = lClosed.GetHead(); node != lClosed.EndOfList(); node = node->GetNext()) {
        if (road_node == node->GetRoadNode() && segment_number == node->GetSegmentIndex()) {
            return node;
        }
    }
    return nullptr;
}

int AStarSearch::AStarCheckFlip(AStarNode *before, AStarNode *after) {
    return static_cast<int>(before->GetTotalCost() <= after->GetTotalCost());
}

bool AStarSearch::Admissible(const WRoadSegment *segment, bool forward, WRoadNav::EPathType path_type) {
    WRoadNetwork &rn = WRoadNetwork::Get();

    switch (path_type) {
        case WRoadNav::kPathGPS:
            if ((segment->IsOneWay() && !forward)) {
                return false;
            }
            if (segment->CrossesBarrier() || segment->CrossesDriveThroughBarrier()) {
                return false;
            }
            break;
        case WRoadNav::kPathCop:
            if (segment->IsOneWay() && !forward) {
                return false;
            }
            break;
        case WRoadNav::kPathRacer: {
            if (segment->IsShortcut()) {
                int shortcut_number = rn.GetSegmentShortcutNumber(segment);
                bool shortcut_allowed = pRoadNav->MakeShortcutDecision(shortcut_number, &nShortcutCached, &nShortcutAllowed);
                if (!shortcut_allowed) {
                    return false;
                }
            }
            if (segment->IsOneWay() && !forward) {
                return false;
            }
            if (segment->CrossesBarrier() || segment->CrossesDriveThroughBarrier()) {
                return false;
            }
            break;
        }
        case WRoadNav::kPathPlayer: {
            if (segment->IsShortcut()) {
                int shortcut_number = rn.GetSegmentShortcutNumber(segment);
                if (shortcut_number != pRoadNav->GetShortcutNumber()) {
                    return false;
                }
            }
            if (segment->IsOneWay() && !forward) {
                return false;
            }
            if (segment->CrossesBarrier() || segment->CrossesDriveThroughBarrier()) {
                return false;
            }
            break;
        }
        case WRoadNav::kPathRaceRoute: {
            if (segment->IsShortcut()) {
                int shortcut_number = rn.GetSegmentShortcutNumber(segment);
                if (!pShortcutAllowed[shortcut_number]) {
                    return false;
                }
            }
            if (segment->CrossesBarrier() || segment->CrossesDriveThroughBarrier()) {
                return false;
            }
            if (segment->IsOneWay() && !forward) {
                return false;
            }
        }
        case WRoadNav::kPathChopper:
        default:
            break;
    }
    return true;
}

// UNSOLVED stack issue
float AStarSearch::Service(float time_limit_ms) {
    unsigned int start_ticker;
    WRoadNav::EPathType path_type;
    bool race_route;

    int prev_timeout_loops = 0x7FFFFFFF;
    nServices++;
    start_ticker = bGetTicker();
    path_type = pRoadNav->GetPathType();
    race_route = (path_type == WRoadNav::kPathRaceRoute);
    if (Joylog::IsReplaying()) {
        prev_timeout_loops = static_cast<int>(Joylog::GetData(32, JOYLOG_CHANNEL_PATHFINDER_TIMEOUT));
    }
    int num_loops = 0;
    while (IsActive() && !lOpen.IsEmpty()) {
        if (Joylog::IsReplaying()) {
            if (num_loops == prev_timeout_loops) {
                break;
            }
        } else {
            if (bGetTickerDifference(start_ticker) > time_limit_ms) {
                break;
            }
        }
        num_loops++;
        nSteps++;
        AStarNode *current_node = lOpen.RemoveTail();
        if (IsGoal(current_node)) {
            pSolution = current_node;
            nState = ASTAR_FULL_WAY;
            break;
        }
        WRoadNetwork &road_network = WRoadNetwork::Get();
        const WRoadNode *current_road_node = current_node->GetRoadNode();
        int num_segments = static_cast<int>(current_road_node->fNumSegments);
        if (bCountFreeSlots(AStarNodeSlotPool) < static_cast<int>(num_segments - 1)) {
            if (race_route) {
                nState = ASTAR_NO_WAY;
            } else {
                nState = ASTAR_HALF_WAY;
            }
            if (race_route) {
                pSolution = nullptr;
            } else {
                pSolution = current_node;
            }
            break;
        }
        for (int i = 0; i < num_segments; i++) {
            int segment_index = static_cast<int>(current_road_node->fSegmentIndex[i]);
            int current_segment_index = current_node->GetSegmentIndex();
            if (segment_index == current_segment_index) {
                continue;
            }
            const WRoadSegment *segment = road_network.GetSegment(segment_index);
            const WRoadSegment *current_segment = road_network.GetSegment(current_segment_index);
            if (current_segment->IsDecision() && segment->IsDecision() && path_type != WRoadNav::kPathCop) {
                continue;
            }
            if (!Admissible(segment, current_road_node->fIndex == segment->fNodeIndex[0], path_type)) {
                continue;
            }
            float actual_cost = current_node->GetActualCost() + segment->GetLength();
            const WRoadNode *next_road_node = road_network.GetSegmentOppNode(segment_index, current_road_node);
            AStarNode *already_open = FindOpenNode(next_road_node, segment_index);
            if ((already_open != nullptr) && already_open->GetActualCost() <= actual_cost) {
                continue;
            }
            AStarNode *already_closed = FindClosedNode(next_road_node, segment_index);
            if ((already_closed != nullptr) && already_closed->GetActualCost() <= actual_cost) {
                continue;
            }
            if (already_open != nullptr) {
                delete already_open->Remove();
            }
            if (already_closed != nullptr) {
                delete already_closed->Remove();
            }
            float estimated_cost = UMath::Distance(next_road_node->fPosition, vGoalPosition);
            AStarNode *new_node = new AStarNode(current_node, next_road_node, segment_index, actual_cost, estimated_cost);
            lOpen.AddSorted(AStarCheckFlip, new_node);
        }
        lClosed.AddHead(current_node);
    }
    if (IsActive() && lOpen.IsEmpty()) {
        pSolution = nullptr;
        nState = ASTAR_NO_WAY;
    }
    if (IsFinished()) {
        AStarNode *node = pSolution;
        int num_segments = 0;
        int max_segments = pRoadNav->GetMaxPathSegments();
        while (node != nullptr) {
            num_segments++;
            node = node->GetParent();
        }
        node = pSolution;
        while ((node != nullptr) && num_segments > max_segments) {
            num_segments--;
            node = node->GetParent();
        }
        bool race_route_bogus = race_route;
        bool segment_found;
        if (num_segments > 0) {
            pRoadNav->SetNavType(WRoadNav::kTypePath);
            race_route_bogus = false;
            if (static_cast<int>(pRoadNav->GetSegmentInd()) == nGoalSegment) {
                UMath::Vector3 dir;
                UMath::Sub(vGoalPosition, pRoadNav->GetPosition(), dir);
                float dot = UMath::Dot(dir, pRoadNav->GetForwardVector());
                if (dot < 0.0f) {
                    pRoadNav->Reverse();
                    race_route_bogus = race_route;
                }
            }
        }
        pRoadNav->SetNumPathSegments(num_segments);
        unsigned short *segments = pRoadNav->GetPathSegments();
        if ((node != nullptr) && num_segments > 0) {
            do {
                int segment_index = node->GetSegmentIndex();
                num_segments--;
                segments[num_segments] = static_cast<unsigned short>(segment_index);
                if (segment_index == pRoadNav->GetSegmentInd()) {
                    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
                    const WRoadSegment *segment = roadNetwork.GetSegment(segment_index);
                    const WRoadNode *currentnode = roadNetwork.GetNode(segment->fNodeIndex[static_cast<int>(pRoadNav->GetNodeInd())]);
                    if (node->GetRoadNode() != currentnode) {
                        pRoadNav->Reverse();
                        race_route_bogus = race_route;
                    }
                }
            } while (((node = node->GetParent()) != nullptr) && num_segments > 0); // this weird to force the inline call outside the scope
        }
        num_segments = static_cast<unsigned int>(pRoadNav->GetNumPathSegments());
        if (pRoadNav->GetPathType() == WRoadNav::kPathGPS && nState != ASTAR_FULL_WAY) {
            pRoadNav->SetNumPathSegments(0);
        }
        if (race_route) {
            typedef UTL::Std::set<unsigned short, _type_set> SEGMENT_SET;
            SEGMENT_SET segment_set;
            for (int i = 0; i < static_cast<int>(num_segments); i++) {
                segment_set.insert(pRoadNav->GetPathSegment(i));
            }
            if (segment_set.size() < num_segments) {
                race_route_bogus = true;
            }
        }
        if (race_route_bogus) {
            bVector2 goal_position_2d(vGoalPosition.z, -vGoalPosition.x);
            bVector2 current_position_2d(pRoadNav->GetPosition().z, -pRoadNav->GetPosition().x);
            pRoadNav->SetNavType(WRoadNav::kTypeDirection);
            pRoadNav->SetNumPathSegments(0);
            nState = ASTAR_NO_WAY;
            pSolution = nullptr;
        }
        if (bPathFinderPrints) {
            float search_time = bGetTickerDifference(start_ticker);
        }
    }
    float elapsed_ms = bGetTickerDifference(start_ticker);
    if (Joylog::IsCapturing()) {
        Joylog::AddData(num_loops, 32, JOYLOG_CHANNEL_PATHFINDER_TIMEOUT);
    }
    if (Joylog::IsCapturing()) {
        // TODO
        Joylog::AddData(reinterpret_cast<int32 &>(elapsed_ms), 32, JOYLOG_CHANNEL_PATHFINDER_TIMEOUT);
    } else if (Joylog::IsReplaying()) {
        // TODO
        int fake = Joylog::GetData(32, JOYLOG_CHANNEL_PATHFINDER_TIMEOUT);
        elapsed_ms = reinterpret_cast<float &>(fake);
    }
    fSearchTime += elapsed_ms;
    return elapsed_ms;
}

BIND_ACTIVITY_FACTORY(PathFinder);

PathFinder *PathFinder::pInstance = nullptr;

PathFinder::PathFinder() : Activity(0) {
    AStarNodeSlotPool = bNewSlotPool(sizeof(AStarNode), 3072, "AStarNodeSlotPool", 0);
    AStarSearchSlotPool = bNewSlotPool(sizeof(AStarSearch), 16, "AStarSearchSlotPool", 0);
    mSimTask = AddTask("AIVehicle", 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE);
    Sim::ProfileTask(mSimTask, "PathFinder");
}

PathFinder::~PathFinder() {
    lSearches.DeleteAllElements();
    bDeleteSlotPool(AStarSearchSlotPool);
    bDeleteSlotPool(AStarNodeSlotPool);
    AStarSearchSlotPool = nullptr;
    AStarNodeSlotPool = nullptr;
    RemoveTask(mSimTask);
    Set(nullptr);
}

Sim::IActivity *PathFinder::Construct(Sim::Param params) {
    PathFinder *manager = Get();
    if (manager == nullptr) {
        manager = new PathFinder();
        Set(manager);
    }
    return manager;
}

void PathFinder::Service(float time_limit_ms) {
    float elapsed_ms = 0.0f;
    while (!lSearches.IsEmpty() && elapsed_ms < time_limit_ms) {
        AStarSearch *search = lSearches.GetHead();
        elapsed_ms += search->Service(time_limit_ms - elapsed_ms);
        if (search->IsFinished()) {
            delete search->Remove();
        }
    }
}

void PathFinder::ServiceAll() {
    while (!lSearches.IsEmpty()) {
        Service(1000.0f);
    }
}

bool PathFinder::OnTask(HSIMTASK htask, float elapsed_seconds) {
    ProfileNode profile_node("TODO", 0);
    if (htask != mSimTask) {
        Object::OnTask(htask, elapsed_seconds);
        return false;
    }
    Service(1.0f);
    return true;
}

void PathFinder::Cancel(WRoadNav *road_nav) {
    AStarSearch *search = lSearches.GetHead();
    while (search != lSearches.EndOfList()) {
        AStarSearch *next_search = search->GetNext();
        if (road_nav == search->GetRoadNav()) {
            delete search->Remove();
        }
        search = next_search;
    }
}

AStarSearch *PathFinder::Pending(WRoadNav *road_nav) {
    for (AStarSearch *search = lSearches.GetHead(); search != lSearches.EndOfList(); search = search->GetNext()) {
        if (road_nav == search->GetRoadNav()) {
            return search;
        }
    }
    return nullptr;
}

AStarSearch *PathFinder::Submit(WRoadNav *road_nav, const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction,
                                const char *shortcut_allowed) {
    Cancel(road_nav);
    AStarSearch *ret = nullptr;
    if (!bIsSlotPoolFull(AStarSearchSlotPool) && bCountFreeSlots(AStarNodeSlotPool) > 1) {
        ret = new AStarSearch(road_nav, goal_position, goal_direction, shortcut_allowed);
        lSearches.AddTail(ret);
    }
    return ret;
}
