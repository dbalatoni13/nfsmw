#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern unsigned int FEngHashString(const char *, ...);
extern eLanguages GetCurrentLanguage();
extern EAXSound *g_pEAXSound;

const char* UserProfile::GetProfileName() {}

bool UserProfile::IsProfileNamed() {
    return m_bNamed;
}

SMSMessage *CareerSettings::GetSMSMessage(unsigned int index) {
    if (index < 0x96) {
        return &SMSMessages[index];
    }
    return nullptr;
}

unsigned short CareerSettings::GetSMSSortOrder() {
    SMSSortOrder = SMSSortOrder + 1;
    return SMSSortOrder;
}

void CareerSettings::SpendCash(int amount) {
    if (static_cast<unsigned int>(amount) > CurrentCash) {
        CurrentCash = 0;
        return;
    }
    CurrentCash = CurrentCash - amount;
}

void CareerSettings::AwardOneTimeCashBonus(bool bOldSaveExists) {
    SpecialFlags = SpecialFlags | 2;
    if (!bOldSaveExists) {
        return;
    }
    CurrentCash = CurrentCash + 10000;
}

void CareerSettings::SetPlayerHasBeatenTheGame() {
    SpecialFlags = SpecialFlags | 0x4000;
}

int CareerSettings::GetSaveBufferSize(bool bExcludeGameplay) {
    if (bExcludeGameplay) {
        return 0x735;
    }
    return 0x59F9;
}

bool GameplaySettings::IsMapItemEnabled(eWorldMapItemType type) {
    if ((MapItems & type) != 0) {
        return true;
    }
    return false;
}

void GameplaySettings::SetMapItem(eWorldMapItemType type, bool enabled) {
    if (enabled) {
        MapItems = MapItems | type;
        return;
    }
    MapItems = MapItems & ~type;
}

void VideoSettings::Default() {
    FEScale = 1.0f;
    ScreenOffsetX = 0.0f;
    ScreenOffsetY = 0.0f;
    WideScreen = 0;
}

RaceSettings *cFrontendDatabase::GetQuickRaceSettings(GRace::Type type) {
    if (static_cast<int>(type) < 11) {
        return &TheQuickRaceSettings[type];
    }
    return &TheQuickRaceSettings[RaceMode];
}

unsigned int cFrontendDatabase::GetUserProfileSaveSize(bool bExcludeGameplay) {
    return CurrentUserProfiles[0]->GetSaveBufferSize(bExcludeGameplay);
}

void cFrontendDatabase::SaveUserProfileToBuffer(void *buffer, int size) {
    CurrentUserProfiles[0]->SaveToBuffer(buffer, size);
}

unsigned int cFrontendDatabase::GetChallengeHeaderHash(unsigned int index) {
    return FEngHashString("CHALLENGE_HEADER_%d", index);
}

unsigned int cFrontendDatabase::GetChallengeDescHash(unsigned int index) {
    return FEngHashString("CHALLENGE_DESCRIPTION_%d", index);
}

unsigned int cFrontendDatabase::GetBountyIconHash(unsigned int index) {
    if (index < 5) {
        return 0x8A21B882;
    }
    if (index < 9) {
        return 0x895EC0AE;
    }
    return 0x9129E7FB;
}

unsigned int cFrontendDatabase::GetBountyHeaderHash(unsigned int index) {
    return FEngHashString("BOUNTY_HEADER_%d", index);
}

unsigned int cFrontendDatabase::GetBountyDescHash(unsigned int index) {
    return FEngHashString("BOUNTY_DESCRIPTION_%d", index);
}

void RaceSettings::Default() {
    NumOpponents = 3;
    CatchUp = true;
    TrafficDensity = 1;
    NumLaps = 2;
    IsLapKO = false;
    AISkill = 1;
    CopDensity = 1;
    CopsOn = false;
    TrackDirection = 0;
    for (int i = 0; i < 2; i++) {
        SelectedCar[i] = 0;
    }
    RegionFilterBits = 3;
}

void cFrontendDatabase::SetPlayersJoystickPort(int player, signed char port) {
    if (port == -1 && PlayerJoyports[player] != -1) {
        cFEngJoyInput::mInstance->SetRequiredJoy(static_cast<JoystickPort>(PlayerJoyports[player]), false);
    }
    PlayerJoyports[player] = port;
}

static int MikeMannBuild;

int GetMikeMannBuild() {
    return MikeMannBuild;
}

static bool IsCollectorsEdition;

bool GetIsCollectorsEdition() {
    return IsCollectorsEdition;
}

void PlayerSettings::Default() {
    Handling = 1;
    CurCam = PSC_DEFAULT;
    Transmission = 0;
    GaugesOn = 1;
    PositionOn = 1;
    LapInfoOn = 1;
    ScoreOn = 1;
    LeaderboardOn = 1;
    TransmissionPromptOn = 1;
    Rumble = 1;
    DriveWithAnalog = 1;
    Config = CC_CONFIG_1;
    SplitTimeType = 0;
}

bool PlayerSettings::operator==(const PlayerSettings& rhs) const {
    return bMemCmp(this, &rhs, 0x2C) == 0;
}

void PlayerSettings::DefaultFromOptionsScreen() {
    int savedDriveWithAnalog = DriveWithAnalog;
    eControllerConfig savedConfig = Config;
    int savedRumble = Rumble;
    Default();
    Config = savedConfig;
    DriveWithAnalog = savedDriveWithAnalog;
    Rumble = savedRumble;
}

