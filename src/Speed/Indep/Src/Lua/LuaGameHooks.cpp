#include "Speed/Indep/Src/Lua/LuaGameHooks.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/AI/AIBasics.hpp"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Gameplay/GCharacter.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GMarker.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GSpeedTrap.h"
#include "Speed/Indep/Src/Gameplay/GTrigger.h"
#include "Speed/Indep/Src/Generated/Events/ECinematicMoment.hpp"
#include "Speed/Indep/Src/Generated/Events/EPlayRaceMovie.hpp"
#include "Speed/Indep/Src/Generated/Events/EPlayRaceNIS.hpp"
#include "Speed/Indep/Src/Generated/Events/EShowTimeExtension.hpp"
#include "Speed/Indep/Src/Generated/Events/EEnterEngagableTrigger.hpp"
#include "Speed/Indep/Src/Generated/Events/EExitEngagableTrigger.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Events/EResetProps.hpp"
#include "Speed/Indep/Src/Generated/Events/EKnockoutRacer.hpp"
#include "Speed/Indep/Src/Generated/Events/EPause.hpp"
#include "Speed/Indep/Src/Generated/Events/EQuitToFE.hpp"
#include "Speed/Indep/Src/Generated/Events/EReloadGame.hpp"
#include "Speed/Indep/Src/Generated/Events/EShowRaceOverMessage.hpp"
#include "Speed/Indep/Src/Generated/Events/EAwardUpgrade.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FeFadeScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/PhotoFinish.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Generated/Events/ECommitAudioAssets.hpp"
#include "Speed/Indep/Src/Generated/Events/ECommitRenderAssets.hpp"
#include "Speed/Indep/Src/Generated/Events/ESndGameState.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
class EAX_CarState;
#include "Speed/Indep/Src/Interfaces/Simables/IEngineDamage.h"
#include "Speed/Indep/Src/Generated/Messages/MNISComplete.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeedTrap.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyRacePlacement.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyKnockedOut.h"
#include "Speed/Indep/Src/Generated/Messages/MForcePursuitStart.h"
#include "Speed/Indep/Src/Generated/Messages/MSetCopsEnabled.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyPlayerRep.h"
#include "Speed/Indep/Src/Generated/Messages/MJackKnife.h"
#include "Speed/Indep/Src/Generated/Messages/MSetTrafficSpeed.h"
#include "Speed/Indep/Src/Generated/Messages/MAIEngineRev.h"
#include "Speed/Indep/Src/Generated/Messages/MLoadingComplete.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyChallengePassed.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyFinished.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"

#include "Speed/Indep/Src/Generated/Messages/MNotifyMovieFinished.h"

#include "Speed/Indep/Src/Generated/Messages/MEnterFreeRoam.h"
#include "Speed/Indep/Src/Generated/Messages/MFlowReadyForOutro.h"

#include "Speed/Indep/Src/Generated/Events/EShowMilestones.hpp"
#include "Speed/Indep/Src/Generated/Events/EShowResults.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/IMenuZoneTrigger.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficMgr.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSoundEnums.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IOnlinePlayer.h"
#include "Speed/Indep/Src/Online/InGame/OnlineRacer.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/Main.hpp"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/Src/World/VisualTreatment.h"
#include "Speed/Indep/Src/World/TimeOfDay.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyMovieScreen.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

bool GPS_Engage(const UMath::Vector3 &target, float maxDeviation);

class WorldMap {
  public:
    static void ClearGPSing();

    static void SetGPSing(GIcon *icon);
};

static inline bool IsCareerRaceUnlocked(GRaceDatabase &db, unsigned int eventHash) {
    return db.CheckRaceScoreFlags(eventHash, GRaceDatabase::kUnlocked_Career);
}

static inline bool IsSinglePlayer() {
    return Sim::GetUserMode() == Sim::USER_SINGLE;
}

// Attrib::Gen::gameplay::ClassKey() es inline en la cabecera generada (compartida
// con otras unidades), pero en el original era una funcion out-of-line a la que
// zLua llamaba. Se declara por su nombre mangled para forzar el bl exacto.
extern "C" unsigned int ClassKey__Q36Attrib3Gen8gameplay();

// Functor de IVehicle::ForEach que acumula el maximo rendimiento de los jugadores.
struct PerfMaximizer {
    PerfMaximizer(float minPerformance)
        : Performance(minPerformance, minPerformance, minPerformance) {}

    void operator()(IVehicle *vehicle) {
        Physics::Info::Performance p;

        if (vehicle->GetPerformance(p)) {
            Performance.Maximize(p);
        }
    }

    Physics::Info::Performance Performance; // offset 0x0, size 0xC
};

// El vector de shuffle del original lleva este tag propio de contenedor
// (visible en el mangled de reserve/push_back del DWARF).
DECLARE_CONTAINER_TYPE(ShuffleVector)

// Physics::Info::FindPerformanceCandidates vive en la unidad zPhysics
// (PhysicsInfo.cpp); se declara aqui para reproducir el bl exacto.
namespace Physics {
namespace Info {
void FindPerformanceCandidates(const Performance &minimum_perf, const Performance &maximum_perf,
                               UTL::Std::list<unsigned int, _type_list> &vlist);
}
}

// Ajustes de stable al ganar evento: viven en otras unidades (zSpeech/zFe).

void AdjustStableImpound_EventWin(int playerNum);

// Attrib::StringToLowerCaseKey (Main/AttribSupport.h): se declara aqui para no
// arrastrar esa cabecera.
namespace Attrib {
unsigned int StringToLowerCaseKey(const char *str);
}

// gGPSDestination es LOCAL de esta unidad: el objetivo lo emite como simbolo
// `local` de 12 B en el .bss de zLua (asm zLua.s, .obj gGPSDestination, local).
static UMath::Vector3 gGPSDestination;

void Activity_Run(GRuntimeInstance *activityInstance) {
    GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(reinterpret_cast<GActivity *>(activityInstance));

    if (parms != NULL) {
        parms->BlockUntilLoaded();
    }
    reinterpret_cast<GActivity *>(activityInstance)->Reset();
    reinterpret_cast<GActivity *>(activityInstance)->Run();
}

void Activity_Suspend(GRuntimeInstance *activityInstance) {
    reinterpret_cast<GActivity *>(activityInstance)->Suspend();
}

void Audio_SetFlag(const char *flagName, bool value) {
    static struct {
        const char *mFlagName;
        int mSndEnum;
    } flagMapping[9] = {
        {"eNIS_Start", 2},   {"eNIS_321GO", 3},     {"eNIS_Busted", 4},          {"eNIS_EndofRace", 5},
        {"eFE_PostRaceScreen", 6}, {"eFE_CameraScreen", 7}, {"eLOAD_GeneralLoading", 0xa},
        {"eLOAD_BustedLoading", 0xb}, {"eMAX_GameFlowStates", 0xf},
    };

    for (int onFlag = 0; onFlag < 9; onFlag++) {
        if (bStrCmp(flagName, flagMapping[onFlag].mFlagName) == 0) {
            new ESndGameState(flagMapping[onFlag].mSndEnum, value);

            switch (flagMapping[onFlag].mSndEnum) {
            case 0:
                SetSoundControlState(value, SNDSTATE_ERROR, "LuaCall:error");
                break;
            case 1:
            case 2:
            case 3:
            case 4:
                break;
            case 5:
                SetSoundControlState(value, SNDSTATE_NIS_INTRO, "LuaCall:end of race");
                break;
            case 6:
                break;
            case 7:
                SetSoundControlState(value, SNDSTATE_PAUSE, "LuaCall:fe cam");
                break;
            case 8:
                SetSoundControlState(value, SNDSTATE_PAUSE, "LuaCall:fe cam");
                break;
            case 9:
                break;
            case 10:
                SetSoundControlState(value, SNDSTATE_OFF, "LuaCall:loading");
                break;
            case 11:
                SetSoundControlState(value, SNDSTATE_MINILOAD, "LuaCall:bust load");
                break;
            case 12:
                break;
            case 13:
                SetSoundControlState(value, SNDSTATE_FE_SMS_MESSAGE, "LuaCall:sms");
                break;
            case 14:
                SetSoundControlState(value, SNDSTATE_FE, "LuaCall:raceover");
                break;
            }
        }
    }
}

bool Audio_IsCopSpeechPlaying() {
    return Speech::Manager::IsPlaying(COPSPEECH_MODULE);
}

int Bin_GetNumChallengesPassed(int binIndex) {
    GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(binIndex);

    if (bin != NULL) {
        return bin->GetCompletedChallenges();
    }
    return 0;
}

int Bin_GetNumRacesWon(int binIndex) {
    GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(binIndex);

    if (bin != NULL) {
        return bin->GetAwardedRaceWins();
    }
    return 0;
}

void Camera_SetGenericCamera(const char *groupName, const char *trackName) {
    new ECinematicMoment(groupName, trackName, 0.0f);

    if (bStrCmp(groupName, "Cinematics") == 0) {
        int isChallenge = bStrCmp(trackName, "Challenge");

        if (isChallenge == 0) {
            MGamePlayMoment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, isChallenge, 0x76221F3D)
                .Send(UCrc32("MomentStrm"));
        }
    }
}

