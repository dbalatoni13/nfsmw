#ifndef FEDATABASE_H
#define FEDATABASE_H

#include <types.h>

#include "RaceDB.hpp"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#if ONLINE_SUPPORT
#include "Speed/Indep/Src/Online/OnlineCfg.hpp"
#endif

class GRaceCustom;

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
    MIN_CONFIG = CC_CONFIG_1,
    MAX_CONFIG = CC_CONFIG_5,
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

enum eLoadSaveGame {
    eLOADSAVE_LOAD = 0,
    eLOADSAVE_SAVE = 1,
};

enum eWorldMapItemType {
    WMIT_NONE = 0,
    WMIT_PLAYER_CAR = 1 << 0,
    WMIT_AI_RACE_CAR = 1 << 1,
    WMIT_COP_CAR = 1 << 2,
    WMIT_COP_HELI = 1 << 3,
    WMIT_TRAFFIC_CAR = 1 << 4,
    WMIT_ROADBLOCK = 1 << 5,
    WMIT_CHECKPOINT = 1 << 6,
    WMIT_CIRCUIT_RACE = 1 << 7,
    WMIT_SPRINT_RACE = 1 << 8,
    WMIT_LAP_KO_RACE = 1 << 9,
    WMIT_DRAG_RACE = 1 << 10,
    WMIT_SPEED_TRAP_RACE = 1 << 11,
    WMIT_TOLLBOOTH_RACE = 1 << 12,
    WMIT_MULTIPOINT_RACE = 1 << 13,
    WMIT_CELL_PHONE_RACE = 1 << 14,
    WMIT_RIVAL_RACE = 1 << 15,
    WMIT_CASH_GRAB_RACE = 1 << 16,
    WMIT_CASH_GRAB_SMALL = 1 << 17,
    WMIT_CASH_GRAB_MED = 1 << 18,
    WMIT_CASH_GRAB_LARGE = 1 << 19,
    WMIT_CASH_GRAB_ALL = WMIT_CASH_GRAB_SMALL | WMIT_CASH_GRAB_MED | WMIT_CASH_GRAB_LARGE,
    WMIT_SPEED_TRAP = 1 << 20,
    WMIT_SAFEHOUSE = 1 << 21,
    WMIT_SHOP = 1 << 22,
    WMIT_CAR_LOT = 1 << 23,
    WMIT_TOKEN = 1 << 24,
    WMIT_HIDING_SPOT = 1 << 25,
    WMIT_PURSUIT_BREAKER = 1 << 26,
};

enum eWorldMapView {
    WMV_NAVIGATION = 0,
    WMV_EVENT = 1,
    WMV_RACE = 2,
    WMV_PURSUIT = 3,
    NUM_WORLD_MAP_VIEWS = 4,
};

enum eWorldMapZoomLevels {
    WMZ_ALL = 0,
    WMZ_LEVEL_1 = 1,
    WMZ_LEVEL_2 = 2,
    WMZ_LEVEL_4 = 3,
    WMZ_MAX_ZOOM = 3,
    NUM_ZOOM_LEVELS = 4,
};

// total size: 0x2C
class PlayerSettings {
  public:
    PlayerSettings() {
        Default();
    }
    void Default();
    void DefaultFromOptionsScreen();
    bool operator==(const PlayerSettings &settings) const;
    void ScrollDriveCam(int dir);
    Attrib::Key GetControllerAttribs(eControllerAttribs type, bool wheel_connected) const;

    bool GaugesOn;                 // offset 0x0, size 0x1
    bool PositionOn;               // offset 0x4, size 0x1
    bool LapInfoOn;                // offset 0x8, size 0x1
    bool ScoreOn;                  // offset 0xC, size 0x1
    bool Rumble;                   // offset 0x10, size 0x1
    bool LeaderboardOn;            // offset 0x14, size 0x1
    bool TransmissionPromptOn;     // offset 0x18, size 0x1
    bool DriveWithAnalog;          // offset 0x1C, size 0x1
    eControllerConfig Config;      // offset 0x20, size 0x4
    ePlayerSettingsCameras CurCam; // offset 0x24, size 0x4
    uint8 SplitTimeType;           // offset 0x28, size 0x1
    uint8 Transmission;            // offset 0x29, size 0x1
    uint8 Handling;                // offset 0x2A, size 0x1
};

// total size: 0x20

