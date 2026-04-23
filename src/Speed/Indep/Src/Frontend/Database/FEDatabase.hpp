#ifndef FRONTEND_DATABASE_FEDATABASE_H
#define FRONTEND_DATABASE_FEDATABASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "RaceDB.hpp"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "VehicleDB.hpp"

#include <types.h>

class GRaceCustom;

#if ONLINE_SUPPORT
#include "Speed/Indep/Src/Online/OnlineCfg.hpp"
#endif

enum eFEGameModes {
    eFE_GAME_MODE_NONE = 0,
    eFE_GAME_MODE_CAREER = 1,
    eFE_GAME_MODE_CHALLENGE = 2,
    eFE_GAME_MODE_QUICK_RACE = 4,
    eFE_GAME_MODE_ONLINE = 8,
    eFE_GAME_MODE_OPTIONS = 16,
    eFE_GAME_MODE_CUSTOMIZE = 32,
    eFE_GAME_MODE_LAN = 64,
    eFE_GAME_MODE_PROFILE_MANAGER = 128,
    eFE_GAME_MODE_CAREER_MANAGER = 256,
    eFE_GAME_MODE_RAP_SHEET = 512,
    eFE_GAME_MODE_MODE_SELECT = 1024,
    eFE_GAME_TRAILERS = 2048,
    eFE_GAME_MODE_CAR_LOT = 32768,
    eFE_GAME_MODE_SAFEHOUSE = 65536,
    eFE_GAME_MODE_POST_RIVAL = 131072,
    eFE_GAME_MODE_BEAT_GAME = 262144,
    eFE_GAME_MODE_ALL = -1,
};

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

enum eExitRacePlaces {
    EXIT_RACE_FROM_PAUSE = 0,
    EXIT_RACE_FROM_POSTRACE = 1,
};

#ifndef FRONTEND_DATABASE_EWORLDMAPITEMTYPE_DEFINED
#define FRONTEND_DATABASE_EWORLDMAPITEMTYPE_DEFINED
enum eWorldMapItemType {
    WMIT_NONE = 0,
    WMIT_PLAYER_CAR = 1,
    WMIT_AI_RACE_CAR = 2,
    WMIT_COP_CAR = 4,
    WMIT_COP_HELI = 8,
    WMIT_TRAFFIC_CAR = 16,
    WMIT_ROADBLOCK = 32,
    WMIT_CHECKPOINT = 64,
    WMIT_CIRCUIT_RACE = 128,
    WMIT_SPRINT_RACE = 256,
    WMIT_LAP_KO_RACE = 512,
    WMIT_DRAG_RACE = 1024,
    WMIT_SPEED_TRAP_RACE = 2048,
    WMIT_TOLLBOOTH_RACE = 4096,
    WMIT_MULTIPOINT_RACE = 8192,
    WMIT_CELL_PHONE_RACE = 16384,
    WMIT_RIVAL_RACE = 32768,
    WMIT_CASH_GRAB_RACE = 65536,
    WMIT_CASH_GRAB_SMALL = 131072,
    WMIT_CASH_GRAB_MED = 262144,
    WMIT_CASH_GRAB_LARGE = 524288,
    WMIT_CASH_GRAB_ALL = 917504,
    WMIT_SPEED_TRAP = 1048576,
    WMIT_SAFEHOUSE = 2097152,
    WMIT_SHOP = 4194304,
    WMIT_CAR_LOT = 8388608,
    WMIT_TOKEN = 16777216,
    WMIT_HIDING_SPOT = 33554432,
    WMIT_PURSUIT_BREAKER = 67108864,
};
#endif

// total size: 0x20
class GameplaySettings {
  public:
    void Default();
    bool operator==(const GameplaySettings &rhs) const;
    bool IsMapItemEnabled(eWorldMapItemType type);
    void SetMapItem(eWorldMapItemType type, bool enabled);

