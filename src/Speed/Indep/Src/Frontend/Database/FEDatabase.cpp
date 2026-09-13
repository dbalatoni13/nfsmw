#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSoundEnums.hpp"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GMilestone.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audiosystem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/music.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Misc/MD5.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"

extern int g_MaxSongs;

extern Attrib::Key HudConfigs[5][2];
extern Attrib::Key DriveConfigs[5][2];

cFrontendDatabase *FEDatabase;

// UNSOLVED
char *SaveSomeData(void *save_to, void *save_from, int bytes, void *maxptr) {
    if (reinterpret_cast<uint32>(save_to) + bytes <= reinterpret_cast<uint32>(maxptr)) {
        bMemCpy(save_to, save_from, bytes);
        save_to = static_cast<char *>(save_to) + bytes;
    }
    return static_cast<char *>(save_to);
}
// UNSOLVED
char *LoadSomeData(void *load_to, void *load_from, int bytes, void *maxptr) {
    if (reinterpret_cast<uint32>(load_from) + bytes <= reinterpret_cast<uint32>(maxptr)) {
        bMemCpy(load_to, load_from, bytes);
    }
    return static_cast<char *>(load_from) + bytes;
}

FEKeyboardSettings::FEKeyboardSettings()
    : AcceptCallbackHash(0xAE83B9DB), DeclineCallbackHash(0x6A97B51F), MaxTextLength(64), DefaultTextHash(0), Mode(0) {
    Buffer[0] = 0;
}

void PlayerSettings::Default() {
    GaugesOn = true;
    PositionOn = true;
    LapInfoOn = true;
    ScoreOn = true;
    LeaderboardOn = true;
    TransmissionPromptOn = true;
    Rumble = true;
    DriveWithAnalog = true;
    Config = CC_CONFIG_1;
    CurCam = PSC_DEFAULT;
    SplitTimeType = 0;
    Transmission = 0;
    Handling = 1;
}

bool PlayerSettings::operator==(const PlayerSettings &settings) const {
    return bMemCmp(this, &settings, sizeof(PlayerSettings)) == 0;
}

void PlayerSettings::DefaultFromOptionsScreen() {
    bool curDriveWithAnalog = DriveWithAnalog;
    eControllerConfig curConfig = Config;
    bool curRumble = Rumble;
    Default();
    DriveWithAnalog = curDriveWithAnalog;
    Config = curConfig;
    Rumble = curRumble;
}

Attrib::Key PlayerSettings::GetControllerAttribs(eControllerAttribs type, bool wheel_connected) const {
    int analog = DriveWithAnalog ? 1 : 0;
    int config = Config;
    if (wheel_connected) {
        config = 0;
        analog = 1;
    }

    switch (type) {
        case CA_DRIVING:
            return DriveConfigs[config][analog];
        case CA_HUD:
            return HudConfigs[config][analog];
        default:
            return 0;
    }
}

void PlayerSettings::ScrollDriveCam(int dir) {
    int cur_cam = CurCam;
    if (dir == 1) {
        do {
            cur_cam++;
            if (cur_cam > 6) {
                cur_cam = 0;
            }
        } while (!IsPlayerCameraSelectable(GetPOVTypeFromPlayerCamera(static_cast<ePlayerSettingsCameras>(cur_cam))));
        CurCam = static_cast<ePlayerSettingsCameras>(cur_cam);
    } else if (dir == -1) {
        do {
            cur_cam--;
            if (cur_cam < 0) {
                cur_cam = 6;
            }
        } while (!IsPlayerCameraSelectable(GetPOVTypeFromPlayerCamera(static_cast<ePlayerSettingsCameras>(cur_cam))));
        CurCam = static_cast<ePlayerSettingsCameras>(cur_cam);
    } else {
        CurCam = static_cast<ePlayerSettingsCameras>(cur_cam);
    }
}