class GameplaySettings {
  public:
    GameplaySettings() {
        Default();
    }
    void Default();
    bool IsMapItemEnabled(eWorldMapItemType type);
    void SetMapItem(eWorldMapItemType type, bool enabled);
    bool operator==(const GameplaySettings &settings) const;

    bool AutoSaveOn;            // offset 0x0, size 0x1
    bool RearviewOn;            // offset 0x4, size 0x1
    bool Damage;                // offset 0x8, size 0x1
    uint8 SpeedoUnits;          // offset 0xC, size 0x1
    uint8 RacingMiniMapMode;    // offset 0xD, size 0x1
    uint8 ExploringMiniMapMode; // offset 0xE, size 0x1
    uint32 MapItems;            // offset 0x10, size 0x4
    uint8 LastMapZoom;          // offset 0x14, size 0x1
    uint8 LastPursuitMapZoom;   // offset 0x15, size 0x1
    uint8 LastMapView;          // offset 0x16, size 0x1
    bool JumpCam;               // offset 0x18, size 0x1
    float HighlightCam;         // offset 0x1C, size 0x4
};

// total size: 0x10
class VideoSettings {
  public:
    VideoSettings() {
        Default();
    }
    void Default();
    bool operator==(const VideoSettings &settings) const;

    float FEScale;       // offset 0x0, size 0x4
    float ScreenOffsetX; // offset 0x4, size 0x4
    float ScreenOffsetY; // offset 0x8, size 0x4
    bool WideScreen;     // offset 0xC, size 0x1
};

// total size: 0x34
class AudioSettings {
  public:
    AudioSettings() {
        Default();
    }
    void Default();
    bool operator==(const AudioSettings &settings) const;
    float GetMasteredSpeechVol() {
        return SpeechVol;
    }
    float GetMasteredFEMusicVol() {
        return FEMusicVol;
    };
    float GetMasteredIGMusicVol() {
        return IGMusicVol;
    };
    float GetMasteredSoundEffectsVol() {
        return SoundEffectsVol;
    };
    float GetMasteredEngineVol() {
        return EngineVol;
    };
    float GetMasteredCarVol() {
        return CarVol;
    };
    float GetMasteredAmbientVol() {
        return AmbientVol;
    };
    float GetMasteredSpeedVol() {
        return SpeedVol;
    };

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
    OptionsSettings() : TheVideoSettings(), TheGameplaySettings(), TheAudioSettings(), ThePlayerSettings() {
        Default();
    }
    void Default();

    eOptionsCategory CurrentCategory;     // offset 0x0, size 0x4
    VideoSettings TheVideoSettings;       // offset 0x4, size 0x10
    GameplaySettings TheGameplaySettings; // offset 0x14, size 0x20
    AudioSettings TheAudioSettings;       // offset 0x34, size 0x34
    PlayerSettings ThePlayerSettings[2];  // offset 0x68, size 0x58
};

#ifdef EA_BUILD_A124
typedef uint16 FESMSHandle;
#else
typedef uint8 FESMSHandle;
#endif

enum SMSMessageFlags {
#ifdef EA_BUILD_A124
    SMS_FLAG_UNREAD = 1,
    SMS_FLAG_READ = 2,
#else
    SMS_FLAG_UNREAD = 2,
    SMS_FLAG_READ = 4,
#endif
};

// total size: 0x4
class SMSMessage {
  public:
    SMSMessage() : Handle(0xFF), Flags(0), SortOrder(0) {}
    ~SMSMessage() {}

    void SetHandle(FESMSHandle handle) {
        Handle = handle;
    }
    FESMSHandle GetHandle() {
        return Handle;
    }
    void SetFlag(uint32 flag) {
        Flags |= flag;
    }
    void ClearFlags() {
        Flags = 0;
    }
    uint32 GetFlags() {
        return Flags;
    }
    void SetSortOrder(uint16 order) {
        SortOrder = order;
    }
    uint16 GetSortOrder() {
        return SortOrder;
    }
    uint32 GetMsgHash() {
        return FEngHashString("SMS_MESSAGE_%d", Handle);
    }
    uint32 GetFromHash() {
        return FEngHashString("SMS_MESSAGE_%d_FROM", Handle);
    }
    uint32 GetVoiceHash() {
        return FEngHashString("SMS_MESSAGE_%d_VOICE", Handle);
    }
    uint32 GetSubjectHash() {
        return FEngHashString("SMS_MESSAGE_%d_SUBJECT", Handle);
    }
    bool IsUnRead() {
        return (Flags & SMS_FLAG_UNREAD) != 0;
    }
    bool IsRead() {
        return (Flags & SMS_FLAG_READ) != 0;
    }
    bool IsValid() {
        return Handle != 0xFF;
    }
    bool IsVoice();