void MiniMap_AddEngagedRace(GRuntimeInstance *engageTriggerInstance) {
    const UMath::Vector3 &position = engageTriggerInstance->Position();
}

void NIS_Play(GRuntimeInstance *startMarker, const char *animName, const char *animType, int cameraTrack,
              const char *preMovie, const char *postMovie) {
    static volatile bool kDisableNIS = false;

    if (kDisableNIS != 0 || animName == NULL || animName[0] == '\0') {
        MNISComplete(animName).Post(UCrc32(0x20D60DBF));
    } else {
        new EPlayRaceNIS(reinterpret_cast<GMarker *>(startMarker), animName, animType, 0, cameraTrack, preMovie, postMovie);
    }
}

void Movie_PlayHackE3FMV() {}

static volatile bool kPrintScriptMessages = false;

void Debug_Print(const char *message) {
    if (kPrintScriptMessages) {
    }
}

void Debug_PrintInstance(GRuntimeInstance *inst) {
    if (kPrintScriptMessages) {
    }
}

void Demo_SetRaceCompleteForFE(GRuntimeInstance *activityInstance) {}

void Demo_StorePursuitRepForFE(GRuntimeInstance *activityInstance) {}

void HUD_ShowMessage(const char *languageString) {
    if (languageString != NULL && languageString[0] != '\0') {
        const char *redundantPrefix = "LANGUAGE_";

        if (bStrNCmp(languageString, redundantPrefix, bStrLen(redundantPrefix)) == 0) {
            languageString += bStrLen(redundantPrefix);
        }

        unsigned int languageHash = bStringHash(languageString);

        if (IPlayer::First(PLAYER_LOCAL)->GetHud() != NULL) {
            IGenericMessage *igenericmessage;

            if (IPlayer::First(PLAYER_LOCAL)->GetHud()->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(GetTranslatedString(languageHash), false, FEHASH_ZOOMIN, 0, 0, GenericMessage_Priority_1);
            }
        }
    }
}

void HUD_ShowTimeExtension(float seconds) {
    new EShowTimeExtension(IPlayer::First(static_cast<ePlayerList>(1)), seconds);
}

int Math_RandomInt(int range) {
    return bRandom(range) + 1;
}

bool Platform_IsNextGen() {
    return false;
}

float Game_GetSimTime() {
    return Sim::GetTime();
}

void Game_AddPlayer(IVehicle *playerVehicle) {
    ISimable *racerSimable = playerVehicle->GetSimable();

    if (racerSimable != NULL) {
        IPlayer *player = racerSimable->GetPlayer();

        if (player != NULL) {
            IPerpetrator *iperp;
            IOnlinePlayer *online;

            GRacerInfo &info = GRaceStatus::Get().AddSimablePlayer(racerSimable);

            if (racerSimable->QueryInterface(&iperp)) {
                iperp->SetRacerInfo(&info);
            }
            if (player->QueryInterface(&online)) {
                info.SetName(online->GetOnlineRacer()->GetPersona());
            }
        }
    }
}

bool Game_FindPerformanceCandidates(UTL::Std::list<unsigned int, _type_list> &candidates, unsigned int limit,
                                    const Physics::Info::Performance &perf) {
    unsigned int i;

    Physics::Info::Performance performance(perf);
    performance.Grow(0.2f);

    candidates.clear();

    Physics::Info::Performance bottom(0.0f, 0.0f, 0.0f);
    Physics::Info::Performance top(performance);

    UTL::Std::list<unsigned int, _type_list> keys;
    Physics::Info::FindPerformanceCandidates(bottom, top, keys);

    if (keys.size() == 0) {
        return false;
    }

    UTL::Std::vector<unsigned int, _type_ShuffleVector> shuffle;
    shuffle.reserve(keys.size());

    for (UTL::Std::list<unsigned int, _type_list>::iterator iter = keys.begin(); iter != keys.end(); iter++) {
        unsigned int key = *iter;

        Attrib::Gen::pvehicle pvehicle(key, 0, NULL);

        if (pvehicle.IsValid() && pvehicle.RandomOpponent()) {
            shuffle.push_back(key);
        }
    }

    if (shuffle.size() == 0) {
        return false;
    }

    for (i = 0; i < shuffle.size(); i++) {
        unsigned int dest = bRandom((int)shuffle.size());

        std::swap(shuffle[i], shuffle[dest]);
    }

    for (i = 0; i < shuffle.size() && i < limit; i++) {
        candidates.push_back(shuffle[i]);
    }

    return !candidates.empty();
}

unsigned int Game_MaxUniqueOpponents() {
    return 2;
}

void Game_SetSplitGrid() {
    GRaceParameters *race_parameters = GRaceStatus::Get().GetRaceParameters();

    UMath::Vector3 position;
    UMath::Vector3 direction;

    race_parameters->GetStartPosition(position);
    race_parameters->GetStartDirection(direction);

    WRoadNav nav;

    nav.SetNavType(WRoadNav::kTypeDirection);
    nav.SetPathType(WRoadNav::kPathPlayer);
    nav.SetLaneType(WRoadNav::kLaneStartingGrid);

    int num_lanes_left = 0;
    int num_lanes_right = 0;

    nav.InitAtPoint(position, direction, false, 1.0f);
    nav.SnapToSelectableLane();

    while (nav.IncLane(-1)) {
        num_lanes_left++;
    }

    nav.InitAtPoint(position, direction, false, 1.0f);
    nav.SnapToSelectableLane();

    while (nav.IncLane(1)) {
        num_lanes_right++;
    }

    int num_lanes_center = num_lanes_right + 1;
    int num_lanes = num_lanes_left + num_lanes_center;
    bool odd_lanes = num_lanes & 1;

    for (int i = 0; i < 2; i++) {
        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(i);
        bool left_player = i == 0;

        ISimable *isimable = racerInfo.GetSimable();

        IVehicle *vehicle = NULL;

        if (isimable != NULL) {
            vehicle = UTL::COM::QueryInterface<IVehicle>(isimable);
        }

        nav.InitAtPoint(position, direction, false, 1.0f);
        nav.SnapToSelectableLane();

        UMath::Vector3 spawnDir;

        Unit(nav.GetForwardVector(), spawnDir);

        if (odd_lanes) {
            if (left_player) {
                if (num_lanes_left != 0) {
                    nav.IncLane(-1);
                }
            } else {
                nav.IncLane(1);

                if (num_lanes_left == 0) {
                    nav.IncLane(1);
                }
            }
        } else {
            if (left_player) {
                if (num_lanes_left > num_lanes_right) {
                    nav.IncLane(-1);
                }
            } else {
                if (num_lanes_left < num_lanes_right) {
                    nav.IncLane(1);
                }
            }
        }

        UMath::Vector3 spawn_pos = isimable->GetRigidBody()->GetDimension();
        float length = spawn_pos.z;
        spawn_pos = nav.GetPosition();
        UMath::ScaleAdd(spawnDir, -length, spawn_pos, spawn_pos);

        vehicle->SetVehicleOnGround(spawn_pos, spawnDir);
        racerInfo.ForceStartPosition(spawn_pos, spawnDir);
    }
}

