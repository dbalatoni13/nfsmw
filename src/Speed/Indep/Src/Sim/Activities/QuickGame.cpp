#include "QuickGame.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/ECommitAudioAssets.hpp"
#include "Speed/Indep/Src/Generated/Events/ECommitRenderAssets.hpp"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"

QuickGame::QuickGame(Sim::Param params)
    : Sim::Activity(4),                      //
      Sim::ITimeManager(this),               //
      Sim::IStateManager(this),              //
      IVehicleCache(this),                   //
      IGameState(this),                      //
      mTestingSplitscreen(false),            //
      mInGameBreaker(false),                 //
      mStartLocation(UMath::Vector3::kZero), //
      mActionQ(nullptr),                     //
      mState(RACESTATE_LOADING_WORLD) {
    mUpdateTask = AddTask(UCrc32("WorldUpdate"), 1.0f, 0.0f, Sim::TASK_FRAME_FIXED);
#ifndef EA_BUILD_A124
    Sim::ProfileTask(mUpdateTask, "QuickGame");
#endif
    World_DEBUGStartLocation(mStartLocation, mStartDirection);

    bool foundIntroNIS = false;
    GRaceCustom *quickRace = GRaceDatabase::Get().GetStartupRace();
    if (quickRace) {
        const char *nisName = quickRace->GetGameplayObj()->IntroNIS(0);
        if (nisName && nisName[0] != '\0') {
            foundIntroNIS = true;
        }
    }
    if (foundIntroNIS) {
        TheTrackStreamer.DisableZoneSwitching();
    } else {
        Sim::SetStream(mStartLocation, false);
    }
}

QuickGame::~QuickGame() {
    RemoveTask(mUpdateTask);
    if (mActionQ) {
        delete mActionQ;
    }
}

bool QuickGame::ShouldPauseInput() {
    if (Sim::GetState() != Sim::STATE_ACTIVE) {
        return true;
    }
    if (INIS::Exists()) {
        return true;
    }
    return false;
}

void QuickGame::RaceReset() {
    IPlayer *player1 = IPlayer::First(PLAYER_LOCAL);
    if (player1) {
        if (player1->InGameBreaker()) {
            player1->ToggleGameBreaker();
        }
    }
    mInGameBreaker = false;
}

void QuickGame::OnUpdate(float dT) {}

bool QuickGame::OnTask(HSIMTASK htask, float dT) {
    ProfileNode profile_node("TODO", 0);

    if (htask == mUpdateTask) {
        OnUpdate(dT);
        return true;
    } else {
        Object::OnTask(htask, dT);
        return false;
    }
}

Sim::IActivity *QuickGame::Construct(Sim::Param params) {
    return new QuickGame(Sim::Param(params));
}

void QuickGame::CreatePlayers() {
    bool split_screen = Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN;
    Sim::IEntity *entity = UTL::COM::Factory<Sim::Param, Sim::IEntity, UCrc32>::CreateInstance(UCrc32("LocalPlayer"), Sim::Param());
    IPlayer *player;

    if (entity->QueryInterface(&player)) {
        player->SetControllerPort(FEDatabase->PlayerJoyports[0]);
        player->SetSettings(0);
        player->SetRenderPort(1);
    }

    if (split_screen) {
        entity = UTL::COM::Factory<Sim::Param, Sim::IEntity, UCrc32>::CreateInstance(UCrc32("LocalPlayer"), Sim::Param());
        if (entity->QueryInterface(&player)) {
            player->SetControllerPort(FEDatabase->PlayerJoyports[1]);
            player->SetSettings(1);
            player->SetRenderPort(2);
        }
    }
}

eVehicleCacheResult QuickGame::OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const {
    for (IPlayer::List::const_iterator iter = IPlayer::GetList(PLAYER_ALL).begin(); iter != IPlayer::GetList(PLAYER_ALL).end(); iter++) {
        IPlayer *player = *iter;
        if (UTL::COM::ComparePtr(player->GetSimable(), removethis)) {
            return VCR_WANT;
        }
    }
    return VCR_DONTCARE;
}

void QuickGame::OnRemovedVehicleCache(IVehicle *ivehicle) {}

void QuickGame::OnBeginState() {
    switch (mState) {
        case RACESTATE_LOADING_PLAYERS:
            HandleSkipFEOptions();
            CreatePlayers();
            CreateCars(mStartLocation);
            break;
        case RACESTATE_LOADING_PLAYER_MODELS:
            new ECommitRenderAssets();
            break;
        case RACESTATE_LOADING_ACTIVITIES:
            UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance(UCrc32("AvoidableManager"), Sim::Param());
            UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance(UCrc32("AICopManager"), Sim::Param());
            UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance(UCrc32("AITrafficManager"), Sim::Param());
            UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance(UCrc32("AIParkedCarSpawner"), Sim::Param());
            UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance(UCrc32("PathFinder"), Sim::Param());
            UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance(UCrc32("GameplayActivity"), Sim::Param());
            UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance(UCrc32("Gps"), Sim::Param());
            UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance(UCrc32("NISListenerActivity"), Sim::Param());
            break;
        case RACESTATE_LOADING_OPPONENT_AUDIO:
        case RACESTATE_LOADING_PLAYER_AUDIO:
            new ECommitAudioAssets();
            break;
        case RACESTATE_LOADING_OPPONENT_MODELS:
            new ECommitRenderAssets();
            break;
    }
}

// Requires MenuScreen stuff
// bool QuickGame::CanSimulate() {}

Sim::State QuickGame::OnManageState(Sim::State state) {
    if ((state == Sim::STATE_INITIALIZING || state == Sim::STATE_NONE) && mState < RACESTATE_READY) {
        if (IsStateDone()) {
            mState = static_cast<eState>(mState + 1);
            OnBeginState();
        }
        return Sim::STATE_INITIALIZING;
    } else {
        if (CanSimulate()) {
            return Sim::STATE_ACTIVE;
        } else {
            return Sim::STATE_IDLE;
        }
    }
}

// TODO move
extern BOOL SkipFE;
extern BOOL SkipFEDisableCops;
extern char SkipFERaceID[16];
extern int SkipFENumLaps;
extern eTrackDirection SkipFETrackDirection;
extern int SkipFENumAICars;
extern eOpponentStrength SkipFEDifficulty;
extern BOOL SkipFEDisableTraffic;
extern int SkipFETrafficDensity;

void QuickGame::HandleSkipFEOptions() {
    if (!SkipFE) {
        return;
    }
    if (!GRaceDatabase::Get().GetStartupRace() && SkipFERaceID[0] != '\0') {
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromName(SkipFERaceID);
        if (parms) {
            GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(parms);
            race->SetReversed(SkipFETrackDirection == eDIRECTION_BACKWARD);
            race->SetNumLaps(parms->GetIsLoopingRace() ? SkipFENumLaps : 1);
            race->SetTrafficDensity(SkipFEDisableTraffic ? 0 : SkipFETrafficDensity);
            race->SetNumOpponents(SkipFENumAICars);
            race->SetDifficulty((GRace::Difficulty)SkipFEDifficulty);
            if (SkipFEDisableCops) {
                race->SetCopsEnabled(false);
            }
            FEDatabase->RaceMode = race->GetRaceType();
            GRaceDatabase::Get().SetStartupRace(race, kRaceContext_QuickRace);
            GRaceDatabase::Get().FreeCustomRace(race);
        }
    }
}