    FESMSHandle Handle; // offset 0x0, size 0x1
    uint8 Flags;        // offset 0x1, size 0x1
    uint16 SortOrder;   // offset 0x2, size 0x2
};

enum CS_SpecialFlags {
    CS_CAREER_STARTED = 1 << 0,
    CS_ONE_TIME_CASH_BONUS = 1 << 1,
    CS_HAS_RAP_SHEET = 1 << 4,
    CS_INTRO_DONE = 1 << 5,
    CS_TUTORIAL_DRAG = 1 << 6,
    CS_TUTORIAL_SPEEDTRAP = 1 << 7,
    CS_TUTORIAL_TOLLBOOTH = 1 << 8,
    CS_TUTORIAL_PURSUIT = 1 << 9,
    CS_TUTORIAL_BOUNTY = 1 << 10,
    CS_GAME_OVER = 1 << 11,
    CS_BEEN_BUSTED_ONCE = 1 << 12,
    CS_AWARDED_BK_REWARD = 1 << 13,
    // TODO: #ifndef EA_BUILD_A124
    CS_BEATEN_CAREER = 1 << 14,
    CS_BEATEN_SPECIAL_CHALLENGE_EVENT = 1 << 15,
    CS_BEATEN_CHALLENGE_SERIES = 1 << 16,
    CS_DEMO_MARKER = 1 << 17,
    CS_CASTROL_GT = 1 << 18,
    CS_MAP_LOADING_TIP_DONE = 1 << 19,
    CS_BEEN_AWARDED_EPIC_CAR = 1 << 20,
    // #endif
};

// total size: 0x27C
class CareerSettings {
  public:
    CareerSettings() : SMSMessages() {}
    ~CareerSettings() {}

