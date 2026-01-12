#include "AvoidableManager.hpp"
#include "Speed/Indep/Src/AI/AIAvoidable.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

AvoidableManager::AvoidableManager(Sim::Param params) : Sim::Activity(0) {
    mSimulateTask = AddTask("AvoidableManager", 0.25f, 0.0f, Sim::TASK_FRAME_FIXED);
    Sim::ProfileTask(mSimulateTask, "AvoidableManager");
}

AvoidableManager::~AvoidableManager() {
    RemoveTask(mSimulateTask);
}

Sim::IActivity *AvoidableManager::Construct(Sim::Param params) {
    return new AvoidableManager(params);
}

void AvoidableManager::OnDebugDraw() {
    AIAvoidable::DrawAll();
}

bool AvoidableManager::OnTask(HSIMTASK htask, float dT) {
    ProfileNode profile_node("TODO", 0);

    if (htask == mSimulateTask) {
        AIAvoidable::UpdateAllAvoidables(dT);
        return true;
    } else {
        Sim::Object::OnTask(htask, dT);
        return false;
    }
}

AIAvoidable::AIAvoidable(UTL::COM::IUnknown *pUnkPersist) : mGridNode(nullptr), mUnk(pUnkPersist) {
    mAll.push_back(this);
}

// what?...
// AIAvoidable::~AIAvoidable() {
//     for (Neighbors::const_iterator iter = mNeighbors.begin(); iter != mNeighbors.end(); iter++) {
//         AIAvoidable *pavoid = *iter;
//         mNeighbors.erase(std::find(mNeighbors.begin(), mNeighbors.end(), this));
//     }
//     mAll.erase(std::find(mAll.begin(), mAll.end(), this));
//     delete mGridNode;
// }

void AIAvoidable::OnOverLap(AIAvoidable &a0, AIAvoidable &a1, float dT) {
    a0.mNeighbors.push_back(&a1);
    a1.mNeighbors.push_back(&a0);
}

void AIAvoidable::DrawAll() {}

// later when we have SAP::Grid inlines
// void AIAvoidable::UpdateAllAvoidables(float dT) {}
