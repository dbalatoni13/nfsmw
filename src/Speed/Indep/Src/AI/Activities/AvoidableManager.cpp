#include "Speed/Indep/Src/AI/AIAvoidable.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

// total size: 0x54
// Decl: 23
class AvoidableManager : public Sim::Activity, public Debugable {
  public:
    AvoidableManager(Sim::Param params);
    ~AvoidableManager() override;

    static Sim::IActivity *Construct(Sim::Param params);

    // ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    virtual void OnDebugDraw();

  private:
    HSIMTASK mSimulateTask; // offset 0x50, size 0x4
};

BIND_ACTIVITY_FACTORY(AvoidableManager); // Decl: 38

AvoidableManager::AvoidableManager(Sim::Param params) : Sim::Activity(0) {
    MakeDebugable(DBG_AIAVOIDABLE);
    this->mSimulateTask = this->AddTask(UCrc32("AvoidableManager"), 0.25f, 0.0f, Sim::TASK_FRAME_FIXED);
    Sim::ProfileTask(this->mSimulateTask, "AvoidableManager");
}

AvoidableManager::~AvoidableManager() {
    this->RemoveTask(this->mSimulateTask);
}

Sim::IActivity *AvoidableManager::Construct(Sim::Param params) {
    return new AvoidableManager(params);
}

void AvoidableManager::OnDebugDraw() {
    AIAvoidable::DrawAll();
}

bool AvoidableManager::OnTask(HSIMTASK htask, float dT) {
    ProfileNode profile_node("TODO", 0);

    if (htask == this->mSimulateTask) {
        AIAvoidable::UpdateAllAvoidables(dT);
        return true;
    } else {
        this->Sim::Object::OnTask(htask, dT);
        return false;
    }
}

IMPLEMENT_SAP_GRID(AIAvoidable); // Decl: 78
AvoidableList AIAvoidable::mAll; //  Decl: 79
AIAvoidable::AIAvoidable(UTL::COM::IUnknown *pUnkPersist) : mGridNode(nullptr), mUnk(pUnkPersist) {
    mAll.push_back(this);
}

AIAvoidable::~AIAvoidable() {
    for (Neighbors::const_iterator iter = mNeighbors.begin(); iter != mNeighbors.end(); iter++) {
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

void AIAvoidable::UpdateAllAvoidables(float dT) {
    unsigned int overlapx = 0;
    unsigned int overlapz = 0;

    for (AvoidableList::const_iterator iter = mAll.begin(); iter != mAll.end(); iter++) {
        AIAvoidable *pavoid = *iter;
        UVector3 pos(UMath::Vector3::kZero);
        float sweep = 0.0f;

        if (pavoid->OnUpdateAvoidable(pos, sweep)) {
            if (pavoid->mGridNode == nullptr) {
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
            if (pavoid->mGridNode != nullptr) {
                delete pavoid->mGridNode;
                pavoid->mGridNode = nullptr;
            }
        }

        pavoid->mNeighbors.clear();
    }

    unsigned int numiters = Grid::Sweep();
    Grid::Prune(overlapx < overlapz, OnOverLap, dT);
}