void GameplaySettings::Default() {
    AutoSaveOn = 1;
    RearviewOn = 1;
    Damage = 1;
    RacingMiniMapMode = 1;
    LastMapZoom = 1;
    JumpCam = 1;
    MapItems = static_cast<unsigned int>(-1);
    LastPursuitMapZoom = 2;
    LastMapView = 0;
    HighlightCam = 127.5f;
    ExploringMiniMapMode = 0;
    eLanguages lang = GetCurrentLanguage();
    if (lang) {
        SpeedoUnits = 1;
    } else {
        SpeedoUnits = 0;
    }
}

bool GameplaySettings::operator==(const GameplaySettings& rhs) const {
    return bMemCmp(this, &rhs, 0x20) == 0;
}

bool VideoSettings::operator==(const VideoSettings& rhs) const {
    return bMemCmp(this, &rhs, 0x10) == 0;
}

void AudioSettings::Default() {
    AudioMode = 2;
    AmbientVol = 1.0f;
    CarVol = 1.0f;
    EngineVol = 1.0f;
    SoundEffectsVol = 1.0f;
    FEMusicVol = 0.8f;
    SpeechVol = 1.0f;
    MasterVol = 1.0f;
    SpeedVol = 1.0f;
    IGMusicVol = 0.8f;
    AudioMode = g_pEAXSound->GetDefaultPlatformAudioMode();
    PlayState = 0;
    EATraxMode = 1;
    InteractiveMusicMode = 1;
}

void OptionsSettings::Default() {
    CurrentCategory = static_cast<eOptionsCategory>(0);
    TheVideoSettings.Default();
    TheAudioSettings.Default();
    TheGameplaySettings.Default();
    ThePlayerSettings[0].Default();
    ThePlayerSettings[1].Default();
}

char *SaveSomeData(void *save_to, void *save_from, int bytes, void *maxptr) {
    if (reinterpret_cast<unsigned int>(save_to) + bytes <= reinterpret_cast<unsigned int>(maxptr)) {
        bMemCpy(save_to, save_from, bytes);
        save_to = static_cast<char *>(save_to) + bytes;
    }
    return static_cast<char *>(save_to);
}

char *LoadSomeData(void *load_to, void *load_from, int bytes, void *maxptr) {
    if (reinterpret_cast<unsigned int>(load_from) + bytes <= reinterpret_cast<unsigned int>(maxptr)) {
        bMemCpy(load_to, load_from, bytes);
    }
    return static_cast<char *>(load_from) + bytes;
}

FEKeyboardSettings::FEKeyboardSettings() {
    AcceptCallbackHash = 0xAE83B9DB;
    DeclineCallbackHash = 0x6A97B51F;
    MaxTextLength = 64;
    Buffer[0] = 0;
    DefaultTextHash = 0;
    Mode = 0;
}

unsigned int cFrontendDatabase::GetMilestoneHeaderHash(unsigned int tag) {
    return FEngHashString("BLACKLIST_PURSUIT_MILESTONES_%02d_SHORT", tag);
}

unsigned int cFrontendDatabase::GetMilestoneDescHash(unsigned int tag) {
    return FEngHashString("BLACKLIST_PURSUIT_MILESTONES_%02d", tag);
}

bool cFrontendDatabase::IsMilestoneTimeFormat(int typeKey) const {
    if (typeKey == 0x33fa23a || typeKey == 0x5392e4fd) {
        return true;
    }
    return false;
}

GameCompletionStats::GameCompletionStats() {
    bMemSet(this, 0, sizeof(GameCompletionStats));
}

void cFrontendDatabase::NotifyExitRaceToFrontend(eExitRacePlaces from_where) {
    PostRaceOptionChosen = static_cast<ePostRaceOptions>(1);
    if (from_where == EXIT_RACE_FROM_PAUSE) {
        CurrentUserProfiles[0]->CommitHighScoresPauseQuit();
    }
}

void cFrontendDatabase::DeallocBackupDB() {
    if (m_pDBBackup) {
        bFree(m_pDBBackup);
        m_pDBBackup = nullptr;
    }
}

int UserProfile::GetSaveBufferSize(bool bExcludeGameplay) {
    int size = TheCareerSettings.GetSaveBufferSize(bExcludeGameplay) + 0x1e4;
    return size + PlayersCarStable.GetSaveBufferSize() + 0xc18;
}

unsigned int GetFECarNameHashFromFEKey(unsigned int feKey) {
    if (!feKey) {
        return 0;
    }
    FECarRecord rec;
    rec.FEKey = feKey;
    return rec.GetNameHash();
}

void cFrontendDatabase::AllocBackupDB(bool bForce) {
    if (!m_pDBBackup && bForce) {
        m_pDBBackup = static_cast<char *>(bMalloc(GetUserProfileSaveSize(false), 0x40));
        SaveUserProfileToBuffer(m_pDBBackup, GetUserProfileSaveSize(false));
    }
}

void cFrontendDatabase::BackupCarStable() {
    if (!m_pCarStableBackup) {
        m_pCarStableBackup = static_cast<char *>(bMalloc(GetPlayerCarStable(0)->GetSaveBufferSize(), 0));
        bMemCpy(m_pCarStableBackup, GetPlayerCarStable(0), GetPlayerCarStable(0)->GetSaveBufferSize());
    }
}

bool cFrontendDatabase::IsCarStableDirty() {
    if (!m_pCarStableBackup) {
        return false;
    }
    bool result = bMemCmp(m_pCarStableBackup, GetPlayerCarStable(0), GetPlayerCarStable(0)->GetSaveBufferSize()) != 0;
    bFree(m_pCarStableBackup);
    m_pCarStableBackup = nullptr;
    return result;
}