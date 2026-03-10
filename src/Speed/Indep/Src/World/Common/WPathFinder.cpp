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
