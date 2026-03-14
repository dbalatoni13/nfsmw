#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"

extern RaceParameters TheRaceParameters;
extern cFrontendDatabase *FEDatabase;

class RaceStarter {
  public:
    static void StartRace();
    static void StartCareerFreeRoam();
    static void SetControllerConfig(int config, JoystickPort port);
};

void RaceStarter::SetControllerConfig(int config, JoystickPort port) {
}

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

void RaceStarter::StartCareerFreeRoam() {
    if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_IN_FRONTEND) {
        TheGameFlowManager.UnloadFrontend();
    } else {
        TheGameFlowManager.LoadTrack();
    }
}