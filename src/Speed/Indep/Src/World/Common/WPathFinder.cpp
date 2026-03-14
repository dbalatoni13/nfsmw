#include "Speed/Indep/Src/World/WPathFinder.h"

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

extern int bPathFinderPrints;

UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::Prototype _PathFinder("PathFinder", PathFinder::Construct);

PathFinder* PathFinder::pInstance;

PathFinder::PathFinder()
    : Activity(0) {
    AStarNodeSlotPool = bNewSlotPool(0x14, 0xC00, "AStarNodeSlotPool", 0);
    AStarSearchSlotPool = bNewSlotPool(0x54, 0x10, "AStarSearchSlotPool", 0);
    mSimTask = AddTask(UCrc32("AIVehicle"), 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE);
    Sim::ProfileTask(mSimTask, "PathFinder");
}

PathFinder::~PathFinder() {
    lSearches.DeleteAllElements();
    bDeleteSlotPool(AStarSearchSlotPool);
    bDeleteSlotPool(AStarNodeSlotPool);
    AStarSearchSlotPool = nullptr;
    AStarNodeSlotPool = nullptr;
    RemoveTask(mSimTask);
    pInstance = nullptr;
}

Sim::IActivity* PathFinder::Construct(Sim::Param params) {
    PathFinder *manager = pInstance;
    if (manager == nullptr) {
        manager = new PathFinder();
        pInstance = manager;
    }
    return manager;
}

void PathFinder::ServiceAll() {
    while (!lSearches.IsEmpty()) {
        Service(1000.0f);
    }
}

bool PathFinder::OnTask(HSIMTASK__* htask, float elapsed_seconds) {
    if (htask != mSimTask) {
        return false;
    }
    Service(1.0f);
    return true;
}

AStarSearch* PathFinder::Pending(WRoadNav* road_nav) {
    for (AStarSearch* s = lSearches.GetHead(); s != lSearches.EndOfList(); s = s->GetNext()) {
        if (road_nav == s->pRoadNav) {
            return s;
        }
    }
    return nullptr;
}

void PathFinder::Service(float time_limit_ms) {
    float elapsed_ms = 0.0f;
    while (!lSearches.IsEmpty() && elapsed_ms < time_limit_ms) {
        AStarSearch *search = lSearches.GetHead();
        elapsed_ms += search->Service(time_limit_ms - elapsed_ms);
        if (search->IsFinished()) {
            search->Remove();
            delete search;
        }
    }
}

void PathFinder::Cancel(WRoadNav *road_nav) {
    AStarSearch *search = lSearches.GetHead();
    while (search != lSearches.EndOfList()) {
        AStarSearch *next_search = search->GetNext();
        if (road_nav == search->GetRoadNav()) {
            search->Remove();
            delete search;
        }
        search = next_search;
    }
}

AStarSearch *PathFinder::Submit(WRoadNav *road_nav, const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction, const char *shortcut_allowed) {
    Cancel(road_nav);
    AStarSearch *ret = nullptr;
    if (!bIsSlotPoolFull(AStarSearchSlotPool) && bCountFreeSlots(AStarNodeSlotPool) > 1) {
        ret = new AStarSearch(road_nav, goal_position, goal_direction, shortcut_allowed);
        lSearches.AddTail(ret);
    }
    return ret;
}