    void Default();
    const char *GetCaseFileName() {
        return CaseFileName;
    }
    void SpendCash(int amount);
    void AwardCash(int amount) {
        CurrentCash += amount;
    }
    int GetCash() {
        return CurrentCash;
    }
    SMSMessage *GetSMSMessage(uint32 index);
    uint16 GetSMSSortOrder();
    char *SaveToBuffer(void *buffer, void *maxbuf);
    char *LoadFromBuffer(void *buffer, void *maxbuf);
    int32 GetSaveBufferSize(bool bExcludeGameplay);
    void ResumeCareer();
    void StartNewCareer(bool bEnterGameplay);
    // TODO: #ifndef EA_BUILD_A124
    bool HasBeenAwardedDemoMarker() {
        return SpecialFlags & CS_DEMO_MARKER;
    }
    void SetAwardedDemoMarker() {
        SpecialFlags |= CS_DEMO_MARKER;
    }
    // #endif
    bool HasCareerStarted() {
        return SpecialFlags & CS_CAREER_STARTED;
    }
    // TODO: #ifndef EA_BUILD_A124
    void TryAwardDemoMarker();
    // #endif
    void SetGameOver() {
        SpecialFlags |= CS_GAME_OVER;
    }
    bool IsGameOver() {
        return SpecialFlags & CS_GAME_OVER;
    }
    bool HasCashBonusBeenAwarded() {
        return SpecialFlags & CS_ONE_TIME_CASH_BONUS;
    }
    void AwardOneTimeCashBonus(bool bGiveCashBonus);
    bool HasBeenBustedOnce() {
        return SpecialFlags & CS_BEEN_BUSTED_ONCE;
    }
    void SetBeenBustedOnce() {
        SpecialFlags |= CS_BEEN_BUSTED_ONCE;
    }
    bool HasBeenAwardedBKReward() {
        return SpecialFlags & CS_AWARDED_BK_REWARD;
    }
    void SetAwardedBKReward() {
        SpecialFlags |= CS_AWARDED_BK_REWARD;
    }
    bool HasBeenAwardedCastrolGT() {
        return SpecialFlags & CS_CASTROL_GT;
    }
    void SetAwardedCastrolGT() {
        SpecialFlags |= CS_CASTROL_GT;
    }
    void SetAdaptiveDifficulty(float difficulty) {
        // TODO
        AdaptiveDifficulty = static_cast<int16>(bClamp(difficulty, 0.0f, 1.0f));
    }
    float GetAdaptiveDifficulty() {
        // TODO
        return bClamp(static_cast<float>(AdaptiveDifficulty), 0.0f, 1.0f);
    }
    uint8 GetCurrentBin() {
        return CurrentBin;
    }
    void SetCurrentBin(uint8 bin) {
        CurrentBin = bin;
    }
    uint32 GetCurrentCar() {
        return CurrentCar;
    }
    void SetCurrentCar(uint32 car) {
        CurrentCar = car;
    }
    void SetHasRapSheet() {
        SpecialFlags |= CS_HAS_RAP_SHEET;
    }
    bool HasRapSheet() {
        return SpecialFlags & CS_HAS_RAP_SHEET;
    }
    bool HasDoneCareerIntro() {
        return SpecialFlags & CS_INTRO_DONE;
    }
    void SetHasDoneCareerIntro() {
        SpecialFlags |= CS_INTRO_DONE;
    }
    bool HasDoneDragTutorial() {
        return SpecialFlags & CS_TUTORIAL_DRAG;
    }
    bool HasDoneSpeedTrapTutorial() {
        return SpecialFlags & CS_TUTORIAL_SPEEDTRAP;
    }
    bool HasDoneTollBoothTutorial() {
        return SpecialFlags & CS_TUTORIAL_TOLLBOOTH;
    }
    bool HasDonePursuitTutorial() {
        return SpecialFlags & CS_TUTORIAL_PURSUIT;
    }
    bool HasDoneBountyTutorial() {
        return SpecialFlags & CS_TUTORIAL_BOUNTY;
    }
    void SetHasDoneDragTutorial() {
        SpecialFlags |= CS_TUTORIAL_DRAG;
    }
    void SetHasDoneSpeedTrapTutorial() {
        SpecialFlags |= CS_TUTORIAL_SPEEDTRAP;
    }
    void SetHasDoneTollBoothTutorial() {
        SpecialFlags |= CS_TUTORIAL_TOLLBOOTH;
    }
    void SetHasDonePursuitTutorial() {
        SpecialFlags |= CS_TUTORIAL_PURSUIT;
    }
    void SetHasDoneBountyTutorial() {
        SpecialFlags |= CS_TUTORIAL_BOUNTY;
    }
    // TODO: #ifndef EA_BUILD_A124
    bool HasBeatenCareer() {
        return SpecialFlags & CS_BEATEN_CAREER;
    }
    void SetHasBeatenCareer() {
        SpecialFlags |= CS_BEATEN_CAREER;
    }
    bool HasBeatenSpecialChallengeEvent() {
        return SpecialFlags & CS_BEATEN_SPECIAL_CHALLENGE_EVENT;
    }
    void SetHasBeatenSpecialChallengeEvent() {
        SpecialFlags |= CS_BEATEN_SPECIAL_CHALLENGE_EVENT;
    }
    bool HasBeatenChallengeSeries() {
        return SpecialFlags & CS_BEATEN_CHALLENGE_SERIES;
    }
    void SetHasBeatenChallengeSeries() {
        SpecialFlags |= CS_BEATEN_CHALLENGE_SERIES;
    }
    bool HasDoneMapLoadigTip() {
        return SpecialFlags & CS_MAP_LOADING_TIP_DONE;
    }
    void SetHasDoneMapLoadigTip() {
        SpecialFlags |= CS_MAP_LOADING_TIP_DONE;
    }
    bool HasBeenAwardedEpicCar() {
        return SpecialFlags & CS_BEEN_AWARDED_EPIC_CAR;
    }
    void SetHasBeenAwardedEpicCar() {
        SpecialFlags |= CS_BEEN_AWARDED_EPIC_CAR;
    }
    void SetPlayerHasBeatenTheGame();
    // #endif

  private:
    char *SaveRaceData(void *save_to, void *maxptr);
    char *SaveUnlockData(void *save_to, void *maxptr);
    char *SaveGameplayData(void *save_to, void *maxptr);
    char *LoadRaceData(void *load_from_here, void *maxptr);
    char *LoadUnlockData(void *load_from_here, void *maxptr);
    char *LoadGameplayData(void *load_from_here, void *maxptr);
    void GenerateCaseFileName();

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
    uint32 SongIndex;       // offset 0x0, size 0x4
    uint8 PlayabilityField; // offset 0x4, size 0x1
};

