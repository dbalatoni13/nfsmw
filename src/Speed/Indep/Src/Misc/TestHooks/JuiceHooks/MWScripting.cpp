#include "Speed/Indep/Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern int DoScreenPrintf;
extern int UnlockAllThings;
extern int ForcePursuitNeverEnd;
extern bool Tweak_InfiniteRaceBreaker;
extern float fpsTolerateValue;
extern int logCountDownMax;
extern int ForcePursuitHeatLevel;
extern void JumpToNewPos(bVector3 *jumpPosition);
extern float Tweak_TuningAero;
extern int Tweak_UseTweakerTunings;

static float last_x;
static float last_y;
static float last_z;
static float last_speed;
extern void Game_ChallengeCompleted();
extern void Game_AwardPlayerBounty(int amount);
extern int Game_GetPlayerBounty();

namespace Juice {

unsigned int MWCommands::mFEngScreenLoading;

MWCommands::MWCommands() {
    Scripting::ICommands::SendMethod(0, this, "TurnDebugTextOn", 8,
                                      &MWCommands::TurnDebugTextOn);
    Scripting::ICommands::SendMethod(0, this, "TurnDebugTextOff", 8,
                                      &MWCommands::TurnDebugTextOff);
    Scripting::ICommands::SendMethod(0, this, "InFrontEnd", 8, &MWCommands::InFrontEnd);
    Scripting::ICommands::SendMethod(0, this, "ScreenLoaded", 8, &MWCommands::ScreenLoaded);
    Scripting::ICommands::SendMethod(0, this, "TeleportToCoords", 8,
                                      &MWCommands::TeleportToCoords);
    Scripting::ICommands::SendMethod(0, this, "FinishRaceInPlace", 8,
                                      &MWCommands::FinishRaceInPlace);
    Scripting::ICommands::SendMethod(0, this, "InDriveMode", 8, &MWCommands::InDriveMode);
    Scripting::ICommands::SendMethod(0, this, "IsCarStuck", 8, &MWCommands::IsCarStuck);
    Scripting::ICommands::SendMethod(0, this, "TurnAutoPilotOn", 8,
                                      &MWCommands::TurnAutoPilotOn);
    Scripting::ICommands::SendMethod(0, this, "TurnAutoPilotOff", 8,
                                      &MWCommands::TurnAutoPilotOff);
    Scripting::ICommands::SendMethod(0, this, "TurnPursuitOn", 8, &MWCommands::TurnPursuitOn);
    Scripting::ICommands::SendMethod(0, this, "TurnPursuitOff", 8, &MWCommands::TurnPursuitOff);
    Scripting::ICommands::SendMethod(0, this, "TurnPursuitForeverOn", 8,
                                      &MWCommands::TurnPursuitForeverOn);
    Scripting::ICommands::SendMethod(0, this, "TurnPursuitForeverOff", 8,
                                      &MWCommands::TurnPursuitForeverOff);
    Scripting::ICommands::SendMethod(0, this, "SpeedBoost", 8, &MWCommands::SpeedBoost);
    Scripting::ICommands::SendMethod(0, this, "SetHeat", 8, &MWCommands::SetHeat);
    Scripting::ICommands::SendMethod(0, this, "SetFrameRateThreshold", 8,
                                      &MWCommands::SetFrameRateThreshold);
    Scripting::ICommands::SendMethod(0, this, "InfRaceBreaker", 8,
                                      &MWCommands::InfRaceBreaker);
    Scripting::ICommands::SendMethod(0, this, "IsSplitScreen", 8, &MWCommands::IsSplitScreen);
    Scripting::ICommands::SendMethod(0, this, "ScatterCops", 8, &MWCommands::ScatterCops);
    Scripting::ICommands::SendMethod(0, this, "IsPursuit", 8, &MWCommands::IsPursuit);
    Scripting::ICommands::SendMethod(2, this, "DisplayFMVfilename", 8,
                                      &MWCommands::DisplayFMVfilename);
    Scripting::ICommands::SendMethod(0, this, "AwardBounty", 8, &MWCommands::AwardBounty);
    Scripting::ICommands::SendMethod(0, this, "GetBountyValue", 8,
                                      &MWCommands::GetBountyValue);
    Scripting::ICommands::SendMethod(0, this, "UnlockAllThings", 8,
                                      &MWCommands::UnlockAllThings);
    Scripting::ICommands::SendMethod(0, this, "IsInFreeRoam", 8, &MWCommands::IsInFreeRoam);
    Scripting::ICommands::SendMethod(0, this, "PassChallenge", 8, &MWCommands::PassChallenge);
    Scripting::ICommands::SendMethod(0, this, "PassSpeedtrap", 8, &MWCommands::PassSpeedtrap);
    Scripting::ICommands::SendMethod(2, this, "GetTrackID", 8, &MWCommands::GetTrackID);
    MWJuicePad::Instance()->Initialize();
}

MWCommands *MWCommands::Instance() {
    static MWCommands mwc;
    return &mwc;
}

char *MWCommands::GetNamespaceName() const {
    return "NFSMW";
}

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
    if (!IsGameFlowInGame()) {
        return -1;
    }
    return static_cast<int>(GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming);
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

int MWCommands::PassChallenge(Scripting::VarArgs &params) {
    CareerSettings *career = FEDatabase->CurrentUserProfiles[0]->GetCareer();
    if (career == nullptr) {
        return -1;
    }
    GMilestone *binMilestone = GManager::Get().GetFirstMilestone(true, career->GetCurrentBin());
    if (binMilestone == nullptr) {
        goto no_milestone;
    }
    binMilestone->DebugForceComplete();
    Game_ChallengeCompleted();
    return 1;
no_milestone:
    return 0;
}

int MWCommands::PassSpeedtrap(Scripting::VarArgs &params) {
    CareerSettings *career = FEDatabase->CurrentUserProfiles[0]->GetCareer();
    if (career == nullptr) {
        return -1;
    }
    GSpeedTrap *binSpeedTrap = GManager::Get().GetFirstSpeedTrap(true, career->GetCurrentBin());
    if (binSpeedTrap == nullptr) {
        goto no_speed_trap;
    }
    binSpeedTrap->DebugForceComplete();
    Game_ChallengeCompleted();
    return 1;
no_speed_trap:
    return 0;
}

int MWCommands::AwardBounty(Scripting::VarArgs &params) {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (player == nullptr) {
        return -1;
    }
    int tempInt;
    params.GetInt(tempInt);
    if (tempInt == 0) {
        tempInt = 1000;
    }
    Game_AwardPlayerBounty(tempInt);
    return 1;
}

int MWCommands::GetBountyValue(Scripting::VarArgs &params) {
    if (IPlayer::First(PLAYER_LOCAL) == nullptr) {
        return -1;
    }
    return Game_GetPlayerBounty();
}

int MWCommands::IsPursuit(Scripting::VarArgs &params) {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (player == nullptr) {
        return -1;
    }
    ISimable *isimable = player->GetSimable();
    if (isimable == nullptr) {
        return -1;
    }
    IVehicle *ivehicle;
    if (!isimable->QueryInterface(&ivehicle)) {
        return -1;
    }
    IVehicleAI *ivehicleai = ivehicle->GetAIVehiclePtr();
    if (ivehicleai == nullptr) {
        return -1;
    }
    return static_cast<int>(ivehicleai->GetPursuit() != nullptr);
}

char *MWCommands::GetTrackID(Scripting::VarArgs &params) {
    if (!GRaceStatus::Exists()) {
        goto no_track;
    }
    {
        GRaceParameters *raceParams = GRaceStatus::Get().GetRaceParameters();
        if (raceParams == nullptr) {
            goto no_track;
        }
        return const_cast<char *>(raceParams->GetEventID());
    }
no_track:
    return "";
}

int MWCommands::InFrontEnd(Scripting::VarArgs &params) {
    bool bVar1;
    if (Sim::GetState() != Sim::STATE_NONE) {
        bVar1 = cFEng::Get()->IsPackagePushed("Loading.fng");
        if (!bVar1) {
            return 0;
        }
    }
    return 1;
}

char *MWCommands::DisplayFMVfilename(Scripting::VarArgs &params) {
    if (gMoviePlayer == nullptr) {
        goto no_movie;
    }
    return gMoviePlayer->GetMovieFilename();
no_movie:
    return "";
}

int MWCommands::SetFrameRateThreshold(Scripting::VarArgs &params) {
    int tempInt = 0;
    if (params.GetNumberOfRemainingArgs() < 2) {
        return 0;
    }
    params.GetInt(tempInt);
    fpsTolerateValue = static_cast<float>(tempInt);
    params.GetInt(logCountDownMax);
    return 1;
}

int MWCommands::ScreenLoaded(Scripting::VarArgs &params) {
    FEPackage *package = cFEng::Get()->FindPackageWithControl();
    if (package == nullptr) {
        goto not_loaded;
    }
    package = cFEng::Get()->FindPackageWithControl();
    if (*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(package) + 0x10) != mFEngScreenLoading) {
        goto not_loaded;
    }
    return 1;
not_loaded:
    return 0;
}