bool AStarSearch::IsGoal(AStarNode *node) {
    bool result = false;
    if (nGoalSegment == node->GetSegmentIndex() && (pGoalNode == nullptr || pGoalNode == node->GetRoadNode())) {
        result = true;
    }
    return result;
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

AStarSearch::AStarSearch(WRoadNav *road_nav, const UMath::Vector3 *goal_position,
                         const UMath::Vector3 *goal_direction, const char *shortcut_allowed)
    : nState(static_cast<AStarSearchState>(0)), //
      pSolution(nullptr),                       //
      nServices(0),                             //
      nSteps(0),                                //
      fSearchTime(0.0f),                        //
      pRoadNav(road_nav) {
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
        unsigned int mask = 1 << shortcut_number;
        nShortcutCached |= mask;
        nShortcutAllowed |= mask;
        if (road_nav->GetPathType() == WRoadNav::kPathRaceRoute
            && shortcut_allowed != nullptr
            && shortcut_allowed[shortcut_number] == '\0') {
            nState = static_cast<AStarSearchState>(3);
            return;
        }
    }

    UMath::Vector3 fake = UMath::Vector3::kZero;
    if (goal_direction != nullptr) {
        goal_nav.InitAtPoint(*goal_position, *goal_direction, false, 0.0f);
    } else {
        goal_nav.InitAtPoint(*goal_position, fake, false, 0.0f);
    }

    WRoadNetwork &road_network = WRoadNetwork::Get();
    bool race_route = (road_nav->GetPathType() == WRoadNav::kPathRaceRoute);

    if (!goal_nav.IsValid()) {
        bVector2 goal_position_2d(goal_position->z, -goal_position->x);
        bVector2 goal_direction_2d(0.0f, 0.0f);
        if (goal_direction != nullptr) {
            goal_direction_2d.x = goal_direction->z;
            goal_direction_2d.y = -goal_direction->x;
        }
        nState = static_cast<AStarSearchState>(3);
        return;
    }

    nGoalSegment = static_cast<int>(goal_nav.GetSegmentInd());
    road_nav->SetPathGoal(static_cast<unsigned short>(nGoalSegment), goal_nav.GetSegmentTime());

    if (road_nav->IsSegmentInCookieTrail(nGoalSegment, false)) {
        nState = static_cast<AStarSearchState>(1);
        return;
    }

    if (goal_direction == nullptr) {
        pGoalNode = nullptr;
    } else {
        const WRoadSegment *goal_segment = road_network.GetSegment(nGoalSegment);
        pGoalNode = road_network.GetNode(goal_segment->fNodeIndex[static_cast<int>(goal_nav.GetNodeInd())]);
    }

    int current_segment_index = static_cast<int>(road_nav->GetSegmentInd());
    const WRoadSegment *current_segment = road_network.GetSegment(current_segment_index);
    const WRoadNode *current_road_node = road_network.GetNode(
        current_segment->fNodeIndex[static_cast<int>(road_nav->GetNodeInd())]);

    if (bPathFinderPrints) {
        bVector2 goal_position_2d(goal_position->z, -goal_position->x);
        bVector2 current_position_2d(road_nav->GetPosition().z, -road_nav->GetPosition().x);
        bVector2 goal_direction_2d(0.0f, 0.0f);
        if (goal_direction != nullptr) {
            goal_direction_2d = bVector2(goal_direction->z, -goal_direction->x);
        }
    }

    float estimated_cost = UMath::Distance(current_road_node->fPosition, vGoalPosition);
    AStarNode *start_node = static_cast<AStarNode *>(bMalloc(AStarNodeSlotPool));
    start_node->nParentSlot = -1;
    start_node->nSegmentIndex = static_cast<short>(current_segment_index);
    start_node->nRoadNode = current_road_node->fIndex;
    start_node->fActualCost = 0;
    start_node->fEstimatedCost = static_cast<unsigned short>(static_cast<int>(estimated_cost / ASTAR_METRIC_SCALE + 0.5f));
    lOpen.AddTail(start_node);

    if (!race_route) {
        const WRoadNode *opp_road_node = road_network.GetSegmentOppNode(current_segment_index, current_road_node);
        float opp_estimated_cost = UMath::Distance(opp_road_node->fPosition, vGoalPosition);
        AStarNode *other_way_node = static_cast<AStarNode *>(bMalloc(AStarNodeSlotPool));
        other_way_node->nParentSlot = -1;
        other_way_node->nSegmentIndex = static_cast<short>(current_segment_index);
        other_way_node->nRoadNode = opp_road_node->fIndex;
        other_way_node->fActualCost = 0;
        other_way_node->fEstimatedCost = static_cast<unsigned short>(static_cast<int>(opp_estimated_cost / ASTAR_METRIC_SCALE + 0.5f));
        lOpen.AddSorted(AStarCheckFlip, other_way_node);
    }
}

AStarSearch::~AStarSearch() {
    delete pSolution;
}