void Game_InitRacers(GRuntimeInstance *startMarker) {
    GRaceStatus::Get().ClearRacers();
    GRaceStatus::Get().SyncronizeAdaptiveBonus();
    GManager::Get().ClearStockCars();
    WRoadNetwork::Get().ResetShortcuts();

    if (ICopMgr::Exists()) {
        ICopMgr::Get()->ResetCopsForRestart(true);
    }

    if (ITrafficMgr::Exists()) {
        ITrafficMgr::Get()->FlushAllTraffic(true);
    }

    IVehicle::ForEach(VEHICLE_PLAYERS, Game_AddPlayer);

    GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
    float minimum_ai_performance = 0.0f;
    int num_spawned;

    if (IsSinglePlayer() && parms->GetRaceType() != GRace::kRaceType_Tollbooth) {
        int numOpponents = parms->GetNumOpponents();
        int numRacers = GRaceStatus::Get().GetRacerCount() - 1;
        unsigned int padRacers = numOpponents - numRacers;

        for (unsigned int onOppAI = 0; onOppAI < padRacers; onOppAI++) {
            GCharacter *opponentChar = parms->GetOpponentChar(onOppAI);

            GRaceStatus::Get().AddRacer(opponentChar);

            if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
                minimum_ai_performance = UMath::Max(opponentChar->MinimumAIPerformance(), minimum_ai_performance);
            }
        }
    }

    GMarker *marker = (GMarker *)startMarker;
    const UMath::Vector3 &markerPos = marker->GetPosition();
    const UMath::Vector3 &markerDir = marker->GetDirection();

    UMath::Vector3 markerDirBackwards;
    bScale((bVector3 *)&markerDirBackwards, (const bVector3 *)&markerDir, -1.0f);

    WRoadNav gridNav;
    gridNav.SetNavType(WRoadNav::kTypeDirection);
    const bool force_centre_lane = false;
    const float dir_weight = 1.0f;
    gridNav.InitAtPoint(markerPos, markerDirBackwards, force_centre_lane, dir_weight);

    IVehicleCache *cache = &GRaceStatus::Get();
    GRaceStatus::Get().LockVehicleCache(true);

    unsigned int num_unique_opponents = Game_MaxUniqueOpponents();

    Physics::Info::Performance matched_performance =
        IVehicle::ForEach(VEHICLE_PLAYERS, PerfMaximizer(minimum_ai_performance)).Performance;

    UTL::Std::list<unsigned int, _type_list> matched_vehicles;
    Game_FindPerformanceCandidates(matched_vehicles, num_unique_opponents, matched_performance);

    UTL::Std::list<unsigned int, _type_list>::iterator match_iter = matched_vehicles.begin();

    const float carSpacing = 7.0f;
    float carLateralStagger = 1.5f;

    for (int onRacer = 0; onRacer < GRaceStatus::Get().GetRacerCount(); onRacer++) {
        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(onRacer);

        GCharacter *racerChar = racerInfo.GetGameCharacter();

        if (racerChar == NULL) {
            continue;
        }

        UMath::Vector3 spawnPos;
        UMath::Vector3 spawnDir;

        const GCollectionKey &startMarkerSpec = racerChar->ForceStartPosition();

        if (unsigned int matched_key = startMarkerSpec.GetCollectionKey()) {
            Attrib::Gen::gameplay startMarkerCollection(
                Attrib::FindCollection(ClassKey__Q36Attrib3Gen8gameplay(), matched_key), 0, NULL);

            const UMath::Vector3 &posSwizzled = startMarkerCollection.Position();

            UMath::Matrix4 rotMat = UMath::Matrix4::kIdentity;
            UMath::Vector3 initialVec = {0.0f, 0.0f, 1.0f};

            UMath::Init(rotMat, 1.0f, 1.0f, 1.0f);
            MATRIX4_multyrot(&rotMat, -startMarkerCollection.Rotation() / 360.0f, &rotMat);
            UMath::Rotate(initialVec, rotMat, initialVec);

            spawnPos = UMath::Vector3Make(-posSwizzled.y, posSwizzled.z, posSwizzled.x);
            spawnDir = initialVec;
        } else {
            gridNav.IncNavPosition(carSpacing, markerDirBackwards, 0.0f);

            spawnPos = gridNav.GetPosition();

            bScale((bVector3 *)&spawnDir, (const bVector3 *)&gridNav.GetForwardVector(), -1.0f);

            UMath::Vector3 upVec = {0.0f, 1.0f, 0.0f};

            bVector3 lateralVec;
            bCross(&lateralVec, (const bVector3 *)&upVec, (const bVector3 *)&spawnDir);
            bScale(&lateralVec, &lateralVec, carLateralStagger);
            bAdd((bVector3 *)&spawnPos, (const bVector3 *)&spawnPos, &lateralVec);
        }

        unsigned int matched_key = 0;

        if (!matched_vehicles.empty()) {
            if (match_iter == matched_vehicles.end()) {
                match_iter = matched_vehicles.begin();
            }

            matched_key = *match_iter;
            ++match_iter;
        }

        {
            IVehicle *vehicle = racerInfo.CreateVehicle(matched_key);

            if (vehicle != NULL) {
                vehicle->SetVehicleOnGround(spawnPos, spawnDir);

                IPerpetrator *iperp;

                if (vehicle->QueryInterface(&iperp)) {
                    iperp->SetRacerInfo(&racerInfo);
                }
            }
        }

        carLateralStagger = -carLateralStagger;
    }

    GRaceParameters *raceParms = GRaceStatus::Get().GetRaceParameters();

    GRaceStatus::Get().LockVehicleCache(false);

    GActivity *raceActivity = raceParms->GetActivity();
    GManager::Get().PreloadStockCarsForActivity(raceActivity);

    const char *copType = GRaceStatus::Get().GetRaceParameters()->GetGameplayObj()->CopSpawnType();

    if (copType != NULL && *copType != 0) {
        GManager::Get().ReserveStockCar(copType);
    }

    if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        new ECommitAudioAssets();
        new ECommitRenderAssets();
    }

    for (IPlayer *const *iter = IPlayer::GetList(PLAYER_LOCAL).begin();
         iter != IPlayer::GetList(PLAYER_LOCAL).end(); iter++) {
        (*iter)->SetHud(PHT_NONE);
    }

    g_pEAXSound->StartNewGamePlay();
}

void Game_KnockoutRacer(ISimable *simable) {
    if (simable == NULL) {
        return;
    }

    int racerIndexOneBased = Game_GetRacerIndex(simable);
    int racerIndex = racerIndexOneBased - 1;

    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(racerIndex);

    racerInfo.KnockOut();

    ISimable *racerSimable = racerInfo.GetSimable();

    if (racerSimable != NULL) {
        MNotifyKnockedOut(racerSimable->GetInstanceHandle()).Post(UCrc32(0x20D60DBF));

        if (!racerSimable->IsPlayer()) {
            IVehicle *vehicle;

            if (racerSimable->QueryInterface(&vehicle)) {
                vehicle->Deactivate();
            }
        }
    }

    GRaceStatus::Get().UpdateAdaptiveDifficulty(GRaceStatus::kAdaptiveGain_FromRacerKOed, racerSimable);
}

void Game_DetachCameraFromRacer(ISimable *simable) {
    if (simable == NULL) {
        return;
    }

    int racerIndex = Game_GetRacerIndex(simable) - 1;

    GRaceStatus::Get().GetRacerInfo(racerIndex).mCameraDetached = true;
}

void Game_WarpPlayerToTrigger(GRuntimeInstance *startMarker) {
    GMarker *marker = static_cast<GMarker *>(static_cast<void *>(startMarker));
    const UMath::Vector3 &markerPos = marker->GetPosition();
    const UMath::Vector3 &markerDir = marker->GetDirection();

    TheTrackStreamer.EnableZoneSwitching();

    Sim::SetStream(markerPos, false);

    if (TheTrackStreamer.IsLoadingInProgress()) {
        new EFadeScreenOn(false);

        while (TheTrackStreamer.IsLoadingInProgress()) {
            MiniMainLoop();
        }
    }

    if (Sim::IsSplitScreen()) {
        Game_SetSplitGrid();
    } else {
        ISimable *player = IPlayer::First(static_cast<ePlayerList>(1))->GetSimable();
        IVehicle *vehicle;

        if (player->QueryInterface(&vehicle)) {
            vehicle->SetVehicleOnGround(markerPos, markerDir);
        }
    }
}

void Game_SetPlayerStartPosition(GRuntimeInstance *startMarker) {
    if (startMarker != NULL) {
        UMath::Vector3 markerPos = UMath::Vector3::kZero;
        UMath::Vector3 markerDir = UMath::Vector3::kZero;

        startMarker->GetPosition(markerPos);
        startMarker->GetDirection(markerDir);

        IPlayer *player = IPlayer::First(static_cast<ePlayerList>(1));
        ISimable *simable = player->GetSimable();

        GRacerInfo *info = GRaceStatus::Get().GetRacerInfo(simable);

        if (info != NULL) {
            info->ForceStartPosition(markerPos, markerDir);
        }
    }
}

void Game_ResetTrigger(GRuntimeInstance *triggerInstance) {
    if (triggerInstance != NULL) {
        static_cast<GTrigger *>(static_cast<void *>(triggerInstance))->Reset();
    }
}

void Game_ShowTriggerIcon(GRuntimeInstance *triggerInstance) {
    if (triggerInstance != NULL) {
        static_cast<GTrigger *>(static_cast<void *>(triggerInstance))->ShowIcon();
    }
}

void Game_HideTriggerIcon(GRuntimeInstance *triggerInstance) {
    if (triggerInstance != NULL) {
        static_cast<GTrigger *>(static_cast<void *>(triggerInstance))->HideIcon();
    }
}

void Game_SpawnCop(GRuntimeInstance *spawnMarker, const char *vehicleName, bool inPursuit, bool roadblock) {
    if (spawnMarker != NULL && vehicleName != NULL) {
        UMath::Vector3 markerPos;
        UMath::Vector3 markerDir;

        if (spawnMarker->GetPosition(markerPos) && spawnMarker->GetDirection(markerDir)) {
            ICopMgr::Get()->LockoutCops(false);
            ICopMgr::Get()->SpawnCop(markerPos, markerDir, vehicleName, inPursuit, roadblock);
        }
    }
}

void Game_SpawnCharacter(GRuntimeInstance *characterInst, GRuntimeInstance *spawnMarker, GRuntimeInstance *targetMarker,
                         float initialSpeed) {
    if (spawnMarker != NULL && characterInst != NULL) {
        UMath::Vector3 markerPos;
        UMath::Vector3 markerDir;

        if (spawnMarker->GetPosition(markerPos) && spawnMarker->GetDirection(markerDir)) {
            GCharacter *character = static_cast<GCharacter *>(static_cast<void *>(characterInst));
            GMarker *target = static_cast<GMarker *>(static_cast<void *>(targetMarker));

            character->Spawn(markerPos, markerDir, target, initialSpeed);
        }
    }
}

