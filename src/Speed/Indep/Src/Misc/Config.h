#ifndef MISC_CONFIG_H
#define MISC_CONFIG_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"

extern const char *BuildVersionChangelistName;

extern int IsSoundEnabled;
extern int IsAudioStreamingEnabled;
extern int IsSpeechEnabled;
extern int IsNISAudioEnabled;
extern bool ShutJosieUp;
extern int IsMemcardEnabled;
extern int IsAutoSaveEnabled;
extern int SkipFE;
extern int SkipFESplitScreen;
extern int SkipFETrackNumber;
extern int SkipFENumPlayerCars;
extern int SkipFENumAICars;
extern int SkipFENumLaps;
extern RaceTypes SkipFERaceType;
extern int SkipFEPoint2Point;
extern int SkipFEMaxCops;
extern int SkipFEHelicopter;
extern int SkipFEPovType1;
extern int SkipFETrafficDensity;
extern float SkipFETrafficOncoming;
extern eOpponentStrength SkipFEDifficulty;
extern int SkipFEDamageEnabled;
extern bVector3 *SkipFEOverrideStartPosition;
extern eLanguages SkipFELanguage;
extern int SkipFEPrintPerformances;
extern int SkipNISs;
extern int SkipFEControllerConfig1;
extern int SkipFEControllerConfig2;
extern int SkipMovies;
extern int UnlockAllThings;
extern int EmergencySaveMemory;
extern char SkipFERaceID[];
extern int SkipFEDisableCops;
extern int SkipFEDisableTraffic;
extern int SkipCareerIntro;
extern int SkipDDayRaces;
extern int UnlockAllThings;
extern int MikeMannBuild;
extern bool IsCollectorsEdition;
extern eTrackDirection SkipFETrackDirection;
extern int OnlineEnabled;
extern bool gVerboseTesterOutput;

void InitConfig();
void LoadConfigItems();
void SaveConfigItems();

#endif
