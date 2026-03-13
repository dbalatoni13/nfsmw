#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"

class RaceStarter {
  public:
    static void StartCareerFreeRoam();
    static void SetControllerConfig(int config, JoystickPort port);
};

void RaceStarter::SetControllerConfig(int config, JoystickPort port) {
}

void RaceStarter::StartCareerFreeRoam() {
    if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_IN_FRONTEND) {
        TheGameFlowManager.UnloadFrontend();
    } else {
        TheGameFlowManager.LoadTrack();
    }
}