void Game_UnspawnCharacter(GRuntimeInstance *characterInstance) {
    static_cast<GCharacter *>(static_cast<void *>(characterInstance))->UnspawnWhenOffscreen();
}

void Game_SendCharacterStimulus(GRuntimeInstance *characterInstance, const char *stimulusName) {
    GCharacter *character = static_cast<GCharacter *>(static_cast<void *>(characterInstance));

    IVehicle *vehicle = character->GetSpawnedVehicle();

    if (vehicle != NULL) {
        IArticulatedVehicle *articulated;

        if (vehicle->QueryInterface(&articulated)) {
            if (articulated->GetTrailer() != NULL) {
                vehicle = articulated->GetTrailer();
            }
        }

        ISimable *simable;

        if (vehicle->QueryInterface(&simable)) {
            EventSequencer::IEngine *sequencer = simable->GetEventSequencer();

            if (sequencer != NULL) {
                sequencer->ProcessStimulus(UCrc32(stimulusName).GetValue(), Sim::GetTime(), NULL, EventSequencer::QUEUE_ALLOW);
            }
        }
    }
}

void Game_SetRacerLapsLeft(int racerIndex, int lapsLeft) {
    GRaceStatus::Get().GetRacerInfo(racerIndex - 1);
}

void Game_SetRacerGoal(int racerLuaIndex, GRuntimeInstance *goalTrigger) {
    if (goalTrigger != NULL) {
        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(racerLuaIndex - 1);
        ISimable *simable = racerInfo.GetSimable();

        if (simable != NULL) {
            IVehicleAI *vehicleAI;

            if (simable->QueryInterface(&vehicleAI)) {
                AITarget *target = vehicleAI->GetTarget();
                GTrigger *trigger = static_cast<GTrigger *>(static_cast<void *>(goalTrigger));
                UMath::Vector3 triggerPos;
                UMath::Vector3 triggerDir;

                trigger->GetPosition(triggerPos);
                triggerDir = trigger->GetDirection();

                target->Aquire(triggerPos, triggerDir);

                WRoadNav *road_nav = vehicleAI->GetDriveToNav();

                road_nav->CancelPathFinding();
            }

            IPlayer *player = simable->GetPlayer();

            if (player == IPlayer::First(static_cast<ePlayerList>(1))) {
                GRaceStatus::Get().SetNextCheckpointPos(goalTrigger);
            }
        }
    }
}

void Game_NotifyCheckpointReached(ISimable *simable, int checkpointIndex) {
    int racerIndex = Game_GetRacerIndex(simable) - 1;

    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(racerIndex);

    float checkpointTime = racerInfo.mCheckTimer.GetTime();

    GRaceStatus::Get().SetCheckpointTime(racerInfo.mLapsCompleted, checkpointIndex, racerIndex, checkpointTime);

    racerInfo.StartCheckpoint(checkpointIndex);
}

void Game_NotifyLapFinished(ISimable *simable, int lapIndexOneBased) {
    int racerIndex = Game_GetRacerIndex(simable) - 1;
    GRaceStatus &raceStatus = GRaceStatus::Get();
    GRacerInfo &racerInfo = raceStatus.GetRacerInfo(racerIndex);
    float lapTime = racerInfo.GetLapTime();

    raceStatus.SetLapTime(racerInfo.GetLapsCompleted(), racerIndex, lapTime);

    racerInfo.StartLap(lapIndexOneBased + 1);

    GRaceStatus::Get().CalculateRankings();

    if (GRaceStatus::IsKnockoutRace()) {
        int koPerLap = raceStatus.GetRaceParameters()->GetKnockoutsPerLap();
        int remaining = raceStatus.GetRaceParameters()->GetNumOpponents() - lapIndexOneBased * koPerLap + 1;
        int numRacersFinshedLap = 0;

        for (int racerIndex = 0; racerIndex < raceStatus.GetRacerCount(); racerIndex++) {
            GRacerInfo &localInfo = raceStatus.GetRacerInfo(racerIndex);

            if (localInfo.GetLapsCompleted() >= lapIndexOneBased) {
                numRacersFinshedLap++;
            }
        }

        if (numRacersFinshedLap == remaining) {
            for (int racerIndex = 0; racerIndex < raceStatus.GetRacerCount(); racerIndex++) {
                GRacerInfo &localInfo = raceStatus.GetRacerInfo(racerIndex);

                if (!localInfo.GetIsKnockedOut() && localInfo.GetLapsCompleted() < lapIndexOneBased) {
                    Game_KnockoutRacer(localInfo.GetSimable());

                    new EKnockoutRacer(&localInfo);
                }
            }
        }
    }
}

void Game_NotifyRaceFinished(ISimable *simable) {
    if (simable == NULL) {
        if (IPlayer::First(PLAYER_LOCAL) != NULL) {
            simable = IPlayer::First(PLAYER_LOCAL)->GetSimable();
        }
    }

    int racerIndexOneBased = Game_GetRacerIndex(simable);
    int racerIndex = racerIndexOneBased - 1;

    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(racerIndex);

    racerInfo.FinishRace();
    racerInfo.SetPctRaceComplete(100.0f);

    GRaceStatus::Get().CalculateRankings();

    if (simable->IsPlayer()) {
        if (Sim::IsSplitScreen() && racerInfo.GetGameCharacter() == NULL && !racerInfo.GetIsEngineBlown() &&
            !racerInfo.GetIsTotalled()) {
            IPlayer *player = simable->GetPlayer();

#ifndef EA_BUILD_A124
            if (player != NULL && player->IsLocal()) {
                new EShowRaceOverMessage(player);
            }
#endif
        }

        if (racerInfo.GetRanking() == 1) {
            unsigned int plrIndex = simable->GetPlayer()->GetSettingsIndex();

            if (plrIndex <= 1) {
                GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
                const Attrib::Gen::gameplay *dbItem = parms->GetGameplayObj();

                if (!GRaceStatus::Get().GetHasBeenWon()) {
                    GRaceStatus::Get().SetHasBeenWon(true);

                    bool doneBefore = GRaceDatabase::Get().IsCareerRaceComplete(parms->GetEventHash());

                    if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
                        UserProfile *prof = FEDatabase->GetUserProfile(plrIndex);
                        CareerSettings *career = prof->GetCareer();

                        career->AwardCash((int)GRaceStatus::Get().GetRaceParameters()->GetCashValue());

                        if (!doneBefore) {
                            for (unsigned int index = 0; index < dbItem->Num_RewardsForWinner(); index++) {
                                new EAwardUpgrade(dbItem->RewardsForWinner(index).GetCollectionKey());
                            }
                        }

                        AdjustStableHeat_EventWin(plrIndex);
                        AdjustStableImpound_EventWin(plrIndex);
                    }

                    if (!doneBefore) {
                        for (unsigned int unlockIndex = 0; unlockIndex < dbItem->Num_UnlockRaces(); unlockIndex++) {
                            unsigned int unlockKey = dbItem->UnlockRaces(unlockIndex).GetCollectionKey();

                            if (unlockKey != 0) {
                                GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromKey(unlockKey);

                                if (parms != NULL) {
                                    GRaceSaveInfo *info = GRaceDatabase::Get().GetScoreInfo(parms->GetEventHash());

                                    info->mFlags |= GRaceDatabase::kUnlocked_QuickRace;

                                    if (!parms->GetNeverInQuickRace()) {
                                        info->mFlags |= GRaceDatabase::kUnlocked_Career;
                                    }
                                }
                            }
                        }
                    }

                }

                GRaceDatabase::Get().UpdateRaceScore(true);
                GManager::Get().RefreshEngageTriggerIcons();
            }

            if (Sim::IsSplitScreen()) {
                racerInfo.ForceStop();
            }
        } else {
            racerInfo.ForceStop();
        }
    }

    IVehicleAI *ivehicleai;

    if (simable->QueryInterface(&ivehicleai)) {
        AITarget *target = ivehicleai->GetTarget();

        if (target != NULL) {
            target->Clear();
        }
    }

    GRaceStatus::Get().UpdateAdaptiveDifficulty(GRaceStatus::kAdaptiveGain_FromRacerFinished, simable);

    if (GRaceStatus::Exists()) {
        GRaceParameters *raceParams = GRaceStatus::Get().GetRaceParameters();

        if (raceParams != NULL && raceParams->GetIsPursuitRace() && bStrICmp(raceParams->GetEventID(), "19.8.69") == 0) {
            FEDatabase->GetCareerSettings()->SetHasBeatenSpecialChallengeEvent();
        }
    }

    GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(0x13);
    bool beaten = true;

    for (unsigned int i = 0; i < bin->GetWorldRaceCount(); i++) {
        unsigned int raceHash = bin->GetWorldRaceHash(i);
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromHash(raceHash);

        if (parms != NULL) {
            bool isBurgerKing = bStrCmp(parms->GetEventID(), GRaceDatabase::Get().GetBurgerKingRace()) == 0;

            if (!parms->GetIsCollectorsEditionRace() && !isBurgerKing) {
                if (!GRaceDatabase::Get().IsQuickRaceComplete(raceHash)) {
                    beaten = false;
                    break;
                }
            }
        }
    }

    if (beaten) {
        FEDatabase->GetCareerSettings()->SetHasBeatenChallengeSeries();
    }
}