int MWCommands::InDriveMode(Scripting::VarArgs &params) {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (player == nullptr) {
        return 0;
    }
    if (IPlayer::First(PLAYER_LOCAL)->GetHud() == nullptr) {
        return 0;
    }
    ICountdown *countdown;
    IPlayer::First(PLAYER_LOCAL)->GetHud()->QueryInterface(&countdown);
    if (Sim::GetState() == Sim::STATE_ACTIVE) {
        if (cFEng::Get()->FindPackageWithControl() == nullptr && !INIS::Exists()) {
            int result = 1;
            bool active = countdown->IsActive();
            if (active) {
                result = 0;
            }
            return result;
        }
    }
    return 0;
}

int MWCommands::SetHeat(Scripting::VarArgs &params) {
    int heat;
    if (params.GetNumberOfRemainingArgs() <= 0) {
        goto default_heat;
    }
    params.GetInt(heat);
    goto set_heat;
default_heat:
    heat = 4;
set_heat:
    ForcePursuitHeatLevel = heat;
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    IPerpetrator *perp;
    player->GetSimable()->QueryInterface(&perp);
    if (perp != nullptr) {
        perp->SetHeat(static_cast<float>(heat));
        return 1;
    }
    return 0;
}

int MWCommands::FinishRaceInPlace(Scripting::VarArgs &params) {
    int place;
    if (params.GetNumberOfRemainingArgs() <= 0) {
        goto default_place;
    }
    params.GetInt(place);
    goto have_place;
default_place:
    place = 1;
have_place:
    if (place > 0) {
        GRaceStatus::Get().SkipToEndOfRaceInPlace(place, 2.0f);
        return 1;
    }
    return 0;
}