    int AutoSaveOn;                     // offset 0x0, size 0x1
    int RearviewOn;                     // offset 0x4, size 0x1
    int Damage;                         // offset 0x8, size 0x1
    unsigned char SpeedoUnits;          // offset 0xC, size 0x1
    unsigned char RacingMiniMapMode;    // offset 0xD, size 0x1
    unsigned char ExploringMiniMapMode; // offset 0xE, size 0x1
    unsigned int MapItems;              // offset 0x10, size 0x4
    unsigned char LastMapZoom;          // offset 0x14, size 0x1
    unsigned char LastPursuitMapZoom;   // offset 0x15, size 0x1
    unsigned char LastMapView;          // offset 0x16, size 0x1
    int JumpCam;                        // offset 0x18, size 0x1
    float HighlightCam;                 // offset 0x1C, size 0x4
};

// total size: 0x2C
class PlayerSettings {
  public:
    void Default();
    void DefaultFromOptionsScreen();
    bool operator==(const PlayerSettings &rhs) const;
    unsigned int GetControllerAttribs(eControllerAttribs type, bool wheel_connected) const;
    void ScrollDriveCam(int dir);

    int GaugesOn;
    int PositionOn;
    int LapInfoOn;
    int ScoreOn;
    int Rumble;
    int LeaderboardOn;
    int TransmissionPromptOn;
    int DriveWithAnalog;
    eControllerConfig Config;
    ePlayerSettingsCameras CurCam;
    unsigned char SplitTimeType;
    unsigned char Transmission;
    unsigned char Handling;
};

// total size: 0x10
class VideoSettings {
  public:
    void Default();
    bool operator==(const VideoSettings &rhs) const;

    float FEScale;       // offset 0x0, size 0x4
    float ScreenOffsetX; // offset 0x4, size 0x4
    float ScreenOffsetY; // offset 0x8, size 0x4
    int WideScreen;      // offset 0xC, size 0x1
};

// total size: 0x34
class AudioSettings {
  public:
    void Default();
    bool operator==(const AudioSettings &rhs) const;

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
    void Default();

    eOptionsCategory CurrentCategory;     // offset 0x0, size 0x4
    VideoSettings TheVideoSettings;       // offset 0x4, size 0x10
    GameplaySettings TheGameplaySettings; // offset 0x14, size 0x20
    AudioSettings TheAudioSettings;       // offset 0x34, size 0x34
    PlayerSettings ThePlayerSettings[2];  // offset 0x68, size 0x58
};

// total size: 0x4
struct SMSMessage {
  public:
    unsigned char GetHandle() {
        return Handle;
    }
    void SetHandle(unsigned char handle) {
        Handle = handle;
    }
    unsigned int GetFlags() {
        return Flags;
    }
    void SetFlag(unsigned int flag) {
        Flags |= flag;
    }
    void ClearFlags() {
        Flags = 0;
    }
    unsigned short GetSortOrder() const {
        return SortOrder;
    }
    void SetSortOrder(unsigned short order) {
        SortOrder = order;
    }
    bool IsValid() {
        return Handle != 0xFF;
    }
    bool IsRead() {
        return (Flags & 4) != 0;
    }
    bool IsUnRead() {
        return (Flags & 2) != 0;
    }
    bool IsVoice();

    unsigned int GetMsgHash();
    unsigned int GetFromHash();
    unsigned int GetSubjectHash();
    unsigned int GetVoiceHash();

    unsigned char Handle;     // offset 0x0, size 0x1
    unsigned char Flags;      // offset 0x1, size 0x1
    unsigned short SortOrder; // offset 0x2, size 0x2
};

// total size: 0x27C
class CareerSettings {
  public:
    uint32 GetCurrentCar() {
        return CurrentCar;
    }
    uint8 GetCurrentBin() const {
        return CurrentBin;
    }
    void AwardOneTimeCashBonus(bool bOldSaveExists);
    const char *GetCaseFileName() {
        return CaseFileName;
    }

