#include "./Config.h"
#include "Joylog.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"

// TODO do all of these
const char *BuildVersionChangelistName = "191527 191537 191573 191602...";

int SkipFE = 0;
int SkipFETrackNumber = 2000;
char SkipFERaceID[16] = {};
const char *SkipFEPlayerCar = "bwm3gtre46";
const char *SkipFEPlayer2Car = "911turbo";
float SkipFEPlayerPerformance = 0.0f;
const char *SkipFEOpponentPresetRide = "";
int SkipFESplitScreen = 0;
const char *SkipFEPlayerModelOverride = 0;
int SkipFENumPlayerCars = 1;
int SkipFENumAICars = 0;
int SkipFENumLaps = 2;
eTrackDirection SkipFETrackDirection = eDIRECTION_FORWARD;
int SkipFEMaxCops = 0;
int SkipFEDisableCops = 1;
int SkipFEHelicopter = 0;
int SkipFEPovType1 = 2;
int SkipFETrafficDensity = 0;
int SkipFEDisableTraffic = 0;
float SkipFETrafficOncoming = 0.5f;
// RaceTypes SkipFERaceType;
int SkipFEPoint2Point = 0;
// eOpponentStrength SkipFEDifficulty;
int SkipFEDamageEnabled = 1;
bVector3 *SkipFEOverrideStartPosition = nullptr;
int SkipFEPrintPerformances;
int SkipNISs = 0;
eLanguages SkipFELanguage;
int SkipFEControllerConfig1 = -1;
int SkipFEControllerConfig2 = -1;

int IsSoundEnabled = 1;
int IsAudioStreamingEnabled = 1;
int IsSpeechEnabled = 1;
int IsNISAudioEnabled = 1;
bool ShutJosieUp = 0;
int IsMemcardEnabled = 1;
int IsAutoSaveEnabled = 1;

void InitConfig() {
    if (SkipFETrackNumber != -1) {
        return;
    }
    SkipFETrackNumber = 2000;
}

void JoylogConfigItems() {
    SkipFE = Joylog::AddOrGetData(SkipFE, 32, JOYLOG_CHANNEL_CONFIG);
    SkipFETrackNumber = Joylog::AddOrGetData(SkipFETrackNumber, 32, JOYLOG_CHANNEL_CONFIG);
    SkipFENumPlayerCars = Joylog::AddOrGetData(SkipFENumPlayerCars, 32, JOYLOG_CHANNEL_CONFIG);
    SkipFENumAICars = Joylog::AddOrGetData(SkipFENumAICars, 32, JOYLOG_CHANNEL_CONFIG);
    SkipFENumLaps = Joylog::AddOrGetData(SkipFENumLaps, 32, JOYLOG_CHANNEL_CONFIG);
    SkipFERaceType = Joylog::AddOrGetData(SkipFERaceType, 32, JOYLOG_CHANNEL_CONFIG);
    SkipFEMaxCops = Joylog::AddOrGetData(SkipFEMaxCops, 32, JOYLOG_CHANNEL_CONFIG);
    SkipFEHelicopter = Joylog::AddOrGetData(SkipFEHelicopter, 32, JOYLOG_CHANNEL_CONFIG);
    SkipFETrafficDensity = Joylog::AddOrGetData(SkipFETrafficDensity, 32, JOYLOG_CHANNEL_CONFIG);
    SkipFETrafficOncoming = Joylog::AddOrGetData(SkipFETrafficOncoming, JOYLOG_CHANNEL_CONFIG);
    SkipFEDifficulty = Joylog::AddOrGetData(SkipFEDifficulty, 32, JOYLOG_CHANNEL_CONFIG);
    IsSoundEnabled = Joylog::AddOrGetData(IsSoundEnabled, 32, JOYLOG_CHANNEL_CONFIG);
    IsAudioStreamingEnabled = Joylog::AddOrGetData(IsAudioStreamingEnabled, 32, JOYLOG_CHANNEL_CONFIG);
    IsSpeechEnabled = Joylog::AddOrGetData(IsSpeechEnabled, 32, JOYLOG_CHANNEL_CONFIG);
    IsNISAudioEnabled = Joylog::AddOrGetData(IsNISAudioEnabled, 32, JOYLOG_CHANNEL_CONFIG);
    SkipMovies = Joylog::AddOrGetData(SkipMovies, 32, JOYLOG_CHANNEL_CONFIG);
    IsMemcardEnabled = Joylog::AddOrGetData(IsMemcardEnabled, 32, JOYLOG_CHANNEL_CONFIG);
    IsAutoSaveEnabled = Joylog::AddOrGetData(IsAutoSaveEnabled, 32, JOYLOG_CHANNEL_CONFIG);
    UnlockAllThings = Joylog::AddOrGetData(UnlockAllThings, 32, JOYLOG_CHANNEL_CONFIG) != 0;
    EmergencySaveMemory = Joylog::AddOrGetData(EmergencySaveMemory, 32, JOYLOG_CHANNEL_CONFIG);
    Joylog::AddOrGetData(SkipFERaceID, JOYLOG_CHANNEL_CONFIG);
}

void LoadConfigItems() {
    JoylogConfigItems();
}

void SaveConfigItems() {
    JoylogConfigItems();
}
