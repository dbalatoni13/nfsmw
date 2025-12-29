#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySimTick.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Lua/LuaAttributes.h"
#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"
#include "Speed/Indep/Src/Lua/LuaRuntime.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

// total size: 0x50
class GameplayActivity : public Sim::Activity {
  public:
    static IActivity *Construct(Sim::Param params);

    GameplayActivity(Sim::Param params);

    // Virtual functions
    ~GameplayActivity() override;

    // ITaskable
    bool OnTask(HSIMTASK task, float dT) override;

  private:
    HSIMTASK mUpdateTask; // offset 0x4C, size 0x4
};

GameplayActivity::GameplayActivity(Sim::Param params) : Sim::Activity(0) {
    mUpdateTask = AddTask(UCrc32("GameplayActivity"), 1.0f, 0.0f, Sim::TASK_FRAME_FIXED);
    Sim::ProfileTask(mUpdateTask, "GameplayActivity");

    GManager::Get().PreBeginGameplay();
    GRaceStatus::Init();
    LuaRuntime::Init(0x10000);
    LuaPostOffice::Init();
    LuaBindery::Init();
    LuaAttributes::Init();
    LuaBindery::Get().BindToGameCode(LuaRuntime::Get().GetState());
    LuaAttributes::Get().BindAccessors(LuaRuntime::Get().GetState());
    LuaRuntime::Get().TakeResetSnapshot();
    GManager::Get().BeginGameplay();
}

GameplayActivity::~GameplayActivity() {
    GManager::Get().EndGameplay();
    LuaAttributes::Shutdown();
    LuaBindery::Shutdown();
    LuaPostOffice::Shutdown();
    LuaRuntime::Shutdown();
    GRaceStatus::Shutdown();
    RemoveTask(mUpdateTask);
}

Sim::IActivity *GameplayActivity::Construct(Sim::Param params) {
    return new GameplayActivity(Sim::Param(params));
}

bool GameplayActivity::OnTask(HSIMTASK task, float dT) {
    ProfileNode profile_node("TODO", 0);

    if (task == mUpdateTask) {
        float simTime = Sim::GetTime();
        float simTimeStep = Sim::GetTimeStep();
        // TODO magic
        MNotifySimTick(simTime, simTimeStep).Post(UCrc32(0x20d60dbf));
        GManager::Get().Update(dT);
        return true;
    } else {
        Object::OnTask(task, dT);
    }
    return false;
}