    bool HasCareerStarted() {
        return SpecialFlags & 1;
    }
    bool IsGameOver() {
        return SpecialFlags & 0x800;
    }
    void SetGameOver() {
        SpecialFlags |= 0x800;
    }
    bool HasRapSheet() {
        return SpecialFlags & 0x10;
    }
    void SetHasRapSheet() {
        SpecialFlags |= 0x10;
    }
    void SetHasDoneCareerIntro() {
        SpecialFlags |= 0x20;
    }
    bool HasDoneCareerIntro() {
        return SpecialFlags & 0x20;
    }
    bool HasDoneDragTutorial() {
        return SpecialFlags & 0x40;
    }
    void SetHasDoneDragTutorial() {
        SpecialFlags |= 0x40;
    }
    bool HasDoneSpeedTrapTutorial() {
        return SpecialFlags & 0x80;
    }
    void SetHasDoneSpeedTrapTutorial() {
        SpecialFlags |= 0x80;
    }
    bool HasDoneTollBoothTutorial() {
        return SpecialFlags & 0x100;
    }
    void SetHasDoneTollBoothTutorial() {
        SpecialFlags |= 0x100;
    }
    bool HasDonePursuitTutorial() {
        return SpecialFlags & 0x200;
    }
    void SetHasDonePursuitTutorial() {
        SpecialFlags |= 0x200;
    }
    bool HasDoneBountyTutorial() {
        return SpecialFlags & 0x400;
    }
    void SetHasDoneBountyTutorial() {
        SpecialFlags |= 0x400;
    }
    bool HasDoneMapLoadigTip() {
        return SpecialFlags & 0x80000;
    }
    void SetHasDoneMapLoadigTip() {
        SpecialFlags |= 0x80000;
    }
    bool HasBeatenCareer() {
        return SpecialFlags & 0x4000;
    }
    void SetHasBeatenCareer() {
        SpecialFlags |= 0x4000;
    }
    bool HasBeenBustedOnce() {
        return SpecialFlags & 0x1000;
    }
    void SetBeenBustedOnce() {
        SpecialFlags |= 0x1000;
    }
    int GetCash() {
        return CurrentCash;
    }
    void AddCash(int amount) {
        CurrentCash += amount;
    }
    SMSMessage *GetSMSMessage(unsigned int index);
    unsigned short GetSMSSortOrder();
    void SpendCash(int amount);
    void SetPlayerHasBeatenTheGame();
    int GetSaveBufferSize(bool bExcludeGameplay);
    void ResumeCareer();
    void StartNewCareer(bool bEnterGameplay);
    char *SaveUnlockData(void *save_to, void *maxptr);
    char *LoadUnlockData(void *load_from, void *maxptr);
    void Default();
    void TryAwardDemoMarker();
    void GenerateCaseFileName();
    char *SaveToBuffer(void *buffer, void *maxbuf);
    char *LoadFromBuffer(void *buffer, void *maxbuf);
    char *SaveRaceData(void *save_to, void *maxptr);
    char *SaveGameplayData(void *save_to, void *maxptr);
    char *LoadRaceData(void *load_from, void *maxptr);
    char *LoadGameplayData(void *load_from, void *maxptr);
    void SetCurrentCar(unsigned int car) {
        CurrentCar = car;
    }
    bool HasBeenAwardedDemoMarker();
    void SetAwardedDemoMarker();

    bool HasBeenAwardedBKReward() {
        return SpecialFlags & 0x2000;
    }

  public:
    uint32 CurrentCar;           // offset 0x0, size 0x4
    uint32 SpecialFlags;         // offset 0x4, size 0x4
    uint8 CurrentBin;            // offset 0x8, size 0x1
    uint32 CurrentCash;          // offset 0xC, size 0x4
    int16 AdaptiveDifficulty;    // offset 0x10, size 0x2
    SMSMessage SMSMessages[150]; // offset 0x12, size 0x258
    uint16 SMSSortOrder;         // offset 0x26A, size 0x2
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
    UserProfile();
    ~UserProfile();
    void SetProfileName(const char *pName, bool isP1);
    const char *GetProfileName();
    bool IsProfileNamed();
    void Default(int player_number, bool commit_default);
    // void CommitHighScoresPreRace(enum eHighScoresRaceTypes race_type, int is_split_screen);
    // void CommitHighScoresPostRace(enum eHighScoresRaceTypes race_type, int track, int direction, int laps, int is_split_screen,
    //                               struct FinishedRaceStatsEntry *stats);
    void CommitHighScoresPauseQuit();
    void CommitPursuitInfo(IPursuit *iPursuit, unsigned int car_FEKey, unsigned int bounty, unsigned int num_infractions);
    void IncInfration(GInfractionManager::InfractionType infrat, unsigned int car);
    void CommitServeInfractions(unsigned int car);
    void WriteProfileHash(void *bufferToHash, void *bufferToWrite, int bytes, void *maxptr);
    bool VerifyProfileHash(void *bufferToHash, void *bufferHash, int bytes);
    void SaveToBuffer(void *buffer, int size);
    bool LoadFromBuffer(void *buffer, int size, bool commit_changes, int player_id);
    int GetSaveBufferSize(bool bExcludeGameplay);

