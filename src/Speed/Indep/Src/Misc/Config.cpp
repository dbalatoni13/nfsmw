#include "./Config.h"
#include "Joylog.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern int SkipFE;
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

int IsSoundEnabled = 1;
int IsAudioStreamingEnabled = 1;
int IsSpeechEnabled = 1;
int IsNISAudioEnabled = 1;
bool ShutJosieUp = true;
int IsMemcardEnabled = 1;
int IsAutoSaveEnabled = 1;

void InitConfig() {
    if (SkipFETrackNumber != -1) {
        return;
    }
    SkipFETrackNumber = 2000;
}

void JoylogConfigItems() {
    SkipFE = Joylog::AddOrGetData(static_cast<unsigned int>(SkipFE), 32, JOYLOG_CHANNEL_CONFIG);
    SkipFETrackNumber = Joylog::AddOrGetData(static_cast<unsigned int>(SkipFETrackNumber), 32, JOYLOG_CHANNEL_CONFIG);
    SkipFENumPlayerCars = Joylog::AddOrGetData(static_cast<unsigned int>(SkipFENumPlayerCars), 32, JOYLOG_CHANNEL_CONFIG);
    SkipFENumAICars = Joylog::AddOrGetData(static_cast<unsigned int>(SkipFENumAICars), 32, JOYLOG_CHANNEL_CONFIG);
    SkipFENumLaps = Joylog::AddOrGetData(static_cast<unsigned int>(SkipFENumLaps), 32, JOYLOG_CHANNEL_CONFIG);
    SkipFERaceType = Joylog::AddOrGetData(static_cast<unsigned int>(SkipFERaceType), 32, JOYLOG_CHANNEL_CONFIG);
    SkipFEMaxCops = Joylog::AddOrGetData(static_cast<unsigned int>(SkipFEMaxCops), 32, JOYLOG_CHANNEL_CONFIG);
    SkipFEHelicopter = Joylog::AddOrGetData(static_cast<unsigned int>(SkipFEHelicopter), 32, JOYLOG_CHANNEL_CONFIG);
    SkipFETrafficDensity = Joylog::AddOrGetData(static_cast<unsigned int>(SkipFETrafficDensity), 32, JOYLOG_CHANNEL_CONFIG);
    SkipFETrafficOncoming = Joylog::AddOrGetData(SkipFETrafficOncoming, JOYLOG_CHANNEL_CONFIG);
    SkipFEDifficulty = Joylog::AddOrGetData(static_cast<unsigned int>(SkipFEDifficulty), 32, JOYLOG_CHANNEL_CONFIG);
    IsSoundEnabled = Joylog::AddOrGetData(static_cast<unsigned int>(IsSoundEnabled), 32, JOYLOG_CHANNEL_CONFIG);
    IsAudioStreamingEnabled = Joylog::AddOrGetData(static_cast<unsigned int>(IsAudioStreamingEnabled), 32, JOYLOG_CHANNEL_CONFIG);
    IsSpeechEnabled = Joylog::AddOrGetData(static_cast<unsigned int>(IsSpeechEnabled), 32, JOYLOG_CHANNEL_CONFIG);
    IsNISAudioEnabled = Joylog::AddOrGetData(static_cast<unsigned int>(IsNISAudioEnabled), 32, JOYLOG_CHANNEL_CONFIG);
    SkipMovies = Joylog::AddOrGetData(static_cast<unsigned int>(SkipMovies), 32, JOYLOG_CHANNEL_CONFIG);
    IsMemcardEnabled = Joylog::AddOrGetData(static_cast<unsigned int>(IsMemcardEnabled), 32, JOYLOG_CHANNEL_CONFIG);
    IsAutoSaveEnabled = Joylog::AddOrGetData(static_cast<unsigned int>(IsAutoSaveEnabled), 32, JOYLOG_CHANNEL_CONFIG);
    UnlockAllThings = Joylog::AddOrGetData(static_cast<unsigned int>(UnlockAllThings), 32, JOYLOG_CHANNEL_CONFIG) != 0;
    EmergencySaveMemory = Joylog::AddOrGetData(static_cast<unsigned int>(EmergencySaveMemory), 32, JOYLOG_CHANNEL_CONFIG);
    Joylog::AddOrGetData(SkipFERaceID, JOYLOG_CHANNEL_CONFIG);
}

void LoadConfigItems() {
    JoylogConfigItems();
}

void SaveConfigItems() {
    JoylogConfigItems();
}