void GameplaySettings::Default() {
    AutoSaveOn = true;
    RearviewOn = true;
    Damage = true;
    RacingMiniMapMode = 1;
    ExploringMiniMapMode = 0;
    MapItems = static_cast<unsigned int>(-1);
    LastMapZoom = 1;
    LastPursuitMapZoom = 2;
    LastMapView = 0;
    JumpCam = true;
    HighlightCam = 127.5f;
    if (GetCurrentLanguage()) {
        SpeedoUnits = 1;
    } else {
        SpeedoUnits = 0;
    }
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

bool GameplaySettings::operator==(const GameplaySettings &settings) const {
    return bMemCmp(this, &settings, sizeof(GameplaySettings)) == 0;
}

void VideoSettings::Default() {
    FEScale = 1.0f;
    ScreenOffsetX = 0.0f;
    ScreenOffsetY = 0.0f;
    WideScreen = false;
}

bool VideoSettings::operator==(const VideoSettings &settings) const {
    return bMemCmp(this, &settings, sizeof(VideoSettings)) == 0;
}

void AudioSettings::Default() {
    MasterVol = 1.0f;
    SpeechVol = 1.0f;
    FEMusicVol = 0.8f;
    IGMusicVol = 0.8f;
    SoundEffectsVol = 1.0f;
    EngineVol = 1.0f;
    CarVol = 1.0f;
    AmbientVol = 1.0f;
    SpeedVol = 1.0f;
    AudioMode = 2;
#ifndef EA_BUILD_A124
    AudioMode = g_pEAXSound->GetDefaultPlatformAudioMode();
#endif
    InteractiveMusicMode = 1;
    EATraxMode = 1;
    PlayState = 0;
}

bool AudioSettings::operator==(const AudioSettings &settings) const {
    if (MasterVol != settings.MasterVol)
        return false;
    if (SpeechVol != settings.SpeechVol)
        return false;
    if (FEMusicVol != settings.FEMusicVol)
        return false;
    if (IGMusicVol != settings.IGMusicVol)
        return false;
    if (SoundEffectsVol != settings.SoundEffectsVol)
        return false;
    if (EngineVol != settings.EngineVol)
        return false;
    if (CarVol != settings.CarVol)
        return false;
    if (AmbientVol != settings.AmbientVol)
        return false;
    if (SpeedVol != settings.SpeedVol)
        return false;
    if (InteractiveMusicMode != settings.InteractiveMusicMode)
        return false;
    if (EATraxMode != settings.EATraxMode)
        return false;
    if (PlayState != settings.PlayState)
        return false;
    if (AudioMode != settings.AudioMode && AudioMode != 0 && AudioMode != 1) {
        return false;
    }
    return true;
}

void OptionsSettings::Default() {
    CurrentCategory = OC_AUDIO;
    TheVideoSettings.Default();
    TheAudioSettings.Default();
    TheGameplaySettings.Default();
    ThePlayerSettings[0].Default();
    ThePlayerSettings[1].Default();
}

void CareerSettings::Default() {
    {
        extern int foo; // Unknown extern
    }
    CurrentBin = 0x10;
    CurrentCar = 0;
    SpecialFlags = 0;
    AdaptiveDifficulty = 0;
    CurrentCash = 0;
    for (int i = 0; i < 150; i++) {
        SMSMessages[i].SetHandle(i);
        if (!DoesStringExist(SMSMessages[i].GetMsgHash())) {
            SMSMessages[i].SetHandle(0xFF);
        }
        SMSMessages[i].ClearFlags();
    }
    SMSSortOrder = 0;
}

SMSMessage *CareerSettings::GetSMSMessage(uint32 index) {
    if (index >= 150) {
        return nullptr;
    }
    return &SMSMessages[index];
}

uint16 CareerSettings::GetSMSSortOrder() {
    SMSSortOrder = SMSSortOrder + 1;
    return SMSSortOrder;
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

void CareerSettings::SpendCash(int amount) {
    if (amount > CurrentCash) {
        CurrentCash = 0;
        return;
    }
    CurrentCash = CurrentCash - amount;
}

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
#ifndef EA_BUILD_A124
    TryAwardDemoMarker();
#endif

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
        int hash = FEHashUpper("M3GTRCAREERSTART");
        FEDatabase->GetCareerSettings()->SetCurrentCar(hash);
        FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->SetSelectedCar(hash, 0);
        gMemcardSetup.Clear();
        const char *firstDDayRace = GRaceDatabase::Get().GetDDayStartRace();
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromName(firstDDayRace);
        GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(parms);
        GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_Career);
        GRaceDatabase::Get().FreeCustomRace(race);
        RaceStarter::StartCareerFreeRoam();
    }
}
#ifndef EA_BUILD_A124
void CareerSettings::TryAwardDemoMarker() {
    if (!HasBeenAwardedDemoMarker() && gEasterEggs.IsEasterEggUnlocked(EASTER_EGG_DEMO_CHEAT)) {
        TheFEMarkerManager.AddMarkerToInventory(FEMarkerManager::MARKER_ENGINE, 0);
        SetAwardedDemoMarker();
    }
}
#endif

void CareerSettings::ResumeCareer() {
    const char *lastDDayRace = GRaceDatabase::Get().GetDDayEndRace();
    bool dday_flow_completed = false;
    if (SkipDDayRaces || GRaceDatabase::Get().IsCareerRaceComplete(GRaceDatabase::Get().GetRaceFromName(lastDDayRace)->GetEventHash())) {
        dday_flow_completed = true;
    }

    const char *lastBossRace = GRaceDatabase::Get().GetFinalBossRace();
    bool finalBossRaceCompleted = false;
    if (!this->HasBeatenCareer()) {
        if (GRaceDatabase::Get().IsCareerRaceComplete(GRaceDatabase::Get().GetRaceFromName(lastBossRace)->GetEventHash())) {
            finalBossRaceCompleted = true;
        }
    }

    if (CurrentBin == 0x10) {
        if (!dday_flow_completed) {
            int hash = FEHashUpper("M3GTRCAREERSTART");
            FEDatabase->GetCareerSettings()->SetCurrentCar(hash);
            FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->SetSelectedCar(hash, 0);
            const char *nextDDayRace = GRaceDatabase::Get().GetNextDDayRace();
            GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromName(nextDDayRace);
            GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(parms);
            GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_Career);
            GRaceDatabase::Get().FreeCustomRace(race);
            if (bStrCmp(nextDDayRace, GRaceDatabase::Get().GetDDayStartRace()) != 0) {
                MemoryCard::GetInstance()->CancelNextAutoSave();
            }
        }
        RaceStarter::StartCareerFreeRoam();
    } else if (finalBossRaceCompleted) {
        const char *finalEpicChaseRace = GRaceDatabase::Get().GetFinalEpicChaseRace();
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromName(finalEpicChaseRace);
        GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(parms);
        GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_Career);
        GRaceDatabase::Get().FreeCustomRace(race);
        RaceStarter::StartCareerFreeRoam();
        MemoryCard::GetInstance()->CancelNextAutoSave();
    } else {
        FEManager::Get()->SetGarageType(GARAGETYPE_CAREER_SAFEHOUSE);
        FEDatabase->ClearGameMode(eFE_GAME_MODE_CAREER_MANAGER);
    }
    FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER);
}

void CareerSettings::AwardOneTimeCashBonus(bool bGiveCashBonus) {
    SpecialFlags = SpecialFlags | CS_ONE_TIME_CASH_BONUS;
    if (!bGiveCashBonus) {
        return;
    }
    CurrentCash = CurrentCash + 10000;
}

#ifndef EA_BUILD_A124
void CareerSettings::SetPlayerHasBeatenTheGame() {
    SetHasBeatenCareer();
}
#endif

void CareerSettings::GenerateCaseFileName() {
    const int SCOTTS_RAND_CASE_FILE_NUMBER_RANGE = 0x19B3;
    const int SCOTTS_RAND_CASE_FILE_NUMBER_START = 0x42D;
    unsigned int num = bRandom(SCOTTS_RAND_CASE_FILE_NUMBER_RANGE) + SCOTTS_RAND_CASE_FILE_NUMBER_START;
    const char *profile_name = FEDatabase->GetUserProfile(0)->GetProfileName();
    bSNPrintf(CaseFileName, 13, "MW-%d-%.4sXXX", num, profile_name);
    bToUpper(CaseFileName);
}