    OptionsSettings *GetOptions() {
        return &TheOptionsSettings;
    }

    CareerSettings *GetCareer() {
        return &TheCareerSettings;
    }
    HighScoresDatabase *GetHighScores() {
        return &HighScores;
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
    RaceSettings() {
        EventHash = 0;
        Default();
    }
    void Default();

    unsigned int GetSelectedCar(int player_num) {
        return SelectedCar[player_num];
    }

    void SetSelectedCar(unsigned int car, int player_num) {
        SelectedCar[player_num] = car;
    }

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
#ifdef EA_BUILD_A124
    int CarSelectFilterBits[2];
#endif
};

// total size: 0x14C
struct FEKeyboardSettings {
    FEKeyboardSettings();

    int AcceptCallbackHash;  // offset 0x0, size 0x4
    int DeclineCallbackHash; // offset 0x4, size 0x4
    int DefaultTextHash;     // offset 0x8, size 0x4
    int MaxTextLength;       // offset 0xC, size 0x4
    int Mode;                // offset 0x10, size 0x4
    char Buffer[156];        // offset 0x14, size 0x9C
    char Title[156];         // offset 0xB0, size 0x9C
};

// total size: 0x6
struct GameCompletionStats {
    GameCompletionStats();

    unsigned char m_nOverall;                 // offset 0x0, size 0x1
    unsigned char m_nCareer;                  // offset 0x1, size 0x1
    unsigned char m_nRapSheetRankings;        // offset 0x2, size 0x1
    unsigned char m_nChallenge;               // offset 0x3, size 0x1
    unsigned char m_nTotalChallengeRaces;     // offset 0x4, size 0x1
    unsigned char m_nCompletedChallengeRaces; // offset 0x5, size 0x1
};

// total size: 0xA28
class cFrontendDatabase {
  public:
    RaceSettings *GetQuickRaceSettings(GRace::Type type);
    void DefaultRaceSettings();
    void FillCustomRace(GRaceCustom *parms, RaceSettings *race);

    PlayerSettings *GetPlayerSettings(int player) {
        return &CurrentUserProfiles[0]->GetOptions()->ThePlayerSettings[player];
    }

    FEPlayerCarDB *GetPlayerCarStable(int player) {
        if (static_cast<unsigned int>(player) <= 1)
            return &CurrentUserProfiles[player]->PlayersCarStable;
        return nullptr;
    }

    CareerSettings *GetCareerSettings() {
        return CurrentUserProfiles[0]->GetCareer();
    }

    bool IsDDay() {
        return GetCareerSettings()->GetCurrentBin() >= 16;
    }

    bool IsSplitScreenMode() {
        return FEGameMode & 4 && iNumPlayers == 2;
    }

    bool IsCareerMode() {
        return FEGameMode & 1;
    }

    bool IsChallengeMode() {
        return FEGameMode & 2;
    }

    bool IsQuickRaceMode() {
        return FEGameMode & 4;
    }

    bool IsOnlineMode() {
        return FEGameMode & 8;
    }

    bool IsOptionsMode() {
        return FEGameMode & 16;
    }

    bool IsCustomizeMode() {
        return FEGameMode & 32;
    }

    bool IsLANMode() {
        return FEGameMode & 64;
    }

    bool IsProfileManagerMode() {
        return FEGameMode & 128;
    }

    bool IsCareerManagerMode() {
        return FEGameMode & 256;
    }

    bool IsRapSheetMode() {
        return FEGameMode & 512;
    }

    bool IsModeSelectMode() {
        return FEGameMode & 1024;
    }

    bool IsCarLotMode() {
        return FEGameMode & 32768;
    }

    bool IsSafehouseMode() {
        return FEGameMode & 65536;
    }

    bool IsPostRivalMode() {
        return FEGameMode & 131072;
    }

    bool IsBeatGameMode() {
        return FEGameMode & 262144;
    }