bool AStarSearch::Admissible(const WRoadSegment *segment, bool forward, WRoadNav::EPathType path_type) {
    WRoadNetwork &rn = WRoadNetwork::Get();

    switch (path_type) {
    case WRoadNav::kPathGPS:
        if (segment->IsOneWay() && !forward)
            return false;
        if (segment->CrossesBarrier() || segment->CrossesDriveThroughBarrier())
            return false;
        return true;
    case WRoadNav::kPathCop:
        if (segment->IsOneWay() && !forward)
            return false;
        return true;
    case WRoadNav::kPathRacer: {
        if (segment->IsShortcut()) {
            int shortcut_number = rn.GetSegmentShortcutNumber(segment);
            bool shortcut_allowed = pRoadNav->MakeShortcutDecision(shortcut_number, &nShortcutCached, &nShortcutAllowed);
            if (!shortcut_allowed)
                return false;
        }
        if (segment->IsOneWay() && !forward)
            return false;
        if (segment->CrossesBarrier() || segment->CrossesDriveThroughBarrier())
            return false;
        return true;
    }
    case WRoadNav::kPathPlayer: {
        if (segment->IsShortcut()) {
            int shortcut_number = rn.GetSegmentShortcutNumber(segment);
            if (shortcut_number != pRoadNav->GetShortcutNumber())
                return false;
        }
        if (segment->IsOneWay() && !forward)
            return false;
        if (segment->CrossesBarrier() || segment->CrossesDriveThroughBarrier())
            return false;
        return true;
    }
    case WRoadNav::kPathRaceRoute: {
        if (segment->IsShortcut()) {
            int shortcut_number = rn.GetSegmentShortcutNumber(segment);
            if (pShortcutAllowed[shortcut_number] == 0)
                return false;
        }
        if (segment->CrossesBarrier() || segment->CrossesDriveThroughBarrier())
            return false;
        if (segment->IsOneWay() && !forward)
            return false;
        return true;
    }
    default:
        return true;
    }
}