char *CareerSettings::SaveToBuffer(void *buffer, void *maxbuf) {
    char *buf = SaveGameplayData(buffer, maxbuf);
    buf = SaveSomeData(buf, &CurrentCar, sizeof(CurrentCar), maxbuf);
    buf = SaveSomeData(buf, &CurrentBin, sizeof(CurrentBin), maxbuf);
    buf = SaveSomeData(buf, &CurrentCash, sizeof(CurrentCash), maxbuf);
    buf = SaveSomeData(buf, &AdaptiveDifficulty, sizeof(AdaptiveDifficulty), maxbuf);
    buf = SaveSomeData(buf, &SpecialFlags, sizeof(SpecialFlags), maxbuf);
    buf = SaveSomeData(buf, SMSMessages, sizeof(SMSMessages), maxbuf);
    buf = SaveSomeData(buf, &SMSSortOrder, sizeof(SMSSortOrder), maxbuf);
    buf = SaveSomeData(buf, CaseFileName, sizeof(CaseFileName), maxbuf);
    buf = SaveRaceData(buf, maxbuf);
    buf = SaveUnlockData(buf, maxbuf);
    buf = TheFEMarkerManager.SaveToBuffer(buf);
    return buf;
}

char *CareerSettings::LoadFromBuffer(void *buffer, void *maxbuf) {
    char *buf = LoadGameplayData(buffer, maxbuf);
    buf = LoadSomeData(&CurrentCar, buf, sizeof(CurrentCar), maxbuf);
    buf = LoadSomeData(&CurrentBin, buf, sizeof(CurrentBin), maxbuf);
    buf = LoadSomeData(&CurrentCash, buf, sizeof(CurrentCash), maxbuf);
    buf = LoadSomeData(&AdaptiveDifficulty, buf, sizeof(AdaptiveDifficulty), maxbuf);
    buf = LoadSomeData(&SpecialFlags, buf, sizeof(SpecialFlags), maxbuf);
    buf = LoadSomeData(SMSMessages, buf, sizeof(SMSMessages), maxbuf);
    buf = LoadSomeData(&SMSSortOrder, buf, sizeof(SMSSortOrder), maxbuf);
    buf = LoadSomeData(CaseFileName, buf, sizeof(CaseFileName), maxbuf);
    buf = LoadRaceData(buf, maxbuf);
    buf = LoadUnlockData(buf, maxbuf);
    buf = TheFEMarkerManager.LoadFromBuffer(buf);
    return buf;
}

int32 CareerSettings::GetSaveBufferSize(bool bExcludeGameplay) {
    // TODO: find how values are calculated
    int size = TheFEMarkerManager.GetSaveBufferSize() + 0x441;
    if (!bExcludeGameplay) {
        size += 0x52C4;
    }
    return size;
}

char *CareerSettings::SaveRaceData(void *save_to, void *maxptr) {
    if (!GRaceDatabase::Exists()) {
        return static_cast<char *>(save_to);
    }
    char *buf = static_cast<char *>(save_to);
    unsigned int nEntries = GRaceDatabase::Get().GetScoreInfoCount();
    nEntries = bMin(300, nEntries);
    buf = SaveSomeData(buf, &nEntries, sizeof(nEntries), maxptr);
    GRaceSaveInfo *current = GRaceDatabase::Get().GetScoreInfo();
    for (unsigned int index = 0; index < nEntries; index++) {
        if (gVerboseTesterOutput && current->mRaceHash != 0 && (current->mFlags & 2)) {
            GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromHash(current->mRaceHash);
        }
        buf = SaveSomeData(buf, current, sizeof(GRaceSaveInfo), maxptr);
        current++;
    }

    return static_cast<char *>(save_to) + 0x12C4;
}

char *CareerSettings::SaveUnlockData(void *save_to, void *maxptr) {
    char *buf = static_cast<char *>(save_to);
    for (unsigned int i = 0; i < 0x39; i++) {
        buf = SaveSomeData(buf, &TheUnlockData[i], 8, maxptr);
    }
    return buf;
}

char *CareerSettings::SaveGameplayData(void *save_to, void *maxptr) {
    if (GManager::Exists()) {
        GManager::Get().SaveGameplayData(reinterpret_cast<uint8 *>(save_to), 0x4000);
    } else {
        bMemSet(save_to, 0, 0x4000);
    }
    return reinterpret_cast<char *>(save_to) + 0x4000;
}

char *CareerSettings::LoadRaceData(void *load_from_here, void *maxptr) {
    if (!GRaceDatabase::Exists()) {
        return static_cast<char *>(load_from_here);
    };
    char *buf = static_cast<char *>(load_from_here);
    unsigned int nEntries = 0;
    buf = LoadSomeData(&nEntries, buf, sizeof(nEntries), maxptr);
    nEntries = bMin(300, nEntries);
    GRaceSaveInfo saveInfoEntries[300];
    GRaceSaveInfo *current = saveInfoEntries;
    for (unsigned int index = 0; index < nEntries; index++) {
        buf = LoadSomeData(current, buf, sizeof(GRaceSaveInfo), maxptr);
        if (gVerboseTesterOutput && current->mRaceHash != 0 && (current->mFlags & 2)) {
            GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromHash(current->mRaceHash);
        }
        current++;
    }
    GRaceDatabase::Get().LoadBestScores(saveInfoEntries, nEntries);

    return static_cast<char *>(load_from_here) + 0x12C4;
}

char *CareerSettings::LoadUnlockData(void *load_from_here, void *maxptr) {
    char *buf = static_cast<char *>(load_from_here);
    for (unsigned int i = 0; i < 0x39; i++) {
        buf = LoadSomeData(&TheUnlockData[i], buf, sizeof(UnlockDatum), maxptr);
    }
    return buf;
}

char *CareerSettings::LoadGameplayData(void *load_from_here, void *maxptr) {
    if (GManager::Exists()) {
        GManager::Get().LoadGameplayData(reinterpret_cast<uint8 *>(load_from_here), 0x4000);
    }
    return reinterpret_cast<char *>(load_from_here) + 0x4000;
}

UserProfile::UserProfile() : TheOptionsSettings(), TheCareerSettings(), HighScores() {}

UserProfile::~UserProfile() {}