    void SetGameMode(eFEGameModes mode) {
        FEGameMode = FEGameMode | static_cast<unsigned int>(mode);
    }

    void ClearGameMode(eFEGameModes mode) {
        FEGameMode = FEGameMode & ~static_cast<unsigned int>(mode);
    }

    void ResetGameMode() {
        FEGameMode = 0;
    }

    unsigned int GetGameMode() {
        return FEGameMode;
    }

    bool IsOptionsDirty() {
        return bIsOptionsDirty;
    }

    void SetOptionsDirty(bool dirty) {
        bIsOptionsDirty = dirty;
    }

    void SetPlayersJoystickPort(int player, signed char port);

    signed char GetPlayersJoystickPort(int player) {
        return PlayerJoyports[player];
    }

    UserProfile *GetMultiplayerProfile(int player) {
        return CurrentUserProfiles[player];
    }
    UserProfile *GetUserProfile(int player) {
        return CurrentUserProfiles[player];
    }
    void CreateMultiplayerProfile(int player);
    void DeleteMultiplayerProfile(int player);

    OptionsSettings *GetOptionsSettings() {
        return CurrentUserProfiles[0]->GetOptions();
    }

    AudioSettings *GetAudioSettings() {
        return &CurrentUserProfiles[0]->GetOptions()->TheAudioSettings;
    }

    VideoSettings *GetVideoSettings() {
        return &CurrentUserProfiles[0]->GetOptions()->TheVideoSettings;
    }

    GameplaySettings *GetGameplaySettings() {
        return &CurrentUserProfiles[0]->GetOptions()->TheGameplaySettings;
    }

    GameCompletionStats GetGameCompletionStats();

    unsigned int GetChallengeHeaderHash(unsigned int index);
    unsigned int GetChallengeDescHash(unsigned int index);
    unsigned int GetBountyIconHash(unsigned int index);
    unsigned int GetBountyHeaderHash(unsigned int index);
    unsigned int GetBountyDescHash(unsigned int index);
    unsigned int GetMilestoneDescHash(unsigned int tag);
    unsigned int GetMilestoneHeaderHash(unsigned int tag);
    unsigned int GetMilestoneIconHash(unsigned int typeKey, bool active);
    void SetMilestoneDescriptionString(char *buf, int value, float number, float goal, bool isTime) const;
    bool IsMilestoneTimeFormat(int typeKey) const;
    unsigned int GetRaceIconHash(GRace::Type type);
    unsigned int GetRaceNameHash(GRace::Type type);

    void BuildCurrentRideForPlayer(int player, class RideInfo *ride);

    bool IsFinalEpicChase();
    unsigned int GetUserProfileSaveSize(bool bExcludeGameplay);
    void SaveUserProfileToBuffer(void *buffer, int size);
    void NotifyExitRaceToFrontend(eExitRacePlaces from_where);
    void AllocBackupDB(bool b);
    void DefaultProfile();
    unsigned int GetDefaultCar();
    bool LoadUserProfileFromBuffer(void *buffer, int size, int player);
    void RestoreFromBackupDB();
    void DeallocBackupDB();
    void RefreshCurrentRide();
    void NotifyDeleteCar(unsigned int handle);
    void BackupCarStable();
    bool IsCarStableDirty();
    bool IsDirty();

    bool MatchesGameMode(unsigned int mode) {
        return FEGameMode & mode;
    }

    static void *operator new(unsigned int size, unsigned int alloc_params) {
#ifdef MILESTONE_OPT
        return bMalloc(size, __FILE__, __LINE__, alloc_params);
#else
        return bMalloc(size, alloc_params);
#endif
    }

    cFrontendDatabase();
    void Default();
    void GetRandomRaceOptions(RaceSettings *race, GRace::Type type);
    unsigned int GetSafehouseIconHash(const char *name);

    unsigned char iNumPlayers;           // offset 0x0, size 0x1
    bool bComingFromBoot;                // offset 0x4, size 0x1
    bool bSavedProfileForMP;             // offset 0x8, size 0x1
    bool bProfileLoaded;                 // offset 0xC, size 0x1
    bool bIsOptionsDirty;                // offset 0x10, size 0x1
    bool bAutoSaveOverwriteConfirmed;    // offset 0x14, size 0x1
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
