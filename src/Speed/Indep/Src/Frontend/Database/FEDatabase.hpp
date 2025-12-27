#ifndef FRONTEND_DATABASE_FEDATABASE_H
#define FRONTEND_DATABASE_FEDATABASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "RaceDB.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "VehicleDB.hpp"

#include <types.h>

#if ONLINE_SUPPORT
#include "Speed/Indep/Src/Online/OnlineCfg.hpp"
#endif

enum eControllerConfig {
    CC_CONFIG_1,
    CC_CONFIG_2,
    CC_CONFIG_3,
    CC_CONFIG_4,
    CC_CONFIG_5,
    NUM_CONTROLLER_CONFIGS,
    MIN_CONFIG = 0,
    MAX_CONFIG = 4,
};

enum ePlayerSettingsCameras {
    PSC_BUMPER,
    PSC_HOOD,
    PSC_CLOSE,
    PSC_FAR,
    PSC_SUPER_FAR,
    PSC_DRIFT,
    PSC_PURSUIT,
    NUM_CAMERAS_IN_OPTIONS,
    PSC_DEFAULT = 2,
};

enum eControllerAttribs {
    CA_HUD = 0,
    CA_DRIVING = 1,
};

enum eOptionsCategory {
    OC_AUDIO = 0,
    OC_VIDEO = 1,
    OC_PC_ADV_DISPLAY = 2,
    OC_GAMEPLAY = 3,
    OC_PLAYER = 4,
    OC_CONTROLS = 5,
    OC_EATRAX = 6,
    OC_TRAILERS = 7,
    OC_CREDITS = 8,
    OC_ONLINE = 9,
    NUM_OPTIONS_CATEGORIES = 10,
};

enum ePostRaceOptions {
    POST_RACE_OPT_NEXT_RACE = 0,
    POST_RACE_OPT_QUIT = 1,
    POST_RACE_OPT_RESTART_RACE = 2,
    POST_RACE_OPT_RESTART_EVENT = 3,
};

enum eLoadSaveGame {
    eLOADSAVE_LOAD = 0,
    eLOADSAVE_SAVE = 1,
};

// total size: 0x20
class GameplaySettings {
  public:
    bool AutoSaveOn;                    // offset 0x0, size 0x1
    bool RearviewOn;                    // offset 0x4, size 0x1
    bool Damage;                        // offset 0x8, size 0x1
    unsigned char SpeedoUnits;          // offset 0xC, size 0x1
    unsigned char RacingMiniMapMode;    // offset 0xD, size 0x1
    unsigned char ExploringMiniMapMode; // offset 0xE, size 0x1
    unsigned int MapItems;              // offset 0x10, size 0x4
    unsigned char LastMapZoom;          // offset 0x14, size 0x1
    unsigned char LastPursuitMapZoom;   // offset 0x15, size 0x1
    unsigned char LastMapView;          // offset 0x16, size 0x1
    bool JumpCam;                       // offset 0x18, size 0x1
    float HighlightCam;                 // offset 0x1C, size 0x4
};

// total size: 0x2C
class PlayerSettings {
  public:
    unsigned int GetControllerAttribs(eControllerAttribs type, bool wheel_connected) const;
    void ScrollDriveCam(int dir);

    bool GaugesOn;
    bool PositionOn;
    bool LapInfoOn;
    bool ScoreOn;
    bool Rumble;
    bool LeaderboardOn;
    bool TransmissionPromptOn;
    bool DriveWithAnalog;
    eControllerConfig Config;
    ePlayerSettingsCameras CurCam;
    unsigned char SplitTimeType;
    unsigned char Transmission;
    unsigned char Handling;
};

// total size: 0x10
class VideoSettings {
  public:
    float FEScale;       // offset 0x0, size 0x4
    float ScreenOffsetX; // offset 0x4, size 0x4
    float ScreenOffsetY; // offset 0x8, size 0x4
    bool WideScreen;     // offset 0xC, size 0x1
};

// total size: 0x34
class AudioSettings {
  public:
    float MasterVol;          // offset 0x0, size 0x4
    float SpeechVol;          // offset 0x4, size 0x4
    float FEMusicVol;         // offset 0x8, size 0x4
    float IGMusicVol;         // offset 0xC, size 0x4
    float SoundEffectsVol;    // offset 0x10, size 0x4
    float EngineVol;          // offset 0x14, size 0x4
    float CarVol;             // offset 0x18, size 0x4
    float AmbientVol;         // offset 0x1C, size 0x4
    float SpeedVol;           // offset 0x20, size 0x4
    int AudioMode;            // offset 0x24, size 0x4
    int InteractiveMusicMode; // offset 0x28, size 0x4
    int EATraxMode;           // offset 0x2C, size 0x4
    int PlayState;            // offset 0x30, size 0x4
};

// total size: 0xC0
class OptionsSettings {
  public:
    eOptionsCategory CurrentCategory;     // offset 0x0, size 0x4
    VideoSettings TheVideoSettings;       // offset 0x4, size 0x10
    GameplaySettings TheGameplaySettings; // offset 0x14, size 0x20
    AudioSettings TheAudioSettings;       // offset 0x34, size 0x34
    PlayerSettings ThePlayerSettings[2];  // offset 0x68, size 0x58
};

// total size: 0x4
struct SMSMessage {
  public:
  private:
    unsigned char Handle;     // offset 0x0, size 0x1
    unsigned char Flags;      // offset 0x1, size 0x1
    unsigned short SortOrder; // offset 0x2, size 0x2
};

