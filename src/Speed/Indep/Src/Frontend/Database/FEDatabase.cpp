#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GMilestone.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audiosystem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
typedef int PathEventEnum;
#include "Speed/Indep/Src/Generated/AttribSys/Classes/music.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern unsigned int FEngHashString(const char *, ...);
extern eLanguages GetCurrentLanguage();
extern EAXSound *g_pEAXSound;
extern unsigned int bCalculateCrc32(const void *data, int size, unsigned int prev_crc32);
extern int SkipFE;
extern int SkipFESplitScreen;
extern int g_MaxSongs;
extern void InitializeEATrax(bool breset);

namespace Sound {

struct stSongInfo {
    char *SongName;
    char *Artist;
    char *Album;
    char *DefPlay;
    int PathEvent;
};

} // namespace Sound

typedef std::vector<Sound::stSongInfo *> SongInfoList;

extern SongInfoList Songs;

const char* UserProfile::GetProfileName() {}

UserProfile::UserProfile() {
    TheOptionsSettings.TheVideoSettings.Default();
    TheOptionsSettings.TheGameplaySettings.Default();
    TheOptionsSettings.TheAudioSettings.Default();
    for (int i = 0; i < 2; i++) {
        TheOptionsSettings.ThePlayerSettings[i].Default();
    }
    TheOptionsSettings.Default();
    for (int i = 0; i < 150; i++) {
        TheCareerSettings.SMSMessages[i].Handle = 0xFF;
        TheCareerSettings.SMSMessages[i].Flags = 0;
        TheCareerSettings.SMSMessages[i].SortOrder = 0;
    }
    for (int i = 0; i < 5; i++) {
        bMemSet(&HighScores.TopEvadedPursuitScores[i], 0, 0x38);
    }
    bMemSet(&HighScores.CareerPursuitDetails, 0, 0x20);
    for (int i = 0; i < 10; i++) {
        bMemSet(&HighScores.BestPursuitRankings[i], 0, 8);
    }
    bMemSet(&HighScores.CostToStateDetails, 0, 0x20);
}

UserProfile::~UserProfile() {
}

bool UserProfile::IsProfileNamed() {
    return m_bNamed;
}