// total size: 0x9CF4
class UserProfile {
  public:
    UserProfile();
    ~UserProfile();
    OptionsSettings *GetOptions() {
        return &TheOptionsSettings;
    }
    CareerSettings *GetCareer() {
        return &TheCareerSettings;
    }
    void WriteProfileHash(void *bufferToHash, void *bufferToWrite, int bytes, void *maxptr);
    bool VerifyProfileHash(void *bufferToHash, void *bufferHash, int bytes);
    HighScoresDatabase *GetHighScores() {
        return &HighScores;
    }
    bool IsProfileNamed();
    void SetProfileName(const char *pName, bool isP1);
    const char *GetProfileName();
    void Default(int player_number, bool commit_default);
    void SaveToBuffer(void *buffer, int size);
    bool LoadFromBuffer(void *buffer, int size, bool commit_changes, int player_id);
    int32 GetSaveBufferSize(bool bExcludeGameplay);
    void CommitHighScoresPreRace(eHighScoresRaceTypes race_type, int is_split_screen);
    void CommitHighScoresPostRace(eHighScoresRaceTypes race_type, int track, int direction, int laps, int is_split_screen,
                                  FinishedRaceStatsEntry *stats);
    void CommitHighScoresPauseQuit();
    void CommitPursuitInfo(IPursuit *iPursuit, uint32 car_FEKey, uint32 bounty, unsigned int num_infractions);
    void IncInfration(GInfractionManager::InfractionType infrat, uint32 car);
    void CommitServeInfractions(uint32 car);

  private:
    char m_aProfileName[32];            // offset 0x0, size 0x20
    bool m_bNamed;                      // offset 0x20, size 0x1
    OptionsSettings TheOptionsSettings; // offset 0x24, size 0xC0
    CareerSettings TheCareerSettings;   // offset 0xE4, size 0x27C
  public:
    JukeboxEntry Playlist[30];                  // offset 0x360, size 0xF0
    FEPlayerCarDB PlayersCarStable;             // offset 0x450, size 0x8CC8
    bool CareerModeHasBeenCompletedAtLeastOnce; // offset 0x9118, size 0x1
    // int a[210];
    HighScoresDatabase HighScores; // offset 0x911C, size 0xBD8
};

// total size: 0x14C
struct FEKeyboardSettings {
    FEKeyboardSettings();

    int32 AcceptCallbackHash;  // offset 0x0, size 0x4
    int32 DeclineCallbackHash; // offset 0x4, size 0x4
    int32 DefaultTextHash;     // offset 0x8, size 0x4
    int MaxTextLength;         // offset 0xC, size 0x4
    int Mode;                  // offset 0x10, size 0x4
    char Buffer[156];          // offset 0x14, size 0x9C
    char Title[156];           // offset 0xB0, size 0x9C
};

// total size: 0x6
struct GameCompletionStats {
    GameCompletionStats();

    uint8 m_nOverall;                 // offset 0x0, size 0x1
    uint8 m_nCareer;                  // offset 0x1, size 0x1
    uint8 m_nRapSheetRankings;        // offset 0x2, size 0x1
    uint8 m_nChallenge;               // offset 0x3, size 0x1
    uint8 m_nTotalChallengeRaces;     // offset 0x4, size 0x1
    uint8 m_nCompletedChallengeRaces; // offset 0x5, size 0x1
};

// total size: 0xA28
class cFrontendDatabase {
  public:
    static void *operator new(size_t size, const char *debug_name) {
        return bMalloc(size, debug_name, 0, GetVirtualMemoryAllocParams());
    }

    static void operator delete(void *ptr) {
        bFree(ptr);
    }

  private:
    eHighScoresRaceTypes CalcRaceTypeForHighScores();

