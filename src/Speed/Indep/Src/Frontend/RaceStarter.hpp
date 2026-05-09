#ifndef FRONTEND_RACESTARTER_H
#define FRONTEND_RACESTARTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"

typedef enum { PLAYER_1 = 0, PLAYER_2 = 1, MAX_NUM_PLAYERS = 2 } PlayerNumbers;

class RaceStarter {
  public:
    static void StartRace();
    static void SetupOnlineRace();
    // static void StartUndergroundModeRace(FERaceEvent *race_event);// STRIPPED along with FERaceEvent definition
    static void StartSkipFERace();
    static void StartReplayRace();
    static void StartCarTuningRace();
    static void StartCareerFreeRoam();
    // static void StartCareerEvent(/* parameters unknown */);
    static void StartCareerFreeRoamFromInGame();
    static int DecideAISkillLevel(eOpponentStrength opponent_strength);
    static void StartXenonE3Demo();

  private:
    static void SetControllerConfig(int config, JoystickPort port);
    static void SetupPlayerCarsAndStuff(int num_players);
    static void AddTrafficCars();
    static void AddCopDriverInfos(int ncops);
    static void AddAIOpponentCars(int num_ai_opponents);
    static void AddRandomEncounterCars(int num_ai_opponents);
    // static void AddCareerEventAIOpponents(/* parameters unknown */);
    static void MatchAICarPerformanceToPlayer();
    static void RandomizePlayerStartPositions();
    static void SetRaceTheRaceParametersThatHaveNothingToDoWithTheRaceType();
    static void SwapAICarOutOfPlayersPosition(int position, PlayerNumbers player_number);
#if ONLINE_SUPPORT
    static void InitSkipFEOnline();
#endif
    static void SetupCircuit();
    static void SetupSprint();
    static void SetupDragRace();
    static void SetupDriftRace();
    static void SetupBurnout();
    static void SetupStreetCross();
    static void SetupGT();
    static void SetupShortTrack();
    static void SetupFreeRun();
    static void SetupURL();
};

#endif
