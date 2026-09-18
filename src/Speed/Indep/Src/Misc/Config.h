#ifndef MISC_CONFIG_H
#define MISC_CONFIG_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/AI/AIBasics.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"

// Los globales que define Config.cpp, con los tipos del DWARF.
extern char *BuildVersionChangelistName;

extern int EmergencySaveMemory;
extern int SkipFE;
extern int SkipFETrackNumber;
extern char SkipFERaceID[16];
extern const char *SkipFEPlayerCar;
extern const char *SkipFEPlayer2Car;
extern float SkipFEPlayerPerformance;
extern const char *SkipFEOpponentPresetRide;
extern int SkipFESplitScreen;
extern int SkipFENumPlayerCars;
extern int SkipFENumAICars;
extern int SkipFENumLaps;
extern eTrackDirection SkipFETrackDirection;
extern int SkipFEMaxCops;
extern int SkipFEDisableCops;
extern int SkipFEHelicopter;
extern int SkipFEPovType1;
extern int SkipFETrafficDensity;
extern int SkipFEDisableTraffic;
extern float SkipFETrafficOncoming;
extern RaceTypes SkipFERaceType;
extern int SkipFEPoint2Point;
extern eOpponentStrength SkipFEDifficulty;
extern int SkipFEDamageEnabled;
extern bVector3 *SkipFEOverrideStartPosition;
extern int SkipFEPrintPerformances;
extern int SkipNISs;
extern eLanguages SkipFELanguage;
extern int SkipFEControllerConfig1;
extern int SkipFEControllerConfig2;

extern int bRumbleEnabled;
extern bool gVerboseTesterOutput;
extern int PrecipitationEnable;
extern int TimeOfDaySwapEnable;
extern bool EnableParticleSystem;
extern float DebugCameraNearPlane;

extern int IsSoundEnabled;
extern int IsAudioStreamingEnabled;
extern int IsSpeechEnabled;
extern int IsNISAudioEnabled;
extern bool ShutJosieUp;
extern int IsMemcardEnabled;
extern int IsAutoSaveEnabled;

extern int AnimCfg_DisableAnimations;
extern int AnimCfg_DebugOutput;
extern int AnimCfg_DisableWorldAnimations;
extern int OnlineEnabled;
extern int DisableCommunication;
extern bool UnlockAllThings;
extern bool SkipCareerIntro;
extern bool SkipDDayRaces;
extern bool ShowAllCarsInFE;
extern bool ShowAllPresetsInFE;
extern int MikeMannBuild;
extern bool IsCollectorsEdition;
extern bool CarGuysCamera;
extern int DoScreenPrintf;
extern int SkipMovies;

void InitConfig();
void LoadConfigItems();
void SaveConfigItems();

#endif