  public:
    cFrontendDatabase();
    void Default();
    void DefaultProfile();
    void DefaultRaceSettings();
    void RestartDemoCareer();
    void RefreshCurrentRide();
    void SetPlayersJoystickPort(int player, int8 joy_port);
    int8 GetPlayersJoystickPort(int player) {
        return PlayerJoyports[player];
    }
    UserProfile *GetMultiplayerProfile(int player) {
        return CurrentUserProfiles[player];
    }
    void CreateMultiplayerProfile(int player);
    void DeleteMultiplayerProfile(int player);
    OptionsSettings *GetOptionsSettings() {
        return CurrentUserProfiles[0]->GetOptions();
    }
    VideoSettings *GetVideoSettings() {
        return &CurrentUserProfiles[0]->GetOptions()->TheVideoSettings;
    }
    GameplaySettings *GetGameplaySettings() {
        return &CurrentUserProfiles[0]->GetOptions()->TheGameplaySettings;
    }
    AudioSettings *GetAudioSettings() {
        return &CurrentUserProfiles[0]->GetOptions()->TheAudioSettings;
    }
    PlayerSettings *GetPlayerSettings(int player) {
        return &CurrentUserProfiles[0]->GetOptions()->ThePlayerSettings[player];
    }
    CareerSettings *GetCareerSettings() {
        return CurrentUserProfiles[0]->GetCareer();
    }
    GameCompletionStats GetGameCompletionStats();
    uint32 GetChallengeHeaderHash(uint32 hal_id);
    uint32 GetChallengeDescHash(uint32 hal_id);
    uint32 GetBountyHeaderHash(uint32 hal_id);
    uint32 GetBountyDescHash(uint32 hal_id);
    uint32 GetBountyIconHash(uint32 hal_id);
    uint32 GetMilestoneDescHash(uint32 hal_id);
    uint32 GetMilestoneHeaderHash(uint32 hal_id);
    uint32 GetMilestoneIconHash(uint32 type, bool isMilestone);
    void SetMilestoneDescriptionString(char *const outputStr, const int milestoneType, float currVal, const float goalVal,
                                       const bool showCurrVal) const;
    bool IsMilestoneTimeFormat(const int milestoneType) const;
    uint32 GetRaceNameHash(GRace::Type raceType);
    uint32 GetRaceIconHash(GRace::Type raceType);
    uint32 GetSafehouseIconHash(const char *safehouseType);
    RaceSettings *GetQuickRaceSettings(GRace::Type type);
    void NotifyDeleteCar(uint32 handle);
    GRaceParameters *GetRandomRace(GRace::Type type);
    void GetRandomRaceOptions(RaceSettings *race, GRace::Type type);
    void FillCustomRace(GRaceCustom *parms, RaceSettings *race);
    void SetGameMode(eFEGameModes mode) {
        FEGameMode = FEGameMode | static_cast<uint32>(mode);
    }
    void ClearGameMode(eFEGameModes mode) {
        FEGameMode = FEGameMode & ~static_cast<uint32>(mode);
    }
    void ResetGameMode() {
        FEGameMode = 0;
    }
    void BackupCarStable();
    bool IsCarStableDirty();
    void AllocBackupDB(bool bForce);
    void DeallocBackupDB();
    void RestoreFromBackupDB();
    bool IsDirty();
    bool IsDDay() {
        return GetCareerSettings()->GetCurrentBin() >= 16;
    }
    bool IsFinalEpicChase();
    void SetOptionsDirty(bool dirty) {
        bIsOptionsDirty = dirty;
    }
    bool IsOptionsDirty() {
        return bIsOptionsDirty;
    }
    bool IsSplitScreenMode() {
        return FEGameMode & eFE_GAME_MODE_QUICK_RACE && iNumPlayers == 2;
    }
    bool IsQuickRaceMode() {
        return FEGameMode & eFE_GAME_MODE_QUICK_RACE;
    }
    bool IsCareerMode() {
        return FEGameMode & eFE_GAME_MODE_CAREER;
    }
    bool IsChallengeMode() {
        return FEGameMode & eFE_GAME_MODE_CHALLENGE;
    }
    bool IsOnlineMode() {
        return FEGameMode & eFE_GAME_MODE_ONLINE;
    }
    bool IsOnlineCustomizeMode() {
        return (FEGameMode & (eFE_GAME_MODE_ONLINE | eFE_GAME_MODE_CUSTOMIZE)) == (eFE_GAME_MODE_ONLINE | eFE_GAME_MODE_CUSTOMIZE);
    }
    bool IsCustomizeMode() {
        return FEGameMode & eFE_GAME_MODE_CUSTOMIZE;
    }
    bool IsOptionsMode() {
        return FEGameMode & eFE_GAME_MODE_OPTIONS;
    }
    bool IsLANMode() {
        return FEGameMode & eFE_GAME_MODE_LAN;
    }
    bool IsModeSelectMode() {
        return FEGameMode & eFE_GAME_MODE_MODE_SELECT;
    }
    bool IsRapSheetMode() {
        return FEGameMode & eFE_GAME_MODE_RAP_SHEET;
    }
    bool IsProfileManagerMode() {
        return FEGameMode & eFE_GAME_MODE_PROFILE_MANAGER;
    }
    bool IsCareerManagerMode() {
        return FEGameMode & eFE_GAME_MODE_CAREER_MANAGER;
    }
    bool IsCarLotMode() {
        return FEGameMode & eFE_GAME_MODE_CAR_LOT;
    }
    bool IsSafehouseMode() {
        return FEGameMode & eFE_GAME_MODE_SAFEHOUSE;
    }
    bool IsPostRivalMode() {
        return FEGameMode & eFE_GAME_MODE_POST_RIVAL;
    }
    bool IsBeatGameMode() {
        return FEGameMode & eFE_GAME_MODE_BEAT_GAME;
    }
    bool MatchesGameMode(uint32 mode) {
        if (mode != eFE_GAME_MODE_ALL) {
            return FEGameMode == mode;
        }

        return true;
    }
    uint32 GetGameMode() {
        return FEGameMode;
    }
    bool IsAutoSave() {
        return GetGameplaySettings()->AutoSaveOn;
    }
    void SetAutoSave(bool flag) {}
#if ONLINE_SUPPORT
    cOnlineSettings *GetOnlineSettings() {
        return &OnlineSettings;
    };
    OnlineCreateUserSettings *GetOnlineCreateUserSettings() {
        return &mOnlineCreateUserSettings;
    };
#endif
    FEKeyboardSettings *GetFEKeyboardSettings() {}
    FEPlayerCarDB *GetPlayerCarStable(int player) {
        if (player == 0 || player == 1)
            return &CurrentUserProfiles[player]->PlayersCarStable;
        return nullptr;
    }
    FECarRecord *GetPlayerCarRecordByHandle(int player, int handle) {
        return GetPlayerCarStable(player)->GetCarRecordByHandle(handle);
    }
    void BuildCurrentRideForPlayer(int player, class RideInfo *ride);
    void RepaintSecondStable();
    UserProfile *GetUserProfile(int player) {
        return CurrentUserProfiles[player];
    }
    void NotifyStartNewRace();
    // void NotifyTheRaceIsOver(Race *race); // STRIPPED with struct Race
    void NotifyRestartRace();
    void NotifyRestartEvent();
    void NotifyExitRaceToFrontend(eExitRacePlaces from_where);
    void NotifyChangeToNextRace();
    bool CanAdvanceToNextRace();
    bool IsThereANextRace();
    bool IsCurrentRaceEventInCareerMode();
    uint32 GetDefaultCar();
    int32 GetUserProfileSaveSize(bool bExcludeGameplay);
    void SaveUserProfileToBuffer(void *buffer, int32 bufsize);
    bool LoadUserProfileFromBuffer(void *buffer, int32 bufsize, int player);
    bool CanCheatToUnlock();