int Game_GetRacerIndex(ISimable *simable) {
    for (int onRacer = 0; onRacer < GRaceStatus::Get().GetRacerCount(); onRacer++) {
        if (GRaceStatus::Get().GetRacerInfo(onRacer).GetSimable() == simable) {
            return onRacer + 1;
        }
    }
    return -1;
}

bool Game_RacerIsHuman(int racerIndex) {
    ISimable *simable = Game_GetRacerElement(racerIndex);
    IVehicle *vehicle;

    if (simable->QueryInterface(&vehicle)) {
        if (vehicle->GetDriverClass() == DRIVER_HUMAN) {
            return true;
        }
    }
    return false;
}

bool Game_PlayerIsLocal(ISimable *simable) {
    for (IPlayer::List::const_iterator onPlayer = IPlayer::GetList(PLAYER_LOCAL).begin();
         onPlayer != IPlayer::GetList(PLAYER_LOCAL).end(); ++onPlayer) {
        IPlayer *player = *onPlayer;

        if (player->GetSimable() == simable) {
            return true;
        }
    }
    return false;
}

ISimable *Game_GetRacerElement(int racerIndex) {
    return GRaceStatus::Get().GetRacerInfo(racerIndex - 1).GetSimable();
}

GRuntimeInstance *Game_GetRacerCharacter(int racerIndex) {
    return GRaceStatus::Get().GetRacerInfo(racerIndex - 1).GetGameCharacter();
}

int Game_GetNumRacers() {
    return GRaceStatus::Get().GetRacerCount();
}

float Game_GetSimableSpeedKmh(ISimable *simable) {
    if (simable != NULL) {

        IRigidBody *rigidBody = simable->GetRigidBody();

        if (rigidBody != NULL) {

            return rigidBody->GetSpeed() * 3600.0f / 1000.0f;
        }
    }
    return 0.0f;
}

bool Game_IsActiveSpeedTrap(GRuntimeInstance *speedTrapInstance) {

    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
        return false;
    }

    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    ISimable *simable = player != NULL ? player->GetSimable() : NULL;
    IVehicleAI *vehicleai = UTL::COM::QueryInterface<IVehicleAI>(simable);
    IPursuit *pursuit = vehicleai != NULL ? vehicleai->GetPursuit() : NULL;

    if (pursuit != NULL) {
        return false;
    }

    if (speedTrapInstance != NULL) {
        unsigned int trapKey = speedTrapInstance->GetCollection();

        GSpeedTrap *activeTrap = GManager::Get().GetFirstSpeedTrap(true, 0);

        while (activeTrap != NULL) {
            if (activeTrap->GetSpeedTrapKey() == trapKey) {
                return true;
            }

            activeTrap = GManager::Get().GetNextSpeedTrap(activeTrap, true, 0);
        }
    }

    return false;
}

bool Game_IsActiveMenuGate(GRuntimeInstance *menuGateInstance) {
    if (menuGateInstance == NULL) {
        return false;
    }

    if (menuGateInstance->GetFlag(0x8000) || menuGateInstance->GetFlag(0x2000) || menuGateInstance->GetFlag(0x4000)) {

        GTrigger *trigger = static_cast<GTrigger *>(static_cast<void *>(menuGateInstance));

        if (trigger->GetIcon() != NULL) {
            if (GManager::Get().GetIsIconVisible(trigger->GetIcon())) {
                return true;
            }
        }
    }

    return false;
}

void Game_NotifySpeedTrapTriggered(GRuntimeInstance *trapActivity, GRuntimeInstance *trapTrigger, ISimable *simable,
                                    float speedKmh) {
    float speed_mps;
    GSpeedTrap *speedTrap;
    unsigned int trapKey;

    MNotifySpeedTrap(trapActivity, simable->GetInstanceHandle(), speedKmh).Post(UCrc32(0x20D60DBF));

    GManager::Get().TrackValue("speedtrap_speed", speedKmh);

    speed_mps = speedKmh * 0.27778f;

    speedTrap = GManager::Get().GetFirstSpeedTrap(true, 0);
    trapKey = trapTrigger->GetCollection();

    while (speedTrap != NULL) {
        if (speedTrap->GetSpeedTrapKey() == trapKey) {
            speedTrap->NotifyTriggered(speed_mps);
            break;
        }

        speedTrap = GManager::Get().GetNextSpeedTrap(speedTrap, true, 0);
    }

    if (GRaceStatus::Exists()) {
        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
            FE_ShowSpeedTrapScreen(speedKmh, speedTrap->GetBounty());

            Game_ChallengeCompleted();
        } else {
            GRacerInfo *racerInfo = GRaceStatus::Get().GetRacerInfo(simable);

            if (racerInfo != NULL) {
                racerInfo->NotifySpeedTrapTriggered(speedKmh * 0.27778f);
            }

            IPlayer *player = simable->GetPlayer();

            if (player != NULL) {
                IGenericMessage *igenericmessage;

                    if (player->GetHud()->QueryInterface(&igenericmessage)) {
                        unsigned int labelHash;
                        float displaySpeed;
                        char speedString[64];

                        if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 0) {
                            labelHash = 0x8569AB44;
                            displaySpeed = MPS2MPH(speed_mps);
                        } else {
                            labelHash = 0x8569A25F;
                            displaySpeed = MPS2KPH(speed_mps);
                        }

                        bSPrintf(speedString, "+%0.0f %s", displaySpeed, GetLocalizedString(labelHash));

                    igenericmessage->RequestGenericMessage(speedString, false, FEHASH_ZOOMIN, bStringHash("SPEEDTRAP_FLASHER_ICON"),
                                                           FEHASH_TIMEBONUS, GenericMessage_Priority_3);
                }
            }
        }
    }
}

void Game_NotifyRacePlacement(GRuntimeInstance *raceInstance, ISimable *simable, int placement) {
    MNotifyRacePlacement(static_cast<GCollectionKey>(raceInstance), simable->GetInstanceHandle(), placement).Post(UCrc32(0x20D60DBF));
}

void Game_SaveStartPositions() {
    for (int onRacer = 0; onRacer < GRaceStatus::Get().GetRacerCount(); onRacer++) {

        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(onRacer);
        racerInfo.SaveStartPosition();
    }
}

void Game_RestoreStartPositions() {
    if (ITrafficMgr::Get() != NULL) {
        ITrafficMgr::Get()->FlushAllTraffic(false);
    }

    for (int onRacer = 0; onRacer < GRaceStatus::Get().GetRacerCount(); onRacer++) {
        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(onRacer);

        racerInfo.RestoreStartPosition();
        racerInfo.ClearRaceStats();
    }
}

void Game_SetRaceActivity(GRuntimeInstance *activityInstance) {
    GRaceStatus::Get().SetRaceActivity(reinterpret_cast<GActivity *>(activityInstance));
}

void Game_StartRace(GRuntimeInstance *raceActivity) {
    GActivity *activity = static_cast<GActivity *>(raceActivity);

    FEngHud::bIsRestartingRace = false;

    GRaceStatus::Get().SetRacing();
    GManager::Get().UnspawnAllCharacters();
    FESoundControl(false, "Game_StartRace");
    GPS_Disengage();

    int density = GRaceStatus::Get().GetRaceParameters()->GetTrafficDensity();

    if (activity->ForceTrafficDensity() > 0) {
        density = activity->ForceTrafficDensity();
    }

    if (GRaceStatus::IsDragRace()) {
        GRaceStatus::Get().SetTrafficDensity(0);
    } else {
        GRaceStatus::Get().SetTrafficDensity(density);
    }

    const char *traffic_pattern = GRaceStatus::Get().GetRaceParameters()->GetTrafficPattern();

    if (traffic_pattern != NULL && traffic_pattern[0] != 0) {
        GRaceStatus::Get().SetTrafficPattern(Attrib::StringToLowerCaseKey(traffic_pattern));
    } else {
        GRaceStatus::Get().ClearTrafficPattern();
    }

    if (bStrCmp("e3demo", GRaceStatus::Get().GetRaceParameters()->GetEventID()) == 0) {
        RedoTopologyAndSceneryGroups();
    }

    GRaceStatus::Get().EnableBarriers();

    new EResetProps();

    GRaceStatus::Get().DetermineRaceLength();
    GRaceStatus::Get().MakeCatchUpData();

    for (int onRacer = 0; onRacer < GRaceStatus::Get().GetRacerCount(); onRacer++) {
        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(onRacer);
        ISimable *simable = racerInfo.GetSimable();

        if (simable != NULL) {
            DriverStyle style = GRaceStatus::Get().GetRaceType() == GRace::kRaceType_Drag ? STYLE_DRAG : STYLE_RACING;

            IRacer *iracer;

            if (simable->QueryInterface(&iracer)) {
                iracer->StartRace(style);
            }

            IPerpetrator *iperp;

            if (simable->QueryInterface(&iperp)) {
                iperp->SetHeat(bMax(iperp->GetHeat(), (float)activity->ForceHeatLevel()));
            }
        }
    }

    if (FadeScreen::IsFadeScreenOn()) {
        new EFadeScreenOff(0x161a918);
    }
}