void UserProfile::SetProfileName(const char *pName, bool isP1) {
    bool bSetIt = false;
    if ((pName != nullptr) && bStrLen(pName) > 0) {
        bSetIt = true;
    }
    bMemSet(m_aProfileName, 0, sizeof(m_aProfileName));
    if (bSetIt) {
        bStrNCpy(m_aProfileName, pName, sizeof(m_aProfileName));
        m_bNamed = true;
    } else {
        char sztemp[32];
        if (isP1 == true) {
            GetLocalizedString(sztemp, sizeof(sztemp), 0x7b070984);
        } else {
            GetLocalizedString(sztemp, sizeof(sztemp), 0x7b070985);
        }
        bStrNCpy(m_aProfileName, sztemp, sizeof(m_aProfileName));
        m_bNamed = false;
    }
}

const char *UserProfile::GetProfileName() {}

bool UserProfile::IsProfileNamed() {
    return m_bNamed;
}

void UserProfile::Default(int player_number, bool commit_default) {
    if (!player_number) {
        SetProfileName(nullptr, true);
    } else {
        SetProfileName(nullptr, false);
    }

    PlayersCarStable.Default();

    if (!player_number) {
        static bool song_info_loaded = false;
        TheOptionsSettings.Default();
        TheCareerSettings.Default();
        HighScores.Default();
        CareerModeHasBeenCompletedAtLeastOnce = false;

        if (!song_info_loaded) {
            song_info_loaded = true;

            Attrib::Gen::audiosystem *playlist_atrs = new Attrib::Gen::audiosystem(0x7E4B0ED2, 0, nullptr);
            if (playlist_atrs->IsValid()) {
                Attrib::Gen::audiosystem licensed_music(playlist_atrs->LicensedMusic(), 0, nullptr);
                g_MaxSongs = licensed_music.Num_PFMapping();

                for (int i = 0; i < static_cast<int>(Songs.size()); i++) {
                    delete Songs[i];
                }
                Songs.clear();

                if (static_cast<int>(Songs.capacity()) < g_MaxSongs) {
                    Songs.reserve(g_MaxSongs);
                }

                for (int i = 0; i < g_MaxSongs; i++) {
                    Sound::stSongInfo *newsong = new ("Sound::stSongInfo", 0) Sound::stSongInfo;
                    Attrib::Gen::music currsong(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);

                    currsong.ChangeWithDefault(licensed_music.PFMapping(i));

                    const char *tmpSongName = currsong.SongName().GetString();
                    newsong->SongName = const_cast<char *>(currsong.SongName().GetString());
                    newsong->Album = const_cast<char *>(currsong.Album().GetString());
                    newsong->Artist = const_cast<char *>(currsong.Artist().GetString());
                    newsong->DefPlay = const_cast<char *>(currsong.DefPlay().GetString());
                    newsong->PathEvent = currsong.PathEvent();
                    Songs.push_back(newsong);
                }
            }
        }

        for (int i = 0; i < g_MaxSongs; i++) {
            Playlist[i].SongIndex = i;

            Sound::stSongInfo *song = Songs[i];
            eSongPlayability epbf;
            if (song == nullptr) {
                epbf = ePLAY_OFF;
            } else if (bStrCmp(song->DefPlay, "FE") == 0) {
                epbf = ePLAY_MENU;
            } else if (bStrCmp(song->DefPlay, "IG") == 0) {
                epbf = ePLAY_RACE;
            } else if (bStrCmp(song->DefPlay, "AL") == 0) {
                epbf = ePLAY_ALL;
            } else {
                epbf = ePLAY_OFF;
            }

            Playlist[i].PlayabilityField = epbf;
        }

        InitializeEATrax(true);
    }

    PlayersCarStable.AwardBonusCars();
#ifndef EA_BUILD_A124
    TheCareerSettings.TryAwardDemoMarker();
#endif
}

void UserProfile::CommitHighScoresPauseQuit() {
    HighScores.CommitHighScoresPauseQuit();
}

void UserProfile::CommitPursuitInfo(IPursuit *iPursuit, uint32 car_FEKey, uint32 bounty, unsigned int num_infractions) {
    HighScores.CommitPursuitInfo(iPursuit, car_FEKey, bounty, num_infractions);
}

void UserProfile::IncInfration(GInfractionManager::InfractionType infrat, unsigned int car) {}
void UserProfile::CommitServeInfractions(unsigned int car) {}

void UserProfile::WriteProfileHash(void *bufferToHash, void *bufferToWrite, int bytes, void *maxptr) {
    MD5 md5;
    md5.Update(bufferToHash, bytes);
    md5.GetRaw();
    SaveSomeData(bufferToWrite, md5.GetRaw(), 0x10, maxptr);
}

bool UserProfile::VerifyProfileHash(void *bufferToHash, void *bufferHash, int bytes) {
    MD5 md5;
    md5.Update(bufferToHash, bytes);
    md5.GetRaw();
    return bMemCmp(md5.GetRaw(), bufferHash, 0x10) == 0;
}

void UserProfile::SaveToBuffer(void *buffer, int size) {
    char aVersion[16];
    char *buf = static_cast<char *>(buffer);
    char *maxbuf = buf + size;

    bMemSet(buf, 0, size);
    bMemSet(aVersion, 0, sizeof(aVersion));

    buf = SaveSomeData(buf, aVersion, sizeof(aVersion), maxbuf);
    buf = TheCareerSettings.SaveToBuffer(buf, maxbuf);
    buf = SaveSomeData(buf, &FEDatabase->iDefaultStableHash, sizeof(FEDatabase->iDefaultStableHash), maxbuf);
    buf = SaveSomeData(buf, m_aProfileName, sizeof(m_aProfileName), maxbuf);
    buf = SaveSomeData(buf, Playlist, sizeof(Playlist), maxbuf);
    buf = SaveSomeData(buf, &TheOptionsSettings, sizeof(TheOptionsSettings), maxbuf);
    buf = PlayersCarStable.SaveToBuffer(buf, PlayersCarStable.GetSaveBufferSize());
    buf = SaveSomeData(buf, &CareerModeHasBeenCompletedAtLeastOnce, sizeof(CareerModeHasBeenCompletedAtLeastOnce), maxbuf);
    buf = SaveSomeData(buf, &HighScores, sizeof(HighScores), maxbuf);

    for (int i = 0; i < 11; i++) {
        uint32 h = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(i))->GetSelectedCar(0);
        buf = SaveSomeData(buf, &h, sizeof(h), maxbuf);
    }
    WriteProfileHash(static_cast<char *>(buffer) + 0x10, buf, size - 0x20, maxbuf);
}

