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