// total size: 0x27C
class CareerSettings {
  public:
    unsigned int CurrentCar;     // offset 0x0, size 0x4
    unsigned int SpecialFlags;   // offset 0x4, size 0x4
    unsigned char CurrentBin;    // offset 0x8, size 0x1
    unsigned int CurrentCash;    // offset 0xC, size 0x4
    short AdaptiveDifficulty;    // offset 0x10, size 0x2
    SMSMessage SMSMessages[150]; // offset 0x12, size 0x258
    unsigned short SMSSortOrder; // offset 0x26A, size 0x2
    char CaseFileName[16];       // offset 0x26C, size 0x10
};

// total size: 0x8
struct JukeboxEntry {
    unsigned int SongIndex;         // offset 0x0, size 0x4
    unsigned char PlayabilityField; // offset 0x4, size 0x1
};

// total size: 0x9CF4
class UserProfile {
  public:
    OptionsSettings *GetOptions() {
        return &TheOptionsSettings;
    }

  private:
    char m_aProfileName[32];            // offset 0x0, size 0x20
    bool m_bNamed;                      // offset 0x20, size 0x1
    OptionsSettings TheOptionsSettings; // offset 0x24, size 0xC0
    CareerSettings TheCareerSettings;   // offset 0xE4, size 0x27C
  public:
    JukeboxEntry Playlist[30];                  // offset 0x360, size 0xF0
    FEPlayerCarDB PlayersCarStable;             // offset 0x450, size 0x8CC8
    bool CareerModeHasBeenCompletedAtLeastOnce; // offset 0x9118, size 0x1
    HighScoresDatabase HighScores;              // offset 0x911C, size 0xBD8
};

// total size: 0x24
struct RaceSettings {
    uint32 EventHash;            // offset 0x0, size 0x4
    uint8 NumLaps;               // offset 0x4, size 0x1
    uint8 TrackDirection;        // offset 0x5, size 0x1
    bool IsLapKO;                // offset 0x8, size 0x1
    uint8 NumOpponents;          // offset 0xC, size 0x1
    uint8 AISkill;               // offset 0xD, size 0x1
    uint8 CopDensity;            // offset 0xE, size 0x1
    uint8 TrafficDensity;        // offset 0xF, size 0x1
    bool CatchUp;                // offset 0x10, size 0x1
    bool CopsOn;                 // offset 0x14, size 0x1
    uint8 RegionFilterBits;      // offset 0x18, size 0x1
    unsigned int SelectedCar[2]; // offset 0x1C, size 0x8
#ifdef EA_PLATFORM_PLAYSTATION2
    int CarSelectFilterBits[2];
#endif
};

// total size: 0x14C
struct FEKeyboardSettings {
    int AcceptCallbackHash;  // offset 0x0, size 0x4
    int DeclineCallbackHash; // offset 0x4, size 0x4
    int DefaultTextHash;     // offset 0x8, size 0x4
    int MaxTextLength;       // offset 0xC, size 0x4
    int Mode;                // offset 0x10, size 0x4
    char Buffer[156];        // offset 0x14, size 0x9C
    char Title[156];         // offset 0xB0, size 0x9C
};

// total size: 0xA28
class cFrontendDatabase {
  public:
    PlayerSettings *GetPlayerSettings(int player) {
        return &CurrentUserProfiles[0]->GetOptions()->ThePlayerSettings[player];
    }

    FEPlayerCarDB *GetPlayerCarStable(int player) {
        return &CurrentUserProfiles[player]->PlayersCarStable;
    }

    unsigned char iNumPlayers; // offset 0x0, size 0x1
    bool bComingFromBoot;      // offset 0x4, size 0x1
    bool bSavedProfileForMP;   // offset 0x8, size 0x1
    bool bProfileLoaded;       // offset 0xC, size 0x1
    bool bIsOptionsDirty;      // offset 0x10, size 0x1
#ifndef EA_PLATFORM_PLAYSTATION2
    bool bAutoSaveOverwriteConfirmed; // offset 0x14, size 0x1
#endif
    unsigned int iDefaultStableHash;     // offset 0x18, size 0x4
    signed char PlayerJoyports[2];       // offset 0x1C, size 0x2
    UserProfile *CurrentUserProfiles[2]; // offset 0x20, size 0x8
    GRace::Type RaceMode;                // offset 0x28, size 0x4
  private:
    RaceSettings TheQuickRaceSettings[11]; // offset 0x2C, size 0x18C
  public:
    char *m_pCarStableBackup; // offset 0x1B8, size 0x4
    char *m_pDBBackup;        // offset 0x1BC, size 0x4
  private:
    unsigned int FEGameMode; // offset 0x1C0, size 0x4
  public:
    eLoadSaveGame LoadSaveGame; // offset 0x1C4, size 0x4
#if ONLINE_SUPPORT
    cOnlineSettings OnlineSettings;
    OnlineCreateUserSettings mOnlineCreateUserSettings;
#endif
    FEKeyboardSettings mFEKeyboardSettings;          // offset 0x1C8, size 0x14C
    int iCurPauseSubOptionType;                      // offset 0x314, size 0x4
    int iCurPauseOptionType;                         // offset 0x318, size 0x4
    FECustomizationRecord *SplitScreenCustomization; // offset 0x31C, size 0x4
    char SplitScreenCarType[256];                    // offset 0x320, size 0x100
    cFinishedRaceStats FinishedRaceStats;            // offset 0x420, size 0x604
    ePostRaceOptions PostRaceOptionChosen;           // offset 0xA24, size 0x4
};

extern cFrontendDatabase *FEDatabase;

void InitFrontendDatabase();

#endif