bool UserProfile::LoadFromBuffer(void *buffer, int size, bool commit_changes, int player_id) {
    char aVersion[16];
    char *buf = static_cast<char *>(buffer);
    char *maxbuf = buf + size;

    buf = LoadSomeData(aVersion, buf, sizeof(aVersion), maxbuf);
    if (!player_id) {
        buf = TheCareerSettings.LoadFromBuffer(buf, maxbuf);
#ifndef EA_BUILD_A124
        TheCareerSettings.TryAwardDemoMarker();
#endif
    } else {
        buf = buf + TheCareerSettings.GetSaveBufferSize(false);
    }
    uint32 version;
    buf = LoadSomeData(&version, buf, sizeof(version), maxbuf);
    if (version != FEDatabase->iDefaultStableHash) {
        return false;
    }
    buf = LoadSomeData(m_aProfileName, buf, sizeof(m_aProfileName), maxbuf);
    if (!player_id) {
        buf = LoadSomeData(Playlist, buf, sizeof(Playlist), maxbuf);
    } else {
        buf = buf + 0xF0;
    }
    buf = LoadSomeData(&TheOptionsSettings, buf, sizeof(TheOptionsSettings), maxbuf);
    buf = PlayersCarStable.LoadFromBuffer(buf, PlayersCarStable.GetSaveBufferSize());
    PlayersCarStable.AwardBonusCars();
    buf = LoadSomeData(&CareerModeHasBeenCompletedAtLeastOnce, buf, sizeof(CareerModeHasBeenCompletedAtLeastOnce), maxbuf);
    buf = LoadSomeData(&HighScores, buf, sizeof(HighScores), maxbuf);
    for (int i = 0; i < 11; i++) {
        uint32 h;
        buf = LoadSomeData(&h, buf, sizeof(h), maxbuf);
        FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(i))->SetSelectedCar(h, player_id);
    }
    if (!VerifyProfileHash(static_cast<char *>(buffer) + 0x10, buf, size - 0x20)) {
        return false;
    }
    m_bNamed = true;
    return true;
}

int32 UserProfile::GetSaveBufferSize(bool bExcludeGameplay) {
    int size = TheCareerSettings.GetSaveBufferSize(bExcludeGameplay) + 0x1e4;
    return size + PlayersCarStable.GetSaveBufferSize() + 0xc18;
}

cFrontendDatabase::cFrontendDatabase()
    : iDefaultStableHash(0), m_pCarStableBackup(nullptr), m_pDBBackup(nullptr), SplitScreenCustomization(nullptr), FinishedRaceStats() {
    for (int i = 0; i < 2; i++) {
        CurrentUserProfiles[i] = nullptr;
    }
    CurrentUserProfiles[0] = new ("Player 1 UserProfile", 0) UserProfile();
}

// UNSOLVED (regswap)
void cFrontendDatabase::Default() {
    int track_number;
    bProfileLoaded = false;
    bIsOptionsDirty = false;
#ifndef EA_BUILD_A124
    bAutoSaveOverwriteConfirmed = false;
#endif
    iNumPlayers = 1;
    bComingFromBoot = true;
    CurrentUserProfiles[0]->Default(0, true);
    iCurPauseSubOptionType = 0;
    iCurPauseOptionType = 0;
    ResetGameMode();
    if (SkipFE && SkipFESplitScreen) {
        SetGameMode(eFE_GAME_MODE_QUICK_RACE);
        iNumPlayers = 2;
    }
    PlayerJoyports[0] = 0;
    PlayerJoyports[1] = -1;
    RaceMode = GRace::kRaceType_Circuit;
    uint32 default_car = GetDefaultCar();
    DefaultRaceSettings();
    GetCareerSettings()->SetCurrentCar(default_car);
    if (!iDefaultStableHash) {
        FEPlayerCarDB *pCarStable = GetPlayerCarStable(0);
        char *buffer = static_cast<char *>(bMalloc(pCarStable->GetSaveBufferSize(), "Memcard Car Stable Hashing", 0, BMEMORY_ALLOCATE_FROM_TOP));
        pCarStable->SaveToBuffer(buffer, pCarStable->GetSaveBufferSize());
        iDefaultStableHash = bCalculateCrc32(buffer, pCarStable->GetSaveBufferSize(), 0xFFFFFFFF);
        bFree(buffer);
    }
}

void cFrontendDatabase::DefaultProfile() {
    CurrentUserProfiles[0]->Default(0, true);
#ifndef EA_BUILD_A124
    bAutoSaveOverwriteConfirmed = false;
#endif
    DefaultRaceSettings();
    uint32 default_car = GetDefaultCar();
    GetCareerSettings()->SetCurrentCar(default_car);
    bIsOptionsDirty = false;
    GetPlayerCarStable(0)->Default();
    MemoryCard::GetInstance()->SetCardRemovedWithAutoSaveEnabled(false);
    DefaultUnlockData();
    TheFEMarkerManager.Default();
    if (GRaceDatabase::Exists()) {
        GRaceDatabase::Get().ClearRaceScores();
    }
    if (GManager::Exists()) {
        GManager::Get().ResetAllGameplayData();
    }
}

void cFrontendDatabase::DefaultRaceSettings() {
    uint32 default_car = GetDefaultCar();
    for (uint32 i = 0; i < 11; i++) {
        RaceSettings &settings = TheQuickRaceSettings[i];
        settings.Default();
        settings.SelectedCar[0] = default_car;
        settings.SelectedCar[1] = default_car;
    }
    TheQuickRaceSettings[0].NumLaps = 1;
    TheQuickRaceSettings[2].NumLaps = 1;
    TheQuickRaceSettings[5].NumLaps = 1;
    TheQuickRaceSettings[4].NumOpponents = 0;
    TheQuickRaceSettings[3].NumLaps = TheQuickRaceSettings[3].NumOpponents;
    TheQuickRaceSettings[4].NumLaps = 1;
}

