#ifndef MISC_CONFIG_H
#define MISC_CONFIG_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

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
extern int SkipFERaceType;
extern int SkipFEMaxCops;
extern int SkipFEHelicopter;
extern int SkipFETrafficDensity;
extern float SkipFETrafficOncoming;
extern int SkipFEDifficulty;
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
extern eTrackDirection SkipFETrackDirection;

void InitConfig();
void LoadConfigItems();
void SaveConfigItems();

#endif