void Game_StartRaceTimers() {
    GRaceStatus::Get().StartMasterTimer();

    for (int onRacer = 0; onRacer < GRaceStatus::Get().GetRacerCount(); onRacer++) {

        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(onRacer);
        racerInfo.StartRace();
    }
}

void Game_AbandonRace() {
    GRaceStatus::Get().StopMasterTimer();

    GRaceStatus::Get().SetNextCheckpointPos(NULL);
    GRaceStatus::Get().ClearCheckpoints();

    if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {

        GManager::Get().UnspawnAllCharacters();
        GManager::Get().ClearStockCars();
    }

    for (int onRacer = 0; onRacer < GRaceStatus::Get().GetRacerCount(); onRacer++) {

        ISimable *simable = GRaceStatus::Get().GetRacerInfo(onRacer).GetSimable();
        IRacer *racer;

        if (simable != NULL && simable->QueryInterface(&racer)) {

            racer->QuitRace();
        }
    }
}

void Game_EnterPostRaceFlow() {
    GRaceStatus::Get().FinalizeRaceStats();
    Game_AbandonRace();

    if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
        GRacerInfo *info = GRaceStatus::Get().GetWinningPlayerInfo();
        bool noPostRace = parms != NULL ? parms->GetNoPostRaceScreen() : false;

        if (noPostRace) {
            MFlowReadyForOutro().Post(UCrc32(0x20D60DBF));

            GRaceSaveInfo *info = GRaceDatabase::Get().GetScoreInfo(parms->GetEventHash());

            info->mFlags |= 2;
        } else {
            FE_ShowWinningPostRaceScreen();
        }
    } else {
        FE_ShowWinningPostRaceScreen();
    }
}

void Game_SetCopsEnabled(bool enabled) {
    MSetCopsEnabled(enabled).Post(UCrc32("AICopManager"));
}

void Game_NoNewPursuitsOrCops() {
    ICopMgr *copMgr = ICopMgr::Get();

    if (copMgr != NULL) {

        copMgr->NoNewPursuitsOrCops();
    }
}

void Game_ForcePursuitStart(int heatLevel) {
    ICopMgr::Get()->LockoutCops(false);
    MForcePursuitStart(heatLevel).Post(UCrc32("AICopManager"));
}

void Game_EnterEngagableTrigger(GRuntimeInstance *triggerInstance) {
    new EEnterEngagableTrigger(triggerInstance);
}

void Game_ExitEngagableTrigger(GRuntimeInstance *triggerInstance) {
    new EExitEngagableTrigger(triggerInstance);
}

void Game_EnterGateZone(GRuntimeInstance *triggerInstance, const char *zoneName) {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);

    if (player != NULL) {
        IHud *hud = player->GetHud();
        IMenuZoneTrigger *menuZone;

        if (hud != NULL && hud->QueryInterface(&menuZone)) {
            menuZone->EnterTrigger(zoneName);
        }
    }
}

void Game_ExitGateZone(GRuntimeInstance *triggerInstance, const char *zoneName) {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);

    if (player != NULL) {
        IHud *hud = player->GetHud();
        IMenuZoneTrigger *menuZone;

        if (hud != NULL && hud->QueryInterface(&menuZone)) {
            menuZone->ExitTrigger();
        }
    }
}

void Game_DoZoneMenuAction(GRuntimeInstance *zoneTrigger) {
    IPlayer *player = IPlayer::First(static_cast<ePlayerList>(1));

    if (player != NULL) {
        GCollectionKey marker = zoneTrigger->RespawnMarker();
        unsigned int markerKey = marker.GetCollectionKey();

        GManager::Get().SetFreeRoamStartMarker(markerKey);

        IHud *hud = player->GetHud();

        if (hud != NULL) {
            IMenuZoneTrigger *izone;

            if (hud->QueryInterface(&izone)) {
                if (izone->IsType("safehouse")) {
                    GManager::Get().SetFreeRoamFromSafeHouseStartMarker(markerKey);
                }

                izone->RequestDoAction();
            }
        }
    }
}

void Game_ShowRaceOverSummary() {
    if (Sim::GetUserMode() != Sim::USER_SPLIT_SCREEN) {
        IPlayer *player = IPlayer::First(static_cast<ePlayerList>(1));

        if (player != NULL) {
            new EShowRaceOverMessage(player);
        }
    }
}

void Game_HideRaceOverSummary() {}

void Game_SetAllStaging(bool staging) {
    for (int onRacer = 0; onRacer < GRaceStatus::Get().GetRacerCount(); onRacer++) {

        ISimable *simable = GRaceStatus::Get().GetRacerInfo(onRacer).GetSimable();

        if (simable != NULL) {

            IVehicle *vehicle;

            if (simable->QueryInterface(&vehicle)) {

                vehicle->SetStaging(staging);
            }
        }
    }

    if (staging) {
        MAIEngineRev(0, -1, NULL, 0).Post(UCrc32("AIRev"));
    }
}

void Game_JackKnife(GRuntimeInstance *trailerInstance) {
    IVehicle *vehicle = static_cast<GCharacter *>(static_cast<void *>(trailerInstance))->GetSpawnedVehicle();

    if (vehicle != NULL) {
        MJackKnife message;

        message.SetID(vehicle->GetSimable()->GetWorldID());
        message.Post(UCrc32("AIAction"));
    }
}

void Game_SetTrafficSpeed(GRuntimeInstance *triggerInstance, float speedMultiplier, float speed) {
    IVehicle *vehicle = static_cast<GCharacter *>(static_cast<void *>(triggerInstance))->GetSpawnedVehicle();

    if (vehicle != NULL) {
        MSetTrafficSpeed message(speedMultiplier, speed, 1);

        message.SetID(vehicle->GetSimable()->GetWorldID());
        message.Post(UCrc32("AIAction"));
    }
}

void Game_ShowPauseMenu() {
    new EPause(0, 0, 0);
}

void Game_AwardCash(ISimable *simable, float amount) {
    Game_AwardPoints(simable, amount);
}

void Game_AwardPoints(ISimable *simable, float points) {
    int racerIndexOneBased = Game_GetRacerIndex(simable);
    int racerIndex = racerIndexOneBased - 1;

    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(racerIndex);

    racerInfo.AddToPointTotal(points);

    IPlayer *player = simable->GetPlayer();

    if (player != NULL) {
        IGenericMessage *igenericmessage;

        if (player->GetHud()->QueryInterface(&igenericmessage)) {
            IVisualTreatment *ivt = IVisualTreatment::Get();

            if (ivt != NULL) {
                ivt->TriggerPulse(0.2f);
            }

            g_pEAXSound->PlayUISoundFX(UISND_COMMON_SELECT);
        }
    }
}

void Game_ChallengeCompleted() {
    MNotifyChallengePassed("").Post(UCrc32(0x20D60DBF));
}

void Game_UnlockRace(GRuntimeInstance *raceInstance) {
    if (raceInstance == NULL) {
        return;
    }

    Debug_Print("unlocking race");

    GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(reinterpret_cast<GActivity *>(raceInstance));

    if (parms == NULL) {
        return;
    }

    GRaceSaveInfo *scoreInfo = GRaceDatabase::Get().GetScoreInfo(parms->GetEventHash());

    if (!parms->GetNeverInQuickRace()) {
        scoreInfo->mFlags |= GRaceDatabase::kUnlocked_QuickRace;
    }
    scoreInfo->mFlags |= GRaceDatabase::kUnlocked_Career;

    IPlayer *player = IPlayer::First(PLAYER_LOCAL);

    if (player != NULL) {
        IHud *hud = player->GetHud();

        if (hud != NULL) {
            hud->RefreshMiniMapItems();
        }
    }
    GManager::Get().RefreshEngageTriggerIcons();
}

bool Game_IsRaceUnlocked(GRuntimeInstance *raceInstance) {
    if (raceInstance == NULL) {
        return false;
    }

    GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(reinterpret_cast<GActivity *>(raceInstance));

    if (parms == NULL) {
        return false;
    }
    return IsCareerRaceUnlocked(GRaceDatabase::Get(), parms->GetEventHash());
}

bool Game_IsRaceCompleted(GRuntimeInstance *raceInstance) {
    if (raceInstance == NULL) {
        return false;
    }

    GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(reinterpret_cast<GActivity *>(raceInstance));

    if (parms == NULL) {
        return false;
    }
    return GRaceDatabase::Get().IsCareerRaceComplete(parms->GetEventHash());
}