void UserProfile::Default(int player_number, bool commit_default) {
    static bool song_info_loaded = false;

    if (!commit_default) {
        SetProfileName(nullptr, true);
    } else {
        SetProfileName(nullptr, false);
    }

    PlayersCarStable.Default();

    if (!commit_default) {
        TheOptionsSettings.Default();
        TheCareerSettings.Default();
        HighScores.Default();
        CareerModeHasBeenCompletedAtLeastOnce = false;

        if (!song_info_loaded) {
            song_info_loaded = true;

            Attrib::Gen::audiosystem *playlist_atrs = new Attrib::Gen::audiosystem(0x7E4B0ED2, 0, nullptr);
            if (playlist_atrs->IsValid()) {
                Attrib::Gen::audiosystem licensed_music(playlist_atrs->LicensedMusic(0), 0, nullptr);
                g_MaxSongs = licensed_music.Num_PFMapping();

                for (int i = 0; i < static_cast<int>(Songs.size()); i++) {
                    delete Songs[i];
                }
                Songs.clear();

                if (static_cast<int>(Songs.capacity()) < g_MaxSongs) {
                    Songs.reserve(g_MaxSongs);
                }

                for (int i = 0; i < g_MaxSongs; i++) {
                    Sound::stSongInfo *newsong = new (__FILE__, __LINE__) Sound::stSongInfo;
                    Attrib::Gen::music currsong(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
                    const char *tmpSongName;

                    currsong.ChangeWithDefault(licensed_music.PFMapping(i));

                    tmpSongName = currsong.SongName().GetString();
                    newsong->SongName = const_cast<char *>(tmpSongName ? tmpSongName : "");
                    tmpSongName = currsong.Album().GetString();
                    newsong->Album = const_cast<char *>(tmpSongName ? tmpSongName : "");
                    tmpSongName = currsong.Artist().GetString();
                    newsong->Artist = const_cast<char *>(tmpSongName ? tmpSongName : "");
                    tmpSongName = currsong.DefPlay().GetString();
                    newsong->DefPlay = const_cast<char *>(tmpSongName ? tmpSongName : "");
                    newsong->PathEvent = currsong.PathEvent();
                    Songs.push_back(newsong);
                }
            }
        }

        for (int i = 0; i < g_MaxSongs; i++) {
            Playlist[i].SongIndex = i;

            unsigned char playability = 0;
            Sound::stSongInfo *song = Songs[i];
            if (song) {
                if (bStrCmp(song->DefPlay, "FE") == 0) {
                    playability = 1;
                } else if (bStrCmp(song->DefPlay, "IG") == 0) {
                    playability = 2;
                } else if (bStrCmp(song->DefPlay, "AL") == 0) {
                    playability = 3;
                }
            }
            Playlist[i].PlayabilityField = playability;
        }

        InitializeEATrax(true);
    }

    PlayersCarStable.AwardBonusCars();
    TheCareerSettings.TryAwardDemoMarker();
}

SMSMessage *CareerSettings::GetSMSMessage(unsigned int index) {
    if (index < 0x96) {
        return &SMSMessages[index];
    }
    return nullptr;
}

bool SMSMessage::IsVoice() {
    switch (Handle) {
    case 0x6E:
    case 0x6F:
    case 0x78:
    case 0x79:
    case 0x7A:
    case 0x7B:
    case 0x7C:
    case 0x7D:
    case 0x7E:
    case 0x7F:
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
    case 0x88:
    case 0x89:
        return false;
    default:
        return true;
    }
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
    SetHasBeatenCareer();
}

int CareerSettings::GetSaveBufferSize(bool bExcludeGameplay) {
    int size = TheFEMarkerManager.GetSaveBufferSize() + 0x441;
    if (!bExcludeGameplay) {
        size += 0x52C4;
    }
    return size;
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

void cFrontendDatabase::FillCustomRace(GRaceCustom *parms, RaceSettings *race) {
    if (!race) {
        return;
    }
    if (!parms) {
        return;
    }
    parms->SetCatchUp(race->CatchUp);
    parms->SetCopsEnabled(race->CopsOn);
    if (race->CopsOn) {
        parms->SetHeatLevel(race->CopDensity);
    }
    parms->SetDifficulty(static_cast<GRace::Difficulty>(race->AISkill));
    parms->SetNumLaps(race->NumLaps);
    parms->SetNumOpponents(race->NumOpponents);
    switch (race->TrafficDensity) {
    case 1:
        parms->SetTrafficDensity(10);
        break;
    case 2:
        parms->SetTrafficDensity(30);
        break;
    case 3:
        parms->SetTrafficDensity(50);
        break;
    default:
        parms->SetTrafficDensity(0);
        break;
    }
    parms->SetReversed(race->TrackDirection == 1);
}

RaceSettings *cFrontendDatabase::GetQuickRaceSettings(GRace::Type type) {
    if (static_cast<int>(type) > 10) {
        return &TheQuickRaceSettings[RaceMode];
    }
    return &TheQuickRaceSettings[type];
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
    DriveWithAnalog = savedDriveWithAnalog;
    Config = savedConfig;
    Rumble = savedRumble;
}

POVTypes GetPOVTypeFromPlayerCamera(ePlayerSettingsCameras cam);
bool IsPlayerCameraSelectable(POVTypes pov);

void PlayerSettings::ScrollDriveCam(int dir) {
    int cam = CurCam;
    if (dir == 1) {
        do {
            cam++;
            if (cam > 6) {
                cam = 0;
            }
        } while (!IsPlayerCameraSelectable(
            GetPOVTypeFromPlayerCamera(static_cast< ePlayerSettingsCameras >(cam))));
        CurCam = static_cast< ePlayerSettingsCameras >(cam);
    } else if (dir == -1) {
        do {
            cam--;
            if (cam < 0) {
                cam = 6;
            }
        } while (!IsPlayerCameraSelectable(
            GetPOVTypeFromPlayerCamera(static_cast< ePlayerSettingsCameras >(cam))));
        CurCam = static_cast< ePlayerSettingsCameras >(cam);
    } else {
        CurCam = static_cast< ePlayerSettingsCameras >(cam);
    }
}

extern unsigned int HudConfigs[];
extern unsigned int DriveConfigs[];

unsigned int PlayerSettings::GetControllerAttribs(eControllerAttribs type, bool wheel_connected) const {
    int analog = DriveWithAnalog != 0;
    int config = Config;
    if (wheel_connected) {
        config = 0;
        analog = 1;
    }
    if (type != CA_HUD) {
        if (type == CA_DRIVING) {
            return DriveConfigs[analog + config * 2];
        }
        return 0;
    }
    return HudConfigs[analog + config * 2];
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

bool AudioSettings::operator==(const AudioSettings& rhs) const {
    if (MasterVol != rhs.MasterVol) return false;
    if (SpeechVol != rhs.SpeechVol) return false;
    if (FEMusicVol != rhs.FEMusicVol) return false;
    if (IGMusicVol != rhs.IGMusicVol) return false;
    if (SoundEffectsVol != rhs.SoundEffectsVol) return false;
    if (EngineVol != rhs.EngineVol) return false;
    if (CarVol != rhs.CarVol) return false;
    if (AmbientVol != rhs.AmbientVol) return false;
    if (SpeedVol != rhs.SpeedVol) return false;
    if (InteractiveMusicMode != rhs.InteractiveMusicMode) return false;
    if (EATraxMode != rhs.EATraxMode) return false;
    if (PlayState != rhs.PlayState) return false;
    return AudioMode == rhs.AudioMode;
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

void cFrontendDatabase::SetMilestoneDescriptionString(char *outputStr, int milestoneType, float currVal, float goalVal, bool showCurrVal) const {
    if (showCurrVal && currVal > goalVal) {
        currVal = goalVal;
    }
    switch (milestoneType) {
    case 0x33fa23a: {
        char currValTimeToPrint[16];
        char goalValTimeToPrint[16];
        Timer currValTimer(currVal);
        currValTimer.PrintToString(currValTimeToPrint, 4);
        Timer goalValTimer(goalVal);
        goalValTimer.PrintToString(goalValTimeToPrint, 4);
        if (showCurrVal) {
            bSPrintf(outputStr, "%s/%s", currValTimeToPrint, goalValTimeToPrint);
        } else {
            bSPrintf(outputStr, "%s", goalValTimeToPrint);
        }
        break;
    }
    case 0x5392e4fd: {
        float printTime = currVal;
        char currValTimeToPrint[16];
        char goalValTimeToPrint[16];
        if (currVal == 0.0f) {
            IPlayer *player = IPlayer::First(PLAYER_LOCAL);
            if (player) {
                ISimable *simable = player->GetSimable();
                if (simable) {
                    IVehicle *vehicle;
                    if (simable->QueryInterface(&vehicle)) {
                        IVehicleAI *vehicleai = vehicle->GetAIVehiclePtr();
                        if (vehicleai) {
                            IPursuit *ipursuit = vehicleai->GetPursuit();
                            if (ipursuit) {
                                float pursuitElapsedTime = ipursuit->GetPursuitDuration();
                                float timeRemaining = goalVal - pursuitElapsedTime;
                                printTime = UMath::Max(timeRemaining, 0.0f);
                            }
                        }
                    }
                }
            }
        }
        Timer currValTimer(printTime);
        currValTimer.PrintToString(currValTimeToPrint, 4);
        Timer goalValTimer(goalVal);
        goalValTimer.PrintToString(goalValTimeToPrint, 4);
        if (showCurrVal) {
            bSPrintf(outputStr, "%s/%s", currValTimeToPrint, goalValTimeToPrint);
        } else {
            bSPrintf(outputStr, "%s", goalValTimeToPrint);
        }
        break;
    }
    default:
        if (showCurrVal) {
            bSPrintf(outputStr, "%$0.0f/%$0.0f", currVal, goalVal);
        } else {
            bSPrintf(outputStr, "%$0.0f", goalVal);
        }
        break;
    }
}

bool cFrontendDatabase::IsMilestoneTimeFormat(int typeKey) const {
    if (typeKey == 0x33fa23a || typeKey == 0x5392e4fd) {
        return true;
    }
    return false;
}

GameCompletionStats::GameCompletionStats()
    : m_nOverall(0) //
    , m_nCareer(0) //
    , m_nRapSheetRankings(0) //
    , m_nChallenge(0) //
    , m_nTotalChallengeRaces(0) //
    , m_nCompletedChallengeRaces(0)
{
}

GameCompletionStats cFrontendDatabase::GetGameCompletionStats() {
    GameCompletionStats stats;
    float nTotalCareerRaces = 0.0f;
    float nCompletedCareerRaces = 0.0f;
    float nTotalMilestones = static_cast<float>(GManager::Get().GetNumMilestones());
    float nMilestonesAwarded = 0.0f;
    float nTotalRapSheetRankings = 140.0f;
    float nRapSheetRankings = 0.0f;

    for (unsigned int i = 1; i < 17; i++) {
        GRaceBin *pBin = GRaceDatabase::Get().GetBinNumber(i);
        unsigned int nBossRaces = pBin->GetBossRaceCount();
        for (unsigned int j = 0; j < nBossRaces; j++) {
            if (GRaceDatabase::Get().IsCareerRaceComplete(pBin->GetBossRaceHash(j))) {
                nCompletedCareerRaces += 1.0f;
            }
        }
        unsigned int nWorldRaces = pBin->GetWorldRaceCount();
        for (unsigned int j = 0; j < nWorldRaces; j++) {
            if (GRaceDatabase::Get().IsCareerRaceComplete(pBin->GetWorldRaceHash(j))) {
                nCompletedCareerRaces += 1.0f;
            }
        }
        nTotalCareerRaces += static_cast<float>(nBossRaces + nWorldRaces);
    }

    for (unsigned int i = 0; i < GRaceDatabase::Get().GetRaceCount(); i++) {
        GRaceParameters *pParams = GRaceDatabase::Get().GetRaceParameters(i);
        if (bStrCmp(pParams->GetEventID(), GRaceDatabase::Get().GetBurgerKingRace()) != 0) {
            if (GetIsCollectorsEdition() || !pParams->GetIsCollectorsEditionRace()) {
                if (pParams->GetIsChallengeSeriesRace()) {
                    stats.m_nTotalChallengeRaces++;
                    if (GRaceDatabase::Get().IsQuickRaceComplete(pParams->GetEventHash())) {
                        stats.m_nCompletedChallengeRaces++;
                    }
                }
            }
        }
    }

    for (unsigned int i = 0; static_cast<float>(i) < nTotalMilestones; i++) {
        if (GManager::Get().GetMilestone(i)->GetIsAwarded()) {
            nMilestonesAwarded += 1.0f;
        }
    }

    for (unsigned int i = 0; i < 10; i++) {
        int rankMovement = FEDatabase->GetMultiplayerProfile(0)->GetHighScores()->CalcPursuitRank(
            static_cast<ePursuitDetailTypes>(i), true);
        if (15 - rankMovement >= 0) {
            nRapSheetRankings += static_cast<float>(15 - rankMovement);
        }
    }

    stats.m_nRapSheetRankings = static_cast<unsigned char>(
        nRapSheetRankings / nTotalRapSheetRankings * 100.0f);
    stats.m_nCareer = static_cast<unsigned char>(
        (nCompletedCareerRaces + nMilestonesAwarded) / (nTotalCareerRaces + nTotalMilestones) * 100.0f);
    stats.m_nChallenge = static_cast<unsigned char>(
        static_cast<float>(stats.m_nCompletedChallengeRaces) /
        static_cast<float>(stats.m_nTotalChallengeRaces) * 100.0f);
    stats.m_nOverall = static_cast<unsigned char>(
        static_cast<float>(static_cast<int>(stats.m_nCareer)) * 0.7f +
        static_cast<float>(static_cast<int>(stats.m_nChallenge)) * 0.2f +
        static_cast<float>(static_cast<int>(stats.m_nRapSheetRankings)) * 0.1f);

    return stats;
}

void cFrontendDatabase::NotifyExitRaceToFrontend(eExitRacePlaces from_where) {
    {
        int is_split = IsSplitScreenMode();
    }
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
        m_pDBBackup = static_cast<char *>(bMalloc(GetUserProfileSaveSize(false), nullptr, 0, 0x40));
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

extern unsigned int FEngHashString(const char *, unsigned char);
extern bool DoesStringExist(unsigned int);
extern bool gVerboseTesterOutput;
extern void bToUpper(char *);

void CareerSettings::Default() {
    CurrentCash = 0;
    CurrentBin = 0x10;
    CurrentCar = 0;
    SpecialFlags = 0;
    AdaptiveDifficulty = 0;
    for (int i = 0; i < 150; i++) {
        SMSMessages[i].SetHandle(static_cast<unsigned char>(i));
        if (!DoesStringExist(FEngHashString("", SMSMessages[i].GetHandle()))) {
            SMSMessages[i].SetHandle(0xFF);
        }
        SMSMessages[i].ClearFlags();
    }
    SMSSortOrder = 0;
}

extern bool SkipDDayRaces;
extern bool SkipCareerIntro;

void CareerSettings::StartNewCareer(bool bEnterGameplay) {
    Default();
    CurrentCar = FEDatabase->GetDefaultCar();
    GenerateCaseFileName();
    SpecialFlags |= 1;

    if (SkipCareerIntro && SkipDDayRaces) {
        CurrentBin = 0xF;
        GRaceDatabase::Get().SimulateDDayComplete();
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
        FECarRecord *rec = stable->CreateNewCareerCar(0x2CF385B2);
        CurrentCar = rec->Handle;
        rec = stable->CreateNewCareerCar(0x03A94520);
        CurrentCar = rec->Handle;
    }

    TryAwardDemoMarker();

    if (!bEnterGameplay) {
        return;
    }

    FEDatabase->ResetGameMode();
    FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER);

    if (SkipDDayRaces) {
        FEManager::Get()->SetGarageType(GARAGETYPE_CAREER_SAFEHOUSE);
        FEDatabase->ClearGameMode(eFE_GAME_MODE_CAREER_MANAGER);
        if (SkipCareerIntro) {
            CurrentBin = 0xF;
        }
    } else {
        unsigned int hash = FEHashUpper("M3GTRCAREERSTART");
        FEDatabase->GetCareerSettings()->SetCurrentCar(hash);
        FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->SetSelectedCar(hash, 0);
        gMemcardSetup.mPreviousCommand = 0;
        gMemcardSetup.mPreviousPrompt = 0;
        gMemcardSetup.mOp = 0;
        gMemcardSetup.mMemScreen = nullptr;
        gMemcardSetup.mToScreen = nullptr;
        gMemcardSetup.mFromScreen = nullptr;
        gMemcardSetup.mTermFunc = nullptr;
        gMemcardSetup.mTermFuncParam = nullptr;
        gMemcardSetup.mLastMessage = 0;
        gMemcardSetup.mSuccessMsg = 0;
        gMemcardSetup.mFailedMsg = 0;
        gMemcardSetup.mInBootFlow = false;
        const char *firstDDayRace = GRaceDatabase::Get().GetDDayStartRace();
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromName(firstDDayRace);
        GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(parms);
        GRaceDatabase::Get().SetStartupRace(race, kRaceContext_Career);
        GRaceDatabase::Get().FreeCustomRace(race);
        RaceStarter::StartCareerFreeRoam();
    }
}

void CareerSettings::ResumeCareer() {
    bool bDDayCompleted = false;
    if (SkipDDayRaces ||
        GRaceDatabase::Get().CheckRaceScoreFlags(
            GRaceDatabase::Get()
                .GetRaceFromHash(Attrib::StringHash32("16.2.1"))
                ->GetEventHash(),
            GRaceDatabase::kCompleted_ContextCareer)) {
        bDDayCompleted = true;
    }

    bool bTutorialCompleted = false;
    if (!(SpecialFlags & 0x4000)) {
        if (GRaceDatabase::Get().CheckRaceScoreFlags(
                GRaceDatabase::Get()
                    .GetRaceFromHash(Attrib::StringHash32("1.2.3"))
                    ->GetEventHash(),
                GRaceDatabase::kCompleted_ContextCareer)) {
            bTutorialCompleted = true;
        }
    }

    if (CurrentBin == 0x10) {
        if (!bDDayCompleted) {
            unsigned int carHash = FEHashUpper("M3GTRCAREERSTART");
            FEDatabase->GetCareerSettings()->CurrentCar = carHash;
            FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->SelectedCar[0] = carHash;
            const char *nextRace = GRaceDatabase::Get().GetNextDDayRace();
            GRaceParameters *parms =
                GRaceDatabase::Get().GetRaceFromHash(Attrib::StringHash32(nextRace));
            GRaceCustom *custom = GRaceDatabase::Get().AllocCustomRace(parms);
            GRaceDatabase::Get().SetStartupRace(custom, kRaceContext_Career);
            GRaceDatabase::Get().FreeCustomRace(custom);
            if (bStrCmp(nextRace, "16.1.0") != 0) {
                MemoryCard::s_pThis->m_bCancelNextAutoSave = true;
            }
        }
        RaceStarter::StartCareerFreeRoam();
    } else if (bTutorialCompleted) {
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromHash(Attrib::StringHash32("1.8.1"));
        GRaceCustom *custom = GRaceDatabase::Get().AllocCustomRace(parms);
        GRaceDatabase::Get().SetStartupRace(custom, kRaceContext_Career);
        GRaceDatabase::Get().FreeCustomRace(custom);
        RaceStarter::StartCareerFreeRoam();
        MemoryCard::s_pThis->m_bCancelNextAutoSave = true;
    } else {
        FEManager::Get()->SetGarageType(static_cast< eGarageType >(2));
        FEDatabase->ClearGameMode(eFE_GAME_MODE_CAREER_MANAGER);
    }
    FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER);
}

void CareerSettings::GenerateCaseFileName() {
    const int SCOTTS_RAND_CASE_FILE_NUMBER_RANGE = 0x19B3;
    const int SCOTTS_RAND_CASE_FILE_NUMBER_START = 0x42D;
    unsigned int num = bRandom(SCOTTS_RAND_CASE_FILE_NUMBER_RANGE) + SCOTTS_RAND_CASE_FILE_NUMBER_START;
    const char *profile_name = FEDatabase->GetUserProfile(0)->GetProfileName();
    bSNPrintf(CaseFileName, 13, "%d%s", num, profile_name);
    bToUpper(CaseFileName);
}

extern int FEngSNPrintf(char *, int, const char *, ...);
extern unsigned long FEHashUpper(const char *);
extern void FixDot(char *str, int len);

cFrontendDatabase::cFrontendDatabase()
    : iDefaultStableHash(0) //
    , m_pCarStableBackup(nullptr) //
    , m_pDBBackup(nullptr) //
    , SplitScreenCustomization(nullptr)
{
    for (int i = 0; i < 2; i++) {
        CurrentUserProfiles[i] = nullptr;
    }
    CurrentUserProfiles[0] = new UserProfile();
}

unsigned int CalcLanguageHash(const char *prefix, GRaceParameters *pRaceParams) {
    char buffer[64];
    FEngSNPrintf(buffer, 0x40, "%s%s", prefix, pRaceParams->GetEventID());
    FixDot(buffer, 0x40);
    return FEHashUpper(buffer);
}

void InitFrontendDatabase() {
    unsigned int alloc_params = GetVirtualMemoryAllocParams();
    FEDatabase = new(alloc_params) cFrontendDatabase();
    FEDatabase->Default();
}

bool cFrontendDatabase::IsFinalEpicChase() {
    if (!GRaceStatus::Exists()) {
        return false;
    }
    if (!GRaceStatus::Get().GetRaceParameters()) {
        return false;
    }
    unsigned int event_hash = GRaceStatus::Get().GetRaceParameters()->GetEventHash();
    unsigned int final_hash = Attrib::StringHash32("1.8.1");
    return event_hash == final_hash;
}

unsigned int cFrontendDatabase::GetRaceNameHash(GRace::Type type) {
    switch (type) {
    case GRace::kRaceType_P2P:
        return 0xb94fd70e;
    case GRace::kRaceType_Circuit:
        return 0x034fa2c1;
    case GRace::kRaceType_Drag:
        return 0x6f547e4c;
    case GRace::kRaceType_Knockout:
        return 0x4930f5fc;
    case GRace::kRaceType_Tollbooth:
        return 0xa15e4505;
    case GRace::kRaceType_SpeedTrap:
        return 0xee1edc76;
    case GRace::kRaceType_Challenge:
        return 0x213cc8d1;
    default:
        return 0x7818f85e;
    }
}

unsigned int cFrontendDatabase::GetRaceIconHash(GRace::Type type) {
    switch (type) {
    case GRace::kRaceType_P2P:
        return 0x2521e5eb;
    case GRace::kRaceType_Circuit:
        return 0xe9638d3e;
    case GRace::kRaceType_Drag:
        return 0xaaab31e9;
    case GRace::kRaceType_Knockout:
        return 0x3a015595;
    case GRace::kRaceType_Tollbooth:
        return 0x1a091045;
    case GRace::kRaceType_SpeedTrap:
    case GRace::kRaceType_JumpToSpeedTrap:
        return 0x66c9a7b6;
    case GRace::kRaceType_JumpToMilestone:
        return 0x1a091045;
    default:
        break;
    }
    return 0;
}

unsigned int cFrontendDatabase::GetSafehouseIconHash(const char *name) {
    unsigned int result = 0;
    if (bStrICmp(name, "carlot") == 0) {
        result = 0x4eaee18b;
    } else if (bStrICmp(name, "safehouse") == 0) {
        result = 0x0ed39f69;
    } else if (bStrICmp(name, "customshop") == 0) {
        result = 0x0cf07089;
    }
    return result;
}

void cFrontendDatabase::GetRandomRaceOptions(RaceSettings *race, GRace::Type type) {
    race->CatchUp = true;
    race->CopDensity = static_cast< uint8 >(bRandom(4));
    race->AISkill = 1;
    race->NumOpponents = static_cast< uint8 >(bRandom(3) + 1);
    if (type == GRace::kRaceType_Circuit) {
        race->NumLaps = static_cast< uint8 >(bRandom(5) + 1);
    } else if (type == GRace::kRaceType_Knockout) {
        race->NumLaps = static_cast< uint8 >(bRandom(3) + 1);
    } else {
        race->NumLaps = 1;
    }
    race->TrafficDensity = static_cast< uint8 >(bRandom(4));
    race->TrackDirection = static_cast< uint8 >(bRandom(1));
}

void cFrontendDatabase::BuildCurrentRideForPlayer(int player, RideInfo *ride) {
    FEPlayerCarDB *stable;
    if (static_cast< unsigned int >(player) < 2) {
        stable = &GetUserProfile(player)->PlayersCarStable;
    } else {
        stable = nullptr;
    }
    unsigned int car;
    unsigned int mode = FEGameMode;
    if ((mode & 4) != 0 || (mode & 0x40) != 0 || (mode & 8) != 0) {
        RaceSettings *settings = GetQuickRaceSettings(GRace::kRaceType_NumTypes);
        car = settings->SelectedCar[player];
    } else {
        car = GetUserProfile(0)->GetCareer()->GetCurrentCar();
    }
    stable->BuildRideForPlayer(car, player, ride);
}

void cFrontendDatabase::Default() {
    bProfileLoaded = false;
    bIsOptionsDirty = false;
    bAutoSaveOverwriteConfirmed = false;
    iNumPlayers = 1;
    bComingFromBoot = true;
    GetUserProfile(0)->Default(0, true);
    FEGameMode = 0;
    iCurPauseSubOptionType = 0;
    iCurPauseOptionType = 0;
    if (SkipFE && SkipFESplitScreen) {
        FEGameMode = 4;
        iNumPlayers = 2;
    }
    PlayerJoyports[0] = 0;
    PlayerJoyports[1] = -1;
    RaceMode = static_cast< GRace::Type >(1);
    unsigned int default_car = GetDefaultCar();
    DefaultRaceSettings();
    GetUserProfile(0)->GetCareer()->SetCurrentCar(default_car);
    if (!iDefaultStableHash) {
        FEPlayerCarDB *stable = &GetUserProfile(0)->PlayersCarStable;
        int buf_size = stable->GetSaveBufferSize();
        char *buf = static_cast< char * >(bMalloc(buf_size, 0x40));
        int save_size = stable->GetSaveBufferSize();
        stable->SaveToBuffer(buf, save_size);
        int crc_size = stable->GetSaveBufferSize();
        iDefaultStableHash = bCalculateCrc32(buf, crc_size, 0xFFFFFFFF);
        bFree(buf);
    }
}

void cFrontendDatabase::NotifyDeleteCar(unsigned int handle) {
    unsigned int default_car = GetDefaultCar();
    for (unsigned int i = 0; i < 11; i++) {
        RaceSettings &settings = TheQuickRaceSettings[i];
        if (settings.SelectedCar[0] == handle) {
            settings.SelectedCar[0] = default_car;
        }
        if (settings.SelectedCar[1] == handle) {
            settings.SelectedCar[0] = default_car;
        }
    }
}

unsigned int cFrontendDatabase::GetMilestoneIconHash(unsigned int type, bool isMilestone) {
    unsigned int hash = 0;
    switch (type) {
    case 0x850A64BC:
        hash = 0x88E8DE9E;
        break;
    case 0x3FD1884D:
    case 0x4FC942CA:
        hash = 0x0FE608E6;
        break;
    case 0xFD989A3A:
        hash = 0x87807869;
        break;
    case 0x7457EED4:
    case 0x23B1BF0E:
    case 0x15E88693:
    case 0x20F1AEF3:
    case 0x411B084E:
    case 0x2CB7CAF4:
    case 0x755F7845:
    case 0x8ED622AD:
    case 0xC8993341:
        return 0;
    case 0x1334DAE6:
    case 0x1BF724E1:
    case 0x254230F5:
    case 0x4D9777F1:
    case 0x9201E1F4:
    case 0x9F8E56CE:
    case 0xABDF316E:
    case 0xCA9AFDF0:
    case 0xE9A4423C:
        return 0;
    case 0x5392E4FD:
        hash = 0x831B7EBE;
        break;
    case 0x033FA23A:
        hash = 0x8C76CD0F;
        if (isMilestone) {
            hash = 0x950FCEBC;
        }
        break;
    case 0xEB45F99D:
        hash = 0xC43959D2;
        break;
    case 0x9E3EBB78:
        hash = 0x3FFE9EC9;
        break;
    case 0xCDF36FC2:
        hash = 0xE621B2EF;
        break;
    case 0xA61CAC24:
        hash = 0x6784A80E;
        break;
    case 0x2377E50D:
        hash = 0xB4E6456B;
        break;
    }
    return hash;
}

unsigned int cFrontendDatabase::GetDefaultCar() {
    Attrib::Gen::frontend TheFrontend(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), 0xeec2271a), 0, nullptr);
    Attrib::RefSpec refSpec;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    refSpec = TheFrontend.default_car();
    Attrib::Gen::pvehicle vehicle(refSpec, 0, nullptr);
    unsigned int default_car = 0;
    unsigned int key = vehicle.GetCollection();
    for (int i = 0; i <= 199; i++) {
        FECarRecord *car = stable->GetCarByIndex(i);
        if (car->IsValid() && car->VehicleKey == key) {
            default_car = car->Handle;
            break;
        }
    }
    return default_car;
}

cFrontendDatabase *FEDatabase;
