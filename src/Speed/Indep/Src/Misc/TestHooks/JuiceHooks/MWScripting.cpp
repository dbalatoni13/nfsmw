#include "Speed/Indep/Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"

extern int DoScreenPrintf;
extern int UnlockAllThings;
extern int ForcePursuitNeverEnd;
extern bool Tweak_InfiniteRaceBreaker;

namespace Juice {

unsigned int MWCommands::mFEngScreenLoading;

void MWCommands::LoadingNewFEngPackage(unsigned int newPkg) {
    mFEngScreenLoading = newPkg;
}

int MWCommands::TurnDebugTextOn(Scripting::VarArgs &params) {
    DoScreenPrintf = 1;
    return 1;
}

int MWCommands::TurnDebugTextOff(Scripting::VarArgs &params) {
    DoScreenPrintf = 0;
    return 1;
}

int MWCommands::UnlockAllThings(Scripting::VarArgs &params) {
    ::UnlockAllThings = 1;
    return 1;
}

int MWCommands::IsInFreeRoam(Scripting::VarArgs &params) {
    if (IsGameFlowInGame()) {
        return static_cast<int>(GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming);
    }
    return -1;
}

int MWCommands::InfRaceBreaker(Scripting::VarArgs &params) {
    if (!Tweak_InfiniteRaceBreaker) {
        Tweak_InfiniteRaceBreaker = true;
        return 1;
    }
    return 0;
}

int MWCommands::IsSplitScreen(Scripting::VarArgs &params) {
    return static_cast<int>(IPlayer::Last(PLAYER_LOCAL) != nullptr);
}

int MWCommands::TurnPursuitForeverOn(Scripting::VarArgs &params) {
    ::ForcePursuitNeverEnd = 1;
    return 1;
}

int MWCommands::TurnPursuitForeverOff(Scripting::VarArgs &params) {
    ::ForcePursuitNeverEnd = 0;
    return 1;
}

}