int MWCommands::TeleportToCoords(Scripting::VarArgs &params) {
    bVector3 teleportPos;
    if (!GRaceStatus::Exists() || GRaceStatus::Get().GetPlayMode() != GRaceStatus::kPlayMode_Roaming) {
        return 0;
    }
    int tempTeleportPos = 0;
    for (int index = 0; index < 3; index++) {
        if (params.GetNumberOfRemainingArgs() < 1) {
            return 0;
        }
        params.GetInt(tempTeleportPos);
        reinterpret_cast<float *>(&teleportPos)[index] = static_cast<float>(tempTeleportPos);
    }
    JumpToNewPos(&teleportPos);
    return 1;
}

int MWCommands::IsCarStuck(Scripting::VarArgs &params) {
    int result = 0;
    IVehicle *pVehicle = IVehicle::First(VEHICLE_PLAYERS);
    float current_y;
    float current_x;
    float current_z;
    float current_speed;
    if (pVehicle != nullptr) {
        const UMath::Vector3 &position = pVehicle->GetPosition();
        current_x = position.x;
        current_y = position.y;
        current_z = position.z;
        current_speed = pVehicle->GetSpeed();
        if (bAbs(current_x - last_x) < 5.0f && bAbs(current_y - last_y) < 5.0f && bAbs(current_z - last_z) < 5.0f &&
            current_speed < 1.0f) {
            result = 1;
        }
        last_x = current_x;
        last_y = current_y;
        last_z = current_z;
        last_speed = current_speed;
    }
    return result;
}

