#include "GameFlow.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

GameFlowManager TheGameFlowManager; // size: 0x24

void RenderTrackMarkers(eView *view) {}

// todo
extern int SkipFE;
extern int SkipFESplitScreen;

// UNSOLVED
Sim::eUserMode CalculateSimMode() {
    Sim::eUserMode mode = Sim::USER_SINGLE;
    if (SkipFE) {
        if (SkipFESplitScreen) {
            mode = Sim::USER_SPLIT_SCREEN;
        }
    } else {
        if (TheOnlineManager.IsOnlineRace()) {
            mode = Sim::USER_ONLINE;
        } else if (FEDatabase->IsSplitScreenMode() && FEDatabase->iNumPlayers == 2) {
            mode = Sim::USER_SPLIT_SCREEN;
        } else {
            mode = Sim::USER_SINGLE;
        }
    }
    return mode;
}

// TODO
void GetBuildVersionName(char *build_version_name) {
#ifdef DEBUG
    bStrCpy(build_version_name, "Milestone");
#else
    bStrCpy(build_version_name, "Release");
#endif
}
