#include "Speed/Indep/Src/World/WPathFinder.h"

PathFinder* PathFinder::pInstance;

Sim::IActivity* PathFinder::Construct(Sim::Param params) {
    if (pInstance == nullptr) {
        pInstance = new PathFinder();
    }
    return pInstance;
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

AStarSearch::~AStarSearch() {
    delete pSolution;
}

int AStarSearch::AStarCheckFlip(AStarNode *before, AStarNode *after) {
    return before->GetTotalCost() <= after->GetTotalCost();
}

void *AStarNode::operator new(unsigned int size) {
    return bMalloc(AStarNodeSlotPool);
}

void AStarNode::operator delete(void *ptr) {
    bFree(AStarNodeSlotPool, ptr);
}