    uint8 iNumPlayers;       // offset 0x0, size 0x1
    bool bComingFromBoot;    // offset 0x4, size 0x1
    bool bSavedProfileForMP; // offset 0x8, size 0x1
    bool bProfileLoaded;     // offset 0xC, size 0x1
    bool bIsOptionsDirty;    // offset 0x10, size 0x1
#ifndef EA_BUILD_A124
    bool bAutoSaveOverwriteConfirmed; // offset 0x14, size 0x1
#endif
    uint32 iDefaultStableHash;           // offset 0x18, size 0x4
    int8 PlayerJoyports[2];              // offset 0x1C, size 0x2
    UserProfile *CurrentUserProfiles[2]; // offset 0x20, size 0x8
    GRace::Type RaceMode;                // offset 0x28, size 0x4
  private:
    RaceSettings TheQuickRaceSettings[11]; // offset 0x2C, size 0x18C
  public:
    char *m_pCarStableBackup; // offset 0x1B8, size 0x4
    char *m_pDBBackup;        // offset 0x1BC, size 0x4
  private:
    uint32 FEGameMode; // offset 0x1C0, size 0x4
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

char *SaveSomeData(void *save_to, void *save_from, int bytes, void *maxptr);
char *LoadSomeData(void *load_to, void *load_from, int bytes, void *maxptr);

void InitFrontendDatabase();
int GetMikeMannBuild();

#endif
