#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"

#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

extern RaceParameters TheRaceParameters;
extern cFrontendDatabase *FEDatabase;

extern ePlayerSettingsCameras GetPlayerCameraFromPOVType(POVTypes pov);

// STRIPPED
void RaceStarter::SetupPlayerCarsAndStuff(int num_players) {}

// STRIPPED
void RaceStarter::AddTrafficCars() {}

// STRIPPED
void RaceStarter::AddAIOpponentCars(int num_ai_opponents) {}

// STRIPPED
void RaceStarter::AddRandomEncounterCars(int num_ai_opponents) {}

// STRIPPED
void RaceStarter::MatchAICarPerformanceToPlayer() {}

// STRIPPED
int RaceStarter::DecideAISkillLevel(eOpponentStrength opponent_strength) {
    int skill_level;
}

// STRIPPED
void RaceStarter::RandomizePlayerStartPositions() {}

// STRIPPED
void RaceStarter::AddCopDriverInfos(int ncops) {}

// STRIPPED
void RaceStarter::SetupCircuit() {
    RaceSettings *settings;
    DriverInfo *player_1_driver_info;
    DriverInfo *player_2_driver_info;
}

// STRIPPED
void RaceStarter::SetupShortTrack() {}

// STRIPPED
void RaceStarter::SetupSprint() {}

// STRIPPED
void RaceStarter::SetupFreeRun() {
    RaceSettings *settings;
}

// STRIPPED
void RaceStarter::SetupBurnout() {
    RaceSettings *settings;
}

// STRIPPED
void RaceStarter::SetupGT() {}

// STRIPPED
void RaceStarter::StartXenonE3Demo() {
    GRaceParameters *parms;
    GRaceCustom *race;
}

void RaceStarter::StartRace() {
    TheRaceParameters.InitWithDefaults();
    TheRaceParameters.bOnlineRace = false;
    TheRaceParameters.TrackNumber = 2000;
    for (int i = 0; i < TheRaceParameters.NumPlayerCars; i++) {
        RaceStarter::SetControllerConfig(FEDatabase->GetPlayerSettings(i)->Config, static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(i)));
    }
    TheGameFlowManager.UnloadFrontend();
}

#if ONLINE_SUPPORT
void RaceStarter::SetupOnlineRace() {}
#endif

void RaceStarter::SetControllerConfig(int config, JoystickPort port) {}

// STRIPPED
void RaceStarter::SetRaceTheRaceParametersThatHaveNothingToDoWithTheRaceType() {}

// STRIPPED
//   void RaceStarter::StartUndergroundModeRace(FERaceEvent *race_event ) {}

// STRIPPED
void RaceStarter::SwapAICarOutOfPlayersPosition(int position, PlayerNumbers player_number) {}

void RaceStarter::StartSkipFERace() {
    int track_num = SkipFETrackNumber;

    if (OnlineEnabled) {
        TheRaceParameters.bOnlineRace = true;
    }

    TheRaceParameters.TrackNumber = track_num;
    TheRaceParameters.NumPlayerCars = SkipFENumPlayerCars;
    TheRaceParameters.TrackDirection = SkipFETrackDirection;
    TheRaceParameters.TrafficOncoming = SkipFETrafficOncoming;
    TheRaceParameters.RaceType = SkipFERaceType;
    TheRaceParameters.Point2Point = SkipFEPoint2Point;
    TheRaceParameters.NumLapsInRace = SkipFENumLaps;
    TheRaceParameters.NumAICars = SkipFENumAICars;
    TheRaceParameters.nMaxCops = SkipFEMaxCops;
    TheRaceParameters.DamageEnabled = SkipFEDamageEnabled != 0;
    TheRaceParameters.PlayerStartPosition[0] = static_cast<signed char>(SkipFENumAICars + 1);
    TheRaceParameters.PlayerStartPosition[1] = static_cast<signed char>(SkipFENumAICars + 2);
    TheRaceParameters.CopStrength = SkipFEDifficulty;
    TheRaceParameters.OpponentStrength = SkipFEDifficulty;

    if (SkipFEControllerConfig1 != -1) {
        FEDatabase->GetPlayerSettings(0)->Config = static_cast<eControllerConfig>(SkipFEControllerConfig1);
    }
    if (SkipFEControllerConfig2 != -1) {
        FEDatabase->GetPlayerSettings(0)->Config = static_cast<eControllerConfig>(SkipFEControllerConfig2);
    }

    FEDatabase->GetPlayerSettings(0)->CurCam = GetPlayerCameraFromPOVType(static_cast<POVTypes>(SkipFEPovType1));
    FEDatabase->GetPlayerSettings(1)->CurCam = GetPlayerCameraFromPOVType(static_cast<POVTypes>(SkipFEPovType1));

    for (int kk = 0; kk < TheRaceParameters.NumPlayerCars; kk++) {
        TheRaceParameters.PlayerJoyports[kk] = static_cast<int>(FEDatabase->GetPlayersJoystickPort(kk));
    }

    TheRaceParameters.NumDriverInfo = 0;
    TheRaceParameters.BoostScale[1] = 1.0f;
    TheRaceParameters.BoostScale[0] = 1.0f;

    if (TheGameFlowManager.IsInFrontend()) {
        TheGameFlowManager.UnloadFrontend();
    } else {
        TheGameFlowManager.LoadTrack();
    }
}

// STRIPPED
void RaceStarter::StartReplayRace() {}

// STRIPPED
void RaceStarter::StartCarTuningRace() {}

void RaceStarter::StartCareerFreeRoam() {
    if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_IN_FRONTEND) {
        TheGameFlowManager.UnloadFrontend();
    } else {
        TheGameFlowManager.LoadTrack();
    }
}

// STRIPPED
void RaceStarter::StartCareerFreeRoamFromInGame() {}