void cFrontendDatabase::NotifyDeleteCar(uint32 handle) {
    uint32 default_car = GetDefaultCar();
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

void cFrontendDatabase::SetPlayersJoystickPort(int player, int8 joy_port) {
    if (joy_port == -1 && PlayerJoyports[player] != -1) {
        cFEngJoyInput::Get()->SetRequiredJoy(static_cast<JoystickPort>(PlayerJoyports[player]), false);
    }
    PlayerJoyports[player] = joy_port;
}

uint32 cFrontendDatabase::GetDefaultCar() {
    Attrib::Gen::frontend TheFrontend(0xeec2271a, 0, nullptr);
    Attrib::RefSpec refSpec;
    uint32 default_car = 0;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);

    if (TheFrontend.default_car(refSpec)) {
        Attrib::Gen::pvehicle vehicle(refSpec, 0, nullptr);
        Attrib::Key key = vehicle.GetCollection();
        FECarRecord *car;
        for (int i = 0; i < 200; i++) {
            car = stable->GetCarByIndex(i);
            if (car->IsValid() && car->VehicleKey == key) {
                default_car = car->Handle;
                break;
            }
        }
    }
    return default_car;
}

void cFrontendDatabase::CreateMultiplayerProfile(int player) {
    if (CurrentUserProfiles[player] == nullptr) {
        CurrentUserProfiles[player] = new ("FEDatabase UserProfile", 0) UserProfile;
        CurrentUserProfiles[player]->Default(player, true);
    }
}

void cFrontendDatabase::DeleteMultiplayerProfile(int player) {
    if (player == 1 && (CurrentUserProfiles[player] != nullptr)) {
        uint32 player_car = GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(1);
        FEPlayerCarDB *stable = GetPlayerCarStable(1);
        FECarRecord *record = stable->GetCarRecordByHandle(player_car);
        FECustomizationRecord *customization = stable->GetCustomizationRecordByHandle(record->Customization);
        bStrCpy(SplitScreenCarType, record->GetDebugName());
        if (customization != nullptr) {
            SplitScreenCustomization =
                static_cast<FECustomizationRecord *>(bMalloc(sizeof(FECustomizationRecord), "Splitscreen FECustomizationRecord", 0, 0x47));
            bMemCpy(SplitScreenCustomization, customization, sizeof(FECustomizationRecord));
        } else {
            SplitScreenCustomization = nullptr;
        }
        if (CurrentUserProfiles[player] != nullptr) {
            delete CurrentUserProfiles[player];
        }
        CurrentUserProfiles[player] = nullptr;
    }
}
void cFrontendDatabase::AllocBackupDB(bool bForce) {
    if ((m_pDBBackup == nullptr) && bForce) {
        m_pDBBackup = static_cast<char *>(bMalloc(GetUserProfileSaveSize(false), "BackupDB Buffer", 0, BMEMORY_ALLOCATE_FROM_TOP));
        SaveUserProfileToBuffer(m_pDBBackup, GetUserProfileSaveSize(false));
    }
}

void cFrontendDatabase::DeallocBackupDB() {
    if (m_pDBBackup != nullptr) {
        bFree(m_pDBBackup);
        m_pDBBackup = nullptr;
    }
}

void cFrontendDatabase::RestoreFromBackupDB() {
    if (m_pDBBackup != nullptr) {
        LoadUserProfileFromBuffer(m_pDBBackup, GetUserProfileSaveSize(false), 0);
        DeallocBackupDB();
    }
}
void cFrontendDatabase::BackupCarStable() {
    if (m_pCarStableBackup == nullptr) {
        m_pCarStableBackup = static_cast<char *>(bMalloc(GetPlayerCarStable(0)->GetSaveBufferSize(), "CarStable backup buffer", 0, 0));
        bMemCpy(m_pCarStableBackup, GetPlayerCarStable(0), GetPlayerCarStable(0)->GetSaveBufferSize());
    }
}

bool cFrontendDatabase::IsCarStableDirty() {
    if (m_pCarStableBackup == nullptr) {
        return false;
    }
    bool bDirty = bMemCmp(m_pCarStableBackup, GetPlayerCarStable(0), GetPlayerCarStable(0)->GetSaveBufferSize()) != 0;
    bFree(m_pCarStableBackup);
    m_pCarStableBackup = nullptr;
    return bDirty;
}

void cFrontendDatabase::RefreshCurrentRide() {
    RideInfo ride;
    FEPlayerCarDB *stable = GetPlayerCarStable(0);
    if (IsCareerMode() || IsSafehouseMode() || IsCareerManagerMode()) {
        BuildCurrentRideForPlayer(0, &ride);
    } else {
        uint32 handle = GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(0);
        stable->BuildRideForPlayer(handle, 0, &ride);
    }
    CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_CATCHALL, eCARVIEWER_PLAYER1_CAR);
}

RaceSettings *cFrontendDatabase::GetQuickRaceSettings(GRace::Type type) {
    if (static_cast<int>(type) > 10) {
        return &TheQuickRaceSettings[RaceMode];
    }
    return &TheQuickRaceSettings[type];
}

bool cFrontendDatabase::IsFinalEpicChase() {
    if (GRaceStatus::Exists()) {
        GRaceParameters *race = GRaceStatus::Get().GetRaceParameters();

        return race != nullptr && race->GetEventHash() == Attrib::StringHash32(GRaceDatabase::Get().GetFinalEpicChaseRace());
    }
    return false;
}

void cFrontendDatabase::GetRandomRaceOptions(RaceSettings *race, GRace::Type type) {
    race->CatchUp = true;
    race->CopDensity = bRandom(4);
    race->AISkill = 1;
    race->NumOpponents = bRandom(3) + 1;

    switch (type) {
        case GRace::kRaceType_Knockout:
            race->NumLaps = race->NumOpponents;
            break;
        case GRace::kRaceType_Circuit:
            race->NumLaps = bRandom(5) + 1;
            break;
        default:
            race->NumLaps = 1;
    }

    race->TrafficDensity = bRandom(4);
    race->TrackDirection = bRandom(1);
}

