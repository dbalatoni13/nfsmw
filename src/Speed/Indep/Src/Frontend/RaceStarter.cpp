#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

extern int OnlineEnabled;
extern int SkipFEPovType1;
extern int SkipFEPoint2Point;
extern int SkipFEDamageEnabled;
extern int SkipFEControllerConfig1;
extern int SkipFEControllerConfig2;
extern ePlayerSettingsCameras GetPlayerCameraFromPOVType(POVTypes pov);

void RaceStarter::AddTrafficCars() {}

void RaceStarter::AddCopDriverInfos(int ncops) {}

void RaceStarter::StartRace() {
    TheRaceParameters.InitWithDefaults();
    TheRaceParameters.bOnlineRace = false;
    TheRaceParameters.TrackNumber = 2000;
    for (int i = 0; i < TheRaceParameters.NumPlayerCars; i++) {
        SetControllerConfig(FEDatabase->GetPlayerSettings(i)->Config,
                            (JoystickPort)FEDatabase->GetPlayersJoystickPort(i));
    }
    TheGameFlowManager.UnloadFrontend();
}

void RaceStarter::SetControllerConfig(int config, JoystickPort port) {
}

void RaceStarter::StartSkipFERace() {
    int track_num = SkipFETrackNumber;

    if (OnlineEnabled != 0) {
        TheRaceParameters.bOnlineRace = true;
    }

    TheRaceParameters.NumPlayerCars = SkipFENumPlayerCars;

    TheRaceParameters.TrackNumber = track_num;
    TheRaceParameters.TrackDirection = SkipFETrackDirection;

    TheRaceParameters.TrafficOncoming = SkipFETrafficOncoming;

    TheRaceParameters.RaceType = (RaceTypes)SkipFERaceType;
    TheRaceParameters.Point2Point = SkipFEPoint2Point;
    TheRaceParameters.NumLapsInRace = SkipFENumLaps;
    TheRaceParameters.NumPlayerCars = SkipFENumPlayerCars;
    TheRaceParameters.NumAICars = SkipFENumAICars;
    TheRaceParameters.nMaxCops = SkipFEMaxCops;
    TheRaceParameters.DamageEnabled = SkipFEDamageEnabled != 0;

    TheRaceParameters.OpponentStrength = (eOpponentStrength)SkipFEDifficulty;
    TheRaceParameters.CopStrength = (eOpponentStrength)SkipFEDifficulty;

    TheRaceParameters.PlayerStartPosition[0] = SkipFENumAICars + 1;
    TheRaceParameters.PlayerStartPosition[1] = SkipFENumAICars + 2;

    if (SkipFEControllerConfig1 != -1) {
        FEDatabase->GetOptionsSettings()->ThePlayerSettings[0].Config =
            (eControllerConfig)SkipFEControllerConfig1;
    }
    if (SkipFEControllerConfig2 != -1) {
        FEDatabase->GetOptionsSettings()->ThePlayerSettings[0].Config =
            (eControllerConfig)SkipFEControllerConfig2;
    }
    FEDatabase->GetOptionsSettings()->ThePlayerSettings[0].CurCam =
        GetPlayerCameraFromPOVType((POVTypes)SkipFEPovType1);
    FEDatabase->GetOptionsSettings()->ThePlayerSettings[1].CurCam =
        GetPlayerCameraFromPOVType((POVTypes)SkipFEPovType1);
    for (int i = 0; i < TheRaceParameters.NumPlayerCars; i++) {
        TheRaceParameters.PlayerJoyports[i] = FEDatabase->PlayerJoyports[i];
    }
    TheRaceParameters.BoostScale[0] = 1.0f;
    TheRaceParameters.BoostScale[1] = 1.0f;
    TheRaceParameters.NumDriverInfo = 0;
    if (TheGameFlowManager.IsInFrontend()) {
        TheGameFlowManager.UnloadFrontend();
    } else {
        TheGameFlowManager.LoadTrack();
    }
}

void RaceStarter::StartCareerFreeRoam() {
    if (TheGameFlowManager.IsInFrontend()) {
        TheGameFlowManager.UnloadFrontend();
    } else {
        TheGameFlowManager.LoadTrack();
    }
}