bool Game_AllRacersDone() {
    for (int onRacer = 0; onRacer < GRaceStatus::Get().GetRacerCount(); onRacer++) {

        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(onRacer);

        if (!racerInfo.IsFinishedRacing() && !racerInfo.GetIsKnockedOut() && !racerInfo.GetIsTotalled() &&
            !racerInfo.GetIsEngineBlown() && !racerInfo.GetIsBusted()) {

            return false;
        }
    }
    return true;
}

bool Game_AllHumanPlayersDone() {
    for (int onRacer = 0; onRacer < GRaceStatus::Get().GetRacerCount(); onRacer++) {
        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(onRacer);
        ISimable *simable = racerInfo.GetSimable();
        IVehicle *vehicle = UTL::COM::QueryInterface<IVehicle>(simable);

        if (vehicle != NULL && vehicle->GetDriverClass() == DRIVER_HUMAN && !racerInfo.IsFinishedRacing() &&
            !racerInfo.GetIsKnockedOut() && !racerInfo.GetIsTotalled() && !racerInfo.GetIsEngineBlown() &&
            !racerInfo.GetIsBusted()) {
            return false;
        }
    }

    return true;
}

void Debug_ShowScreenMessage(const char *message, float duration) {
    IPlayer *player = IPlayer::First(static_cast<ePlayerList>(1));

    if (player != NULL && player->GetHud() != NULL) {
        IGenericMessage *igenericmessage;

        if (player->GetHud()->QueryInterface(&igenericmessage)) {
            igenericmessage->RequestGenericMessage("ERROR - Debug.ShowScreenMessage", false, FEHASH_ZOOMIN, 0, 0,
                                                  GenericMessage_Priority_1);
        }
    }
}

void Game_AwardPlayerBounty(int amount) {
    ISimable *simable = NULL;

    if (IPlayer::First(static_cast<ePlayerList>(1)) != NULL) {
        simable = IPlayer::First(static_cast<ePlayerList>(1))->GetSimable();
    }

    if (simable != NULL && simable->IsPlayer()) {
        int plrIndex = simable->GetPlayer()->GetSettingsIndex();
        UserProfile *prof = FEDatabase->GetUserProfile(plrIndex);
        CareerSettings *career = prof->GetCareer();

        FEPlayerCarDB *stable = NULL;
        if (plrIndex >= 0 && plrIndex <= 1) {
            stable = &prof->PlayersCarStable;
        }

        if (stable != NULL) {
            FECarRecord *fe_car = stable->GetCarRecordByHandle(prof->GetCareer()->GetCurrentCar());

            if (fe_car != NULL) {
                FECareerRecord *record = stable->GetCareerRecordByHandle(fe_car->CareerHandle);

                if (record != NULL) {
                    // FECareerRecord::TweakBounty esta vacia en VehicleDB.hpp (territorio
                    // Frontend); se escribe Bounty (offset 0x10) como hacia el inline original.
                    *reinterpret_cast<uint32 *>(reinterpret_cast<char *>(record) + 0x10) =
                        amount + record->GetBounty();

                    MNotifyPlayerRep(simable->GetInstanceHandle(), stable->GetTotalBounty()).Post(UCrc32(0x20D60DBF));
                }
            }
        }
    }
}

int Game_GetPlayerBounty() {
    ISimable *simable = NULL;

    if (IPlayer::First(static_cast<ePlayerList>(1)) != NULL) {
        simable = IPlayer::First(static_cast<ePlayerList>(1))->GetSimable();
    }

    if (simable != NULL && simable->IsPlayer()) {
        int plrIndex = simable->GetPlayer()->GetSettingsIndex();
        UserProfile *prof = FEDatabase->GetUserProfile(plrIndex);
        CareerSettings *career = prof->GetCareer();

        FEPlayerCarDB *stable = NULL;
        if (plrIndex >= 0 && plrIndex <= 1) {
            stable = &prof->PlayersCarStable;
        }

        if (stable == NULL) {
            return 0;
        }

        return stable->GetTotalBounty();
    }

    return 0;
}

void Game_NotifyCountdownDone() {

    for (int onRacer = 0; onRacer < GRaceStatus::Get().GetRacerCount(); onRacer++) {

        ISimable *simable = GRaceStatus::Get().GetRacerInfo(onRacer).GetSimable();

        if (simable != NULL) {

            IVehicle *vehicle;
            if (simable->QueryInterface(&vehicle)) {

                vehicle->Launch();
            }
        }
    }
}

void Game_ResetCopsForRestart() {
    if (ICopMgr::Exists()) {

        ICopMgr::Get()->ResetCopsForRestart(true);
    }
}

void Game_JumpToCarLot() {
    SetCurrentTimeOfDay(0.0f);
    new EQuitToFE(GARAGETYPE_CAR_LOT, "Car_Select.fng");
}

void Game_JumpToSafeHouse() {
    new EQuitToFE(GARAGETYPE_CAREER_SAFEHOUSE, "MainMenu_Sub.fng");
}

// El original devuelve ISimable*: en BindToGameCode la instanciacion es
// BindFunction<ISimable*, int> (H2ZP8ISimableZi), no <int, int>.
ISimable *Game_GetPlayerElement(int playerIndex) {
    IPlayer *player;

    if (playerIndex == 0) {
        player = IPlayer::First(PLAYER_LOCAL);
    } else {
        player = IPlayer::Last(PLAYER_LOCAL);
    }

    if (player != NULL) {
        return player->GetSimable();
    }
    return NULL;
}

void Game_BlowEngine(ISimable *simable) {
    IEngineDamage *engineDamage;

    if (simable != NULL && simable->QueryInterface(&engineDamage)) {
        engineDamage->Blow();
    }
}

void Game_ChallengeComplete(ISimable *simable) {
    if (simable == NULL) {
        return;
    }

    int racerIndex = Game_GetRacerIndex(simable) - 1;

    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(racerIndex);

    racerInfo.ChallengeComplete();
}

void Game_SabotageEngine(ISimable *simable, float damage) {
    IEngineDamage *engineDamage;

    if (simable != NULL && simable->QueryInterface(&engineDamage)) {

        engineDamage->Sabotage(damage);
    }
}

void Game_ForceAIControl(int racerIndex) {
    ISimable *simable = Game_GetPlayerElement(racerIndex);

    if (simable == NULL) {
        return;
    }
    IHumanAI *humanAI;

    if (simable->QueryInterface(&humanAI)) {

        if (!humanAI->GetAiControl()) {
            humanAI->SetAiControl(true);
        }
    }
}

void Game_ClearAIControl(int racerIndex) {
    ISimable *simable = reinterpret_cast<ISimable *>(Game_GetPlayerElement(racerIndex));

    if (simable == NULL) {
        return;
    }
    IHumanAI *humanAI;

    if (simable->QueryInterface(&humanAI)) {

        if (humanAI->GetAiControl()) {
            humanAI->SetAiControl(false);
        }
    }
}

void Game_SetTimer(const char *timerName, float seconds) {
    GManager::Get().SetTimer(timerName, seconds);
}

void Game_KillTimer(const char *timerName) {
    GManager::Get().KillTimer(timerName);
}

void Game_ShowGPS(bool show) {
    if (!show) {
        GPS_Disengage();
        WorldMap::ClearGPSing();
    }
}

void Game_NavigatePlayerTo(GRuntimeInstance *destinationInstance, GRuntimeInstance *routeInstance, float maxDeviation,
                           bool useGPS) {
    if (destinationInstance != NULL) {
        destinationInstance->GetPosition(gGPSDestination);

        if (routeInstance != NULL) {
            WorldMap::SetGPSing(static_cast<GTrigger *>(static_cast<void *>(routeInstance))->GetIcon());
        }

        if (useGPS) {
            GPS_Engage(gGPSDestination, maxDeviation);
        }
    }
}

float Game_SimableDistance(ISimable *simable, GRuntimeInstance *targetInstance) {
    if (simable != NULL && targetInstance != NULL) {

        UMath::Vector3 simablePos = simable->GetPosition();
        UMath::Vector3 targetPos;

        targetInstance->GetPosition(targetPos);

        return UMath::Distance(simablePos, targetPos);
    }

    return 0.0f;
}

float Game_SimableAngle(ISimable *simable, GRuntimeInstance *targetInstance) {
    if (simable != NULL && targetInstance != NULL) {

        IRigidBody *rigidBody = simable->GetRigidBody();

        if (rigidBody != NULL) {

            UMath::Vector3 forward;
            UMath::Vector3 direction;

            rigidBody->GetForwardVector(forward);
            targetInstance->GetDirection(direction);

            return UMath::Dot(forward, direction);
        }
    }

    return 0.0f;
}

void Debug_Assert(bool assertion, const char *message) {
    bAssertMsg(assertion, message);
}

void Game_NotifyFinished(GRuntimeInstance *activityInstance) {
    MNotifyFinished(static_cast<GCollectionKey>(activityInstance)).Post(UCrc32(0x20D60DBF));
}

bool Game_IsOnlineGame() {
    return FEDatabase->IsOnlineMode();
}

bool Game_IsLANGame() {
    return FEDatabase->IsLanMode();
}