void cFrontendDatabase::FillCustomRace(GRaceCustom *parms, RaceSettings *race) {
    if (race == nullptr) {
        return;
    }
    if (parms == nullptr) {
        return;
    }
    parms->SetCatchUp(race->CatchUp);
    parms->SetCopsEnabled(race->CopsOn);
#ifdef EA_BUILD_A124
    parms->SetCopDensity(static_cast<GRace::CopDensity>(race->CopDensity));
#else
    if (race->CopsOn) {
        parms->SetHeatLevel(race->CopDensity);
    }
#endif
    parms->SetDifficulty(static_cast<GRace::Difficulty>(race->AISkill));
    parms->SetNumLaps(race->NumLaps);
    parms->SetNumOpponents(race->NumOpponents);
    switch (race->TrafficDensity) {
        case 0:
        default:
            parms->SetTrafficDensity(0);
            break;
        case 1:
            parms->SetTrafficDensity(10);
            break;
        case 2:
            parms->SetTrafficDensity(30);
            break;
        case 3:
            parms->SetTrafficDensity(50);
            break;
    }
    parms->SetReversed(race->TrackDirection == 1);
}

void cFrontendDatabase::BuildCurrentRideForPlayer(int player, RideInfo *ride) {
    FEPlayerCarDB *stable = GetPlayerCarStable(player);
    uint32 current_car;
    if (FEDatabase->IsQuickRaceMode() || FEDatabase->IsLANMode() || FEDatabase->IsOnlineMode()) {
        current_car = GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(player);
    } else {
        current_car = FEDatabase->GetCareerSettings()->GetCurrentCar();
    }
    stable->BuildRideForPlayer(current_car, player, ride);
}

void cFrontendDatabase::NotifyExitRaceToFrontend(eExitRacePlaces from_where) {
    PostRaceOptionChosen = POST_RACE_OPT_QUIT;
    if (from_where == EXIT_RACE_FROM_PAUSE) {
        int is_split = static_cast<int>(IsSplitScreenMode());
        CurrentUserProfiles[0]->CommitHighScoresPauseQuit();
    }
}

int32 cFrontendDatabase::GetUserProfileSaveSize(bool bExcludeGameplay) {
    return CurrentUserProfiles[0]->GetSaveBufferSize(bExcludeGameplay);
}

void cFrontendDatabase::SaveUserProfileToBuffer(void *buffer, int32 bufsize) {
    CurrentUserProfiles[0]->SaveToBuffer(buffer, bufsize);
}

bool cFrontendDatabase::LoadUserProfileFromBuffer(void *buffer, int bufsize, int player) {
    if (player == 0) {
        return CurrentUserProfiles[0]->LoadFromBuffer(buffer, bufsize, true, 0);
    } else {
        bool res = CurrentUserProfiles[player]->LoadFromBuffer(buffer, bufsize, false, player);
        bMemCpy(&CurrentUserProfiles[0]->GetOptions()->ThePlayerSettings[1], &CurrentUserProfiles[1]->GetOptions()->ThePlayerSettings[0],
                sizeof(PlayerSettings));
        return res;
    }
}

uint32 cFrontendDatabase::GetChallengeHeaderHash(uint32 hal_id) {
    return FEngHashString("CHALLENGE_SERIES_DESC_%02d_HEADER", hal_id);
}

uint32 cFrontendDatabase::GetChallengeDescHash(uint32 hal_id) {
    return FEngHashString("CHALLENGE_SERIES_DESC_%02d", hal_id);
}

uint32 cFrontendDatabase::GetBountyIconHash(uint32 hal_id) {
    if (hal_id < 5) {
        return 0x8A21B882;
    }
    if (hal_id < 9) {
        return 0x895EC0AE;
    }
    return 0x9129E7FB;
}

uint32 cFrontendDatabase::GetBountyHeaderHash(uint32 hal_id) {
    return FEngHashString("BLACKLIST_BOUNTY_SPAWN_POINT_%02d", hal_id);
}

uint32 cFrontendDatabase::GetBountyDescHash(uint32 hal_id) {
    return FEngHashString("BLACKLIST_BOUNTY_SPAWN_POINT_%02d_DESC", hal_id);
}

uint32 cFrontendDatabase::GetMilestoneHeaderHash(uint32 hal_id) {
    return FEngHashString("BLACKLIST_PURSUIT_MILESTONES_%02d_SHORT", hal_id);
}

uint32 cFrontendDatabase::GetMilestoneDescHash(uint32 hal_id) {
    return FEngHashString("BLACKLIST_PURSUIT_MILESTONES_%02d", hal_id);
}

// UNSOLVED
uint32 cFrontendDatabase::GetMilestoneIconHash(uint32 type, bool isMilestone) {
    uint32 hash = 0;
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
            if (isMilestone) {
                hash = 0x950FCEBC;
            } else {
                hash = 0x8C76CD0F;
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
        default:
            hash = 0;
    }
    return hash;
}

