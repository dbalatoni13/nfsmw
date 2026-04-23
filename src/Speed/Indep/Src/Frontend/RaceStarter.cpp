#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"

extern RaceParameters TheRaceParameters;
extern cFrontendDatabase *FEDatabase;

extern int OnlineEnabled;
extern int SkipFETrackNumber;
extern eTrackDirection SkipFETrackDirection;
extern float SkipFETrafficOncoming;
extern RaceTypes SkipFERaceType;
extern int SkipFEPoint2Point;
extern int SkipFENumLaps;
extern int SkipFENumPlayerCars;
extern int SkipFENumAICars;
extern int SkipFEMaxCops;
extern int SkipFEDamageEnabled;
extern eOpponentStrength SkipFEDifficulty;
extern int SkipFEControllerConfig1;
extern int SkipFEControllerConfig2;
extern int SkipFEPovType1;

enum POVTypes {
    kPOV_Far = 0,
    kPOV_Close = 1,
    kPOV_Bumper = 2,
    kPOV_Hood = 3,
    kPOV_Drift = 4,
    kPOV_Pursuit = 5,
    kPOV_Pullback = 6
};

extern ePlayerSettingsCameras GetPlayerCameraFromPOVType(POVTypes pov);

class RaceStarter {
  public:
    static void StartRace();
    static void StartSkipFERace();
    static void StartCareerFreeRoam();
    static void SetControllerConfig(int config, JoystickPort port);
};

void RaceStarter::StartRace() {
    TheRaceParameters.InitWithDefaults();
    TheRaceParameters.bOnlineRace = false;
    TheRaceParameters.TrackNumber = 2000;
    for (int i = 0; i < TheRaceParameters.NumPlayerCars; i++) {
        RaceStarter::SetControllerConfig(
            FEDatabase->GetPlayerSettings(i)->Config,
            static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(i)));
    }
    TheGameFlowManager.UnloadFrontend();
}

void RaceStarter::SetControllerConfig(int config, JoystickPort port) {
}

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

void RaceStarter::StartCareerFreeRoam() {
    if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_IN_FRONTEND) {
        TheGameFlowManager.UnloadFrontend();
    } else {
        TheGameFlowManager.LoadTrack();
    }
}