int MWCommands::TurnPursuitForeverOn(Scripting::VarArgs &params) {
    ::ForcePursuitNeverEnd = 1;
    return 1;
}

int MWCommands::TurnPursuitForeverOff(Scripting::VarArgs &params) {
    ::ForcePursuitNeverEnd = 0;
    return 1;
}

int MWCommands::TurnPursuitOn(Scripting::VarArgs &params) {
    if (ICopMgr::Exists() && TheGameFlowManager.IsInGame()) {
        if (ICopMgr::mDisableCops) {
            ICopMgr::mDisableCops = 0;
        }
        return 1;
    }
    return -1;
}

int MWCommands::TurnPursuitOff(Scripting::VarArgs &params) {
    if (ICopMgr::Exists() && TheGameFlowManager.IsInGame()) {
        if (ICopMgr::mDisableCops == 0) {
            ICopMgr::mDisableCops = 1;
        }
        return 1;
    }
    return -1;
}

int MWCommands::SpeedBoost(Scripting::VarArgs &params) {
    float gSpeed = 350.0f;
    params.GetFloat(gSpeed);
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (player != nullptr) {
        IVehicle *vehicle;
        if (player->GetSimable()->QueryInterface(&vehicle)) {
            vehicle->SetSpeed(gSpeed * 0.447027266f);
            Tweak_TuningAero = 1.0f;
            Tweak_UseTweakerTunings = 1;
            return 1;
        } else {
            return -1;
        }
    }
    return -1;
}

int MWCommands::ScatterCops(Scripting::VarArgs &params) {
    if (TheGameFlowManager.GetState() != GAMEFLOW_STATE_RACING) {
        return -1;
    }

    IRigidBody *playerIRB = IPlayer::First(PLAYER_LOCAL)->GetSimable()->GetRigidBody();
    static float gScatterForce;
    static float gScatterJump;
    static float gScatterRoll;

    IVehicle::List &cops = const_cast<IVehicle::List &>(IVehicle::GetList(VEHICLE_AICOPS));
    for (IVehicle::List::iterator iter = cops.begin(); iter != cops.end(); ++iter) {
        IVehicle *vehicle = *iter;
        ISimable *simable = vehicle->GetSimable();
        IRigidBody *rigidBody = simable->GetRigidBody();
        const UMath::Vector3 &playerPosition = playerIRB->GetPosition();
        UMath::Vector3 toCop;
        UMath::Vector3 linearVel;
        UMath::Vector3 angularVel;

        UMath::Sub(rigidBody->GetPosition(), playerPosition, toCop);
        UMath::Normalize(toCop);
        UMath::Scale(toCop, gScatterForce, linearVel);
        linearVel.y += gScatterJump;
        angularVel.x = Sim::GetRandom()._SimRandom_FloatRange(gScatterRoll);
        angularVel.y = Sim::GetRandom()._SimRandom_FloatRange(gScatterRoll);
        angularVel.z = Sim::GetRandom()._SimRandom_FloatRange(gScatterRoll);
        rigidBody->SetLinearVelocity(linearVel);
        rigidBody->SetAngularVelocity(angularVel);
    }
    return 1;
}

int MWCommands::TurnAutoPilotOn(Scripting::VarArgs &params) {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (player != nullptr) {
        ISimable *simable = player->GetSimable();
        if (simable != nullptr) {
            IHumanAI *humanAI;
            if (simable->QueryInterface(&humanAI)) {
                humanAI->SetAiControl(true);
                return 1;
            }
        }
    }
    return -1;
}

int MWCommands::TurnAutoPilotOff(Scripting::VarArgs &params) {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (player != nullptr) {
        ISimable *simable = player->GetSimable();
        if (simable != nullptr) {
            IHumanAI *humanAI;
            if (simable->QueryInterface(&humanAI)) {
                humanAI->SetAiControl(false);
                return 0;
            }
        }
    }
    return -1;
}

}