float AStarSearch::Service(float time_limit_ms) {
    unsigned int start_ticker = bGetTicker();
    WRoadNav::EPathType path_type = pRoadNav->GetPathType();
    bool race_route = (path_type == WRoadNav::kPathRaceRoute);
    int prev_timeout_loops = 0x7FFFFFFF;
    nServices++;
    if (Joylog::IsReplaying()) {
        prev_timeout_loops = static_cast<int>(Joylog::GetData(32, JOYLOG_CHANNEL_PATHFINDER_TIMEOUT));
    }
    int num_loops = 0;
    while (IsActive()) {
        if (lOpen.IsEmpty()) {
            if (IsActive() && lOpen.IsEmpty()) {
                pSolution = nullptr;
                nState = static_cast<AStarSearchState>(3);
            }
            break;
        }
        if (Joylog::IsReplaying()) {
            if (num_loops == prev_timeout_loops) {
                break;
            }
        } else {
            float elapsed_ms = bGetTickerDifference(start_ticker);
            if (elapsed_ms > time_limit_ms) {
                break;
            }
        }
        AStarNode *current_node = lOpen.RemoveTail();
        num_loops++;
        nSteps++;
        if (IsGoal(current_node)) {
            pSolution = current_node;
            nState = static_cast<AStarSearchState>(1);
            break;
        }
        WRoadNetwork &road_network = WRoadNetwork::Get();
        const WRoadNode *current_road_node = current_node->GetRoadNode();
        int num_segments = static_cast<int>(current_road_node->fNumSegments);
        if (bCountFreeSlots(AStarNodeSlotPool) < static_cast<int>(num_segments - 1)) {
            if (race_route) {
                nState = static_cast<AStarSearchState>(2);
            } else {
                nState = static_cast<AStarSearchState>(3);
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
            bool forward = (current_road_node->fIndex == segment->fNodeIndex[0]);
            if (!Admissible(segment, forward, path_type)) {
                continue;
            }
            float actual_cost = current_node->GetActualCost() + segment->GetLength();
            const WRoadNode *next_road_node = road_network.GetSegmentOppNode(segment_index, current_road_node);
            AStarNode *already_open = FindOpenNode(next_road_node, segment_index);
            if (already_open != nullptr && actual_cost >= already_open->GetActualCost()) {
                continue;
            }
            AStarNode *already_closed = FindClosedNode(next_road_node, segment_index);
            if (already_closed != nullptr && actual_cost >= already_closed->GetActualCost()) {
                continue;
            }
            if (already_open != nullptr) {
                already_open->Remove();
                delete already_open;
            }
            if (already_closed != nullptr) {
                already_closed->Remove();
                delete already_closed;
            }
            float estimated_cost = UMath::Distance(next_road_node->fPosition, vGoalPosition);
            AStarNode *new_node = new AStarNode(current_node, next_road_node, segment_index, actual_cost, estimated_cost);
            lOpen.AddSorted(AStarCheckFlip, new_node);
        }
        lClosed.AddHead(current_node);
    }
    if (nState > 0) {
        AStarNode *node = pSolution;
        int num_segments = 0;
        while (node != nullptr) {
            num_segments++;
            node = node->GetParent();
        }
        node = pSolution;
        int max_segments = pRoadNav->GetMaxPathSegments();
        while (node != nullptr && num_segments > max_segments) {
            num_segments--;
            node = node->GetParent();
        }
        bool race_route_bogus = race_route;
        bool segment_found = num_segments > 0;
        if (segment_found) {
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
        if (node != nullptr) {
            while (segment_found) {
                int segment_index = node->GetSegmentIndex();
                num_segments--;
                segments[num_segments] = static_cast<unsigned short>(segment_index);
                if (segment_index == static_cast<int>(pRoadNav->GetSegmentInd())) {
                    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
                    const WRoadSegment *segment = roadNetwork.GetSegment(segment_index);
                    const WRoadNode *currentnode = roadNetwork.GetNode(segment->fNodeIndex[static_cast<int>(pRoadNav->GetNodeInd())]);
                    if (currentnode != node->GetRoadNode()) {
                        pRoadNav->Reverse();
                        race_route_bogus = race_route;
                    }
                }
                node = node->GetParent();
                if (node == nullptr) {
                    break;
                }
                segment_found = num_segments > 0;
            }
        }
        unsigned int path_segment_count = static_cast<unsigned int>(pRoadNav->GetNumPathSegments());
        if (pRoadNav->GetPathType() == WRoadNav::kPathRaceRoute && nState != static_cast<AStarSearchState>(1)) {
            pRoadNav->SetNumPathSegments(0);
        }
        if (race_route) {
            typedef UTL::Std::set<unsigned short, _type_set> SEGMENT_SET;
            SEGMENT_SET segment_set;
            for (int i = 0; i < static_cast<int>(path_segment_count); i++) {
                segment_set.insert(pRoadNav->GetPathSegment(i));
            }
            if (segment_set.size() < path_segment_count) {
                race_route_bogus = true;
            }
        }
        if (race_route_bogus) {
            bVector2 goal_position_2d(vGoalPosition.z, -vGoalPosition.x);
            bVector2 current_position_2d(pRoadNav->GetPosition().z, -pRoadNav->GetPosition().x);
            pRoadNav->SetNavType(WRoadNav::kTypeDirection);
            pRoadNav->SetNumPathSegments(0);
            nState = static_cast<AStarSearchState>(3);
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
        Joylog::AddData(static_cast<int>(elapsed_ms), 32, JOYLOG_CHANNEL_PATHFINDER_TIMEOUT);
    } else if (Joylog::IsReplaying()) {
        elapsed_ms = static_cast<float>(static_cast<int>(Joylog::GetData(32, JOYLOG_CHANNEL_PATHFINDER_TIMEOUT)));
    }
    fSearchTime += elapsed_ms;
    return elapsed_ms;
}


int AStarSearch::AStarCheckFlip(AStarNode *before, AStarNode *after) {
    return before->GetTotalCost() <= after->GetTotalCost();
}

void AStarNode::operator delete(void *ptr) {
    bFree(AStarNodeSlotPool, ptr);
}
