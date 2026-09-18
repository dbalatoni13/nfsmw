#ifndef LUA_LUAGAMEHOOKS_H
#define LUA_LUAGAMEHOOKS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Gameplay/GRuntimeInstance.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

class GRaceParameters;
class IVehicle;

namespace Physics {
namespace Info {
    struct Performance;
}
}

void Activity_Run(GRuntimeInstance *activityInstance);
void Activity_Suspend(GRuntimeInstance *activityInstance);

void Audio_SetFlag(const char *flagName, bool value);
bool Audio_IsCopSpeechPlaying();

int Bin_GetNumChallengesPassed(int binIndex);
int Bin_GetNumRacesWon(int binIndex);

void Camera_SetGenericCamera(const char *groupName, const char *trackName);

void MiniMap_AddEngagedRace(GRuntimeInstance *engageTriggerInstance);

void NIS_Play(GRuntimeInstance *startMarker, const char *animName, const char *animType, int cameraTrack,
              const char *preMovie, const char *postMovie);

void Movie_PlayHackE3FMV();

void Debug_Print(const char *message);
void Debug_PrintInstance(GRuntimeInstance *inst);
void Debug_ShowScreenMessage(const char *message, float duration);
void Debug_Assert(bool assertion, const char *message);

void Demo_SetRaceCompleteForFE(GRuntimeInstance *activityInstance);
void Demo_StorePursuitRepForFE(GRuntimeInstance *activityInstance);

void HUD_ShowMessage(const char *languageString);
void HUD_ShowTimeExtension(float seconds);

int Math_RandomInt(int range);

bool Platform_IsNextGen();

float Game_GetSimTime();
void Game_AddPlayer(IVehicle *playerVehicle);
bool Game_FindPerformanceCandidates(UTL::Std::list<unsigned int, _type_list> &candidates, unsigned int limit,
                                    const Physics::Info::Performance &perf);
unsigned int Game_MaxUniqueOpponents();
void Game_SetSplitGrid();
void Game_InitRacers(GRuntimeInstance *startMarker);
void Game_KnockoutRacer(ISimable *simable);
void Game_DetachCameraFromRacer(ISimable *simable);
void Game_WarpPlayerToTrigger(GRuntimeInstance *triggerInstance);
void Game_SetPlayerStartPosition(GRuntimeInstance *startMarker);
void Game_ResetTrigger(GRuntimeInstance *triggerInstance);
void Game_ShowTriggerIcon(GRuntimeInstance *triggerInstance);
void Game_HideTriggerIcon(GRuntimeInstance *triggerInstance);
void Game_SpawnCop(GRuntimeInstance *copInstance, const char *carName, bool isPursuit, bool unknown);
void Game_SpawnCharacter(GRuntimeInstance *characterInstance, GRuntimeInstance *playerMarker,
                         GRuntimeInstance *carMarker, float facing);
void Game_UnspawnCharacter(GRuntimeInstance *characterInstance);
void Game_SendCharacterStimulus(GRuntimeInstance *characterInstance, const char *stimulusName);
void Game_SetRacerLapsLeft(int racerIndex, int lapsLeft);
void Game_SetRacerGoal(int racerIndex, GRuntimeInstance *goalInstance);
void Game_NotifyCheckpointReached(ISimable *simable, int checkpointIndex);
void Game_NotifyLapFinished(ISimable *simable, int lapIndex);
void Game_NotifyRaceFinished(ISimable *simable);
int Game_GetRacerIndex(ISimable *simable);
bool Game_RacerIsHuman(int racerIndex);
bool Game_PlayerIsLocal(ISimable *simable);
ISimable *Game_GetRacerElement(int racerIndex);
GRuntimeInstance *Game_GetRacerCharacter(int racerIndex);
int Game_GetNumRacers();
float Game_GetSimableSpeedKmh(ISimable *simable);
bool Game_IsActiveSpeedTrap(GRuntimeInstance *speedTrapInstance);
bool Game_IsActiveMenuGate(GRuntimeInstance *menuGateInstance);
void Game_NotifySpeedTrapTriggered(GRuntimeInstance *speedTrapInstance, GRuntimeInstance *carInstance,
                                   ISimable *simable, float speed);
