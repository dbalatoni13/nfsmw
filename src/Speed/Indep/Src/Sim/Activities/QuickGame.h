#ifndef SIM_ACTIVITIES_QUICKGAME_H
#define SIM_ACTIVITIES_QUICKGAME_H

#include "types.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IGameState.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

// total size: 0xA0
class QuickGame : public Sim::Activity, public Sim::ITimeManager, Sim::IStateManager, public IVehicleCache, public IGameState {
  public:
    enum eState {
        RACESTATE_LOADING_WORLD = 0,
        RACESTATE_LOADING_PLAYERS = 1,
        RACESTATE_LOADING_PLAYER_AUDIO = 2,
        RACESTATE_LOADING_PLAYER_MODELS = 3,
        RACESTATE_LOADING_ACTIVITIES = 4,
        RACESTATE_LOADING_UI = 5,
        RACESTATE_LOADING_OPPONENT_AUDIO = 6,
        RACESTATE_LOADING_OPPONENT_MODELS = 7,
        RACESTATE_LOADING_WAIT_FOR_USER_INPUT = 8,
        RACESTATE_LOADING_NIS = 9,
        RACESTATE_READY = 10,
        NUM_RACESTATES = 11,
    };

    static Sim::IActivity *Construct(Sim::Param params);

    QuickGame(Sim::Param params);

    // Virtual functions
    // IUnknown
    ~QuickGame() override;

    // IGameState
    bool InGameBreaker() const override;

    // ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    // ITimeManager
    float OnManageTime(float real_time_delta, float sim_speed) override;

    // IStateManager
    Sim::State OnManageState(Sim::State state) override;
    bool ShouldPauseInput() override;

    // IVehicleCache
    eVehicleCacheResult OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const override;
    void OnRemovedVehicleCache(IVehicle *ivehicle) override;

    // IGameState
    void RaceReset() override;

  private:
    void OnBeginState();
    bool IsStateDone() const;
    void OnUpdate(float dT);
    void CreatePlayers();
    void CreateCars(const UMath::Vector3 &startLoc);
    bool CanSimulate();
    void HandleSkipFEOptions();

    HSIMTASK mUpdateTask;                           // offset 0x74, size 0x4
    bool mTestingSplitscreen;                       // offset 0x78, size 0x1
    bool mInGameBreaker;                            // offset 0x7C, size 0x1
    ALIGN_XENON(16) UMath::Vector3 mStartLocation;  // offset 0x80, size 0xC
    ALIGN_XENON(16) UMath::Vector3 mStartDirection; // offset 0x8C, size 0xC
    eState mState;                                  // offset 0x98, size 0x4
    ActionQueue *mActionQ;                          // offset 0x9C, size 0x4
};

#endif