bool Game_SkipCareerIntro() {
    if (SkipFE != 0 || SkipCareerIntro == 1) {
        return true;
    }

    IPlayer *player = IPlayer::First(PLAYER_LOCAL);

    if (player == NULL) {
        return true;
    }

    UserProfile *profile = FEDatabase->GetMultiplayerProfile(player->GetSettingsIndex());

    return profile->GetCareer()->HasDoneCareerIntro();
}

void Game_SetChanceOfRain(float chance) {
    float rainChance = UMath::Clamp(chance, 0.0f, 1.0f);

    SetOverRideRainIntensity(rainChance < 0.01f ? -1.0f : 0.0f);
}

void Game_DoFade() {
    new EFadeScreenOn(false);
}

void Game_IntroduceRival() {
}

void Game_PlayTutorial() {
    bool startedMovie = false;

    if (GRaceStatus::Exists() && !SkipFE && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        IPlayer *player = IPlayer::First(static_cast<ePlayerList>(1));

        if (player != NULL) {
            int settingsIndex = player->GetSettingsIndex();
            UserProfile *prof = FEDatabase->GetUserProfile(settingsIndex);
            CareerSettings *career = prof->GetCareer();
            GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
            char *movieName = NULL;

            if (parms != NULL) {
                switch (parms->GetRaceType()) {
                case GRace::kRaceType_Drag:
                    if (!prof->GetCareer()->HasDoneDragTutorial()) {
                        prof->GetCareer()->SetHasDoneDragTutorial();
                        movieName = "drag_tutorial";
                    }
                    break;

                case GRace::kRaceType_SpeedTrap:
                    if (!prof->GetCareer()->HasDoneSpeedTrapTutorial()) {
                        prof->GetCareer()->SetHasDoneSpeedTrapTutorial();
                        movieName = "speedtrap_tutorial";
                    }
                    break;

                case GRace::kRaceType_Tollbooth:
                    if (!prof->GetCareer()->HasDoneTollBoothTutorial()) {
                        prof->GetCareer()->SetHasDoneTollBoothTutorial();
                        movieName = "tollbooth_tutorial";
                    }
                    break;
                }
            }

            if (movieName != NULL) {
                startedMovie = true;

                new EPlayRaceMovie(movieName);
            }
        }
    }

    if (!startedMovie) {
        MNotifyMovieFinished().Post(UCrc32(0x20D60DBF));
    }
}

void Game_DoSafeHouseIntro(GRuntimeInstance *respawnMarker) {
    if (respawnMarker != NULL) {
        unsigned int markerKey = static_cast<unsigned int>(respawnMarker->GetCollection());

        GManager::Get().SetFreeRoamStartMarker(markerKey);
        GManager::Get().SetFreeRoamFromSafeHouseStartMarker(markerKey);
        GManager::Get().SetStartingFreeRoamFromSafeHouse();
    }

    FEDatabase->GetCareerSettings()->SetCurrentBin(0x10);

    FEAnyMovieScreen::PlaySafehouseIntroMovie();

    SetCurrentTimeOfDay(0.0f);

    new EQuitToFE(GARAGETYPE_CAREER_SAFEHOUSE, FEAnyMovieScreen::GetFEngPackageName());
}

bool Game_IsCareerMode() {
    if (GRaceStatus::Exists()) {
        return GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career;
    }
    return false;
}

bool Game_IsSplitScreen() {
    return Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN;
}

bool Game_AllowEngageEvents() {
    return GManager::Get().GetAllowEngageEvents();
}

bool Game_AllowMenuGates() {
    return GManager::Get().GetAllowMenuGates();
}

bool Game_AllowEngageSafehouse() {
    return GManager::Get().GetAllowEngageSafehouse();
}

void Game_SetHasRapSheet() {
    FEDatabase->GetCareerSettings()->SetHasRapSheet();
}

void Game_SetWorldHeat(float heat) {
    IPlayer *player = IPlayer::First(static_cast<ePlayerList>(1));

    if (player != NULL) {
        ISimable *simable = player->GetSimable();

        if (simable != NULL) {
            IPerpetrator *iperp;

            if (simable->QueryInterface(&iperp)) {
                iperp->SetHeat(heat);
            }
        }
    }
}

void FE_ShowLosingPostRaceScreen() {
    FE_ShowPostRaceScreen(false);
}

void FE_ShowWinningPostRaceScreen() {
    FE_ShowPostRaceScreen(true);
}

void FE_ShowPostRaceScreen(bool playerWon) {
    if (GRaceStatus::Exists()) {
        GRaceStatus::Get().FinalizeRaceStats();
    }

    if (FE_ShowOnlinePostRaceScreen()) {
        return;
    }

    if (!GRaceStatus::Exists()) {
        return;
    }

    if (GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace()) {
        bool music_active = false;

        if (SoundAI::Get() != NULL && SoundAI::Get()->IsMusicActive()) {
            music_active = true;
        }

        if (playerWon) {
            if (music_active) {
                MControlPathfinder(false, 14, 0, 0).Send(UCrc32("Event"));
            }

            new EShowMilestones(0);
        } else {
            if (music_active) {
                MControlPathfinder(false, 15, 0, 0).Send(UCrc32("Event"));
            }

            new EPause(0, 1, 0);
        }
    } else if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
        if (playerWon && !Sim::IsSplitScreen()) {
            const char *screen = "InGamePhotoMaster.fng";

            PhotoFinishScreen::SetActive();

            int port = FEngMapJoyportToJoyParam(FEDatabase->GetPlayersJoystickPort(0));

            cFEng::Get()->QueuePackagePush(screen, 0, port, true);
        } else {
            new EShowResults(static_cast<FERESULTTYPE>(0), false);
        }
    }
}

bool FE_ShowOnlinePostRaceScreen() {
    return false;
}

void FE_ShowSpeedTrapScreen(float speed, float record) {
    PhotoFinishScreen::SetTrapSpeed(speed * 0.27778f);
    PhotoFinishScreen::SetTrapBounty(record);
    PhotoFinishScreen::SetActive();

    const char *packageName = "InGamePhotoMaster.fng";
    int joyParam = FEngMapJoyportToJoyParam(FEDatabase->GetPlayersJoystickPort(0));

    cFEng::Get()->QueuePackagePush(packageName, 2, joyParam, true);
}

bool Game_SetTimeOfDay(GRuntimeInstance *activityInstance) {
    GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(reinterpret_cast<GActivity *>(activityInstance));

    if (parms != NULL) {
        if (parms->GetTimeOfDay() < 0.0f) {
            return false;
        }

        if (parms->GetTimeOfDay() >= 0.0f) {
            SetCurrentTimeOfDay(parms->GetTimeOfDay());
        }
    }
    return false;
}

void Game_ReloadWorld(GRuntimeInstance *restartRace) {
    new EReloadGame(restartRace);
}

void Game_PreventPlayerBeingBusted() {
#ifndef EA_BUILD_A124
    if (ICopMgr::Exists()) {
        ICopMgr::Get()->SetAllBustedTimersToZero();
    }
#endif
}

bool Game_DoSpecialSetup(GRuntimeInstance *activityInstance) {
    if (activityInstance == NULL) {
        return false;
    }

    GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(reinterpret_cast<GActivity *>(activityInstance));

    if (parms == NULL) {
        return false;
    }

    if (!parms->GetIsEpicPursuitRace()) {
        return false;
    }

    if (FEDatabase->GetCareerSettings()->HasBeenAwardedEpicCar()) {
        return false;
    }

    FECarRecord *rivalCar = FEDatabase->GetPlayerCarStable(0)->AwardRivalCar(0x3A94520);

    FEDatabase->GetCareerSettings()->SetCurrentCar(rivalCar->Handle);
    return true;
}

void Game_DoSpecialFinalization(GRuntimeInstance *activityInstance) {
    if (activityInstance != NULL) {
        GRaceDatabase::Get().GetRaceFromActivity(reinterpret_cast<GActivity *>(activityInstance));
    }
}

int Game_CalculateRanking(ISimable *racer, int rankingAtFinishLine) {
    if (!Sim::IsSplitScreen() && GRaceStatus::Exists() && GRaceStatus::Get().GetRaceContext() != GRace::kRaceContext_TimeTrial &&
        racer != NULL && racer->IsPlayer()) {
        if (GRaceStatus::Get().GetRaceType() == GRace::kRaceType_SpeedTrap) {
            GRaceStatus::Get().FinalizeRaceStats();
            GRaceStatus::Get().CalculateRankings();

            GRacerInfo *info = GRaceStatus::Get().GetRacerInfo(racer);

            if (info != NULL) {
                rankingAtFinishLine = info->GetRanking();
            }
        }
    }

    return rankingAtFinishLine;
}

void Game_WarpToMarkerWhenRoaming(GRuntimeInstance *markerInstance) {
#ifndef EA_BUILD_A124
    if (markerInstance != NULL) {
        GRaceStatus::Get().SetWarpWhenInFreeRoam(markerInstance->GetCollection());
    }
#endif
}