void Game_NotifyRacePlacement(GRuntimeInstance *raceInstance, ISimable *simable, int placement);
void Game_SaveStartPositions();
void Game_RestoreStartPositions();
void Game_SetRaceActivity(GRuntimeInstance *activityInstance);
void Game_StartRace(GRuntimeInstance *raceInstance);
void Game_StartRaceTimers();
void Game_AbandonRace();
void Game_EnterPostRaceFlow();
void Game_SetCopsEnabled(bool enabled);
void Game_NoNewPursuitsOrCops();
void Game_ForcePursuitStart(int heatLevel);
void Game_EnterEngagableTrigger(GRuntimeInstance *triggerInstance);
void Game_ExitEngagableTrigger(GRuntimeInstance *triggerInstance);
void Game_EnterGateZone(GRuntimeInstance *triggerInstance, const char *zoneName);
void Game_ExitGateZone(GRuntimeInstance *triggerInstance, const char *zoneName);
void Game_DoZoneMenuAction(GRuntimeInstance *triggerInstance);
void Game_ShowRaceOverSummary();
void Game_HideRaceOverSummary();
void Game_SetAllStaging(bool staging);
void Game_JackKnife(GRuntimeInstance *trailerInstance);
void Game_SetTrafficSpeed(GRuntimeInstance *triggerInstance, float speedMultiplier, float speed);
void Game_ShowPauseMenu();
void Game_AwardCash(ISimable *simable, float amount);
void Game_AwardPoints(ISimable *simable, float points);
void Game_ChallengeCompleted();
void Game_UnlockRace(GRuntimeInstance *raceInstance);
bool Game_IsRaceUnlocked(GRuntimeInstance *raceInstance);
bool Game_IsRaceCompleted(GRuntimeInstance *raceInstance);
bool Game_AllRacersDone();
bool Game_AllHumanPlayersDone();
void Game_AwardPlayerBounty(int bounty);
int Game_GetPlayerBounty();
void Game_NotifyCountdownDone();
void Game_ResetCopsForRestart();
void Game_JumpToCarLot();
void Game_JumpToSafeHouse();
ISimable *Game_GetPlayerElement(int playerIndex);
void Game_BlowEngine(ISimable *simable);
void Game_ChallengeComplete(ISimable *simable);
void Game_SabotageEngine(ISimable *simable, float damage);
void Game_ForceAIControl(int racerIndex);
void Game_ClearAIControl(int racerIndex);
void Game_SetTimer(const char *timerName, float seconds);
void Game_KillTimer(const char *timerName);
void Game_ShowGPS(bool show);
void Game_NavigatePlayerTo(GRuntimeInstance *destinationInstance, GRuntimeInstance *routeInstance, float speed,
                           bool useGPS);
float Game_SimableDistance(ISimable *simable, GRuntimeInstance *targetInstance);
float Game_SimableAngle(ISimable *simable, GRuntimeInstance *targetInstance);
void Game_NotifyFinished(GRuntimeInstance *activityInstance);
bool Game_IsOnlineGame();
bool Game_IsLANGame();
bool Game_SkipCareerIntro();
void Game_SetChanceOfRain(float chance);
void Game_DoFade();
void Game_IntroduceRival();
void Game_PlayTutorial();
void Game_DoSafeHouseIntro(GRuntimeInstance *safeHouseInstance);
bool Game_IsCareerMode();
bool Game_IsSplitScreen();
bool Game_AllowEngageEvents();
bool Game_AllowMenuGates();
bool Game_AllowEngageSafehouse();
void Game_SetHasRapSheet();
void Game_SetWorldHeat(float heat);
bool Game_SetTimeOfDay(GRuntimeInstance *timeOfDayInstance);
void Game_ReloadWorld(GRuntimeInstance *worldInstance);
void Game_PreventPlayerBeingBusted();
bool Game_DoSpecialSetup(GRuntimeInstance *activityInstance);
void Game_DoSpecialFinalization(GRuntimeInstance *activityInstance);
int Game_CalculateRanking(ISimable *simable, int position);
void Game_WarpToMarkerWhenRoaming(GRuntimeInstance *markerInstance);

void FE_ShowLosingPostRaceScreen();
void FE_ShowWinningPostRaceScreen();
void FE_ShowPostRaceScreen(bool playerWon);
bool FE_ShowOnlinePostRaceScreen();
void FE_ShowSpeedTrapScreen(float speed, float record);

#endif
