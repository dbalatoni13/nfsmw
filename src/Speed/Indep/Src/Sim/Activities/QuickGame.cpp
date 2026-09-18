#include "QuickGame.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/ECommitAudioAssets.hpp"
#include "Speed/Indep/Src/Generated/Events/ECommitRenderAssets.hpp"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/InGameMovieScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FELoadingTips.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAudible.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorld.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern int TrackCopCameraMover_IdleSim;

int bSawLoadingScreen = 0;

float Tweak_GameSpeed = 1.0f;

extern float SkipFEPlayerPerformance;
extern const char *SkipFEPlayer2Car;

PresetCar *FindFEPresetCar(unsigned int key);

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
    mUpdateTask = AddTask("WorldUpdate", 1.0f, 0.0f, Sim::TASK_FRAME_FIXED);
#ifndef EA_BUILD_A124
    Sim::ProfileTask(mUpdateTask, "QuickGame");
#endif
    World_DEBUGStartLocation(mStartLocation, mStartDirection);

    bool foundIntroNIS = false;
    GRaceCustom *quickRace = GRaceDatabase::Get().GetStartupRace();
    if (quickRace) {
        const char *nisName = quickRace->GetGameplayObj()->IntroNIS();
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
    ProfileNode profile_node;

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
    Sim::IEntity *entity = Sim::IEntity::CreateInstance("LocalPlayer", Sim::Param());
    IPlayer *player;

    if (entity->QueryInterface(&player)) {
        player->SetControllerPort(FEDatabase->PlayerJoyports[0]);
        player->SetSettings(0);
        player->SetRenderPort(1);
    }

    if (split_screen) {
        entity = Sim::IEntity::CreateInstance("LocalPlayer", Sim::Param());
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

// NOTE: Physics::Info::Performance::Set() in the original header.
static inline void SetPerformance(Physics::Info::Performance &perf, float amount) {
    perf.TopSpeed = UMath::Clamp(amount, 0.0f, 1.0f);
    perf.Handling = UMath::Clamp(amount, 0.0f, 1.0f);
    perf.Acceleration = UMath::Clamp(amount, 0.0f, 1.0f);
}

// total size: 0x1B4
struct CarBuilder {
    unsigned int Key;                            // offset 0x0, size 0x4
    unsigned int Flags;                          // offset 0x4, size 0x4
    FECustomizationRecord *Customization;        // offset 0x8, size 0x4
    Physics::Info::Performance *Performance;     // offset 0xC, size 0x4
    FECustomizationRecord mCustomization;        // offset 0x10, size 0x198
    Physics::Info::Performance mPerformance;     // offset 0x1A8, size 0xC

    CarBuilder()
        : Key(0),                 //
          Flags(11),              //
          Customization(nullptr), //
          Performance(nullptr) {}

    void Match(float amount) {
        if (amount > 0.0f) {
            Physics::Info::Performance perf;
            SetPerformance(perf, amount);
            Match(perf);
        }
    }

    void Match(const Physics::Info::Performance &perf) {
        Attrib::Gen::pvehicle test(Key, 0, nullptr);

        if (Physics::Upgrades::MatchPerformance(test, perf)) {
            mPerformance = perf;
            Performance = &mPerformance;
        } else {
            Performance = nullptr;
        }
    }

    void Customize(const FECustomizationRecord &cust) {
        mCustomization = cust;
        Customization = &mCustomization;
    }
};

void QuickGame::CreateCars(const UMath::Vector3 &startLoc) {
    UMath::Vector3 initialPos = startLoc;
    UMath::Vector3 initialVec = mStartDirection;

    if (!WWorld::Get().IsValid()) {
        initialPos = UMath::Vector3::kZero;
    }

    float height = 0.0f;
    if (WCollisionMgr(0, 3).GetWorldHeightAtPoint(initialPos, height, nullptr)) {
        initialPos.y = height + 1.0f;
    }

    float Heat = 1.0f;
    CarBuilder player_1;

    if (SkipFE && SkipFEPlayerCar && SkipFEPlayerCar[0] != '\0') {
        player_1.Key = Attrib::StringToKey(SkipFEPlayerCar);
        player_1.Match(SkipFEPlayerPerformance);
    } else {
        GRaceCustom *startupRace = GRaceDatabase::Get().GetStartupRace();
        if (startupRace) {
            const char *presetRide = startupRace->GetPlayerCarType();
            if (presetRide && presetRide[0] != '\0') {
                PresetCar *preset = FindFEPresetCar(bStringHashUpper(presetRide));
                if (preset) {
                    player_1.Key = preset->VehicleKey;
                    player_1.Flags |= 0x10;

                    FECustomizationRecord preset_customization;
                    preset_customization.BecomePreset(preset);
                    player_1.Customize(preset_customization);
                    player_1.Match(startupRace->GetPlayerCarPerformance());
                }
            }
        }

        if (player_1.Key == 0) {
            unsigned int player_car = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(0);

            if (FEDatabase->IsCareerMode()) {
                CareerSettings *career = FEDatabase->GetCareerSettings();
                player_car = career->GetCurrentCar();

                if (career) {
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                    if (stable) {
                        FECarRecord *fe_car = stable->GetCarByIndex(career->GetCurrentCar());
                        if (fe_car) {
                            FECareerRecord *fe_career = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
                            if (fe_career) {
                                Heat = UMath::Max(1.0f, fe_career->GetVehicleHeat());
                            }
                        }
                    }
                }
            }

            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            FECarRecord *record = stable->GetCarRecordByHandle(player_car);

            player_1.Key = record->VehicleKey;

            FECustomizationRecord *crecord = stable->GetCustomizationRecordByHandle(record->Customization);
            if (crecord) {
                player_1.Customize(*crecord);
            }
        }
    }

    CarBuilder player_2;

    player_2.Flags = 11;
    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
        if (!SkipFE) {
            player_2.Key = Attrib::StringToKey(FEDatabase->SplitScreenCarType);
            if (FEDatabase->SplitScreenCustomization) {
                player_2.Customize(*FEDatabase->SplitScreenCustomization);

                bFree(FEDatabase->SplitScreenCustomization);
            }
        } else {
            player_2.Key = Attrib::StringToKey(SkipFEPlayer2Car);
        }
    }

    CarViewer::haveLoadedOnce = false;

    ISimable *icar = ISimable::CreateInstance(
        "PVehicle", VehicleParams(this, DRIVER_HUMAN, player_1.Key, initialVec, initialPos, player_1.Flags, player_1.Customization,
                                  player_1.Performance));

    icar->Attach(IPlayer::First(PLAYER_LOCAL));

    IPerpetrator *ip;
    if (icar->QueryInterface(&ip)) {
        ip->SetHeat(Heat);
    }

    if (player_2.Key) {
        const float initialVelocity = 5.0f;
        UMath::ScaleAdd(initialVec, initialVelocity, initialPos, initialPos);

        ISimable *icar = ISimable::CreateInstance(
            "PVehicle", VehicleParams(this, DRIVER_HUMAN, player_2.Key, initialVec, initialPos, player_2.Flags, player_2.Customization,
                                      player_2.Performance));

        icar->Attach(IPlayer::Last(PLAYER_LOCAL));
    }
}

// ORDEN: las cuatro que siguen (OnManageTime, OnBeginState, IsStateDone,
// CanSimulate) van en ESTE orden porque es el del objetivo -- 0xF630, 0xF864,
// 0xFA70, 0xFDDC. No las reordenes: el `.text` sale del mismo tamano y el DOL
// se rompe sin que objdiff diga nada (`scripts/textorder.py zSim`).
float QuickGame::OnManageTime(float real_time_delta, float sim_speed) {
    if (Tweak_GameSpeed != 1.0f) {
        return Tweak_GameSpeed;
    }

    float target_speed = 1.0f;
    float delta_speed;
    IPlayer *player1 = IPlayer::First(PLAYER_LOCAL);

    eView *view = eGetView(EVIEW_PLAYER1, false);
    Camera *camera = view ? view->GetCamera() : nullptr;
    float camera_time;
    if (camera) {
        camera_time = camera->GetSimTimeMultiplier();
        target_speed = camera_time;
    }

    if (Sim::IsSplitScreen()) {
        return 1.0f;
    }

    if (target_speed < 1.0f) {
        target_speed = UMath::Max(target_speed, 0.01f);
        delta_speed = 0.0f;
        mInGameBreaker = false;
    } else if (INIS::Exists()) {
        mInGameBreaker = false;
        target_speed = 1.0f;
        delta_speed = 0.0f;
    } else if (player1 && player1->InGameBreaker()) {
        mInGameBreaker = true;
        target_speed = 0.25f;
        delta_speed = 2.0f;
    } else if (mInGameBreaker && sim_speed < 1.0f) {
        target_speed = 1.0f;
        delta_speed = 0.5f;
    } else {
        target_speed = 1.0f;
        delta_speed = 0.0f;
        mInGameBreaker = false;
    }

    if (target_speed != sim_speed) {
        if (delta_speed > 0.0f) {
            if (sim_speed < target_speed) {
                sim_speed = UMath::Min(sim_speed + delta_speed * real_time_delta, target_speed);
            } else {
                sim_speed = UMath::Max(sim_speed - delta_speed * real_time_delta, target_speed);
            }
        } else {
            sim_speed = target_speed;
        }
    }

    return sim_speed;
}

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
            Sim::IActivity::CreateInstance("AvoidableManager", Sim::Param());
            Sim::IActivity::CreateInstance("AICopManager", Sim::Param());
            Sim::IActivity::CreateInstance("AITrafficManager", Sim::Param());
            Sim::IActivity::CreateInstance("AIParkedCarSpawner", Sim::Param());
            Sim::IActivity::CreateInstance("PathFinder", Sim::Param());
            Sim::IActivity::CreateInstance("GameplayActivity", Sim::Param());
            Sim::IActivity::CreateInstance("Gps", Sim::Param());
            Sim::IActivity::CreateInstance("NISListenerActivity", Sim::Param());
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

bool QuickGame::IsStateDone() const {
    if (mState == RACESTATE_LOADING_WORLD) {
        if (TheTrackStreamer.IsLoadingInProgress()) {
            return false;
        }
    } else if (mState == RACESTATE_LOADING_PLAYERS) {
    } else if (mState == RACESTATE_LOADING_PLAYER_AUDIO) {
        const IVehicle::List &vehicles = IVehicle::GetList(VEHICLE_PLAYERS);
        for (IVehicle::List::const_iterator i = vehicles.begin(); i != vehicles.end(); ++i) {
            IVehicle *ivehicle = *i;

            IAudible *iaudible;
            if (ivehicle && ivehicle->QueryInterface(&iaudible)) {
                if (!iaudible->IsAudible()) {
                    return false;
                }
            }
        }
    } else if (mState == RACESTATE_LOADING_PLAYER_MODELS) {
        const IVehicle::List &vehicles = IVehicle::GetList(VEHICLE_PLAYERS);
        for (IVehicle::List::const_iterator i = vehicles.begin(); i != vehicles.end(); ++i) {
            IVehicle *ivehicle = *i;

            IRenderable *irenderable;
            if (ivehicle && ivehicle->QueryInterface(&irenderable)) {
                if (!irenderable->IsRenderable()) {
                    return false;
                }
            }
        }
    } else if (mState == RACESTATE_LOADING_ACTIVITIES) {
    } else if (mState == RACESTATE_LOADING_UI) {
        for (IHud::List::const_iterator i = IHud::GetList().begin(); i != IHud::GetList().end(); ++i) {
            IHud *ihud = *i;
            if (!ihud->AreResourcesLoaded()) {
                return false;
            }
        }
    } else if (mState == RACESTATE_LOADING_OPPONENT_AUDIO) {
        if (GRaceStatus::Exists() && GRaceStatus::Get().IsAudioLoading()) {
            return false;
        }
    } else if (mState == RACESTATE_LOADING_OPPONENT_MODELS) {
        if (GRaceStatus::Exists() && GRaceStatus::Get().IsModelsLoading()) {
            return false;
        }

        GRaceCustom *startupRace = GRaceDatabase::Get().GetStartupRace();
        if (!startupRace) {
            if (GManager::Get().GetStartFreeRoamPursuit()) {
                ICopMgr::Get()->LockoutCops(false);
                ICopMgr::Get()->PursueAtHeatLevel(static_cast<int>(GManager::Get().GetQueuedPursuitMinHeat()));

                if (!ICopMgr::Get()->PlayerPursuitHasCop()) {
                    return false;
                }

                GManager::Get().ClearFreeRoamPursuit();
            }
        }
    } else if (mState == RACESTATE_LOADING_WAIT_FOR_USER_INPUT) {
        if (!LoadingTips::IsDoneShowingLoadingTips()) {
            LoadingTips::SetDoneLoading(true);
            return false;
        }
    } else if (mState == RACESTATE_LOADING_NIS) {
        if (INIS::Exists() && !INIS::Get()->IsLoaded()) {
            return false;
        }
    }

    return true;
}

bool QuickGame::CanSimulate() {
    if (TrackCopCameraMover_IdleSim) {
        return false;
    }

    if (InGameAnyMovieScreen::IsPlaying()) {
        return false;
    }

    if (INIS::Exists() && (!INIS::Get()->IsLoaded() || INIS::Get()->InMovie())) {
        return false;
    }

    if (GRaceStatus::Exists() && GRaceStatus::Get().IsLoading()) {
        return false;
    }

    if (GManager::Exists() && GManager::Get().GetIsWarping()) {
        return false;
    }

    if (FEManager::Get() && FEManager::ShouldPauseSimulation(true)) {
        return false;
    }

    return true;
}

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
            GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_QuickRace);
            GRaceDatabase::Get().FreeCustomRace(race);
        }
    }
}

BIND_ACTIVITY_FACTORY(QuickGame)
