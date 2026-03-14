#include "AvoidableManager.hpp"
#include "Speed/Indep/Src/AI/AIAvoidable.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::Prototype _AvoidableManager("AvoidableManager", AvoidableManager::Construct);

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

AIAvoidable::~AIAvoidable() {
    for (Neighbors::const_iterator iter = mNeighbors.begin(); iter != mNeighbors.end(); ++iter) {
        AIAvoidable *pavoid = *iter;
        pavoid->mNeighbors.erase(std::find(pavoid->mNeighbors.begin(), pavoid->mNeighbors.end(), this));
    }
    mAll.erase(std::find(mAll.begin(), mAll.end(), this));
    delete mGridNode;
}

void AIAvoidable::OnOverLap(AIAvoidable &a0, AIAvoidable &a1, float dT) {
    a0.mNeighbors.push_back(&a1);
    a1.mNeighbors.push_back(&a0);
}

void AIAvoidable::DrawAll() {}

// later when we have SAP::Grid inlines
void AIAvoidable::UpdateAllAvoidables(float dT) {
    unsigned int overlapx = 0;
    unsigned int overlapz = 0;

    for (AvoidableList::const_iterator iter = mAll.begin(); iter != mAll.end(); ++iter) {
        AIAvoidable *pavoid = *iter;
        UVector3 pos(UMath::Vector3::kZero);
        float sweep = 0.0f;

        if (pavoid->OnUpdateAvoidable(pos, sweep)) {
            if (!pavoid->mGridNode) {
                pavoid->mGridNode = new Grid(*pavoid, pos, sweep);
            } else {
                pavoid->mGridNode->SetPosition(pos, sweep);
                if (pavoid->mGridNode->GetX().Overlaps()) {
                    overlapx++;
                }
                if (pavoid->mGridNode->GetZ().Overlaps()) {
                    overlapz++;
                }
            }
        } else {
            if (pavoid->mGridNode) {
                delete pavoid->mGridNode;
                pavoid->mGridNode = nullptr;
            }
        }

        pavoid->mNeighbors.clear();
    }

    Grid::Sweep();
    Grid::Prune(overlapz <= overlapx, OnOverLap, dT);
}