void cFrontendDatabase::SetMilestoneDescriptionString(char *const outputStr, const int milestoneType, float currVal, const float goalVal,
                                                      const bool showCurrVal) const {
    if (showCurrVal && currVal > goalVal) {
        currVal = goalVal;
    }
    switch (milestoneType) {
        case 0x33fa23a: {
            char currValTimeToPrint[16];
            Timer currValTimer(currVal);
            currValTimer.PrintToString(currValTimeToPrint, 4);
            char goalValTimeToPrint[16];
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
            if (currVal == 0.0f) {
                IPlayer *player = IPlayer::First(PLAYER_LOCAL);
                ISimable *simable;
                if (player != nullptr) {
                    simable = player->GetSimable();
                    if (simable != nullptr) {
                        IVehicle *vehicle;
                        if (simable->QueryInterface(&vehicle)) {
                            IVehicleAI *vehicleai = vehicle->GetAIVehiclePtr();
                            if (vehicleai != nullptr) {
                                IPursuit *ipursuit = vehicleai->GetPursuit();
                                if (ipursuit != nullptr) {
                                    float pursuitElapsedTime = ipursuit->GetPursuitDuration();
                                    float timeRemaining = UMath::Max(0.0f, goalVal - pursuitElapsedTime);
                                    printTime = timeRemaining;
                                }
                            }
                        }
                    }
                }
            }
            char currValTimeToPrint[16];
            Timer currValTimer(printTime);
            currValTimer.PrintToString(currValTimeToPrint, 4);
            char goalValTimeToPrint[16];
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

bool cFrontendDatabase::IsMilestoneTimeFormat(const int milestoneType) const {
    if (milestoneType == 0x33fa23a || milestoneType == 0x5392e4fd) {
        return true;
    }
    return false;
}

uint32 cFrontendDatabase::GetRaceNameHash(GRace::Type raceType) {
    uint32 hash = 0;
    switch (raceType) {
        case GRace::kRaceType_P2P:
            hash = 0xb94fd70e;
            break;
        case GRace::kRaceType_Circuit:
            hash = 0x034fa2c1;
            break;
        case GRace::kRaceType_Drag:
            hash = 0x6f547e4c;
            break;
        case GRace::kRaceType_Knockout:
            hash = 0x4930f5fc;
            break;
        case GRace::kRaceType_Tollbooth:
            hash = 0xa15e4505;
            break;
        case GRace::kRaceType_SpeedTrap:
            hash = 0xee1edc76;
            break;
        case GRace::kRaceType_Challenge:
            hash = 0x213cc8d1;
            break;
        default:
            hash = 0x7818f85e;
            break;
    }
    return hash;
}

uint32 cFrontendDatabase::GetRaceIconHash(GRace::Type raceType) {
    uint32 hash = 0;
    switch (raceType) {
        case GRace::kRaceType_P2P:
            hash = 0x2521e5eb;
            break;
        case GRace::kRaceType_Circuit:
            hash = 0xe9638d3e;
            break;
        case GRace::kRaceType_Drag:
            hash = 0xaaab31e9;
            break;
        case GRace::kRaceType_Knockout:
            hash = 0x3a015595;
            break;
        case GRace::kRaceType_Tollbooth:
            hash = 0x1a091045;
            break;
        case GRace::kRaceType_SpeedTrap:
        case GRace::kRaceType_JumpToSpeedTrap:
            hash = 0x66c9a7b6;
            break;
        case GRace::kRaceType_JumpToMilestone:
            hash = 0x1a091045;
            break;
    }
    return hash;
}

uint32 cFrontendDatabase::GetSafehouseIconHash(const char *safehouseType) {
    uint32 hash = 0;
    if (bStrICmp(safehouseType, "carlot") == 0) {
        hash = 0x4eaee18b;
    } else if (bStrICmp(safehouseType, "safehouse") == 0) {
        hash = 0x0ed39f69;
    } else if (bStrICmp(safehouseType, "customshop") == 0) {
        hash = 0x0cf07089;
    }
    return hash;
}

GameCompletionStats::GameCompletionStats()
    : m_nOverall(0), m_nCareer(0), m_nRapSheetRankings(0), m_nChallenge(0), m_nTotalChallengeRaces(0), m_nCompletedChallengeRaces(0) {}

GameCompletionStats cFrontendDatabase::GetGameCompletionStats() {
    GameCompletionStats stats;
    float nTotalCareerRaces = 0.0f;
    float nCompletedCareerRaces = 0.0f;
    float nTotalMilestones = static_cast<float>(GManager::Get().GetNumMilestones());
    float nMilestonesAwarded = 0.0f;
    float nTotalRapSheetRankings = 140.0f;
    float nRapSheetRankings = 0.0f;
    GRaceBin *pBin;
    unsigned int nBossRaces;
    unsigned int nWorldRaces;
    GRaceParameters *pParams;
    int rankMovement;

    for (unsigned int i = 1; i < 17; i++) {
        pBin = GRaceDatabase::Get().GetBinNumber(i);
        nBossRaces = pBin->GetBossRaceCount();
        for (unsigned int j = 0; j < nBossRaces; j++) {
            if (GRaceDatabase::Get().IsCareerRaceComplete(pBin->GetBossRaceHash(j))) {
                nCompletedCareerRaces += 1.0f;
            }
        }
        nWorldRaces = pBin->GetWorldRaceCount();
        for (unsigned int j = 0; j < nWorldRaces; j++) {
            if (GRaceDatabase::Get().IsCareerRaceComplete(pBin->GetWorldRaceHash(j))) {
                nCompletedCareerRaces += 1.0f;
            }
        }
        nTotalCareerRaces += static_cast<float>(nBossRaces + nWorldRaces);
    }

    for (unsigned int i = 0; i < GRaceDatabase::Get().GetRaceCount(); i++) {
        pParams = GRaceDatabase::Get().GetRaceParameters(i);
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

    for (unsigned int i = 0; i < PD_NUM_PD_TYPES; i++) {
        rankMovement = 15 - FEDatabase->GetMultiplayerProfile(0)->GetHighScores()->CalcPursuitRank(static_cast<ePursuitDetailTypes>(i), true);
        if (rankMovement >= 0) {
            nRapSheetRankings += static_cast<float>(rankMovement);
        }
    }

    stats.m_nRapSheetRankings = static_cast<unsigned char>(nRapSheetRankings / nTotalRapSheetRankings * 100.0f);
    stats.m_nCareer = static_cast<unsigned char>((nCompletedCareerRaces + nMilestonesAwarded) / (nTotalCareerRaces + nTotalMilestones) * 100.0f);
    stats.m_nChallenge =
        static_cast<unsigned char>(static_cast<float>(stats.m_nCompletedChallengeRaces) / static_cast<float>(stats.m_nTotalChallengeRaces) * 100.0f);
    stats.m_nOverall = static_cast<unsigned char>(static_cast<float>(static_cast<int>(stats.m_nCareer)) * 0.7f +
                                                  static_cast<float>(static_cast<int>(stats.m_nChallenge)) * 0.2f +
                                                  static_cast<float>(static_cast<int>(stats.m_nRapSheetRankings)) * 0.1f);

    return stats;
}

void InitFrontendDatabase() {
    FEDatabase = new ("cFrontendDatabase") cFrontendDatabase();
    FEDatabase->Default();
}

int GetMikeMannBuild() {
    return MikeMannBuild;
}

bool GetIsCollectorsEdition() {
    return IsCollectorsEdition;
}
